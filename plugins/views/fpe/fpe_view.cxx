/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/


#if 0
NaN expands to 'not a number'. Various floating point operations can lead to this value. 
IEEE-754 is the most widely used floating point computation standards. It defines 5 possible 
exceptions that can occur during floating point arithematic:

    1) Invalid Operation (NaN): when an invalid operation is performed on a number, for example, 
       taking square root of a negative value, taking log of 0 or a negative number, etc.
    2) Division by Zero: when a number is divided by zero
    3) Overflow: when the result is too large to be representable
    4) Underflow: when the result is too small to be representable
    5) Inexact: when the result is inexact meaning the number was rounded and caused discarding of any non-zero digits

The cases falling under invalid operation would result in a value of NaN as per the above mentioned standard. 
(I get -INF with gcc 3.4.4, but NaN for any negative number's logarithm). You would check for NaNs and Infinities 
resulting out of computations in order to determine the success and validity of the operation but that can 
lead to code that is cluttered with ifs and elses and can become almost un-readable. You can, however, 
do the checks at every logical chunk of the computation. However, NaNs and INFs can get lost mid-calculations. 
The standard dictates that an exceptional value NaN or INF should get propagated throughout the calculations 
so that the checks at the end of a computation could catch them but certain operations can lose them, for example:

    1) NaN operation with INF value would result in INF, as in NaN + INF = INF.
    2) INF can become 0, as in x/INF = 0

The way to catch these would be to check the sticky bits that are essentially the exception flags that are set 
on occurence of floating point exceptions anywhere during the calculation. But let's not get too carried away and 
leave it to the credibility of the programmers and users to be able to handle the various cases in the most sensible way.

#endif


#include "SS_Input_Manager.hxx"
#include "SS_View_Expr.hxx"
#include "FPECollector.hxx"
#include "FPEDetail.hxx"

// Define a new data type to handle the FPEType.
class CommandResult_Fpetype :
     public CommandResult {
  FPEType  fpetype_value;

 public:
  CommandResult_Fpetype () : CommandResult(CMD_RESULT_EXTENSION) {
    fpetype_value = Unknown;
  }
  CommandResult_Fpetype (FPEType Fp) : CommandResult(CMD_RESULT_EXTENSION) {
    SetValueIsID();
    fpetype_value = Fp;
  }
  CommandResult_Fpetype (CommandResult_Fpetype *C) :
       CommandResult(CMD_RESULT_UINT) {
    SetValueIsID();
    fpetype_value = C->fpetype_value;
  }
  virtual ~CommandResult_Fpetype () { }

  virtual CommandResult *Init () { return new CommandResult_Fpetype (); }
  virtual CommandResult *Copy () { return new CommandResult_Fpetype ((CommandResult_Fpetype *)this); }
  virtual bool LT(CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Fpetype));
    return fpetype_value < ((CommandResult_Fpetype *)A)->fpetype_value; }
  virtual bool GT(CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Fpetype));
    return fpetype_value > ((CommandResult_Fpetype *)A)->fpetype_value; }
  virtual void Accumulate_Value (CommandResult *A) { }
  virtual void Accumulate_Min (CommandResult *A) { }
  virtual void Accumulate_Max (CommandResult *A) { }

  void Min_Fpetype (CommandResult_Fpetype *B) {
    fpetype_value = std::min (fpetype_value, B->fpetype_value);
  }
  void Max_Fpetype (CommandResult_Fpetype *B) {
    fpetype_value = std::max (fpetype_value, B->fpetype_value);
  }
  void Accumulate_Fpetype (CommandResult_Fpetype *B) {
  }
  void Value (FPEType& Fp) {
    Fp = fpetype_value;
  }

  virtual std::string Form () {
    std::string S;
    switch (fpetype_value) {
     case InexactResult:
          S = "InexactResult";
          break;
     case Underflow:
          S = "Underflow";
          break;
     case Overflow:
          S = "Overflow";
          break;
     case DivisionByZero:
          S = "DivisionByZero";
          break;
     case Unnormal:
          S = "Unnormal";
          break;
     case Invalid:
          S = "Invalid";
          break;
     case Unknown:
     default:
          S = "Unknown";
          break;
    }
    return std::string (S);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("s",Form().c_str());
  }
  virtual void Print (std::ostream& to, int64_t fieldsize, bool leftjustified) {
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << Form();
  }
};

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage fpe collector data.
#define excnt_temp   VMulti_free_temp
#define incnt_temp   VMulti_free_temp+1
#define start_temp   VMulti_free_temp+2
#define fpeType_temp VMulti_free_temp+3
#define tmean_temp   VMulti_free_temp+4
#define tmin_temp    VMulti_free_temp+5
#define timin_temp   VMulti_free_temp+6
#define tmax_temp    VMulti_free_temp+7
#define timax_temp   VMulti_free_temp+8
#define extra_division_by_zero_temp VMulti_free_temp+9
#define extra_inexact_result_temp VMulti_free_temp+10
#define extra_invalid_temp VMulti_free_temp+11
#define extra_overflow_temp VMulti_free_temp+12
#define extra_underflow_temp VMulti_free_temp+13
#define extra_unknown_temp VMulti_free_temp+14
#define extra_unnormal_temp VMulti_free_temp+15

#define division_by_zero_index ViewReduction_Count
#define inexact_index ViewReduction_Count+1
#define invalid_index ViewReduction_Count+2
#define overflow_index ViewReduction_Count+3
#define underflow_index ViewReduction_Count+4
#define unknown_index ViewReduction_Count+5
#define unnormal_index ViewReduction_Count+6

// fpe view

#define def_FPE_values                                 \
            Time start = Time::TheEnd();               \
            int64_t incnt = 0;                         \
            int64_t excnt = 0;                         \
            FPEType fpeType = Unknown;

#define get_FPE_invalues(primary, num_calls, function_name)           \
              start = std::min(start,primary.dm_time);      \
              fpeType = primary.dm_type;               \
              incnt++;

#define get_FPE_exvalues(secondary,num_calls)           \
              excnt++;

#define get_inclusive_values(stdv, num_calls, function_name)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_FPE_invalues(stdv[i],num_calls, function_name)       \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_FPE_exvalues(stdv[i],num_calls)       \
            }                                           \
}

#define set_FPE_values(value_array, sort_excnt)                                              \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;        \
              if (num_temps > start_temp) {                                                  \
                int64_t x= (start-base_time);                                                \
                value_array[start_temp] = new CommandResult_Duration (x);                    \
              }                                                                              \
              if (num_temps > VMulti_time_temp)  {                                           \
               /* By default, sort on counts, not time. */                                   \
                value_array[VMulti_time_temp]  = CRPTR (sort_excnt ? excnt : incnt);         \
              }                                                                              \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (incnt);           \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (excnt);           \
              if (num_temps > fpeType_temp) value_array[fpeType_temp] = new CommandResult_Fpetype (fpeType);

#define set_ExtraMetric_values(value_array, ExtraValues, index)                                      \
              if (num_temps > tmean_temp) {                                                          \
                if (ExtraValues[ViewReduction_mean]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_mean]->end()) {   \
                  value_array[tmean_temp]                                                            \
                       = ExtraValues[ViewReduction_mean]->find(index)->second->Copy();               \
                } else {                                                                             \
                  value_array[tmean_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmin_temp) {                                                           \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
                  value_array[tmin_temp]                                                             \
                       = ExtraValues[ViewReduction_min]->find(index)->second->Copy();                \
                } else {                                                                             \
                  value_array[tmin_temp] = CRPTR ((int64_t)0);                                       \
                }                                                                                    \
              }                                                                                      \
             if (num_temps > timin_temp) {                                                           \
                if (ExtraValues[ViewReduction_imin]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_imin]->end()) {   \
                  value_array[timin_temp]                                                            \
                                   = ExtraValues[ViewReduction_imin]->find(index)->second->Copy();   \
                } else {                                                                             \
                  value_array[timin_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmax_temp) {                                                           \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
                  value_array[tmax_temp]                                                             \
                       = ExtraValues[ViewReduction_max]->find(index)->second->Copy();                \
                } else {                                                                             \
                  value_array[tmax_temp] = CRPTR ((int64_t)0);                                       \
                }                                                                                    \
              }                                                                                      \
             if (num_temps > timax_temp) {                                                           \
                if (ExtraValues[ViewReduction_imax]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_imax]->end()) {   \
                  value_array[timax_temp]                                                            \
                                   = ExtraValues[ViewReduction_imax]->find(index)->second->Copy();   \
                } else {                                                                             \
                  value_array[timax_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }											     \
              if (num_temps > extra_division_by_zero_temp) {                                         \
                if (ExtraValues[division_by_zero_index]->find(index)                                 \
                                                      != ExtraValues[division_by_zero_index]->end()) { \
                  value_array[extra_division_by_zero_temp]                                           \
                       = ExtraValues[division_by_zero_index]->find(index)->second->Copy();           \
                } else {                                                                             \
                  value_array[extra_division_by_zero_temp] = CRPTR ((uint64_t)0);                    \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_inexact_result_temp) {                                           \
                if (ExtraValues[inexact_index]->find(index)                                          \
                                                      != ExtraValues[inexact_index]->end()) {        \
                  value_array[extra_inexact_result_temp]                                             \
                       = ExtraValues[inexact_index]->find(index)->second->Copy();                    \
                } else {                                                                             \
                  value_array[extra_inexact_result_temp] = CRPTR ((uint64_t)0);                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_invalid_temp) {                                                  \
                if (ExtraValues[invalid_index]->find(index)                                          \
                                                      != ExtraValues[invalid_index]->end()) {        \
                  value_array[extra_invalid_temp]                                                    \
                       = ExtraValues[invalid_index]->find(index)->second->Copy();                    \
                } else {                                                                             \
                  value_array[extra_invalid_temp] = CRPTR ((uint64_t)0);                             \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_overflow_temp) {                                                 \
                if (ExtraValues[overflow_index]->find(index)                                         \
                                                      != ExtraValues[overflow_index]->end()) {       \
                  value_array[extra_overflow_temp]                                                   \
                       = ExtraValues[overflow_index]->find(index)->second->Copy();                   \
                } else {                                                                             \
                  value_array[extra_overflow_temp] = CRPTR ((uint64_t)0);                            \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_underflow_temp) {                                                \
                if (ExtraValues[underflow_index]->find(index)                                        \
                                                      != ExtraValues[underflow_index]->end()) {      \
                  value_array[extra_underflow_temp]                                                  \
                       = ExtraValues[underflow_index]->find(index)->second->Copy();                  \
                } else {                                                                             \
                  value_array[extra_underflow_temp] = CRPTR ((uint64_t)0);                           \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_unknown_temp) {                                                  \
                if (ExtraValues[unknown_index]->find(index)                                          \
                                                      != ExtraValues[unknown_index]->end()) {        \
                  value_array[extra_unknown_temp]                                                    \
                       = ExtraValues[unknown_index]->find(index)->second->Copy();                    \
                } else {                                                                             \
                  value_array[extra_unknown_temp] = CRPTR ((uint64_t)0);                             \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_unnormal_temp) {                                                 \
                if (ExtraValues[unnormal_index]->find(index)                                         \
                                                      != ExtraValues[unnormal_index]->end()) {       \
                  value_array[extra_unnormal_temp]                                                   \
                       = ExtraValues[unnormal_index]->find(index)->second->Copy();                   \
                } else {                                                                             \
                  value_array[extra_unnormal_temp] = CRPTR ((uint64_t)0);                            \
                }                                                                                    \
              }

#define def_Detail_values def_FPE_values
#define get_inclusive_trace get_FPE_invalues
#define get_exclusive_trace get_FPE_exvalues
#define set_Detail_values set_FPE_values
#define Determine_Objects Get_Filtered_Objects
#include "SS_View_detail.txx"


static std::string allowed_fpe_V_options[] = {
  "LinkedObject",
  "LinkedObjects",
  "Dso",
  "Dsos",
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Loop",
  "Loops",
  "Trace",
  "ButterFly",
  "CallTree",
  "CallTrees",
  "TraceBack",
  "TraceBacks",
  "FullStack",
  "FullStacks",
  "DontExpand",
  "Summary",
  "SummaryOnly",
  ""
};

static bool define_fpe_columns (
            CommandObject *cmd,
            ExperimentObject *exp,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Number of columns of information displayed.
  int64_t totalIndex  = 0;  // Number of totals needed to perform % calculations.
  int64_t last_used_temp = extra_unnormal_temp; // Track maximum temps - needed for expressions.
  bool user_defined = false;
  bool ByThread_Rank = exp->Has_Ranks();

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_division_by_zero_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_inexact_result_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_invalid_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_overflow_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_underflow_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_unknown_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_unnormal_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, incnt_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool Generate_Summary = false;
  bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");
  if (!Generate_Summary_Only) {
     Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  }

  bool generate_nested_accounting = false;

 if (Generate_Summary_Only) {
    if (Generate_ButterFly) {
      Generate_Summary_Only = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summaryonly' is not supported with '-v ButterFly'.");
    }
  } else if (Generate_Summary) {
    if (Generate_ButterFly) {
      Generate_Summary = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summary' is not supported with '-v ButterFly'.");
    } else {
     // Total time is always displayed - also add display of the summary time.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
    }
  }

 // Define map for metrics to metric temp.
  std::map<std::string, int64_t> MetricMap;
  MetricMap["count"] = excnt_temp;
  MetricMap["counts"] = excnt_temp;
  MetricMap["event"] = excnt_temp;
  MetricMap["events"] = excnt_temp;
  MetricMap["total_count"] = excnt_temp;
  MetricMap["total_counts"] = excnt_temp;
  MetricMap["exclusive_count"] = excnt_temp;
  MetricMap["exclusive_counts"] = excnt_temp;
  MetricMap["exclusive_detail"] = excnt_temp;
  MetricMap["exclusive_details"] = excnt_temp;
  MetricMap["inclusive_count"] = incnt_temp;
  MetricMap["inclusive_counts"] = incnt_temp;
  MetricMap["inclusive_detail"] = incnt_temp;
  MetricMap["inclusive_details"] = incnt_temp;
  if ((vfc == VFC_Trace) ||
      (vfc == VFC_CallStack)) {
    MetricMap["type"] = fpeType_temp;
    MetricMap["types"] = fpeType_temp;
  }
  if (vfc == VFC_Trace) {
    MetricMap["time"] = start_temp;
    MetricMap["times"] = start_temp;
    MetricMap["start_time"] = start_temp;
    MetricMap["start_times"] = start_temp;
  }

/* The following items are only available with By_Thread compuatations,
 * which are not supported the current expression mechanism.
  MetricMap["DivideByZero"] = extra_division_by_zero_temp;
  MetricMap["Divide_By_Zero"] = extra_division_by_zero_temp;
  MetricMap["DivisionByZero"] = extra_division_by_zero_temp;
  MetricMap["division_by_zero"] = extra_division_by_zero_temp;
  MetricMap["division_by_zero_count"] = extra_division_by_zero_temp;
  MetricMap["InexactResult"] = extra_inexact_result_temp;
  MetricMap["inexact_result"] = extra_inexact_result_temp;
  MetricMap["inexact_result_count"] = extra_inexact_result_temp;
  MetricMap["invalid"] = extra_invalid_temp;
  MetricMap["invalid_operation"] = extra_invalid_temp;
  MetricMap["invalid_count"] = extra_invalid_temp;
  MetricMap["overflow"] = extra_overflow_temp;
  MetricMap["overflow_count"] = extra_overflow_temp;
  MetricMap["underflow"] = extra_underflow_temp;
  MetricMap["underflow_count"] = extra_underflow_temp;
  MetricMap["unknown"] = extra_unknown_temp;
  MetricMap["unknown_count"] = extra_unknown_temp;
  MetricMap["unnormal"] = extra_unnormal_temp;
  MetricMap["unnormal_count"] = extra_unnormal_temp;
*/

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    bool time_metric_selected = false;
    std::vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {

// Look for a metric expression and invoke processing.
#include "SS_View_metric_expressions.hxx"

      bool column_is_DateTime = false;
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "fpe")) {
         // We only know what to do with the fpe collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'fpe' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          continue;
        }
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
         if ( !strcasecmp(M_Name.c_str(), "count") ||
              !strcasecmp(M_Name.c_str(), "counts") ||
              !strcasecmp(M_Name.c_str(), "event") ||
              !strcasecmp(M_Name.c_str(), "events") ||
              !strcasecmp(M_Name.c_str(), "total_count") ||
              !strcasecmp(M_Name.c_str(), "total_counts") ||
              !strcasecmp(M_Name.c_str(), "exclusive_count") ||
              !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
              !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
              !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          HV.push_back("Exclusive Fpe Events");
          user_defined = true;
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display total inclusive counts
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Inclusive Fpe Events");
          user_defined = true;
        } else if ( !strcasecmp(M_Name.c_str(), "Type") ||
                    !strcasecmp(M_Name.c_str(), "Types")) {
         // display the event type
          if ((vfc == VFC_Trace) ||
              (vfc == VFC_CallStack)) {
            IV.push_back(new ViewInstruction (VIEWINST_Require_Field_Equal, -1, fpeType_temp));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, fpeType_temp));
            HV.push_back("Fpe Event Type");
            user_defined = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m types' is only supported with callstack information.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcmp(M_Name.c_str(), "%")           ||
                   !strcasecmp(M_Name.c_str(), "%event") ||
                   !strcasecmp(M_Name.c_str(), "%events") ||
                   !strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_details") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total counts.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the excnt_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
          HV.push_back("% of Total Exclusive Fpe Events");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_details") ) {
         // percent is calculate from 2 temps: number of events for this row and total inclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total counts.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the excnt_temp values.
            generate_nested_accounting = true;
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, incnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp, totalIndex++));
          HV.push_back("% of Total Inclusive Fpe Events");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "time") ||
                   !strcasecmp(M_Name.c_str(), "times") ||
                   !strcasecmp(M_Name.c_str(), "start_time") ||
                   !strcasecmp(M_Name.c_str(), "start_times")) {
          if (vfc == VFC_Trace) {
           // display event time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
            HV.push_back("Fpe Event Time(d:h:m:s)");
            column_is_DateTime = true;
            user_defined = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if ( !strcasecmp(M_Name.c_str(), "DivideByZero") ||
                    !strcasecmp(M_Name.c_str(), "Divide_By_Zero") ||
                    !strcasecmp(M_Name.c_str(), "DivisionByZero") ||
                    !strcasecmp(M_Name.c_str(), "division_by_zero") ||
                    !strcasecmp(M_Name.c_str(), "division_by_zero_count") ||
                    !strcasecmp(M_Name.c_str(), "InexactResult") ||
                    !strcasecmp(M_Name.c_str(), "inexact_result") ||
                    !strcasecmp(M_Name.c_str(), "inexact_result_count") ||
                    !strcasecmp(M_Name.c_str(), "invalid_operation") ||
                    !strcasecmp(M_Name.c_str(), "invalid_count") ||
                    !strcasecmp(M_Name.c_str(), "overflow") ||
                    !strcasecmp(M_Name.c_str(), "overflow_count") ||
                    !strcasecmp(M_Name.c_str(), "underflow") ||
                    !strcasecmp(M_Name.c_str(), "underflow_count") ||
                    !strcasecmp(M_Name.c_str(), "unknown") ||
                    !strcasecmp(M_Name.c_str(), "unknown_count") ||
                    !strcasecmp(M_Name.c_str(), "unnormal") ||
                    !strcasecmp(M_Name.c_str(), "unnormal_count")) {
         // display specific exception counts
         // Accomplish this by changing the requested metric.
          int64_t extra_index = 0;
          int64_t extra_temp = 0;
          if (!strcasecmp(M_Name.c_str(), "DivisionByZero") ||
              !strcasecmp(M_Name.c_str(), "division_by_zero") ||
              !strcasecmp(M_Name.c_str(), "division_by_zero_count")) {
            M_Name = "division_by_zero_count";
            extra_index = division_by_zero_index;
            extra_temp = extra_division_by_zero_temp;
          } else if (!strcasecmp(M_Name.c_str(), "InexactResult") ||
                     !strcasecmp(M_Name.c_str(), "inexact_result") ||
                     !strcasecmp(M_Name.c_str(), "inexact_result_count")) {
            M_Name = "inexact_result_count";
            extra_index = inexact_index;
            extra_temp = extra_inexact_result_temp;
          } else if (!strcasecmp(M_Name.c_str(), "invalid") ||
                     !strcasecmp(M_Name.c_str(), "invalid_operation") ||
                     !strcasecmp(M_Name.c_str(), "invalid_count")) {
            M_Name = "invalid_count";
            extra_index = invalid_index;
            extra_temp = extra_invalid_temp;
          } else if (!strcasecmp(M_Name.c_str(), "overflow") ||
                     !strcasecmp(M_Name.c_str(), "overflow_count")) {
            M_Name = "overflow_count";
            extra_index = overflow_index;
            extra_temp = extra_overflow_temp;
          } else if (!strcasecmp(M_Name.c_str(), "underflow") ||
                     !strcasecmp(M_Name.c_str(), "underflow_count")) {
            M_Name = "underflow_count";
            extra_index = underflow_index;
            extra_temp = extra_underflow_temp;
          } else if (!strcasecmp(M_Name.c_str(), "unknown") ||
                     !strcasecmp(M_Name.c_str(), "unknown_count")) {
            M_Name = "unknown_count";
            extra_index = unknown_index;
            extra_temp = extra_unknown_temp;
          } else if (!strcasecmp(M_Name.c_str(), "unnormal") ||
                     !strcasecmp(M_Name.c_str(), "unnormal_count")) {
            M_Name = "unnormal_count";
            extra_index = unnormal_index;
            extra_temp = extra_unnormal_temp;
          }
          int64_t cmIx = CV.size();
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, extra_index, cmIx, ViewReduction_sum));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extra_temp));
          CV.push_back(CV[0]);
          MV.push_back(M_Name);
          HV.push_back( Find_Metadata(CV[cmIx],MV[cmIx]).getDescription() );
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                   !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average of the overflows..
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
          HV.push_back("Average Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
         // Find the By-Thread Min.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
          HV.push_back("Min Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMinIndex")) {
         // Find the Rank of the By-Thread Min.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imin));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
          HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Min");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
          HV.push_back("Max Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMaxIndex")) {
         // Find the Rank of the By-Thread Max.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imax));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
          HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Max");
          user_defined = true;
        } else if ( !strcasecmp(M_Name.c_str(), "loadbalance")) {
         // Find the By-Thread Max.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
          HV.push_back("Max Exclusive Fpe Event Counts Across Threads");

         // Report ThreadId of Max.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imax));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
          HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Max");

         // Find the By-Thread Min.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
          HV.push_back("Min Exclusive Fpe Event Counts Across Threads");

         // Report ThreadId of Min.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imin));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
          HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Min");

         // Do a By-Thread average of the overflows..
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
          HV.push_back("Average Exclusive Fpe Event Counts Across Threads");
          user_defined = true;

        } else if (!strcasecmp(M_Name.c_str(), "absdiff")) {
         // Ignore this because cview -c 3 -c 5 -mtime,absdiff actually works outside of this view code
         // Mark_Cmd_With_Soft_Error(cmd,"AbsDiff option, '-m " + M_Name + "'");
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
          return false;
        }
      }
      if (last_column == 1) {
        IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, (int64_t)(column_is_DateTime) ? 1 : 0));
      }
    }
  } 

  if (!user_defined) {
    if (Generate_ButterFly) {
     // Default ButterFly view.
     // Column[0] is inclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
      HV.push_back("Inclusive Fpe Event Counts");

    // Column[1] in % of exclusive events
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
      IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp, totalIndex++));
      HV.push_back("% of Total Counts");
    } else {
     // If nothing is requested ...
      if (vfc == VFC_Trace) {
        // Insert event time into report.
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
        HV.push_back("Fpe Event Time(d:h:m:s)");
        IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, 1)); // final report in ascending time order
      }
     // Always display counts.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
      HV.push_back("Exclusive Fpe Event Counts");

      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
      HV.push_back("Inclusive Fpe Event Counts");

     // And display % of exclusive events
      if (Filter_Uses_F(cmd)) {
       // Use the metric needed for calculating total counts.
        IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
      } else {
       // Sum the excnt_temp values.
        IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
      }
      IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
      HV.push_back("% of Total Counts");
    }
  }
  // Add display of the summary time.
  if (Generate_Summary_Only) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Only));
   } else if (Generate_Summary) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
   }

  if (generate_nested_accounting) {
    IV.push_back(new ViewInstruction (VIEWINST_StackExpand, incnt_temp));
  }
  
  return (HV.size() > 0);
}

static bool fpe_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
    Assert (CV.begin() != CV.end());
    CollectorGroup cgrp = exp->FW()->getCollectors();
    Collector C = *CV.begin();
    if (cgrp.find(C) == std::set<Collector>::iterator(cgrp.end())) {
      std::string C_Name = C.getMetadata().getUniqueId();
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
    std::string M_Name = MV[0];
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    // Warn about misspelled of meaningless options and exit command processing without generating a view.
    bool all_valid = Validate_V_Options (cmd, allowed_fpe_V_options);
    if ( all_valid == false ) {
      return false;
    }

    return define_fpe_columns (cmd, exp, CV, MV, IV, HV, vfc);
}


static std::string VIEW_fpe_brief = "Fpe Report";
static std::string VIEW_fpe_short = "Report each floating point exception.";
static std::string VIEW_fpe_long  =
                  "\nA positive integer can be added to the end of the keyword"
                  " 'fpe' to indicate the maximum number of items in the report."
                  " The selection will be based on the number of times events occurred."
                  "\n\nThe form of the information displayed can be controlled through"
                  " the  '-v' option.  Except for the '-v Trace' option, the report will"
                  " be sorted in descending order of the number of times events occurred."
                  "\n\t'-v Functions' will produce a summary report of the function the"
                  " in descending order of the number of times each fpe event"
                  " event ocurred in.  This is the default display."
                  "\n\t'-v Statements' will produce a summary report of the statement the"
                  " event occurred in."
                  "\n\t'-v Loops' will produce a summary report of the loop the"
                  " event occurred in."
                  "\n\t'-v LinkeObjects' will produce a summary report of the linked object"
                  " the event occurred in."
                  "\n\t'-v Trace' will produce a report of each call to an mpi function."
                  " It will be sorted in ascending order of the starting time for the event."
                  "\n\t'-v CallTrees' will produce a calling stack report that is presented"
                  " in calling tree order - from the start of the program to the measured"
                  " program."
                  "\n\t'-v TraceBacks' will produce a calling stack report that is presented"
                  " in traceback order - from the measured function to the start of the"
                  " program."
                  "\n\tThe addition of 'FullStack' with either 'CallTrees' of 'TraceBacks'"
                  " will cause the report to include the full call stack for each measured"
                  " function.  Redundant portions of a call stack are suppressed by default."
                  "\n\tThe addition of 'Summary' to the '-v' option list along with 'Functions',"
                  " 'Statements', 'LinkeObjects','Loops'"
                  " 'CallTrees' or 'TraceBacks' will result in an additional line of output at"
                  " the end of the report that summarizes the information in each column."
                  "\n\tThe addition of 'SummaryOnly' to the '-v' option list along with 'Functions',"
                  " 'CallTrees' or 'TraceBacks' or without those options will cause only the"
                  " one line of output at the end of the report that summarizes the information in each column."
                  "\n\t'-v ButterFly' along with a '-f <function_list>' will produce a report"
                  " that summarizes the calls to a function and the calls from the function."
                  " The calling functions will be listed before the named function and the"
                  " called functions afterwards, by default, although the addition of"
                  " 'TraceBacks' to the '-v' specifier will reverse this ordering."
                  "\n\nThe information included in the report can be controlled with the"
                  " '-m' option.  More than one item can be selected but only the items"
                  " listed after the option will be printed and they will be printed in"
                  " the order that they are listed."
                  " If no '-m' option is specified, the default is equivalent to"
                  " '-m counts, percent'."
                  " Each option reports information about the set of fpe events that is"
                  " reported for the items on that particular line in the report."
                  " \n\t'-m time' reports the first time at which the event occurred."
                  " \n\t'-m counts' reports the number of times the event occurred in the code unit."
                  " \n\t'-m percent' reports the percent of total counts the code unit."
                  " \n\t'-m ThreadMax' reports the maximum counts for a process."
                  " \n\t'-m ThreadMaxIndex' reports the procecss Id for the thread of the 'ThreadMax'."
                  " \n\t'-m ThreadMin' reports the minimum counts for a process."
                  " \n\t'-m ThreadMinIndex' reports the procecss Id for the thread of the 'ThreadMin'."
                  " \n\t'-m ThreadAverage' reports the average counts for a process."
                  " \n\t'-m loadbalance' is the same as '-m ThreadMax, ThreadMaxIndex, ThreadMin,"
                  " ThreadMinIndex, ThreadAverage'."

                  " \n\t'-m type' reports the type of the floating point exception:"
                  "\n\nIt is also possible to select a subset of event types to display."
                  " This is done with the '-m' option and a single key word:"
                  " \n\t'-m inexact_result_count'"
                  " \n\t'-m underflow_count'"
                  " \n\t'-m overflow_count'"
                  " \n\t'-m division_by_zero_count'"
                  " \n\t'-m unnormal_count'"
                  " \n\t'-m invalid_count'"
                  " \n\t'-m unknown_count'";
static std::string VIEW_fpe_example = "\texpView fpe\n"
                                      "\texpView -v CallTrees,FullStack fpe10 -m underflow_count\n";
static std::string VIEW_fpe_metrics[] =
  { "total_count",
    "exclusive_details",
    "inclusive_details",
    "division_by_zero_count",
    "inexact_result_count",
    "invalid_count",
    "overflow_count",
    "underflow_count",
    "unknown_count",
    "unnormal_count",
    ""
  };
static std::string VIEW_fpe_collectors[] =
  { "fpe",
    ""
  };
class fpe_view : public ViewType {

 public: 
  fpe_view() : ViewType ("fpe",
                         VIEW_fpe_brief,
                         VIEW_fpe_short,
                         VIEW_fpe_long,
                         VIEW_fpe_example,
                        &VIEW_fpe_metrics[0],
                        &VIEW_fpe_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "fpe"));  // Define the collector
    MV.push_back ("inclusive_details"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "fpe"));  // Define the collector
    MV.push_back ("total_count"); // define the metric needed for calculating total counts.
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (fpe_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<FPEDetail> dummyVector;
      FPEDetail *dummyDetail;
      switch (vfc) {
       case VFC_Trace:
        return Detail_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                    true, dummyDetail, view_output);
       case VFC_CallStack:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          true, &dummyVector, view_output);
        } else {
          return Detail_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          true, &dummyVector, view_output);
        }
       case VFC_Function:
        Framework::Function *fp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, fp, vfc, &dummyVector, view_output);
       case VFC_LinkedObject:
        LinkedObject *lp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, lp, vfc, &dummyVector, view_output);
       case VFC_Statement:
        Statement *sp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, sp, vfc, &dummyVector, view_output);
       case VFC_Loop:
        Loop *loopp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, loopp, vfc, &dummyVector, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name supplied.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(There is no requested information to report for 'fpe' view.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void fpe_view_LTX_ViewFactory () {
  Define_New_View (new fpe_view());
}
