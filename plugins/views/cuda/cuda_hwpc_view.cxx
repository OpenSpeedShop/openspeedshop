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
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <cassert>
#include <cmath>
#include <set>
#include <string>
#include <utility>

#include <ArgoNavis/CUDA/stringify.hpp>

#include "SS_Input_Manager.hxx"
#include "SS_View_Expr.hxx"

#include "CUDACollector.hxx"
#include "CUDACountsDetail.hxx"
#include "CUDAQueries.hxx"

//#define DEBUG_RESAMPLING

#define WARN(x) Mark_Cmd_With_Soft_Error(command, x);

using namespace OpenSpeedShop::Framework;



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
             i = threads.begin(); i != threads.end(); ++i)
    {
        collector.getMetricValues<std::vector<std::string> >(
            "count_counters", *i, subextents, data
            );
    }
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



/** Compute the average sampling interval of the specified counts. */
boost::uint64_t compute_average_sampling_interval(
    const std::vector<std::set<CUDACountsDetail> >& counts
    )
{
    boost::uint64_t sum = 0, n = 0;
    
    for (std::vector<std::set<CUDACountsDetail> >::const_iterator
             i = counts.begin(); i != counts.end(); ++i)
    {
        Time previous;
        for (std::set<CUDACountsDetail>::const_iterator
                 j = i->begin(); j != i->end(); ++j)
        {
            Time current = Queries::ConvertFromArgoNavis(j->getTime());

            if (j != i->begin())
            {
                sum += current.getValue() - previous.getValue();
                n++;
            }

            previous = current;
        }
    }

    return static_cast<boost::uint64_t>(
        round(static_cast<double>(sum) / static_cast<double>(n))
        );
}



/** Compute the smallest time interval enclosing all of the specified counts. */
TimeInterval compute_smallest_time_interval(
    const std::vector<std::set<CUDACountsDetail> >& counts
    )
{
    TimeInterval interval;

    for (std::vector<std::set<CUDACountsDetail> >::const_iterator
             i = counts.begin(); i != counts.end(); ++i)
    {
        if (!i->empty())
        {
            interval |= TimeInterval(
                Queries::ConvertFromArgoNavis(i->begin()->getTime())
                );

            interval |= TimeInterval(
                Queries::ConvertFromArgoNavis(i->rbegin()->getTime())
                );
        }
    }

    return interval;
}



/** Get the CUDA collector. */
static boost::optional<Collector> get_collector(const Experiment& experiment)
{
    CollectorGroup collectors = experiment.getCollectors();

    for (CollectorGroup::const_iterator
             i = collectors.begin(); i != collectors.end(); ++i)
    {
        if (i->getMetadata().getUniqueId() == "cuda")
        {
            return *i;
        }
    }

    return boost::none;
}



/** Get the counts for the specified threads over an optional time interval. */
static std::vector<std::set<CUDACountsDetail> > get_counts(
    const Collector& collector, const ThreadGroup& threads,
    const TimeInterval& interval = TimeInterval(Time::TheBeginning(),
                                                Time::TheEnd())
    )
{
    reset(collector, threads);

    ExtentGroup subextents;
    subextents.push_back(Extent(interval, AddressRange(Address::TheLowest(),
                                                       Address::TheHighest())));
    
    std::vector<std::set<CUDACountsDetail> > counts;
    
    for (ThreadGroup::const_iterator
             i = threads.begin(); i != threads.end(); ++i)
    {
        std::vector<std::vector<CUDACountsDetail> > data(subextents.size());

        collector.getMetricValues<std::vector<CUDACountsDetail> >(
            "count_exclusive_details", *i, subextents, data
            );
        
        counts.push_back(std::set<CUDACountsDetail>());
        std::set<CUDACountsDetail>& thread_counts = counts[counts.size() - 1];

        for (std::vector<CUDACountsDetail>::const_iterator
                 j = data[0].begin(); j != data[0].end(); ++j)
        {
            thread_counts.insert(*j);
        }
    }
    
    return counts;
}



/** Get the names of the sampled counters. */
static std::vector<std::string> get_counters(const Collector& collector,
                                             const ThreadGroup& threads)
{
    reset(collector, threads);

    ExtentGroup subextents;
    subextents.push_back(Extent(TimeInterval(Time::TheBeginning(),
                                             Time::TheEnd()),
                                AddressRange(Address::TheLowest(),
                                             Address::TheHighest())));

    std::vector<std::vector<std::string> > data(subextents.size());

    collector.getMetricValues<std::vector<std::string> >(
        "count_counters", *threads.begin(), subextents, data
        );
    
    std::vector<std::string> counters;

    for (std::vector<std::string>::const_iterator
             i = data[0].begin(); i != data[0].end(); ++i)
    {
        using namespace ArgoNavis::CUDA;

        counters.push_back(stringify<>(CounterName(*i)));
    }
    
    return counters;
}



/**
 * Resample the specified (original) counts to the given fixed sampling
 * interval. If no sampling interval is provided, the average sampling
 * interval (to the neareset ms) is used.
 */
std::pair<boost::uint64_t, std::vector<std::vector<boost::uint64_t> > >
resample(const std::vector<std::set<CUDACountsDetail> >& counts,
         boost::uint64_t interval = 0)
{
    //
    // Use the average sampling interval (to the nearest ms) as the resampling
    // interval if no explicit resampling interval was provided.
    //
    
    boost::uint64_t tinterval = (interval != 0) ? interval :
        (1000000 /* ms/ns */ * static_cast<boost::uint64_t>(
            round(
                static_cast<double>(
                    compute_average_sampling_interval(counts)
                    ) / 1000000.0 /* ms/ns */
                )
            ));
    
    // Determine the number of (re)samples
    
    TimeInterval smallest = compute_smallest_time_interval(counts);
    
    boost::uint64_t tbegin = tinterval *
        (smallest.getBegin().getValue() / tinterval);

    boost::uint64_t tend = tinterval *
        (1 + (smallest.getEnd().getValue() / tinterval));
    
    boost::uint64_t nsamples = (tend - tbegin) / tinterval;
    
    // Determine the number of sampled counters
    
    std::size_t ncounters = 0;
    
    for (std::vector<std::set<CUDACountsDetail> >::const_iterator
             i = counts.begin(); i != counts.end(); ++i)
    {
        if (!i->empty())
        {
            ncounters = i->begin()->getCounts().size();
            break;
        }
    }

#if defined(DEBUG_RESAMPLING)
    cout << endl;
    cout << "DEBUG: ncounters=" << ncounters << endl;
    cout << "DEBUG: tinterval=" << tinterval << endl;
    cout << "DEBUG:    tbegin=" << tbegin << endl;
    cout << "DEBUG:     begin=" << smallest.getBegin().getValue() << endl;
    cout << "DEBUG:       end=" << smallest.getEnd().getValue() << endl;
    cout << "DEBUG:      tend=" << tend << endl;
    cout << "DEBUG:  nsamples=" << nsamples << endl;
#endif
    
    // Allocate vectors to hold the resampled counts    
    std::vector<std::vector<boost::uint64_t> > resampled(
        nsamples, std::vector<boost::uint64_t>(ncounters, 0)
        );

    // Iterate over each thread's set of original samples
    for (std::vector<std::set<CUDACountsDetail> >::const_iterator
             i = counts.begin(); i != counts.end(); ++i)
    {
        // Initialize structures for tracking the previous original sample
        boost::uint64_t tprevious = tbegin;
        std::vector<boost::uint64_t> cprevious(ncounters, 0);
        
#if defined(DEBUG_RESAMPLING)
        int DebugCount = 10;
#endif

        // Iterate over each of this thread's original samples
        for (std::set<CUDACountsDetail>::const_iterator
                 j = i->begin(); j != i->end(); ++j)
        {
#if defined(DEBUG_RESAMPLING)
            bool DoDebug = 
                (j == i->begin()) || (j == --i->end()) || (DebugCount-- > 0);
#endif

            boost::uint64_t t = Queries::ConvertFromArgoNavis(
                j->getTime()
                ).getValue();

            // Compute the time range covered by the original sample
            boost::uint64_t tb_orig = tprevious;
            boost::uint64_t te_orig = t;
            
            // Compute the range of new samples covering this original sample
            boost::uint64_t kbegin = (tprevious - tbegin) / tinterval;
            boost::uint64_t   kend = 1 + ((t - tbegin) / tinterval);

            // Compute the deltas (time and counters) for the original sample
            boost::uint64_t dt_orig = te_orig - tb_orig;
            std::vector<boost::uint64_t> dc_orig(ncounters, 0);
            for (size_t c = 0; c < ncounters; ++c)
            {
                dc_orig[c] = j->getCounts()[c] - cprevious[c];
            }

#if defined(DEBUG_RESAMPLING)
            if (DoDebug)
            {
                cout << endl;
                if (j == i->begin())
                {
                    cout << "DEBUG: (First)" << endl;
                }
                else if (j == --i->end())
                {
                    cout << "DEBUG: (Last)" << endl;
                }
                cout << "DEBUG: tb_orig=" << tb_orig << endl;
                cout << "DEBUG: te_orig=" << te_orig << endl;
                cout << "DEBUG: dt_orig=" << dt_orig;
                for (size_t c = 0; c < ncounters; ++c)
                {
                    cout << ", dc_orig[" << c << "]=" << dc_orig[c];
                }
                cout << endl;
                cout << "DEBUG: kbegin=" << kbegin << endl;
                cout << "DEBUG:   kend=" << kend << endl;
            }
#endif
            
            // Iterate over each new sample covering this original sample
            for (size_t k = kbegin; k < kend; ++k)
            {
                // Compute the time range covered by this new sample
                boost::uint64_t tb_new = tbegin + (k * tinterval);
                boost::uint64_t te_new = tbegin + ((k + 1) * tinterval);

                // Compute the intersection of the new and original samples
                boost::uint64_t tb_inter = std::max<>(tb_orig, tb_new);
                boost::uint64_t te_inter = std::min<>(te_orig, te_new);
                
#if defined(DEBUG_RESAMPLING)
                if (DoDebug)
                {
                    cout << "DEBUG: k=" << k << endl;
                    cout << "DEBUG:       tb_new=" << tb_new << endl;
                    cout << "DEBUG:       te_new=" << te_new << endl;
                    cout << "DEBUG:     tb_inter=" << tb_inter << endl;
                    cout << "DEBUG:     te_inter=" << te_inter << endl;
                }
#endif

                if (tb_inter > te_inter)
                {
                    continue;
                }

                boost::uint64_t dt_inter = te_inter - tb_inter;
                
#if defined(DEBUG_RESAMPLING)
                if (DoDebug)
                {
                    cout << "DEBUG:     dt_inter=" << dt_inter;
                }
#endif
                
                for (size_t c = 0; c < ncounters; ++c)
                {
                    boost::uint64_t dc_inter = (dt_orig == 0) ? 0 :
                        static_cast<boost::uint64_t>(
                            round(static_cast<double>(dc_orig[c]) * 
                                  static_cast<double>(dt_inter) /
                                  static_cast<double>(dt_orig))
                            );
                    
#if defined(DEBUG_RESAMPLING)
                    if (DoDebug)
                    {
                        cout << ", dc_inter[" << c << "]=" << dc_inter;
                    }
#endif

                    resampled[k][c] += dc_inter;
                }
                
#if defined(DEBUG_RESAMPLING)
                if (DoDebug)
                {
                    cout << endl;
                }
#endif
            } // k

            // Make this original sample the previous original sample
            tprevious = t;
            cprevious = j->getCounts();
        }
    }

    return make_pair(tinterval, resampled);
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

    // Cache all of the CUDA performance data for the specified threads
    cache(*collector, threads);
    
    // Proceed no further if no counters were sampled
    std::vector<std::string> counters = get_counters(*collector, threads);
    if (counters.empty())
    {
        WARN("(There are no metrics specified to report.)");
        return false;
    }

    // Parse some of the additional options

    bool show_summary_only = Look_For_KeyWord(command, "SummaryOnly");

    bool show_summary =  show_summary_only |
        Look_For_KeyWord(command, "Summary");

    // Get the counts for the specified threads
    std::vector<std::set<CUDACountsDetail> > counts = get_counts(
        *collector, threads
        );

    // Resample these counts using the requested fixed sampling interval
    std::pair<boost::uint64_t, std::vector<std::vector<boost::uint64_t> > >
        resampled = resample(
            counts, static_cast<boost::uint64_t>(top_n) * 1000000 /* ms/ns */
            );
    
    // Generate the CPU/GPU balance histogram statistics
    
    bool had_cpu = false;
    bool had_gpu = false;
    
    std::vector<boost::uint64_t> data_cpu(resampled.second.size(), 0);
    std::vector<boost::uint64_t> data_gpu(resampled.second.size(), 0);

    for (size_t i = 0; i < counters.size(); ++i)
    {
        if (counters[i].find("CPU") != std::string::npos)
        {
            had_cpu = true;
            for (size_t j = 0; j < resampled.second.size(); ++j)
            {
                data_cpu[j] += resampled.second[j][i];                
            }
        }
        else if (counters[i].find("GPU") != std::string::npos)
        {
            had_gpu = true;            
            for (size_t j = 0; j < resampled.second.size(); ++j)
            {
                data_gpu[j] += resampled.second[j][i];                
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

    for (size_t i = 0; i < counters.size(); ++i)
    {
        headers->Add_Header(new CommandResult_String(counters[i]));
    }

    if (!show_summary_only && show_balance)
    {
        assert(kHistogramBins >= 7);
        std::string balance;

        balance += "|<";

        for (size_t i = 0; i < (kHistogramBins - 7); ++i)
        {
            balance += "-";
        }

        balance += "CPU---|---GPU";

        for (size_t i = 0; i < (kHistogramBins - 7); ++i)
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
        for (size_t i = 0; i < resampled.second.size(); ++i)
        {
            CommandResult_Columns* columns = new CommandResult_Columns();
            
            boost::uint64_t t = (i * resampled.first) / 1000000 /* ms/ns */;
            
            columns->Add_Column(new CommandResult_Uint(t));
            
            for (size_t j = 0; j < resampled.second[i].size(); ++j)
            {
                columns->Add_Column(
                    new CommandResult_Uint(resampled.second[i][j])
                    );
            }
            
            if (show_balance)
            {
                std::string balance;
                
                size_t ncpu = (scale_cpu == 0) ? 0 : (data_cpu[i] / scale_cpu);
                size_t ngpu = (scale_gpu == 0) ? 9 : (data_gpu[i] / scale_gpu);
                
                balance += "|";
                
                for (size_t i = 0; i < kHistogramBins; ++i)
                {
                    balance += (ncpu > (kHistogramBins - i - 1)) ? "*" : " ";
                }
                
                balance += "|";

                for (size_t i = 0; i < kHistogramBins; ++i)
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
            (resampled.second.size() * resampled.first) / 1000000 /* ms/ns */;
        
        std::vector<boost::uint64_t> totals(counters.size(), 0);
        
        for (size_t i = 0; i < resampled.second.size(); ++i)
        {
            for (size_t j = 0; j < resampled.second[i].size(); ++j)
            {
                totals[j] += resampled.second[i][j];
            }
        }
        
        CommandResult_Enders* enders = new CommandResult_Enders();
        
        enders->Add_Ender(new CommandResult_Uint(t));
        
        for (size_t i = 0; i < counters.size(); ++i)
        {
            enders->Add_Ender(new CommandResult_Uint(totals[i]));
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
