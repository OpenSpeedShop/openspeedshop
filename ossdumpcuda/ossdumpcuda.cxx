////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>
#include <cmath>
#include <cstdlib>
#include <cxxabi.h>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "ToolAPI.hxx"

using namespace boost;
using namespace boost::assign;
using namespace boost::tuples;
using namespace OpenSpeedShop::Framework;
using namespace std;



/** Enumeration of the attributes supported by colorize(). */
enum Attributes { Normal = 0, Bold = 1, Underline = 2, Blink = 4 };

/** Enumeration of the colors supported by colorize(). */
enum Color { None, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

/** Type of container used to store ordered fields (key/value pairs). */
typedef list<tuple<string, string> > Fields;

/**
 * Type representing a byte count. Its only reason for existence is to
 * allow the Stringify<> template below to be specialized for byte counts
 * versus other integer values.
 */
class ByteCount
{
public:
    ByteCount(const uint64_t& value) : dm_value(value) { }
    operator uint64_t() const { return dm_value; }
private:
    uint64_t dm_value;
};

/**
 * Type representing a clock rate. Its only reason for existence is to
 * allow the Stringify<> template below to be specialized for clock rates
 * versus other integer values.
 */
class ClockRate
{
public:
    ClockRate(const uint64_t& value) : dm_value(value) { }
    operator uint64_t() const { return dm_value; }
private:
    uint64_t dm_value;
};

/**
 * Type representing a function name. Its only reason for existence is to
 * allow the Stringify<> template below to be specialized for function names
 * versus other strings.
 */
class FunctionName
{
public:
    FunctionName(const char* value) : dm_value(value) { }
    FunctionName(const string& value) : dm_value(value) { }
    operator string() const { return dm_value; }
private:
    string dm_value;
};



/**
 * Colorize the specified text via ASCII escape sequences.
 *
 * @param text          String containing the text to be colorized.
 * @param foreground    Foreground color for this text.
 * @param background    Background color for this text.
 * @param attributes    Attributes (bold, underline, etc.) for this text.
 * @return              String containing the colorized text.
 */
string colorize(const string& text,
                const Color& foreground = None,
                const Color& background = None,
                const Attributes& attributes = Normal)
{
    bool semicolon = false;

    stringstream colorized;
    colorized << "\e[";

    if (foreground != None)
    {
        colorized << (29 + static_cast<int>(foreground));
        semicolon = true;
    }
    
    if (background != None)
    {
        colorized << (semicolon ? ";" : "")
                  << (39 + static_cast<int>(background));
        semicolon = true;
    }

    if (attributes & Bold)
    {
        colorized << (semicolon ? ";" : "") << "1";
        semicolon = true;
    }

    if (attributes & Underline)
    {
        colorized << (semicolon ? ";" : "") << "4";
        semicolon = true;
    }

    if (attributes & Blink)
    {
        colorized << (semicolon ? ";" : "") << "5";
        semicolon = true;
    }

    colorized << "m" << text << "\e[m";
    return colorized.str();
}



/**
 * Implementation of the stringify() function. This template is used to
 * circumvent the C++ prohibition against partial template specialization
 * of functions.
 */
template <typename T>
struct Stringify
{
    static string impl(const T& value)
    {
        return str(format("%1%") % value);
    }
};

/** Convert the specified value to a string. */
template <typename T>
string stringify(const T& value)
{
    return Stringify<T>::impl(value);
}



template <>
struct Stringify<bool>
{
    static string impl(const bool& value)
    {
        return value ? "true" : "false";
    }
};
    
template <>
struct Stringify<uint64_t>
{
    static string impl(const uint64_t& value)
    {
        return str(format("%016X") % value);
    }
};

template <typename T>
struct Stringify<vector<T> >
{
    static string impl(const vector<T>& value)
    {
        stringstream stream;

        stream << "[";
        for (typename vector<T>::size_type i = 0; i < value.size(); ++i)
        {
            stream << value[i];
            if (i < (value.size() - 1))
            {
                stream << ", ";
            }
        }
        stream << "]";

        return stream.str();
    }
};

template <>
struct Stringify<Fields>
{
    static string impl(const Fields& value)
    {
        stringstream stream;
        
        int n = 0;
        
        for (Fields::const_iterator i = value.begin(); i != value.end(); ++i)
        {
            n = max<int>(n, i->get<0>().size());
        }
        
        for (Fields::const_iterator i = value.begin(); i != value.end(); ++i)
        {
            stream << "    ";
            for (int j = 0; j < (n - i->get<0>().size()); ++j)
            {
                stream << " ";
            }
            stream << i->get<0>() << " = " << i->get<1>() << endl;
        }
        
        return stream.str();
    }
};

template <>
struct Stringify<ByteCount>
{
    static string impl(const ByteCount& value)
    {
        const struct { const double value; const char* label; } kUnits[] = {
            { 1024.0 * 1024.0 * 1024.0 * 1024.0, "TB" },
            {          1024.0 * 1024.0 * 1024.0, "GB" },
            {                   1024.0 * 1024.0, "MB" },
            {                            1024.0, "KB" },
            {                               0.0, NULL } // End-Of-Table
        };
        
        double x = static_cast<double>(value);
        string label = "Bytes";

        for (int i = 0; kUnits[i].label != NULL; ++i)
        {
            if (static_cast<double>(value) >= kUnits[i].value)
            {
                x = static_cast<double>(value) / kUnits[i].value;
                label = kUnits[i].label;
                break;
            }
        }

        return str(
            (x == floor(x)) ?
            (format("%1% %2%") % static_cast<uint64_t>(x) % label) :
            (format("%1$0.1f %2%") % x % label)
            );
    }
};

template <>
struct Stringify<ClockRate>
{
    static string impl(const ClockRate& value)
    {
        const struct { const double value; const char* label; } kUnits[] = {
            { 1024.0 * 1024.0 * 1024.0 * 1024.0, "THz" },
            {          1024.0 * 1024.0 * 1024.0, "GHz" },
            {                   1024.0 * 1024.0, "MHz" },
            {                            1024.0, "KHz" },
            {                               0.0, NULL  } // End-Of-Table
        };

        double x = static_cast<double>(value);
        string label = "Hz";

        for (int i = 0; kUnits[i].label != NULL; ++i)
        {
            if (static_cast<double>(value) >= kUnits[i].value)
            {
                x = static_cast<double>(value) / kUnits[i].value;
                label = kUnits[i].label;
                break;
            }
        }

        return str(
            (x == floor(x)) ?
            (format("%1% %2%") % static_cast<uint64_t>(x) % label) :
            (format("%1$0.1f %2%") % x % label)
            );
    }
};

template <>
struct Stringify<FunctionName>
{
    static string impl(const FunctionName& value)
    {
        string retval = value;

        int status = -2;
        char* demangled =  abi::__cxa_demangle(
            retval.c_str(), NULL, NULL, &status
            );

        if (demangled != NULL)
        {
            if (status == 0)
            {
                retval = string(demangled);
            }
            free(demangled);
        }

        return retval;
    }
};

template <>
struct Stringify<CBTF_Protocol_FileName>
{
    static string impl(const CBTF_Protocol_FileName& value)
    {
        return str(format("%1% (%2%)") % 
                   value.path % stringify<uint64_t>(value.checksum));
    }
};

template <>
struct Stringify<CUDA_CachePreference>
{
    static string impl(const CUDA_CachePreference& value)
    {
        switch (value)
        {
        case InvalidCachePreference: return "InvalidCachePreference";
        case NoPreference: return "NoPreference";
        case PreferShared: return "PreferShared";
        case PreferCache: return "PreferCache";
        case PreferEqual: return "PreferEqual";
        }
        return "?";
    }
};

template <>
struct Stringify<CUDA_CopyKind>
{
    static string impl(const CUDA_CopyKind& value)
    {
        switch (value)
        {
        case InvalidCopyKind: return "InvalidCopyKind";
        case UnknownCopyKind: return "UnknownCopyKind";
        case HostToDevice: return "HostToDevice";
        case DeviceToHost: return "DeviceToHost";
        case HostToArray: return "HostToArray";
        case ArrayToHost: return "ArrayToHost";
        case ArrayToArray: return "ArrayToArray";
        case ArrayToDevice: return "ArrayToDevice";
        case DeviceToArray: return "DeviceToArray";
        case DeviceToDevice: return "DeviceToDevice";
        case HostToHost: return "HostToHost";
        }
        return "?";
    }
};

template <>
struct Stringify<CUDA_MemoryKind>
{
    static string impl(const CUDA_MemoryKind& value)
    {
        switch (value)
        {
        case InvalidMemoryKind: return "InvalidMemoryKind";
        case UnknownMemoryKind: return "UnknownMemoryKind";
        case Pageable: return "Pageable";
        case Pinned: return "Pinned";
        case Device: return "Device";
        case Array: return "Array";
        }
        return "?";
    }
};

template <>
struct Stringify<CUDA_MessageTypes>
{
    static string impl(const CUDA_MessageTypes& value)
    {
        switch (value)
        {
        case ContextInfo: return "ContextInfo";
        case CopiedMemory: return "CopiedMemory";
        case DeviceInfo: return "DeviceInfo";
        case EnqueueRequest: return "EnqueueRequest";
        case ExecutedKernel: return "ExecutedKernel";
        case LoadedModule: return "LoadedModule";
        case OverflowSamples: return "OverflowSamples";
        case PeriodicSamples: return "PeriodicSamples";
        case ResolvedFunction: return "ResolvedFunction";
        case SamplingConfig: return "SamplingConfig";
        case SetMemory: return "SetMemory";
        case UnloadedModule: return "UnloadedModule";
        }
        return "?";
    }
};

template <>
struct Stringify<CUDA_RequestTypes>
{
    static string impl(const CUDA_RequestTypes& value)
    {
        switch (value)
        {
        case LaunchKernel: return "LaunchKernel";
        case MemoryCopy: return "MemoryCopy";
        case MemorySet: return "MemorySet";
        }
        return "?";
    }
};

template<>
struct Stringify<CUDA_EventDescription>
{
    static string impl(const CUDA_EventDescription& value)
    {
        return (value.threshold == 0) ? string(value.name) :
            str(format("%1% (threshold=%2%)") % value.name % value.threshold);
    }
};

template <>
struct Stringify<CUDA_ContextInfo>
{
    static string impl(const CUDA_ContextInfo& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("context", stringify(value.context))
            ("device", stringify(value.device))
            ("compute_api", stringify(value.compute_api))
            );
    }
};

template <>
struct Stringify<CUDA_CopiedMemory>
{
    static string impl(const CUDA_CopiedMemory& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("context", stringify(value.context))
            ("stream", stringify(value.stream))
            ("time_begin", stringify(value.time_begin))
            ("time_end", stringify(value.time_end))
            ("size", stringify<ByteCount>(value.size))
            ("kind", stringify(value.kind))
            ("source_kind", stringify(value.source_kind))
            ("destination_kind", stringify(value.destination_kind))
            ("asynchronous", stringify<bool>(value.asynchronous))
            );
    }
};

template <>
struct Stringify<CUDA_DeviceInfo>
{
    static string impl(const CUDA_DeviceInfo& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("device", stringify(value.device))
            ("name", stringify(value.name))
            ("compute_capability",
             stringify<vector<uint32_t> >(
                 list_of
                 (value.compute_capability[0])
                 (value.compute_capability[1])
                 ))
            ("max_grid",
             stringify<vector<uint32_t> >(
                 list_of
                 (value.max_grid[0])
                 (value.max_grid[1])
                 (value.max_grid[2])
                 ))
            ("max_block",
             stringify<vector<uint32_t> >(
                 list_of
                 (value.max_block[0])
                 (value.max_block[1])
                 (value.max_block[2])
                 ))
            ("global_memory_bandwidth", 
             stringify<ByteCount>(
                 1024ULL * value.global_memory_bandwidth
                 ) + "/Second")
            ("global_memory_size",
             stringify<ByteCount>(value.global_memory_size))
            ("constant_memory_size",
             stringify<ByteCount>(value.constant_memory_size))
            ("l2_cache_size", stringify<ByteCount>(value.l2_cache_size))
            ("threads_per_warp", stringify(value.threads_per_warp))
            ("core_clock_rate", 
             stringify<ClockRate>(1024ULL * value.core_clock_rate))
            ("memcpy_engines", stringify(value.memcpy_engines))
            ("multiprocessors", stringify(value.multiprocessors))
            ("max_ipc", stringify(value.max_ipc))
            ("max_warps_per_multiprocessor",
             stringify(value.max_warps_per_multiprocessor))
            ("max_blocks_per_multiprocessor",
             stringify(value.max_blocks_per_multiprocessor))
            ("max_registers_per_block",
             stringify(value.max_registers_per_block))
            ("max_shared_memory_per_block",
             stringify<ByteCount>(value.max_shared_memory_per_block))
            ("max_threads_per_block", stringify(value.max_threads_per_block))
            );
    }
};

template <>
struct Stringify<CUDA_EnqueueRequest>
{
    static string impl(const CUDA_EnqueueRequest& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("type", stringify(value.type))
            ("time", stringify(value.time))
            ("context", stringify(value.context))
            ("stream", stringify(value.stream))
            ("call_site", stringify(value.call_site))
            );
    }
};

template <>
struct Stringify<CUDA_ExecutedKernel>
{
    static string impl(const CUDA_ExecutedKernel& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("context", stringify(value.context))
            ("stream", stringify(value.stream))
            ("time_begin", stringify(value.time_begin))
            ("time_end", stringify(value.time_end))
            ("function", stringify<FunctionName>(value.function))
            ("grid", 
             stringify<vector<int32_t> >(
                 list_of(value.grid[0])(value.grid[1])(value.grid[2])
                 ))
            ("block",
             stringify<vector<int32_t> >(
                 list_of(value.block[0])(value.block[1])(value.block[2])
                 ))
            ("cache_preference", stringify(value.cache_preference))
            ("registers_per_thread", stringify(value.registers_per_thread))
            ("static_shared_memory", 
             stringify<ByteCount>(value.static_shared_memory))
            ("dynamic_shared_memory",
             stringify<ByteCount>(value.dynamic_shared_memory))
            ("local_memory", stringify<ByteCount>(value.local_memory))
            );
    }
};

template <>
struct Stringify<CUDA_LoadedModule>
{
    static string impl(const CUDA_LoadedModule& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("time", stringify(value.time))
            ("module", stringify(value.module))
            ("handle", stringify(value.handle))
            );
    }
};

template <>
struct Stringify<CUDA_OverflowSamples>
{
    static string impl(const CUDA_OverflowSamples& value)
    {
        stringstream stream;

        stream << stringify<Fields>(
            tuple_list_of
            ("time_begin", stringify(value.time_begin))
            ("time_end", stringify(value.time_end))
            );

        stream << endl << "    pcs = ";
        for (u_int i = 0; i < value.pcs.pcs_len; ++i)
        {
            if ((i % 4) == 0)
            {
                stream << endl << (format("[%1$4d] ") % i);
            }
            
            stream << stringify(value.pcs.pcs_val[i]) << " ";
        }
        if ((value.pcs.pcs_len % 4) != 0)
        {
            stream << endl;
        }

        stream << endl << "    counts = ";
        for (u_int i = 0; i < value.counts.counts_len; ++i)
        {
            if ((i % 4) == 0)
            {
                stream << endl << (format("[%1$4d] ") % i);
            }
            
            stream << stringify(value.counts.counts_val[i]) << " ";
        }
        if ((value.counts.counts_len % 4) != 0)
        {
            stream << endl;
        }
        
        return string();
    }
};

template <>
struct Stringify<CUDA_PeriodicSamples>
{
    static string impl(const CUDA_PeriodicSamples& value)
    {
        static int N[4] = { 0, 2, 3, 8 };

        stringstream stream;
        stream << "    deltas = " << endl;

        for (u_int i = 0; i < value.deltas.deltas_len;)
        {
            uint8_t* ptr = &value.deltas.deltas_val[i];
            
            uint64_t delta = 0;
            uint8_t encoding = ptr[0] >> 6;
            if (encoding < 3)
            {
                delta = static_cast<uint64_t>(ptr[0]) & 0x3F;
            }
            else
            {
                delta = 0;
            }            
            for (int j = 0; j < N[encoding]; ++j)
            {
                delta <<= 8;
                delta |= static_cast<uint64_t>(ptr[1 + j]);
            }

            stringstream bytes;
            for (int j = 0; j < (1 + N[encoding]); ++j)
            {
                bytes << (format("%02X ") % static_cast<unsigned int>(ptr[j]));
            }

            stream << (format("    [%4d] %-27s (%s)") % i % bytes.str() %
                       stringify(delta)) << endl;
            
            i += 1 + N[encoding];
        }

        return stream.str();
    }
};

template <>
struct Stringify<CUDA_ResolvedFunction>
{
    static string impl(const CUDA_ResolvedFunction& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("time", stringify(value.time))
            ("module_handle", stringify(value.module_handle))
            ("function", stringify<FunctionName>(value.function))
            ("handle", stringify(value.handle))
            );
    }
};

template <>
struct Stringify<CUDA_SamplingConfig>
{
    static string impl(const CUDA_SamplingConfig& value)
    {
        Fields fields = tuple_list_of("interval", stringify(value.interval));
        
        for (u_int i = 0; i < value.events.events_len; ++i)
        {
            fields.push_back(
                tuple<string, string>(
                    str(format("event %1%") % i),
                    stringify(value.events.events_val[i])
                    )
                );
        }

        return stringify(fields);
    }
};

template <>
struct Stringify<CUDA_SetMemory>
{
    static string impl(const CUDA_SetMemory& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("context", stringify(value.context))
            ("stream", stringify(value.stream))
            ("time_begin", stringify(value.time_begin))
            ("time_end", stringify(value.time_end))
            ("size", stringify<ByteCount>(value.size))
            );
    }
};

template <>
struct Stringify<CUDA_UnloadedModule>
{
    static string impl(const CUDA_UnloadedModule& value)
    {
        return stringify<Fields>(
            tuple_list_of
            ("time", stringify(value.time))
            ("handle", stringify(value.handle))
            );
    }
};

template <>
struct Stringify<CBTF_cuda_message>
{
    static string impl(const CBTF_cuda_message& value)
    {
        switch (value.type)
        {
        case ContextInfo:
            return stringify(value.CBTF_cuda_message_u.context_info);
            
        case CopiedMemory:
            return stringify(value.CBTF_cuda_message_u.copied_memory);
            
        case DeviceInfo:
            return stringify(value.CBTF_cuda_message_u.device_info);
            
        case EnqueueRequest:
            return stringify(value.CBTF_cuda_message_u.enqueue_request);
            
        case ExecutedKernel:
            return stringify(value.CBTF_cuda_message_u.executed_kernel);
 
        case LoadedModule:
            return stringify(value.CBTF_cuda_message_u.loaded_module);

        case OverflowSamples:
            return stringify(value.CBTF_cuda_message_u.overflow_samples);

        case PeriodicSamples:
            return stringify(value.CBTF_cuda_message_u.periodic_samples);

        case ResolvedFunction:
            return stringify(value.CBTF_cuda_message_u.resolved_function);

        case SamplingConfig:
            return stringify(value.CBTF_cuda_message_u.sampling_config);
            
        case SetMemory:
            return stringify(value.CBTF_cuda_message_u.set_memory);
            
        case UnloadedModule:
            return stringify(value.CBTF_cuda_message_u.unloaded_module);
        }
        
        return string();
    }
};

template <>
struct Stringify<CBTF_cuda_data>
{
    static string impl(const CBTF_cuda_data& value)
    {
        stringstream stream;

        for (u_int i = 0; i < value.messages.messages_len; ++i)
        {
            const CBTF_cuda_message& msg = value.messages.messages_val[i];
            
            stream << endl
                   << (format("[%1$3d] %2%") % i % 
                       stringify(static_cast<CUDA_MessageTypes>(msg.type))) 
                   << endl << endl << stringify(msg);
        }

        stream << endl << "stack_traces = ";
        for (u_int i = 0, n = 0;
             i < value.stack_traces.stack_traces_len;
             ++i, ++n)
        {
            if (((n % 4) == 0) ||
                ((i > 0) && (value.stack_traces.stack_traces_val[i - 1] == 0)))
            {
                stream << endl << (format("[%1$4d] ") % i);
                n = 0;
            }
            
            stream << stringify(value.stack_traces.stack_traces_val[i]) << " ";
        }
        stream << endl;
        
        return stream.str();
    }
};



/**
 * Parse the command-line arguments and dump the requested CUDA performance
 * data blobs.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Command-line arguments.
 * @return        Exit code. Either 1 if a failure occurred, or 0 otherwise.
 */
int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    stringstream stream;
    stream << endl
           << "This tool scans the specified Open|SpeedShop experiment "
           << "database for CUDA " << endl
           << "performance data blobs and dumps their contents to the "
           << "standard output stream." << endl
           << endl;
    string kExtraHelp = stream.str();

    // Parse and validate the command-line arguments
    
    options_description kNonPositionalOptions("ossdumpcuda options");
    kNonPositionalOptions.add_options()
 
        ("blob", value< vector<int> >(),
         "Restrict the dump to the CUDA performance data blob with this "
         "id. Multiple blobs may be specified.")
        
        ("database", value<string>(),
         "Open|SpeedShop experiment database to be dumped. May also be "
         "specified as a positional argument.")
        
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

    set<int> blobs;
    if (values.count("blob") > 0)
    {
        vector<int> temp = values["blob"].as< vector<int> >();
        blobs = set<int>(temp.begin(), temp.end());
    }

    // Iterate over the CUDA performance data blobs in the specified database

    SmartPtr<Database> database(new Database(values["database"].as<string>()));
    
    BEGIN_TRANSACTION(database);

    database->prepareStatement(
        "SELECT Data.id, "
        "       Data.collector, "
        "       Data.thread, "
        "       Data.time_begin, "
        "       Data.time_end, "
        "       Data.addr_begin, "
        "       Data.addr_end, "
        "       Data.data "
        "FROM Data "
        "  JOIN Collectors "
        "ON Data.collector = Collectors.id "
        "WHERE Collectors.unique_id = \"cuda\";"
        );

    while (database->executeStatement())
    {
        int id = database->getResultAsInteger(1);

        if (!blobs.empty() && (blobs.find(id) == blobs.end()))
        {
            continue;
        }

        int collector = database->getResultAsInteger(2);
        int thread = database->getResultAsInteger(3);

        TimeInterval interval(database->getResultAsTime(4),
                              database->getResultAsTime(5));
        
        AddressRange range(database->getResultAsAddress(6),
                           database->getResultAsAddress(7));

        Blob blob = database->getResultAsBlob(8);

        CBTF_cuda_data data;
        memset(&data, 0, sizeof(data));
        blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &data
            );

        cout << endl << endl
             << colorize(
                 str(format("CUDA Performance Data Blob ID %1% from %2%") %
                     id % database->getName()),
                 None, Green, Bold) << endl
             << colorize(str(format("Collector ID : %1%") % collector),
                         None, Green, Bold) << endl
             << colorize(str(format("Thread ID    : %1%") % thread),
                         None, Green, Bold) << endl
             << colorize(str(format("Time Interval: %1%") % interval),
                         None, Green, Bold) << endl
             << colorize(str(format("Address Range: %1%") % range),
                         None, Green, Bold) << endl
             << stringify(data);
        
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
                 reinterpret_cast<char*>(&data));        
    }

    END_TRANSACTION(database);

    cout << endl << endl;
    return 0;
}
