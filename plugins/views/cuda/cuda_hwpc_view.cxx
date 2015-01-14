////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010-2014 Krell Institute. All Rights Reserved.
// Copyright (c) 2015 Argo Navis Technologies. All Rights Reserved.
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

#include "SS_Input_Manager.hxx"
#include "SS_View_Expr.hxx"

#include "CUDACollector.hxx"
#include "CUDADeviceDetail.hxx"
#include "CUDACountsDetail.hxx"

#define PUSH_HV(x) HV.push_back(x)
#define PUSH_IV(...) IV.push_back(new ViewInstruction(__VA_ARGS__))

#define WARN(x) Mark_Cmd_With_Soft_Error(cmd, x);

using namespace std;



static const string kOptions[] = {
    "HWPC", // This is the option that selects this particular sub-view
    "Dso", "Dsos",
    "Function", "Functions",
    "LinkedObject", "LinkedObjects",
    "Loop", "Loops",
    "Statement", "Statements",
    "Summary", "SummaryOnly",
    ""
};



#if defined(DISABLE_FOR_NOW)

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage cuda collector data.
#define excnt_temp   VMulti_free_temp
#define extime_temp  VMulti_free_temp+1
#define event_temps  VMulti_free_temp+2

#define First_ByThread_Temp VMulti_free_temp+OpenSS_NUMCOUNTERS+3
#define ByThread_use_intervals 2 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 // otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"

#define Determine_Objects Get_Filtered_Objects

#define def_CUDA_values                                                 \
    double extime = 0.0;                                                \
    uint64_t excnt = 0;                                                 \
    uint64_t exevent[OpenSS_NUMCOUNTERS];                               \
    {                                                                   \
        for (int i = 0; i < OpenSS_NUMCOUNTERS; ++i)                    \
        {                                                               \
            exevent[i] = 0;                                             \
        }                                                               \
    }

#define get_CUDA_invalues(primary, num_calls, function_name)

#define get_CUDA_exvalues(secondary, num_calls)                         \
    extime += secondary.dm_time / num_calls;                            \
    excnt++;                                                            \
    {                                                                   \
        for (int i = 0; i < OpenSS_NUMCOUNTERS; ++i)                    \
        {                                                               \
            exevent[i] += secondary.dm_event_values[i];                 \
        }                                                               \
    }

#define get_inclusive_values(stdv, num_calls, function_name)            \
    {                                                                   \
        int64_t len = stdv.size();                                      \
        for (int64_t i = 0; i < len; i++)                               \
        {                                                               \
            get_CUDA_invalues(stdv[i], num_calls, function_name)        \
        }                                                               \
    }

#define get_exclusive_values(stdv, num_calls)           \
    {                                                   \
        int64_t len = stdv.size();                      \
        for (int64_t i = 0; i < len; i++)               \
        {                                               \
            get_CUDA_exvalues(stdv[i], num_calls)       \
        }                                               \
    }

#define set_CUDA_values(value_array, sort_excnt)                        \
    if (num_temps > VMulti_sort_temp)                                   \
    {                                                                   \
        value_array[VMulti_sort_temp] = NULL;                           \
    }                                                                   \
    if (num_temps > VMulti_time_temp)                                   \
    {                                                                   \
        value_array[VMulti_time_temp] = CRPTR(extime);                  \
    }                                                                   \
    if (num_temps > extime_temp)                                        \
    {                                                                   \
        value_array[extime_temp] = CRPTR(extime);                       \
    }                                                                   \
    if (num_temps > excnt_temp)                                         \ 
    {                                                                   \
        value_array[excnt_temp] = CRPTR(excnt);                         \
    }                                                                   \
    if (num_temps > event_temps)                                        \
    {                                                                   \
        for (int i = 0; i < OpenSS_NUMCOUNTERS; ++i)                    \ 
        {                                                               \
            if (num_temps <= (event_temps + i))                         \
            {                                                           \
                break;                                                  \
            }                                                           \
            value_array[event_temps + i] = CRPTR(exevent[i]);           \
        }                                                               \
    }

#define def_Detail_values def_CUDA_values
#define get_inclusive_trace get_inclusive_values
#define get_exclusive_trace get_exclusive_values
#define set_Detail_values set_CUDA_values

#include "SS_View_detail.txx"



static bool define_cuda_columns(CommandObject* cmd,
                                ExperimentObject* exp,
                                vector<Collector>& CV,
                                vector<string>& MV,
                                vector<ViewInstruction*>& IV,
                                vector<string>& HV,
                                View_Form_Category vfc)
{
    int64_t last_column = 0;  // # of columns of information displayed
    int64_t totalIndex  = 0;  // # of totals needed to perform % calculations

    // Track maximum temps - needed for expressions
    int64_t last_used_temp = Last_ByThread_Temp;

    // Define combination instructions for predefined temporaries
    PUSH_IV(VIEWINST_Add, VMulti_sort_temp);
    PUSH_IV(VIEWINST_Add, VMulti_time_temp);
    PUSH_IV(VIEWINST_Add, extime_temp);
    PUSH_IV(VIEWINST_Add, excnt_temp);

    OpenSpeedShop::cli::ParseResult* p_result = cmd->P_Result();
    vector<ParseRange>* p_slist = p_result->getexpMetricList();
    
    bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
    bool Generate_Summary = false;
    bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");

    if (!Generate_Summary_Only)
    {
        Generate_Summary = Look_For_KeyWord(cmd, "Summary");
    }

    int64_t View_ByThread_Identifier = Determine_ByThread_Id(exp, cmd);
    string ByThread_Header = Find_Metadata(CV[0], "time").getShortName();
    
    if (Generate_ButterFly)
    {
        Generate_ButterFly = FALSE;
        WARN("Warning: '-v ButterFly' is not supported with this view.");
    }
    
    map<string, int64_t> MetricMap;
    
    MetricMap["time"] = extime_temp;
    MetricMap["times"] = extime_temp;

    // Determine the available events for the detail metric.

    int64_t num_events = 0;
    string papi_names[OpenSS_NUMCOUNTERS];
    Collector c = CV[0];
    string Value;
    CV[0].getParameterValue("event", Value);
    
    istringstream evStream(Value);
    string evElement;
    while (getline(evStream, evElement, ','))
    {
        string event_name = lowerstring(evElement);
        papi_names[num_events] = event_name;
        MetricMap[event_name] = event_temps + num_events;
        num_events++;
    }
    
    for (int i = 0; i < num_events; i++)
    {
        PUSH_IV(VIEWINST_Add, event_temps + i);
    }
    
    if (p_slist->begin() != p_slist->end())
    {
        // Add modifiers to output list.
        vector<ParseRange>::iterator mi;
        for (mi = p_slist->begin(); mi != p_slist->end(); mi++)
        {
            if ((*mi).getParseType() == PARSE_EXPRESSION_VALUE)
            {
                string header = "user expression";
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

            parse_range_t* m_range = (*mi).getRange();
            string C_Name, M_name;

            if (m_range->is_range)
            {
                C_Name = m_range->start_range.name;
                if (!strcasecmp(M_Name.c_str(), "cuda"))
                {
                    // We only know what to do with the cuda collector.
                    WARN(string("The specified collector, ") + C_Name + 
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
                if (!strcasecmp(M_Name.c_str(), "time") ||
                    !strcasecmp(M_Name.c_str(), "times"))
                {                    
                    PUSH_IV(VIEWINST_Display_Tmp, last_column++, extime_temp);
                    PUSH_HV("Exclusive Time (ms)");
                }
                else if (!strcasecmp(M_Name.c_str(), "%time") ||
                         !strcasecmp(M_Name.c_str(), "%times"))
                {
                    // Percent is calculate from 2 temps:
                    //     time for this row and total time
                    PUSH_IV(VIEWINST_Define_Total_Tmp, totalIndex, extime_temp);
                    PUSH_IV(VIEWINST_Display_Percent_Tmp, last_column++,
                            extime_temp, totalIndex++);
                    PUSH_HV("% of Total Exclusive Time");
                }
                
                // Recognize and generate pseudo instructions to calculate and
                // display by-thread metrics for ThreadMax, ThreadMaxIndex,
                // ThreadMin, ThreadMinIndex, ThreadAverage and LoadBalance.
                #include "SS_View_bythread_recognize.hxx"

                else
                {
                    // Look for Event options.
                    bool event_found = false;
                    for (int i = 0; i < num_events; i++)
                    {
                        const char *c = papi_names[i].c_str();
                        
                        if (c == NULL)
                        {
                            break;
                        }

                        if (!strcasecmp(M_Name.c_str(), c) ||
                            !strcasecmp(M_Name.c_str(), "AllEvents")) 
                        {
                            event_found = true;
                            PUSH_IV(VIEWINST_Display_Tmp, last_column++,
                                    event_temps + i);
                            PUSG_HV(papi_names[i]);
                        }
                    }
                    
                    if (!event_found)
                    {
                        // Unrecognized '-m' option.
                        WARN("Warning: Unsupported option, '-m " + 
                             M_Name + "'");
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        // If nothing is requested ...

        PUSH_IV(VIEWINST_Display_Tmp, last_column++, extime_temp);
        PUSH_HV(Find_Metadata(CV[0], "time").getDescription());

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
        PUSH_HV(string("% of ") + Find_Metadata(CV[0], "time").getShortName());
        
        // This code (copied from above) adds the individual event results
        // into the default view

        // Look for Event options.
        for (int i = 0; i < num_events; i++)
        {
            const char *c = papi_names[i].c_str();

            if (c == NULL)
            {
                break;
            }

            PUSH_IV(VIEWINST_Display_Tmp, last_column++, event_temps + i);
            PUSH_HV(papi_names[i]);
        }
    }
    
    if (Generate_Summary_Only)
    {
        PUSH_IV(VIEWINST_Display_Summary_Only);
    }
    else
    {
        PUSH_IV(VIEWINST_Display_Summary);
    }
    
    return last_column > 0;
}



static bool cuda_definition(CommandObject* cmd,
                            ExperimentObject* exp,
                            int64_t topn,
                            ThreadGroup& tgrp,
                            vector<Collector>& CV,
                            vector<string>& MV,
                            vector<ViewInstruction*>& IV,
                            vector<string>& HV,
                            View_Form_Category vfc)
{
    // Warn about misspelled of meaningless options and
    // exit command processing without generating a view.
    if (!Validate_V_Options(cmd, const_cast<string*>(kOptions)))
    {
        return false;
    }

    // Define the collector and metric needed for getting event counts
    CV.push_back(Get_Collector(exp->FW(), "cuda"));
    MV.push_back("count_exclusive_details");
    
    return define_cuda_columns(cmd, exp, CV, MV, IV, HV, vfc);
}

#endif



bool generate_cuda_hwpc_view(CommandObject* cmd,
                             ExperimentObject* exp,
                             int64_t topn,
                             ThreadGroup& tgrp,
                             list<CommandResult*>& view_output)
{
#if defined(DISABLE_FOR_NOW)
    vector<Collector> CV;
    vector<string> MV;
    vector<ViewInstruction*> IV;
    vector<string> HV;

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
   
        case VFC_Function:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV, false,
                reinterpret_cast<Framework::Function*>(NULL), vfc,
                reinterpret_cast<vector<CUDACountsDetail>*>(NULL), view_output
                );

        case VFC_LinkedObject:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV, false,
                reinterpret_cast<Framework::LinkedObject*>(NULL), vfc,
                reinterpret_cast<vector<CUDACountsDetail>*>(NULL), view_output
                );

        case VFC_Loop:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV, false,
                reinterpret_cast<Framework::Loop*>(NULL), vfc,
                reinterpret_cast<vector<CUDACountsDetail>*>(NULL), view_output
                );
            
        case VFC_Statement:
            return Detail_Base_Report(
                cmd, exp, topn, tgrp, CV, MV, IV, HV, false,
                reinterpret_cast<Framework::Statement*>(NULL), vfc,
                reinterpret_cast<vector<CUDACountsDetail>*>(NULL), view_output
                );
            
        }
          
        WARN("(There is no supported view name recognized.)");
        return false;
    }
#endif
    
    WARN("(We couldn't determine what information to report for 'cuda' view.)");
    return false;
}
