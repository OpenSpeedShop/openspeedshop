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

class ViewType
{

  std::string Id;                 // Unique name of this report.
  std::string Brief_Description;  // What is the meaning of each column?
  std::string Short_Description;  // What type of report is generated?
  std::string Long_Description;   // When would I want to use this?
  std::string *Collector_Name;    // This view requires these collectors.
  std::string *Headers_Name;      // The names associated with each column.
  bool Requires_Exp;              // This view is only meaninful with an experiment.

 public:
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    cmd->Result_String ("The requested view is unavailable.");
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
            std::string *Collect,
            std::string *Head,
            bool NeedsExp = true) {
    Id = viewname;
    Brief_Description = BriefD;
    Short_Description = ShortD;
    Long_Description  = LongD;
    Collector_Name    = Collect;
    Headers_Name      = Head;
    Requires_Exp      = NeedsExp;
  }
  std::string Unique_Name() { return Id; }
  std::string Brief_Name() { return Brief_Description; }
  std::string Short_Name() { return Short_Description; }
  std::string Long_Name() { return Long_Description; }
  std::string *Collectors() { return Collector_Name; }
  std::string *Headers() { return Headers_Name; }
  bool        Need_Exp() { return Requires_Exp; }
};

extern std::list<ViewType *> Available_Views;
ViewType *Find_View (std::string viewname);
void Add_Header (CommandObject *cmd, std::string *column_titles);

inline void Define_New_View (ViewType *vnew) { Available_Views.push_front (vnew); }

void SS_Init_BuiltIn_Views ();
void SS_Get_Views (CommandObject *cmd);
void SS_Get_Views (CommandObject *cmd, std::string collector_name);
void SS_Get_Views (CommandObject *cmd, OpenSpeedShop::Framework::Experiment *fexp);

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname);
