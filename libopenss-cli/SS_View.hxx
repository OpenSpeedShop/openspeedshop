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

enum ViewOpCode {
     VIEWINST_Define_Total,
     VIEWINST_Display_Metric,
     VIEWINST_Display_Tmp,
     VIEWINST_Display_Percent_Column,
     VIEWINST_Display_Percent_Metric,
     VIEWINST_Display_Percent_Tmp
};

class ViewInstruction
{
 private:
  ViewOpCode Instruction;
  int64_t TmpResult;  // result temp or column number
  int64_t TMP_index1; // index of Collector::Metric vectors or temp
  int64_t TMP_index2; // index of temp

 public:
  ViewInstruction (ViewOpCode I, int64_t TR) {
    Instruction = I;
    TmpResult = -1;
    TMP_index1 = TR;
    TMP_index2 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1) {
    Instruction = I;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = -1;
  }
  ViewInstruction (ViewOpCode I, int64_t TR, int64_t TMP1, int64_t TMP2) {
    Instruction = I;
    TmpResult = TR;
    TMP_index1 = TMP1;
    TMP_index2 = TMP2;
  }
  ViewOpCode OpCode () { return Instruction; }
  int64_t TR () { return TmpResult; }
  int64_t TMP1 () { return TMP_index1; }
  int64_t TMP2 () { return TMP_index2; }

  void Print (FILE *TFile) {
    std::string op;
    switch (Instruction) {
     case VIEWINST_Define_Total: op = "Define_Total"; break;
     case VIEWINST_Display_Metric: op = "Display_Metric"; break;
     case VIEWINST_Display_Tmp: op = "Display_Tmp"; break;
     case VIEWINST_Display_Percent_Column: op = "Display_Percent_Column"; break;
     case VIEWINST_Display_Percent_Metric: op = "Display_Percent_Metric"; break;
     case VIEWINST_Display_Percent_Tmp: op = "Display_Percent_Tmp"; break;
     default: op ="(unknown)"; break;
    }
    fprintf(TFile,"%s %lld %lld %lld\n",op.c_str(),TmpResult,TMP_index1,TMP_index2);
  }
};

class ViewType
{

  std::string Id;                 // Unique name of this report.
  std::string Brief_Description;  // What is the meaning of each column?
  std::string Short_Description;  // What type of report is generated?
  std::string Long_Description;   // When would I want to use this?
  std::string *Metric_Name;       // This view requires these metrics.
  std::string *Collector_Name;    // This view requires these collectors.
  std::string *Headers_Name;      // The names associated with each column.
  bool Requires_Exp;              // This view is only meaninful with an experiment.
  bool PreDetermine_Metrics;      // If false, the View will deterimne what is needed.

 public:
  // The call to generate the view must have this form.
  // topn will be 0, if there is no integer on the right of the view name.
  // useCollector will be a dummy string, if none is specified.
  // virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp,
  //                            int64_t topn, std::string useCollector) {
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                             std::vector<ViewInstruction *>IV) {
    cmd->Result_String ("The requested view has not been implemented.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 private:
  ViewType() {}

 public:
  ViewType (std::string viewname,
            std::string BriefD,
            std::string ShortD,
            std::string LongD,
            std::string *Metric,
            std::string *Collect,
            std::string *Head,
            bool NeedsExp = true,
            bool DeterimneMetrics = true) {
    Id = viewname;
    Brief_Description    = BriefD;
    Short_Description    = ShortD;
    Long_Description     = LongD;
    Metric_Name          = Metric;
    Collector_Name       = Collect;
    Headers_Name         = Head;
    Requires_Exp         = NeedsExp;
    PreDetermine_Metrics = DeterimneMetrics;

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
    element.add_example("expView " + viewname);

   // Submit the message to the database
    czar.Add_Help(element);
  }
  std::string Unique_Name() { return Id; }
  std::string Brief_Name() { return Brief_Description; }
  std::string Short_Name() { return Short_Description; }
  std::string Long_Name() { return Long_Description; }
  std::string *Metrics() { return Metric_Name; }
  std::string *Collectors() { return Collector_Name; }
  std::string *Headers() { return Headers_Name; }
  bool        Need_Exp() { return Requires_Exp; }
  bool        Deterimne_Metrics() { return PreDetermine_Metrics; }
};


extern std::list<ViewType *> Available_Views;
void Define_New_View (ViewType *vnew);
bool Generic_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                   ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                   std::vector<ViewInstruction *>IV, std::string *HV = NULL);

typedef std::pair<Function, CommandResult *> Function_CommandResult_pair;
typedef std::pair<Function, double> Function_double_pair;
CommandResult *Init_Collector_Metric (CommandObject *cmd,
                                      Collector collector,
                                      std::string metric);
CommandResult *Get_Function_Metric (CommandObject *cmd,
                                    Function F,
                                    ThreadGroup tgrp,
                                    Collector collector,
                                    std::string metric);
CommandResult *Get_Total_Metric (CommandObject *cmd,
                                 ThreadGroup tgrp,
                                 Collector collector,
                                 std::string metric);
void GetMetricByFunction (CommandObject *cmd,
                          bool ascending_sort,
                          ThreadGroup tgrp,
                          Collector C,
                          std::string metric,
                          std::vector<Function_CommandResult_pair>& items);
void GetMetricByFunctionSet (CommandObject *cmd,
                             ThreadGroup& tgrp,
                             Collector& collector,
                             std::string& metric,
                             std::set<Function>& objects,
                             SmartPtr<std::map<Function, CommandResult *> >& items);
ViewType *Find_View (std::string viewname);
bool Collector_Generates_Metrics (Collector C, std::string *Metric_List);
std::string Find_Collector_With_Metrics (CollectorGroup cgrp,
                                         std::string *Metric_List);
bool Collector_Generates_Metric (Collector C, std::string Metric_Name);
std::string Find_Collector_With_Metric (CollectorGroup cgrp,
                                        std::string Metric_Name);
bool Metadata_hasName (Collector C, std::string name);
Metadata Find_Metadata (Collector C, std::string name);
CommandResult *gen_F_name (Function F);
ViewInstruction *Find_Base_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Total_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Percent_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>IV, int64_t Column);
int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>IV);
void Print_View_Params (FILE *TFile,
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

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname);
