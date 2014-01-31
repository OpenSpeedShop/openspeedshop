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

#include "SS_Input_Manager.hxx"

#include "Python.h"


#include "SS_Parse_Result.hxx"
#include "SS_Parse_Target.hxx"

using namespace OpenSpeedShop::cli;

// Global data definition
std::list<ViewType *> Available_Views;

// Local utilities

static int64_t Get_Trailing_Int (std::string viewname, int64_t start) {
  int64_t topn = 0;
  for (int i = start; i < viewname.length(); i++) {
    char c = viewname[i];
    if ((c >= *"0") && (c <= *"9")) {
      topn = (topn * 10) + (c - *"0");
    }
  }
  return topn;
}

static int64_t Get_Trailing_Int (char *S, int64_t start, int64_t length) {
  int64_t val = 0;
  for (int i = start; i < start+length; i++) {
    char c = S[i];
    if ((c >= *"0") && (c <= *"9")) {
      val = (val * 10) + (c - *"0");
    }
  }
  return val;
}

bool SS_Find_Previous_View (CommandObject *cmd, ExperimentObject *exp)
{
  if (!OPENSS_SAVE_VIEWS_FOR_REUSE) return false;

 // Is there a good chance that a matching view has been saved?
//  if (exp->Status() == ExpStatus_Paused) return false;
  if (exp->Status() == ExpStatus_Running) return false;
  if (exp->FW() == NULL) return false;
 // Use a time stamp to convience us that the database we are told
 // to look at is the one used to generate the view.
  Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
  Time EndTime = databaseExtent.getTimeInterval().getEnd();

  std::string previous_view("");

  InputLineObject *clip = cmd->Clip();
  if (clip == NULL) return false;
  std::string command = clip->Command();

  std::string Data_File_Name;
  Data_File_Name = exp->FW()->getName();
  if (Data_File_Name.length() == 0) return false;

 // Determine format information.
  std::string eoc_marker = OPENSS_VIEW_EOC;
  std::string eol_marker = OPENSS_VIEW_EOL;
  if ( (cmd->P_Result() != NULL) &&
       (cmd->P_Result()->getexpFormatList() != NULL) ) {
    std::vector<ParseRange> *f_list = cmd->P_Result()->getexpFormatList();
    ParseRange *key_range = Look_For_Format_Specification ( f_list, "viewEOC");
    if ( (key_range  != NULL) &&
         (key_range->getRange()->is_range) &&
         (key_range->getRange()->end_range.tag == VAL_STRING) ) {
      eoc_marker = key_range->getRange()->end_range.name;
    }
    key_range = Look_For_Format_Specification ( f_list, "viewEOL");
    if ( (key_range  != NULL) &&
         (key_range->getRange()->is_range) &&
         (key_range->getRange()->end_range.tag == VAL_STRING) ) {
      eol_marker = key_range->getRange()->end_range.name;
    }
  }

 // Isolate the important parts of the command.
  std::string cmdstr = clip->Command();
  std::string viewcmd("expview ");
  char *cidx = strcasestr( (char *)cmdstr.c_str(), (char *)viewcmd.c_str() );
  if (cidx != 0) cmdstr = std::string( cidx );
 // Remove any ending '\n'.
  cidx = (char *)cmdstr.c_str();
  for (int shorten_to = cmdstr.length()-1; shorten_to != 0; shorten_to--) {
    if (cidx[shorten_to] == *("\n")) {
      cmdstr = cmdstr.substr( 0, shorten_to);
      break;
    }
  }
 // Remove any redirect-output specifier.
  cidx = (char *)cmdstr.c_str();
  for (int shorten_to = cmdstr.length()-1; shorten_to != 0; shorten_to--) {
    if (cidx[shorten_to] == *(">")) {
      if ( (shorten_to > 0) &&
           (cidx[shorten_to] == *(">")) ) {
        shorten_to--;
      }
      cmdstr = cmdstr.substr( 0, shorten_to);
      break;
    }
  }
 // Remove trailing blanks.
  cidx = (char *)cmdstr.c_str();
  for (int shorten_to = cmdstr.length()-1; shorten_to != 0; shorten_to--) {
    if (cidx[shorten_to] != *(" ")) {
      if (shorten_to != (cmdstr.length()-1)) {
        cmdstr = cmdstr.substr( 0, shorten_to);
      }
      break;
    }
  }
 // Remove '-x' specifier.
  cidx = (char *)cmdstr.c_str();
  int prior_end_of_field = 0;
  for (int xat = 0; xat < cmdstr.length()-1; xat++) {
    if (cidx[xat] != *(" ")) prior_end_of_field++;
    if ( strncasecmp( &cidx[xat], "-x", 2 ) == 0) {
     //  Asssume that parsing has handled any errors
     //  and that there is exactly 1 experiment id specified.
      bool looking_for_start_of_field = true;
      bool found_end_of_field = false;
      int toc = xat+2;
      for ( ; toc < cmdstr.length()-1; toc++) {
        if (cidx[toc] == *(" ")) {
          if (!looking_for_start_of_field) {
            found_end_of_field = true;
          }
        } else if (cidx[toc] == *(",")) {
          if (!looking_for_start_of_field) {
           // Multiple experiments are ot handled.
            return false;
          }
        } else {
          if (found_end_of_field) break;
          looking_for_start_of_field = false;
        }
      }
      cmdstr = cmdstr.substr( 0, prior_end_of_field) + cmdstr.substr( toc, cmdstr.length()-1);
    }
  }

 // Look for an existing output that was generated with the same command.
  savedViewInfo *use_ViewInfo = exp->FindExisting_savedViewInfo (eoc_marker, eol_marker, cmdstr);
  if (use_ViewInfo != NULL) {
    cmd->setSaveResultViewInfo(use_ViewInfo);
    cmd->setSaveResultFile(use_ViewInfo->FileName());
    cmd->setSaveResult(false);
    cmd->setSaveResultDataOffset( use_ViewInfo->file_offset_to_data() );
    cmd->setSaveEoc( eoc_marker );
    cmd->setSaveEol( eol_marker );
    return true;
  }

 // There was no previously generated report that we could reuse.
 // Need to create a new file to save view.
  savedViewInfo *new_ViewInfo = exp->GetFree_savedViewInfo();
  if (new_ViewInfo != NULL) {
   // Generate and save the header descriptor in binary form.
    save_file_header H;
    H.type = 2;
    H.command_offset = sizeof(H) + 1;
    H.eoc_offset = H.command_offset + cmdstr.length() + 1;
    H.eol_offset = H.eoc_offset + eoc_marker.length() + 1;
    H.data_offset = H.eol_offset + eol_marker.length() + 1;
    H.last_sample = EndTime;
    H.generation_time = 0; // Added after generation.
    new_ViewInfo->setHeader(H, eoc_marker, eol_marker, cmdstr);
   // Preserve file name and format information in the command
   // for immediate display of the report after it has been saved.
    cmd->setSaveResultViewInfo(new_ViewInfo);
    cmd->setSaveResultFile(new_ViewInfo->FileName());
    cmd->setSaveResult(true);
    cmd->setSaveEoc( eoc_marker );
    cmd->setSaveEol( eol_marker );
    cmd->setSaveResultDataOffset(H.data_offset);
   // Leave file open to receive output of `expView` command.
    return true;
  }
  return false;
}

// Supporting semantic routines

void SS_Get_Views (CommandObject *cmd) {
 // List all known views.
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    cmd->Result_String ( vt->Unique_Name() );
  }
}

void SS_Get_Views (CommandObject *cmd, std::string collector_name) {
 // List all views that require a specific collector.
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    std::string *Collector_List = vt->Collectors();
    std::string *Metric_List = vt->Metrics();
    bool all_collectors_available = false;

    if (Collector_List[0].length() > 0) {
     // Check that all the required collectors are available.
      std::string cs = Collector_List[0];
      all_collectors_available = true;

      while (cs.length() != 0) {
        if ( strcasecmp (cs.c_str(), collector_name.c_str()) ) {
          all_collectors_available = false;
          break;
        }
        cs = *(++Collector_List);
      }

    }

    if (!all_collectors_available &&
        (Metric_List[0].length() > 0)) {
     // Look for an available collector that produces the required metrics.
      try {
        std::string tmpdb = std::string("./ssdbtmpview.openss");
        OpenSpeedShop::Framework::Experiment::create (tmpdb);
        OpenSpeedShop::Framework::Experiment *fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);
        Collector C = Get_Collector (fw_exp, collector_name);
        if (Collector_Generates_Metrics ( C, &Metric_List[0] )) {
          all_collectors_available = true;
        }
        delete fw_exp;
        (void) remove (tmpdb.c_str());
      }
      catch(const Exception& error) {
       // Guess not.
        all_collectors_available = false;
      }
    }

    if (all_collectors_available) {
        cmd->Result_String ( vt->Unique_Name() );
    }
  }
  
}

void SS_Get_Views (CommandObject *cmd, OpenSpeedShop::Framework::Experiment *fexp, std::string s) {
 // List all views that can be generate from the collectors used in an experiment.
 // Skip views that don't depend on any collector.
  CollectorGroup cgrp;
  try {
    cgrp = fexp->getCollectors();

    std::list<ViewType *>::iterator vi;
    for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
     // All the required collectors must exist in the CollectorGroup.
      ViewType *vt = (*vi);
      std::string *Collector_List = vt->Collectors();
      std::string *Metric_List = vt->Metrics();
      bool all_collectors_available = false;

      if (Collector_List[0].length() != 0) {
       // Check that all the required collectors are available.
        std::string C = Collector_List[0];
        all_collectors_available = true;

        while (C.length() != 0) {
          bool required_collector_found = false;
          CollectorGroup::iterator ci;

          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata m = c.getMetadata();
            std::string collector_name = m.getUniqueId();
            if ( !strcasecmp (C.c_str(), collector_name.c_str()) ) {
              required_collector_found = true;
              break;
            }
          }

          if (required_collector_found) {
            C = *(++Collector_List);
            continue;
          } else {
            all_collectors_available = false;
            break;
          }
        }

      }

      if (all_collectors_available) {
        cmd->Result_String ( s + vt->Unique_Name() );
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }
  
}

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname) {
 
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  Assert(p_result != NULL);

#if DEBUG_CLI
  printf("Enter SS_Generate_View in SS_View.cxx, viewname.c_str()=%s\n", viewname.c_str());
#endif

 // Determine the availability of the view.
  ViewType *vt = Find_View (viewname);
  if (vt == NULL) {
    std::string s("The requested view is unavailable.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

 // Check for a required experiment.
  if (vt->Need_Exp()) {
    if (exp == NULL) {
     // The requested view requires an ExperimentObject.
      std::string s("An Experiment has not been specified.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    } else if (exp->FW() == NULL) {
     // There should always be a link to the FrameWork.
      std::string s("The experiment has been disconnected from the FrameWork.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
  }

  std::string *Metric_List = vt->Metrics();
  std::vector<ParseRange> *p_slist = p_result->getexpMetricList();

  if (p_slist->empty()) {

   // Use the metrics specified in the experiment definition.
   // Check to be sure we have a chance of generating the view.

    bool collector_found = false;
    std::string *Collector_List = vt->Collectors();
    std::string C_Name = Collector_List[0];

    if ((Collector_List[0].length() != 0) &&
        (Collector_List[1].length() == 0)) {

     // There is a single, required collector.
       C_Name = vt->Unique_Name();
       collector_found = true;

    } else if ((Metric_List[0].length() > 0) &&
               (exp != NULL) &&
               (exp->FW() != NULL)) {

     // Look for a collector that produces the required metrics.

      CollectorGroup cgrp = exp->FW()->getCollectors();
      C_Name = Find_Collector_With_Metrics (cgrp, &Metric_List[0]);
      collector_found = (C_Name.length() > 0);

    } else {

     // We handle multiple collectors by letting the View figure out what to do.
     // This also includes Views that require no collectors.

    }

    if (vt->Need_Exp() &&
        ((Collector_List[0].length() != 0) ||
         (Metric_List[0].length() != 0))) {
      if (!collector_found) {
    	std::string s("The metrics required to generate the view are not available in the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
        return false;
      }
      if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
    	std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
        return false;
      }
    }

  }

 // Determine threads that are required.
  ThreadGroup tgrp;
  if ((exp != NULL) &&
      (exp->FW() != NULL)) {
    tgrp = exp->FW()->getThreads();
  }
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

#if DEBUG_CLI
  printf("In SS_Generate_View in SS_View.cxx, after calling Filter_ThreadGroup\n");
#endif

 // Look for a saved view.
  savedViewInfo *svi = NULL;
  if ( OPENSS_SAVE_VIEWS_FOR_REUSE &&
       SS_Find_Previous_View( cmd, exp ) ) {

#if DEBUG_CLI
    printf("In SS_Generate_View in SS_View.cxx, reuse view from file: %s\n",
           cmd->SaveResultFile().c_str());
#endif

    if (!cmd->SaveResult()) {
     // Previously generated output file found with requested view.
      return true;
    }

   // An existing saved view is not available but provision
   // has been made to create a new one.
    svi = cmd->SaveResultViewInfo();
    if (svi != NULL) {
     // Set StartTIme to measure how long it takes to generate the view.
      svi->setStartTime();
    }
  }

#if DEBUG_CLI
  printf("In SS_Generate_View in SS_View.cxx, before calling vt->GenerateView\n");
#endif

 // Try to Generate the Requested View!
  bool success = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                                   tgrp, cmd->Result_List());

 // Set EndTIme if saving info after generation.
  svi = cmd->SaveResultViewInfo();
  if (svi != NULL) {
    if (success) {
      svi->setEndTime();
      std::ostream *tof = svi->writeHeader ();
      cmd->setSaveResultOstream( tof );
    } else {
      svi->setDoNotSave();
    }
  }

#if DEBUG_CLI
  printf("In SS_Generate_View in SS_View.cxx, after calling vt->GenerateView\n");
#endif

#if DEBUG_CLI
  printf("Exit SS_Generate_View in SS_View.cxx\n");
#endif
  return success;
}

// Initialize definitions of the predefined views.
extern "C" void stats_LTX_ViewFactory ();
void SS_Load_View_plugins ();

void SS_Init_BuiltIn_Views () {
  stats_LTX_ViewFactory ();  // This is the generic, built-in view
}
