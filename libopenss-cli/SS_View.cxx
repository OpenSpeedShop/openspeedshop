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

// Debug flag for the save and reuse view feature
//#define DEBUG_REUSEVIEWS 1

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

#if DEBUG_REUSEVIEWS
  std::cerr << "Enter SS_Find_Previous_View, OPENSS_SAVE_VIEWS_FOR_REUSE=" << OPENSS_SAVE_VIEWS_FOR_REUSE 
              << " exp->Status()=" << exp->Status() << " cmd=" << cmd << std::endl;
#endif

  if (!OPENSS_SAVE_VIEWS_FOR_REUSE) return false;

//  if (exp->Status() == ExpStatus_Paused) return false;

 // Is there a good chance that a matching view has been saved?
  if (exp->Status() == ExpStatus_Running) return false;
  if (exp->FW() == NULL) return false;


  // Use a time stamp to convince us that the database we are told
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
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_Find_Previous_View, setting eoc_marker=" << eoc_marker << " f_list=" << f_list << std::endl;
#endif
    }
    key_range = Look_For_Format_Specification ( f_list, "viewEOL");
    if ( (key_range  != NULL) &&
         (key_range->getRange()->is_range) &&
         (key_range->getRange()->end_range.tag == VAL_STRING) ) {
      eol_marker = key_range->getRange()->end_range.name;
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_Find_Previous_View, setting eol_marker=" << eol_marker << " f_list=" << f_list << std::endl;
#endif
    }
  }

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, looking for matching views, after potentially setting, eoc_marker=" << eoc_marker 
              << " eol_marker=" << eol_marker << " Data_File_Name=" << Data_File_Name << std::endl;
#endif

 // Isolate the important parts of the command.
  std::string cmdstr = clip->Command();
  std::string viewcmd("expview ");
  char *cidx = strcasestr( (char *)cmdstr.c_str(), (char *)viewcmd.c_str() );

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, looking for matching views, cmdstr.c_str()=" << cmdstr.c_str() 
              << " viewcmd.c_str()=" << viewcmd.c_str() << " Data_File_Name=" << Data_File_Name << std::endl;
#endif

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
        cmdstr = cmdstr.substr( 0, shorten_to+1);
      }
      break;
    }
  }


 // Remove '-x' specifier.
  cidx = (char *)cmdstr.c_str();
  int prior_end_of_field = 0;

  //for (int xat = 0; xat < cmdstr.length()-1; xat++) {
  for (int xat = 0; xat < cmdstr.length(); xat++) {
    if (cidx[xat] != *(" ")) {
        prior_end_of_field++;
    }
    if ( strncasecmp( &cidx[xat], "-x", 2 ) == 0) {
     //  Asssume that parsing has handled any errors
     //  and that there is exactly 1 experiment id specified.
      bool looking_for_start_of_field = true;
      bool found_end_of_field = false;
      int toc = xat+2;
      //for ( ; toc < cmdstr.length()-1; toc++) {
      for ( ; toc < cmdstr.length(); toc++) {
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


 // Remove trailing blanks.
  cidx = (char *)cmdstr.c_str();
  for (int shorten_to = cmdstr.length()-1; shorten_to != 0; shorten_to--) {
    if (cidx[shorten_to] != *(" ")) {
      if (shorten_to != (cmdstr.length()-1)) {
        cmdstr = cmdstr.substr( 0, shorten_to+1);
      }
      break;
    }
  }

 //
 // Look first in the database to see if there is a view that already exists.
 // Short circuit the read from file code that is used when first creating a file and reading it to store the data in the database
 // We do not have to do that if it is already in the database.
 // We need the file read code for creating the database view_data field but not for the next session
 // In this code we dummy up the cmd object to make it look somewhat like it would when we find a view in the current session.
 // This code gets activated when we come back into openss for another view session.  If this code is not active, the
 // CLI view code will generate another view because the ViewInfo and command result structures are not set up.  They think
 // they are starting from scratch again.  This code says, we have a view, so don't start over, we found it, so don't generate another.
 //
  std::string view_cmd_in_db;
  std::vector<std::string> savedDbCommands = exp->FW()->Experiment::getSavedCommandList(Data_File_Name);
  std::vector<std::string>::iterator k;
  for (k=savedDbCommands.begin();k != savedDbCommands.end(); k++) {
     view_cmd_in_db = *k;

#if DEBUG_REUSEVIEWS
     std::cerr << "In SS_Find_Previous_View, commands match, cmdstr=" << cmdstr 
               << " view_cmd_in_db.length()=" << view_cmd_in_db.length() 
               << " cmdstr.length()=" << cmdstr.length() 
               << " *k=" << *k << std::endl; 
#endif

     if ( (cmdstr.compare( view_cmd_in_db ) == 0) && cmdstr.length() == view_cmd_in_db.length()) {
       // commands match the command already exists in the database
#if DEBUG_REUSEVIEWS
       std::cerr << "In SS_Find_Previous_View, commands match, found command in database, return true, cmdstr=" << cmdstr << std::endl; 
#endif

       savedViewInfo *new_ViewInfo = exp->GetFree_savedViewInfo();
       save_file_header H;
       H.type = 2;
       new_ViewInfo->setHeader( Data_File_Name, H, eoc_marker, eol_marker, cmdstr);
       cmd->setSaveResultViewInfo(new_ViewInfo);
       cmd->setSaveExpId(exp->ExperimentObject_ID());
       cmd->setSaveResultFile(new_ViewInfo->FileName());
       cmd->setSaveResult(false);
       cmd->setSaveResultDataOffset( 0 );  //not used
       cmd->setSaveEoc( eoc_marker );
       cmd->setSaveEol( eol_marker );

       return true;
     }
  }


#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, after -x removal, cmdstr.c_str()=" << cmdstr.c_str() << std::endl;
  std::cerr << "In SS_Find_Previous_View, looking for matching views AFTER clean-up, " 
            << "  by calling FindExisting_savedViewInfo with, eoc_marker=" << eoc_marker 
            << " eol_marker=" << eol_marker << " cmdstr=" << cmdstr << std::endl;
#endif

 // Look for an existing output that was generated with the same command.
  savedViewInfo *use_ViewInfo = exp->FindExisting_savedViewInfo (eoc_marker, eol_marker, cmdstr);

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, WAS A EXISTING OUTPUTVIEW FOUND by CALLING FindExisting_savedViewInfo?" 
            << "  use_ViewInfo=" << use_ViewInfo << std::endl; 
#endif

  if (use_ViewInfo != NULL) {
    cmd->setSaveResultViewInfo(use_ViewInfo);
    cmd->setSaveExpId(exp->ExperimentObject_ID());
    cmd->setSaveResultFile(use_ViewInfo->FileName());
    cmd->setSaveResult(false);
    cmd->setSaveResultDataOffset( use_ViewInfo->file_offset_to_data() );
    cmd->setSaveEoc( eoc_marker );
    cmd->setSaveEol( eol_marker );

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, YES VIEW was FOUND matching view with, eoc_marker=" << eoc_marker 
            << " eol_marker=" << eol_marker << " cmdstr=" << cmdstr << " use_ViewInfo->file_offset_to_data()=" 
            << use_ViewInfo->file_offset_to_data() << " use_ViewInfo->FileName()=" << use_ViewInfo->FileName() << std::endl;
  std::cerr << "EXIT, VIEW FOUND SS_Find_Previous_View (SS_View.cxx), return TRUE" << std::endl;
#endif

    return true;
  }

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View, Need to create a new file, NO VIEW was FOUND with, eoc_marker=" << eoc_marker 
              << " eol_marker=" << eol_marker << " cmdstr=" << cmdstr << std::endl;
#endif

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

#if DEBUG_REUSEVIEWS
    std::cerr << "In SS_Find_Previous_View (SS_View.cxx), calling setHeader, with eoc_marker=" << eoc_marker 
              << " eol_marker=" << eol_marker << " cmdstr=" << cmdstr << std::endl;
#endif

    new_ViewInfo->setHeader( Data_File_Name, H, eoc_marker, eol_marker, cmdstr);

   // Preserve file name and format information in the command
   // for immediate display of the report after it has been saved.
    cmd->setSaveResultViewInfo(new_ViewInfo);
    cmd->setSaveExpId(exp->ExperimentObject_ID());
    cmd->setSaveResultFile(new_ViewInfo->FileName());
    cmd->setSaveResult(true);
    cmd->setSaveEoc( eoc_marker );
    cmd->setSaveEol( eol_marker );
    cmd->setSaveResultDataOffset(H.data_offset);
   // Leave file open to receive output of `expView` command.

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Find_Previous_View (SS_View.cxx), NEW CMD was setup to receive the expview data for, eoc_marker=" << eoc_marker 
            << " eol_marker=" << eol_marker << " cmdstr=" << cmdstr << " H.data_offset=" << H.data_offset 
            << " new_ViewInfo->FileName()=" << new_ViewInfo->FileName() << std::endl;
  std::cerr << "EXIT SS_Find_Previous_View (SS_View.cxx), NEW VIEW CREATED, return TRUE" << std::endl;
#endif

    return true;
  }

#if DEBUG_REUSEVIEWS
   std::cerr << "EXIT SS_Find_Previous_View (SS_View.cxx), return FALSE" << std::endl;
#endif

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

 // Look for a saved view.
  savedViewInfo *svi = NULL;

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_Generate_View (SS_View.cxx), calling SS_Find_Previous_View if OPENSS_SAVE_VIEWS_FOR_REUSE is set" << std::endl;
#endif

  // Check to see the preference for saving views is enabled and a view for this command already exists
  //
  if ( OPENSS_SAVE_VIEWS_FOR_REUSE && SS_Find_Previous_View( cmd, exp ) ) {

    if (!cmd->SaveResult()) {
     // Previously generated output file found with requested view.
      return true;
    }

   // An existing saved view is not available but provision has been made to create a new one.
   //
    svi = cmd->SaveResultViewInfo();
    if (svi != NULL) {
     // Set StartTIme to measure how long it takes to generate the view.
      svi->setStartTime();
    }
  }

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
  printf("In SS_Generate_View (SS_View.cxx), after calling Filter_ThreadGroup\n");
#endif

#if DEBUG_CLI
  printf("In SS_Generate_View in SS_View.cxx, before calling vt->GenerateView\n");
#endif

 // Try to Generate the Requested View!
  bool success = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                                   tgrp, cmd->Result_List());

#if DEBUG_CLI
  printf("In SS_Generate_View in SS_View.cxx, after calling vt->GenerateView\n");
#endif

  // Read up the save and reuse file metadata information structure
  svi = cmd->SaveResultViewInfo();

  if (svi != NULL) {

    // Set EndTIme if saving info after generation.
    if (success) {  svi->setEndTime(); }

#if DEBUG_REUSEVIEWS
    // Print out how long it took the view to be generated
    std::cerr << "In Print_Results, ((svi->EndTime()-svi->StartTime())/1000000000)=" << ((svi->EndTime()-svi->StartTime())/1000000000)
              << " OPENSS_SAVE_VIEWS_TIME=" << OPENSS_SAVE_VIEWS_TIME << std::endl;
#endif

    // Decide if we want to save this view in the database for reuse
    // If the time to generate the view is greater than or equal to the preference setting, then save the view
    // Otherwise go forward with the setting that indicates to the rest of the CLI code that the view will not be saved
    //
    if ((success)  &&
        (svi->StartTime() != 0) &&
        (svi->EndTime() != 0) &&
        (OPENSS_SAVE_VIEWS_TIME >= 0) &&
        ( (OPENSS_SAVE_VIEWS_TIME == 0) ||
        (((svi->EndTime() - svi->StartTime())/1000000000) >= OPENSS_SAVE_VIEWS_TIME) ) ) {

      std::ostream *tof = svi->writeHeader ();

#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_Generate_View in SS_View.cxx, after calling writeHeader, tof=" << tof 
                << " SAVE the ostream DATA pointer HERE!!!  Use setSaveResultOstream( tof )" << std::endl;
#endif

      // Save the ostream pointer into the command result structure
      //
      cmd->setSaveResultOstream( tof );

    } else {

      // KEY ITEM: Indicate that for this view we will not save into the database.
      // This means: no intermediate disk file will be used and no saving of the view to the database
      //
      svi->setDoNotSave();

#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_Generate_View in SS_View.cxx, setting DONOTSAVE for view, because GenerateView did not succeed." 
                << " Or the time to generate the view was less than the preference time to generate value." << std::endl;
#endif

    }
  }

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
