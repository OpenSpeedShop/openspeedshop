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
     VIEWINST_Display_Percent_Tmp,
     VIEWINST_Display_Average_Tmp
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
     default: op ="(unknown)"; break;
    }
    to << op << " " << TmpResult << " " << TMP_index1 << " " << TMP_index2 << std::endl;
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
#if 1
    {
    	std::string s("The requested view has not been implemented.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
    }
#else
    cmd->Result_String ("The requested view has not been implemented.");
    cmd->set_Status(CMD_ERROR);
#endif
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
                   std::vector<ViewInstruction *> IV, std::string *HV = NULL);

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

template <typename TE>
bool Cmp_F2Object (std::string F_Name, TE object) {
  if (F_Name.size() == 0) return false;
  int64_t len = F_Name.size();
  int64_t pos = F_Name.find( "/" );
  if ((pos >= 0) &&
      (pos < len) &&
      ((pos != 1) || (F_Name[0] != *"."))) {
   // Compare path name.
    return (F_Name == object.getPath());
  } else {
   // Compare basename.
    int64_t dot = F_Name.find( "." );
    if ((dot >= 0) &&
        (dot < len)) {
     // look for exact match.
      return (F_Name == object.getPath().getBaseName());
    } else {
     // Look for partial match.
      std::string baseName = object.getPath().getBaseName();
      int64_t subl = baseName.size();
      int64_t subs = baseName.find (F_Name);
      return ((subs >= 0) && (subs < subl));
    }
  }
}

inline bool Include_Object (std::string F_Name, Thread thread, LinkedObject object) {
 // F_Name can only be a LinkedObject name.
  return Cmp_F2Object (F_Name, object);
}

inline  bool Include_Object (std::string F_Name, Thread thread, Function object) {
 // Is the object in the set of previously selected threads?
  std::set<Thread> tt = object.getThreads();
  if (tt.find(thread) == tt.end()) return false;

 // Is F_Name a function name?
  if (F_Name == object.getName()) return true;

 // Is F_Name a LinkedObject name?
  LinkedObject L = object.getLinkedObject();
  if (Cmp_F2Object (F_Name, L)) return true;

 // Is F_Name a containing file name?
  std::set<Statement> T = object.getDefinitions();
  for (std::set<Statement>::iterator ti = T.begin(); ti != T.end(); ti++) {
    Statement st = *ti;
    std::set<Thread> ts = st.getThreads();
    if (ts.find(thread) == ts.end()) continue;
    std::set<Function> tf = st.getFunctions();
    if (tf.find(object) == tf.end()) continue;
    if (Cmp_F2Object (F_Name, st)) return true;
  }

  return false;
}

inline bool Include_Object (std::string F_Name, Thread thread, Statement object) {
 // Is the object in the set of previously selected threads?
  std::set<Thread> tt = object.getThreads();
  if (tt.find(thread) == tt.end()) return false;

 // Is F_Name a function name?
  std::set<Function> tf = object.getFunctions();
  for (std::set<Function>::iterator fi = tf.begin(); fi != tf.end(); fi++) {
    Function f = *fi;
    std::set<Thread> tt = f.getThreads();
    if (tt.find(thread) == tt.end()) continue;
    if (F_Name == f.getName()) return true;
  }

 // Is F_Name a LinkedObject name?
  LinkedObject L = object.getLinkedObject();
  if (Cmp_F2Object (F_Name, L)) return true;

 // Is F_Name a containing file name?
  if (Cmp_F2Object (F_Name, object)) return true;

  return false;
}

template <typename TE>
void Get_Filtered_Objects (CommandObject *cmd, ThreadGroup& tgrp, std::set<TE >& objects ) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  bool has_f = false;
  OpenSpeedShop::cli::ParseTarget pt;
  vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
  if (p_tlist->begin() != p_tlist->end()) {
    // There is a list.  Is there a "-f" specifier?
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();
    has_f = !((f_list == NULL) || f_list->empty());
  }

  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

   // Check for asnychonous abort command
    if (cmd->Status() == CMD_ABORTED) {
      return;
    }

    Thread thread = *ti;
    std::set<TE> threadObjects;
    OpenSpeedShop::Queries::GetSourceObjects(thread, threadObjects);
    if (has_f) {
     // Only include selected objects.
      bool object_inserted = false;
      vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
      for (typename std::set<TE>::iterator newi = threadObjects.begin();
                // ((newi != threadObjects.end()) && (!object_inserted));
                newi != threadObjects.end();
                newi++) {
        TE new_object = *newi;
        for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
          OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
          OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
          Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
          std::string F_Name = pval1.name;
          if (Include_Object (F_Name, thread, new_object)) {
            objects.insert(new_object);
            object_inserted = true;
            break;
          }
        }
      }
    } else {
     // Include everything.
      objects.insert(threadObjects.begin(), threadObjects.end());
    }
  }
}


ViewType *Find_View (std::string viewname);
bool Collector_Generates_Metrics (Collector C, std::string *Metric_List);
std::string Find_Collector_With_Metrics (CollectorGroup cgrp,
                                         std::string *Metric_List);
bool Collector_Generates_Metric (Collector C, std::string Metric_Name);
std::string Find_Collector_With_Metric (CollectorGroup cgrp,
                                        std::string Metric_Name);
bool Metadata_hasName (Collector C, std::string name);
Metadata Find_Metadata (Collector C, std::string name);
std::string gen_F_name (Function F);
ViewInstruction *Find_Base_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Total_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Percent_Def (std::vector<ViewInstruction *>IV);
ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>IV, int64_t Column);
int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>IV);
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

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname);
