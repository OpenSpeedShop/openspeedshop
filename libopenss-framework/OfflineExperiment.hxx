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

#ifndef _OpenSpeedShop_Framework_OfflineExperiment_
#define _OpenSpeedShop_Framework_OfflineExperiment_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SymbolTable.hxx"
#include "OfflineParameters.hxx"
#if defined(BUILD_CBTF)
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#endif

namespace OpenSpeedShop { namespace Framework {

typedef std::map<AddressRange,
        std::pair<SymbolTable, std::set<LinkedObject> > >
        SymbolTableMap;

    class Collector;
    class CollectorGroup;
    class Extent;
    class Function;
    class LinkedObject;
    class Statement;
    class Thread;
    class ThreadGroup;

class OfflineExperiment
{
    public:
    OfflineExperiment (std::string data_base_name, std::string dirname) {
	if (!Experiment::isAccessible(data_base_name)) {
	    Experiment::create (data_base_name);
	}
	theExperiment = new Experiment (data_base_name);
	rawdatadir = dirname;
    }

    ~OfflineExperiment() {
        delete theExperiment;
    }

    void CopyDB (std::string New_DB)
    {
        if (theExperiment != NULL) {
          theExperiment->copyTo(New_DB);
        }
    }

    Experiment *getExperiment() {return theExperiment;}
    void findUniqueAddresses();

    int		getRawDataFiles(std::string dir);
    int		convertToOpenSSDB();
    void	createOfflineSymbolTable();
    void	finalizeDB();
    void	findThreadsForExe(std::set<std::pair<LinkedObject,ThreadGroup> > &);
    void	processOfflineSymbolTable();
    void	getAddressesForThreads(ThreadGroup&, PCBuffer *);

    std::string expHost;
    std::string expCollector;
    std::set<std::string> expExecutableName;

    int		expPid;
    uint64_t	expPosixTid;
    int		expExpId;
    int		expColId;
    int		expRate;
    int		expRank;
    int		expOmpTid;
    std::string	expEvent;
    std::string expTraced;

    bool	process_expinfo(const std::string rawfilename);
    bool	process_data(const std::string rawfilename);
    bool	process_objects(const std::string rawfilename);
#if defined(BUILD_CBTF)
    // with cbtf-krell collection we use only one file to
    // record performance data and then append the linked object
    // information to that file (.openss-data).
    bool	process_cbtf_objects(const std::string rawfilename);
    std::vector<Blob>        cbtf_objs_blobs;
#endif

    Experiment *theExperiment;

    private:

    SymbolTableMap symtabmap;

    std::vector<std::string> rawfiles;
    std::string rawdatadir;
    std::string DBname;

    ThreadGroup threads_processed;

    // mpi experiments create multiple directories
    std::vector<std::string> rawdirs;

    bool is_cbtf_data;

#ifndef NDEBUG
    static bool is_debug_offlinesymbols_enabled;
    static bool is_debug_offlinesymbols_detailed_enabled;
    static bool is_debug_offline_enabled;
#endif

};

} }

#endif
