////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2017 Argo Navis Technologies. All Rights Reserved.
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

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <ArgoNavis/Base/PeriodicSamplesGroup.hpp>
#include <ArgoNavis/Base/PeriodicSamples.hpp>

#include <ArgoNavis/CUDA/stringify.hpp>

#include "SS_Input_Manager.hxx"
#include "SS_View_Expr.hxx"

#include "CUDACollector.hxx"
#include "CUDACountsDetail.hxx"
#include "CUDAQueries.hxx"

#define WARN(x) Mark_Cmd_With_Soft_Error(command, x);

using namespace OpenSpeedShop::Framework;

/** Typed used to hold a flattened PeriodicSamplesGroup. */
typedef std::map<Time, std::vector<boost::uint64_t> > FlattenedData;



static const std::string kOptions[] = {
    "HWPC", // This is the option that selects this particular sub-view
    "Summary", "SummaryOnly",
    ""
};



/** Cache all of the CUDA performance data for the specified threads. */
static void cache(const Collector& collector, const ThreadGroup& threads)
{
    ExtentGroup subextents;
    subextents.push_back(Extent(TimeInterval(Time::TheBeginning(),
                                             Time::TheEnd()),
                                AddressRange(Address::TheLowest(),
                                             Address::TheHighest())));
    
    std::vector<std::vector<std::string> > data(subextents.size());
    
    for (ThreadGroup::const_iterator
             i = threads.begin(), i_end = threads.end(); i != i_end; ++i)
    {
        collector.getMetricValues<std::vector<std::string> >(
            "count_counters", *i, subextents, data
            );
    }
}



/** Visitor used to flatten a PeriodicSamplesGroup. */
bool flatten(const ArgoNavis::Base::Time& time,
             const std::vector<boost::uint64_t>& values,
             const std::size_t& n, const std::size_t& N,
             FlattenedData& data)
{
    FlattenedData::iterator i = data.find(Queries::ConvertFromArgoNavis(time));
    
    if (i == data.end())
    {
        i = data.insert(std::make_pair(
                            Queries::ConvertFromArgoNavis(time),
                            std::vector<boost::uint64_t>(N, 0)
                            )).first;            
    }
    
    i->second[n] = values[0];
    
    return true;
}



/** Reset the CUDA collector getMetric() state. */
static void reset(const Collector& collector, const ThreadGroup& threads)
{
    ExtentGroup subextents;
    subextents.push_back(Extent(TimeInterval(Time::TheBeginning(),
                                             Time::TheEnd()),
                                AddressRange(Address::TheLowest(),
                                             Address::TheHighest() + -1)));

    std::vector<std::vector<std::string> > data(subextents.size());

    collector.getMetricValues<std::vector<std::string> >(
        "count_counters", *threads.begin(), subextents, data
        );
}



/** Get the CUDA collector. */
static boost::optional<Collector> get_collector(const Experiment& experiment)
{
    CollectorGroup collectors = experiment.getCollectors();

    for (CollectorGroup::const_iterator
             i = collectors.begin(), i_end = collectors.end(); i != i_end; ++i)
    {
        if (i->getMetadata().getUniqueId() == "cuda")
        {
            return *i;
        }
    }
    
    return boost::none;
}



/** Flatten the specified group of periodic samples. */
static FlattenedData get_flattened(
    const ArgoNavis::Base::PeriodicSamplesGroup& group
    )
{
    FlattenedData flattened;
    
    for (std::size_t n = 0, N = group.size(); n < N; ++n)
    {
        group[n].visit(
            group[n].interval(),
            boost::bind(flatten, _1, _2,
                        boost::cref(n), boost::cref(N), boost::ref(flattened))
            );
    }
    
    return flattened;
}



/**
 * Get the samples for the specified threads over an optional time interval,
 * and then resample them at the given optional fixed sampling rate.
 */
static ArgoNavis::Base::PeriodicSamplesGroup get_samples(
    const Collector& collector, const ThreadGroup& threads,
    const boost::optional<TimeInterval>& interval_,
    const boost::optional<Time>& rate_
    )
{
    using ArgoNavis::Base::PeriodicSamplesGroup;
    
    reset(collector, threads);

    PeriodicSamplesGroup group;
    
    ExtentGroup subextents;
    subextents.push_back(Extent(
        interval_ ?
            *interval_ : TimeInterval(Time::TheBeginning(), Time::TheEnd()),
        AddressRange(Address::TheLowest(), Address::TheHighest())
        ));
    
    for (ThreadGroup::const_iterator
             i = threads.begin(), i_end = threads.end(); i != i_end; ++i)
    {
        std::vector<PeriodicSamplesGroup> data(subextents.size());
        
        collector.getMetricValues<PeriodicSamplesGroup>(
            "periodic_samples", *i, subextents, data
            );
        
        group.insert(group.end(), data[0].begin(), data[0].end());
    }

    boost::optional<ArgoNavis::Base::TimeInterval> interval = boost::none;

    if (interval_)
    {
        interval = Queries::ConvertToArgoNavis(*interval_);
    }
    
    boost::optional<ArgoNavis::Base::Time> rate = boost::none;

    if (rate_)
    {
        rate = ArgoNavis::Base::Time(rate_->getValue());
    }
    
    return ArgoNavis::Base::getResampledAndCombined(group, interval, rate);    
}



bool generate_cuda_hwpc_view(CommandObject* command,
                             ExperimentObject* experiment,
                             boost::int64_t top_n,
                             ThreadGroup& threads,
                             std::list<CommandResult*>& view)
{
    const size_t kHistogramBins = 20;
    
    // Proceed no further if invalid options were specified
    if (!Validate_V_Options(command, const_cast<std::string*>(kOptions)))
    {
        return false;
    }

    // Proceed no further if the CUDA collector cannot be found
    boost::optional<Collector> collector = get_collector(*experiment->FW());
    if (!collector)
    {
        WARN("(There are no metrics specified to report.)");
        return false;
    }

    // Parse some of the additional options

    bool show_summary_only = Look_For_KeyWord(command, "SummaryOnly");

    bool show_summary =  show_summary_only |
        Look_For_KeyWord(command, "Summary");
    
    // Cache all of the CUDA performance data for the specified threads
    cache(*collector, threads);

    // Get the counts for the specified threads and resample them using the
    // requested fixed sampling interval. Then flatten the data into arrays.

    ArgoNavis::Base::PeriodicSamplesGroup samples = (top_n == 0) ?
        get_samples(*collector, threads, boost::none, boost::none) :
        get_samples(
            *collector, threads, boost::none, Time(top_n * 1000000 /* ms/ns */)
            );
    
    FlattenedData data = get_flattened(samples);

    const std::size_t kNumCounters = samples.size();
    const std::size_t kNumSamples = data.size();
        
    std::vector<boost::uint64_t> data_times(kNumSamples, 0);

    std::vector<std::vector<boost::uint64_t> > data_values(
        kNumSamples, std::vector<boost::uint64_t>(kNumCounters, 0)
        );

    std::size_t s = 0;
    for (FlattenedData::const_iterator
             i = data.begin(), i_end = data.end(); i != i_end; ++i, ++s)
    {
        data_times[s] = i->first.getValue();

        for (std::size_t c = 0; c < kNumCounters; ++c)
        {
            data_values[s][c] = i->second[c];
        }
    }

    for (std::size_t c = 0; c < kNumCounters; ++c)
    {
        if (samples[c].kind() == ArgoNavis::Base::PeriodicSamples::kCount)
        {
            for (std::size_t s = kNumSamples - 1; s > 0; --s)
            {
                data_values[s][c] -= data_values[s - 1][c];
            }

            data_values[0][c] = 0;
        }
    }
    
    // Generate the CPU/GPU balance histogram statistics
    
    bool had_cpu = false;
    bool had_gpu = false;

    std::vector<boost::uint64_t> data_cpu(kNumSamples, 0);
    std::vector<boost::uint64_t> data_gpu(kNumSamples, 0);

    for (std::size_t c = 0; c < kNumCounters; ++c)
    {
        std::string name = ArgoNavis::CUDA::stringify<>(
            ArgoNavis::CUDA::CounterName(samples[c].name())
            );
            
        if (name.find("CPU") != std::string::npos)
        {
            had_cpu = true;

            for (std::size_t s = 0; s < kNumSamples; ++s)
            {
                data_cpu[s] += data_values[s][c];
            }
        }
        else if (name.find("GPU") != std::string::npos)
        {
            had_gpu = true;            

            for (std::size_t s = 0; s < kNumSamples; ++s)
            {
                data_gpu[s] += data_values[s][c];
            }
        }
    }
    
    boost::uint64_t scale_cpu =
        *max_element(data_cpu.begin(), data_cpu.end()) / kHistogramBins;

    boost::uint64_t scale_gpu = 
        *max_element(data_gpu.begin(), data_gpu.end()) / kHistogramBins;

    bool show_balance = had_cpu || had_gpu;

    // Generate the headers for the view

    CommandResult_Headers* headers = new CommandResult_Headers();

    headers->Add_Header(new CommandResult_String("Time (ms)"));

    for (std::size_t c = 0; c < kNumCounters; ++c)
    {
        std::string name = ArgoNavis::CUDA::stringify<>(
            ArgoNavis::CUDA::CounterName(samples[c].name())
            );
        
        headers->Add_Header(new CommandResult_String(name));
    }

    if (!show_summary_only && show_balance)
    {
        assert(kHistogramBins >= 7);
        std::string balance;

        balance += "|<";

        for (std::size_t i = 0; i < (kHistogramBins - 7); ++i)
        {
            balance += "-";
        }

        balance += "CPU---|---GPU";

        for (std::size_t i = 0; i < (kHistogramBins - 7); ++i)
        {
            balance += "-";
        }
        
        balance += ">|";

        headers->Add_Header(new CommandResult_String(balance));
    }

    view.push_back(headers);

    // Generate the data columns for the view
    if (!show_summary_only)
    {
        for (std::size_t s = 0; s < kNumSamples; ++s)
        {
            CommandResult_Columns* columns = new CommandResult_Columns();

            boost::uint64_t t =
                (data_times[s] - data_times[0]) / 1000000 /* ms/ns */;
            
            columns->Add_Column(new CommandResult_Uint(t));
            
            for (std::size_t c = 0; c < kNumCounters; ++c)
            {
                columns->Add_Column(new CommandResult_Uint(data_values[s][c]));
            }
            
            if (show_balance)
            {
                std::string balance;
                
                size_t ncpu = (scale_cpu == 0) ? 0 : (data_cpu[s] / scale_cpu);
                size_t ngpu = (scale_gpu == 0) ? 0 : (data_gpu[s] / scale_gpu);
                
                balance += "|";
                
                for (std::size_t i = 0; i < kHistogramBins; ++i)
                {
                    balance += (ncpu > (kHistogramBins - i - 1)) ? "*" : " ";
                }
                
                balance += "|";

                for (std::size_t i = 0; i < kHistogramBins; ++i)
                {
                    balance += (ngpu > i) ? "*" : " ";
                }
                
                balance += "|";
                
                columns->Add_Column(new CommandResult_String(balance));
            }
            
            view.push_back(columns);
        }
    }

    // Generate the enders for the view
    if (show_summary)
    {
        boost::uint64_t t =
            (data_times[kNumSamples - 1] - data_times[0]) / 1000000 /* ms/ns */;
        
        std::vector<boost::uint64_t> totals(kNumCounters, 0);

        for (std::size_t c = 0; c < kNumCounters; ++c)
        {
            for (std::size_t s = 0; s < kNumSamples; ++s)
            {
                totals[c] += data_values[s][c];
            }
        }
        
        CommandResult_Enders* enders = new CommandResult_Enders();
        
        enders->Add_Ender(new CommandResult_Uint(t));
        
        for (std::size_t c = 0; c < kNumCounters; ++c)
        {
            enders->Add_Ender(new CommandResult_Uint(totals[c]));
        }
        
        if (show_balance)
        {
            enders->Add_Ender(new CommandResult_String("Report Summary"));
        }
        
        view.push_back(enders);
    }
    
    // Done!
    return true;
}
