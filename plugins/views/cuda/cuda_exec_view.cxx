////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Krell Institute. All Rights Reserved.
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

#include <boost/format.hpp>

#include "SS_Input_Manager.hxx"
#include "SS_View_Expr.hxx"

#include "CUDACollector.hxx"
#include "CUDAExecDetail.hxx"
#include "CUDAQueries.hxx"

#define PUSH_HV(x) HV.push_back(x)
#define PUSH_IV(...) IV.push_back(new ViewInstruction(__VA_ARGS__))

#define WARN(x) Mark_Cmd_With_Soft_Error(cmd, x);



static const std::string kOptions[] = {
    "Exec", // This is the option that selects this particular sub-view
    "ButterFly",
    "CallTree", "CallTrees",
    "DontExpand",
    "Dso", "Dsos",
    "FullStack", "FullStacks",
    "Function", "Functions",
    "LinkedObject", "LinkedObjects",
    "Loop", "Loops",
    "Trace",
    "TraceBack", "TraceBacks",
    "Statement", "Statements",
    "Summary", "SummaryOnly",
    ""
};



// There are 2 reserved locations in the predefined-temporary table.
// Additional items may be defined for individual collectors.

// These are needed to manage CUDA kernel execution data
#define intime_temp VMulti_free_temp
#define incnt_temp VMulti_free_temp+1
#define extime_temp VMulti_free_temp+2
#define excnt_temp VMulti_free_temp+3
#define start_temp VMulti_free_temp+4
#define stop_temp VMulti_free_temp+5
#define min_temp VMulti_free_temp+6
#define max_temp VMulti_free_temp+7
#define ssq_temp VMulti_free_temp+8
#define grid_temp VMulti_free_temp+9
#define block_temp VMulti_free_temp+10
#define cache_temp VMulti_free_temp+11
#define rpt_temp VMulti_free_temp+12
#define ssm_temp VMulti_free_temp+13
#define dsm_temp VMulti_free_temp+14
#define lm_temp VMulti_free_temp+15

#define First_ByThread_Temp VMulti_free_temp+16
#define ByThread_use_intervals 1 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 // otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"

#define Determine_Objects Get_Filtered_Objects

#define def_CUDA_values               \
    Time start = Time::TheEnd();      \
    Time end = Time::TheBeginning();  \
    double intime = 0.0;              \
    int64_t incnt = 0;                \
    double extime = 0.0;              \
    int64_t excnt = 0;                \
    double vmax = 0.0;                \
    double vmin = LONG_MAX;           \
    double sum_squares = 0.0;         \
    std::string detail_grid = "";     \
    std::string detail_block = "";    \
    std::string detail_cache = "";    \
    uint32_t detail_rpt = 0;          \
    uint64_t detail_ssm = 0;          \
    uint64_t detail_dsm = 0;          \
    uint64_t detail_lm = 0;

#define get_CUDA_invalues(primary, num_calls, function_name)                   \
    double v = primary.getTime() / num_calls;                                  \
    intime += v;                                                               \
    incnt++;                                                                   \
    start = std::min(start, Queries::ConvertFromArgoNavis(primary.time_begin));\
    end = std::max(end, Queries::ConvertFromArgoNavis(primary.time_end));      \
    vmin = std::min(vmin, v);                                                  \
    vmax = std::max(vmax, v);                                                  \
    sum_squares += v * v;                                                      \
    detail_grid = boost::str(boost::format("%1%,%2%,%3%") %                    \
        primary.grid.template get<0>() %                                       \
        primary.grid.template get<1>() %                                       \
        primary.grid.template get<2>()                                         \
        );                                                                     \
    detail_block = boost::str(boost::format("%1%,%2%,%3%") %                   \
        primary.block.template get<0>() %                                      \
        primary.block.template get<1>() %                                      \
        primary.block.template get<2>()                                        \
        );                                                                     \
    detail_cache = primary.cache_preference;                                   \
    detail_rpt = primary.registers_per_thread;                                 \
    detail_ssm = primary.static_shared_memory;                                 \
    detail_dsm = primary.dynamic_shared_memory;                                \
    detail_lm = primary.local_memory;

#define get_CUDA_exvalues(secondary, num_calls)  \
    extime += secondary.getTime() / num_calls;   \
    excnt++;

#define get_inclusive_values(stdv, num_calls, function_name)      \
    {                                                             \
        int64_t len = stdv.size();                                \
        for (int64_t i = 0; i < len; i++)                         \
        {                                                         \
            /* Use macro to combine all the values. */            \
            get_CUDA_invalues(stdv[i], num_calls, function_name)  \
        }                                                         \
    }

#define get_exclusive_values(stdv, num_calls)           \
    {                                                   \
        int64_t len = stdv.size();                      \
        for (int64_t i = 0; i < len; i++)               \
        {                                               \
            /* Use macro to combine all the values. */  \
            get_CUDA_exvalues(stdv[i], num_calls)       \
        }                                               \
    }

#define set_CUDA_values(value_array, sort_extime)                         \
    if (num_temps > VMulti_sort_temp)                                     \
    {                                                                     \
        value_array[VMulti_sort_temp] = NULL;                             \
    }                                                                     \
    if (num_temps > start_temp)                                           \
    {                                                                     \
        int64_t x = start.getValue() /* - base_time */;            \
        value_array[start_temp] = new CommandResult_Time(x);              \
    }                                                                     \
    if (num_temps > stop_temp)                                            \
    {                                                                     \
        int64_t x = end.getValue() /* - base_time */;              \
        value_array[stop_temp] = new CommandResult_Time(x);               \
    }                                                                     \
    if (num_temps > VMulti_time_temp)                                     \
    {                                                                     \
        value_array[VMulti_time_temp] =                                   \
            new CommandResult_Interval(sort_extime ? extime : intime);    \
    }                                                                     \
    if (num_temps > intime_temp)                                          \
    {                                                                     \
        value_array[intime_temp] = new CommandResult_Interval(intime);    \
    }                                                                     \
    if (num_temps > incnt_temp)                                           \
    {                                                                     \
        value_array[incnt_temp] = CRPTR(incnt);                           \
    }                                                                     \
    if (num_temps > extime_temp)                                          \
    {                                                                     \
        value_array[extime_temp] = new CommandResult_Interval(extime);    \
    }                                                                     \
    if (num_temps > excnt_temp)                                           \
    {                                                                     \
        value_array[excnt_temp] = CRPTR(excnt);                           \
    }                                                                     \
    if (num_temps > min_temp)                                             \
    {                                                                     \
        value_array[min_temp] = new CommandResult_Interval(vmin);         \
    }                                                                     \
    if (num_temps > max_temp)                                             \
    {                                                                     \
        value_array[max_temp] = new CommandResult_Interval(vmax);         \
    }                                                                     \
    if (num_temps > ssq_temp)                                             \
    {                                                                     \
        value_array[ssq_temp] = new CommandResult_Interval(sum_squares);  \
    }                                                                     \
    if (num_temps > grid_temp)                                            \
    {                                                                     \
        CommandResult* p = CRPTR(detail_grid);                            \
        p->SetValueIsID();                                                \
        value_array[grid_temp] = p;                                       \
    }                                                                     \
    if (num_temps > block_temp)                                           \
    {                                                                     \
        CommandResult* p = CRPTR(detail_block);                           \
        p->SetValueIsID();                                                \
        value_array[block_temp] = p;                                      \
    }                                                                     \
    if (num_temps > cache_temp)                                           \
    {                                                                     \
        CommandResult* p = CRPTR(detail_cache);                           \
        p->SetValueIsID();                                                \
        value_array[cache_temp] = p;                                      \
    }                                                                     \
    if (num_temps > rpt_temp)                                             \
    {                                                                     \
        CommandResult* p = CRPTR(detail_rpt);                             \
        value_array[rpt_temp] = p;                                        \
    }                                                                     \
    if (num_temps > ssm_temp)                                             \
    {                                                                     \
        CommandResult* p = CRPTR(detail_ssm);                             \
        value_array[ssm_temp] = p;                                        \
    }                                                                     \
    if (num_temps > dsm_temp)                                             \
    {                                                                     \
        CommandResult* p = CRPTR(detail_dsm);                             \
        value_array[dsm_temp] = p;                                        \
    }                                                                     \
    if (num_temps > lm_temp)                                              \
    {                                                                     \
        CommandResult* p = CRPTR(detail_lm);                              \
        value_array[lm_temp] = p;                                         \
    }



static bool Determine_Metric_Ordering(std::vector<ViewInstruction*>& IV)
{
    // Determine which metric is the primary.
    int64_t master_temp = 0;
    int64_t search_column = 0;

    while ((search_column == 0) && (search_column < IV.size()))
    {
        ViewInstruction* primary_column = Find_Column_Def(IV, search_column++);
        if (primary_column == NULL)
        {
            break;
        }
        if (primary_column->OpCode() == VIEWINST_Display_Tmp)
        {
            master_temp = primary_column->TMP1();
            break;
        }
    }
    
    if ((master_temp != intime_temp) && (master_temp != extime_temp))
    {
        master_temp = intime_temp;
    }
    
    return master_temp == intime_temp;
}



#define def_Detail_values def_CUDA_values
#define get_inclusive_trace get_CUDA_invalues
#define get_exclusive_trace get_CUDA_exvalues
#define set_Detail_values set_CUDA_values

#include "SS_View_detail.txx"



static bool define_cuda_columns(CommandObject* cmd,
                                ExperimentObject* exp,
                                std::vector<Collector>& CV,
                                std::vector<std::string>& MV,
                                std::vector<ViewInstruction*>& IV,
                                std::vector<std::string>& HV,
                                View_Form_Category vfc)
{
    int64_t last_column = 0;  // # of columns of information displayed
    int64_t totalIndex  = 0;  // # of totals needed to perform % calcs

    // Track maximum temps - needed for expressions
    int64_t last_used_temp = Last_ByThread_Temp;
        
    // Define combination instructions for predefined temporaries
    PUSH_IV(VIEWINST_Add, VMulti_sort_temp);
    PUSH_IV(VIEWINST_Add, VMulti_time_temp);
    PUSH_IV(VIEWINST_Add, intime_temp);
    PUSH_IV(VIEWINST_Add, incnt_temp);
    PUSH_IV(VIEWINST_Add, extime_temp);
    PUSH_IV(VIEWINST_Add, excnt_temp);
    PUSH_IV(VIEWINST_Min, start_temp);
    PUSH_IV(VIEWINST_Max, stop_temp);
    PUSH_IV(VIEWINST_Min, min_temp);
    PUSH_IV(VIEWINST_Max, max_temp);
    PUSH_IV(VIEWINST_Add, ssq_temp);
    PUSH_IV(VIEWINST_Summary_Max, intime_temp);
    
    OpenSpeedShop::cli::ParseResult* p_result = cmd->P_Result();
    std::vector<ParseRange>* p_slist = p_result->getexpMetricList();
    
    bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
    bool Generate_Summary = false;
    bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");

    if (!Generate_Summary_Only)
    {
        Generate_Summary = Look_For_KeyWord(cmd, "Summary");
    }
    
    bool generate_nested_accounting = false;

    int64_t View_ByThread_Identifier = Determine_ByThread_Id(exp, cmd);
    std::string Default_Header = Find_Metadata(CV[0], MV[1]).getShortName();
    std::string ByThread_Header = Default_Header;
    
    if (Generate_Summary_Only)
    {
        if (Generate_ButterFly)
        {
            Generate_Summary_Only = false;
            WARN("Warning: 'summaryonly' is not "
                 "supported with '-v ButterFly'.");
        }
    }
    else if (Generate_Summary)
    {
        if (Generate_ButterFly)
        {
            Generate_Summary = false;
            WARN("Warning: 'summary' is not supported with '-v ButterFly'.");
        }
        else
        {
            PUSH_IV(VIEWINST_Display_Summary);
        }
    }
    
    std::map<std::string, int64_t> MetricMap;

    MetricMap["count"] = excnt_temp;
    MetricMap["counts"] = excnt_temp;
    MetricMap["exclusive_count"] = excnt_temp;
    MetricMap["exclusive_counts"] = excnt_temp;

    MetricMap["inclusive_count"] = incnt_temp;
    MetricMap["inclusive_counts"] = incnt_temp;

    MetricMap["time"] = extime_temp;
    MetricMap["times"] = extime_temp;
    MetricMap["exclusive_time"] = extime_temp;
    MetricMap["exclusive_times"] = extime_temp;

    MetricMap["inclusive_time"] = intime_temp;
    MetricMap["inclusive_times"] = intime_temp;

    MetricMap["min"] = min_temp;
    MetricMap["minimum"] = min_temp;

    MetricMap["max"] = max_temp;
    MetricMap["maximum"] = max_temp;

    if (vfc == VFC_Trace)
    {
        MetricMap["start"] = start_temp;
        MetricMap["start_time"] = start_temp;

        MetricMap["stop"] = stop_temp;
        MetricMap["stop_time"] = stop_temp;

        MetricMap["grid"] = grid_temp;
        MetricMap["block"] = block_temp;
        MetricMap["cache"] = cache_temp;
        MetricMap["rpt"] = rpt_temp;
        MetricMap["ssm"] = ssm_temp;
        MetricMap["dsm"] = dsm_temp;
        MetricMap["lm"] = lm_temp;
    }

    if (p_slist->begin() != p_slist->end())
    {
        // Add modifiers to output list.
        int64_t i = 0;
        bool time_metric_selected = false;

        std::vector<ParseRange>::iterator mi;
        for (mi = p_slist->begin(); mi != p_slist->end(); mi++)
        {
            if ((*mi).getParseType() == PARSE_EXPRESSION_VALUE)
            {
                std::string header = "user expression";
                ParseRange* pr = &(*mi);
                if (pr->getOperation() == EXPRESSION_OP_HEADER)
                {
                    // Replace the default header with the first argument and
                    // replace the original expression with the second argument
                    header = pr->getExpression()->
                        exp_operands[0]->getRange()->start_range.name;
                    pr = pr->getExpression()->exp_operands[1];
                }
                
                // Generate the instructions for the expression.
                int64_t new_result = evaluate_parse_expression(
                    cmd, exp, CV, MV, IV, HV, vfc,
                    pr, last_used_temp, "cuda", MetricMap
                    );
                
                if (new_result < 0)
                {
                    char s[100 + OPENSS_VIEW_FIELD_SIZE];
                    sprintf(s, "Column %lld not be generated because "
                            "of an error in the metric expression.",
                            static_cast<long long int>(last_column));
                    WARN(s);
                    continue;
                }
                
                PUSH_IV(VIEWINST_Display_Tmp, last_column++, new_result);
                PUSH_HV(header);
                continue;
            }

            bool column_is_DateTime = false;
            parse_range_t* m_range = (*mi).getRange();
            std::string C_Name, M_Name;

            if (m_range->is_range)
            {
                C_Name = m_range->start_range.name;
                if (!strcasecmp(M_Name.c_str(), "cuda"))
                {
                    // We only know what to do with the cuda collector.
                    WARN(std::string("The specified collector, ") + C_Name + 
                         ", can not be displayed as part of a 'cuda' view.");
                    continue;
                }
                M_Name = m_range->end_range.name;
            }
            else
            {
                M_Name = m_range->start_range.name;
            }

            // Try to match the name with built in values
            if (M_Name.length() > 0)
            {
                // Select temp values for columns and build column headers
                if (!strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_counts"))
                {
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, excnt_temp);
                    PUSH_HV("Exclusive Count");
                }
                else if (!strcasecmp(M_Name.c_str(), "inclusive_count") ||
                         !strcasecmp(M_Name.c_str(), "inclusive_counts"))
                {
                    generate_nested_accounting = true;
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, incnt_temp);
                    PUSH_HV("Inclusive Count");
                }
                else if (!strcasecmp(M_Name.c_str(), "time") ||
                         !strcasecmp(M_Name.c_str(), "times") ||
                         !strcasecmp(M_Name.c_str(), "exclusive_time") ||
                         !strcasecmp(M_Name.c_str(), "exclusive_times"))
                {
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, extime_temp);
                    PUSH_HV("Exclusive Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "inclusive_time") ||
                         !strcasecmp(M_Name.c_str(), "inclusive_times"))
                {
                    generate_nested_accounting = true;
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, intime_temp);
                    PUSH_HV("Inclusive Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "min") ||
                         !strcasecmp(M_Name.c_str(), "minimum"))
                {
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, min_temp);
                    PUSH_HV("Minimum Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "max") ||
                         !strcasecmp(M_Name.c_str(), "maximum"))
                {
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, max_temp);
                    PUSH_HV("Maximum Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "avg") ||
                         !strcasecmp(M_Name.c_str(), "average"))
                {
                    // Average time is calculated from two temps:
                    //     sum and total counts
                    PUSH_IV(VIEWINST_Display_Average_Tmp, last_column++,
                            VMulti_time_temp, incnt_temp);
                    PUSH_HV("Average Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "stddev"))
                {
                    // The standard deviation is calculated from 3 temps:
                    //     sum, sum of squares, and total counts
                    PUSH_IV(VIEWINST_Display_StdDeviation_Tmp, last_column++,
                            VMulti_time_temp, ssq_temp, incnt_temp);
                    PUSH_HV("StdDev of Time");
                }
                else if (!strcasecmp(M_Name.c_str(), "%count") ||
                         !strcasecmp(M_Name.c_str(), "%counts") ||
                         !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                         !strcasecmp(M_Name.c_str(), "%exclusive_counts"))
                {
                    // Percent is calculate from 2 temps:
                    //     counts for this row and total counts
                    if (!Generate_ButterFly && Filter_Uses_F(cmd))
                    {
                        WARN("Warning: '-m exclusive_counts' is "
                             "not supported with '-f' option.");
                        continue;
                    }
                    else
                    {
                        PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex,
                                excnt_temp);
                    }
                    
                    PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++,
                            excnt_temp, totalIndex++);
                    PUSH_HV("% of Total Exclusive Counts");
                }
                else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                         !strcasecmp(M_Name.c_str(), "%inclusive_counts"))
                {
                    // Percent is calculate from 2 temps:
                    //     number of counts for this row and 
                    //     total inclusive counts
                    if (!Generate_ButterFly && Filter_Uses_F(cmd))
                    {
                        WARN("Warning: '-m inclusive_counts' is "
                             "not supported with '-f' option.");
                        continue;
                    }
                    else
                    {
                        generate_nested_accounting = true;
                        PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex,
                                excnt_temp);
                    }
                    
                    PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++,
                            incnt_temp, totalIndex++);
                    PUSH_HV("% of Total Inclusive Counts");
                }
                else if (!strcasecmp(M_Name.c_str(), "%time") ||
                         !strcasecmp(M_Name.c_str(), "%times") ||
                         !strcasecmp(M_Name.c_str(), "%exclusive_time") ||
                         !strcasecmp(M_Name.c_str(), "%exclusive_times"))
                {
                    // Percent is calculate from 2 temps:
                    //     time for this row and total time
                    if (!Generate_ButterFly && Filter_Uses_F(cmd))
                    {
                        PUSH_IV(VIEWINST_Define_Total_Metric, totalIndex, 1);
                    }
                    else
                    {
                        PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex,
                                extime_temp);
                    }

                    PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++,
                            extime_temp, totalIndex++);
                    PUSH_HV("% of Total Exclusive Time");
                }
                else if (!strcasecmp(M_Name.c_str(), "%inclusive_time") ||
                         !strcasecmp(M_Name.c_str(), "%inclusive_times"))
                {
                    // Percent is calculate from 2 temps:
                    //     time for this row and total inclusive counts
                    if (!Generate_ButterFly && Filter_Uses_F(cmd))
                    {
                        PUSH_IV(VIEWINST_Define_Total_Metric, totalIndex, 1);
                    }
                    else
                    {
                        generate_nested_accounting = true;
                        PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex,
                                extime_temp);
                    }

                    PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++,
                            intime_temp, totalIndex++);
                    PUSH_HV("% of Total Inclusive Time");
                }
                else if (!strcasecmp(M_Name.c_str(), "start") || 
                         !strcasecmp(M_Name.c_str(), "start_time"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++,
                                start_temp);
                        PUSH_HV("Start Time (d:h:m:s)");
                        column_is_DateTime = true;
                    }
                    else
                    {
                        WARN("Warning: '-m start_time' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "stop") || 
                         !strcasecmp(M_Name.c_str(), "stop_time"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, stop_temp);
                        PUSH_HV("Stop Time (d:h:m:s)");
                        column_is_DateTime = true;
                    }
                    else
                    {
                        WARN("Warning: '-m stop_time' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "grid"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, grid_temp);
                        PUSH_HV("Grid Dims");
                    }
                    else
                    {
                        WARN("Warning: '-m grid' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "block"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++,
                                block_temp);
                        PUSH_HV("Block Dims");
                    }
                    else
                    {
                        WARN("Warning: '-m block' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "cache"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++,
                                cache_temp);
                        PUSH_HV("Cache Pref");
                    }
                    else
                    {
                        WARN("Warning: '-m cache' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "rpt"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, rpt_temp);
                        PUSH_HV("Registers Per Thread");
                    }
                    else
                    {
                        WARN("Warning: '-m rpt' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "ssm"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, ssm_temp);
                        PUSH_HV("Static Shared Memory");
                    }
                    else
                    {
                        WARN("Warning: '-m kernel' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "dsm"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, dsm_temp);
                        PUSH_HV("Dynamic Shared Memory");
                    }
                    else
                    {
                        WARN("Warning: '-m dsm' only "
                             "supported for '-v Trace' option.");
                    }
                }
                else if (!strcasecmp(M_Name.c_str(), "lm"))
                {
                    if (vfc == VFC_Trace)
                    {
                        PUSH_IV(VIEWINST_Display_Tmp, last_column++, lm_temp);
                        PUSH_HV("Local Memory");
                    }
                    else
                    {
                        WARN("Warning: '-m lm' only "
                             "supported for '-v Trace' option.");
                    }
                }

                // Recognize and generate pseudo instructions to calculate and
                // display by-thread metrics for ThreadMax, ThreadMaxIndex,
                // ThreadMin, ThreadMinIndex, ThreadAverage and LoadBalance.
                #include "SS_View_bythread_recognize.hxx"

                else
                {
                    WARN(std::string("Warning: Unsupported option, '-m ") +
                         M_Name + "'");
                    return false;
                }

            }
            if (last_column == 1)
            {
                PUSH_IV(VIEWINST_Sort_Ascending,
                        (int64_t)(column_is_DateTime) ? 1 : 0);
            }
        }
    }
    else if (Generate_ButterFly)
    {
        PUSH_IV(VIEWINST_Display_Tmp, last_column++, intime_temp);
        PUSH_HV("Inclusive Time (ms)");
        
        PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex, extime_temp);
        PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++, intime_temp,
                totalIndex++);
        PUSH_HV("% of Total Inclusive Time");
    }
    else
    {
        // If nothing is requested ...

        if (vfc == VFC_Trace)
        {
            PUSH_IV(VIEWINST_Display_Tmp, last_column++, start_temp);
            PUSH_HV("Start Time (d:h:m:s)");

            PUSH_IV(VIEWINST_Sort_Ascending, 1);
        }

        if (Look_For_KeyWord(cmd, "CallTree") ||
            Look_For_KeyWord(cmd, "CallTrees"))
        {
            generate_nested_accounting = true;
        }
        
        PUSH_IV(VIEWINST_Display_Tmp, last_column++, extime_temp);
        PUSH_HV("Exclusive Time (ms)");

        if (Filter_Uses_F(cmd))
        {
            PUSH_IV(VIEWINST_Define_Total_Metric, totalIndex, 1);
        }
        else
        {
            PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex, extime_temp);
        }
        
        PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++, extime_temp,
                totalIndex++);
        PUSH_HV("% of Total Exclusive Time");
        
        if (vfc == VFC_Trace)
        {
            PUSH_IV(VIEWINST_Display_Tmp, last_column++, grid_temp);
            PUSH_HV("Grid Dims");
            
            PUSH_IV(VIEWINST_Display_Tmp, last_column++, block_temp);
            PUSH_HV("Block Dims");
        }
        else
        {
            PUSH_IV(VIEWINST_Display_Tmp, last_column++, excnt_temp);
            PUSH_HV("Exclusive Count");
        }
    }

    if (generate_nested_accounting)
    {
        PUSH_IV(VIEWINST_StackExpand, intime_temp);
        PUSH_IV(VIEWINST_StackExpand, incnt_temp);
    }
    
    if (Generate_Summary_Only)
    {
        PUSH_IV(VIEWINST_Display_Summary_Only);
    }
    else if (Generate_Summary)
    {
        PUSH_IV(VIEWINST_Display_Summary);
    }
    
    return !HV.empty();
}



static bool cuda_definition(CommandObject* cmd,
                            ExperimentObject* exp,
                            int64_t topn,
                            ThreadGroup& tgrp,
                            std::vector<Collector>& CV,
                            std::vector<std::string>& MV,
                            std::vector<ViewInstruction*>& IV,
                            std::vector<std::string>& HV,
                            View_Form_Category vfc)
{
    // Warn about misspelled of meaningless options and
    // exit command processing without generating a view.
    if (!Validate_V_Options(cmd, const_cast<std::string*>(kOptions)))
    {
        return false;
    }
    
    // Define the collector and metric needed for getting main time values
    CV.push_back(Get_Collector(exp->FW(), "cuda"));
    MV.push_back("exec_inclusive_details");
    
    // Define the collector and metric needed for calculating total time
    CV.push_back(Get_Collector(exp->FW(), "cuda"));
    MV.push_back("exec_time");
    
    return define_cuda_columns(cmd, exp, CV, MV, IV, HV, vfc);
}



bool generate_cuda_exec_view(CommandObject* cmd,
                             ExperimentObject* exp,
                             int64_t topn,
                             ThreadGroup& tgrp,
                             std::list<CommandResult*>& view_output)
{
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction*> IV;
    std::vector<std::string> HV;
    
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (cuda_definition(cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc))
    {
        if (CV.empty() || MV.empty())
        {
            WARN("(There are no metrics specified to report.)");
            return false;
        }
        
        switch (vfc)
        {
            
        case VFC_CallStack:
            if (Look_For_KeyWord(cmd, "ButterFly"))
            {
                return Detail_ButterFly_Report(
                    cmd, exp, topn, tgrp, CV, MV, IV, HV,
                    Determine_Metric_Ordering(IV),
                    reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                    view_output
                    );
            }
            else
            {
                return Detail_CallStack_Report(
                    cmd, exp, topn, tgrp, CV, MV, IV, HV,
                    Determine_Metric_Ordering(IV),
                    reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                    view_output
                    );
            }
            
        case VFC_Function:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV,
                Determine_Metric_Ordering(IV),
                reinterpret_cast<Framework::Function*>(NULL), vfc,
                reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                view_output
                );

        case VFC_LinkedObject:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV,
                Determine_Metric_Ordering(IV),
                reinterpret_cast<Framework::LinkedObject*>(NULL), vfc,
                reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                view_output
                );
        
        case VFC_Loop:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV,
                Determine_Metric_Ordering(IV),
                reinterpret_cast<Framework::Loop*>(NULL), vfc,
                reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                view_output
                );
        
        case VFC_Statement:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV,
                Determine_Metric_Ordering(IV),
                reinterpret_cast<Framework::Statement*>(NULL), vfc,
                reinterpret_cast<std::vector<CUDAExecDetail>*>(NULL),
                view_output
                );

        case VFC_Trace:
            return Detail_Trace_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV,
                Determine_Metric_Ordering(IV),
                reinterpret_cast<CUDAExecDetail*>(NULL), view_output
                );
                        
        }
        
        WARN("(There is no supported view name recognized.)");
        return false;
    }

    WARN("(We couldn't determine what information to report for 'cuda' view.)");
    return false;
}
