/******************************************************************************
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

// use the save to database code instead of save to file for saving and reusing perf output views
#define DATABASE_SAVEDVIEWS 1

// Debug flags to enable printing debug output
//#define DEBUG_SYNC_SIGNAL 1
//#define DEBUG_CLI 1
//#define DEBUG_REUSEVIEWS 1
//#define DEBUG_REUSEVIEWS_EXTENDED 1

/** @file
 *
 * Definition of the ExperimentObject class.
 *
 */



// ExperimentObject
// Note: ALL the instrumentation in the experiment's list is applied
//       to ALL of the executables in the experiment's list.
class ExperimentObject;
extern EXPID Experiment_Sequence_Number;
extern pthread_mutex_t Experiment_List_Lock;
extern std::list<ExperimentObject *> ExperimentObject_list;
extern void openss_error(char *);

#define ExpStatus_NonExistent 0
#define ExpStatus_Paused      1
#define ExpStatus_Running     2
#define ExpStatus_Terminated  3
#define ExpStatus_InError     4

class ExperimentObject
{
 private:
  EXPID Exp_ID;
  int ExpStatus;
  bool Data_File_Has_A_Generated_Name;
  OpenSpeedShop::Framework::Experiment *FW_Experiment;
  pthread_mutex_t Experiment_Lock;
  bool exp_reserved;
  bool expRunAtLeastOnceAlreadyFlag;
  std::list<CommandObject *> waiting_cmds;
  bool isBatch;
  bool isInstrumentorOffline;
  bool instrumentorUsesCBTF;
  std::string offlineAppCommand;

  bool found_existing_saved_files;
  int64_t maxSavedViewFiles;
  savedViewInfo **Saved_View_Files;

  std::string create_savedFileName( std::string base_name, int64_t cnt ) {
    char base[32];
    snprintf(base, 32, ".%lld.view", static_cast<long long int>(cnt));
    std::string new_name = base_name + std::string(base);

#if DEBUG_REUSEVIEWS
    std::cerr << "EXIT create_savedFileName, base_name=" << base_name << " returning new_name=" << new_name << std::endl;
#endif
    return new_name;
  }

  void get_savedViewInfo () {
   // Look for existing output files that were generated with the same command and saved.
    std::string first_unused_file_name = "";
    bool first_unused_file_name_found = false;

#if DEBUG_REUSEVIEWS
    std::cerr << "Enter get_savedViewInfo, found_existing_saved_files=" << found_existing_saved_files << std::endl;
#endif

    if (!found_existing_saved_files) {
      found_existing_saved_files = true; // Only do this search once.
      Saved_View_Files = (savedViewInfo **)calloc( OPENSS_SAVE_VIEWS_FILE_LIMIT, sizeof(savedViewInfo *) );
      maxSavedViewFiles = OPENSS_SAVE_VIEWS_FILE_LIMIT;
      std::string Data_File_Name = FW_Experiment->getName();

#if DEBUG_REUSEVIEWS
    std::cerr << "In get_savedViewInfo, Data_File_Name=" << Data_File_Name << std::endl;
#endif
 
     int max_loop_cnt;
#if DATABASE_SAVEDVIEWS
     max_loop_cnt = 1;
#else
     max_loop_cnt = maxSavedViewFiles;
#endif
      char* buffer = new char[OPENSS_VIEW_MAX_FIELD_SIZE]; // Create a buffer to read the header.
      for (int64_t i = 0; i < max_loop_cnt; i++) {
        std::string viewPath = create_savedFileName (Data_File_Name, i);
        int fd = open(viewPath.c_str(), O_RDONLY);
        
#if DEBUG_REUSEVIEWS
        //std::cerr << "In get_savedViewInfo, LOOPING THROUGH SAVEDVIEWS viewPath=" << viewPath << " index (i)=" << i << std::endl;
#endif

        if (fd != -1) {
          Saved_View_Files[i] = new savedViewInfo (viewPath, false, i);
          int num = read( fd, buffer, OPENSS_VIEW_MAX_FIELD_SIZE );
          save_file_header H;
          if (num >= sizeof(H)) {
            char *cp = (char *)(&H);
            for (int i = 0; i < sizeof(H); i ++) { cp[i] = buffer[i]; }
          }
#if DEBUG_REUSEVIEWS
          std::cerr << "In get_savedViewInfo, LOOPING THROUGH SAVEDVIEWS buffer=" << buffer << std::endl; 

          std::cerr << "In get_savedViewInfo, LOOPING THROUGH SAVEDVIEWS viewPath=" << viewPath 
                    << " index (i)=" << i << " H.type=" << H.type << " sizeof(H)=" << sizeof(H) 
                    << " num=" << num << " H.data_offset=" << H.data_offset << std::endl;
#endif
          if ( ( (H.type == 1) || (H.type == 2) ) && (num >= (sizeof(H) + H.data_offset)) ) {
           // Set the EOC and EOL strings to end with proper string terminator.
            buffer[H.eoc_offset-1] = *("\0");
            buffer[H.eol_offset-1] = *("\0");
            buffer[H.command_offset-1] = *("\0");
            buffer[H.data_offset-1] = *("\0");

            std::string new_eoc(&buffer[H.eoc_offset]);
            std::string new_eol(&buffer[H.eol_offset]);
            std::string new_cmd(&buffer[H.command_offset]);
#if DEBUG_REUSEVIEWS
            std::cerr << "In get_savedViewInfo, Calling setHeader with new info, new_eoc=" << new_eoc 
                      << " new_eol=" << new_eol << " new_cmd=" << new_cmd << " sizeof(buffer)=" << sizeof(buffer) << std::endl;
#endif
            Saved_View_Files[i]->setHeader( Data_File_Name, H, new_eoc, new_eol, new_cmd);

#if DEBUG_REUSEVIEWS
            std::cerr << "In get_savedViewInfo, Calling FW()->setDatabaseViewHeader(Data_File_Name, new_cmd)"
                      << " Data_File_Name=" << Data_File_Name << " new_cmd=" << new_cmd << std::endl;
#endif

          }

          Assert(close(fd) == 0);
        } else {
          Saved_View_Files[i] = NULL;
        }
      }
      delete [] buffer;  // Destroy the read buffer.

    }
    found_existing_saved_files = true;
  }

 public:

  std::list<std::string> offlineCollectorList;

  void deleteSavedViews() {

   // Remove or save remembered views.
    if ( found_existing_saved_files &&
         (Saved_View_Files != NULL) ) {
#if DEBUG_REUSEVIEWS
      std::cerr << "In deleteSavedViews delete for maxSavedViewFiles=" << maxSavedViewFiles 
                << " found_existing_saved_files=" << found_existing_saved_files 
                << "Saved_View_Files=" << Saved_View_Files << std::endl;
#endif
      for (int64_t i=0; i<maxSavedViewFiles; i++) {
        savedViewInfo *svi = Saved_View_Files[i];
        if (svi != NULL) {
#if DEBUG_REUSEVIEWS
          std::cerr << "In deleteSavedViews delete for i=" << i << " svi=" << svi << std::endl;
#endif
          delete svi;
          Saved_View_Files[i] = NULL;
        }
      }
    }
    Saved_View_Files = NULL;
    found_existing_saved_files = NULL;

  }


  ExperimentObject (std::string data_base_name = std::string(""), EXPID preferred_ID = 0) {

    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);

#ifdef DEBUG_CLI
    std::cerr << "Enter ExperimentObject, preferred_ID=" << preferred_ID << "Experiment_Sequence_Number=" << Experiment_Sequence_Number << "\n";
#endif

    // preferred_ID indicates that this is an offline experiment that wants to write over the previous ExperimentObject
    if (preferred_ID != 0) {

#ifdef DEBUG_CLI
      std::cerr << "ExperimentObject, preferred_ID=" << preferred_ID << "Experiment_Sequence_Number=" << Experiment_Sequence_Number << "\n";
#endif

      Assert (Experiment_Sequence_Number <= preferred_ID);

      Exp_ID = preferred_ID;
      Experiment_Sequence_Number = preferred_ID;

    } else {

      Exp_ID = ++Experiment_Sequence_Number;

#ifdef DEBUG_CLI
      std::cerr << "ExperimentObject, preferred_ID=" << preferred_ID << " after updating Experiment_Sequence_Number=" << Experiment_Sequence_Number << " Exp_ID=" << Exp_ID << "\n";
#endif
    } 
//    Exp_ID = ++Experiment_Sequence_Number;
    ExpStatus = ExpStatus_Paused;
    exp_reserved = false;
    expRunAtLeastOnceAlreadyFlag = false;
    exp_rerun_count = 0;

    // Determine real value for this from the expCreate command (-i offline)
#if 0 
-- This did not work - jeg 05/24/2011 so I am reverting back to previous setting
-- Change was to help obtain application command for offline convenience script runs
// Since the default instrumentation is offline, initialize the offline instrumentation accordingly
//    setIsInstrumentorOffline(true);
#else
    setIsInstrumentorOffline(false);
#endif

#if (BUILD_CBTF == 1 )
    setInstrumentorUsesCBTF(true);
#else
    setInstrumentorUsesCBTF(false);
#endif

    offlineCollectorList.clear();

    Assert(pthread_mutex_init(&Experiment_Lock, NULL) == 0); // dynamic initialization
    Assert(pthread_mutex_lock(&Experiment_Lock) == 0);       // Lock it!

   // Allocate a data base file for the information connected with the experiment.
    std::string Data_File_Name;
    if (data_base_name.length() == 0) {
      bool database_not_allocated = true;
      Data_File_Has_A_Generated_Name = !OPENSS_SAVE_EXPERIMENT_DATABASE;
      if (OPENSS_SAVE_EXPERIMENT_DATABASE) {
        const char *database_directory = getenv("OPENSS_DB_DIR");
        if (database_directory == NULL) {
            static const char* const kPeriod = ".";
             database_directory = kPeriod;
        }
       // Try to create a file in the current directory
       // of the form "X<exp_id>.XXXX.openss".
       // If the generated name already exists, increment
       // the "XXXX" field and try again with the new name.
       // If we try 1000 times and can't generate a unique name,
       // fall into logic to use 'tempnam' to generate a name.
        char base[256];
        int64_t cnt = 0;
        for (cnt = 0; cnt < 1000; cnt++) {
          snprintf(base, 256, "%s/X%lld.%lld.openss", database_directory,
                   static_cast<long long int>(Exp_ID),
                   static_cast<long long int>(cnt));
      
          int fd;
          if ((fd = open(base, O_RDONLY)) != -1) {
           // File name is already used!
            Assert(close(fd) == 0);
            continue;
          }
          Data_File_Name = std::string(base);
          database_not_allocated = false;
          break;
        }
      }
      if (database_not_allocated) {   
       // Create a file in /tmp, for fastest access,
       // of the form "X<exp_id>.XXXXXX.openss".
//        char base[L_tmpnam];
//        snprintf(base, L_tmpnam, "X%lld.",Exp_ID);
//        char *tName = tempnam ( (Data_File_Has_A_Generated_Name ? NULL : "./"), base );
//        Assert(tName != NULL);
//        Data_File_Name = std::string(tName) + ".openss";
//        free (tName);
        Data_File_Name = createName(Exp_ID, Data_File_Has_A_Generated_Name);
      }
      try {
        OpenSpeedShop::Framework::Experiment::create (Data_File_Name);
      }
      catch(const Exception& error) {
       // Don't really care why.
       // Calling routine must handle the problem.
        Exp_ID = 0;
        Data_File_Has_A_Generated_Name = false;
        FW_Experiment = NULL;
      }
    } else {
      Data_File_Name = data_base_name;
      Data_File_Has_A_Generated_Name = false;
    }

   // Create and open an experiment
    try {

#ifdef DEBUG_CLI
      std::cerr << "try create/open experiment preferred_ID=" << preferred_ID << " Experiment_Sequence_Number=" << Experiment_Sequence_Number << "\n";
#endif
      FW_Experiment = new OpenSpeedShop::Framework::Experiment (Data_File_Name);
      if (preferred_ID == 0) {

#ifdef DEBUG_CLI
        std::cerr << "Creating new experiment ID, pushing this to ExperimentObject_list, Experiment_Sequence_Number=" << Experiment_Sequence_Number << "\n";
#endif

        ExperimentObject_list.push_front(this);

      } else {

#ifdef DEBUG_CLI
        std::cerr << "NOT CREATING A NEW FRAMEWORK EXPERIMENT because preferred_ID=" << preferred_ID << "\n";
        std::cerr << "POP original experiment object and push this to ExperimentObject_list because preferred_ID=" << preferred_ID << "\n";
#endif
        ExperimentObject_list.pop_back();
        ExperimentObject_list.push_front(this);
      }
    }
    catch(const Exception& error) {
     // Don't really care why.
     // Calling routine must handle the problem.
      Exp_ID = 0;
      Data_File_Has_A_Generated_Name = false;
      FW_Experiment = NULL;
    }

   // Initialize remembered views information.
    found_existing_saved_files = FALSE;
    maxSavedViewFiles = 0;
    Saved_View_Files = NULL;

    Assert(pthread_mutex_unlock(&Experiment_Lock) == 0);       // Unlock new experiment
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);

#ifdef DEBUG_CLI
    std::cerr << "Exit ExperimentObject, Experiment_Sequence_Number=" << Experiment_Sequence_Number << "\n";
#endif

  }


  ~ExperimentObject () {
    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
    if (FW_Experiment != NULL) {
       std::string Data_File_Name = FW_Experiment->getName();
// At some point we may want to remove commands that are not cross session safe
// for now we are not saving cview, cviewcreate, or cviewcluster commands
#if 0
       if (OPENSS_SAVE_VIEWS_FOR_REUSE) {
         FW_Experiment->removeNonCrossSessionViews(Data_File_Name);
       }
#endif
    }
    ExperimentObject_list.remove (this);
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);

    if (FW_Experiment != NULL) {
      try {
        std::string Data_File_Name;
        if (Data_File_Has_A_Generated_Name) {
         // Delete the file AFTER deleting the experiment object.
          Data_File_Name = FW_Experiment->getName();
        }
        delete FW_Experiment;
        if (Data_File_Has_A_Generated_Name) {
          OpenSpeedShop::Framework::Experiment::remove (Data_File_Name);
        }
      }
      catch(const Exception& error) {
       // Don't really care why.
      }
      Data_File_Has_A_Generated_Name = false;
      FW_Experiment = NULL;
    }
    Exp_ID = 0;
    ExpStatus = ExpStatus_NonExistent;

   // Remove or save remembered views.
    if ( found_existing_saved_files &&
         (Saved_View_Files != NULL) ) {
      for (int64_t i=0; i<maxSavedViewFiles; i++) {
        savedViewInfo *svi = Saved_View_Files[i];
        if (svi != NULL) {
#if DEBUG_REUSEVIEWS
          std::cerr << "In Experiment destructor, delete for i=" << i << " svi=" << svi << std::endl;
#endif
          delete svi;
          Saved_View_Files[i] = NULL;
        }
      }
    }
    Saved_View_Files = NULL;
    found_existing_saved_files = NULL;

    pthread_mutex_destroy(&Experiment_Lock);
  }

  EXPID ExperimentObject_ID() {return Exp_ID;}
  Experiment *FW() {return FW_Experiment;}
  bool expRunAtLeastOnceAlready () {return expRunAtLeastOnceAlreadyFlag;}
  void setExpRunAtLeastOnceAlready (bool flag) {expRunAtLeastOnceAlreadyFlag = flag;}
  bool expIsBatch () {return isBatch;}
  void setExpIsBatch (bool flag) {isBatch = flag;}
  bool getIsInstrumentorOffline () {return isInstrumentorOffline;}
  void setIsInstrumentorOffline (bool flag) {isInstrumentorOffline = flag;}
  // flag for CBTF
  bool getInstrumentorUsesCBTF () {return instrumentorUsesCBTF;}
  void setInstrumentorUsesCBTF (bool flag) {instrumentorUsesCBTF = flag;}

  void setOfflineAppCommand(std::string appCmd) {
         offlineAppCommand = appCmd;
  }

  std::string getOfflineAppCommand() {
     return offlineAppCommand;
  }

  uint32_t exp_rerun_count;

  std::string createName (EXPID LocalExpId, bool LocalDataFileHasAGeneratedName) {
     std::string LocalDataFileName;
     // Create a file in /tmp, for fastest access,
     // of the form "X<exp_id>.XXXXXX.openss".
     char base[L_tmpnam];
     snprintf(base, L_tmpnam, "X%lld.", static_cast<long long int>(LocalExpId));
     char *database_directory = getenv("OPENSS_DB_DIR");
     char *tName = NULL;
     char tmp_tName[256];
     if (database_directory) {
        int64_t cnt = 0;
        for (cnt = 0; cnt < 1000; cnt++) {
         snprintf(tmp_tName, 256, "%s/X%lld.%lld.openss", database_directory,
                  static_cast<long long int>(LocalExpId),
                  static_cast<long long int>(cnt));
         Assert(tmp_tName != NULL);

         int fd;
         if ((fd = open(tmp_tName, O_RDONLY)) != -1) {
          // File name is already used!
           Assert(close(fd) == 0);
           continue;
         }
         LocalDataFileName = std::string(tmp_tName);
         break;
        }
     } else {
       tName = tempnam ( (LocalDataFileHasAGeneratedName ? NULL : "./"), base );
       Assert(tName != NULL);
       LocalDataFileName = std::string(tName) + ".openss";
       free (tName);
     }

     return LocalDataFileName;
  }

  std::string createRerunNameFromCurrentName (EXPID LocalExpId, uint32_t rerun_count, const char* current_exp_name) {
     std::string LocalDataFileName;
     // Create a file in /tmp, for fastest access,
     // of the form "X<exp_id>.XXXXXX.openss".
     char base[256];
     char newName[256];

#if DEBUG_CLI
     printf("[TID=%ld], createRerunNameFromCurrentName, newname=%s-%d.\n", pthread_self(), current_exp_name, rerun_count);
#endif

     int nameLen = strlen(current_exp_name);
     nameLen = nameLen - 7; // .openss has seven characters and we want to eliminate them
     strncpy (newName, current_exp_name, nameLen);
     newName[nameLen] = '\0';
     snprintf(base, 256, "%s-%d", newName, rerun_count);

#if DEBUG_CLI
     printf("[TID=%ld], createRerunNameFromCurrentName, base=%s, rerun_count=%ld\n", pthread_self(), base, rerun_count);
#endif

     Assert(base != NULL);
     LocalDataFileName = std::string(base) + ".openss";

#if DEBUG_CLI
     printf("[TID=%ld], createRerunNameFromCurrentName, LocalDataFileName.c_str()=%s\n", pthread_self(), LocalDataFileName.c_str());
#endif

     return LocalDataFileName;
  }

  bool Data_Base_Is_Tmp () {return Data_File_Has_A_Generated_Name;}
  std::string Data_Base_Name () {
    if (FW() == NULL) {
      return "";
    } else {
      try {
        return FW()->getName();
      }
      catch(const Exception& error) {
       // Don't really care why.
        return "(Unable to determine.)";
      }
    }
  }
  void RenameDB (std::string New_DB) {
   // Rename the data base file.
    if (FW_Experiment != NULL) {
      FW_Experiment->renameTo(New_DB);
      Data_File_Has_A_Generated_Name = false;
    }
  }
  void CopyDB (std::string New_DB) {
   // Make a copy of the data base file.
    if (FW_Experiment != NULL) {
      FW_Experiment->copyTo(New_DB);
    }
  }

  bool Has_Ranks() {
    ThreadGroup tgrp = FW()->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if (ti != tgrp.end()) {
      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      return prank.first;
    }
    return false;
  }

  void Q_Lock (CommandObject *cmd, bool start_next_cmd = false) {

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_Lock, before calling pthread_mutex_lock(&Experiment_Lock=%ld), start_next_cmd=%ld\n", pthread_self(), Experiment_Lock, start_next_cmd);
#endif

    Assert(pthread_mutex_lock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_Lock, after calling pthread_mutex_lock(&Experiment_Lock=%ld), start_next_cmd=%ld\n", pthread_self(), Experiment_Lock, start_next_cmd);
#endif

    if (start_next_cmd) {

#ifdef DEBUG_SYNC_SIGNAL
     printf("[TID=%ld], Q_Lock, before calling SafeToDoNextCmd(), start_next_cmd=%ld\n", pthread_self(), start_next_cmd);
#endif
      
      SafeToDoNextCmd ();

#ifdef DEBUG_SYNC_SIGNAL
     printf("[TID=%ld], Q_Lock, after calling SafeToDoNextCmd(), start_next_cmd=%ld\n", pthread_self(), start_next_cmd);
#endif

    }

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_Lock, exp_reserved=%ld\n", pthread_self(), exp_reserved);
#endif

    if (exp_reserved) {
     // Queue myself up and wait until the previous
     // commands have completed execution.
      waiting_cmds.push_back(cmd);
      cmd->Wait_On_Dependency(Experiment_Lock);
     // When we return, the lock has been reset.
    }
    exp_reserved = true;

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_Lock, before calling pthread_mutex_unlock(&Experiment_Lock=%ld)\n", pthread_self(), Experiment_Lock);
#endif

    Assert(pthread_mutex_unlock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_Lock, after calling pthread_mutex_unlock(&Experiment_Lock=%ld)\n", pthread_self(), Experiment_Lock);
#endif

    return;
  }

  void Q_UnLock () {

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_UnLock, before calling pthread_mutex_lock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

    Assert(pthread_mutex_lock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_UnLock, after calling pthread_mutex_lock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

    if (waiting_cmds.begin() != waiting_cmds.end()) {
      CommandObject *cmd = *waiting_cmds.begin();
      waiting_cmds.pop_front();
      cmd->All_Clear ();
    }
    exp_reserved = false;

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_UnLock, before calling pthread_mutex_unlock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

    Assert(pthread_mutex_unlock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Q_UnLock, after calling pthread_mutex_unlock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

  }

  bool TS_Lock () {

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], TS_LOCK, before calling pthread_mutex_lock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

    Assert(pthread_mutex_lock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], TS_LOCK, before calling pthread_mutex_lock(&Experiment_Lock=%ld), exp_reserved=%ld\n", pthread_self(), Experiment_Lock, exp_reserved);
#endif

    bool already_in_use = exp_reserved;
    exp_reserved = true;

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], TS_LOCK, before calling pthread_mutex_unlock(&Experiment_Lock=%ld), already_in_use=%ld, exp_reserved=%ld\n", 
            pthread_self(), Experiment_Lock, already_in_use, exp_reserved);
#endif

    Assert(pthread_mutex_unlock(&Experiment_Lock) == 0);

#ifdef DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], TS_LOCK, after calling pthread_mutex_unlock(&Experiment_Lock=%ld), already_in_use=%ld, exp_reserved=%ld\n", 
            pthread_self(), Experiment_Lock, already_in_use, exp_reserved);
#endif

    return !already_in_use;
  }

  bool CanExecute () {
    bool itcan = false;
    if (FW() != NULL) {
      if (TS_Lock()) {
        try {
          ThreadGroup tgrp = FW()->getThreads();
          if (!tgrp.empty()) itcan = true;
        }
        catch(const Exception& error) {
         // Don't care
        }
        Q_UnLock();
      }
    }
    return itcan;

  }

  int64_t Get_SavedViewFileCnt () { 
    if (!found_existing_saved_files) {
      get_savedViewInfo ();
    }
    return maxSavedViewFiles;
  }

  savedViewInfo *Get_savedViewInfo (int64_t i) {
    if (i < maxSavedViewFiles) {
      return Saved_View_Files[i];
    }
    return NULL;
  }

  savedViewInfo *FindExisting_savedViewInfo (std::string eoc_str,
                                             std::string eol_str,
                                             std::string cmd_str) {
    if (!found_existing_saved_files) {
      get_savedViewInfo ();
    }
    for (int64_t i = 0; i < maxSavedViewFiles; i++) {
      savedViewInfo *svi = Saved_View_Files[i];
      if ( (svi != NULL) &&
           ( !svi->NewFile() ||
             ( (svi->StartTime() != 0) &&
               (svi->EndTime() != 0) ) ) &&
           svi->Header_Matches( eoc_str, eol_str, cmd_str) ) {
       // Verify that the file can still be read.
#if DEBUG_REUSEVIEWS
        std::cerr << "In FindExisting_savedViewInfo call where Header_Matches==true for svi=" << svi << " eoc_str=" 
                  << eoc_str << " eol_str=" << eoc_str << " cmd_str=" << cmd_str << " svi->FileName()=" << svi->FileName() << std::endl;
#endif
        int source_fd = open(svi->FileName().c_str(), O_RDONLY);
        if (source_fd != -1) {
         // Close the input file and return the pointer to the savedViewInfo.
          Assert(close(source_fd) == 0);
          return svi;
        }
       // Something has happened to this file since we read the header.
       // Continue looking to see if another file contains what we are looking for.
        Saved_View_Files[i] = NULL;
        delete svi;
      }
    }
    return NULL;
  }

  savedViewInfo *GetFree_savedViewInfo () {
    savedViewInfo *svi = NULL;
    if (!found_existing_saved_files) {
      get_savedViewInfo ();
    }
    int max_loop_cnt;
#if DATABASE_SAVEDVIEWS
     max_loop_cnt = 1;
#else
     max_loop_cnt = maxSavedViewFiles;
#endif
    for (int64_t i = 0; i < max_loop_cnt; i++) {
      savedViewInfo *svi = Saved_View_Files[i];
#ifndef DATABASE_SAVEDVIEWS
      if (svi == NULL) {
#endif
        std::string Data_File_Name = FW_Experiment->getName();
        std::string viewPath = create_savedFileName (Data_File_Name, i);
        svi = new savedViewInfo (viewPath, true, i);
        Saved_View_Files[i] = svi;
        return svi;
#ifndef DATABASE_SAVEDVIEWS
      }
#endif
    }
    return NULL;
  }

  int Status() { return ExpStatus; }
  void setStatus (int S) {ExpStatus = S;}
  int Determine_Status();
  std::string ExpStatus_Name ();

  void Print_Waiting (std::ostream &mystream);
  void Print(std::ostream &mystream);
};

// Make sure all experiments are closed and associated files freed.
void Experiment_Termination ();

// Experiment Utilities
ExperimentObject *Find_Experiment_Object (EXPID ExperimentID);
bool Find_SavedView (CommandObject *cmd, std::string local_tag = std::string());

bool Collector_Used_In_Experiment (OpenSpeedShop::Framework::Experiment *fexp, std::string myname);
Collector Get_Collector (OpenSpeedShop::Framework::Experiment *fexp, std::string myname);
bool Filter_Uses_F (CommandObject *cmd);
void Filter_ThreadGroup (OpenSpeedShop::cli::ParseResult *p_result, ThreadGroup& tgrp);

// Error reporting and stopping the presses
inline void Mark_Cmd_With_Std_Error (CommandObject *cmd, const Exception& error) {
   cmd->Result_String ( error.getDescription() );
   cmd->set_Status(CMD_ERROR);
   savedViewInfo *svi = cmd->SaveResultViewInfo();
   if (svi != NULL) {
   
#if DEBUG_REUSEVIEWS
     std::cerr << "In Mark_Cmd_With_Std_Error, setting DONOTSAVE, svi=" << svi << std::endl;
#endif
     svi->setDoNotSave();
   } 
   
   // Put in python exception
   openss_error((char *)error.getDescription().c_str());
   return;
}

// Error reporting and stopping the presses only 
// for scripting
inline void 
Mark_Cmd_With_Soft_Error (CommandObject *cmd, const std::string S) {
   cmd->Result_String ( S );
   cmd->set_Status(CMD_ERROR);
   
   // Put in python exception
   openss_error((char *)S.c_str());
   return;
}

//  Just put out the message with out error reporting
inline void 
Mark_Cmd_With_Message (CommandObject *cmd, const std::string S) {
   cmd->Result_String ( S );
   return;
}
