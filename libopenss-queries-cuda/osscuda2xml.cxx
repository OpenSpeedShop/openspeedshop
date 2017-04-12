////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014-2017 Argo Navis Technologies. All Rights Reserved.
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

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <cxxabi.h>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <ArgoNavis/Base/StackTrace.hpp>
#include <ArgoNavis/Base/Time.hpp>

#include <ArgoNavis/CUDA/DataTransfer.hpp>
#include <ArgoNavis/CUDA/Device.hpp>
#include <ArgoNavis/CUDA/KernelExecution.hpp>
#include <ArgoNavis/CUDA/PerformanceData.hpp>
#include <ArgoNavis/CUDA/stringify.hpp>
#include <ArgoNavis/CUDA/Vector.hpp>

#include "ToolAPI.hxx"
#include "CUDAQueries.hxx"

using namespace ArgoNavis;
using namespace boost;
using namespace boost::program_options;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Demangle a C++ function name. */
string demangle(const string& mangled)
{
    string demangled = mangled;

    int status = -2;
    char* tmp = abi::__cxa_demangle(mangled.c_str(), NULL, NULL, &status);
    
    if (tmp != NULL)
    {
        if (status == 0)
        {
            demangled = std::string(tmp);
        }
        free(tmp);
    }

    return demangled;
}



/** Create an XML element containing a text value. */
template <typename T>
string text(const string& tag, const T& value)
{
    stringstream stream;
    stream << "  <" << tag << ">" << value << "</" << tag << ">" << endl;
    return stream.str();
}



/** Create a XML element with x, y, and z attributes from a Vector3u value. */
string xyz(const string& tag, const CUDA::Vector3u& value)
{
    stringstream stream;
    stream << "  <" << tag
           << " x=\"" << value.get<0>() << "\""
           << " y=\"" << value.get<1>() << "\""
           << " z=\"" << value.get<2>() << "\""
           << "/>" << endl;
    return stream.str();
}



/** Convert counters into XML and output them to a stream. */
void convert_counters(const CUDA::PerformanceData& data, ostream& xml)
{
    xml << endl;
    for (vector<string>::size_type i = 0; i < data.counters().size(); ++i)
    {
        xml << "<Counter id=\"" << i << "\">"
            << data.counters()[i].name
            << "</Counter>" << endl;
    }
}



/** Convert devices into XML and output them to a stream. */
void convert_devices(const CUDA::PerformanceData& data, ostream& xml)
{
    for (vector<CUDA::Device>::size_type i = 0; i < data.devices().size(); ++i)
    {
        const CUDA::Device& device = data.devices()[i];
        
        xml << endl << "<Device id=\"" << i << "\">" << endl;
        xml << text("Name", device.name);
        xml << "  <ComputeCapability"
            << " major=\"" << device.compute_capability.get<0>() << "\""
            << " minor=\"" << device.compute_capability.get<1>() << "\""
            << "/>" << endl;
        xml << xyz("MaxGrid", device.max_grid);
        xml << xyz("MaxBlock", device.max_block);
        xml << text("GlobalMemoryBandwidth",
                    1024ULL * device.global_memory_bandwidth);
        xml << text("GlobalMemorySize", device.global_memory_size);
        xml << text("ConstantMemorySize", device.constant_memory_size);
        xml << text("L2CacheSize", device.l2_cache_size);
        xml << text("ThreadsPerWarp", device.threads_per_warp);
        xml << text("CoreClockRate", 1024ULL * device.core_clock_rate);
        xml << text("MemcpyEngines", device.memcpy_engines);
        xml << text("Multiprocessors", device.multiprocessors);
        xml << text("MaxIPC", device.max_ipc);
        xml << text("MaxWarpsPerMultiprocessor",
                    device.max_warps_per_multiprocessor);
        xml << text("MaxBlocksPerMultiprocessor",
                    device.max_blocks_per_multiprocessor);
        xml << text("MaxRegistersPerBlock", device.max_registers_per_block);
        xml << text("MaxSharedMemoryPerBlock",
                    device.max_shared_memory_per_block);
        xml << text("MaxThreadsPerBlock", device.max_threads_per_block);
        xml << "</Device>" << endl;
    }
}



/** Convert call sites into Open|SpeedShop Framework StackTrace objects. */
template <typename T>
bool convert_sites_in_event(const CUDA::PerformanceData& data,
                            const Thread& thread,
                            const T& details,
                            vector<boost::shared_ptr<StackTrace> >& sites,
                            size_t& sites_found)
{
    size_t n = details.call_site;
    
    if (!sites[n])
    {
        sites[n].reset(new StackTrace(thread, Time(details.time)));
        
        for (Base::StackTrace::const_iterator
                 i = data.sites()[n].begin(); i != data.sites()[n].end(); ++i)
        {
            sites[n]->push_back(Address(*i));
        }
        
        sites_found++;
    }
    
    return sites_found < data.sites().size();
}



/** Convert call sites into Open|SpeedShop Framework StackTrace objects. */
bool convert_sites_in_thread(const CUDA::PerformanceData& data,
                             const map<Base::ThreadName, Thread>& threads,
                             const Base::ThreadName& thread,
                             vector<boost::shared_ptr<StackTrace> >& sites,
                             size_t& sites_found)
{
    data.visitDataTransfers(
        thread, data.interval(),
        bind(&convert_sites_in_event<CUDA::DataTransfer>,
             boost::cref(data), boost::cref(threads.find(thread)->second), _1,
             boost::ref(sites), boost::ref(sites_found))
        );
    
    if (sites_found == data.sites().size())
    {
        return false;
    }
    
    data.visitKernelExecutions(
        thread, data.interval(),
        bind(&convert_sites_in_event<CUDA::KernelExecution>,
             boost::cref(data), boost::cref(threads.find(thread)->second), _1,
             boost::ref(sites), boost::ref(sites_found))
        );
    
    return sites_found < data.sites().size();
}



/** Convert call sites into XML and output them to a stream. */
void convert_sites(const CUDA::PerformanceData& data, 
                   const map<Base::ThreadName, Thread>& threads,
                   ostream& xml)
{
    vector<boost::shared_ptr<StackTrace> > sites(data.sites().size());
    size_t sites_found = 0;
    
    data.visitThreads(
        bind(&convert_sites_in_thread,
             boost::cref(data), boost::cref(threads), _1, boost::ref(sites), boost::ref(sites_found))
        );
    
    for (size_t i = 0; i < sites.size(); ++i)
    {
        xml << endl << "<CallSite id=\"" << i << "\">" << endl;

        if (sites[i])
        {
            const StackTrace& trace = *sites[i];
            
            for (StackTrace::size_type j = 0; j < trace.size(); ++j)
            {
                xml << "  <Frame>" << endl;
                xml << "    <Address>" << trace[j] << "</Address>" << endl;
                
                pair<bool, LinkedObject> linked_object = 
                    trace.getLinkedObjectAt(j);
                if (linked_object.first)
                {
                    xml << "    <LinkedObject>"
                        << linked_object.second.getPath()
                        << "</LinkedObject>" << endl;
                }
                
                pair<bool, Function> function = trace.getFunctionAt(j);
                if (function.first)
                {
                    xml << "    <Function>"
                        << function.second.getDemangledName()
                        << "</Function>" << endl;
                }
                
                set<Statement> statements = trace.getStatementsAt(j);
                for (set<Statement>::const_iterator
                         k = statements.begin(); k != statements.end(); ++k)
                {
                    xml << "    <Statement>"
                        << k->getPath() << ", " << k->getLine()
                        << "</Statement>" << endl;
                }
                
                xml << "  </Frame>" << endl;
            }
        }
        else
        {
            const Base::StackTrace& trace = data.sites()[i];
            
            for (StackTrace::size_type j = 0; j < trace.size(); ++j)
            {
                xml << "  <Frame>" << endl;
                xml << "    <Address>" << trace[j] << "</Address>" << endl;
                xml << "  </Frame>" << endl;
            }
        }

        xml << "</CallSite>" << endl;
    }
}



/** Convert a data transfer into XML and output it to a stream. */
bool convert_data_transfer(const Base::Time& time_origin,
                           const CUDA::DataTransfer& details,
                           ostream& xml)
{
    xml << endl << "<DataTransfer"
        << " call_site=\"" << details.call_site << "\""
        << " device=\"" << details.device << "\""
        ">" << endl;
    xml << text("Time",
                static_cast<boost::uint64_t>(details.time - time_origin));
    xml << text("TimeBegin",
                static_cast<boost::uint64_t>(details.time_begin - time_origin));
    xml << text("TimeEnd",
                static_cast<boost::uint64_t>(details.time_end - time_origin));
    xml << text("Size", details.size);
    xml << text("Kind", CUDA::stringify(details.kind));
    xml << text("SourceKind", CUDA::stringify(details.source_kind));
    xml << text("DestinationKind", CUDA::stringify(details.destination_kind));
    xml << text("Asynchronous", (details.asynchronous ? "true" : "false"));
    xml << "</DataTransfer>" << endl;

    return true; // Always continue the visitation
}



/** Convert a kernel execution into XML and output it to a stream. */
bool convert_kernel_execution(const Base::Time& time_origin,
                              const CUDA::KernelExecution& details,
                              ostream& xml)
{
    xml << endl << "<KernelExecution"
        << " call_site=\"" << details.call_site << "\""
        << " device=\"" << details.device << "\""
        ">" << endl;
    xml << text("Time",
                static_cast<boost::uint64_t>(details.time - time_origin));
    xml << text("TimeBegin",
                static_cast<boost::uint64_t>(details.time_begin - time_origin));
    xml << text("TimeEnd",
                static_cast<boost::uint64_t>(details.time_end - time_origin));
    xml << text("Function", demangle(details.function));
    xml << xyz("Grid", details.grid);
    xml << xyz("Block", details.block);
    xml << text("CachePreference", CUDA::stringify(details.cache_preference));
    xml << text("RegistersPerThread", details.registers_per_thread);
    xml << text("StaticSharedMemory", details.static_shared_memory);
    xml << text("DynamicSharedMemory", details.dynamic_shared_memory);
    xml << text("LocalMemory", details.local_memory);
    xml << "</KernelExecution>" << endl;

    return true; // Always continue the visitation
}



/** Convert a periodic sample into XML and output it to a stream. */
bool convert_periodic_sample(const Base::Time& time_origin,
                             const Base::Time& time,
                             const vector<boost::uint64_t>& counts,
                             ostream& xml)
{
    xml << "<Sample>" << endl;
    xml << text("Time", static_cast<boost::uint64_t>(time - time_origin));
    for (vector<boost::uint64_t>::size_type i = 0; i < counts.size(); ++i)
    {
        xml << "  <Count counter=\"" << i << "\">"
            << counts[i]
            << "</Count>" << endl;
    }
    xml << "</Sample>" << endl;

    return true; // Always continue the visitation
}



/** Convert a thread into XML and output it to a stream. */
void convert_thread(const Base::ThreadName& thread, ostream& xml)
{
    xml << endl << "<Thread>" << endl;
    xml << "  <Host>" << thread.host() << "</Host>" << endl;
    xml << "  <ProcessId>" << thread.pid() << "</ProcessId>" << endl;

    if (thread.tid())
    {
        xml << "  <PosixThreadId>" << *thread.tid() 
            << "</PosixThreadId>" << endl;
    }

    if (thread.mpi_rank())
    {
        xml << "  <MPIRank>" << *thread.mpi_rank() << "</MPIRank>" << endl;
    }
    
    if (thread.omp_rank())
    {
        xml << "  <OpenMPThreadId>" << *thread.omp_rank()
            << "</OpenMPThreadId>" << endl;
    }
    
    xml << "</Thread>" << endl;    
}



/** Convert CUDA performance data into XML and output it to a stream. */
bool convert_performance_data(const CUDA::PerformanceData& data,
                              const map<Base::ThreadName, Thread>& threads,
                              const Base::ThreadName& thread,
                              ostream& xml)
{
    xml << endl << "<DataSet>" << endl;

    convert_thread(thread, xml);

    data.visitDataTransfers(
        thread, data.interval(),
        bind(&convert_data_transfer,
             boost::cref(data.interval().begin()), _1, boost::ref(xml))
        );

    data.visitKernelExecutions(
        thread, data.interval(),
        bind(&convert_kernel_execution,
             boost::cref(data.interval().begin()), _1, boost::ref(xml))
        );

    data.visitPeriodicSamples(
        thread, data.interval(),
        bind(&convert_periodic_sample,
             boost::cref(data.interval().begin()), _1, _2, boost::ref(xml))
        );
    
    xml << endl << "</DataSet>" << endl;

    return true; // Always continue the visitation
}



/**
 * Parse the command-line arguments and convert the requested CUDA performance
 * data to XML.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Command-line arguments.
 * @return        Exit code. Either 1 if a failure occurred, or 0 otherwise.
 */
int main(int argc, char* argv[])
{
    stringstream stream;
    stream << endl
           << "This tool converts CUDA performance data from the specified "
           << "Open|SpeedShop" << endl
           << "database into XML for further processing and/or visualization."
           << endl
           << endl;
    string kExtraHelp = stream.str();

    options_description kNonPositionalOptions("osscuda2xml options");
    kNonPositionalOptions.add_options()
 
        ("database", value<string>(),
         "Open|SpeedShop experiment database to be converted. May also be "
         "specified as a positional argument.")

        ("rank", value< vector<int> >(),
         "Restrict the conversion to CUDA performance data for this MPI rank. "
         "Multiple ranks may be specified.")

        ("xml", value<string>(),
         "XML file to contain the converted CUDA performance data. The XML "
         "output is sent to the standard output stream if this argument is "
         "not specified.")
        
        ("help", "Display this help message.")
        
        ;
    
    positional_options_description kPositionalOptions;
    kPositionalOptions.add("database", 1);
    
    variables_map values;
    
    try
    {
        store(command_line_parser(argc, argv).options(kNonPositionalOptions).
              positional(kPositionalOptions).run(), values);       
        notify(values);
    }
    catch (const std::exception& error)
    {
        cout << endl << "ERROR: " << error.what() << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (values.count("help") > 0)
    {
        cout << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (values.count("database") == 0)
    {
        cout << endl << "ERROR: database must be specified" << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (!Experiment::isAccessible(values["database"].as<string>()))
    {
        cout << endl << "ERROR: " << values["database"].as<string>()
             << " isn't a database" << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }

    Experiment experiment(values["database"].as<string>());

    optional<Collector> collector;
    CollectorGroup collectors = experiment.getCollectors();
    for (CollectorGroup::const_iterator
             i = collectors.begin(); i != collectors.end(); ++i)
    {
        if (i->getMetadata().getUniqueId() == "cuda")
        {
            collector = *i;
            break;
        }
    }

    if (!collector)
    {
        cout << endl << "ERROR: database " << values["database"].as<string>()
             << " doesn't contain CUDA performance data" << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;        
    }

    set<int> ranks;
    if (values.count("rank") > 0)
    {
        vector<int> temp = values["rank"].as< vector<int> >();
        ranks = set<int>(temp.begin(), temp.end());
    }

    ostream* xml = NULL;
    if (values.count("xml") == 1)
    {
        xml = new ofstream(values["xml"].as<string>().c_str());
    }
    else
    {
        xml = &cout;
    }

    *xml << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
    *xml << "<CUDA>" << endl;

    CUDA::PerformanceData data;
    map<Base::ThreadName, Thread> threads;

    ThreadGroup all_threads = experiment.getThreads();
    for (ThreadGroup::const_iterator
             i = all_threads.begin(); i != all_threads.end(); ++i)
    {
        pair<bool, int> rank = i->getMPIRank();
        
        if (ranks.empty() || 
            (rank.first && (ranks.find(rank.second) != ranks.end())))
        {
            GetCUDAPerformanceData(*collector, *i, data);
            threads.insert(make_pair(ConvertToArgoNavis(*i), *i));
        }
    }

    *xml << endl << "<TimeOrigin>"
         << static_cast<boost::uint64_t>(data.interval().begin())
         << "</TimeOrigin>" << endl;
    
    convert_counters(data, *xml);
    convert_devices(data, *xml);
    convert_sites(data, threads, *xml);
    
    data.visitThreads(bind(
        &convert_performance_data, boost::cref(data), boost::cref(threads), _1, boost::ref(*xml)
        ));
    
    *xml << endl << "</CUDA>" << endl;

    if (values.count("xml") == 1)
    {
        delete xml;
    }
    
    return 0;
}
