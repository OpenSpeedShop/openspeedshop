/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Definition of the ViewInstruction class.
 * Definition of the ViewType class.
 *
 */

#include "Queries.hxx"

enum ViewOpCode {
     VIEWINST_Define_Total_Metric,      // TmpResult is the id of the total.
                                        // TMP_index1 is CV & MV index of value that, when summed, is the Total.
     VIEWINST_Define_Total_Tmp,         // TmpResult is the id of the total.
                                        // TMP_index1 is row_tmp# of fields in the records that are summed.
     VIEWINST_Define_ByThread_Metric,   // TmpResult is only set for ViewReduction_sum, otherwise it must always be '-1'.
                                        // TMP_index1 is CV & MV index of the value to display.
                                        // TMP_index2 is the indicator of the reduction that is applied.
                                        // TMP_index3 is the indicator of the associated thread identifier.
     VIEWINST_Require_Field_Equal,      // TMP_index1 is row_tmp# of fields in the records that must match.
     VIEWINST_Display_Metric,           // TmpResult is column# to display in.
                                        // TMP_index1 is CV & MV index of the value to display.
     VIEWINST_Display_ByThread_Metric,  // TmpResult is column# to display in.
                                        // TMP_index1 is CV & MV index of the value to display.
                                        // TMP_index2 is the indicator of the reduction that is applied.
     VIEWINST_Display_Tmp,              // TmpResult is column# to display in.
                                        // TMP_index1 is predefined temp# of the value to display..
     VIEWINST_Display_Percent_Column,   // TmpResult is column# to display in.
                                        // Column[TMP_index1]*100/Total is value to display in the column.
                                        // TMP_index2 is the id of the Total that is used.
     VIEWINST_Display_Percent_Tmp,      // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with numerator.
                                        // TMP_index2 is the id of the Total that is used.
     VIEWINST_Display_Average_Tmp,      // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with numerator.
                                        // TMP_index2 is row_tmp# with denominator.
     VIEWINST_Display_StdDeviation_Tmp, // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with sums.
                                        // TMP_index2 is row_tmp# with sum of squares
     VIEWINST_Display_Flops_Tmp,        // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with counts
                                        // TMP_index2 is row_tmp# with time
     VIEWINST_Display_Ratio_Tmp,        // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with counts
                                        // TMP_index2 is row_tmp# with counts
     VIEWINST_Display_Ratio_Percent_Tmp,// TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with counts
                                        // TMP_index2 is row_tmp# with counts
     VIEWINST_Display_Inverse_Ratio_Percent_Tmp,// TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with counts
                                        // TMP_index2 is row_tmp# with counts
     VIEWINST_Display_Summary,          // Generate column summary.
     VIEWINST_Display_Summary_Only,     // Only generate and display column summary.
     VIEWINST_Sort_Ascending,           // if TMP_index1 is not 0, sort final report in ascending order
     VIEWINST_Expression,               // TmpResult is the predefined temp# that is the result of an expression evaluation
                                        // Expression_op indicates the arithmetic operation
                                        // TMP_index1 is row_tmp# with first value
                                        // TMP_index2 is row_tmp# with second value
                                        // TMP_index3 is row_tmp# with third value
     VIEWINST_Add,                      // TMP_index1 is predefined temp# combined with '+' op. 
     VIEWINST_Min,                      // TMP_index1 is predefined temp# combined with 'min' op.
     VIEWINST_Max,                      // TMP_index1 is predefined temp# combined with 'max' op.
     VIEWINST_Summary_Max,              // TMP_index1 is predefined temp# combined with 'max' op.
     VIEWINST_StackExpand,              // TMP_index1 is predefined temp# that is expanded by copy.
     VIEWINST_SetConstString,           // TmpResult is the predefined temp# that is assigned the value.
                                        // TMP_index1 is the value of the constant.
     VIEWINST_SetConstInt,              // TmpResult is the predefined temp# that is assigned the value.
                                        // TMP_index1 is the value of the constant.
     VIEWINST_SetConstFloat,            // TmpResult is the predefined temp# that is assigned the value.
                                        // TMP_index1 is the value of the constant.
};

// Indicators of the ByThread reduction functions.
#define ViewReduction_sum   0
#define ViewReduction_mean  1
#define ViewReduction_min   2
#define ViewReduction_imin  3
#define ViewReduction_max   4
#define ViewReduction_imax  5
#define ViewReduction_Count 6

// Indicators of the ByThread Ids.
#define View_ByThread_NotSpecified 0
#define View_ByThread_Rank         1
#define View_ByThread_OpenMPThread 2
#define View_ByThread_PosixThread  3
#define View_ByThread_Process      4
extern std::string View_ByThread_Id_name[5];

class ViewInstruction
{
 private:
  ViewOpCode Instruction;
  expression_operation_t Expression_op;
  bool Accumulate_expression; // Mark VIEWINST_Expression ops that are used to accumulate values
  int64_t TmpResult;  // result temp or column number
  int64_t TMP_index1; // index of Collector::Metric vectors or temp
  int64_t TMP_index2; // index of temp
  int64_t TMP_index3; // index of temp
  CommandResult *constValue; // only valid for 'VIEWINST_SetConst...' entries.

 public:
  ViewInstruction (ViewOpCode I) {
    Instruction = I;
    Expression_op = EXPRESSION_OP_ERROR;
    Accumulate_expression = false;
    TmpResult = -1;
    TMP_index1 = -1;
    TMP_index2 = -1;
    TMP_index3 = -1;
    constValue = NULL;
  }
  ViewInstruction (ViewOpCode I, int64_t TR) {
    Instruction = I;
    Expression_op = EXPRESSION_OP_ERROR;
    Accumulate_expression = false;
    TmpResult = -1;
    TMP_index1 = TR;
    TMP_index2 = -1;
    TMP_index3 = -1;
    constValue = NULL;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1) {
    Instruction = I;
    Expression_op = EXPRESSION_OP_ERROR;
    Accumulate_expression = false;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = -1;
    TMP_index3 = -1;
    constValue = NULL;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1, int64_t TMP2) {
    Instruction = I;
    Expression_op = EXPRESSION_OP_ERROR;
    Accumulate_expression = false;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = TMP2;
    TMP_index3 = -1;
    constValue = NULL;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1, int64_t TMP2, int64_t TMP3) {
    Instruction = I;
    Expression_op = EXPRESSION_OP_ERROR;
    Accumulate_expression = false;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = TMP2;
    TMP_index3 = TMP3;
    constValue = NULL;
  }

  ~ViewInstruction () {
    if (constValue != NULL) delete constValue;
   }
/*

  inline ViewInstruction *ViewInstructionConstant (ViewOpCode I, int64_t TR, CommandResult *cv) {
    ViewInstruction *VI = new ViewInstruction (I, TR);
    VI->constValue = cv;
    return VI;
  }
*/

  ViewOpCode OpCode () { return Instruction; }
  expression_operation_t ExprOpCode () { return Expression_op;}
  bool AccumulateExpr () { return (Accumulate_expression || Can_Accumulate(Expression_op)); }
  int64_t TR () { return TmpResult; }
  int64_t TMP1 () { return TMP_index1; }
  int64_t TMP2 () { return TMP_index2; }
  int64_t TMP3 () { return TMP_index3; }
  CommandResult *ConstValue() { return constValue; }

  void Set_ViewExprOpCode (expression_operation_t op) {
    Assert (Instruction == VIEWINST_Expression);
    Expression_op = op;
  }
  void Set_ViewExprAccumulate () {
    Assert (Instruction == VIEWINST_Expression);
    Assert (Expression_op != EXPRESSION_OP_ERROR);
    Accumulate_expression = true;
  }

  void Print (std::ostream &to) {
    std::string op;
    switch (Instruction) {
     case VIEWINST_Define_Total_Metric: op = "Define_Total_Metric"; break;
     case VIEWINST_Define_Total_Tmp: op = "Define_Total_Tmp"; break;
     case VIEWINST_Define_ByThread_Metric: op = "Define_ByThread_Metric"; break;
     case VIEWINST_Require_Field_Equal: op = "Require_Field_Equal"; break;
     case VIEWINST_Display_Metric: op = "Display_Metric"; break;
     case VIEWINST_Display_ByThread_Metric: op = "Display_ByThread_Metric"; break;
     case VIEWINST_Display_Tmp: op = "Display_Tmp"; break;
     case VIEWINST_Display_Percent_Column: op = "Display_Percent_Column"; break;
     case VIEWINST_Display_Percent_Tmp: op = "Display_Percent_Tmp"; break;
     case VIEWINST_Display_Average_Tmp: op = "Display_Average_Tmp"; break;
     case VIEWINST_Display_StdDeviation_Tmp: op = "Display_StdDeviation_Tmp"; break;
     case VIEWINST_Display_Flops_Tmp: op = "Display_Flops_Tmp"; break;
     case VIEWINST_Display_Ratio_Tmp: op = "Display_Ratio_Tmp"; break;
     case VIEWINST_Display_Summary: op = "Display_Summary"; break;
     case VIEWINST_Display_Summary_Only: op = "Display_Summary_Only"; break;
     case VIEWINST_Sort_Ascending: op = "Ascending_Sort"; break;
     case VIEWINST_Expression: op = "Expression[";
                               op += ExprOperator(Expression_op).c_str();
                               op += "]"; break;
     case VIEWINST_Add: op = "Add"; break;
     case VIEWINST_Min: op = "Min"; break;
     case VIEWINST_Max: op = "Max"; break;
     case VIEWINST_Summary_Max: op = "Summary_Max"; break;
     case VIEWINST_StackExpand: op = "StackExpand"; break;
     case VIEWINST_SetConstString: op = "SetConstString"; break;
     case VIEWINST_SetConstInt: op = "SetConstInt"; break;
     case VIEWINST_SetConstFloat: op = "SetConstFloat"; break;
     default: op ="(unknown)"; break;
    }
    if (Accumulate_expression) to << " Accumulate ";
    to << op << " " << TmpResult  << " "
                    << TMP_index1 << " "
                    << TMP_index2 << " "
                    << TMP_index3;
    if (constValue != NULL) {to << "  :";constValue->Print(to, 20, true);}
    to << std::endl;
  }

friend ViewInstruction *ViewInstructionConstant (ViewOpCode I, int64_t TR, CommandResult *cv);
};

  inline ViewInstruction *ViewInstructionConstant (ViewOpCode I, int64_t TR, CommandResult *cv) {
    ViewInstruction *VI = new ViewInstruction (I);
    VI->TmpResult = TR;
    VI->constValue = cv;
    return VI;
  }

class ViewType
{

  std::string Id;                 // Unique name of this report.
  std::string *Metric_Name;       // This view requires these metrics.
  std::string *Collector_Name;    // This view requires these collectors.
  bool Requires_Exp;              // This view is only meaninful with an experiment.

 public:
  // The call to generate the view must have this form.
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    {
    	std::string s("The requested view has not been implemented.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
    }
    return false;
  }

 private:
  ViewType() {}

 public:
  ViewType (std::string viewname,
            std::string BriefD,
            std::string ShortD,
            std::string LongD,
            std::string ExampleD,
            std::string *Metric,
            std::string *Collect,
            bool NeedsExp = true) {
    Id = viewname;
    Metric_Name          = Metric;
    Collector_Name       = Collect;
    Requires_Exp         = NeedsExp;

   // Send information to the message czar.
    SS_Message_Czar& czar = theMessageCzar();

    SS_Message_Element element;

   // Set keyword. You really need to have a keyword.
    element.set_keyword(viewname);

   // This message is not associated with an error number so we won't set this field
   //element.set_id(p_element->errnum);

   // Related keywords
    element.add_related("expView");

   // General topic
    element.set_topic("viewType");

   // Brief, one line description
    element.set_brief(BriefD);

   // More than one line description
    element.add_normal(BriefD + ":\n\n" + ShortD + " " + LongD);

   // A wordy explaination
    element.add_verbose(BriefD + ":\n\n" + ShortD + " " + LongD);

   // Standard example for views.
    element.add_example(ExampleD);

   // Submit the message to the database
    czar.Add_Help(element);
  }
  std::string Unique_Name() { return Id; }
  std::string *Metrics() { return Metric_Name; }
  std::string *Collectors() { return Collector_Name; }
  bool        Need_Exp() { return Requires_Exp; }
};


extern std::list<ViewType *> Available_Views;
void Define_New_View (ViewType *vnew);
bool Generic_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                   ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                   std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                   std::list<CommandResult *>& view_output);

enum View_Form_Category {
      VFC_Unknown,
      VFC_Trace,
      VFC_Function,
      VFC_CallStack,
      VFC_LinkedObject,
      VFC_Statement,
      VFC_Loop,
};

// Reserved locations in the std::vector<CommandResult *> of c_items argument to Generic_Multi_View
#define VMulti_sort_temp 0
#define VMulti_time_temp 1
#define VMulti_free_temp 2

bool Generic_Multi_View (
           CommandObject *cmd, ExperimentObject *exp, int64_t topn,
           ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
           std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
           View_Form_Category vfc,
           std::vector<std::pair<CommandResult *,
                                 SmartPtr<std::vector<CommandResult *> > > >& c_items,
           std::list<CommandResult *>& view_output);

CommandResult *Init_Collector_Metric (CommandObject *cmd,
                                      Collector collector,
                                      std::string metric);
CommandResult *Get_Total_Metric (CommandObject *cmd,
                                 ThreadGroup tgrp,
                                 Collector collector,
                                 std::string metric);

void GetMetricByObjectSet (CommandObject *cmd,
                            ExperimentObject *exp,
                            ThreadGroup& tgrp,
                            Collector& collector,
                            std::string& metric,
                            std::set<Function>& objects,
                            SmartPtr<std::map<Function, CommandResult *> >& items);
void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Statement>& objects,
                           SmartPtr<std::map<Statement, CommandResult *> >& items);
void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<LinkedObject>& objects,
                           SmartPtr<std::map<LinkedObject, CommandResult *> >& items);
void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Loop>& objects,
                           SmartPtr<std::map<Loop, CommandResult *> >& items);

bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Function>& objects,
                       std::vector<SmartPtr<std::map<Function, CommandResult *> > >& Values);
bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Statement>& objects,
                       std::vector<SmartPtr<std::map<Statement, CommandResult *> > >& Values);
bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<LinkedObject>& objects,
                       std::vector<SmartPtr<std::map<LinkedObject, CommandResult *> > >& Values);
bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Loop>& objects,
                       std::vector<SmartPtr<std::map<Loop, CommandResult *> > >& Values);


template <typename TE>
CommandResult *Get_Object_Metric (CommandObject *cmd,
                                  ExperimentObject *exp,
                                  TE F,
                                  ThreadGroup tgrp,
                                  Collector collector,
                                  std::string metric) {
  std::set<TE> objects; objects.insert (F);
  SmartPtr<std::map<TE, CommandResult *> > items =
             Framework::SmartPtr<std::map<TE, CommandResult *> >(
                new std::map<TE, CommandResult * >()
                );;
  GetMetricByObjectSet (cmd, exp, tgrp, collector ,metric ,objects, items);
  return (items->begin() != items->end()) ? (*(items->begin())).second : NULL;
}


void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<LinkedObject>& objects );

void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<Function>& objects );

void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<Statement>& objects );

void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<Loop>& objects );


ViewType *Find_View (std::string viewname);
bool Collector_Generates_Metrics (Collector C, std::string *Metric_List);
std::string Find_Collector_With_Metrics (CollectorGroup cgrp,
                                         std::string *Metric_List);
bool Collector_Generates_Metric (Collector C, std::string Metric_Name);
std::string Find_Collector_With_Metric (CollectorGroup cgrp,
                                        std::string Metric_Name);
bool Metadata_hasName (Collector C, std::string name);
Metadata Find_Metadata (Collector C, std::string name);
ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>& IV, int64_t Column);
int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>& IV);
int64_t Find_Max_Temp (std::vector<ViewInstruction *>& IV);
int64_t Find_Max_ExtraMetrics (std::vector<ViewInstruction *>& IV);

bool Select_User_Metrics (CommandObject *cmd, ExperimentObject *exp,
                          std::vector<Collector>& CV, std::vector<std::string>& MV,
                          std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV);
void Print_View_Params (std::ostream &to,
                        std::vector<Collector>& CV,
                        std::vector<std::string>& MV,
                        std::vector<ViewInstruction *>& IV);

void Add_Column_Headers (CommandResult_Headers *H, std::string *column_titles);
void Add_Header (CommandObject *cmd, std::string *column_titles);

void SS_Init_BuiltIn_Views ();
void SS_Load_View_plugins ();
void SS_Remove_View_plugins ();
void SS_Get_Views (CommandObject *cmd);
void SS_Get_Views (CommandObject *cmd, std::string collector_name);
void SS_Get_Views (CommandObject *cmd, OpenSpeedShop::Framework::Experiment *fexp, std::string s = "");

bool Validate_V_Options(CommandObject *cmd, std::string allowed[]);
View_Form_Category Determine_Form_Category (CommandObject *cmd);
bool Determine_TraceBack_Ordering (CommandObject *cmd);
int64_t Determine_ByThread_Id (ExperimentObject *exp, CommandObject *cmd);
std::vector<CommandResult *> *
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st);
std::vector<CommandResult *> *
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st,
                           std::map<Address, CommandResult *>& knownTraces);
bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname);

void Construct_View_Output (CommandObject *cmd,
                            ExperimentObject *exp,
                            ThreadGroup& tgrp,
                            std::vector<Collector>& CV,
                            std::vector<std::string>& MV,
                            std::vector<ViewInstruction *>& IV,
                            std::vector<CommandResult *>& Total_Value,
                            std::vector<std::pair<CommandResult *,
                                                  SmartPtr<std::vector<CommandResult *> > > >& items,
                            std::list<CommandResult *>& view_result );

inline std::string lowerstring (std::string A)
{
  const char *old_name = A.c_str();
  int64_t old_len = strlen(old_name);
  std::string new_name;
  for (int64_t cix=0; cix<old_len; cix++) {
    new_name += tolower(old_name[cix]);
  }
  return new_name;
}
