////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

#include "ToolAPI.hxx"
#include "CUDAData.hxx"

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
string xyz(const string& tag, const CUDAData::Vector3u& value)
{
    stringstream stream;
    stream << "  <" << tag
           << " x=\"" << value.get<0>() << "\""
           << " y=\"" << value.get<1>() << "\""
           << " z=\"" << value.get<2>() << "\""
           << "/>" << endl;
    return stream.str();
}



/** Convert a thread into XML and output it to a stream. */
void convert(const Thread& thread, ostream& xml)
{
    xml << endl << "<Thread>" << endl;
    xml << "  <Host>" << thread.getHost() << "</Host>" << endl;
    xml << "  <ProcessId>" << thread.getProcessId() << "</ProcessId>" << endl;
    
    pair<bool, pthread_t> posix = thread.getPosixThreadId();
    if (posix.first)
    {
        xml << "  <PosixThreadId>" << posix.second 
            << "</PosixThreadId>" << endl;
    }

    pair<bool, int> openmp = thread.getOpenMPThreadId();
    if (openmp.first)
    {
        xml << "  <OpenMPThreadId>" << openmp.second
            << "</OpenMPThreadId>" << endl;
    }
            
    pair<bool, int> mpi = thread.getMPIRank();
    if (mpi.first)
    {
        xml << "  <MPIRank>" << mpi.second << "</MPIRank>" << endl;
    }
    
    xml << "</Thread>" << endl;    
}



/** Convert call sites into XML and output them to a stream. */
void convert(const vector<StackTrace>& call_sites, ostream& xml)
{
    for (vector<StackTrace>::size_type i = 0; i < call_sites.size(); ++i)
    {
        const StackTrace& trace = call_sites[i];

        xml << endl << "<CallSite id=\"" << i << "\">" << endl;
        
        for (vector<Address>::size_type j = 0; j < trace.size(); ++j)
        {
            xml << "  <Frame>" << endl;
            xml << "    <Address>" << trace[j] << "</Address>" << endl;

            pair<bool, LinkedObject> linked_object = trace.getLinkedObjectAt(j);
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

        xml << "</CallSite>" << endl;
    }
}



/** Convert counters into XML and output them to a stream. */
void convert(const vector<string>& counters, ostream& xml)
{
    xml << endl;
    for (vector<string>::size_type i = 0; i < counters.size(); ++i)
    {
        xml << "<Counter id=\"" << i << "\">"
            << counters[i]
            << "</Counter>" << endl;
    }
}



/** Convert devices into XML and output them to a stream. */
void convert(const vector<CUDAData::DeviceDetails>& devices, ostream& xml)
{
    for (vector<CUDAData::DeviceDetails>::size_type 
             i = 0; i < devices.size(); ++i)
    {
        const CUDAData::DeviceDetails& device = devices[i];
        
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
        xml << text("MaxBlocksPerMultiprocessor",
                    device.max_blocks_per_multiprocessor);
        xml << text("MaxRegistersPerBlock", device.max_registers_per_block);
        xml << text("MaxSharedMemoryPerBlock",
                    device.max_shared_memory_per_block);
        xml << text("MaxThreadsPerBlock", device.max_threads_per_block);
        xml << "</Device>" << endl;
    }
}



/** Convert a kernel execution into XML and output it to a stream. */
void convert_kernel_execution(
    const Time& time_origin,
    const CUDAData::KernelExecutionDetails& details,
    ostream& xml
    )
{
    xml << endl << "<KernelExecution"
        << " call_site=\"" << details.call_site << "\""
        << " device=\"" << details.device << "\""
        ">" << endl;
    xml << text("Time", details.time - time_origin);
    xml << text("TimeBegin", details.time_begin - time_origin);
    xml << text("TimeEnd", details.time_end - time_origin);
    xml << text("Function", demangle(details.function));
    xml << xyz("Grid", details.grid);
    xml << xyz("Block", details.block);
    xml << text("CachePreference",
                CUDAData::stringify(details.cache_preference));
    xml << text("RegistersPerThread", details.registers_per_thread);
    xml << text("StaticSharedMemory", details.static_shared_memory);
    xml << text("DynamicSharedMemory", details.dynamic_shared_memory);
    xml << text("LocalMemory", details.local_memory);
    xml << "</KernelExecution>" << endl;
}



/** Convert a memory copy into XML and output it to a stream. */
void convert_memory_copy(
    const Time& time_origin,
    const CUDAData::MemoryCopyDetails& details,
    ostream& xml
    )
{
    xml << endl << "<MemoryCopy"
        << " call_site=\"" << details.call_site << "\""
        << " device=\"" << details.device << "\""
        ">" << endl;
    xml << text("Time", details.time - time_origin);
    xml << text("TimeBegin", details.time_begin - time_origin);
    xml << text("TimeEnd", details.time_end - time_origin);
    xml << text("Size", details.size);
    xml << text("Kind", CUDAData::stringify(details.kind));
    xml << text("SourceKind", CUDAData::stringify(details.source_kind));
    xml << text("DestinationKind",
                CUDAData::stringify(details.destination_kind));
    xml << text("Asynchronous", (details.asynchronous ? "true" : "false"));
    xml << "</MemoryCopy>" << endl;
}



/** Convert a memory set into XML and output it to a stream. */
void convert_memory_set(
    const Time& time_origin,
    const CUDAData::MemorySetDetails& details,
    ostream& xml
    )
{
    xml << endl << "<MemorySet"
        << " call_site=\"" << details.call_site << "\""
        << " device=\"" << details.device << "\""
        ">" << endl;
    xml << text("Time", details.time - time_origin);
    xml << text("TimeBegin", details.time_begin - time_origin);
    xml << text("TimeEnd", details.time_end - time_origin);
    xml << text("Size", details.size);
    xml << "</MemorySet>" << endl;
}



/** Convert a periodic sample into XML and output it to a stream. */
void convert_periodic_sample(
    const Time& time_origin,
    const Time& time,
    const vector<uint64_t>& counts,
    ostream& xml
    )
{
    xml << "<Sample>" << endl;
    xml << text("Time", time - time_origin);
    for (vector<uint64_t>::size_type i = 0; i < counts.size(); ++i)
    {
        xml << "  <Count counter=\"" << i << "\">"
            << counts[i]
            << "</Count>" << endl;
    }
    xml << "</Sample>" << endl;
}



/** Convert CUDA performance data into XML and output it to a stream. */
void convert(const Time& time_origin, const CUDAData& data, ostream& xml)
{
    convert(data.call_sites(), xml);
    convert(data.counters(), xml);
    convert(data.devices(), xml);

    boost::function<
        void (const CUDAData::KernelExecutionDetails&)
        > kernel_execution_visitor(
            boost::bind(&convert_kernel_execution,
                        boost::cref(time_origin), _1, boost::ref(xml))
            );
    
    data.visit_kernel_executions(kernel_execution_visitor);

    boost::function<
        void (const CUDAData::MemoryCopyDetails&)
        > memory_copy_visitor(
            boost::bind(&convert_memory_copy,
                        boost::cref(time_origin), _1, boost::ref(xml))
            );
    
    data.visit_memory_copies(memory_copy_visitor);

    boost::function<
        void (const CUDAData::MemorySetDetails&)
        > memory_set_visitor(
            boost::bind(&convert_memory_set,
                        boost::cref(time_origin), _1, boost::ref(xml))
            );
    
    data.visit_memory_sets(memory_set_visitor);

    boost::function<
        void (const Time&, const vector<uint64_t>&)
        > periodic_sample_visitor(
            boost::bind(&convert_periodic_sample,
                        boost::cref(time_origin), _1, _2, boost::ref(xml))
            );

    xml << endl;
    data.visit_periodic_samples(periodic_sample_visitor);
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

    // Parse and validate the command-line arguments
    
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

    //
    // NOTE: Currently each dataset contains one (and only one) thread. Ideally
    // in the future some sort of cluster analysis will be used to group similar
    // performing threads together into a single dataset.
    //

    typedef vector<pair<ThreadGroup, shared_ptr<CUDAData> > > DataSets;
    
    Time time_origin = Time::TheEnd();
    DataSets datasets;
    
    ThreadGroup threads = experiment.getThreads();
    for (ThreadGroup::const_iterator
             i = threads.begin(); i != threads.end(); ++i)
    {
        pair<bool, int> rank = i->getMPIRank();
        
        if (ranks.empty() || 
            (rank.first && (ranks.find(rank.second) != ranks.end())))
        {
            ThreadGroup thread;
            thread.insert(*i);

            shared_ptr<CUDAData> data(new CUDAData(*collector, *i));
            Time t = data->time_origin();
            
            if (t < time_origin)
            {
                time_origin = t;
            }
            
            datasets.push_back(make_pair(thread, data));
        }
    }

    *xml << endl
         << "<TimeOrigin>" << time_origin.getValue() << "</TimeOrigin>" << endl;

    for (DataSets::const_iterator
             i = datasets.begin(); i != datasets.end(); ++i)
    {
        *xml << endl << "<DataSet>" << endl;

        for (ThreadGroup::const_iterator
                 j = i->first.begin(); j != i->first.end(); ++j)
        {
            convert(*j, *xml);
        }

        convert(time_origin, *i->second.get(), *xml);

        *xml << endl << "</DataSet>" << endl;        
    }
    
    *xml << endl << "</CUDA>" << endl;
    
    if (values.count("xml") == 1)
    {
        delete xml;
    }
    
    return 0;
}
