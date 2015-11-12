////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2015 The Krell Institute. All Rights Reserved.
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
 * Declaration of the SymtabAPISymbols class.
 *
 */

#include "SymtabAPISymbols.hxx"
#include "Address.hxx"
#include "AddressRange.hxx"
#include "LinkedObject.hxx"
#include "Function.hxx"
#include "Statement.hxx"
/* These are coming from the Dyninst include directory */
#include "Symtab.h"
#include "LineInformation.h"
#include "Function.h"


using namespace OpenSpeedShop::Framework;
using namespace Dyninst;
using namespace Dyninst::SymtabAPI;

#ifndef NDEBUG
/** Flag indicating if debuging for offline symbols is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_enabled =
    (getenv("OPENSS_DEBUG_SYMTABAPI_SYMBOLS") != NULL);
/** Flag indicating if debuging for detailed offline symbols is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_detailed_enabled =
    (getenv("OPENSS_DEBUG_SYMTABAPI_SYMBOLS_DETAILS") != NULL);
#endif


void
SymtabAPISymbols::getSymbols(const std::set<Address>& addresses,
			     const LinkedObject& linkedobject,
			     SymbolTableMap& stm)
{
    std::string objname = linkedobject.getPath();
    std::set<AddressRange> lorange = linkedobject.getAddressRange();
    std::set<AddressRange>::iterator si;
    AddressRange range_for_stm;
    Framework::Address base_for_stm(0);
    bool found_symtab = false;

    //  A linkedobject can be loaded in differing addressranges
    //  across processes.  So we cycle though the unique
    //  address ranges finding matching sample addresses and
    //  then resolving symbols. But, a symboltable is only
    //  asociated with one address range.  So all functions
    //  and possibly statements need to be added to the symboltable
    //  using the base for the addressrange of the symboltable
    //  found.  Therefore we use range_for_stm and base_for_stm.
    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	AddressRange lrange;


// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "Processing linked object "
	    << objname << " with address range " << (*si)
	    << " addresses length is " << addresses.size()
	    << std::endl;
	}
#endif

	lrange = (*si);
	if (!found_symtab || stm.find(*si) != stm.end()) {
	    range_for_stm = lrange;
	    found_symtab = true;
	}

	if (found_symtab) {
	    SymbolTable& st =  stm.find(range_for_stm)->second.first;

	    Symtab *symtab;

	    bool err = Symtab::openFile(symtab, objname);

            // The object may not have any symbols, so skip it.
            if (symtab == NULL) {
               return;
            }

	    Framework::Address image_offset(symtab->imageOffset());
	    Framework::Address image_length(symtab->imageLength());
	    AddressRange image_range(image_offset,image_offset+image_length);

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
	        std::cerr << "Image range of " << objname << " is " << image_range << std::endl;
	    }
#endif

	    Framework::Address base(0);
	    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
		base = lrange.getBegin();
	    }

	    if ( (image_range.getBegin() - range_for_stm.getBegin()) < 0 ) {
		base_for_stm = range_for_stm.getBegin();
	    }

#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
	        std::cerr << "base for " << objname << " is " << base << std::endl;
	        std::cerr << "base for symboltable is " << base_for_stm << std::endl;
	    }
#endif

	    std::vector <SymtabAPI::Function *>fsyms;


            // Make sure we get the full filename
	    if(symtab) {
                symtab->setTruncateLinePaths(false);
            }

	    if(symtab && !symtab->getAllFunctions(fsyms)) {
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "Dyninst::SymtabAPI::Symbol::getAllFunctions unable to get functions\n`"
		    << "from " << objname << " range: " << image_range << "\n"
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
		}
#endif

	    }


	    // set up interators to only iterate over addresses that
	    // are within the range of the shared object we are processing.
	    // Do this out side of the loop on all symtabpai function symbols.
	    // The same will be done later on symtabpai statements. 
	    std::set<Address>::iterator ai;
	    std::set<Address>::iterator ai_begin = addresses.equal_range(lrange.getBegin()).first;
	    std::set<Address>::iterator ai_end = addresses.equal_range(lrange.getEnd()).second;
	    std::set<Framework::Address> function_begin_addresses;
	    std::vector<SymtabAPI::Function *>::iterator fsit;

	    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
		unsigned int sym_size = (*fsit)->getSize();
		Framework::Address begin((*fsit)->getOffset());
		Framework::Address end(begin + sym_size);

		// don't let an invalid range through...
		if (begin >= end) continue;

		AddressRange frange(begin,end);
		std::string fname =
				(*fsit)->getFirstSymbol()->getMangledName();
#if 0
		            std::cerr << "TESTING FUNCTION " << fname
			    << " RANGE " << frange
			    << " REALRANGE " << AddressRange(frange.getBegin()+base,frange.getEnd()+base)
			    << std::endl;
#endif

		for (ai=ai_begin; ai!=ai_end; ++ai)
		{
		    // normalize address for testing range from symtabapi.
		    Framework::Address theAddr(*ai - base.getValue()) ; 
		    if (frange.doesContain( theAddr)) {

// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
		            std::cerr << "ADDING FUNCTION " << fname
			    << " RANGE " << AddressRange(frange.getBegin()+base,frange.getEnd()+base)
			    << " for pc " << *ai
			    << std::endl;
			}
#endif
			// use base_for_stm when adding a symbol. All linked objects
			// in our symboltable record only one address range.
			st.addFunction(begin+base_for_stm,end+base_for_stm,fname);

			// Record the function begin addresses, This allows the
			// cli and gui to focus on or display the first
			// statement of a function.
			// The function begin addresses will be processed later
			// for statement info and added to our statements.
			function_begin_addresses.insert(begin+base_for_stm);
			break;

		    }
		}
	    }


	    std::vector <Module *>mods;
	    AddressRange module_range;
	    std::string module_name;
	    if(symtab && !symtab->getAllModules(mods)) {
		std::cerr << "getAllModules unable to get all modules  "
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
	    } else {
// DEBUG
#ifndef NDEBUG
	        if(is_debug_symtabapi_symbols_detailed_enabled) {
		    for(unsigned i = 0; i< mods.size();i++){
		        module_range =
			   AddressRange(mods[i]->addr(),
					mods[i]->addr() + symtab->imageLength());
		        module_name = mods[i]->fullName();
		        std::cerr << "getAllModules module name " << mods[i]->fullName()
			    << " Range " << module_range
			    << std::endl;
		    }
		}
#endif
	    }

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {

	        std::cerr << "symtabAPISymbols: image_offset " << image_offset
		<< " image_length " << image_length
		<< " image_range " << image_range << std::endl;

		std::cerr << "USING BASE OFFSET " << base << std::endl;
	    }
#endif

	    for (ai=ai_begin; ai!=ai_end; ++ai)
	    {
		// normalize address for testing range from symtabapi.
		Framework::Address theAddr(*ai - base.getValue()) ; 
		Offset myoffset = theAddr.getValue();

	      for(unsigned i = 0; i< mods.size();i++) {
		std::vector< Dyninst::SymtabAPI::Statement *> mylines;
		mods[i]->getSourceLines(mylines,myoffset);

		Dyninst::SymtabAPI::Statement *real_stmt;
		Offset real_stmt_size = 0;
		if (mylines.size() > 0) {
		    for (std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
			 si != mylines.end(); si++) {
			Offset stmt_size = (*si)->endAddr() - (*si)->startAddr();
#if 0
			std::cerr << "stmt_size size is " << stmt_size << " for offset " << myoffset
				  << " " << (*si)->getFile()
				  << " " << (*si)->getLine()
				  << " numlines found " << mylines.size()
			<< std::endl;
#endif
			// Due to faulty dwarf information from some compilers, we
			// can get multiple statements corresponding to a sample address.
			// We really only expect one statement at this point. So we
			// choose the statement with the smallest address range.
			// seen with adagio at SNL.
			if (real_stmt_size == 0) {
			    // this sets real_stmt to first stmt from mylines.
			    // If there is only one statment in mylines, this is used.
			    real_stmt = *si;
			    real_stmt_size = stmt_size;
			} else if (stmt_size < real_stmt_size) {
			    // if more that one statement is in mylines, we use the
			    // statement with the smallest address range.
			    real_stmt = *si;
			    real_stmt_size = stmt_size;
			}
		    }
// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
			    std::cerr << " SAMPLE Address:" << theAddr + base
				  << " File:" << real_stmt->getFile()
				  << " Line:" << real_stmt->getLine()
				  << " Column:" << (int) real_stmt->getColumn()
				  << " startAddr:" << Framework::Address(real_stmt->startAddr()) +base
				  << " endAddr:" << Framework::Address(real_stmt->endAddr()) +base
				  << std::endl;
			}
#endif
			// add the base offset back when recording statement.
			st.addStatement(Framework::Address(real_stmt->startAddr()) +base,
					Framework::Address(real_stmt->endAddr()) +base,
					real_stmt->getFile(),
					real_stmt->getLine(),
					(int) real_stmt->getColumn()
					);
		}
	      } // mods loop
	    } // sampled addresses loop

	    // Find any statements for the beginning of a function that
	    // contained a valid sample address.
	    for(std::set<Framework::Address>::const_iterator fi = function_begin_addresses.begin();
			                                      fi != function_begin_addresses.end();
			                                      ++fi) {
		// normalize address for testing range from symtabapi.
		Framework::Address theAddr(*fi - base.getValue()) ; 
		Offset myoffset = theAddr.getValue();
// DEBUG
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_detailed_enabled) {
		    std::cerr
			  << "FIND BEGIN STATMENT for function begin at address " << theAddr
			  << " base " << base
			  << " fi " << *fi
			  << std::endl;
		}
#endif
	        for(unsigned i = 0; i< mods.size();i++) {
		  std::vector< Dyninst::SymtabAPI::Statement *> mylines;
		  mods[i]->getSourceLines(mylines,myoffset);
		  if (mylines.size() > 0) {
		    for(std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
			si != mylines.end(); si++) {

// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
			std::cerr << " FUNCTION BEGIN Address:" << theAddr + base
				  << " File:" << (*si)->getFile()
				  << " Line:" << (*si)->getLine()
				  << " Column:" << (int) (*si)->getColumn()
				  << " startAddr:" << Framework::Address((*si)->startAddr()) +base
				  << " endAddr:" << Framework::Address((*si)->endAddr()) +base
				  << std::endl;
			}
#endif

			// add the base offset back when recording statement.
			st.addStatement(Framework::Address((*si)->startAddr()) +base,
					Framework::Address((*si)->endAddr()) +base,
					(*si)->getFile(),
					(*si)->getLine(),
					(int) (*si)->getColumn()
					);
		    }
		}
	      } // mods loop
	    } // function begin statement loop
	} // if stm loop
	else {
	    std::cerr << "NO STM "
	    << objname << " with address range " << lrange
	    << std::endl;
	}
    }
}

void
SymtabAPISymbols::getSymbols(PCBuffer* addrbuf,
			     const LinkedObject& linkedobject,
			     SymbolTableMap& stm)
{
    std::set<Address> addresses;
    std::set<Address>::iterator ai;
    for (unsigned ii = 0; ii < addrbuf->length; ++ii) {
	addresses.insert(Address(addrbuf->pc[ii]));
    }

    std::string objname = linkedobject.getPath();
    std::set<AddressRange> lorange = linkedobject.getAddressRange();
    std::set<AddressRange>::iterator si;

    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	AddressRange lrange;


// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "Processing linked object "
	    << objname << " with address range " << (*si)
	    << " addrbuf->length is " << addrbuf->length
	    << std::endl;
	}
#endif

	lrange = (*si);

	if (stm.find(*si) != stm.end()) {
	    SymbolTable& st =  stm.find(*si)->second.first;

	    Symtab *symtab;

	    bool err = Symtab::openFile(symtab, objname);

            // The object may not have any symbols, so skip it.
            if (symtab == NULL) {
               return;
            }

	    Framework::Address image_offset(symtab->imageOffset());
	    Framework::Address image_length(symtab->imageLength());
	    AddressRange image_range(image_offset,image_offset+image_length);

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
	        std::cerr << "Image range of " << objname << " is " << image_range << std::endl;
	    }
#endif

	    Framework::Address base(0);
	    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
		base = lrange.getBegin();
	    }

	    std::vector <SymtabAPI::Function *>fsyms;
            
            // Make sure we get the full filename
	    if(symtab) {
                symtab->setTruncateLinePaths(false);
            }

	    if(symtab && !symtab->getAllFunctions(fsyms)) {
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "Dyninst::SymtabAPI::Symbol::getAllFunctions unable to get functions\n`"
		    << "from " << objname << " range: " << image_range << "\n"
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
		}
#endif

	    }


	    std::set<Framework::Address> function_begin_addresses;
	    std::vector<SymtabAPI::Function *>::iterator fsit;

	    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
		int sym_size = (*fsit)->getSize();
		Framework::Address begin((*fsit)->getOffset());
		Framework::Address end(begin + sym_size);

		// don't let an invalid range through...
		if (begin >= end) continue;

		AddressRange frange(begin,end);

		for (ai=addresses.equal_range(lrange.getBegin()).first;
	     		ai!=addresses.equal_range(lrange.getEnd()).second;ai++) {
		    // normalize address for testing range from symtabapi.
		    Framework::Address theAddr(*ai - base.getValue()) ; 
		    if (frange.doesContain( theAddr )) {

			std::string fname =
				(*fsit)->getFirstSymbol()->getPrettyName();
// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
		            std::cerr << "ADDING FUNCTION " << fname
			    << " RANGE " << frange
			    << " for pc " << *ai
			    << " adjusted pc " << theAddr
			    << std::endl;
			}
#endif
			st.addFunction(begin+base,end+base,fname);

			// Record the function begin addresses, This allows the
			// cli and gui to focus on or display the first
			// statement of a function.
			// The function begin addresses will be processed later
			// for statement info and added to our statements.
			function_begin_addresses.insert(begin);
			break;
		    }
		}
	    }


	    std::vector <Module *>mods;
	    AddressRange module_range;
	    std::string module_name;
	    if(symtab && !symtab->getAllModules(mods)) {
		std::cerr << "getAllModules unable to get all modules  "
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
	    } else {
// DEBUG
#ifndef NDEBUG
	        if(is_debug_symtabapi_symbols_detailed_enabled) {
		    for(unsigned i = 0; i< mods.size();i++){
		        module_range =
			   AddressRange(mods[i]->addr(),
					mods[i]->addr() + symtab->imageLength());
		        module_name = mods[i]->fullName();
		        std::cerr << "getAllModules module name " << mods[i]->fullName()
			    << " Range " << module_range
			    << std::endl;
		    }
		}
#endif
	    }

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {

	        std::cerr << "symtabAPISymbols: image_offset " << image_offset
		<< " image_length " << image_length
		<< " image_range " << image_range << std::endl;

		std::cerr << "USING BASE OFFSET " << base << std::endl;
	    }
#endif

	    for (ai=addresses.equal_range(lrange.getBegin()).first;
	     		    ai!=addresses.equal_range(lrange.getEnd()).second;ai++) {
		// normalize address for testing range from symtabapi.
		Framework::Address theAddr(*ai - base.getValue()) ; 
		Offset myoffset = theAddr.getValue();

	      for(unsigned i = 0; i< mods.size();i++) {
		std::vector< Dyninst::SymtabAPI::Statement *> mylines;
		mods[i]->getSourceLines(mylines,myoffset);
		if (mylines.size() > 0) {
		    for (std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
			 si != mylines.end(); si++) {
// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
			    std::cerr << " SAMPLE Address:" << theAddr + base
				  << " File:" << (*si)->getFile()
				  << " Line:" << (*si)->getLine()
				  << " Column:" << (int) (*si)->getColumn()
				  << " startAddr:" << Framework::Address((*si)->startAddr()) +base
				  << " endAddr:" << Framework::Address((*si)->endAddr()) +base
				  << std::endl;
			}
#endif
			// add the base offset back when recording statement.
			st.addStatement(Framework::Address((*si)->startAddr()) +base,
					Framework::Address((*si)->endAddr()) +base,
					(*si)->getFile(),
					(*si)->getLine(),
					(int) (*si)->getColumn()
					);
		    }
		}
	      } // mods loop
	    } // sampled addresses loop

	    // Find any statements for the beginning of a function that
	    // contained a valid sample address.
	    for(std::set<Framework::Address>::const_iterator fi = function_begin_addresses.begin();
			                                      fi != function_begin_addresses.end();
			                                      ++fi) {
		// Lookup address by subtracting base offset.
		Framework::Address theAddr(*fi - base.getValue()) ; 
		Offset myoffset = theAddr.getValue();
	        for(unsigned i = 0; i< mods.size();i++) {
		  std::vector< Dyninst::SymtabAPI::Statement *> mylines;
		  mods[i]->getSourceLines(mylines,myoffset);
		  if (mylines.size() > 0) {
		    for(std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
			si != mylines.end(); si++) {

// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
			std::cerr << " FUNCTION BEGIN Address:" << theAddr + base
				  << " File:" << (*si)->getFile()
				  << " Line:" << (*si)->getLine()
				  << " Column:" << (int) (*si)->getColumn()
				  << " startAddr:" << Framework::Address((*si)->startAddr()) +base
				  << " endAddr:" << Framework::Address((*si)->endAddr()) +base
				  << std::endl;
			}
#endif

			// add the base offset back when recording statement.
			st.addStatement(Framework::Address((*si)->startAddr()) +base,
					Framework::Address((*si)->endAddr()) +base,
					(*si)->getFile(),
					(*si)->getLine(),
					(int) (*si)->getColumn()
					);
		    }
		}
	      } // mods loop
	    } // function begin statement loop
	} // if stm loop
    }
}


void
SymtabAPISymbols::getDepenentLibs(const std::string& objname,
           std::vector<std::string>& dependencies)
{
    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);
    if (symtab) {
        dependencies = symtab->getDependencies();
        symtab->setTruncateLinePaths(false);
    }
}


bool SymtabAPISymbols::foundLibrary(const std::string& exename, const std::string& libname)
{

    std::vector<std::string> depends;
    getDepenentLibs(exename,depends);
    for (std::vector<std::string>::iterator curDep = depends.begin(); curDep != depends.end(); curDep++) {
      if (curDep->find(libname) != std::string::npos) {
        return true;
      }
    }
   return false;
}
