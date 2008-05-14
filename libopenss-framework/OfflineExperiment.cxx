////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration of the OfflineExperiment class.
 *
 */

#include "EntrySpy.hxx"
#include "ToolAPI.hxx"
#include "AddressSpace.hxx"
#include "AddressRange.hxx"
#include "DataQueues.hxx"
#include "OfflineExperiment.hxx"
#include "SymbolTable.hxx"
#include "Instrumentor.hxx"
#include "Blob.hxx"
#include "BFDSymbols.hxx"
#include "ThreadName.hxx"

#include <iostream>
#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "OpenSS_DataHeader.h"
#include "offline/offline.h"

#include "OpenSSPath.cxx"

using namespace OpenSpeedShop::Framework;

typedef std::map<AddressRange,
        std::pair<SymbolTable, std::set<LinkedObject> > >
        SymbolTableMap;

#ifndef NDEBUG
/** Flag indicating if debuging for MPI jobs is enabled. */
bool OfflineExperiment::is_debug_offlinesymbols_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS") != NULL);

/** Flag indicating if debuging for offline experiments is enabled. */
bool OfflineExperiment::is_debug_offline_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE") != NULL);
#endif

int
OfflineExperiment::getRawDataFiles (std::string dir)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cerr << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }


    struct stat file_info;
    stat(dir.c_str(), &file_info);
    bool is_dir = S_ISDIR(file_info.st_mode);

    if (is_dir) {
	while ((dirp = readdir(dp)) != NULL) {
// DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
	        std::cerr << "OfflineExperiment::getRawDataFiles testing "
		<< dirp->d_name << std::endl;
	    }
#endif
	    struct stat file_info1;
	    std::string fname(dir+"/"+dirp->d_name);
	    stat(fname.c_str(), &file_info1);
	    bool is_subdir = S_ISDIR(file_info1.st_mode);
	    bool is_regfile = S_ISREG(file_info1.st_mode);

	    if (is_subdir) {
		std::string subdir(dirp->d_name);
		if (subdir.find("..") == string::npos &&
		    subdir.find(".") == string::npos ) {

		    rawdirs.push_back(dir+"/"+subdir);
		}
	    }
	    else {
	    }
	}

	// no subdirs.  must be a single raw data dir.
	if (rawdirs.size() == 0 ) {
	    rawdirs.push_back(dir);
	}
    }


    DIR *dpsub;
    struct dirent *dirpsub;

    for (unsigned int i = 0;i < rawdirs.size();i++) {

	if((dpsub  = opendir(rawdirs[i].c_str())) == NULL) {
            std::cerr << "Error(" << errno << ") opening "
		<< rawdirs[i] << std::endl;
            return errno;
	}


	while ((dirpsub = readdir(dpsub)) != NULL) {

	    std::string rawfile(dirpsub->d_name);
	    stat(rawfile.c_str(), &file_info);
	    bool is_dir = S_ISDIR(file_info.st_mode);

	    if (rawfile.find(".openss-data") != string::npos ||
		rawfile.find(".openss-info") != string::npos ||
		rawfile.find(".openss-dsos") != string::npos ) {

		rawfiles.push_back(std::string(rawfile));

	    }
	}
	if (rawfiles.size() > 0 ) {
		rawdatadir = rawdirs[i];
	        convertToOpenSSDB();
	        createOfflineSymbolTable();
	}
	rawfiles.clear();
	closedir(dpsub);
    }

    return 0;
}

int OfflineExperiment::convertToOpenSSDB()
{
    std::string rawname;

    // process offline info blobs first.
    for (unsigned int i = 0;i < rawfiles.size();++i) {
	bool_t found_infofile = false;
	if (rawfiles[i].find(".openss-info") != string::npos) {
	    std::cerr << "processing " << rawfiles[i] << std::endl;
	    rawname = rawdatadir + "/" + rawfiles[i];
	    found_infofile = true;
	} 

        if (found_infofile) {
            bool_t rval = process_expinfo(rawname);
            if (!rval) {
    	        std::cerr << "Could not process experiment info for: "
		    << rawname << std::endl;
            }

            // add this pid and host to database.
            theExperiment->updateThreads(expPid,expPosixTid,expHost);
        } else {
	}
    }

    // Set LD_LIBRARY_PATH and plugin path so we can find the
    // collector implementation.
    std::string dummy;
    SetOpenssLibPath(dummy);

    CollectorGroup cgrp = theExperiment->getCollectors();
    std::string collector_name;
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        Metadata m = c.getMetadata();
        collector_name = m.getUniqueId();
    }

    if (expCollector != collector_name) {
        theExperiment->createCollector(expCollector);
    }

    // Process data first so we can find the unique pc values
    // found in this experiments performance date.  These pc values
    // will be used to restrict the symbol lookup to just those
    // linked objects, addressspaces, files, functions, statements
    // where performance data was collected.

    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_datafile = false;
	if (rawfiles[i].find(".openss-data") != string::npos) {
	    std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawdatadir + "/" + rawfiles[i];
	    found_datafile = true;
	} 

        if (found_datafile) {
            bool_t rval = process_data(rawname);
            if (!rval) {
	        std::cerr << "Could not process experiment info for: "
		        << rawname << std::endl;
            }
        }
    }
 
    // Process the list of dsos and address ranges for this experiment.
    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_dsofile = false;
	if (rawfiles[i].find(".openss-dsos") != string::npos) {
	    std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawdatadir + "/" + rawfiles[i];
	    found_dsofile = true;
	} 

        if (found_dsofile) {
            bool_t rval = process_objects(rawname);
            if (!rval) {
	        std::cerr << "Could not process experiment info for: "
	    	    << rawname << std::endl;
            }
        }
    }

    return(0);
}


/*
 * currently experiment info blob is first blob in file followed
 * by the either a string of data blobs or dso blobs and data blobs
 * the are intermixed.
 * Each blob will have a header and the actual blob data.
 * For an experiment info blob, this the usual header and an
 * expinfo blob as described by the runtime's offline.x file.
 */

bool
OfflineExperiment::process_expinfo(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";
    long where = 0;

    FILE *f = fopen(rawfilename.c_str(),"r");

    xdrstdio_create(&xdrs, f, XDR_DECODE);

    unsigned int blobsize;
    if (!xdr_u_int(&xdrs, &blobsize)) {
	std::cerr << "process_expinfo fails to find blobsize" << std::endl;
    }

    /* info blob */
    openss_expinfo info;
    memset(&info, 0, sizeof(info));

    OpenSS_DataHeader infoheader;
    memset(&infoheader, 0, sizeof(infoheader));

    bool_t infoheadercall = xdr_OpenSS_DataHeader(&xdrs, &infoheader);
    if(!infoheadercall) {
	std::cerr << "Could Not find a valid data header in raw file "
		<< rawfilename << std::endl;
	fclose(f);
	return false;
    }

    bool_t infocall = xdr_openss_expinfo(&xdrs, &info);

    if(infocall) {
	std::string command = "empty command";
	expHost = info.hostname;
	expExecutableName = info.exename;
	expCollector = info.collector;
	expPosixTid = infoheader.posix_tid;
	expPid = info.pid;
	expColId = infoheader.collector;
	expExpId = infoheader.experiment;

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cout << "info..." << std::endl
		  << "collector name: " << info.collector << std::endl
		  << "host name:      " << info.hostname << std::endl
		  << "pid:            " << info.pid << std::endl
		  << "tid:            " << info.tid << std::endl;

	std::cout << "infoheader..." << std::endl
		  << "experiment id:  " << infoheader.experiment << std::endl
		  << "collector id:   " << infoheader.collector << std::endl
		  << "host:           " << infoheader.host << std::endl
		  << "pid:            " << infoheader.pid << std::endl
		  << "posix_tid:      " << infoheader.posix_tid << std::endl
		  << "time_begin:     " << infoheader.time_begin << std::endl
		  << "time_end:       " << infoheader.time_end << std::endl
		  << "addr_begin:     " << infoheader.addr_begin << std::endl
		  << "addr_end:       " << infoheader.addr_end << std::endl;
	}
#endif
	where = ftell(f);
    } else {
	std::cerr << "process_expinfo info call FAILED" << std::endl;
	return false;
    }

    fclose(f);
    return true;
}

/*
 *
 * We handle specific xdr routines for the collector named
 * in expCollector by instantiating the collector implementation
 * in the same way that openss does.
 *
 */
bool
OfflineExperiment::process_data(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";

    FILE *f = fopen(rawfilename.c_str(),"r");
    xdrstdio_create(&xdrs, f, XDR_DECODE);

    bool_t done = false;

    while (!done) {
	unsigned int blobsize;
	if (!xdr_u_int(&xdrs, &blobsize)) {
	    //  no more entries. done.
	    break;
	}

	char *theData;
	theData = (char *) malloc (blobsize+4);
	if (theData == 0) {
	    std::cerr << "Could Not allocate memory for data!" << std::endl;
	    fclose(f);
	    abort ();
	}

	memset(theData, 0, blobsize);
	int bytesRead = fread(theData, 1, blobsize, f);
	if (bytesRead == 0 || bytesRead < blobsize) {
	    // FIXME: What does this mean?
	    //std::cerr << "Could Not read any more data..." << std::endl;
	}

	// For offline collection we really maintain one dataqueue.
	// So the collector runtimes need to set the header.experiment to 0.
	// This is the first index into the DataQueue.
	DataQueues::enqueuePerformanceData(Blob(blobsize, theData));
	theExperiment->flushPerformanceData();

    } // while

    fclose(f);
    return true;
}

bool OfflineExperiment::process_objects(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";

    FILE *f = fopen(rawfilename.c_str(),"r");
    xdrstdio_create(&xdrs, f, XDR_DECODE);

    bool_t done = false;
    while (!done) {
	unsigned int blobsize;
	if (!xdr_u_int(&xdrs, &blobsize)) {
	    // just means no more items in this file...
	    break;
	}

	// dso or executable blob.
	openss_objects objs;
	memset(&objs, 0, sizeof(objs));

	OpenSS_DataHeader objsheader;
	memset(&objsheader, 0, sizeof(objsheader));
	bool_t objsheadercall = xdr_OpenSS_DataHeader(&xdrs, &objsheader);

	if(!objsheadercall) {
	    // just means no more items in this file...
	    break;
	}

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cerr << "OfflineExperiment::process_objects: "
	    << " OBJECT loaded at TIME: begin " << Time(objsheader.time_begin)
	    << " end " << Time(objsheader.time_end)
	    << std::endl;
	}
#endif

	bool_t objcall = xdr_openss_objects(&xdrs, &objs);

	if (objcall) {

	    uint64_t endaddr = objs.addr_end;
	    Address b_addr(objs.addr_begin);

	    Address e_addr(endaddr);
	    AddressRange range(b_addr,e_addr);

	    // do not include entries with no name or [vdso].
	    std::string objname = objs.objname;
            if (objname.empty()) {
		continue;
	    }

	    // TODO:  If we want to merge mpi raw data into
	    // one directory for processing, then we need to group
	    // the dsos per host-pid-thread.
	    // Skip the vdso entry from /proc/self/maps.
	    if ( objname.find("[vdso]") == string::npos ) {
		names_to_range.push_back(std::make_pair(objname,range));
	    }

//DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cout << "range for " << objs.objname << " is"
		<< range << std::endl;

	std::cout << "objectsheader..." << std::endl
		  << "experiment id:  " << objsheader.experiment << std::endl
		  << "collector id:   " << objsheader.collector << std::endl
		  << "host:           " << objsheader.host << std::endl
		  << "pid:            " << objsheader.pid << std::endl
		  << "posix_tid:      " << objsheader.posix_tid << std::endl
		  << "time_begin:     " << objsheader.time_begin << std::endl
		  << "time_end:       " << objsheader.time_end << std::endl
		  << "addr_begin:     " << objsheader.addr_begin << std::endl
		  << "addr_end:       " << objsheader.addr_end << std::endl;
        std::cout << std::endl;
	}
#endif
	}
    } // end while

    fclose(f);
    return true;
}


void OfflineExperiment::createOfflineSymbolTable()
{
    // Pass a buffer of unique sampled addresses for this thread
    // to the symbol table processing method. Currently bfd based...
    static PCBuffer data_addr_buffer;

    SymbolTableMap symtabmap;

    BFDSymbols bfd_symbols;
    FuncMap FunctionsWithData;
    StatementsVec StatementsWithData;


    // Find current threads used in this experiment.
    ThreadGroup threads;
    ThreadGroup original = theExperiment->getThreads();

    // Find any new threads.
    insert_iterator< ThreadGroup > ii( threads, threads.begin() );
    std::set_difference(original.begin(), original.end(),
			threads_processed.begin(), threads_processed.end(),
			ii);

// DEBUG
#ifndef NDEBUG
    if(is_debug_offline_enabled) {
        std::cerr << "OfflineExperiment::createOfflineSymbolTable have "
	<< threads.size() << " threads" << std::endl;

        for(ThreadGroup::const_iterator ni = threads.begin();
				    ni != threads.end(); ++ni) {
	    std::cerr << "OfflineExperiment::createOfflineSymbolTable "
	    << " NEW THREAD: " << EntrySpy(*ni).getEntry() << std::endl;

        }
    }
#endif

    insert_iterator< ThreadGroup > iu( threads_processed,
					threads_processed.begin() );
    std::set_union(original.begin(), original.end(),
			threads.begin(), threads.end(),
			iu);

    // Create an Extent for all time and full address range
    TimeInterval ti(Time::TheBeginning(),Time::TheEnd());
    AddressRange ar(Address::TheLowest(),Address::TheHighest());
    Extent e(ti,ar);

    // Add extent to extent group for use with getUniquePCValues.
    ExtentGroup eg;
    eg.push_back(e);

    CollectorGroup cgrp = theExperiment->getCollectors();
    //
    // Find the unique address values collected for this experiment.
    // For mpi jobs it would be better to also group threads to 
    // an executable. That would then allow us to find all the
    // sampled address from each mpi rank and then search the
    // linked objects for that group for functions and statements.
    for(ThreadGroup::const_iterator i = threads.begin();
                                    i != threads.end(); ++i) {
        Instrumentor::retain(*i);
	std::string collector_name;
	CollectorGroup::iterator ci;
	for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
	    Collector c = *ci;
	    Metadata m = c.getMetadata();
	    collector_name = m.getUniqueId();
	    c.getUniquePCValues(*i,eg,&data_addr_buffer);
	}
    }

    std::set<std::string> dsos_used;
    AddressSpace address_space;

    for (unsigned i = 0; i < data_addr_buffer.length; i++) {
 	Address addr = Address(data_addr_buffer.pc[i]);

	// create list of dso's for which there is an address in the performance
	// data. Update database address_space with threads and linked objects.
	// Mark the executable (is_executable = true, shared librarys = false).
	for(std::vector<std::pair<std::string, AddressRange> >::iterator
              j = names_to_range.begin(); j != names_to_range.end(); ++j) {

	    bool_t range_found = false;
	    if (j->second.doesContain(addr) && !range_found) {
		range_found = true;
		if (dsos_used.find(j->first) == dsos_used.end()) {

		    dsos_used.insert(j->first);
		    bool is_exe = false;

		    if (j->first == expExecutableName) {
			is_exe = true;
		    }
// DEBUG
#ifndef NDEBUG
		    if(is_debug_offline_enabled) {
		      std::cerr << "OfflineExperiment::createOfflineSymbolTable:"
			<< " Added " << j->first
			<< " to database linkedobjects, is executable "
			<< is_exe << std::endl;
		    }
#endif
		    address_space.setValue(j->second, j->first,
					   /*is_executable*/ is_exe);
		}
	    }
	} 
    }

    // creates empty symboltable for these.
    std::map<AddressRange, std::set<LinkedObject> > needed =
    address_space.updateThreads(threads, Time::TheBeginning(),
				/*update_time_interval*/ true);

    // Iterate over each needed symbol table for this address space
    for(std::map<AddressRange, std::set<LinkedObject> >::const_iterator
	    t = needed.begin(); t != needed.end(); ++t) {

// DEBUG
#ifndef NDEBUG
    if(is_debug_offline_enabled) {
	for (std::set<LinkedObject>::const_iterator
		l = t->second.begin();
		l != t->second.end(); ++l) {
	    std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	    << " INSERT SYMTAB for " <<  (*l).getPath()
	    << " RANGE " << t->first
	    << std::endl;
	}
    }
#endif

	// Add an empty symbol table to the state for this address range
	symtabmap.insert( std::make_pair(t->first,
			  std::make_pair(SymbolTable(t->first), t->second))
			);
    }

    int addresses_found =-1;
 
    // TODO: Make this configurable for use with symtabAPI from dyninst.
    // Using bfd to find symbols.
    // Foreach linked object we find that has pc data,
    // pass the buffer of PC addresses to getBFDFunctionStatements.
    std::set<LinkedObject> tgrp_lo = threads.getLinkedObjects();
    for(std::set<LinkedObject>::const_iterator j = tgrp_lo.begin();
		j != tgrp_lo.end(); ++j) {
	    LinkedObject lo = (*j);
            addresses_found =
		bfd_symbols.getBFDFunctionStatements(&data_addr_buffer, &lo);
    }

    FunctionsWithData = bfd_symbols.bfd_functions;
    StatementsWithData =  bfd_symbols.bfd_statements;

// DEBUG
#ifndef NDEBUG
    if(is_debug_offline_enabled) {
      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " getBFDFunctionStatements found functions for " << addresses_found
	<< " sample addresses from total "
	<< data_addr_buffer.length << std::endl;

      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " Functions with DATA: " << FunctionsWithData.size()
	<< std::endl;
      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " StatementsWithData with DATA: " << StatementsWithData.size()
	<< std::endl;
    }
#endif

    // Add Functions
    // RESTRICT functions to only those with sampled addresses.
    int functionsadded = 0;
    for(FuncMap::const_iterator ic = FunctionsWithData.begin();
				ic != FunctionsWithData.end(); ++ic) {
	AddressRange frange(ic->second.func_begin,ic->second.func_end);
        if (symtabmap.find(frange) != symtabmap.end()) {
	    SymbolTable& symbol_table =  symtabmap.find(frange)->second.first;
	    Address start = ic->second.func_begin;
	    Address end = ic->second.func_end;
// DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
              std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
		<< "Add to symboltable function " << ic->first
		<< " with range " << frange << std::endl;
	    }
#endif
	    symbol_table.addFunction(start, end, ic->first);
	    functionsadded++;
	}
    }

    // Add Statements
    // RESTRICT statement to only those with sampled addresses.
    int statementsadded = 0;
    for(StatementsVec::iterator objsyms = StatementsWithData.begin() ;
	objsyms != StatementsWithData.end(); ++objsyms) {
      AddressRange frange(objsyms->pc,objsyms->pc+1);
      if (symtabmap.find(frange) != symtabmap.end()) {
	SymbolTable& symbol_table =  symtabmap.find(frange)->second.first;
	Address s_begin = objsyms->pc;
	Address s_end = objsyms->pc + 1;
	std::string path = objsyms->file_name;
	unsigned int line = objsyms->lineno;
	unsigned int col = 0;
// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
            std::cerr << "OfflineExperiment::createOfflineSymbolTable"
		<< " ADDING STATEMENT for " << Address(objsyms->pc)
		<< " with path " << path
		<< " line " << line
		<< std::endl;
	}
#endif
	if (path.size() != 0) {
	    symbol_table.addStatement(s_begin,s_end,path,line,col);
	    statementsadded++;
	}
      }
    }

    std::cerr << "Added " << functionsadded
	<< " functions to symboltable" << std::endl;
    std::cerr << "Added " << statementsadded
	<< " statements to symboltable" << std::endl;

    // Now update the database with all our functions and statements...
    for(SymbolTableMap::iterator i = symtabmap.begin();
        i != symtabmap.end();
        ++i) {
        for(std::set<LinkedObject>::const_iterator j = i->second.second.begin();
            j != i->second.second.end();
            ++j) {
            i->second.first.processAndStore(*j);
        }
    }

    // clear names to range for our next linked object.
    names_to_range.clear();
}
