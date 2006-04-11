/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

#include "Queries.hxx"

enum ViewOpCode {
     VIEWINST_Define_Total,
     VIEWINST_Display_Metric,
     VIEWINST_Display_Tmp,
     VIEWINST_Display_Percent_Column,
     VIEWINST_Display_Percent_Metric,
     VIEWINST_Display_Percent_Tmp,      // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with numerator.
                                        // TMP_index2 is row_tmp# with denominator.
     VIEWINST_Display_Average_Tmp,      // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with numerator.
                                        // TMP_index2 is row_tmp# with denominator.
     VIEWINST_Display_StdDeviation_Tmp, // TmpResult is column# to display in.
                                        // TMP_index1 is row_tmp# with sums.
                                        // TMP_index2 is row_tmp# with sum of squares
     VIEWINST_Display_Summary,          // Generate column summary.
     VIEWINST_Sort_Ascending,           // if TMP_index1 is not 0, sort final report in ascending order
     VIEWINST_Add,                      // TMP_index1 is predefined temp# combined with '+' op. 
     VIEWINST_Min,                      // TMP_index1 is predefined temp# combined with 'min' op.
     VIEWINST_Max,                      // TMP_index1 is predefined temp# combined with 'max' op.
     VIEWINST_Summary_Max,              // TMP_index1 is predefined temp# combined with 'max' op.
};

class ViewInstruction
{
 private:
  ViewOpCode Instruction;
  int64_t TmpResult;  // result temp or column number
  int64_t TMP_index1; // index of Collector::Metric vectors or temp
  int64_t TMP_index2; // index of temp
  int64_t TMP_index3; // index of temp

 public:
  ViewInstruction (ViewOpCode I) {
    Instruction = I;
    TmpResult = -1;
    TMP_index1 = -1;
    TMP_index2 = -1;
    TMP_index3 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR) {
    Instruction = I;
    TmpResult = -1;
    TMP_index1 = TR;
    TMP_index2 = -1;
    TMP_index3 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1) {
    Instruction = I;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = -1;
    TMP_index3 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1, int64_t TMP2) {
    Instruction = I;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = TMP2;
    TMP_index3 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1, int64_t TMP2, int64_t TMP3) {
    Instruction = I;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = TMP2;
    TMP_index3 = TMP3;
  }
  ViewOpCode OpCode () { return Instruction; }
  int64_t TR () { return TmpResult; }
  int64_t TMP1 () { return TMP_index1; }
  int64_t TMP2 () { return TMP_index2; }
  int64_t TMP3 () { return TMP_index3; }

  void Print (ostream &to) {
    std::string op;
    switch (Instruction) {
     case VIEWINST_Define_Total: op = "Define_Total"; break;
     case VIEWINST_Display_Metric: op = "Display_Metric"; break;
     case VIEWINST_Display_Tmp: op = "Display_Tmp"; break;
     case VIEWINST_Display_Percent_Column: op = "Display_Percent_Column"; break;
     case VIEWINST_Display_Percent_Metric: op = "Display_Percent_Metric"; break;
     case VIEWINST_Display_Percent_Tmp: op = "Display_Percent_Tmp"; break;
     case VIEWINST_Display_Average_Tmp: op = "Display_Average_Tmp"; break;
     case VIEWINST_Display_StdDeviation_Tmp: op = "Display_StdDeviation_Tmp"; break;
     case VIEWINST_Display_Summary: op = "Display_Summary"; break;
     case VIEWINST_Sort_Ascending: op = "Ascending_Sort"; break;
     case VIEWINST_Add: op = "Add"; break;
     case VIEWINST_Min: op = "Min"; break;
     case VIEWINST_Max: op = "Max"; break;
     case VIEWINST_Summary_Max: op = "Summary_Max"; break;
     default: op ="(unknown)"; break;
    }
    to << op << " " << TmpResult  << " "
                    << TMP_index1 << " "
                    << TMP_index2 << " "
                    << TMP_index3 << std::endl;
  }
};

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


template <typename TO, typename TS>
void GetMetricInThreadGroup(
    const Collector& collector,
    const std::string& metric,
    const ThreadGroup& tgrp,
    const std::set<TO >& objects,
    SmartPtr<std::map<TO, TS > >& result)
{
    // Allocate (if necessary) a new map of source objects to values
    if(result.isNull())
        result = SmartPtr<std::map<TO, TS > >(new std::map<TO, TS >());
    Assert(!result.isNull());

    // Get the summation reduced metric values
    SmartPtr<std::map<TO, std::map<Thread, TS > > > individual;
    Queries::GetMetricValues(collector, metric,
	                     TimeInterval(Time::TheBeginning(), Time::TheEnd()),
			     tgrp, objects, individual);
    SmartPtr<std::map<TO, TS > > reduced =
	Queries::Reduction::Apply(individual, Queries::Reduction::Summation);
    individual = SmartPtr<std::map<TO, std::map<Thread, TS > > >();

    // Merge the temporary reduction into the actual results
    for(typename std::map<TO, TS >::const_iterator
	    i = reduced->begin(); i != reduced->end(); ++i)
	if(result->find(i->first) == result->end())
	    result->insert(std::make_pair(i->first, i->second));
	else
	    (*result)[i->first] += i->second;
}


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
};

// Reserved locations in the std::vector<CommandResult *> of c_items argument to Generic_Multi_View
#define VMulti_sort_temp 0
#define VMulti_time_temp 1

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
                            ThreadGroup& tgrp,
                            Collector& collector,
                            std::string& metric,
                            std::set<Function>& objects,
                            SmartPtr<std::map<Function, CommandResult *> >& items);
void GetMetricByObjectSet (CommandObject *cmd,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Statement>& objects,
                           SmartPtr<std::map<Statement, CommandResult *> >& items);
void GetMetricByObjectSet (CommandObject *cmd,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<LinkedObject>& objects,
                           SmartPtr<std::map<LinkedObject, CommandResult *> >& items);

template <typename TE>
CommandResult *Get_Object_Metric (CommandObject *cmd,
                                  TE F,
                                  ThreadGroup tgrp,
                                  Collector collector,
                                  std::string metric) {
  std::set<TE> objects; objects.insert (F);
  SmartPtr<std::map<TE, CommandResult *> > items =
             Framework::SmartPtr<std::map<TE, CommandResult *> >(
                new std::map<TE, CommandResult * >()
                );;
  GetMetricByObjectSet (cmd, tgrp, collector ,metric ,objects, items);
  return (items->begin() != items->end()) ? (*(items->begin())).second : NULL;
}


void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<LinkedObject>& objects );

void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<Function>& objects );

void Get_Filtered_Objects (CommandObject *cmd, ExperimentObject *exp,
                           ThreadGroup& tgrp, std::set<Statement>& objects );


ViewType *Find_View (std::string viewname);
bool Collector_Generates_Metrics (Collector C, std::string *Metric_List);
std::string Find_Collector_With_Metrics (CollectorGroup cgrp,
                                         std::string *Metric_List);
bool Collector_Generates_Metric (Collector C, std::string Metric_Name);
std::string Find_Collector_With_Metric (CollectorGroup cgrp,
                                        std::string Metric_Name);
bool Metadata_hasName (Collector C, std::string name);
Metadata Find_Metadata (Collector C, std::string name);
ViewInstruction *Find_Base_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Total_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Percent_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>IV, int64_t Column);
int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>IV);
int64_t Find_Max_Temp (std::vector<ViewInstruction *>IV);
bool Select_User_Metrics (CommandObject *cmd, ExperimentObject *exp,
                          std::vector<Collector>& CV, std::vector<std::string>& MV,
                          std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV);
void Print_View_Params (ostream &to,
                        std::vector<Collector> CV,
                        std::vector<std::string> MV,
                        std::vector<ViewInstruction *>IV);

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
SmartPtr<std::vector<CommandResult *> >
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st);
SmartPtr<std::vector<CommandResult *> >
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st,
                           std::map<Address, CommandResult *>& knownTraces);
bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname);

void Construct_View_Output (CommandObject *cmd,
                            ThreadGroup& tgrp,
                            std::vector<Collector>& CV,
                            std::vector<std::string>& MV,
                            std::vector<ViewInstruction *>& IV,
                            int64_t num_columns,
                            bool Gen_Total_Percent,
                            int64_t percentofcolumn,
                            CommandResult *TotalValue,
                            std::vector<std::pair<CommandResult *,
                                                  SmartPtr<std::vector<CommandResult *> > > >& items,
                            std::list<CommandResult *>& view_result );
