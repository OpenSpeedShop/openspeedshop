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

//#define DEBUG_REUSEVIEWS 1

#include "SS_Input_Manager.hxx"


using namespace OpenSpeedShop::cli;

int ExperimentObject::Determine_Status() {
  int expstatus = ExpStatus_NonExistent;
  if (FW() == NULL) {
    ExpStatus = expstatus;
  } else {
    ThreadGroup tgrp = FW()->getThreads();
    if (tgrp.empty()) {
      expstatus = ExpStatus_Paused;
    } else {
      ThreadGroup::iterator ti;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
        Thread t = *ti;
        try {
          if (t.getState() == Thread::Running) {
           // if any thread is Running, the experiment is also.
            expstatus = ExpStatus_Running;
            break;
          } else if (t.getState() == Thread::Suspended) {
           // Paused can override Terminated
            expstatus = ExpStatus_Paused;
          } else if (t.getState() == Thread::Terminated) {
           // The experiment is terminated only if all the threads are.
            if (expstatus != ExpStatus_Paused) {
              expstatus = ExpStatus_Terminated;
            }
          } else if ((t.getState() == Thread::Connecting) ||
                     (t.getState() == Thread::Disconnected) ||
                     (t.getState() == Thread::Nonexistent)) {
           // These are 'Don't care" states at the user level.
           // Note: we might default to ExpStatus_NonExistent.
          } else {
            expstatus = ExpStatus_InError;
            break;
          }
        }
        catch(const Exception& error) {
         // Don't really care why.
         // Mark the experiment with an error and continue on.
          expstatus = ExpStatus_InError;
          break;
        }
      }
    }
    ExpStatus = expstatus;
  }
  return expstatus;
}

std::string ExperimentObject::ExpStatus_Name () {
  int expstatus = Status();
  if ((this == NULL) || (ExpStatus == ExpStatus_NonExistent)) return std::string("NonExistent");
  if (expstatus == ExpStatus_Paused) return std::string("Paused");
  if (expstatus == ExpStatus_Running) return std::string("Running");
  if (expstatus == ExpStatus_Terminated) return std::string("Terminated");
  if (expstatus == ExpStatus_InError) return std::string("Error");
  return std::string("Unknown");
}

void ExperimentObject::Print_Waiting (std::ostream &mystream) {
  Assert(pthread_mutex_lock(&Experiment_Lock) == 0);
  if (waiting_cmds.begin() != waiting_cmds.end()) {
    mystream << "  Waiting Commands" << std::endl;
    std::list<CommandObject *>::iterator wi;
    for (wi = waiting_cmds.begin(); wi != waiting_cmds.end(); wi++) {
      CommandObject *cmd = *wi;
      mystream << "    " << cmd->Clip()->Command() << std::endl;
    }
  }
  Assert(pthread_mutex_unlock(&Experiment_Lock) == 0);
}

void ExperimentObject::Print(std::ostream &mystream) {
  mystream << "Experiment " << ExperimentObject_ID() << " " << ExpStatus_Name() << " data->";
  if (TS_Lock()) {
    mystream << ((FW_Experiment != NULL) ? FW_Experiment->getName() : "(null)") << std::endl;
    if (FW_Experiment != NULL) {
      ThreadGroup tgrp = FW_Experiment->getThreads();
      ThreadGroup::iterator ti;
      bool atleastone = false;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
        Thread t = *ti;
        std::string host = t.getHost();
        pid_t pid = t.getProcessId();
        if (!atleastone) {
          atleastone = true;
        }
        mystream << "    -h " << host << " -p " << pid;
        std::pair<bool, int> pthread = t.getOpenMPThreadId();
        if (pthread.first) {
          mystream << " -t " << pthread.second;
        } else {
          std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
          if (pthread.first) {
            mystream << " -t " << pthread.second;
          }
        }
        std::pair<bool, int> rank = t.getMPIRank();
        if (rank.first) {
          mystream << " -r " << rank.second;
        }
        CollectorGroup cgrp = t.getCollectors();
        CollectorGroup::iterator ci;
        int collector_count = 0;
        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector c = *ci;
          Metadata m = c.getMetadata();
          if (collector_count) {
            mystream << ",";
          } else {
            mystream << " ";
            collector_count = 1;
          }
          mystream << " " << m.getUniqueId();
        }
        mystream << std::endl;
      }

      CollectorGroup cgrp = FW_Experiment->getCollectors();
      CollectorGroup::iterator ci;
      atleastone = false;
      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        ThreadGroup tgrp = c.getThreads();
        if (tgrp.empty()) {
          Metadata m = c.getMetadata();
          if (atleastone) {
            mystream << ",";
          } else {
            mystream << "   ";
            atleastone = true;
          }
          mystream << " " << m.getUniqueId();
        }
      }
      if (atleastone) {
        mystream << std::endl;
      }
    }
    Q_UnLock ();
  } else {
    mystream << "(Currently unable to access the database)" << std::endl;
  }
  Print_Waiting (mystream);
}

//
// Find a saved view
//
bool Find_SavedView (CommandObject *cmd, std::string local_tag)
{
#if DEBUG_REUSEVIEWS
    std::cerr << "Enter Find_SavedView, local_tag=" << local_tag << std::endl;
#endif

  if (!OPENSS_SAVE_VIEWS_FOR_REUSE) {

#if DEBUG_REUSEVIEWS
     std::cerr << "EXIT Find_SavedView, Save and Reuse is not enabled." << std::endl;
#endif

     return false;
  }


 // Determine the ExperimentObject from the command.
  InputLineObject *clip = cmd->Clip();
  //std::cerr << "IN Find_SavedView, RETURN if NULL==clip=" << clip << std::endl;
  if (clip == NULL) return false;
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = NULL;
  std::string cmdstr = clip->Command();
  bool save_only_for_current_session = false;

  std::vector<ParseRange> *p_elist = cmd->P_Result()->getExpIdList();
  if (p_elist->begin() == p_elist->end()) {
   // The user has not selected a view look for the focused experiment.
    EXPID ExperimentID = Experiment_Focus ( WindowID );
    exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;
  } else {
   // Look through the '-x' list for the first experiment.
    std::vector<ParseRange>::iterator ei;
    for (ei = p_elist->begin(); ei != p_elist->end(); ei++) {
     // Determine the experiment needed for the next set.
      parse_range_t *e_range = (*ei).getRange();
      Assert (e_range != NULL);
      parse_val_t eval1 = e_range->start_range;
      parse_val_t eval2 = (e_range->is_range) ? e_range->end_range : eval1;
      Assert (eval1.tag == VAL_NUMBER);

      for (int64_t i = eval1.num; i <= eval2.num; i++) {
        if (exp != NULL) {
         // Do not permenently save the file when multiple experiments are
         // specified because we track savedViews through the ExperimentObject
         // and another session may not have the same data bases available.
         //
         // However, we can attach the file to the first experiment and reuse
         // it during the current session because experiment IDs and cView
         // Ids are not reused during a session.
#if DEBUG_REUSEVIEWS
          std::cerr << "Enter Find_SavedView, multiple experiments, setting save_only_for_current_session=true" << std::endl;
#endif
          save_only_for_current_session = true;
        } else {
          EXPID ExperimentID = i;
          exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;
          if ((exp == NULL) ||
              (exp->FW() == NULL)) {
           // This is an error and should be reported.
            std::ostringstream M;
            M << "Experiment ID '-x " << ExperimentID << "' is not valid.";
            Mark_Cmd_With_Soft_Error(cmd, M.str());
#if DEBUG_REUSEVIEWS
            std::cerr << "IN Find_SavedView, RETURN if NULL==ExperimentID=" << ExperimentID << std::endl;
#endif
            return false;
          }
        }
      }

    }

  }

 // NOTE/FIXME:  We will want to change this when this code is revamped to save cview commands across sessions
 // If there are any "-c" items, allow reuse only during the current session.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseRange> *cv_list = p_result->getViewSet ();

  if (cv_list->begin() != cv_list->end()) {
#if DEBUG_REUSEVIEWS
    std::cerr << "IN Find_SavedView, there are -c items, setting save_only_for_current_session=true" << std::endl;
#endif
    save_only_for_current_session = true;
  }

  if ((exp == NULL) ||
      (exp->FW() == NULL)) {
   // No experiment was specified, so we can't find a useful view to generate.
    Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified for command.");
    //std::cerr << "IN Find_SavedView, NO valid experiment" << std::endl;
    return false;
  }

  EXPID ExperimentID = exp->ExperimentObject_ID();

// Is there a good chance that a matching view has been saved?
// if (Status() == ExpStatus_Paused) return false;
  if (exp->Status() == ExpStatus_Running) return false;
  if (exp->FW() == NULL) return false;
 // Use a time stamp to convience us that the database we are told
 // to look at is the one used to generate the view.
  Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
  Time EndTime = databaseExtent.getTimeInterval().getEnd();

  std::string previous_view("");

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

#if DEBUG_REUSEVIEWS
  std::cerr << "IN Find_SavedView, before strcasestr, cmdstr.c_str()=" << cmdstr.c_str() << std::endl;
#endif

 // Isolate the important parts of the command.
  std::string viewcmd("cView");
  char *cidx = strcasestr( (char *)cmdstr.c_str(), std::string("cView").c_str() );

#if DEBUG_REUSEVIEWS
  std::cerr << "after strcasestr for cView, cmdstr=" << cmdstr << " viewcmd=" << viewcmd << std::endl;
#endif

  if (cidx == NULL) {
    viewcmd = "expView";
    cidx = strcasestr( (char *)cmdstr.c_str(), std::string("expView").c_str() );
  }
  if (cidx == NULL) {
    viewcmd = "expCompare";
    cidx = strcasestr( (char *)cmdstr.c_str(), std::string("expCompare").c_str() );
  }
  if (cidx == NULL) {
    viewcmd = " cviewCluster";
    cidx = strcasestr( (char *)cmdstr.c_str(), (char *)viewcmd.c_str() );
  } else {
    // we don't want to save cview type commands in the database
    return false;
  }
  if (cidx == NULL) {
    viewcmd = " cView";
    cidx = strcasestr( (char *)cmdstr.c_str(), (char *)viewcmd.c_str() );
  } else {
    // we don't want to save cview type commands in the database
    return false;
  }


  if (cidx == NULL) {
#if DEBUG_REUSEVIEWS
   std::cerr << "IN Find_SavedView, EXIT early if NULL==cidx" << std::endl;
#endif
    return false;
  }
  cmdstr = std::string( cidx );

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

  if ( !save_only_for_current_session ) {
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
          if ( (cidx[toc] == *(" ")) ||		// Allow blanks between items
               (cidx[toc] == *(",")) ||		// Allow lists of items
               (cidx[toc] == *(":")) ||		// Allow ranges of numbers
               ( (cidx[toc] >= *("0")) &&	// Allow integer numbers
                 (cidx[toc] <= *("9")) ) ) {
            continue;
          }
          break;
        }
        cmdstr = cmdstr.substr( 0, prior_end_of_field) + cmdstr.substr( toc, cmdstr.length()-1);
      }
    }
  }

 // If not empty, append 'local_tag' and do not save file after current session.
  if ( !local_tag.empty() ) {
    cmdstr = cmdstr + " " + local_tag;

#if DEBUG_REUSEVIEWS
    std::cerr << "In Find_SavedView, there are local_tag is not empty, setting save_only_for_current_session=true" 
              << " local_tag=" << local_tag << " cmdstr=" << cmdstr << std::endl;
#endif

    save_only_for_current_session = true;
  }

#if DEBUG_REUSEVIEWS
    std::cerr << "Toward end of Find_SavedView, local_tag=" << local_tag << " cmdstr=" << cmdstr << std::endl;
#endif

 // Look for an existing output that was generated with the same command.
  savedViewInfo *use_ViewInfo = exp->FindExisting_savedViewInfo (eoc_marker, eol_marker, cmdstr);

#if DEBUG_REUSEVIEWS
    std::cerr << "Toward end of Find_SavedView, use_ViewInfo=" << use_ViewInfo << " cmdstr=" << cmdstr << std::endl;
#endif

  if (use_ViewInfo != NULL) {
    cmd->setSaveResultViewInfo(use_ViewInfo);
    cmd->setSaveExpId( ExperimentID );
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
    new_ViewInfo->setHeader( Data_File_Name, H, eoc_marker, eol_marker, cmdstr);

#if DEBUG_REUSEVIEWS
    std::cerr << "In get_savedViewInfo, Calling exp->FW()->setDatabaseViewHeader(Data_File_Name, cmdstr)"
              << " Data_File_Name=" << Data_File_Name << " cmdstr=" << cmdstr << std::endl;
#endif

    if (save_only_for_current_session) {

#if DEBUG_REUSEVIEWS
    std::cerr << "In get_savedViewInfo, setting REMOVEENTRY because save_only_for_current_session=" << save_only_for_current_session << std::endl;
#endif

      new_ViewInfo->setRemoveEntryAtSessionEnd();

    }

   // Preserve file name and format information in the command
   // for immediate display of the report after it has been saved.
    cmd->setSaveResultViewInfo(new_ViewInfo);
    cmd->setSaveExpId( ExperimentID );
    cmd->setSaveResultFile(new_ViewInfo->FileName());
    cmd->setSaveResult(true);
    cmd->setSaveEoc( eoc_marker );
    cmd->setSaveEol( eol_marker );
    cmd->setSaveResultDataOffset(H.data_offset);
   // Leave file open to receive output of `expView` command.
#if DEBUG_REUSEVIEWS
    std::cerr << "EXIT Find_SavedView, RETURN TRUE" << std::endl;
#endif
    return true;
  }
#if DEBUG_REUSEVIEWS
  std::cerr << "EXIT Find_SavedView, RETURN FALSE" << std::endl;
#endif
  return false;
}
