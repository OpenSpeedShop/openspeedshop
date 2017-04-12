////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2016 Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

//
// To enable debuging uncomment define DEBUG_StatsPanel statement
//
#define SAVE_REUSE_DATABASE 1
#define DBNAMES 1

//#define DEBUG_StatsPanel_cache 1
//#define DEBUG_StatsPanel_reuse 1
//#define DEBUG_StatsPanel 1
//#define DEBUG_StatsPanel_APPC 1
//#define DEBUG_StatsPanel_chart 1
//#define DEBUG_Sorting 1
//#define DEBUG_INTRO 1
//#define DEBUG_StatsPanel_info 1
//#define DEBUG_StatsPanel_source 1
//#define DEBUG_StatsPanel_toolbar 1
//#define DEBUG_StatsPanel_cview 1
//#define DEBUG_StatsPanel_menu 1
//#define DEBUG_StatsPanel_diff 1
//#define DEBUG_StatsPanel_details 1
//#define DEBUG_StatsPanel_info_details 1

#include "StatsPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include "CollectorListObject.hxx"
#include "CollectorMetricEntryClass.hxx"

#include <qapplication.h>
#include <qheader.h>
#include <qregexp.h>

#include <qvaluelist.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qdockwindow.h>
#include <qdockarea.h>
#include <qbutton.h>
#include <qpushbutton.h>
#include <map>

#include <qpixmap.h>
#include "defaultView.xpm"
#include "functions.xpm"
#include "linkedObjects.xpm"
#include "statements.xpm"
#include "statementsByFunction.xpm"
#include "calltrees.xpm"
#include "calltreesByFunction.xpm"
#include "calltreesfull.xpm"
#include "calltreesfullByFunction.xpm"
#include "tracebacks.xpm"
#include "tracebacksByFunction.xpm"
#include "tracebacksfull.xpm"
#include "tracebacksfullByFunction.xpm"
#include "butterfly.xpm"
#include "meta_information_plus.xpm"
#include "meta_information_minus.xpm"
#include "compare_and_analyze.xpm"
#include "custom_comparison.xpm"
#include "update_icon.xpm"
#include "load_balance_icon.xpm"
#include "event_list_icon.xpm"
#include "clear_auxiliary.xpm"
#include "timeSegment.xpm"
#include "optional_views_icon.xpm"
#include "sourceAnnotation_icon.xpm"
#include "hotcallpath_icon.xpm"
#include "memLeaked_icon.xpm"
#include "memHighwater_icon.xpm"
#include "memUnique_icon.xpm"


class MetricHeaderInfo;
class QPushButton;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;
#include "CLIInterface.hxx"

#include "ManageProcessesPanel.hxx"

#define CLUSTERANALYSIS 1
#define OPTIONAL_VIEW 1
// enable the viewing of pathnames for iot experiments
#define PATHNAME_READY 1

#include "GenericProgressDialog.hxx"


#if 1
// These are the pie chart colors..
static const char *hotToCold_color_names[] = {
  "red",
  "darkorange",
  "orange",
  "gold",
  "yellow",
  "khaki",
  "yellowgreen",
  "limegreen",
  "forestgreen",
  "darkturquoise",
  "turquoise",
  "steelblue",
  "skyblue",
  "lightblue",
};
static const char *coldToHot_color_names[] = {
  "lightblue",
  "skyblue",
  "steelblue",
  "turquoise",
  "darkturquoise",
  "forestgreen",
  "limegreen",
  "yellowgreen",
  "khaki",
  "yellow",
  "gold",
  "orange",
  "darkorange",
  "red",
};
#else
static const char *hotToCold_color_names[] = { 
  "red", 
  "magenta",
  "cyan",
  "darkorange",
  "lightcoral",
  "hotpink",
  "green",
  "orange",
  "darksalmon",
  "coral",
  "skyblue",
  "lightGray",
  "lightblue",
  "lightgreen",
};
static const char *coldToHot_color_names[] = { 
  "lightgreen",
  "lightblue",
  "lightGray"
  "skyblue",
  "coral",
  "orange",
  "darksalmon",
  "green",
  "hotpink",
  "lightcoral",
  "darkorange",
  "cyan",
  "magenta",
  "red", 
};
#endif
#define MAX_COLOR_CNT 14
static const char *blue_color_names[] = { 
  "blue", 
  "blue", 
  "blue", 
};


#define PTI "Present Trace Information"

#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "PrepareToRerunObject.hxx"
#include "FocusObject.hxx"
#include "FocusCompareObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"

#include "preference_plugin_info.hxx"
#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;

namespace {

#define CLI_NANOSLEEP_MS 250
   /**
     * Suspend the calling thread.
     *
     * Suspends the calling thread for approximately CLI_NANOSLEEP_MS. Used to implement
     * busy loops that are waiting for state changes in threads.
     *
     * The suspend here replaced sleep(1) calls wherever seen in this file (11/18/13)
     */
    void suspend()
    {
        // Setup to wait
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = CLI_NANOSLEEP_MS * 1000 * 1000;

        // Suspend ourselves temporarily
        // This while loop ensures that nanosleep will sleep at
        // least the amount of time even if a signal interupts nanosleep.
        while(nanosleep(&wait, &wait));
    }
}


// This routine is strongly based (copy of) 
// on the Tokenizer routine found at this URL:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html

void createTokens(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);

#ifdef DEBUG_StatsPanel
        printf("createTokens, in while, str.c_str()=%s, lastPos = %d, pos = %d\n", str.c_str(), lastPos, pos);
#endif

    }
}

/**
 * StatsPanel::isCommandAssociatedWith: Determine if a cview cluster command is associated with a cview view command.
 *
 * Returns a boolean value reflecting whether or not the cviewcluster command output
 * views (-c values) were used in a cview command and that command was previously 
 * associated with the cviewcluster command.  We do this so we can prevent calling
 * down to the framework and CLI for the data that would have already been produced.
 *
 * @return    Boolean value indicating whether or not a cview cluster command is linked with a cview view command.
 */

bool StatsPanel::isCommandAssociatedWith( std::string input_cview_cluster_command)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER isCommandAssociatedWith, input_cview_cluster_command=" << input_cview_cluster_command << std::endl;
  for (std::map<std::string,std::string>::iterator it=associatedCommandMap.begin(); it!=associatedCommandMap.end(); ++it) {
    std::cerr << "ENTER isCommandAssociatedWith, LOOP THROUGH associatedCommandMap, command:cview_cluster_command=(" << it->first << ": " << it->second << ")" << std::endl;
  }
#endif

  std::map<std::string,std::string>:: iterator clipit ;
  if ( ! associatedCommandMap.empty() ) {
      clipit = associatedCommandMap.find(input_cview_cluster_command);
      
      if (clipit != associatedCommandMap.end() ) {

#ifdef DEBUG_StatsPanel_cache
          std::cerr << "RETURNING TRUE from isCommandAssociatedWith, clipit->first=" << clipit->first << " clipit->second=" << clipit->second << std::endl;
#endif
          return true;
      } else {

#ifdef DEBUG_StatsPanel_cache
          std::cerr << "RETURNING FALSE from isCommandAssociatedWith" << std::endl;
#endif
          return false;
      }

  }

#ifdef DEBUG_StatsPanel_cache
          std::cerr << "FALL THROUGH EXIT, RETURNING FALSE from isCommandAssociatedWith" << std::endl;
#endif
          return false;

}

/**
 * StatsPanel::getAssociatedCommand: Return the cview command associated with cview cluster command.
 *
 * Returns the command string that was associated with its corresponding cviewcluster command.
 *
 * @return    String value linked with a cviewcluster command.
 */

std::string StatsPanel::getAssociatedCommand( std::string input_cview_cluster_command)
{
#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER getAssociatedCommand, input_cview_cluster_command=" << input_cview_cluster_command << std::endl;
  for (std::map<std::string,std::string>::iterator it=associatedCommandMap.begin(); it!=associatedCommandMap.end(); ++it) {
    std::cerr << "ENTER getAssociatedCommand, LOOP THROUGH associatedCommandMap, command:cview_cluster_command=(" << it->first << ": " << it->second << ")" << std::endl;
  }
#endif
  std::map<std::string,std::string>:: iterator clipit ;
  if ( ! associatedCommandMap.empty() ) {
      clipit = associatedCommandMap.find(input_cview_cluster_command);
      
      if (clipit != associatedCommandMap.end() ) {

#ifdef DEBUG_StatsPanel_cache
          std::cerr << "RETURNING TRUE from getAssociatedCommand, clipit->first=" 
                    << clipit->first << " clipit->second=" << clipit->second << std::endl;
#endif
          return clipit->second;
      } else {
#ifdef DEBUG_StatsPanel_cache
          std::cerr << "RETURNING FALSE from getAssociatedCommand" << std::endl;
#endif
          return NULL;
      }

  }

}

/**
 * StatsPanel::associateTheseCommands: Associate cview cluster command its counterpart cview view command.
 *
 * Links or associates the cviewcluster command with the cview command.
 * We do this so we can prevent calling down to the framework and CLI for 
 * the data that would have already been produced.
 *
 * @return    none
 */

void StatsPanel::associateTheseCommands( std::string input_command, std::string input_cview_cluster_command)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER associateTheseCommands, input_command=" << input_command << " input_cview_cluster_command=" << input_cview_cluster_command << std::endl;
  for (std::map<std::string,std::string>::iterator it=associatedCommandMap.begin(); it!=associatedCommandMap.end(); ++it) {
    std::cerr << "ENTER associateTheseCommands, LOOP THROUGH associatedCommandMap, command:cview_cluster_command=(" << it->first << ": " << it->second << ")" << std::endl;
  }
#endif

  std::map<std::string,std::string>:: iterator clipit = associatedCommandMap.begin();
  associatedCommandMap.insert(clipit, std::pair< std::string, std::string>(input_cview_cluster_command, input_command));

#ifdef DEBUG_StatsPanel_cache
  for (std::map<std::string,std::string>::iterator it= associatedCommandMap.begin(); it!=associatedCommandMap.end(); ++it) {
    std::cerr << "EXIT associateTheseCommands, LOOP THROUGH associatedCommandMap, command:cview_cluster_command=(" << it->first << ": " << it->second << ")" << std::endl;
  }
#endif

}

/**
 * StatsPanel::addClipForThisCommand: 
 * Add the clip corresponding to the input argument command string value to the 
 * cmdToClipMap map data structure.  This data structure keeps a running map of 
 * view commands to their corresponding clip.  We use the clip pointer to access
 * the command objects representing the view, so we don't have to request this data
 * from the CLI (and therefore the framework and database file).  We just reuse the
 * already requested data structures to regenerate the same view again.
 *
 * @return    none
 */

void StatsPanel::addClipForThisCommand( std::string input_command, InputLineObject* input_clip)
{
#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER addClipForThisCommand, input_command=" << input_command << " input_clip=" << input_clip << std::endl;

  for (std::map<std::string,InputLineObject*>::iterator it=cmdToClipMap.begin(); it!=cmdToClipMap.end(); ++it) {
    std::cerr << "ENTER addClipForThisCommand, LOOP THROUGH cmdToClipMap, command:clip=(" << it->first << ": " << it->second << ")" << std::endl;
  }

#endif

    // Note: we could check to see if the same view command already exists but existing logic
    // doesn't require this.  I may add this anyway for completeness.  JEG NOTE FIXME

    std::map<std::string,InputLineObject*>:: iterator clipit = cmdToClipMap.begin();
    cmdToClipMap.insert(clipit, std::pair< std::string, InputLineObject*>(input_command, input_clip));

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "EXIT addClipForThisCommand, ADDED THIS ITEM TO cmdToClipMap: input_command=" << input_command << " input_clip=" << input_clip << std::endl;
#endif

}

/**
 * StatsPanel::check_for_existing_clip:
 * Search for the clip corresponding to the input argument command string value to the 
 * cmdToClipMap map data structure.  This data structure keeps a running map of 
 * view commands to their corresponding clip.  We use the clip pointer to access
 * the command objects representing the view, so we don't have to request this data
 * from the CLI (and therefore the framework and database file).  We just reuse the
 * already requested data structures to regenerate the same view again.
 *
 * @return The clip value corresponding to the data representing the command.
 */

InputLineObject* StatsPanel::check_for_existing_clip(std::string command)
{
#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER check_for_existing_clip, command=" << command << std::endl;
  if ( ! cmdToClipMap.empty() ) {
    for (std::map<std::string,InputLineObject*>::iterator it=cmdToClipMap.begin(); it!=cmdToClipMap.end(); ++it) {
      std::cerr << "ENTER check_for_existing_clip, LOOP THROUGH cmdToClipMap, command:clip=(" << it->first << " " << ")" << std::endl;
    }
  }
#endif

    std::map<std::string,InputLineObject*>:: iterator clipit;

    if ( ! cmdToClipMap.empty() ) {

      // find the clip that corresponds to the command of interest
      clipit = cmdToClipMap.find(command);
      
      if (clipit != cmdToClipMap.end() && clipit->second) {

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT check_for_existing_clip, returning found value=" << clipit->second << std::endl;
#endif

        // Return the corresponding clip to be used in the regeneration of the requested view.
        return clipit->second;
      } else { 

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT check_for_existing_clip, returning NULL" << std::endl;
#endif

        return NULL;
      }
    } else {
        return NULL;
    }

    return NULL;
  
}

/**
  * StatsPanel::checkForExistingIntList:
  * Routine to checks to see if the list -v command already has a list of int's associated with it.
  * Search for the list of integer values corresponding to the input argument command string 
  * cmdToIntListMap map data structure.  This data structure keeps a running map of value to the 
  * view commands to their corresponding list values.  We use the list values to create 
  * parts of the view, so we don't have to request this data from the CLI (and therefore the 
  * framework and database file).  We just reuse the already requested data structures.
  *
  */

bool StatsPanel::checkForExistingIntList(std::string command, std::list<int64_t> & return_list)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER checkForExistingIntList, command=" << command << std::endl;

  if ( ! cmdToIntListMap.empty() ) {
    for (std::map<std::string,std::list<int64_t> >::iterator it=cmdToIntListMap.begin(); it!=cmdToIntListMap.end(); ++it) {
      std::cerr << "ENTER checkForExistingIntList, LOOP THROUGH cmdToIntListMap, command: listofints=(" << it->first << " " << ")" << std::endl;
    }
  }
#endif

    std::map<std::string,std::list<int64_t> >:: iterator clipit;

    if ( ! cmdToIntListMap.empty() ) {

      // Search for the list of integer values corresponding to the input argument command string 
      // cmdToIntListMap map data structure.  This data structure keeps a running map of value to the 
      // view commands to their corresponding list values.  We use the list values to create 
      // parts of the view, so we don't have to request this data from the CLI (and therefore the 
      // framework and database file).  We just reuse the already requested data structures.

      clipit = cmdToIntListMap.find(command);
      
      if (clipit != cmdToIntListMap.end() /* && *clipit.size() */) {

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT checkForExistingIntList, returning found value for command=" << command << std::endl;
#endif

        // Return the corresponding list to be used in the regeneration of the requested view.
        return_list = clipit->second;
        return true;

      } else { 

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT checkForExistingIntList, returning NULL" << std::endl;
#endif
        return false;
      }
    } else {
        return false;
    }
  return false;
}


/**
  * StatsPanel::checkForExistingStringList:
  * Routine to checks to see if the list -v command already has a list of string's associated with it.
  * Search for the list of string values corresponding to the input argument command string 
  * cmdToStringListMap map data structure.  This data structure keeps a running map of value to the 
  * view commands to their corresponding list values.  We use the list values to create 
  * parts of the view, so we don't have to request this data from the CLI (and therefore the 
  * framework and database file).  We just reuse the already requested data structures.
  */

bool StatsPanel::checkForExistingStringList(std::string command, std::list<std::string> &return_list)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER checkForExistingStringList, command=" << command << std::endl;
#endif

    std::map<std::string,std::list<std::string> >:: iterator clipit;

    if ( ! cmdToStringListMap.empty() ) {

      // Search for the list of integer values corresponding to the input argument command string 
      // cmdToStringListMap map data structure.  This data structure keeps a running map of value to the 
      // view commands to their corresponding list values.  We use the list values to create 
      // parts of the view, so we don't have to request this data from the CLI (and therefore the 
      // framework and database file).  We just reuse the already requested data structures.

      clipit = cmdToStringListMap.find(command);
      
      if (clipit != cmdToStringListMap.end() ) {

        // Return the corresponding list to be used in the regeneration of the requested view.
        return_list = clipit->second;

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT checkForExistingStringList, returning clipit->second" << std::endl;
#endif

        return true;

      } else { 

#ifdef DEBUG_StatsPanel_cache
        std::cerr << "EXIT checkForExistingStringList, returning NULL" << std::endl;
#endif
        return false;
      }
    } else {
        return false;
    }
  return false;
}

/**
  * StatsPanel::addIntListForThisCommand:
  * Routine that adds the list corresponding to the input argument command integer value to the 
  * cmdToIntListMap map data structure.  This data structure keeps a running map of 
  * view commands to their corresponding clip.  We use the clip pointer to access
  * the command objects representing the view, so we don't have to request this data
  * from the CLI (and therefore the framework and database file).  We just reuse the
  * already requested data structures to regenerate the same view again.
  */

void StatsPanel::addIntListForThisCommand(std::string command, std::list<int64_t> listToSave)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER addIntListForThisCommand, command=" << command << " listToSave=" << &listToSave << std::endl;
#endif

    // Add the list corresponding to the input argument command integer value to the 
    // cmdToIntListMap map data structure.  This data structure keeps a running map of 
    // view commands to their corresponding clip.  We use the clip pointer to access
    // the command objects representing the view, so we don't have to request this data
    // from the CLI (and therefore the framework and database file).  We just reuse the
    // already requested data structures to regenerate the same view again.

    // Note: we could check to see if the same view command already exists but existing logic
    // doesn't require this.  I may add this anyway for completeness.  JEG NOTE FIXME
    std::map<std::string,std::list<int64_t> >:: iterator clipit = cmdToIntListMap.begin();
    cmdToIntListMap.insert(clipit, std::pair< std::string, std::list<int64_t> >(command, listToSave));

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "EXIT addIntListForThisCommand, ADDED THIS ITEM TO cmdToClipMap: command=" << command << " listToSave=" << &listToSave << std::endl;
#endif

}

/**
  * StatsPanel::addStringListForThisCommand:
  * Routine that adds the list corresponding to the input argument command string value to the 
  * cmdToStringListMap map data structure.  This data structure keeps a running map of 
  * view commands to their corresponding clip.  We use the clip pointer to access
  * the command objects representing the view, so we don't have to request this data
  * from the CLI (and therefore the framework and database file).  We just reuse the
  * already requested data structures to regenerate the same view again.
  */

void StatsPanel::addStringListForThisCommand(std::string command, std::list<std::string> listToSave)
{

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "ENTER addStringListForThisCommand, command=" << command << " listToSave=" << &listToSave << std::endl;
#endif

    // Add the list corresponding to the input argument command string value to the 
    // cmdToStringListMap map data structure.  This data structure keeps a running map of 
    // view commands to their corresponding clip.  We use the clip pointer to access
    // the command objects representing the view, so we don't have to request this data
    // from the CLI (and therefore the framework and database file).  We just reuse the
    // already requested data structures to regenerate the same view again.

    // Note: we could check to see if the same view command already exists but existing logic
    // doesn't require this.  I may add this anyway for completeness.  JEG NOTE FIXME
    std::map<std::string,std::list<std::string> >:: iterator clipit = cmdToStringListMap.begin();
    cmdToStringListMap.insert(clipit, std::pair< std::string, std::list<std::string> >(command, listToSave));

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "EXIT addStringListForThisCommand, ADDED THIS ITEM TO cmdToClipMap: command=" << command << " listToSave=" << &listToSave << std::endl;
#endif

}

class AboutOutputClass : public ss_ostream
{
  public:
    StatsPanel *sp;
    void setSP(StatsPanel *_sp) { sp = _sp;line_buffer = QString::null; };
    QString line_buffer;
  private:
    virtual void output_string (std::string s)
    {
       line_buffer += s.c_str();
       if( QString(s.c_str()).contains("\n") )
       {
	 QString *data = new QString(line_buffer);
	 sp->outputAboutData(data);
	 line_buffer = QString::null;
       }
    }
    virtual void flush_ostream ()
    {
      qApp->flushX();
    }
};

void StatsPanel::clearModifiers()
{
  list_of_modifiers.clear(); // This is the global known list of modifiers.

  list_of_mpi_modifiers.clear();
  current_list_of_mpi_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_mpip_modifiers.clear();
  current_list_of_mpip_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_mpit_modifiers.clear();
  current_list_of_mpit_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_io_modifiers.clear();
  current_list_of_io_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_iot_modifiers.clear();
  current_list_of_iot_modifiers.clear();  // This is this list of user selected modifiers.

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clearModifiers() CLEARING current_list_of_iot_modifiers\n");
#endif

  list_of_hwc_modifiers.clear();
  current_list_of_hwc_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_hwcsamp_modifiers.clear();
  current_list_of_hwcsamp_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_hwctime_modifiers.clear();
  current_list_of_hwctime_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_pcsamp_modifiers.clear();
  current_list_of_pcsamp_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_usertime_modifiers.clear();
  current_list_of_usertime_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_omptp_modifiers.clear();
  current_list_of_omptp_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_iop_modifiers.clear();
  current_list_of_iop_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_mem_modifiers.clear();
  current_list_of_mem_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_pthreads_modifiers.clear();
  current_list_of_pthreads_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_fpe_modifiers.clear();
  current_list_of_fpe_modifiers.clear();  // This is this list of user selected modifiers.

  current_list_of_modifiers.clear();  // This is this list of user selected modifiers.

  IOtraceFLAG = FALSE;
  MPItraceFLAG = FALSE;
  delete sourcePanelAnnotationDialog;
  sourcePanelAnnotationDialog = NULL;
}

/*! Create a Stats Panel.
*/
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor entered\n");
#endif
  thisPC = pc;
  setCaption("StatsPanel");
  timeSegmentDialog = NULL;;
#if OPTIONAL_VIEW
  optionalViewsDialog = NULL;;
  sourcePanelAnnotationDialog = NULL;;
#endif
  chooseExperimentDialog = NULL;;

  metadataAllSpaceFrame = NULL;
  metadataAllSpaceLayout = NULL;
  metadataOneLineInfoLayout = NULL;

  metadataToolButton = NULL;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor CLEARING metadataToolButton=0x%lx\n", metadataToolButton);
#endif

  statspanel_clip = NULL;
  cmdToClipMap.clear();
  cmdToIntListMap.clear();
  cmdToStringListMap.clear();
  associatedCommandMap.clear();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif
  progressTimer = NULL;
  pd = NULL;

  IOtraceFLAG = FALSE;
  MPItraceFLAG = FALSE;

  insertDiffColumnFLAG = FALSE;
  focusedExpID = -1;

  compareExpIDs.clear();
  experimentGroupList.clear();

  currentThread = NULL;
  currentCollector = NULL;
  currentItem = NULL;
  currentItemIndex = 0;
  lastlvi = NULL;
  lastIndentLevel = 0;
  gotHeader = FALSE;
  fieldCount = 0;
  percentIndex = -1;
  gotColumns = FALSE;
  aboutOutputString = QString::null;
  aboutString = QString::null;

  currentDisplayUsingType = displayUsingFunctionType; // default compare type is by function
  currentDisplayUsingTypeStr = "functions"; // default current compare by string
  firstGenerateCommandCall = true;


  // for the metadata information header
  // uses some of what is in the aboutString string but format is different
  infoString = QString::null;
  infoAboutString = QString::null;
  infoAboutComparingString = QString::null;
  infoAboutStringCompareExpIDs = QString::null;

  lastAbout = QString::null;
  timeIntervalString = QString::null;
  prevTimeIntervalString = QString::null;

  // In an attempt to optimize the update of this panel;
  // If the data file is static (i.e. read from a file or 
  // the processes status is terminated) and the command is
  // the same, don't update this panel. 
  originalCommand = QString::null;
  lastCommand = QString::null;
  lastCurrentThreadsStr = QString::null; 
  staticDataFLAG = false;

#ifdef DEBUG_StatsPanel
  printf("currentItemIndex initialized to 0\n");
#endif

  f = NULL;
  modifierMenu = NULL;
  experimentsMenu = NULL;

  mpiModifierMenu = NULL;
  mpipModifierMenu = NULL;
  mpitModifierMenu = NULL;
  ioModifierMenu = NULL;
  iopModifierMenu = NULL;
  memModifierMenu = NULL;
  pthreadsModifierMenu = NULL;
  iotModifierMenu = NULL;
  hwcModifierMenu = NULL;
  hwcsampModifierMenu = NULL;
  hwctimeModifierMenu = NULL;
  pcsampModifierMenu = NULL;
  usertimeModifierMenu = NULL;
  omptpModifierMenu = NULL;
  fpeModifierMenu = NULL;

  iop_menu = NULL;
  mem_menu = NULL;
  pthreads_menu = NULL;
  mpi_menu = NULL;
  io_menu = NULL;
  hwc_menu = NULL;
  hwcsamp_menu = NULL;
  hwctime_menu = NULL;
  pcsamp_menu = NULL;
  usertime_menu = NULL;
  omptp_menu = NULL;
  fpe_menu = NULL;

#if 1
  clearModifiers();
#else
  list_of_modifiers.clear(); // This is the global known list of modifiers.

  list_of_mpi_modifiers.clear();
  current_list_of_mpi_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_mpip_modifiers.clear();
  current_list_of_mpip_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_mpit_modifiers.clear();
  current_list_of_mpit_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_io_modifiers.clear();
  current_list_of_io_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_iot_modifiers.clear();
  current_list_of_iot_modifiers.clear();  // This is this list of user selected modifiers.

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor CLEARING current_list_of_iot_modifiers AND OTHERS\n");
#endif

  list_of_hwc_modifiers.clear();
  current_list_of_hwc_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_hwcsamp_modifiers.clear();
  current_list_of_hwcsamp_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_hwctime_modifiers.clear();
  current_list_of_hwctime_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_pcsamp_modifiers.clear();
  current_list_of_pcsamp_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_usertime_modifiers.clear();
  current_list_of_usertime_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_omptp_modifiers.clear();
  current_list_of_omptp_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_fpe_modifiers.clear();
  current_list_of_fpe_modifiers.clear();  // This is this list of user selected modifiers.

  current_list_of_modifiers.clear();  // This is this list of user selected modifiers.
#endif
  demangled_mangled_vector.clear();

  selectedFunctionStr = QString::null;
  threadMenu = NULL;
  currentMetricStr = QString::null;
  sourcePanelMetricStr = QString::null;
  currentUserSelectedReportStr = QString::null;
  lastUserSelectedReportStr = QString::null;
  originatingUserSelectedReportStr = QString::null;
  traceAddition = QString::null;
  metricHeaderTypeArray = NULL;
  currentThreadsStr = QString::null;
  currentMenuThreadsStr = QString::null;
  currentCollectorStr = QString::null;
  lastCollectorStr = QString::null;
  infoSummaryStr = QString::null;
  collectorStrFromMenu = QString::null;
  groupID = ao->int_data;
  expID = -1;
#ifdef DEBUG_Sorting
  printf("StatsPanel::StatsPanel set descending_sort to true\n");
#endif
  descending_sort = true;
  TotalTime = 0;
  maxEntryBasedOnTotalTime = 0;

  if( ao->loadedFromSavedFile == TRUE ) {

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel static data!!!\n");
#endif

    staticDataFLAG = TRUE;
  }

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");
  splitterA->setOrientation( QSplitter::Vertical );
//  splitterA->setOrientation( QSplitter::Horizontal );

  int Awidth = pc->width();
  int Aheight = pc->height();

#ifdef DEBUG_StatsPanel
  printf("in Splitter section of code, Awidth=%d, Aheight=%d\n", Awidth, Aheight);
#endif

  QValueList<int> AsizeList;
  AsizeList.clear();
  if( splitterA->orientation() == QSplitter::Vertical ) {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is vertical, (Aheight-(int)(Aheight/5))=%d, (Aheight/5)=%d\n", (Aheight-(int)(Aheight/5)), (Aheight/5));
#endif
    AsizeList.push_back((int)(Aheight/5));
    AsizeList.push_back(Aheight-(int)(Aheight/5));
  } else {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is horizontal, (Awidth/5)=%d, (Awidth-(int)(Awidth/5))=%d\n", (Awidth/5), (Awidth-(int)(Awidth/5)));
#endif
    AsizeList.push_back((int)(Awidth/5));
    AsizeList.push_back(Awidth-(int)(Awidth/5));
  }
  splitterA->setSizes(AsizeList);
// NEW HEADER CODE

  metadataAllSpaceFrame = new QFrame( splitterA, "metadataAllSpaceFrame" );
  metadataAllSpaceLayout = new QVBoxLayout( metadataAllSpaceFrame );

  metadataOneLineInfoLayout = new QHBoxLayout( metadataAllSpaceLayout, 0, "metadataOneLineInfoLayout" );
//  metadataOneLineInfoLayout->setMargin(1);

  metadataAllSpaceFrame->setMinimumSize( QSize(0,0) );
  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
#ifdef DEBUG_StatsPanel
  printf("stats panel metadataAllSpaceFrame->height()=%d\n", metadataAllSpaceFrame->height());
#endif
  metadataAllSpaceFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

#if MORE_BUTTON
  infoEditHeaderMoreButton = new QPushButton( metadataAllSpaceFrame, "infoEditHeaderMoreButton" );
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
  infoEditHeaderMoreButton->setEnabled(TRUE);
  infoEditHeaderMoreButton->setMinimumSize( QSize(0,0) );
  infoEditHeaderMoreButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, infoEditHeaderMoreButton->sizePolicy().hasHeightForWidth() ) );
#else
  metaToolBar = new QToolBar( QString("label"), getPanelContainer()->getMainWindow(), metadataAllSpaceFrame, "file operations" );
  metaToolBar->setOrientation( Qt::Horizontal );
  metaToolBar->setLabel( "Metadata Operations" );

//  QPixmap *MoreMetadata_icon = new QPixmap(meta_information_plus_xpm);
//  new QToolButton(*MoreMetadata_icon, "Show More Experiment Metadata", 
//                  QString::null, this, SLOT( infoEditHeaderMoreButtonSelected()), 
//                  metaToolBar, "show more experiment metadata");
#endif

  infoEditHeaderLabel = new QLabel( metadataAllSpaceFrame, "info label", 0 );
  infoEditHeaderLabel->setCaption("StatsPanel: info label");
  infoEditHeaderLabel->setPaletteBackgroundColor( QColor(Qt::cyan).light(185) );
//  infoEditHeaderLabel->setPaletteBackgroundColor( QColor(Qt::skyblue).light(145) );
//  infoEditHeaderLabel->setPaletteBackgroundColor( QColor(Qt::blue).light(185));
#ifdef DEBUG_StatsPanel
  printf("stats panel 2nd infoEditHeaderLabel->height()=%d\n", infoEditHeaderLabel->height());
#endif

  QString label_text = QString("There is no known information about this experiment's statistics available\n");

  infoEditHeaderLabel->setText(label_text);
  infoEditHeaderLabel->setMinimumSize( QSize(0,0) );
  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
  infoEditHeaderLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );
  QToolTip::add( infoEditHeaderLabel, tr( "This quick summary line shows partial information about the executable(s),\nhost(s), and process(es) used in creating the experiment performance\ndata displayed below.  To see more complete information use the\nexperiment metadata icon above to toggle between the\nsummary and expanded information." ) );

#ifdef DEBUG_StatsPanel
  printf("stats panel 3rd infoEditHeaderLabel->height()=%d\n", infoEditHeaderLabel->height());
#endif

  infoButtonAndLabelLayout = new QHBoxLayout( 0, 0, 0, "infoButtonAndLabelLayout");
#if MORE_BUTTON
  infoButtonAndLabelLayout->addWidget(infoEditHeaderMoreButton);
#endif
  infoButtonAndLabelLayout->addWidget(infoEditHeaderLabel);
  metadataOneLineInfoLayout->addLayout( infoButtonAndLabelLayout );

//#ifdef TEXT
  metaDataTextEdit = new SPTextEdit( this, metadataAllSpaceFrame );
  metaDataTextEdit->setCaption("SourcePanel: SPTextEdit");
  metaDataTextEdit->setHScrollBarMode( QScrollView::AlwaysOn );
  metaDataTextEdit->setVScrollBarMode( QScrollView::AlwaysOn );
  metadataAllSpaceLayout->addWidget( metaDataTextEdit );
  metaDataTextEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  addWhatsThis(metaDataTextEdit, this);

  metaDataTextEdit->setTextFormat(PlainText);  // This makes one para == 1 line.
  metaDataTextEdit->setReadOnly(TRUE);
  metaDataTextEdit->setWordWrap(QTextEdit::NoWrap);
  vscrollbar = metaDataTextEdit->verticalScrollBar();
  hscrollbar = metaDataTextEdit->horizontalScrollBar();
  if( vscrollbar )
  {
    connect( vscrollbar, SIGNAL(valueChanged(int)),
           this, SLOT(valueChanged(int)) );
  }

  defaultColor = metaDataTextEdit->color();

  connect( metaDataTextEdit, SIGNAL(clicked(int, int)),
           this, SLOT(clicked(int, int)) );

  QValueList<int> metaDataTextEditsizeList;
  metaDataTextEditsizeList.clear();
  int metaDataTextEditwidth = pc->width();
  int metaDataTextEditleft_side_size = (int)(metaDataTextEditwidth/4);
  if( DEFAULT_CANVAS_WIDTH < metaDataTextEditleft_side_size )
  {
    metaDataTextEditleft_side_size = DEFAULT_CANVAS_WIDTH;
  }
  metaDataTextEditsizeList.push_back( metaDataTextEditleft_side_size );
  metaDataTextEditsizeList.push_back( metaDataTextEditwidth-metaDataTextEditleft_side_size );
//jeg 9-1807  metaDataTextEdit->show();
  metaDataTextEdit->setFocus();
  metaDataTextEdit->hide();
  metaDataTextEditFLAG = FALSE;
//#endif

//#endif

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 0, 0, getName() );
//  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

// END NEW HEADER CODE

  splitterB = new QSplitter( splitterA, "splitterB");
  splitterB->setCaption("StatsPanelSplitterB");
  splitterB->setOrientation( QSplitter::Horizontal );
#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB created as Horizontal\n");
#endif

  cf = new SPChartForm(this, splitterB, getName(), 0);
  cf->setCaption("SPChartFormIntoSplitterA");

  splv = new SPListView(this, splitterB, getName(), 0);
#ifdef DEBUG_Sorting
  printf("StatsPanel:: splitterB calling setSorting with FALSE\n");
#endif
  splv->setSorting ( 0, FALSE );
  splv->setShowSortIndicator ( TRUE );

  sml =new QLabel(splitterB,"stats_message_label");
  sml->setText("There were no data samples for this experiment execution.\nPossible reasons for this could be:\n   The executable being run didn't run long enough to record performance data.\n   The type of performance data being gathered may not be present in the executable being executed.\n   The executable was not compiled with debug symbols enabled (-g option or variant).\n");
  // Hide this and only show it when we don't see any performance data samples
  sml->hide();


#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB being use as chart form holder\n");
#endif
#ifdef DEBUG_StatsPanel_chart
  printf("StatsPanel:: splitterB being use as chart form holder\n");
#endif


#ifdef OLDWAY
  QHeader *header = splv->header();
  header->setMovingEnabled(FALSE);
  header->setClickEnabled(TRUE);
  connect( header, SIGNAL(clicked(int)), this, SLOT( headerSelected( int )) );
#endif // OLDWAY

  connect( splv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

  connect( splv, SIGNAL(returnPressed(QListViewItem *)), this, SLOT( returnPressed( QListViewItem* )) );

  int Bwidth = pc->width();
  int Bheight = pc->height();

#ifdef DEBUG_StatsPanel
  printf("in Splitter section of code, Bwidth=%d, Bheight=%d\n", Bwidth, Bheight);
#endif

  QValueList<int> BsizeList;
  BsizeList.clear();
  if( splitterB->orientation() == QSplitter::Vertical ) {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is vertical, (Bheight-(int)(Bheight/4)=%d, (Bheight/4)=%d\n", (Bheight-(int)(Bheight/4), (Bheight/4)));
#endif
    BsizeList.push_back((int)(Bheight/4));
    BsizeList.push_back(Bheight-(int)(Bheight/4));
  } else {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is horizontal, (Bwidth-(int)(Bwidth/4)=%d, (Bwidth/4)=%d\n", (Bwidth-(int)(Bwidth/4), (Bwidth/4)));
#endif
    BsizeList.push_back((int)(Bwidth/4));
    BsizeList.push_back(Bwidth-(int)(Bwidth/4));
  }
  splitterB->setSizes(BsizeList);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB is set to setSizes(BsizeList)\n");
#endif


#ifdef DEBUG_StatsPanel
  bool debug_toolbarFLAG = getPreferenceShowToolbarCheckBox();
  printf("StatsPanel::StatsPanel debug_toolbarFLAG from getPreferenceShowToolbarCheckBox()=%d\n", debug_toolbarFLAG);
#endif

// Begin - Move to Panel.cxx
  fileTools = new QToolBar( QString("label"), getPanelContainer()->getMainWindow(), (QWidget *)getBaseWidgetFrame(), "file operations" );
  fileTools->setOrientation( Qt::Horizontal );
  fileTools->setLabel( "File Operations" );
//  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );
  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );
// End - Move to Panel.cxx

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel, addWidget(fileTools=0x%lx)\n", fileTools);
#endif


  // default setting to match default views
  //   toolbar_status_label->setText("");
  //     fileTools->setStretchableWidget(toolbar_status_label);
  //
  //     #endif 

  generateBaseToolBar(QString::null);

  frameLayout->addWidget(fileTools);
//  frameLayout->addWidget(metadataAllSpaceFrame);

  // Show statistics metadata information initially

  if( getPreferenceShowMetadataCheckBox() == TRUE ) {
    infoHeaderFLAG = TRUE;
    metadataAllSpaceFrame->show();
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::StatsPanel, SHOW metadataAllSpaceFrame, infoHeaderFLAG=%d)\n", infoHeaderFLAG);
#endif
  } else {
    infoHeaderFLAG = FALSE;
    metadataAllSpaceFrame->hide();
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::StatsPanel, HIDE metadataAllSpaceFrame, infoHeaderFLAG=%d)\n", infoHeaderFLAG);
#endif
  }

  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
    toolBarFLAG = TRUE;
    fileTools->show();

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::StatsPanel, (636) SHOW,fileTools=0x%lx)\n", fileTools);
#endif

  } else {

    toolBarFLAG = FALSE;
    fileTools->hide();

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::StatsPanel, (637) HIDE,fileTools=0x%lx)\n", fileTools);
#endif

  }

//  frameLayout->addLayout( metadataAllSpaceLayout );
  frameLayout->addWidget( splitterA );

#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::StatsPanel, before checks getMainWindow->preferencesDialog->showGraphicsCheckBox, chartFLAG=%d\n", chartFLAG);
#endif

  if( pc->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() ) {

    chartFLAG = TRUE;

#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::StatsPanel, insides checks getMainWindow->preferencesDialog->showGraphicsCheckBox, setting TRUE, chartFLAG=%d\n", chartFLAG);

#endif

    cf->show();
  } else {
#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::StatsPanel, insides checks getMainWindow->preferencesDialog->showGraphicsCheckBox, setting FALSE, chartFLAG=%d\n", chartFLAG);
#endif
    chartFLAG = FALSE;
    cf->hide();
  }

if( !getChartTypeComboBox() ) {
  cf->setChartType((ChartType)0);
} else {
  cf->setChartType((ChartType)getChartTypeComboBox());
}
  statsFLAG = TRUE;
  splv->show();

  splitterA->show();

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), groupID);
  setName(name_buffer);

  // Initialize the experiment id for the info header already processed
  // to something that can't occur as the first value.
  setHeaderInfoAlreadyProcessed(-1);

  // ----------------------------------------------------
  // SIGNAL SETUP AREA
  // ---------------------------------------------------

#if DEBUG_INTRO
  printf("StatsPanel::StatsPanel() constructor, start connect section of code\n");
#endif
#if MORE_BUTTON
  connect( infoEditHeaderMoreButton, SIGNAL( clicked() ), this, SLOT( infoEditHeaderMoreButtonSelected() ) );
#endif

#if DEBUG_StatsPanel
  printf("EXIT StatsPanel::StatsPanel() constructor\n");
#endif

}



/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanel::~StatsPanel()
{
  // Delete anything you new'd from the constructor.
  nprintf( DEBUG_CONST_DESTRUCT ) ("  StatsPanel::~StatsPanel() destructor called\n");

#ifdef DEBUG_StatsPanel
  std::cerr << "IN StatsPanel::~StatsPanel() destructor called." << std::endl; 
#endif


  // We must reset the directing of output, otherwise the cli goes nuts
  // trying to figure out where the output is suppose to go.
  resetRedirect();

  // We must clean-up the cached data objects used for saving and restoring clips and their command objects
  // which allow the GUI to not have to access the CLI after the initial view (for each view) is created.
  for (std::map<std::string,InputLineObject*>::iterator it=cmdToClipMap.begin(); it!=cmdToClipMap.end(); ++it) {
#ifdef DEBUG_StatsPanel_cache
    std::cerr << "IN StatsPanel::~StatsPanel(), LOOP THROUGH cmdToClipMap, setting RESULTS USED for command:clip=(" 
              << it->first << ": " << it->second << ")" << std::endl;
#endif
    it->second->Set_Results_Used();
  }

  // We allocated a Collect, we must delete it.   Otherwise the framework
  // issues a warning on exit.
  if( currentCollector ) {

#ifdef DEBUG_StatsPanel
   std::cerr << "IN StatsPanel::~StatsPanel(), Destructor delete the currentCollector" << std::endl; 
#endif

    delete currentCollector;
  }

#ifdef DEBUG_StatsPanel
  printf("  StatsPanel::~StatsPanel() destructor finished\n");
#endif

}

int
StatsPanel::findExperimentID(QString command)
{

 if (command.isEmpty() ) {
   return -1;
 } else {
   int start_index = command.find("-x");
   if( start_index != -1 ) {
     QString s = command.mid(start_index+3);

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::findExperimentID, Have a -x in the command s=(%s)\n", s.ascii() );
#endif // DEBUG_StatsPanel

     int end_index = s.find(" ");
     if( end_index == -1 ) {
       end_index == 99999;
     }

     QString exp_x = s.mid(0, end_index);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::findExperimentID, exp_x=%s\n", exp_x.ascii() );
#endif // DEBUG_StatsPanel

     int expID = exp_x.toInt();
     return (expID);
  }
 }
}

/*! The user clicked.  -unused. */
void
StatsPanel::clicked(int para, int offset)
{
  nprintf(DEBUG_PANELS) ("You clicked?\n");
}


/*! The value changed... That means we've scrolled.   
    Should not have to do anything for the statspanel */
void
StatsPanel::valueChanged(int passed_in_value)
{
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::valueChanged(%d), ignoring\n", passed_in_value );
#endif
   return;

}


void StatsPanel::infoEditHeaderMoreButtonSelected()
{

#if DEBUG_INTRO
  printf("Enter StatsPanel::infoEditHeaderMoreButtonSelected(), metaDataTextEditFLAG=%d\n",metaDataTextEditFLAG);
#endif

//#ifdef TEXT
  if (metaDataTextEditFLAG) {

    metaDataTextEdit->hide();
    metaDataTextEditFLAG = FALSE;
#if MORE_BUTTON
    infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
    infoEditHeaderMoreButton->setEnabled(TRUE);
#else

#if DEBUG_INTRO
    printf("Enter StatsPanel::infoEditHeaderMoreButtonSelected(), this=0x%lx, metadataToolButton=0x%lx\n", 
           this, metadataToolButton);
#endif

    metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
    metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
    QToolTip::add( metadataToolButton, tr( "Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
#endif

  } else {

#if DEBUG_INTRO
    printf("SHOW StatsPanel::infoEditHeaderMoreButtonSelected(), this=0x%lx, metadataToolButton=0x%lx\n", 
           this, metadataToolButton);
#endif

    metaDataTextEdit->setCursorPosition(0, 0);
    metaDataTextEdit->show();
    metaDataTextEditFLAG = TRUE;
#if MORE_BUTTON
    infoEditHeaderMoreButton->setText( tr( "Less Metadata" ) );
    infoEditHeaderMoreButton->setEnabled(TRUE);
#else
    metadataToolButton->setIconSet( QIconSet(*LessMetadata_icon));
    metadataToolButton->setIconText(QString("Show Less Experiment Metadata"));
    QToolTip::add( metadataToolButton, tr( "Push to hide the expanded experiment metadata information." ) );
#endif
  } 
//#endif
//
  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
#if MORE_BUTTON
// maybe move the above into here - I did but commented out for now
//  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
//  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
#else
  metaToolBar->update();
#endif

#if DEBUG_INTRO
    printf("Exit StatsPanel::infoEditHeaderMoreButtonSelected()\n");
#endif

}



void
StatsPanel::raiseManageProcessesPanel()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::raiseManageProcessesPanel(), expID=%d\n", expID);
#endif

  QString name = QString("ManageProcessesPanel [%1]").arg(expID);

  Panel *manageProcessesPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessesPanel ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::raiseManageProcessesPanel(), RAISING MANAGE PROCESS PANEL, expID=%d\n", expID);
#endif
    getPanelContainer()->raisePanel(manageProcessesPanel);
  }

}



void
StatsPanel::languageChange()
{
  // Set language specific information here.
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::languageChange() SETTING metadataToolButton=0x%lx, setIconSet\n", metadataToolButton);
#endif

#if MORE_BUTTON
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
  infoEditHeaderMoreButton->setEnabled(TRUE);
  QToolTip::add( infoEditHeaderMoreButton, tr( "Push for more or less complete experiment metadata (information about the experiment performance data)." ) );
#else
  metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
  QToolTip::add( metadataToolButton, tr( "Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
  metaToolBar->update();
#endif
}


void
StatsPanel::displayUsingFunction()
{
#ifdef DEBUG_StatsPanel_toolbar 
  printf("StatsPanel::displayUsingFunction() entered, currentUserSelectedReportStr=%s, originatingUserSRS=%s\n",
         currentUserSelectedReportStr.ascii(), originatingUserSelectedReportStr.ascii());
#endif

  currentDisplayUsingTypeStr = "functions";
  currentDisplayUsingType = displayUsingFunctionType;

#if 0
  // Comment out this code.  It used to be used when clicking on the display type 
  // would automatically call the previously selected view, but now we have reversed 
  // the method to select the view type and then hit the type of report.
  if (originatingUserSelectedReportStr.startsWith("minMaxAverage") ) {

//     minMaxAverageSelected();

  } else if (originatingUserSelectedReportStr.startsWith("clusterAnalysis") &&
             currentUserSelectedReportStr.startsWith("Comparison" ) ) {

//     clusterAnalysisSelected();

  } else if (originatingUserSelectedReportStr.startsWith("DefaultView") &&
            (currentUserSelectedReportStr.startsWith("Functions") ) || 
            (currentUserSelectedReportStr.startsWith("Statements") ) || 
#if defined(HAVE_DYNINST)
            (currentUserSelectedReportStr.startsWith("Loops") ) || 
#endif
            (currentUserSelectedReportStr.startsWith("LinkedObjects") ) ) {

//     defaultViewSelected();

  }
#endif
}

void
StatsPanel::displayUsingStatement()
{
#ifdef DEBUG_StatsPanel_toolbar 
  printf("StatsPanel::displayUsingStatement() entered, currentUserSelectedReportStr=%s, originatingUserSRS=%s\n",
         currentUserSelectedReportStr.ascii(), originatingUserSelectedReportStr.ascii());
#endif
  currentDisplayUsingTypeStr = "statements";
  currentDisplayUsingType = displayUsingStatementType;
}

void
StatsPanel::displayUsingLinkedObject()
{
#ifdef DEBUG_StatsPanel_toolbar 
  printf("StatsPanel::displayUsingLinkedObject() entered, currentUserSelectedReportStr=%s, originatingUserSRS=%s\n",
         currentUserSelectedReportStr.ascii(), originatingUserSelectedReportStr.ascii());
#endif
  currentDisplayUsingTypeStr = "linkedobjects";
  currentDisplayUsingType = displayUsingLinkedObjectType;
}


#if defined(HAVE_DYNINST)
void
StatsPanel::displayUsingLoop()
{
#ifdef DEBUG_StatsPanel_toolbar 
  printf("StatsPanel::displayUsingLoop() entered, currentUserSelectedReportStr=%s, originatingUserSRS=%s\n",
         currentUserSelectedReportStr.ascii(), originatingUserSelectedReportStr.ascii());
#endif
  currentDisplayUsingTypeStr = "loops";
  currentDisplayUsingType = displayUsingLoopType;
}

#endif

//
// Call to the CLI with the list -v mangled command
// Then use the results to fill in the std::vector with the demangled 
// and mangled names from the list -v mangled command
//
void StatsPanel::getDemangledMangledNames(int exp_id)
{

// Now get the list of mangled and demangled names

 QString command = QString::null;

 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 ) {
    command = QString("list -v mangled -x %1").arg(exp_id);
  } else {
    command = QString("list -v mangled -x %1").arg(focusedExpID);
  }

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  std::list<std::string> list_of_demangled_mangled_names;
  list_of_demangled_mangled_names.clear();

  bool list_is_cached = checkForExistingStringList( command.ascii(), list_of_demangled_mangled_names);

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_demangled_mangled_names.clear();
    InputLineObject *clip = NULL;

    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_demangled_mangled_names, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    addStringListForThisCommand(command.ascii(), list_of_demangled_mangled_names);
  }

  if( list_of_demangled_mangled_names.size() > 1 ) {
    for( std::list<std::string>::const_iterator it = list_of_demangled_mangled_names.begin();
                                                it != list_of_demangled_mangled_names.end(); it++ ) {
      std::string mangled_demangled_item = *it;
      std::string mangled = mangled_demangled_item.substr(mangled_demangled_item.find(";") + 1);
      std::string demangled = mangled_demangled_item.substr(0, mangled_demangled_item.find(";", 0));

#ifdef DEBUG_StatsPanel
      std::cerr << "StatsPanel::getDemangledMangledNames, \n"
                <<  " mangled="  << mangled << "\n"
                <<  " demangled=" << demangled << "\n"
                <<  " mangled_demangled_item=" << mangled_demangled_item << std::endl;
#endif

      // Fill in the std::vector with the demangled and mangled names from the list -v mangled command
      // This will be used when a function name is required for one of the GUI actions.
      // We will try to match the demangled name and then return the mangled to use for calls to the CLI
      // because the CLI can't handle :: syntax.
      demangled_mangled_vector.push_back(std::make_pair(demangled, mangled));

    }
  }
 } 

}

//
// This function is called when a function name is required for one of the GUI actions.
// We will try to match the demangled name and then return the mangled to use for calls to the CLI
// because the CLI can't handle :: syntax.
// First match the sizes, that should eliminate some search time.
// Then look for a match:  string to string
//
QString StatsPanel::findMangledNameForCLIcommand(std::string inputFunctionStr)
{
#ifdef DEBUG_StatsPanel
    std::cerr << "ENTER StatsPanel::findMangledNameForCLIcommand, inputFunctionStr=" << inputFunctionStr
              << " inputFunctionStr.length()=" << inputFunctionStr.length() << std::endl;
#endif

    for(std::vector<std::pair<std::string, std::string> >::iterator j = demangled_mangled_vector.begin(); 
                                                                     j != demangled_mangled_vector.end(); ++j) {
        //std::cerr << "StatsPanel::findMangledNameForCLIcommand, j->first.size()=" << j->first.size() 
        //          << " inputFunctionStr.length()=" << inputFunctionStr.length()
        //          << " j->first=" << j->first 
        //          << " j->second=" << j->second 
        //          << std::endl;

        if (j->first.size() == inputFunctionStr.length() && 
            (j->first.compare(inputFunctionStr) == 0)) {

           //std::cerr << "StatsPanel::findMangledNameForCLIcommand, return=" << j->second 
           //          << " inputFunctionStr=" << inputFunctionStr << std::endl;

           return QString(j->second) ;
        }               
    }               
    return QString::null;
}

/*! When a message has been sent (from anyone) and the message broker is
    notifying panels that they may want to know about the message, this is the
    function the broker notifies.   The listener then needs to determine
    if it wants to handle the message.
    \param msg is the incoming message.
    \return 0 means you didn't do anything with the message.
    \return 1 means you handled the message.
 */
#include "SaveAsObject.hxx"
int 
StatsPanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

  MessageObject *msgObject = (MessageObject *)msg;

#ifdef DEBUG_StatsPanel
   printf("Enter StatsPanel::listener(&msg = 0x%x), msgObject->msgType.ascii()=%s , recycleFLAG=%d\n", 
           &msg, msgObject->msgType.ascii() , recycleFLAG );
#endif

  nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() msg->msgType = (%s)\n", msgObject->msgType.ascii() );

  if( msgObject->msgType == getName() && recycleFLAG == TRUE ) {

    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() interested!\n");

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener-1-(%s)\n", msgObject->msgType.ascii() );
   printf("StatsPanel::listener-1- RAISING THIS PANEL this=%d, (getName()=%s)\n", this, getName() );
#endif

    getPanelContainer()->raisePanel(this);
    return 1;
  }

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener-2-(%s)\n", msgObject->msgType.ascii() );
   if (msgObject) msgObject->print();
#endif

 if(  msgObject->msgType  == "PrepareToRerun" ) {

   // ----------------------------
   // ---------------------------- PREPARE-TO-RERUN
   // ----------------------------
   // In an attempt to optimize the update of this panel;
   // If the data file is static (i.e. read from a file or
   // the processes status is terminated) and the command is
   // the same, don't update this panel.
   // FOR RERUN - need to clear lastCommand so it does update
#ifdef DEBUG_StatsPanel
    printf("StatsPanel in PrepareToRerun\n");
#endif // DEBUG_StatsPanel
   recycleFLAG = FALSE;
   originalCommand = QString::null;
   lastCommand = QString::null;
   lastCurrentThreadsStr = QString::null; 
   staticDataFLAG = false;
   raiseManageProcessesPanel();
   ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
   if( eo && eo->FW() ) {
     Experiment *fw_experiment = eo->FW();
#ifdef DEBUG_StatsPanel
     printf("StatsPanel in PrepareToRerun, fw-experiment->rerun_count=%d\n", fw_experiment->getRerunCount());
#endif // DEBUG_StatsPanel
   }

  } else if(  msgObject->msgType  == "FocusObject" && recycleFLAG == TRUE ) {

   // ---------------------------- 
   // ---------------------------- FOCUS-OBJECT
   // ----------------------------
   //
#ifdef DEBUG_StatsPanel
    printf("StatsPanel got a new FocusObject\n");
#endif // DEBUG_StatsPanel

    FocusObject *msg = (FocusObject *)msgObject;

#ifdef DEBUG_StatsPanel
// msg->print();
#endif // DEBUG_StatsPanel

    expID = msg->expID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, FocusObject, B: expID = %d\n", expID);
    printf("StatsPanel::listener, FocusObject, B: msg->threadNameString.isEmpty() = %d\n", msg->threadNameString.isEmpty());
#endif // DEBUG_StatsPanel

#if 0
    if( msg->host_pid_vector.size() == 0 && !msg->pidString.isEmpty() )
    { // Soon to be obsoleted
      currentThreadsStr = msg->pidString;
      currentThreadGroupStrList.clear();
      QString ws = msg->pidString;
      int cnt = ws.contains(",");
      if( cnt > 1 )
      {
        for(int i=0;i<=cnt;i++)
        {
          currentThreadGroupStrList.push_back( ws.section(",", i, i) ); 
        }
      } else
      {
        if( !ws.isEmpty() )
        {
          currentThreadGroupStrList.push_back( ws );
        }
      }
    } else {
#else
      currentThreadGroupStrList.clear();
      currentThreadsStr = QString::null;
      if( msg->descriptionClassList.count() > 0 ) {

#ifdef DEBUG_StatsPanel
       printf("StatsPanel::listener, FocusObject, Focusing with the new (more robust) syntax.\n");
#endif // DEBUG_StatsPanel

        for( QValueList<DescriptionClassObject>::iterator it = msg->descriptionClassList.begin(); it != msg->descriptionClassList.end(); it++)
          {
            DescriptionClassObject dco = (DescriptionClassObject)*it;
           if( !dco.tid_name.isEmpty() && !msg->focusOnRankOnlyFLAG) {
             if ( !dco.rid_name.isEmpty() ) {
               currentThreadsStr += QString(" -h %1 -t %2 -r %3").arg(dco.host_name).arg(dco.tid_name).arg(dco.rid_name);
             } else {
               currentThreadsStr += QString(" -h %1 -t %2").arg(dco.host_name).arg(dco.tid_name);
             }
           } else if( !dco.rid_name.isEmpty() && msg->focusOnRankOnlyFLAG ) {
             currentThreadsStr += QString(" -h %1 -r %2").arg(dco.host_name).arg(dco.rid_name);
           } else // pid...
           {
             currentThreadsStr += QString(" -h %1 -p %2").arg(dco.host_name).arg(dco.pid_name);
           }
#ifdef DEBUG_StatsPanel
           printf("StatsPanel::listener, FocusObject, Building currentThreadsStr.ascii()=%s\n", currentThreadsStr.ascii() );
#endif // DEBUG_StatsPanel
     
         }
     }
#endif
#if 0
     } else {

#ifdef DEBUG_StatsPanel
       printf("StatsPanel::listener, FocusObject, Here in StatsPanel::listener()\n");
       msg->print();
#endif // DEBUG_StatsPanel

      currentThreadGroupStrList.clear();
      currentThreadsStr = QString::null;
      std::vector<HostPidPair>::const_iterator sit = msg->host_pid_vector.begin();
      for(std::vector<HostPidPair>::const_iterator
                      sit = msg->host_pid_vector.begin();
                      sit != msg->host_pid_vector.end(); ++sit)
      {
        if( sit->first.size() ) {
          currentThreadsStr += QString(" -h %1 -p %2").arg(sit->first.c_str()).arg(sit->second.c_str() );
        } else {
          currentThreadsStr += QString(" -p %1").arg(sit->second.c_str() );
        }
        currentThreadGroupStrList.push_back( sit->second.c_str() );
      }
     }
    }
#endif

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener, FocusObject, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif // DEBUG_StatsPanel

#if 1
// Begin determine if there's mpi stats
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() ) {
        Experiment *fw_experiment = eo->FW();
        CollectorGroup cgrp = fw_experiment->getCollectors();

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::listener, FocusObject, The FW says you have %d collectors.\n", cgrp.size() );
#endif

        if( cgrp.size() == 0 ) {
          fprintf(stderr, "There are no known collectors for this experiment.\n");
        }

        for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
        {
          Collector collector = *ci;
          Metadata cm = collector.getMetadata();
          QString name = QString(cm.getUniqueId().c_str());

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::listener, FocusObject, B: Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
#endif 

        }
      }
    }
    catch(const std::exception& error)
    { 
      std::cerr << std::endl << "Error: "
                << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                "Unknown runtime error." : error.what()) << std::endl
                << std::endl;
      QApplication::restoreOverrideCursor( );
      return FALSE;
    }
// End determine if there's mpi stats
#else
   // want to try list -v collector here?
#endif

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener FocusObject, call updateStatsPanelData  Do we need to update?\n");
#endif // DEBUG_StatsPanel

    if (originatingUserSelectedReportStr.startsWith("clusterAnalysis") &&
      currentUserSelectedReportStr.startsWith("Comparison")) {
      clusterAnalysisSelected();
    } else {
      updateStatsPanelData(DONT_FORCE_UPDATE);
    }
    if( msg->raiseFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener FocusObject, after calling updateStatsPanelData  RAISE THIS PANEL, this=%d\n", this);
#endif

      getPanelContainer()->raisePanel(this);
    }
// now focus a source file that's listening....

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusObject, Now focus the source panel, if it's up..\n");
#endif // DEBUG_StatsPanel

    //First get the first item...
    QListViewItemIterator it( splv );
    QListViewItem *item = *it;

  // Now call the match routine, this should focus any source panels, if the 
  // focus source panel preference is set

  if( getPreferencesFocusSourcePanel() &&  
      item && matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) )) {

#ifdef DEBUG_StatsPanel
         printf("StatsPanel::listener, FocusObject, match\n");
#endif // DEBUG_StatsPanel

        return 1;
    } else {
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::listener, FocusObject, no match\n");
#endif // DEBUG_StatsPanel
        return 0;
    }

   // ---------------------------- 
   // ---------------------------- FOCUS-COMPARE-OBJECT
   // ----------------------------
  } else if(  msgObject->msgType  == "FocusCompareObject" && recycleFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel got a new FocusCompareObject expID was %d\n", expID);
#endif // DEBUG_StatsPanel

    FocusCompareObject *msg = (FocusCompareObject *)msgObject;

#ifdef DEBUG_StatsPanel
   msg->print();
#endif // DEBUG_StatsPanel

    if( !msg->compare_command.startsWith("cview -c") ) {
      expID = msg->expID;

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusCompareObject, C: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

    }

    if( !msg->compare_command.isEmpty()  ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener() FocusCompareObject, calling updateStatsPanelData, command=(%s)\n", msg->compare_command.ascii() );
#endif // DEBUG_StatsPanel

      updateStatsPanelData(DONT_FORCE_UPDATE, msg->compare_command);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, FocusCompareObject, StatsPanel::listener() called \n");
#endif // DEBUG_StatsPanel

    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener() FocusCompareObject, raise this panel? msg->raiseFLAG=%d\n",msg->raiseFLAG);
#endif // DEBUG_StatsPanel

    if( msg->raiseFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener() FocusCompareObject, RAISE THIS PANEL, raise this panel.., this=%d\n", this);
#endif // DEBUG_StatsPanel

      getPanelContainer()->raisePanel(this);

    }
   // ---------------------------- 
   // ---------------------------- UPDATE-EXPERIMENT-DATA-OBJECT
   // ----------------------------
  } else if(  msgObject->msgType  == "UpdateExperimentDataObject" ) {

    UpdateObject *msg = (UpdateObject *)msgObject;

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT msgType == UpdateExperimentDataObject\n");
   printf("StatsPanel::listener,  UPDATE-EXPERIMENT-DATA-OBJECTmsg->expID=%d\n, msg->expID");
#endif // DEBUG_StatsPanel

    if( msg->expID == -1 ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener,  UPDATE-EXPERIMENT-DATA-OBJECT We got the command=(%s)\n", msg->experiment_name.ascii() );
#endif // DEBUG_StatsPanel

      QString command = msg->experiment_name;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT We got the command=(%s), calling updateStatsPanelData\n", command.ascii() );
#endif // DEBUG_StatsPanel

      updateStatsPanelData(DONT_FORCE_UPDATE, command);

//Hack - NOTE: You may have to snag the expID out of the command.
#ifdef OLDWAY
expID = groupID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, D: expID = %d\n", expID);
#endif // DEBUG_StatsPanel
updateCollectorList();
#else // OLDWAY

int start_index = command.find("-x");
if( start_index != -1 ) {
  QString s = command.mid(start_index+3);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT Got a -x in the command s=(%s)\n", s.ascii() );
#endif // DEBUG_StatsPanel

  int end_index = s.find(" ");
  if( end_index == -1 )
  {
    end_index == 99999;
  }

  QString exp_x = s.mid(0, end_index);

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT exp_x=%s\n", exp_x.ascii() );
#endif // DEBUG_StatsPanel

  expID = exp_x.toInt();

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT E: SETTING expID = %d\n", expID);
#endif // DEBUG_StatsPanel

   updateCollectorList();

} else {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT no -x in the command\n");
//    expID = groupID;
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT G: SETTING expID = %d\n", expID);
#endif // DEBUG_StatsPanel

}
#endif // OLDWAY
      return(1);
    }
    

    expID = msg->expID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT H: SETTING expID = %d\n", expID);
#endif // DEBUG_StatsPanel

    // Begin determine if there's mpi stats
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() ) {

        Experiment *fw_experiment = eo->FW();
        CollectorGroup cgrp = fw_experiment->getCollectors();

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT It says you have %d collectors.\n", cgrp.size() );
#endif // DEBUG_StatsPanel

        if( cgrp.size() == 0 ) {
          fprintf(stderr, "There are no known collectors for this experiment.\n");
        }

        for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
        {
          Collector collector = *ci;
          Metadata cm = collector.getMetadata();
          QString name = QString(cm.getUniqueId().c_str());
        }
      }
    }
    catch(const std::exception& error)
    { 
      std::cerr << std::endl << "Error: "
                << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                "Unknown runtime error." : error.what()) << std::endl
                << std::endl;
      QApplication::restoreOverrideCursor( );
      return FALSE;
    }
// End determine if there's mpi stats

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT Call updateStatsPanelData() \n");
#endif // DEBUG_StatsPanel

    // Raise or Create a new manage processes panel for loading saved data files
#if 0
    manageProcessesSelected();
#endif

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT, msg->raiseFLAG=%d, calling updateStatsPanelData \n", msg->raiseFLAG );
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT msgType=(%s)\n", msgObject->msgType.ascii() );
#endif // DEBUG_StatsPanel


    // Find the demangled/mangled names using the "list -v mangled" command
    // Create a vector of std:string, std::string which represents the demangled, mangled pairs
    getDemangledMangledNames(expID);

    updateStatsPanelData(DONT_FORCE_UPDATE);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT msg->raiseFLAG=%d \n", msg->raiseFLAG );
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT (%s)\n", msgObject->msgType.ascii() );
#endif // DEBUG_StatsPanel

    if( msg->raiseFLAG ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT RAISING THIS PANEL this=%d \n", this );
#endif
      getPanelContainer()->raisePanel((Panel *)this);
    }
   // ---------------------------- 
   // ---------------------------- PREFERENCE-CHANGED-OBJECT
   // ----------------------------
  } else if( msgObject->msgType == "PreferencesChangedObject" ) {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getPreferenceShowToolbarCheckBox()=%d\n",
           getPreferenceShowToolbarCheckBox() );
#endif // DEBUG_StatsPanel

    if( getPreferenceShowToolbarCheckBox() == TRUE ) {
      toolBarFLAG = TRUE;
      fileTools->show();

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::listener, SHOW,fileTools=0x%lx)\n", fileTools);
#endif

    } else {

      fileTools->hide();
      toolBarFLAG = FALSE;

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::listener, (1309) HIDE,fileTools=0x%lx)\n", fileTools);
#endif

    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getPreferenceShowMetadataCheckBox()=%d\n",
           getPreferenceShowMetadataCheckBox() );
#endif // DEBUG_StatsPanel

    if( getPreferenceShowMetadataCheckBox() == TRUE ) {
      infoHeaderFLAG = TRUE;
    } else {
      infoHeaderFLAG = FALSE;
    }


#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getChartTypeComboBox()=%d\n",
           getChartTypeComboBox() );
#endif // DEBUG_StatsPanel

    if( !getChartTypeComboBox() ) {
      cf->setChartType((ChartType)0);
    } else {
      cf->setChartType((ChartType)getChartTypeComboBox());
    }


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener() PREFERENCE-CHANGED-OBJECT, about to call updateStatsPanelData\n"  );
#endif
    updateStatsPanelData(DO_FORCE_UPDATE);

   // ---------------------------- 
   // ---------------------------- SAVE-AS-OBJECT
   // ----------------------------
  } else if( msgObject->msgType == "SaveAsObject" ) {

    SaveAsObject *sao = (SaveAsObject *)msg;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, Save as\n");
#endif // DEBUG_StatsPanel

    if( !sao ) {
      return 0;  // 0 means, did not act on message.
    }

//    exportData(sao->f, sao->ts);
// Currently you're not passing the file descriptor down... you need to.sao->f, sao->ts);

    f = sao->f;
    exportData(EXPORT_TEXT);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener bottom exit\n");
#endif // DEBUG_StatsPanel

  return 0;  // 0 means, did not want this message and did not act on anything.
}

bool
StatsPanel::menu( QPopupMenu* contextMenu)
{

#ifdef DEBUG_StatsPanel_menu
 printf("StatsPanel::menu(),  focusedExpID=%d\n", focusedExpID );
 printf("StatsPanel::menu(),  compareExpIDs.size()=%d\n", compareExpIDs.size());
#endif

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::menu() entered.\n");
 printf("StatsPanel::menu(),  focusedExpID=%d\n", focusedExpID );
 printf("StatsPanel::menu(),  compareExpIDs.size()=%d\n", compareExpIDs.size());
#endif

  Panel::menu(contextMenu);

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::menu, B: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
 printf("StatsPanel::menu, B: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif


  popupMenu = contextMenu; // So we can look up the text easily later.

  QAction *qaction = NULL;

  qaction = new QAction( this,  "_aboutStatsPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Context..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( aboutSelected() ) );
  qaction->setStatusTip( tr("Shows information about what is currently being displayed in the StatsPanel.") );

  qaction = new QAction( this,  "_updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Update Panel..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Attempt to update this panel's display with fresh data.") );

  contextMenu->insertSeparator();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::menu, expID=(%d) focusedExpID=(%d)\n", expID, focusedExpID );
#endif

  if( focusedExpID > 0 ) {
    qaction = new QAction( this,  "_originalQuery");
    qaction->addTo( contextMenu );
    qaction->setText( QString("Original Query (%1) ...").arg(originalCommand) );
    connect( qaction, SIGNAL( activated() ), this, SLOT( originalQuery() ) );
    qaction->setStatusTip( tr("Update this panel with the data from the initial query.") );

    qaction = new QAction( this,  "_cviewQueryStatemnts");
    qaction->addTo( contextMenu );
    qaction->setText( QString("Query Statements (%1) ...").arg(originalCommand) );
    connect( qaction, SIGNAL( activated() ), this, SLOT( cviewQueryStatements() ) );
    qaction->setStatusTip( tr("Update this panel with the statements related to the initial query.") );
  }

  // Over all the collectors....
  // Round up the metrics ....
  // Create a menu of metrics....
  contextMenu->setCheckable(TRUE);
  int mid = -1;
  QString defaultStatsReportStr = QString::null;

  // The time segment doesn't apply to comparisons well currently
  if (compareExpIDs.size() < 1) {
    qaction = new QAction(this, "selectTimeSlice");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Select: Time Segment") );
    qaction->setToolTip(tr("Select a time segment to limiting future reports.") );
    connect( qaction, SIGNAL( activated() ), this, SLOT(timeSliceSelected()) );
  }

#ifdef DEBUG_StatsPanel_menu
  printf("Do you have a list of collectors?, list_of_collectors.size()=%d\n",
         list_of_collectors.size() );
#endif

// printf("Do you have a list of collectors?\n");
  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
  {
     std::string collector_name = (std::string)*it;
#ifdef DEBUG_StatsPanel
     printf("collector_name = (%s)\n", collector_name.c_str() );
#endif
    if( QString(collector_name.c_str()).startsWith("mpi") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an mpi* menu\n");
#endif
      generateMPIMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("iop") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an iop menu\n");
#endif
      generateIOPMenu();
    } else if( QString(collector_name.c_str()).startsWith("io") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an io* menu\n");
#endif
      generateIOMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("hwcsamp") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an hwcsamp menu\n");
#endif
      generateHWCSampMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("hwctime") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an hwctime menu\n");
#endif
      generateHWCTimeMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("hwc") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an hwc menu\n");
#endif
      generateHWCMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("usertime") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an usertime menu.\n");
#endif
      generateUserTimeMenu();
    } else if( QString(collector_name.c_str()).startsWith("omptp") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate an omptp menu.\n");
#endif
      generateOMPTPMenu();
    } else if( QString(collector_name.c_str()).startsWith("mem") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate a mem menu.\n");
#endif
      generateMEMMenu(QString(collector_name.c_str()));
    } else if( QString(collector_name.c_str()).startsWith("pcsamp") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate a pcsamp menu\n");
#endif
      generatePCSampMenu();
    } else if( QString(collector_name.c_str()).startsWith("fpe") ) {
#ifdef DEBUG_StatsPanel
      printf("Generate a fpe menu\n");
#endif
      generateFPEMenu();
    } else {
#ifdef DEBUG_StatsPanel
      printf("Generate an other (%s) menu\n", collector_name.c_str() );
#endif
      generateGenericMenu();
    }

  }

  if( threadMenu ) {
    delete threadMenu;
  }

  threadMenu = new QPopupMenu(this);

  int MAX_PROC_MENU_DISPLAY = 8;
  if( list_of_pids.size() > 1 && list_of_pids.size() <= MAX_PROC_MENU_DISPLAY )
  {
    contextMenu->insertItem(QString("Show Rank/Thread/Process"), threadMenu);
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int64_t pid = (int64_t)*it;
#ifdef DEBUG_StatsPanel_details
      printf("Inside threadMenu generation, pid=(%ld)\n", pid );
#endif
      QString pidStr = QString("%1").arg(pid);
      int mid = threadMenu->insertItem(pidStr);
      threadMenu->setCheckable(TRUE);

      if( currentThreadGroupStrList.count() == 0 ) {
        threadMenu->setItemChecked(mid, TRUE);
      }

      if( currentMenuThreadsStr.isEmpty() || currentMenuThreadsStr == pidStr ) {
        currentMenuThreadsStr = pidStr;
#ifdef DEBUG_StatsPanel
        printf("Inside threadMenu generation, setting currentMenuThreadsStr = %s\n", currentMenuThreadsStr.ascii() );
#endif
      }

      for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); 
                                          it != currentThreadGroupStrList.end(); ++it)
      {
        QString ts = (QString)*it;
        if( ts == pidStr ) {
          threadMenu->setItemChecked(mid, TRUE);
#ifdef DEBUG_StatsPanel
          printf("Inside threadMenu generation, setting mid=(%d) item checked\n", mid );
#endif
        }
      }
    }
    connect(threadMenu, SIGNAL( activated(int) ),
        this, SLOT(threadSelected(int)) );
    // By default select them all...
    if( currentThreadGroupStrList.count() == 0 ) {

      for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
           it != list_of_pids.end(); it++ )
      {
        int64_t pid = (int64_t)*it;
        currentThreadGroupStrList.push_back(QString("%1").arg(pid));
      }
    }

  }

#ifdef DEBUG_StatsPanel
  printf("After threadMenu generation, currentThreadGroupStrList.count()=(%d)\n", currentThreadGroupStrList.count() );
#endif

  contextMenu->insertSeparator();


#ifndef COLUMNS_MENU
  int id = 0;
  QPopupMenu *columnsMenu = new QPopupMenu(this);
  columnsMenu->setCaption("Manage Columns Menu Options");
  contextMenu->insertItem("&Manage Columns Menu Options", columnsMenu, CTRL+Key_C);

  for( ColumnList::Iterator pit = columnHeaderList.begin();
           pit != columnHeaderList.end();
           ++pit )
  { 
    QString s = (QString)*pit;
    columnsMenu->insertItem(s, this, SLOT(doOption(int)), CTRL+Key_1, id, -1);
    if( splv->columnWidth(id) ) {
      columnsMenu->setItemChecked(id, TRUE);
    } else {
      columnsMenu->setItemChecked(id, FALSE);
    }
    id++;
  }
#endif // COLUMNS_MENU

  qaction = new QAction( this,  "exportDataCSVAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Report Data (csv)..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( exportCSVData() ) );
  qaction->setStatusTip( tr("Save the report's data as a comma separated list to a csv file.") );

  qaction = new QAction( this,  "exportDataTextAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Report Data (text)..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( exportTextData() ) );
  qaction->setStatusTip( tr("Save the report's data as a space separated list to an ascii text file.") );

#ifdef DEBUG_StatsPanel_menu
  printf("menu, splv->selectedItem()=%d\n", splv->selectedItem());
#endif
  if( splv->selectedItem() ) {
 

#ifdef DEBUG_StatsPanel
     printf("menu, in splv->selectedItem(), compareExpIDs.size()=(%d)\n", compareExpIDs.size() );
#endif

     if (compareExpIDs.size() > 1) {

#ifdef DEBUG_StatsPanel
         printf("StatsPanel::menu, CHECK if compareExpIDs>2, currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif
        // There is no source location to go to for these collectors 
        if (compareExpIDs.size() >= 2 && !((currentCollectorStr == "mpi" || 
                                            currentCollectorStr == "mpit" || 
                                            currentCollectorStr == "io" || 
                                            currentCollectorStr == "mem" || 
                                            currentCollectorStr == "pthreads" || 
                                            currentCollectorStr == "iot" ) )) {
#ifdef DEBUG_StatsPanel
          printf("menu, size >= 2,  in splv->selectedItem(), compareExpIDs[0]=(%d)\n", compareExpIDs[0] );
          printf("menu, size >= 2,  in splv->selectedItem(), compareExpIDs[1]=(%d)\n", compareExpIDs[1] );
#endif
          // If we have more than one experiment... figure out focus. 
          qaction = new QAction( this,  "gotoSource-compare1");
          qaction->addTo( contextMenu );
          qaction->setText( QString("Go to source location (for Exp %1) ...").arg(compareExpIDs[0]) );
          connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSourceCompare1Selected() ) );
          qaction->setStatusTip( tr("Position at source location of this item.") );

          qaction = new QAction( this,  "gotoSource-compare2");
          qaction->addTo( contextMenu );
          qaction->setText( QString("Go to source location (for Exp %1) ...").arg(compareExpIDs[1]) );
          connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSourceCompare2Selected() ) );
          qaction->setStatusTip( tr("Position at source location of this item.") );
        }

        if (compareExpIDs.size() >= 3) {
#ifdef DEBUG_StatsPanel
          printf("menu, size >= 3, in splv->selectedItem(), compareExpIDs[2]=(%d)\n", compareExpIDs[2] );
#endif
          qaction = new QAction( this,  "gotoSource-compare3");
          qaction->addTo( contextMenu );
          qaction->setText( QString("Go to source location (for Exp %1) ...").arg(compareExpIDs[2]) );
          connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSourceCompare3Selected() ) );
          qaction->setStatusTip( tr("Position at source location of this item.") );
        } 

        if (compareExpIDs.size() >= 4) {
#ifdef DEBUG_StatsPanel
          printf("menu, size >= 4, in splv->selectedItem(), compareExpIDs[3]=(%d)\n", compareExpIDs[3] );
#endif
          qaction = new QAction( this,  "gotoSource-compare4");
          qaction->addTo( contextMenu );
          qaction->setText( QString("Go to source location (for Exp %1) ...").arg(compareExpIDs[3]) );
          connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSourceCompare4Selected() ) );
          qaction->setStatusTip( tr("Position at source location of this item.") );
        } 

        if (compareExpIDs.size() >= 5) {
#ifdef DEBUG_StatsPanel
          printf("menu, size >= 5, in splv->selectedItem(), compareExpIDs[4]=(%d)\n", compareExpIDs[4] );
#endif
          qaction = new QAction( this,  "gotoSource-compare5");
          qaction->addTo( contextMenu );
          qaction->setText( QString("Go to source location (for Exp %1) ...").arg(compareExpIDs[4]) );
          connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSourceCompare5Selected() ) );
          qaction->setStatusTip( tr("Position at source location of this item.") );
        }

    } else {

      qaction = new QAction( this,  "gotoSource");
      qaction->addTo( contextMenu );
      if( focusedExpID != -1 ) {
        qaction->setText( QString("Go to source location (for Exp %1) ...").arg(focusedExpID) );
      } else {
        qaction->setText( "Go to source location..." );
      }
      connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSource() ) );
      qaction->setStatusTip( tr("Position at source location of this item.") );
   }

  }

#ifdef DEBUG_StatsPanel_menu
  printf("menu, experimentGroupList.count()=%d\n", experimentGroupList.count());
#endif

  if( experimentGroupList.count() > 1 )
  {
    int id = 0;
    experimentsMenu = new QPopupMenu(this);
    columnsMenu->setCaption("Select Focused Experiment:");
    contextMenu->insertItem("&Select Focused Experiment:", experimentsMenu, CTRL+Key_M);
  
    for(ExperimentGroupList::iterator egi = experimentGroupList.begin();egi != experimentGroupList.end();egi++)
    {
      QString s = (QString)*egi;
      int index = s.find(":");
      if( index != -1 )
      {
        index++;
        int exp_id = s.mid(index,9999).stripWhiteSpace().toInt();
        s = QString("Experiment: %1").arg(exp_id);
        id = experimentsMenu->insertItem(s);
        if( exp_id == focusedExpID ) {
          experimentsMenu->setItemChecked(id, TRUE);
        }
      }
    }

    connect(experimentsMenu, SIGNAL( activated(int) ), this, SLOT(focusOnExp(int)) );
  }


  contextMenu->insertSeparator();

#ifdef DEBUG_StatsPanel_chart
  printf("menu re-orientate, chartFLAG=%d\n", chartFLAG);
  printf("menu re-orientate, statsFLAG=%d\n", statsFLAG);
#endif
 
  if( chartFLAG == TRUE && statsFLAG == TRUE )
  {
    qaction = new QAction( this,  "re-orientate");
    qaction->addTo( contextMenu );
    qaction->setText( "Re-orientate" );
    connect( qaction, SIGNAL( activated() ), this, SLOT( setOrientation() ) );
    qaction->setStatusTip( tr("Display chart/statistics horizontal/vertical.") );
  }

#ifdef DEBUG_StatsPanel_chart
  printf("menu hideChart, chartFLAG=%d\n", chartFLAG);
  printf("menu hideChart, statsFLAG=%d\n", statsFLAG);
#endif

  if( chartFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are shown, hide the graphic chart.") );
  } else {
    qaction = new QAction( this,  "showChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are available, show the graphic chart.") );
  }

  if( statsFLAG == TRUE ) {
    qaction = new QAction( this,  "hideStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Hide the statistics display.") );
  } else {
    qaction = new QAction( this,  "showStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Show the statistics display.") );
  }

  if( infoHeaderFLAG == TRUE ) {
    qaction = new QAction( this,  "hideInfoHeader");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Experiment Metadata..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showInfoHeader() ) );
    qaction->setStatusTip( tr("Hide the experiment header information display.") );
  } else {
    qaction = new QAction( this,  "showInfoHeader");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Experiment Metadata Info..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showInfoHeader() ) );
    qaction->setStatusTip( tr("Show the experiment metadata display.") );
  }

  if( toolBarFLAG == TRUE ) {
    qaction = new QAction( this,  "hideToolBar");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Experiment Display Option ToolBar..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showToolBar() ) );
    qaction->setStatusTip( tr("Hide the experiment display option toolbar.") );
  } else {
    qaction = new QAction( this,  "showToolBar");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Experiment Display Option ToolBar..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showToolBar() ) );
    qaction->setStatusTip( tr("Show the experiment display option toolbar.") );
  }

// printf("menu: canWeDiff()?\n");

  if( canWeDiff() ) {

// printf("menu: canWeDiff() says we can!\n");

    if( insertDiffColumnFLAG == TRUE ) {
      qaction = new QAction( this,  "hideDifference");
      qaction->addTo( contextMenu );
      qaction->setText( "Hide Difference..." );
      connect( qaction, SIGNAL( activated() ), this, SLOT( showDiff() ) );
      qaction->setStatusTip( tr("Hide the difference column.") );
    } else {
      qaction = new QAction( this,  "showDifference");
      qaction->addTo( contextMenu );
      qaction->setText( "Show Difference..." );
      connect( qaction, SIGNAL( activated() ), this, SLOT( showDiff() ) );
      qaction->setStatusTip( tr("Show the difference column.") );
    }
  }

#ifdef CLUSTERANALYSIS
// if( focusedExpID == -1 && currentCollectorStr == "usertime" )
     if( focusedExpID == -1 && compareExpIDs.size() < 1) {
       contextMenu->insertSeparator();
       qaction = new QAction( this,  "clusterAnalysisSelected");
       qaction->addTo( contextMenu );
       qaction->setText( "Compare and Analyze using cluster analysis" );
       connect( qaction, SIGNAL( activated() ), this, SLOT( clusterAnalysisSelected() ) );
       qaction->setStatusTip( tr("Perform analysis on the processes, threads, or ranks of this experiment to group similar processes, threads, or ranks.") );
     }

#endif// CLUSTERANALYSIS


     contextMenu->insertSeparator();

     qaction = new QAction( this,  "customizeExperimentsSelected");
     qaction->addTo( contextMenu );
     qaction->setText( "Custom Comparison ..." );
     connect( qaction, SIGNAL( activated() ), this, SLOT( customizeExperimentsSelected() ) );
     qaction->setStatusTip( tr("Customize column data in the StatsPanel.") );

//#ifdef MIN_MAX_ENABLED
     if (compareExpIDs.size() < 1) {
       contextMenu->insertSeparator();

       qaction = new QAction( this,  "minMaxAverageSelected");
       qaction->addTo( contextMenu );
       qaction->setText( "Show Load Balance Overview..." );
       connect( qaction, SIGNAL( activated() ), this, SLOT( minMaxAverageSelected() ) );
       qaction->setStatusTip( tr("Generate a min, max, average report in the StatsPanel.") );
     }
//#endif

  return( TRUE );
}

QString
StatsPanel::getMostImportantClusterMetric(QString collector_name)
{
  QString metric = QString::null;

#if 0
  if( collector_name == "pcsamp" ) {
    metric = "-m pcsamp::exclusive_time";
  } else if( collector_name == "usertime" ) {
    metric = "-m usertime::exclusive_time";
  } else if( collector_name == "omptp" ) {
    metric = "-m omptp::exclusive_time";
  } else if( collector_name == "hwc" ) {
    metric = "-m hwc::ThreadAverage";
  } else if( collector_name == "hwctime" ) {
    metric = "-m hwc::ThreadAverage";
  } else if( collector_name == "mpi" ) {
    metric = "-m mpi::ThreadAverage";
  } else if( collector_name == "mpit" ) {
    metric = "-m mpit::ThreadAverage";
  } else if( collector_name == "io" ) {
    metric = "-m io::ThreadAverage";
  } else if( collector_name == "iot" ) {
    metric = "-m io::ThreadAverage";
  }
#endif

  return(metric);

}

void
StatsPanel::clusterAnalysisSelected()
{

  QString command = QString::null;
  QString cview_cluster_command = QString::null;
  QString mim = getMostImportantClusterMetric(currentCollectorStr);
  QString displayType;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() ENTERED, currentDisplayUsingType=%d\n", currentDisplayUsingType );
  printf("StatsPanel::clusterAnalysisSelected() ENTERED, currentCollectorStr=%s\n", currentCollectorStr.ascii() );
  printf("StatsPanel::clusterAnalysisSelected() ENTERED, currentThreadsStr=%s\n", currentThreadsStr.ascii() );
#endif

  displayType = "functions";
  if (currentDisplayUsingType == displayUsingStatementType) {
      displayType = "statements";
  } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
      displayType = "linkedobjects";
#if defined(HAVE_DYNINST)
  } else if (currentDisplayUsingType == displayUsingLoopType) {
      displayType = "loops";
#endif
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() most important cluster metric: mim=%s\n", mim.ascii() );
#endif

  if( focusedExpID == -1 ) {
    cview_cluster_command = QString("cviewCluster -x %1 %2 %3 -v %4 %5").arg(expID).arg(timeIntervalString).arg(mim).arg(displayType).arg(currentThreadsStr);
  } else {
    cview_cluster_command = QString("cviewCluster -x %1 %2 %3 -v %4 %5").arg(focusedExpID).arg(timeIntervalString).arg(mim).arg(displayType).arg(currentThreadsStr);
  }

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  std::list<int64_t> list_of_cids;
  list_of_cids.clear();
  InputLineObject *clip = NULL;

  bool command_seen_before = isCommandAssociatedWith(cview_cluster_command.ascii());

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::clusterAnalysisSelected(), before check_for_existing_clip, command_seen_before=" 
            << command_seen_before << std::endl;
#endif
  

 if (command_seen_before) {

   // cview cluster command is associated with an existing cview command
   // we want to use that command instead of creating a new one (code in the main if block above)
   std::string returned_string_command = getAssociatedCommand(cview_cluster_command.ascii());
   if (returned_string_command != NULL) {
      command = QString::fromUtf8(returned_string_command.c_str());
      //command = QString::fromStdString(returned_string_command.c_str());
   } else {
      //FIX ME ERROR
      command = QString::fromUtf8("");
      //command = QString::fromStdString("");
   }

 } else {

  // Call to the framework for the cviewCluster information
  // Then save the clip for reuse

  if( !cli->getIntListValueFromCLI( (char *)cview_cluster_command.ascii(), &list_of_cids, clip, TRUE ) ) {
     printf("Unable to run %s cview_cluster_command.\n", cview_cluster_command.ascii() );
  } else {
#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel::clusterAnalysisSelected(), ran this cview_cluster_command=" << cview_cluster_command.ascii() << std::endl;
#endif
  }

#ifdef DEBUG_StatsPanel_cache
    std::cerr << "StatsPanel::clusterAnalysisSelected(), ran this cview_cluster_command=" << cview_cluster_command.ascii() << std::endl;
#endif

 if( clip ) {
   clip->Set_Results_Used();
 }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() list_of_cids.size()=%d\n", list_of_cids.size() );
#endif

  QString cidlist = QString::null;
  if( list_of_cids.size() >= 1 ) {
    for( std::list<int64_t>::const_iterator it = list_of_cids.begin();
         it != list_of_cids.end(); it++ ) {
      int64_t pid = (int64_t)*it;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::clusterAnalysisSelected() in loop, pid=%ld\n", pid);
#endif

      if( cidlist.isEmpty() ) { 
        cidlist = QString("%1").arg(pid);
      } else {
        cidlist += QString(", %1").arg(pid);
      }
    }
  } else {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::clusterAnalysisSelected(), No outliers...\n");
#endif

    return;
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() after loop, cidlist=%s\n", cidlist.ascii() );
  printf("StatsPanel::clusterAnalysisSelected() after loop, timeIntervalString=%s\n", timeIntervalString.ascii() );
#endif

  if (currentDisplayUsingType == displayUsingFunctionType) {
     if (timeIntervalString != NULL) {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v functions").arg(timeIntervalString);
     } else {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v functions");
     }
  } else if (currentDisplayUsingType == displayUsingStatementType) {
     if (timeIntervalString != NULL) {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v statements").arg(timeIntervalString);
     } else {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v statements");
     }
  } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
     if (timeIntervalString != NULL) {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v linkedobjects").arg(timeIntervalString);
     } else {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v linkedobjects");
     }
#if defined(HAVE_DYNINST)
  } else if (currentDisplayUsingType == displayUsingLoopType) {
     if (timeIntervalString != NULL) {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v loops").arg(timeIntervalString);
     } else {
       command = QString("cview -c %1 %2 %3").arg(cidlist).arg("-m ThreadAverage").arg("-v loops");
     }
#endif
  }

  associateTheseCommands( command.ascii(), cview_cluster_command.ascii() );

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() about to call updateStatsPanelData, command=%s cview_cluster_command=%s\n", 
         command.ascii(), cview_cluster_command.ascii() );
#endif

   }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() about to call updateStatsPanelData with ASSOCIATED COMMAND, command=%s cview_cluster_command=%s\n", 
         command.ascii(), cview_cluster_command.ascii() );
#endif
 //}

  currentUserSelectedReportStr = "clusterAnalysis";
  originatingUserSelectedReportStr = "clusterAnalysis";
  toolbar_status_label->setText("Generating Comparative Analysis Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Showing Comparative Analysis Report:");
}


//#ifdef MIN_MAX_ENABLED
void
StatsPanel::minMaxAverageSelected()
{
  QString displayType;
  QString command = QString::null;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::minMaxAverageSelected() ENTERED, currentDisplayUsingType=%d\n", currentDisplayUsingType );
#endif

  displayType = "functions";
  if (currentDisplayUsingType == displayUsingStatementType) {
      displayType = "statements";
  } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
      displayType = "linkedobjects";
#if defined(HAVE_DYNINST)
  } else if (currentDisplayUsingType == displayUsingLoopType) {
      displayType = "loops";
#endif
  }

  if( focusedExpID == -1 ) {

#if 0
    command = QString("expview -x %1 %2 -m %3::ThreadMin, %4::ThreadMax, %5::ThreadAverage -v %6").arg(expID).arg(timeIntervalString).arg(currentCollectorStr).arg(currentCollectorStr).arg(currentCollectorStr).arg(displayType);
#else
    command = QString("expview -x %1 %2 -m loadbalance -v %3 %4").arg(expID).arg(timeIntervalString).arg(displayType).arg(currentThreadsStr);
#endif

  } else {

#if 0
    command = QString("expview -x %1 %2 -m %3::ThreadMin, %4::ThreadMax, %5::ThreadAverage -v %6").arg(focusedExpID).arg(timeIntervalString).arg(currentCollectorStr).arg(currentCollectorStr).arg(currentCollectorStr).arg(displayType);
#else
    command = QString("expview -x %1 %2 -m loadbalance -v %3 %4").arg(focusedExpID).arg(timeIntervalString).arg(displayType).arg(currentThreadsStr);
#endif
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::minMaxAverageSelected() about to call updateStatsPanelData, command=%s\n", command.ascii() );
#endif

  // jeg added 4/6/2010
  currentUserSelectedReportStr = "minMaxAverage";

  originatingUserSelectedReportStr = "minMaxAverage";
  toolbar_status_label->setText("Generating Load Balance (min,max,ave) Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Showing Load Balance (min,max,ave) Report:");
#ifdef DEBUG_StatsPanel
  printf("Exit StatsPanel::minMaxAverageSelected()\n" );
#endif

}
//#endif


void
StatsPanel::clearAuxiliarySelected()
{


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clearAuxiliarySelected() entered\n" );
#endif
  currentUserSelectedReportStr = QString::null;

  toolbar_status_label->setText("Clearing Auxiliary Setttings:");

  selectedFunctionStr = QString::null;

  timeIntervalString = QString::null;

  traceAddition = QString::null;

  currentUserSelectedReportStr = QString::null;

  currentThreadsStr = QString::null;

// This was changed on 10/13/2011 in order to make the GUI more flexible.
// The Default, Cluster Analysis and Load Balance views will now honor previous metric selections, 
// time segments, specific thread or rank selections
// Users must now use the CL (clear icon) to clear these selections away.
// This call clears the modifiers
  clearModifiers();

//  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Cleared Auxiliary Setttings, future reports are aggregated over all processes,threads, or ranks:");

}

void
StatsPanel::showEventListSelected()
{
  // Clear all residual view selections
  clearAuxiliarySelected();

  QString command = QString::null;
  traceAddition = " -v trace";

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showEventListSelected() about to call updateStatsPanelData, command=%s\n", 
         command.ascii() );
#endif

  toolbar_status_label->setText("Generating Per Event Report:");
  updateStatsPanelData(DO_FORCE_UPDATE, NULL);
  toolbar_status_label->setText("Showing Per Event Report:");

}

void
StatsPanel::customizeExperimentsSelected()
{
  nprintf( DEBUG_PANELS ) ("StatsPanel::customizeExperimentsSelected()\n");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::customizeExperimentsSelected()\n");
#endif

  QString name = QString("CustomizeStatsPanel [%1]").arg(expID);

  Panel *customizePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( customizePanel ) { 

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::customizePanel() found customizePanel found.. RAISE customizePanel.\n");
#endif
    nprintf( DEBUG_PANELS ) ("customizePanel() found customizePanel found.. raise it.\n");
    getPanelContainer()->raisePanel(customizePanel);

  } else {

//    nprintf( DEBUG_PANELS ) ("customizePanel() no customizePanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

//    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);

   ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);

   if( focusedExpID != -1 ) {

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::assigning qstring_data\n");
#endif

     ao->qstring_data = QString("%1").arg(focusedExpID);
   } else {
     ao->qstring_data = QString("%1").arg(groupID);
   }
    customizePanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("CustomizeStatsPanel", bestFitPC, ao, (const char *)NULL);
    delete ao;
  }

}   

void
StatsPanel::generateModifierMenu(QPopupMenu *menu, 
                                 std::list<std::string> modifier_list, 
                                 std::list<std::string> current_list)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::generateModifierMenu() entered\n");
#endif
  menu->setCheckable(TRUE);
  for( std::list<std::string>::const_iterator it = modifier_list.begin();
          it != modifier_list.end(); it++ )
  {
    std::string modifier = (std::string)*it;

#ifdef DEBUG_StatsPanel
    printf("modifier = (%s)\n", modifier.c_str() );
#endif

    QString s = QString(modifier.c_str() );
     int mid = menu->insertItem(s);
    for( std::list<std::string>::const_iterator it = current_list.begin();
         it != current_list.end(); it++ )
    {
      std::string current_modifier = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("building menu : current_list here's one (%s)\n", current_modifier.c_str() );
#endif
      if( modifier == current_modifier )
      {
#ifdef DEBUG_StatsPanel
        printf("WE have a match to check\n");
#endif
        menu->setItemChecked(mid, TRUE);
      }
    }
  }
}

/*! Create the context senstive menu for the report. */
bool
StatsPanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
// printf("StatsPanel::createPopupMenu(contextMenu=0x%x) entered\n", contextMenu);
  menu(contextMenu);
  return( TRUE );
}



void
StatsPanel::showChart()
{

#ifdef DEBUG_StatsPanel_chart
  printf("ENTER StatsPanel::showChart() entered, CHART, statsFLAG=%d\n", statsFLAG);
  printf("ENTER StatsPanel::showChart() entered, CHART, chartFLAG=%d\n", chartFLAG);
#endif

  if( chartFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::showChart() entered, CHART, setting to FALSE, chartFLAG=%d\n", chartFLAG);
#endif
    chartFLAG = FALSE;
    cf->hide();

  } else {

#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::showChart() entered, CHART, setting to TRUE, chartFLAG=%d\n", chartFLAG);
#endif
    chartFLAG = TRUE;

#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::showChart(), CHART, lastCommand=(%s)\n", lastCommand.ascii() );
#endif

    cf->show();

    if( !lastCommand.startsWith("cview") ) {

#ifdef DEBUG_StatsPanel_chart
      printf("StatsPanel::showChart(), CHART, CLEARING lastCommand=(%s)\n", lastCommand.ascii() );
#endif

      lastCurrentThreadsStr = QString::null; 
      lastCommand = QString::null;  // This will force a redraw of the data.
      // I'm not sure why, but the text won't draw unless the 
      // piechart is visible.
#ifdef DEBUG_StatsPanel_chart
     printf("StatsPanel::showChart(), CHART, calling updatePanel, lastCommand=(%s)\n", lastCommand.ascii() );
#endif
      // jeg commented out 4/7/2010 ( the text seems to draw well without this update)
      // updatePanel();

    } else {

#ifdef DEBUG_StatsPanel_chart
     printf("StatsPanel::showChart(), CHART, else, calling updatePanel, lastCommand=(%s)\n", lastCommand.ascii() );
#endif
      cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);
      updatePanel();

    }

    cf->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( chartFLAG == FALSE && statsFLAG == FALSE ) {
    statsFLAG = TRUE;
    splv->show();
  }

#ifdef DEBUG_StatsPanel_chart
  printf("EXIT StatsPanel::showChart() EXITING, statsFLAG=%d\n", statsFLAG);
  printf("EXIT StatsPanel::showChart() EXITING, chartFLAG=%d\n", chartFLAG);
#endif

}


void
StatsPanel::showStats()
{
#ifdef DEBUG_StatsPanel_chart
  printf("StatsPanel::showStats() entered, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showStats() entered, chartFLAG=%d\n", chartFLAG);
#endif
// printf("StatsPanel::showStats() entered\n");
  if( statsFLAG == TRUE )
  {
    statsFLAG = FALSE;
    splv->hide();
  } else
  {
    statsFLAG = TRUE;
    splv->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( statsFLAG == FALSE && chartFLAG == FALSE )
  {
    chartFLAG = TRUE;
    cf->show();
  }
#ifdef DEBUG_StatsPanel_chart
  printf("StatsPanel::showStats() exitted, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showStats() exitted, chartFLAG=%d\n", chartFLAG);
#endif
}


void
StatsPanel::showToolBar()
{
#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::showToolBar() entered, toolBarFLAG=%d, fileTools=0x%lx\n", toolBarFLAG, fileTools);
#endif

  if( toolBarFLAG == TRUE ) {
    toolBarFLAG = FALSE;
    fileTools->hide();

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::showToolBar, (2108) HIDE,fileTools=0x%lx)\n", fileTools);
#endif

  } else {

    toolBarFLAG = TRUE;
    fileTools->show();

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::showToolBar, SHOW,fileTools=0x%lx)\n", fileTools);
#endif

  }
}

void
StatsPanel::showInfoHeader()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showInfoHeader() entered, infoHeaderFLAG=%d\n", infoHeaderFLAG);
#endif
  if( infoHeaderFLAG == TRUE )
  {
    infoHeaderFLAG = FALSE;
    metadataAllSpaceFrame->hide();
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::showInfoHeader, HIDE metadataAllSpaceFrame, infoHeaderFLAG=%d)\n", infoHeaderFLAG);
#endif
  } else
  {
    infoHeaderFLAG = TRUE;
    metadataAllSpaceFrame->show();
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::showInfoHeader, SHOW metadataAllSpaceFrame, infoHeaderFLAG=%d)\n", infoHeaderFLAG);
#endif
  }
}


void
StatsPanel::showDiff()
{
// printf("StatsPanel::showDiff() entered\n");
  if( insertDiffColumnFLAG == TRUE )
  {
// printf("Remove the diff column.\n");
    removeDiffColumn(0); // zero is always the "|Difference|" column.
    // Force a resort in this case...
    splv->setSorting ( 0, FALSE );
    splv->setShowSortIndicator ( TRUE );
    splv->sort();
    insertDiffColumnFLAG = FALSE;
  } else
  {
// printf("insert the diff column.\n");
    int insertColumn = 0;
    insertDiffColumn(insertColumn);
    insertDiffColumnFLAG = TRUE;

    // Force a resort in this case...
    splv->setSorting ( insertColumn, FALSE );
    splv->setShowSortIndicator ( TRUE );
    splv->sort();
  }
}


/*! Reset the orientation of the graph/text relationship with setOrientation */
void
StatsPanel::setOrientation()
{
// printf("StatsPanel::setOrientation() entered\n");
  Orientation o = splitterB->orientation();
  if( o == QSplitter::Vertical )
  {
    splitterB->setOrientation(QSplitter::Horizontal);
  } else
  {
    splitterB->setOrientation(QSplitter::Vertical);
  }
}


/*! Go to source menu item was selected. */
void
StatsPanel::details()
{
#ifdef DEBUG_StatsPanel
  printf("details() menu selected.\n");
#endif
}
void
StatsPanel::exportCSVData()
{
      exportData(EXPORT_CSV);
}
 
void
StatsPanel::exportTextData()
{
      exportData(EXPORT_TEXT);
}

void
StatsPanel::exportData(EXPORT_TYPE_ENUM exportTypeParam)
{
// printf("exportData() menu selected.\n");
  Orientation o = splitterB->orientation();
  QListViewItemIterator it( splv );
  int cols =  splv->columns();
  int i=0;
  QString fileName;
  if (exportTypeParam == EXPORT_TEXT ) {
    fileName = "StatsPanel.txt";
  } else {
    fileName = "StatsPanel.csv";
  }
  QString dirName = QString::null;

  if( f == NULL)
  {
    QFileDialog *fd = new QFileDialog(this, "save_StatsPanelData:", TRUE );
    fd->setCaption( QFileDialog::tr("Save StatsPanel data:") );
    fd->setMode( QFileDialog::AnyFile );
    fd->setSelection(fileName);
    QString types( 
                      "Data files (*.dat);;"
                      "CSV files (*.csv);;"
                      "Text files (*.txt);;"
                      "Any File (*.*);;"
                      );
    fd->setFilters( types );
    // Pick the initial default types to put out.
    QString mask;
    if (exportTypeParam == EXPORT_TEXT ) {
      mask = QString("*.txt");
    } else {
      mask = QString("*.csv");
    }
    fd->setSelectedFilter( mask );
    fd->setDir(dirName);
  
    if( fd->exec() == QDialog::Accepted ) {
      fileName = fd->selectedFile();
    }
    
    if( !fileName.isEmpty() ) {
        f = new QFile(fileName);
        f->open(IO_WriteOnly );
    }
  }

  bool datFLAG = FALSE;
  if( fileName.endsWith(".dat") ) {
    datFLAG = TRUE;
  }

  if( f != NULL ) {
    // Write out the header info
    QString line = QString("  ");
    for(i=0;i<cols;i++)
    {
      for(i=0;i<cols;i++)
      {
        if( datFLAG == TRUE )
        {
          if( i < cols-1 ) {
            line += QString(splv->columnText(i))+"; ";
#if 0
            printf("header, i < cols-1, line=%s\n", line.ascii());
            printf("header, i < cols-1, QString(splv->columnText(i)).ascii()=%s\n", QString(splv->columnText(i)).ascii());
#endif
          } else {
            line += QString(splv->columnText(i));
            if (exportTypeParam == EXPORT_TEXT ) {
               line += " ";
            } else {
               line += ",";
            }
#if 0
            printf("header, else of i < cols-1, line=%s\n", line.ascii());
            printf("header, else of i < cols-1, QString(splv->columnText(i)).ascii()=%s\n", QString(splv->columnText(i)).ascii());
#endif
          }
        } else {
          QString my_column_text = QString(splv->columnText(i));

          if (exportTypeParam == EXPORT_CSV ) {
            if(!QString(splv->columnText(i)).isEmpty() ) {
#if 0
              printf("TOP of SEARCH,else of dataFLAG, QString(splv->columnText(i)).ascii()=%s\n", QString(splv->columnText(i)).ascii());
#endif
              int comma_search_start_index = 0;
              int comma_dx = -1;
#if 0
              printf("else of dataFLAG, comma_dx=%d, comma_search_start_index=%d\n", comma_dx, comma_search_start_index);
#endif
              while (comma_search_start_index != -1) {
                comma_dx = QString(splv->columnText(i)).find(",", comma_search_start_index);
#if 0
                printf("else of dataFLAG, comma_dx=%d\n", comma_dx);
#endif
                
                if (comma_dx != -1) {
                  my_column_text = QString(splv->columnText(i)).replace( comma_dx, 1, ":");
#if 0
                  printf("REPLACED COMMA, else of dataFLAG, my_column_columnText.ascii()=%s\n", my_column_text.ascii());
#endif
                  // End the search, should only be 1 comma
                  comma_search_start_index = -1;
                  break;
                }
                comma_search_start_index = comma_dx;
              }
             }
            line += my_column_text+",";
          } else {
            line += my_column_text+" ";
          }

#if 0
          line += QString(splv->columnText(i));
          if (exportTypeParam == EXPORT_TEXT ) {
             line += " ";
          } else {
             line += ",";
          }
#endif
#if 0
          printf("header, else of dataFLAG, line=%s\n", line.ascii());
          printf("header, else of dataFLAG, QString(splv->columnText(i)).ascii()=%s\n", QString(splv->columnText(i)).ascii());
#endif
        }
      }
      line += QString("\n");
    }
    f->writeBlock( line, qstrlen(line) );

   // Write out the body info
    while( it.current() )
    {
      QListViewItem *item = *it;
      line = QString("  ");
      for(i=0;i<cols;i++)
      {
        if( datFLAG == TRUE )
        {
          if( i < cols-1 ) {
            line += QString(item->text(i))+"; ";
#if 0
            printf("i < cols-1, line=%s\n", line.ascii());
            printf("i < cols-1, QString(item->text(i)).ascii()=%s\n", QString(item->text(i)).ascii());
#endif

          } else {
            line += QString(item->text(i));
            if (exportTypeParam == EXPORT_TEXT ) {
               line += " ";
            } else {
               line += ",";
            }
#if 0
            printf("else of i < cols-1, line=%s\n", line.ascii());
            printf("else of i < cols-1, QString(item->text(i)).ascii()=%s\n", QString(item->text(i)).ascii());
#endif
          }
        } else {
          QString my_column_text = QString(item->text(i));

          if (exportTypeParam == EXPORT_CSV ) {
            if(!QString(item->text(i)).isEmpty() ) {
#if 0
              printf("TOP of SEARCH,else of dataFLAG, QString(item->text(i)).ascii()=%s\n", QString(item->text(i)).ascii());
#endif
              int comma_search_start_index = 0;
              int comma_dx = -1;
#if 0
              printf("else of dataFLAG, comma_dx=%d, comma_search_start_index=%d\n", comma_dx, comma_search_start_index);
#endif
              while (comma_search_start_index != -1) {
                comma_dx = QString(item->text(i)).find(",", comma_search_start_index);
#if 0
                printf("else of dataFLAG, comma_dx=%d\n", comma_dx);
#endif
                
                if (comma_dx != -1) {
                  my_column_text = QString(item->text(i)).replace( comma_dx, 1, ":");
#if 0
                  printf("REPLACED COMMA, else of dataFLAG, my_column_text.ascii()=%s\n", my_column_text.ascii());
#endif
                  // End the search, should only be 1 comma
                  comma_search_start_index = -1;
                  break;
                }
                comma_search_start_index = comma_dx;
              }
             }
            line += my_column_text+",";
          } else {
            line += my_column_text+" ";
          }
#if 0
//          line += QString(item->text(i))+",";
          printf("else of dataFLAG, line=%s\n", line.ascii());
          printf("else of dataFLAG, QString(item->text(i)).ascii()=%s\n", QString(item->text(i)).ascii());
          printf("END of SEARCH,else of dataFLAG, QString(item->text(i)).ascii()=%s\n", QString(item->text(i)).ascii());
          printf("END of SEARCH,else of dataFLAG, my_column_text.ascii()=%s\n", my_column_text.ascii());
#endif
        }
      }
      line += QString("\n");
      f->writeBlock( line, qstrlen(line) );
      ++it;
    }
    f->close();
  }

  f = NULL;

}

void
StatsPanel::updatePanel()
{
#ifdef DEBUG_StatsPanel
  printf("updatePanel() about to call updateStatsPanelData, lastCommand=%s\n", lastCommand.ascii());
#endif

#ifdef DEBUG_StatsPanel_chart
  printf("updatePanel() about to call updateStatsPanelData, lastCommand=%s\n", lastCommand.ascii());
#endif

//  updateStatsPanelData(DONT_FORCE_UPDATE, lastCommand);
  updateStatsPanelData(DONT_FORCE_UPDATE, NULL);
}




QString
StatsPanel::getCollectorName()
{

}


void
StatsPanel::originalQuery()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::originalQuery() about to call updateStatsPanelData()\n");
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE, originalCommand);
}

void
StatsPanel::cviewQueryStatements()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::cviewQueryStatements about to call updateStatsPanelData, originalCommand,with vstatements=(%s)\n", 
          QString(originalCommand + " -v statements").ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE, originalCommand + " -v statements");
}

#include "CustomExperimentPanel.hxx"


void
StatsPanel::updateCurrentModifierList( std::list<std::string> genericModifierList, 
                                       std::list<std::string> *currentSelectedModifierList,
                                       std::map<std::string, bool> newDesiredModifierList)
{

 bool isDesired = FALSE;
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::updateCurrentModifierList ENTERED\n");
#endif

  // loop throught the generic list of modifiers available
  for( std::list<std::string>::const_iterator genericIt = genericModifierList.begin();
       genericIt != genericModifierList.end();  )
  {
    std::string gModifier = (std::string)*genericIt;
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateCurrentModifierList, gModifier=(%s)\n", gModifier.c_str() );
#endif

    bool FOUND = FALSE;

    // loop through the current selected list of modifiers 
    for( std::list<std::string>::const_iterator selectedIt = currentSelectedModifierList->begin();
         selectedIt != currentSelectedModifierList->end();  )
    {
      std::string sModifier = (std::string)*selectedIt;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateCurrentModifierList, sModifier=(%s)\n", sModifier.c_str() );
#endif

      if( gModifier ==  sModifier ) {   
         // It's in the list, return now
#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateCurrentModifierList, gModifier=(%s)==sModifier=(%s) FOUND==TRUE\n", gModifier.c_str(), sModifier.c_str() );
#endif
         FOUND = TRUE;
         break;
      }

      selectedIt++;

    } // end for

    std::map<std::string, bool>::iterator iter = newDesiredModifierList.find(gModifier);
    if( iter != newDesiredModifierList.end() ) {
      isDesired = iter->second;
    }
    // loop through the new user desired list of modifiers 
//    for( std::list<bool>::const_iterator desiredIt = newDesiredModifierList.begin();
//         desiredIt != newDesiredModifierList.end();  )
//    {
//      bool isDesired = (bool)*desiredIt;

      if( FOUND && isDesired ) {   
         // this modifier was set in the current list and was in the desired list so, do nothing
#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateCurrentModifierList, FOUND is TRUE, and isDesired=%d is TRUE\n", isDesired );
#endif
      } else if ( FOUND && !isDesired ) {
         // if the flag is not set - remove the existing entry.  The user doesn't want to see it
         currentSelectedModifierList->remove(gModifier);
#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateCurrentModifierList, FOUND is TRUE, and isDesired=%d is FALSE\n", isDesired );
#endif
      } else if ( !FOUND && isDesired ) {
         // the modifier is not in the list.  if the user wants to see this modifier add it else do nothing
#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateCurrentModifierList, FOUND is FALSE, and isDesired=%d is TRUE\n", isDesired );
#endif
         currentSelectedModifierList->push_back(gModifier);
      } else {
#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateCurrentModifierList, FOUND is FALSE, and isDesired=%d is FALSE\n", isDesired );
#endif
          // this modifier was not set in the current list and was not in the desired list so, do nothing
      }

//      desiredIt++;

//    } // end for desired loop

      genericIt++;
  }  // end for list_of_mo
}


#if OPTIONAL_VIEW

void
StatsPanel::sourcePanelAnnotationCreationSelected()
{

std::list<std::string> generic_list_of_modifiers;

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::sourcePanelAnnotationCreationSelected, WE have a sourcePanelAnnotationDialog=%d\n", sourcePanelAnnotationDialog);
 printf("StatsPanel::sourcePanelAnnotationCreationSelected, currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif

 if( currentCollectorStr == "hwcsamp" ) {
  generateHWCSAMPmodifiers();
  generic_list_of_modifiers = list_of_hwcsamp_modifiers;

 } else if( currentCollectorStr == "usertime" ) {
  generateUSERTIMEmodifiers();
  generic_list_of_modifiers = list_of_usertime_modifiers;
 }

  if( sourcePanelAnnotationDialog == NULL ) {
    sourcePanelAnnotationDialog = new SourcePanelAnnotationDialog(getPanelContainer()->getMainWindow(), 
                                                                  "Source Panel Annotation Views Creation:", 
                                                                  currentCollectorStr, &generic_list_of_modifiers);
    sourcePanelAnnotationDialog->show();
  } else {

#ifdef DEBUG_StatsPanel
      printf("WE have a sourcePanelAnnotationDialog=%d\n", sourcePanelAnnotationDialog);
#endif
      sourcePanelAnnotationDialog->show();
  } 

    if( sourcePanelAnnotationDialog->exec() == QDialog::Accepted ) { 

     if( currentCollectorStr == "pcsamp" ) {

      // Generate the list of pcsamp modifiers
      //generatePCSAMPmodifiers();
      //std::map<std::string, bool> pcsamp_desired_list;
      //pcsamp_desired_list.clear();
      //updateCurrentModifierList(list_of_pcsamp_modifiers, &current_list_of_pcsamp_modifiers, pcsamp_desired_list);

     }else if( currentCollectorStr == "usertime" ) {
       std::map<std::string, bool> usertime_desired_list;
       usertime_desired_list.clear();

#if DEBUG_StatsPanel
       printf("StatsPanel::sourcePanelAnnotationCreationSelected, after returning, SourcePanelAnnotationCreationDialog, sourcePanelAnnotationDialog->usertime_maxModIdx=%d\n", sourcePanelAnnotationDialog->usertime_maxModIdx);
#endif

      for (int idx=0; idx < sourcePanelAnnotationDialog->usertime_maxModIdx; idx++) {

#if DEBUG_StatsPanel
        printf("StatsPanel::sourcePanelAnnotationCreationSelected, after returning, SourcePanelAnnotationCreationDialog, sourcePanelAnnotationDialog->displayed_usertime_CheckBox_status[idx=%d]=%d\n", idx, sourcePanelAnnotationDialog->displayed_usertime_CheckBox_status[idx]);
#endif

        usertime_desired_list.insert(std::pair<std::string,int>(sourcePanelAnnotationDialog->usertime_Modifiers[idx],
                                                               sourcePanelAnnotationDialog->displayed_usertime_CheckBox_status[idx]));
      }

      updateCurrentModifierList(list_of_usertime_modifiers, &current_list_of_usertime_modifiers, usertime_desired_list);

     }else if( currentCollectorStr == "hwc" ) {

      // Generate the list of hwc modifiers
      //generateHWCmodifiers();
      //std::map<std::string, bool> hwc_desired_list;
      //hwc_desired_list.clear();
      //updateCurrentModifierList(list_of_hwc_modifiers, &current_list_of_hwc_modifiers, hwc_desired_list);

     }else if( currentCollectorStr == "hwcsamp" ) {

      // Generate the list of hwcsamp modifiers
//      generateHWCSAMPmodifiers();

      std::map<std::string, bool> hwcsamp_desired_list;
      hwcsamp_desired_list.clear();

#if DEBUG_StatsPanel
      printf("StatsPanel::sourcePanelAnnotationCreationSelected, after returning, SourcePanelAnnotationCreationDialog, sourcePanelAnnotationDialog->hwcsamp_maxModIdx=%d\n", sourcePanelAnnotationDialog->hwcsamp_maxModIdx);
#endif

      for (int idx=0; idx < sourcePanelAnnotationDialog->hwcsamp_maxModIdx; idx++) {

#if DEBUG_StatsPanel
        printf("StatsPanel::sourcePanelAnnotationCreationSelected, after returning, SourcePanelAnnotationCreationDialog, sourcePanelAnnotationDialog->displayed_hwcsamp_CheckBox_status[idx=%d]=%d\n", idx, sourcePanelAnnotationDialog->displayed_hwcsamp_CheckBox_status[idx]);
#endif

        hwcsamp_desired_list.insert(std::pair<std::string,int>(sourcePanelAnnotationDialog->hwcsamp_Modifiers[idx],
                                                               sourcePanelAnnotationDialog->displayed_hwcsamp_CheckBox_status[idx]));
      }

      updateCurrentModifierList(list_of_hwcsamp_modifiers, &current_list_of_hwcsamp_modifiers, hwcsamp_desired_list);

     }else if( currentCollectorStr == "hwctime" ) {

      // Generate the list of hwctime modifiers
      //generateHWCTIMEmodifiers();
      //std::map<std::string, bool> hwctime_desired_list;
      //hwctime_desired_list.clear();
      //updateCurrentModifierList(list_of_hwctime_modifiers, &current_list_of_hwctime_modifiers, hwctime_desired_list);

     }else if( currentCollectorStr == "io" ) {

      // Generate the list of io modifiers
      //generateIOmodifiers();
      //std::map<std::string, bool> io_desired_list;
      //io_desired_list.clear();
      //updateCurrentModifierList(list_of_io_modifiers, &current_list_of_io_modifiers, io_desired_list);

     }else if( currentCollectorStr == "iot" ) {
      // Generate the list of iot modifiers
      //generateIOTmodifiers();
      //std::map<std::string, bool> iot_desired_list;
      //iot_desired_list.clear();

     }else if( currentCollectorStr == "mpi" ) {

      // Generate the list of MPI modifiers
      //generateMPImodifiers();
      //std::map<std::string, bool> mpi_desired_list;
      //mpi_desired_list.clear();
      //updateCurrentModifierList(list_of_mpi_modifiers, &current_list_of_mpi_modifiers, mpi_desired_list);

     }else if( currentCollectorStr == "mpit" ) {

      // Generate the list of MPIT modifiers
      //generateMPITmodifiers();
      //std::map<std::string, bool> mpit_desired_list;
      //mpit_desired_list.clear();
      //updateCurrentModifierList(list_of_mpit_modifiers, &current_list_of_mpit_modifiers, mpit_desired_list);

     }else if( currentCollectorStr == "fpe" ) {

      // Generate the list of FPE modifiers
      //generateFPEmodifiers();
      //std::map<std::string, bool> fpe_desired_list;
      //fpe_desired_list.clear();
      //updateCurrentModifierList(list_of_fpe_modifiers, &current_list_of_fpe_modifiers, fpe_desired_list);

     } // end fpe specific

     updateStatsPanelData(DONT_FORCE_UPDATE);
   }
#ifdef DEBUG_StatsPanel
 printf("EXIT StatsPanel::sourcePanelAnnotationCreationSelected, sourcePanelAnnotationDialog=%d\n", sourcePanelAnnotationDialog);
#endif
}

void
StatsPanel::optionalViewsCreationSelected()
{
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::optionalViewsCreationSelected, WE have a OptionalViewsCreationDialog, optionalViewsDialog=%d\n", optionalViewsDialog);
 printf("StatsPanel::optionalViewsCreationSelected, currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif

  if( optionalViewsDialog == NULL ) {
    optionalViewsDialog = new OptionalViewsDialog(getPanelContainer()->getMainWindow(), "Optional Views Creation:", currentCollectorStr, &current_list_of_iot_modifiers);
    optionalViewsDialog->show();
    } else {

#ifdef DEBUG_StatsPanel
 printf("WE have a OptionalViewsCreationDialog, optionalViewsDialog=%d\n", optionalViewsDialog);
#endif
      optionalViewsDialog->show();
  } 

    if( optionalViewsDialog->exec() == QDialog::Accepted ) { 

     if( currentCollectorStr == "pcsamp" ) {

      // Generate the list of pcsamp modifiers
      generatePCSAMPmodifiers();

      std::map<std::string, bool> pcsamp_desired_list;
      pcsamp_desired_list.clear();
      pcsamp_desired_list.insert(std::pair<std::string,int>("pcsamp::time",optionalViewsDialog->pcsamp_time));
      pcsamp_desired_list.insert(std::pair<std::string,int>("pcsamp::percent",optionalViewsDialog->pcsamp_percent));
      pcsamp_desired_list.insert(std::pair<std::string,int>("pcsamp::ThreadAverage",optionalViewsDialog->pcsamp_ThreadAverage));
      pcsamp_desired_list.insert(std::pair<std::string,int>("pcsamp::ThreadMin",optionalViewsDialog->pcsamp_ThreadMin));
      pcsamp_desired_list.insert(std::pair<std::string,int>("pcsamp::ThreadMax",optionalViewsDialog->pcsamp_ThreadMax));

      updateCurrentModifierList(list_of_pcsamp_modifiers, &current_list_of_pcsamp_modifiers, pcsamp_desired_list);

     }else if( currentCollectorStr == "usertime" ) {
      // Generate the list of usertime modifiers
      generateUSERTIMEmodifiers();

      std::map<std::string, bool> usertime_desired_list;
      usertime_desired_list.clear();
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::exclusive_times",optionalViewsDialog->usertime_exclusive_times));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::inclusive_times",optionalViewsDialog->usertime_inclusive_times));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::percent",optionalViewsDialog->usertime_percent));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::count",optionalViewsDialog->usertime_count));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::ThreadAverage",optionalViewsDialog->usertime_ThreadAverage));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::ThreadMin",optionalViewsDialog->usertime_ThreadMin));
      usertime_desired_list.insert(std::pair<std::string,int>("usertime::ThreadMax",optionalViewsDialog->usertime_ThreadMax));
      updateCurrentModifierList(list_of_usertime_modifiers, &current_list_of_usertime_modifiers, usertime_desired_list);

     }else if( currentCollectorStr == "omptp" ) {
      // Generate the list of omptp modifiers
      generateOMPTPmodifiers();

      std::map<std::string, bool> omptp_desired_list;
      omptp_desired_list.clear();
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::exclusive_times",optionalViewsDialog->omptp_exclusive_times));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::inclusive_times",optionalViewsDialog->omptp_inclusive_times));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::percent",optionalViewsDialog->omptp_percent));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::count",optionalViewsDialog->omptp_count));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::ThreadAverage",optionalViewsDialog->omptp_ThreadAverage));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::ThreadMin",optionalViewsDialog->omptp_ThreadMin));
      omptp_desired_list.insert(std::pair<std::string,int>("omptp::ThreadMax",optionalViewsDialog->omptp_ThreadMax));
      updateCurrentModifierList(list_of_omptp_modifiers, &current_list_of_omptp_modifiers, omptp_desired_list);

     }else if( currentCollectorStr == "iop" ) {

      // Generate the list of iop modifiers
      generateIOPmodifiers();

      std::map<std::string, bool> iop_desired_list;
      iop_desired_list.clear();
      iop_desired_list.insert(std::pair<std::string,int>("iop::exclusive_times",optionalViewsDialog->iop_exclusive_times));
      iop_desired_list.insert(std::pair<std::string,int>("iop::inclusive_times",optionalViewsDialog->iop_inclusive_times));
      iop_desired_list.insert(std::pair<std::string,int>("iop::percent",optionalViewsDialog->iop_percent));
      iop_desired_list.insert(std::pair<std::string,int>("iop::count",optionalViewsDialog->iop_count));
      iop_desired_list.insert(std::pair<std::string,int>("iop::ThreadAverage",optionalViewsDialog->iop_ThreadAverage));
      iop_desired_list.insert(std::pair<std::string,int>("iop::ThreadMin",optionalViewsDialog->iop_ThreadMin));
      iop_desired_list.insert(std::pair<std::string,int>("iop::ThreadMax",optionalViewsDialog->iop_ThreadMax));
      updateCurrentModifierList(list_of_iop_modifiers, &current_list_of_iop_modifiers, iop_desired_list);

     }else if( currentCollectorStr == "hwc" ) {

      // Generate the list of hwc modifiers
      generateHWCmodifiers();

      std::map<std::string, bool> hwc_desired_list;

      hwc_desired_list.clear();
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::overflows",optionalViewsDialog->hwc_overflows));
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::counts",optionalViewsDialog->hwc_counts));
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::percent",optionalViewsDialog->hwc_percent));
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::ThreadAverage",optionalViewsDialog->hwc_ThreadAverage));
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::ThreadMin",optionalViewsDialog->hwc_ThreadMin));
      hwc_desired_list.insert(std::pair<std::string,int>("hwc::ThreadMax",optionalViewsDialog->hwc_ThreadMax));

      updateCurrentModifierList(list_of_hwc_modifiers, &current_list_of_hwc_modifiers, hwc_desired_list);

     }else if( currentCollectorStr == "hwcsamp" ) {

      // Generate the list of hwcsamp modifiers
      generateHWCSAMPmodifiers();

      std::map<std::string, bool> hwcsamp_desired_list;
      hwcsamp_desired_list.clear();
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::time",optionalViewsDialog->hwcsamp_time));
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::allEvents",optionalViewsDialog->hwcsamp_allEvents));
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::percent",optionalViewsDialog->hwcsamp_percent));
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::ThreadAverage",optionalViewsDialog->hwcsamp_ThreadAverage));
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::ThreadMin",optionalViewsDialog->hwcsamp_ThreadMin));
      hwcsamp_desired_list.insert(std::pair<std::string,int>("hwcsamp::ThreadMax",optionalViewsDialog->hwcsamp_ThreadMax));

      updateCurrentModifierList(list_of_hwcsamp_modifiers, &current_list_of_hwcsamp_modifiers, hwcsamp_desired_list);

     }else if( currentCollectorStr == "hwctime" ) {

      // Generate the list of hwctime modifiers
      generateHWCTIMEmodifiers();

      std::map<std::string, bool> hwctime_desired_list;
      hwctime_desired_list.clear();
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::exclusive_counts",optionalViewsDialog->hwctime_exclusive_counts));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::exclusive_overflows",optionalViewsDialog->hwctime_exclusive_overflows));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::inclusive_overflows",optionalViewsDialog->hwctime_inclusive_overflows));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::inclusive_counts",optionalViewsDialog->hwctime_inclusive_counts));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::percent",optionalViewsDialog->hwctime_percent));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::ThreadAverage",optionalViewsDialog->hwctime_ThreadAverage));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::ThreadMin",optionalViewsDialog->hwctime_ThreadMin));
      hwctime_desired_list.insert(std::pair<std::string,int>("hwctime::ThreadMax",optionalViewsDialog->hwctime_ThreadMax));

      updateCurrentModifierList(list_of_hwctime_modifiers, &current_list_of_hwctime_modifiers, hwctime_desired_list);

     } else if( currentCollectorStr == "io" ) {

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::optionalViewsCreationSelected, io, The user hit accept.\n");
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->io_exclusive_times=%d\n", optionalViewsDialog->io_exclusive_times);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->io_min=%d\n", optionalViewsDialog->io_min);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->io_percent=%d\n", optionalViewsDialog->io_percent);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->io_average=%d\n", optionalViewsDialog->io_average);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->io_count=%d\n", optionalViewsDialog->io_count);
#endif

      // Generate the list of io modifiers
      generateIOmodifiers();

      std::map<std::string, bool> io_desired_list;
      io_desired_list.clear();
      io_desired_list.insert(std::pair<std::string,int>("io::exclusive_times",optionalViewsDialog->io_exclusive_times));
      io_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->io_min));
      io_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->io_max));
      io_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->io_average));
      io_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->io_count));
      io_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->io_percent));
      io_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->io_stddev));
      io_desired_list.insert(std::pair<std::string,int>("ThreadAverage",optionalViewsDialog->io_ThreadAverage));
      io_desired_list.insert(std::pair<std::string,int>("ThreadMin",optionalViewsDialog->io_ThreadMin));
      io_desired_list.insert(std::pair<std::string,int>("ThreadMax",optionalViewsDialog->io_ThreadMax));

      updateCurrentModifierList(list_of_io_modifiers, &current_list_of_io_modifiers, io_desired_list);

     } else if( currentCollectorStr == "mem" ) {

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::optionalViewsCreationSelected, mem, The user hit accept.\n");
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->mem_exclusive_times=%d\n", optionalViewsDialog->mem_exclusive_times);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->mem_min=%d\n", optionalViewsDialog->mem_min);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->mem_percent=%d\n", optionalViewsDialog->mem_percent);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->mem_average=%d\n", optionalViewsDialog->mem_average);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->mem_count=%d\n", optionalViewsDialog->mem_count);
#endif

      // Generate the list of mem modifiers
      generateMEMmodifiers();

      std::map<std::string, bool> mem_desired_list;
      mem_desired_list.clear();
      mem_desired_list.insert(std::pair<std::string,int>("mem::exclusive_times",optionalViewsDialog->mem_exclusive_times));
      mem_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->mem_min));
      mem_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->mem_max));
      mem_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->mem_average));
      mem_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->mem_count));
      mem_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->mem_percent));
      mem_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->mem_stddev));
      mem_desired_list.insert(std::pair<std::string,int>("ThreadAverage",optionalViewsDialog->mem_ThreadAverage));
      mem_desired_list.insert(std::pair<std::string,int>("ThreadMin",optionalViewsDialog->mem_ThreadMin));
      mem_desired_list.insert(std::pair<std::string,int>("ThreadMax",optionalViewsDialog->mem_ThreadMax));

      updateCurrentModifierList(list_of_mem_modifiers, &current_list_of_mem_modifiers, mem_desired_list);

     } else if( currentCollectorStr == "pthreads" ) {

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::optionalViewsCreationSelected, mem, The user hit accept.\n");
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->pthreads_exclusive_times=%d\n", optionalViewsDialog->pthreads_exclusive_times);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->pthreads_min=%d\n", optionalViewsDialog->pthreads_min);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->pthreads_percent=%d\n", optionalViewsDialog->pthreads_percent);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->pthreads_average=%d\n", optionalViewsDialog->pthreads_average);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->pthreads_count=%d\n", optionalViewsDialog->pthreads_count);
#endif

      // Generate the list of mem modifiers
      generatePTHREADSmodifiers();

      std::map<std::string, bool> pthreads_desired_list;
      pthreads_desired_list.clear();
      pthreads_desired_list.insert(std::pair<std::string,int>("mem::exclusive_times",optionalViewsDialog->pthreads_exclusive_times));
      pthreads_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->pthreads_min));
      pthreads_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->pthreads_max));
      pthreads_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->pthreads_average));
      pthreads_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->pthreads_count));
      pthreads_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->pthreads_percent));
      pthreads_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->pthreads_stddev));
      pthreads_desired_list.insert(std::pair<std::string,int>("ThreadAverage",optionalViewsDialog->pthreads_ThreadAverage));
      pthreads_desired_list.insert(std::pair<std::string,int>("ThreadMin",optionalViewsDialog->pthreads_ThreadMin));
      pthreads_desired_list.insert(std::pair<std::string,int>("ThreadMax",optionalViewsDialog->pthreads_ThreadMax));

      updateCurrentModifierList(list_of_pthreads_modifiers, &current_list_of_pthreads_modifiers, pthreads_desired_list);

     }else if( currentCollectorStr == "iot" ) {

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::optionalViewsCreationSelected, The user hit accept.\n");
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_exclusive_times=%d\n", optionalViewsDialog->iot_exclusive_times);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_inclusive_times=%d\n", optionalViewsDialog->iot_inclusive_times);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_min=%d\n", optionalViewsDialog->iot_min);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_max=%d\n", optionalViewsDialog->iot_max);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_average=%d\n", optionalViewsDialog->iot_average);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_count=%d\n", optionalViewsDialog->iot_count);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_percent=%d\n", optionalViewsDialog->iot_percent);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_stddev=%d\n", optionalViewsDialog->iot_stddev);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_start_time=%d\n", optionalViewsDialog->iot_start_time);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_stop_time=%d\n", optionalViewsDialog->iot_stop_time);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_syscallno=%d\n", optionalViewsDialog->iot_syscallno);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_nsysargs=%d\n", optionalViewsDialog->iot_nsysargs);
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_retval=%d\n", optionalViewsDialog->iot_retval);
#if PATHNAME_READY
     printf("StatsPanel::optionalViewsCreationSelected, after returning, OptionalViewsCreationDialog, optionalViewsDialog->iot_pathname=%d\n", optionalViewsDialog->iot_pathname);
#endif

#endif

      // Generate the list of iot modifiers
      generateIOTmodifiers();

      std::map<std::string, bool> iot_desired_list;
      iot_desired_list.clear();
      iot_desired_list.insert(std::pair<std::string,int>("iot::exclusive_times",optionalViewsDialog->iot_exclusive_times));
      iot_desired_list.insert(std::pair<std::string,int>("iot::inclusive_times",optionalViewsDialog->iot_inclusive_times));
      iot_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->iot_min));
      iot_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->iot_max));
      iot_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->iot_average));
      iot_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->iot_count));
      iot_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->iot_percent));
      iot_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->iot_stddev));
      iot_desired_list.insert(std::pair<std::string,int>("start_time",optionalViewsDialog->iot_start_time));
      iot_desired_list.insert(std::pair<std::string,int>("stop_time",optionalViewsDialog->iot_stop_time));
      iot_desired_list.insert(std::pair<std::string,int>("syscallno",optionalViewsDialog->iot_syscallno));
      iot_desired_list.insert(std::pair<std::string,int>("nsysargs",optionalViewsDialog->iot_nsysargs));
      iot_desired_list.insert(std::pair<std::string,int>("retval",optionalViewsDialog->iot_retval));
#if PATHNAME_READY
      iot_desired_list.insert(std::pair<std::string,int>("pathname",optionalViewsDialog->iot_pathname));
#endif

      // If the modifier is in the list already then ....
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::optionalViewsCreationSelected, before calling updateCurrentModifierList\n");
      printf("StatsPanel::optionalViewsCreationSelected, &current_list_of_iot_modifiers=(%x)\n", &current_list_of_iot_modifiers);

      for( std::list<std::string>::const_iterator iot_it = list_of_iot_modifiers.begin();
           iot_it != list_of_iot_modifiers.end(); iot_it++ ) {
         std::string iot_modifier = (std::string)*iot_it;
         printf("StatsPanel::optionalViewsCreationSelected,generic iot_modifier = (%s)\n", iot_modifier.c_str() );
      }

      for( std::list<std::string>::const_iterator iot_it = current_list_of_iot_modifiers.begin();
           iot_it != current_list_of_iot_modifiers.end(); iot_it++ ) {
         std::string iot_modifier = (std::string)*iot_it;
         printf("StatsPanel::optionalViewsCreationSelected, selected iot_modifier = (%s)\n", iot_modifier.c_str() );
      }
#endif
      updateCurrentModifierList(list_of_iot_modifiers, &current_list_of_iot_modifiers, iot_desired_list);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::optionalViewsCreationSelected, after calling updateCurrentModifierList\n");
      printf("StatsPanel::optionalViewsCreationSelected, &current_list_of_iot_modifiers=(%x)\n", &current_list_of_iot_modifiers);

      for( std::list<std::string>::const_iterator iot_it = current_list_of_iot_modifiers.begin();
           iot_it != current_list_of_iot_modifiers.end(); iot_it++ ) {
         std::string iot_modifier = (std::string)*iot_it;
         printf("StatsPanel::optionalViewsCreationSelected,, iot_modifier = (%s)\n", iot_modifier.c_str() );
      }
#endif

     }else if( currentCollectorStr == "mpi" ) {

      // Generate the list of MPI modifiers
      generateMPImodifiers();

      std::map<std::string, bool> mpi_desired_list;
      mpi_desired_list.clear();
      mpi_desired_list.insert(std::pair<std::string,int>("mpi::exclusive_times",optionalViewsDialog->mpi_exclusive_times));
      mpi_desired_list.insert(std::pair<std::string,int>("mpi::inclusive_times",optionalViewsDialog->mpi_inclusive_times));
      mpi_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->mpi_min));
      mpi_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->mpi_max));
      mpi_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->mpi_average));
      mpi_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->mpi_count));
      mpi_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->mpi_percent));
      mpi_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->mpi_stddev));

      updateCurrentModifierList(list_of_mpi_modifiers, &current_list_of_mpi_modifiers, mpi_desired_list);

     }else if( currentCollectorStr == "mpip" ) {

      // Generate the list of mpip modifiers
      generateMPIPmodifiers();

      std::map<std::string, bool> mpip_desired_list;
      mpip_desired_list.clear();
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::exclusive_times",optionalViewsDialog->mpip_exclusive_times));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::inclusive_times",optionalViewsDialog->mpip_inclusive_times));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::percent",optionalViewsDialog->mpip_percent));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::count",optionalViewsDialog->mpip_count));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::ThreadAverage",optionalViewsDialog->mpip_ThreadAverage));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::ThreadMin",optionalViewsDialog->mpip_ThreadMin));
      mpip_desired_list.insert(std::pair<std::string,int>("mpip::ThreadMax",optionalViewsDialog->mpip_ThreadMax));
      updateCurrentModifierList(list_of_mpip_modifiers, &current_list_of_mpip_modifiers, mpip_desired_list);

     }else if( currentCollectorStr == "mpit" ) {

      // Generate the list of MPIT modifiers
      generateMPITmodifiers();

      std::map<std::string, bool> mpit_desired_list;
      mpit_desired_list.clear();
      mpit_desired_list.insert(std::pair<std::string,int>("mpit::exclusive_times",optionalViewsDialog->mpit_exclusive_times));
      mpit_desired_list.insert(std::pair<std::string,int>("mpit::inclusive_times",optionalViewsDialog->mpit_inclusive_times));
      mpit_desired_list.insert(std::pair<std::string,int>("min",optionalViewsDialog->mpit_min));
      mpit_desired_list.insert(std::pair<std::string,int>("max",optionalViewsDialog->mpit_max));
      mpit_desired_list.insert(std::pair<std::string,int>("average",optionalViewsDialog->mpit_average));
      mpit_desired_list.insert(std::pair<std::string,int>("count",optionalViewsDialog->mpit_count));
      mpit_desired_list.insert(std::pair<std::string,int>("percent",optionalViewsDialog->mpit_percent));
      mpit_desired_list.insert(std::pair<std::string,int>("stddev",optionalViewsDialog->mpit_stddev));
      mpit_desired_list.insert(std::pair<std::string,int>("start_time",optionalViewsDialog->mpit_start_time));
      mpit_desired_list.insert(std::pair<std::string,int>("stop_time",optionalViewsDialog->mpit_stop_time));
      mpit_desired_list.insert(std::pair<std::string,int>("source",optionalViewsDialog->mpit_source));
      mpit_desired_list.insert(std::pair<std::string,int>("dest",optionalViewsDialog->mpit_dest));
      mpit_desired_list.insert(std::pair<std::string,int>("size",optionalViewsDialog->mpit_size));
      mpit_desired_list.insert(std::pair<std::string,int>("tag",optionalViewsDialog->mpit_tag));
      mpit_desired_list.insert(std::pair<std::string,int>("communicator",optionalViewsDialog->mpit_communicator));
      mpit_desired_list.insert(std::pair<std::string,int>("datatype",optionalViewsDialog->mpit_datatype));
      mpit_desired_list.insert(std::pair<std::string,int>("retval",optionalViewsDialog->mpit_retval));

      updateCurrentModifierList(list_of_mpit_modifiers, &current_list_of_mpit_modifiers, mpit_desired_list);

     }else if( currentCollectorStr == "fpe" ) {

      // Generate the list of FPE modifiers
      generateFPEmodifiers();

      std::map<std::string, bool> fpe_desired_list;
      fpe_desired_list.clear();

#if 0
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::time",optionalViewsDialog->fpe_time));
#endif
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::counts",optionalViewsDialog->fpe_counts));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::inclusive_counts",optionalViewsDialog->fpe_inclusive_counts));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::percent",optionalViewsDialog->fpe_percent));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::ThreadAverage",optionalViewsDialog->fpe_ThreadAverage));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::ThreadMin",optionalViewsDialog->fpe_ThreadMin));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::ThreadMax",optionalViewsDialog->fpe_ThreadMax));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::inexact_result_count",optionalViewsDialog->fpe_inexact_result_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::underflow_count",optionalViewsDialog->fpe_underflow_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::overflow_count",optionalViewsDialog->fpe_overflow_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::division_by_zero_count",optionalViewsDialog->fpe_division_by_zero_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::unnormal_count",optionalViewsDialog->fpe_unnormal_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::invalid_count",optionalViewsDialog->fpe_invalid_count));
      fpe_desired_list.insert(std::pair<std::string,int>("fpe::unknown_count",optionalViewsDialog->fpe_unknown_count));

      updateCurrentModifierList(list_of_fpe_modifiers, &current_list_of_fpe_modifiers, fpe_desired_list);

     } // end fpe specific

     updateStatsPanelData(DONT_FORCE_UPDATE);
   }
#ifdef DEBUG_StatsPanel
 printf("EXIT StatsPanel::optionalViewsCreationSelected, optionalViewsDialog=%d\n", optionalViewsDialog);
#endif
}

#endif



void
StatsPanel::timeSliceSelected()
{
#ifdef DEBUG_StatsPanel
 printf("WE have a SELECT_TIME_SEGMENT\n");
#endif
  if( timeSegmentDialog == NULL )
  {
    timeSegmentDialog = new SelectTimeSegmentDialog(getPanelContainer()->getMainWindow(), "Select Time Interval:");
  }

  int skylineFLAG = getPreferenceShowSkyline();
  if( skylineFLAG )
  {
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, look up the skyline....\n");
   printf("StatsPanel::timeSliceSelected, currentCollectorStr=%s\n", currentCollectorStr.ascii() );
#endif

    InputLineObject *clip = NULL;
    QString mim = CustomExperimentPanel::getMostImportantMetric(currentCollectorStr);

// int segmentSize = 10;
    int segmentSize = getPreferenceShowSkyLineLineEdit().toInt();
    int startSegment = 0;
    int endSegment=segmentSize;
    QString segmentString = QString::null;
    while( startSegment < 100 )
    {
      segmentString += QString(" -I % %1:%2").arg(startSegment).arg(endSegment);
      startSegment = endSegment+1;
      endSegment += segmentSize;
      if( endSegment > 100 ) {
        endSegment = 100;
      }
    }

//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:25 -I % 26:50 -I % 51:75 -I % 76:100";
//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:20 -I % 21:40 -I % 41:60 -I % 61:80 -I % 81:100";
//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:10 -I % 11:20 -I % 21:30 -I % 31:40 -I % 41:50 -I % 51:60 -I % 61:70 -I % 71:80 -I % 81:90 -I % 91:100";

//    QString command = QString("expCompare -x %1 %2 %3 -v Summary -I % 0:20 -I % 21:40 -I % 41:60 -I % 61:80 -I % 81:100").arg(expID).arg(mim).arg(currentCollectorStr);
//    QString command = QString("expCompare -x %1 %2 %3 -v Summary -I % 0:10 -I % 11:20 -I % 21:30 -I % 31:40 -I % 41:50 -I % 51:60 -I % 61:70 -I % 71:80 -I % 81:90 -I % 91:100").arg(expID).arg(mim).arg(currentCollectorStr);


    QString command = QString("expCompare -x %1 %2 %3 -v Summary %4").arg(expID).arg(mim).arg(currentCollectorStr).arg(segmentString);
    command += QString(" %1").arg(currentThreadsStr);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::timeSliceSelected, do expCompare command=(%s)\n", command.ascii() );
#endif

    skylineValues.clear();
    skylineText.clear();

    QApplication::setOverrideCursor(QCursor::WaitCursor);
 
#ifdef DEBUG_StatsPanel_cache
    printf("StatsPanel::timeSliceSelected, do expCompare, about to issue a command via cli->run_Append_Input_String(), command=(%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

    bool cached_clip_processing = false;
    clip = check_for_existing_clip(command.ascii());

    if (clip) {

#ifdef DEBUG_StatsPanel_cache
       std::cerr << "StatsPanel:timeSliceSelected, FOUND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE, command=" << command << std::endl;
#endif

      // call process_clip here?
      // maybe set some flags to skip the wait loop below
      cached_clip_processing = true;

    } else {


       clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());

#ifdef DEBUG_StatsPanel_cache
       std::cerr << "StatsPanel:timeSliceSelected, DID NOT FIND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE" 
                 << ", ADDING CLIP AFTER ISSUING COMMAND, (command:clip)=(" << command << ":" << clip << ")" << std::endl;
#endif

       cached_clip_processing = false;

#ifdef DEBUG_StatsPanel_cache
       std::cerr << "StatsPanel:updateStatsPanelData, after issuing command, clip=" << clip 
                 << " sizeof(&clip)=" << sizeof(&clip) << std::endl;
#endif
       addClipForThisCommand(command.ascii(), clip);

    }


    if( clip == NULL ) {
      std::cerr << "No skyline available for this experiment.\n";
      QApplication::restoreOverrideCursor( );
      return;
    }

    Input_Line_Status status = ILO_UNKNOWN;

    if (!cached_clip_processing ) {
      while( !clip->Semantics_Complete() )
      {
        qApp->processEvents(4000);
        suspend();
        //sleep(1);
      }
    }

    std::list<CommandObject *>::iterator coi;

    coi = clip->CmdObj_List().begin();
    CommandObject *co = (CommandObject *)(*coi);

    std::list<CommandResult *>::iterator cri;
    std::list<CommandResult *> cmd_result = co->Result_List();
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++)
    {
// std::cerr << "TYPE: = " << (*cri)->Type() << "\n";
//      if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES)

      if ((*cri)->Type() == CMD_RESULT_COLUMN_ENDER)
      {
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::timeSliceSelected, Here CMD_RESULT_COLUMN_ENDER:\n");
#endif
        std::list<CommandResult *> columns;
        CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
        ccp->Value(columns);
        std::list<CommandResult *>::iterator column_it;
        for (column_it = columns.begin(); column_it != columns.end(); column_it++)
        {
          CommandResult *cr = (CommandResult *)(*column_it);
          QString vs = (*column_it)->Form().c_str();
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::timeSliceSelected, vs=(%s)\n", vs.ascii() );
#endif
          unsigned int value = 0;
          switch( cr->Type() )
          {
            case CMD_RESULT_NULL:
// std::cerr << "Got CMD_RESULT_NULL\n";
              value = 0;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_UINT:
// std::cerr << "Got CMD_RESULT_UINT\n";
              value = vs.toUInt();;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_INT:
// std::cerr << "Got CMD_RESULT_INT\n";
              value = vs.toInt();;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_FLOAT:
// std::cerr << "Got CMD_RESULT_FLOAT\n";
              value = (int)(vs.toFloat());
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_STRING:
// std::cerr << "Got CMD_RESULT_STRING\n";
//              value = vs.toInt();;
              value = 1; // FIX
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            default:
              continue;
              break;
          }
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::timeSliceSelected, int value = (%d)\n", value );
#endif
        }
      }
    }
    QApplication::restoreOverrideCursor( );
    //jeg clip->Set_Results_Used();
  
    // For now don't show text.
    skylineText.clear();
    timeSegmentDialog->cf->show();
    timeSegmentDialog->cf->setValues(skylineValues, skylineText, blue_color_names, 1);
//     timeSegmentDialog->cf->setValues(skylineValues, skylineText, color_names, MAX_COLOR_CNT);
  } else {
    timeSegmentDialog->cf->hide();
  }

  if( timeSegmentDialog->exec() == QDialog::Accepted ) { 

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, The user hit accept.\n");
   printf("StatsPanel::timeSliceSelected, start=%s end=%s\n", timeSegmentDialog->startValue->text().ascii(), timeSegmentDialog->endValue->text().ascii() );
#endif

   if (!timeIntervalString.isEmpty()) {
     prevTimeIntervalString = timeIntervalString;
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::timeSliceSelected, BEFORE UPDATING timeIntervalString=(%s)\n", timeIntervalString.ascii() );
     printf("StatsPanel::timeSliceSelected, BEFORE UPDATING timeIntervalString=(%s)\n", timeIntervalString.ascii() );
#endif
   }

   // Don't have an interval string if the values are reset back to 0:100
   int start_index = timeSegmentDialog->startValue->text().toInt();
   int end_index = timeSegmentDialog->endValue->text().toInt();
   if ( start_index == 0 && end_index == 100) {
     timeIntervalString = QString("");
   } else {
     timeIntervalString = QString(" -I % %1:%2").arg(timeSegmentDialog->startValue->text()).arg(timeSegmentDialog->endValue->text());
   }

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, start_index=%d, end_index=%d, timeIntervalString=(%s)\n", 
         start_index, end_index, timeIntervalString.ascii() );
#endif

  } else if( timeSegmentDialog->exec() == QDialog::Rejected ) {
     printf("StatsPanel::timeSliceSelected, QDIALOG::REJECTED, return\n");
     return;
  }

  // Now update the data

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::timeSliceSelected(), calling updateStatsPanelData\n" );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);


  return;
}

void
StatsPanel::focusOnExp(int val)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::focusOnExp, Just set the focus to the first for now... val=%d\n", val );
  printf("StatsPanel::focusOnExp, Menu item %s selected \n", experimentsMenu->text(val).ascii() );
#endif

  QString valStr = experimentsMenu->text(val).ascii();
  int index = valStr.find(":");
  if( index != -1 )
  {
    index++;
    int id = valStr.mid(index,9999).stripWhiteSpace().toInt();
    focusedExpID = id;
    if( experimentGroupList.count() > 0 ) {
      updateCollectorList();
    }

#ifdef DEBUG_StatsPanel
   printf("You just assigned the focusedExpID=%d\n", focusedExpID);
#endif

  }
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSourceCompare1Selected(bool use_current_item)
{

#ifdef DEBUG_StatsPanel
  printf("gotoSourceCompare1Selected() menu selected, compareExpIDs[0]=%d\n", compareExpIDs[0]);
#endif

  focusedExpID = compareExpIDs[0];
  gotoSource(use_current_item);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSourceCompare2Selected(bool use_current_item)
{
#ifdef DEBUG_StatsPanel
  printf("gotoSourceCompare2Selected() menu selected, compareExpIDs[0]=%d\n", compareExpIDs[0]);
  printf("gotoSourceCompare2Selected() menu selected, compareExpIDs[1]=%d\n", compareExpIDs[1]);
#endif

  focusedExpID = compareExpIDs[1];
  gotoSource(use_current_item);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSourceCompare3Selected(bool use_current_item)
{
#ifdef DEBUG_StatsPanel
  printf("gotoSourceCompare3Selected() menu selected, compareExpIDs[0]=%d\n", compareExpIDs[0]);
  printf("gotoSourceCompare3Selected() menu selected, compareExpIDs[1]=%d\n", compareExpIDs[1]);
  printf("gotoSourceCompare3Selected() menu selected, compareExpIDs[2]=%d\n", compareExpIDs[2]);
#endif
  focusedExpID = compareExpIDs[2];
  gotoSource(use_current_item);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSourceCompare4Selected(bool use_current_item)
{
#ifdef DEBUG_StatsPanel
  printf("gotoSourceCompare4Selected() menu selected, compareExpIDs[0]=%d\n", compareExpIDs[0]);
  printf("gotoSourceCompare4Selected() menu selected, compareExpIDs[1]=%d\n", compareExpIDs[1]);
  printf("gotoSourceCompare4Selected() menu selected, compareExpIDs[2]=%d\n", compareExpIDs[2]);
  printf("gotoSourceCompare4Selected() menu selected, compareExpIDs[3]=%d\n", compareExpIDs[3]);
#endif
  focusedExpID = compareExpIDs[3];
  gotoSource(use_current_item);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSourceCompare5Selected(bool use_current_item)
{
#ifdef DEBUG_StatsPanel
  printf("gotoSourceCompare5Selected() menu selected, compareExpIDs[0]=%d\n", compareExpIDs[0]);
  printf("gotoSourceCompare5Selected() menu selected, compareExpIDs[1]=%d\n", compareExpIDs[1]);
  printf("gotoSourceCompare5Selected() menu selected, compareExpIDs[2]=%d\n", compareExpIDs[2]);
  printf("gotoSourceCompare5Selected() menu selected, compareExpIDs[3]=%d\n", compareExpIDs[3]);
  printf("gotoSourceCompare5Selected() menu selected, compareExpIDs[4]=%d\n", compareExpIDs[4]);
#endif
  focusedExpID = compareExpIDs[4];
  gotoSource(use_current_item);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSource(bool use_current_item)
{
  QListViewItem *lvi = NULL;

#ifdef DEBUG_StatsPanel_source
    printf("StatsPanel::gotoSource() entered, use_current_item=%d\n", use_current_item);
#endif

  if( use_current_item ) {

#ifdef DEBUG_StatsPanel_source
    printf("gotoSource() menu selected, USE_CURRENT_ITEM.\n");
#endif

    nprintf(DEBUG_PANELS) ("gotoSource() menu selected, USE_CURRENT_ITEM.\n");
    lvi = currentItem;

  } else {

#ifdef DEBUG_StatsPanel_source
    printf("gotoSource() calls splv->selectedItem.\n");
#endif

    lvi =  splv->selectedItem();
  }

#ifdef DEBUG_StatsPanel_source
  printf("gotoSource() calls itemSelected(lvi).\n");
#endif
  itemSelected(lvi);
}

#include "AboutDialog.hxx"
void
StatsPanel::aboutSelected()
{
  aboutOutputString = QString("%1\n\n").arg(aboutString);
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::aboutSelected, aboutOutputString.ascii()=%s\n", aboutOutputString.ascii());
#endif

  QString command = QString::null;

  AboutOutputClass *aboutOutputClass = NULL;

  int cviewinfo_index = lastCommand.find("cview ");

  if( cviewinfo_index != -1 ) {
    aboutOutputString = QString("%1\n\n").arg(aboutString);
    aboutOutputString += QString("Where:\n");
    for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
    {
      CInfoClass *cic = (CInfoClass *)*it;
// cic->print();
      aboutOutputString += QString("-c %1:\n").arg(cic->cid);
      aboutOutputString += QString("  Experiment: %1\n").arg(cic->expID);
      aboutOutputString += QString("  Collector: %1\n").arg(cic->collector_name);
      if( cic->host_pid_names.isEmpty() ) {
        aboutOutputString += QString("  Host/pids: All\n");
      } else {
        aboutOutputString += QString("  Host/pids: %1\n").arg(cic->host_pid_names);
      }
      aboutOutputString += QString("  Metric: %1\n").arg(cic->metricStr);
    }
  }

  AboutDialog *aboutDialog = new AboutDialog(this, "StatsPanel Context:", FALSE, 0, aboutOutputString);
  aboutDialog->show();
}

void
StatsPanel::MPItraceSelected()
{
  if( MPItraceFLAG == TRUE ) {
    MPItraceFLAG = FALSE;
  } else {
    MPItraceFLAG = TRUE;
  }
}

void
StatsPanel::IOtraceSelected()
{
  if( IOtraceFLAG == TRUE ) {
    IOtraceFLAG = FALSE;
  } else {
    IOtraceFLAG = TRUE;
  }
}


void
StatsPanel::manageProcessesSelected()
{
// printf("manageProcessesSelected() menu selected.\n");
  QString name = QString("ManageProcessesPanel [%1]").arg(expID);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::manageProcessesSelected(), expID=%d\n", expID);
#endif

  Panel *manageProcessesPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::manageProcessesSelected(), manageProcessesPanel=%d\n", manageProcessesPanel);
#endif

  if( manageProcessesPanel ) { 
    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() found ManageProcessesPanel found.. raise it.\n");
#ifdef DEBUG_StatsPanel
    printf( "StatsPanel::manageProcessesSelected() found ManageProcessesPanel found.. raise it.\n");
#endif
    getPanelContainer()->raisePanel(manageProcessesPanel);
  } else {
//    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() no ManageProcessesPanel found.. create one.\n");
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::manageProcessesSelected() no ManageProcessesPanel found.. create one.\n");
#endif

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    manageProcessesPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", startPC, ao);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::manageProcessesSelected(), just created manageProcessesPanel=%d\n", manageProcessesPanel);
#endif
    delete ao;
  }

  if( manageProcessesPanel ) {

//    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", manageProcessesPanel->getName());
//
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::manageProcessesSelected, call (%s)'s listener routine.\n", manageProcessesPanel->getName());
#endif

    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() ) {
      Experiment *experiment = eo->FW();
      UpdateObject *msg = new UpdateObject((void *)experiment, expID, "pcsamp", 1);
      manageProcessesPanel->listener( (void *)msg );
    }
  }

}

#ifdef OLDWAY
void
StatsPanel::headerSelected(int index)
{
  QString headerStr = splv->columnText(index);
// printf("StatsPanel::%d headerSelected(%s)\n", index, headerStr.ascii() );

  if(  headerStr == "|Difference|" )
  {
#ifdef DEBUG_Sorting
    printf("StatsPanel::headerSelected set absDiffFLAG to true\n");
#endif
    absDiffFLAG = TRUE; // Major hack to do sort based on absolute values.
  }
}
#endif // OLDWAY

void
StatsPanel::itemSelected(int index)
{

#ifdef DEBUG_StatsPanel_source
  printf("StatsPanel::itemSelected (index) entered , index=(%d), splv=%0x\n", index, splv);
#endif

  QListViewItemIterator it( splv );
  int i = 0;
  while( it.current() ) {
    QListViewItem *item = *it;

#ifdef DEBUG_StatsPanel_source
    printf("StatsPanel::itemSelected,i=%d, index=%d\n", i, index);
#endif

    if( i == index ) {

      currentItem = (SPListViewItem *)item;
      currentItemIndex = index;

#ifdef DEBUG_StatsPanel_source
      printf("A: SETTING currentItem, currentItemIndex set to %d\n", currentItemIndex);
#endif

     // highlight the list item
     // Now call the action routine.
      splv->setSelected((QListViewItem *)item, TRUE);
      itemSelected(item);
      break;

    } // end i==index

    i++;
    it++;
  } // end while
    
#ifdef DEBUG_StatsPanel_source
  printf("StatsPanel::itemSelected exit, index=(%d)\n", index);
#endif
}

void
StatsPanel::returnPressed(QListViewItem *item)
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::returnPressed, lastCommand=(%s)\n", lastCommand.ascii());
#endif

  if( lastCommand.contains("Butterfly") ) {
    updateStatsPanelData(DONT_FORCE_UPDATE );
  } else {
    itemSelected( item );
  }
}

void
StatsPanel::itemSelected(QListViewItem *item)
{
  nprintf(DEBUG_PANELS) ("StatsPanel::itemSelected(QListViewItem *) item=%s\n", item->text(0).ascii() );

  if( item ) {

#ifdef DEBUG_StatsPanel_source
    printf("StatsPanel::itemSelected(QListViewItem *) item=%s, splv=0x%x\n", item->text(fieldCount-1).ascii(), splv );
#endif

    matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) );

  }
}


static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanel::doOption(int id)
{

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::doOption() id=%d\n", id);
#endif

  if( splv->columnWidth(id) ) {

    cwidth = splv->columnWidth(id);
    splv->hideColumn(id);

#ifdef DEBUG_StatsPanel
 printf("doOption() if cwidth=%d\n", cwidth);
#endif

  } else {

    splv->setColumnWidth(id, cwidth);

#ifdef DEBUG_StatsPanel
 printf("doOption() else cwidth=%d\n", cwidth);
#endif

  } 
}

bool
StatsPanel::matchSelectedItem(QListViewItem *item, std::string sf )
{
#ifdef DEBUG_StatsPanel_source
  printf("StatsPanel::matchSelectedItem() entered. sf=%s\n", sf.c_str() );
#endif
  nprintf( DEBUG_PANELS) ("matchSelectedItem() entered. sf=%s\n", sf.c_str() );

  SPListViewItem *spitem = (SPListViewItem *)item;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, matchSelectedItem() entered. sf=%s\n", sf.c_str() );
  printf("StatsPanel::matchSelectedItem, A: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
  printf("StatsPanel::matchSelectedItem, A: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
  printf("StatsPanel::matchSelectedItem, A: currentMetricStr=(%s)\n", currentMetricStr.ascii() );
#endif

  QString lineNumberStr = "-1";
  QString filename = QString::null;
  SourceObject *spo = NULL;
  QString ssf = QString(sf.c_str()).stripWhiteSpace();

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::matchSelectedItem after stripWhiteSpace on sf, ssf.ascii()=(%s)\n", ssf.ascii() ); 
#endif

  filename = spitem->fileName.ascii();

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::matchSelectedItem after spitem->fileName.ascii(), filename.ascii()=(%s)\n", filename.ascii() ); 
#endif

  if (filename == QString::null) {
    filename = getFilenameFromString(ssf);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::matchSelectedItem after getFilenameFromString(ssf), filename=(%s)\n", filename.ascii() ); 
#endif
  }

  lineNumberStr = QString("%1").arg(spitem->lineNumber);
  if (lineNumberStr == "-1") {
    lineNumberStr = getLinenumberFromString(ssf);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::matchSelectedItem after getLinenumberFromString(ssf), lineNumberStr=(%s)\n", lineNumberStr.ascii() ); 
#endif
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, ssf=(%s)\n", ssf.ascii() ); 
  printf("StatsPanel::matchSelectedItem, spitem->funcName=(%s)\n", spitem->funcName.ascii() ); 
  printf("StatsPanel::matchSelectedItem, spitem->fileName=(%s)\n", spitem->fileName.ascii() ); 
  printf("StatsPanel::matchSelectedItem, spitem->lineNumber=(%d)\n", spitem->lineNumber ); 
#endif

  nprintf( DEBUG_PANELS) ("spitem->funcName=(%s)\n", spitem->funcName.ascii() ); 
  nprintf( DEBUG_PANELS) ("spitem->fileName=(%s)\n", spitem->fileName.ascii() ); 
  nprintf( DEBUG_PANELS) ("spitem->lineNumber=(%d)\n", spitem->lineNumber ); 


  // Explicitly make sure the highlightList is clear.
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();


  QApplication::setOverrideCursor(QCursor::WaitCursor);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, LOOK UP FILE HIGHLIGHTS THE NEW WAY!, filename.ascii()=%s, expID=%d, focusedExpID=%d\n", 
         filename.ascii(), expID, focusedExpID);
#endif
#ifdef DEBUG_StatsPanel_source
  printf("StatsPanel::matchSelectedItem, LOOK UP FILE HIGHLIGHTS (SOURCEPANELMETRIC), sourcePanelMetricStr.ascii()=%s\n", sourcePanelMetricStr.ascii());
  printf("StatsPanel::matchSelectedItem, LOOK UP FILE HIGHLIGHTS (CURRENTMETRIC), currentMetricStr.ascii()=%s\n", currentMetricStr.ascii());
#endif

  if ( sourcePanelMetricStr.isEmpty() ) {
     spo = lookUpFileHighlights(filename, lineNumberStr, highlightList, currentMetricStr);
  } else {
     spo = lookUpFileHighlights(filename, lineNumberStr, highlightList, sourcePanelMetricStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, after calling lookUpFileHighlights, filename=%s expID=%d, focusedExpID=%d\n", 
         filename.ascii(), expID, focusedExpID);
#endif

  if( !spo ) {
      int compareID = -1;

      // If the groupID doesn't equal the expID we are looking at an offshoot from a compare panel
      // The source panel won't raise if the groupID and expID are not the same because it doesn't want
      // to change everyone elses source panel focus.  By passing a compare id that matches the groupID
      // we can tell the source panel it is ok to show source from experiment 3 in a groupID of 1 panel.

      // For example we openss and compare two experiments.  They will be experiment 1 and experiment 3
      // if we then click to view functions for experiment three we still retain the groupID of 1
      // if we try to view the source, we are asking the groupID compare panel to raise the source panel for
      // experiment 3 which normally would have a groupID of 3.  This extra argument (compare_id) should get us
      // around that problem.

      if (groupID != expID) {
        compareID = expID;
      }
      spo = new SourceObject(NULL, NULL, -1, expID, TRUE, NULL, compareID);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::matchSelectedItem, created spo new SourceObject with expID=%d, and linenumber = -1, compareID=%d\n",
            expID, compareID);
#endif
  }

  if( spo ) {

      QString name = QString::null;

      if( expID == -1  ) {
        name = QString("Source Panel [%1]").arg(groupID);
      } else {
        if ( focusedExpID != -1) {
           name = QString("Source Panel [%1]").arg(focusedExpID);
        } else {
           name = QString("Source Panel [%1]").arg(expID);
        }
      }

      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel ) {

        char *panel_type = (char *) "Source Panel";
        PanelContainer *startPC = NULL;

        if( getPanelContainer()->parentPanelContainer != NULL ) {
          startPC = getPanelContainer()->parentPanelContainer;
        } else {
          startPC = getPanelContainer();
        }

        PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);
        sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
      }

      if( sourcePanel ) {
       sourcePanel->listener((void *)spo);
#ifdef DEBUG_StatsPanel_source
       printf("StatsPanel::matchSelectedItem, Returned from sending of spo, via sourcePanel->listener!\n");
#endif
      }
  }
  QApplication::restoreOverrideCursor( );

}

// Start looking for tokens at start_index and return  the value you find
// Be careful with this routine.  It isn't context sensitive.  The searchStr
// is a hint on what the caller is expecting to find, but if not found it
// searches for other tokens in no particular order.
static int findNextMajorToken(QString str, int start_index, QString searchStr)
{

 int eol_index = str.length() - 1;
 int end_index = -1;
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, enter start_index=%d, str.ascii()=(%s), searchStr=(%s)\n", start_index, str.ascii(), searchStr.ascii() );
#endif


 end_index = str.find(searchStr, start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, searchStr=(%s), end_index=%d\n", searchStr.ascii(), end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }
 

 end_index = str.find("-h", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -h, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-m", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -m, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }
 end_index = str.find("-p", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -p, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-r", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -r, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-t", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -t, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("Average", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, Average, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find(";", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, semi-colon-> ;, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-I", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, -I, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("%", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, %, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("\n", start_index);
#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, \n end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

#ifdef DEBUG_StatsPanel_info
 printf(" findNextMajorToken, FallThrough CASE, assume we hit end of line, eol_index=%d\n", eol_index);
#endif
 return(eol_index);

}


void StatsPanel::getRankThreadPidList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getRankThreadPidList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 ) {
    command = QString("list -v ranks -x %1").arg(exp_id);
  } else {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = RANK;

#ifdef DEBUG_StatsPanel_info
  std::cerr << "StatsPanel::getRankThreadPidList-attempt to run command.ascii()" << command.ascii() << std::endl;
#endif

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  list_of_pids.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getRankThreadPidList, LIST_V_RANKS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getRankThreadPidList, CACHING LIST_V_RANKS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;

    addIntListForThisCommand(command.ascii(), list_of_pids);
  } 

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getRankThreadPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif

  if( list_of_pids.size() == 0 ) {
    currentThreadsStrENUM = THREAD;
    if( focusedExpID == -1 ) {
      command = QString("list -v threads -x %1").arg(exp_id);
    } else {
      command = QString("list -v threads -x %1").arg(focusedExpID);
    }

// printf("attempt to run (%s)\n", command.ascii() );

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  list_of_pids.clear();
  list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getRankThreadPidList, LIST_V_THREADS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;

    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getRankThreadPidList, CACHING LIST_V_THREADS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
    addIntListForThisCommand(command.ascii(), list_of_pids);

  } 

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getRankThreadPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif

  } 

  if( list_of_pids.size() == 0 ) {
    currentThreadsStrENUM = PID;
    if( focusedExpID == -1 ) {
      command = QString("list -v pids -x %1").arg(exp_id);
    } else {
      command = QString("list -v pids -x %1").arg(focusedExpID);
    }

// printf("StatsPanel::getRankThreadPidList, attempt to run (%s)\n", command.ascii() );

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  list_of_pids.clear();
  list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getRankThreadPidList, LIST_V_PIDS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }
    //std::cerr << "StatsPanel::getRankThreadPidList, CACHING LIST_V_PIDS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
    addIntListForThisCommand(command.ascii(), list_of_pids);

  } 

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getRankThreadPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif

  } 

  if( list_of_pids.size() > 1 ) {

    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ ) {

      int64_t pid = (int64_t)*it;

#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::getRankThreadPidList, pid=(%ld)\n", pid );
#endif

    }
  }

 } else {

    list_of_pids.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getRankThreadPidList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif

 }
}


void StatsPanel::getSeparateRanksAndThreadsList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparateRanksAndThreadsList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

  if( focusedExpID == -1 ) {
    command = QString("list -v ranksandthreads -x %1").arg(exp_id);
  } else {
    command = QString("list -v ranksandthreads -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = RANK;

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.

  separate_list_of_ranksandthreads.clear();
  bool list_is_cached = checkForExistingStringList( command.ascii(), separate_list_of_ranksandthreads);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparateRankList, LIST_V_RANKS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached
            << " separate_list_of_ranksandthreads.size()=" << separate_list_of_ranksandthreads.size() << std::endl;
#endif

  if (!list_is_cached) {

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRankList-attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    separate_list_of_ranksandthreads.clear();
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &separate_list_of_ranksandthreads, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparateRankList, CACHING LIST_V_RANKS, command=" << command.ascii()
    //          << " separate_list_of_ranksandthreads.size()=" << separate_list_of_ranksandthreads.size() << std::endl;

    addStringListForThisCommand(command.ascii(), separate_list_of_ranksandthreads);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparateRankList, ran %s, separate_list_of_ranksandthreads.size()=%d\n", command.ascii(), separate_list_of_ranksandthreads.size() );
#endif


 } else {
    separate_list_of_ranksandthreads.clear();
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRankList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif
 }

}


void StatsPanel::getSeparateRanksAndPidsList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparateRanksAndPidsList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

  if( focusedExpID == -1 ) {
    command = QString("list -v ranksandpids -x %1").arg(exp_id);
  } else {
    command = QString("list -v ranksandpids -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = RANK;

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.

  separate_list_of_ranksandpids.clear();
  bool list_is_cached = checkForExistingStringList( command.ascii(), separate_list_of_ranksandpids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparateRanksAndPidsList, LIST_V_RANKS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached
            << " separate_list_of_ranksandpids.size()=" << separate_list_of_ranksandpids.size() << std::endl;
#endif

  if (!list_is_cached) {

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRanksAndPidsList-attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    separate_list_of_ranksandpids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &separate_list_of_ranksandpids, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparateRanksAndPidsList, CACHING LIST_V_RANKS, command=" << command.ascii()
    //          << " separate_list_of_ranksandpids.size()=" << separate_list_of_ranksandpids.size() << std::endl;

    addStringListForThisCommand(command.ascii(), separate_list_of_ranksandpids);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparateRanksAndPidsList, ran %s, separate_list_of_ranksandpids.size()=%d\n", command.ascii(), separate_list_of_ranksandpids.size() );
#endif


 } else {
    separate_list_of_ranksandpids.clear();
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRanksAndPidsList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif
 }

}



void StatsPanel::getSeparatePidsAndThreadsList(int exp_id)
{

// Now get the process and thread pairs.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparatePidsAndThreadsList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

  if( focusedExpID == -1 ) {
    command = QString("list -v pidsandthreads -x %1").arg(exp_id);
  } else {
    command = QString("list -v pidsandthreads -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = PID;

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.

  separate_list_of_pidsandthreads.clear();
  bool list_is_cached = checkForExistingStringList( command.ascii(), separate_list_of_pidsandthreads);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparatePidsAndThreadsList, LIST_V_PROCESSESANDTHREADS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached
            << " separate_list_of_pidsandthreads.size()=" << separate_list_of_pidsandthreads.size() << std::endl;
#endif

  if (!list_is_cached) {

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparatePidsAndThreadsList-attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    separate_list_of_pidsandthreads.clear();
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &separate_list_of_pidsandthreads, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparatePidsAndThreadsList, CACHING LIST_V_RANKS, command=" << command.ascii()
    //          << " separate_list_of_pidsandthreads.size()=" << separate_list_of_pidsandthreads.size() << std::endl;

    addStringListForThisCommand(command.ascii(), separate_list_of_pidsandthreads);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparatePidsAndThreadsList, ran %s, separate_list_of_pidsandthreads.size()=%d\n", command.ascii(), separate_list_of_pidsandthreads.size() );
#endif


 } else {
    separate_list_of_pidsandthreads.clear();
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparatePidsAndThreadsList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif
 }

}


void StatsPanel::getSeparatePidList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparatePidList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

    currentThreadsStrENUM = PID;
    if( focusedExpID == -1 ) {
      command = QString("list -v pids -x %1").arg(exp_id);
    } else {
      command = QString("list -v pids -x %1").arg(focusedExpID);
    }

// printf("StatsPanel::getSeparatePidList, attempt to run (%s)\n", command.ascii() );

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  separate_list_of_pids.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), separate_list_of_pids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparatePidList, LIST_V_PIDS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " separate_list_of_pids.size()=" << separate_list_of_pids.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    separate_list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &separate_list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparatePidList, CACHING LIST_V_THREADS, command=" << command.ascii() << " separate_list_of_pids.size()=" << separate_list_of_pids.size() << std::endl;
    addIntListForThisCommand(command.ascii(), separate_list_of_pids);
  } 

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparatePidList, ran %s, separate_list_of_pids.size()=%d\n", command.ascii(), separate_list_of_pids.size() );
#endif

  if( separate_list_of_pids.size() > 1 ) {

    for( std::list<int64_t>::const_iterator it = separate_list_of_pids.begin();
         it != separate_list_of_pids.end(); it++ ) {

      int64_t pid = (int64_t)*it;

#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::getSeparatePidList, pid=(%ld)\n", pid );
#endif

    }
  }

 } else {

    separate_list_of_pids.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparatePidList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif

 }
}

void StatsPanel::getPartialPidList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getPartialPidList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

    currentThreadsStrENUM = PID;
    if( focusedExpID == -1 ) {
      command = QString("list -v pids -x %1 %2").arg(exp_id).arg(currentThreadsStr);
    } else {
      command = QString("list -v pids -x %1 %2").arg(focusedExpID).arg(currentThreadsStr);
    }

// printf("attempt to run (%s)\n", command.ascii() );

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    partial_list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &partial_list_of_pids, clip, TRUE ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getPartialPidList, ran %s, partial_list_of_pids.size()=%d\n", command.ascii(), partial_list_of_pids.size() );
#endif

  if( partial_list_of_pids.size() > 1 ) {

    for( std::list<int64_t>::const_iterator it = partial_list_of_pids.begin();
         it != partial_list_of_pids.end(); it++ ) {

      int64_t pid = (int64_t)*it;

#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::getPartialPidList, pid=(%ld)\n", pid );
#endif

    }
  }

 } else {

    partial_list_of_pids.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getPartialPidList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif

 }
}

void StatsPanel::getSeparateThreadList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparateThreadList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

    currentThreadsStrENUM = THREAD;
    if( focusedExpID == -1 ) {
      command = QString("list -v threads -x %1").arg(exp_id);
    } else {
      command = QString("list -v threads -x %1").arg(focusedExpID);
    }

// printf("attempt to run (%s)\n", command.ascii() );

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  separate_list_of_threads.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), separate_list_of_threads);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparateThreadList, LIST_V_THREADS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " separate_list_of_threads.size()=" << separate_list_of_threads.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;

    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &separate_list_of_threads, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }
    //std::cerr << "StatsPanel::getSeparateThreadList, CACHING LIST_V_THREADS, command=" << command.ascii() << " separate_list_of_threads.size()=" << separate_list_of_threads.size() << std::endl;
    addIntListForThisCommand(command.ascii(), separate_list_of_threads);
  } 

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparateThreadList, ran %s, separate_list_of_threads.size()=%d\n", command.ascii(), separate_list_of_threads.size() );
#endif

 } else {

    separate_list_of_threads.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateThreadList, not valid exp_id=%d, no threads\n", exp_id);
#endif

 }
}

void StatsPanel::getSeparateRankList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getSeparateRankList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

  if( focusedExpID == -1 ) {
    command = QString("list -v ranks -x %1").arg(exp_id);
  } else {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = RANK;

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  separate_list_of_ranks.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), separate_list_of_ranks);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparateRankList, LIST_V_RANKS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " separate_list_of_ranks.size()=" << separate_list_of_ranks.size() << std::endl;
#endif

  if (!list_is_cached) {

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRankList-attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    separate_list_of_ranks.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &separate_list_of_ranks, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparateRankList, CACHING LIST_V_RANKS, command=" << command.ascii() 
    //          << " separate_list_of_ranks.size()=" << separate_list_of_ranks.size() << std::endl;

    addIntListForThisCommand(command.ascii(), separate_list_of_ranks);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getSeparateRankList, ran %s, separate_list_of_ranks.size()=%d\n", command.ascii(), separate_list_of_ranks.size() );
#endif


 } else {

    separate_list_of_ranks.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getSeparateRankList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif

 }
}


void StatsPanel::getRankThreadList(int exp_id)
{

// Now get the threads.

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getRankThreadList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {

  if( focusedExpID == -1 ) {
    command = QString("list -v ranks -x %1").arg(exp_id);
  } else {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }

  currentThreadsStrENUM = RANK;

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getRankThreadList-attempt to run (%s)\n", command.ascii() );
#endif

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  rt_list_of_ranks.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), rt_list_of_ranks);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getSeparateThreadList, LIST_V_THREADS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " rt_list_of_ranks.size()=" << rt_list_of_ranks.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    rt_list_of_ranks.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &rt_list_of_ranks, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getSeparateThreadList, CACHING LIST_V_THREADS, command=" << command.ascii() << " rt_list_of_ranks.size()=" << rt_list_of_ranks.size() << std::endl;

    addIntListForThisCommand(command.ascii(), rt_list_of_ranks);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getRankThreadList, ran %s, rt_list_of_ranks.size()=%d\n", command.ascii(), rt_list_of_ranks.size() );
#endif

  if( rt_list_of_ranks.size() > 0 ) {
    infoString += QString("\n Ranks and Underlying Threads: ");
    bool first_time = true;
    int rank_count = 0;
    for( std::list<int64_t>::const_iterator it = rt_list_of_ranks.begin();
         it != rt_list_of_ranks.end(); it++ )
    {
      rank_count = rank_count + 1;
      int64_t rank = (int64_t)*it;
      QString rankStr = QString("%1").arg(rank);
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::getRankThreadList, rank=%ld, rank_count=%d, rt_list_of_ranks.size()=%d\n", rank, rank_count, rt_list_of_ranks.size() );
#endif
     currentThreadsStrENUM = THREAD;
     if( focusedExpID == -1 ) {
       command = QString("list -v threads -x %1 -r %2").arg(exp_id).arg(rank);
     } else {
       command = QString("list -v threads -x %1 -r %2").arg(focusedExpID).arg(rank);
     }

// printf("attempt to run (%s)\n", command.ascii() );

     CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
     rt_list_of_threads.clear();
     int thread_count = 0;
     InputLineObject *clip = NULL;

     if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
            &rt_list_of_threads, clip, TRUE ) ) {
       printf("Unable to run %s command.\n", command.ascii() );
     }

#ifdef DEBUG_StatsPanel_info
     printf("StatsPanel::getRankThreadList, ran %s, rt_list_of_threads.size()=%d\n", command.ascii(), rt_list_of_threads.size() );
#endif
     if( rt_list_of_threads.size() > 0 )
     {
       for( std::list<int64_t>::const_iterator it = rt_list_of_threads.begin();
            it != rt_list_of_threads.end(); it++ )
       {
         thread_count = thread_count + 1;
         int64_t thread = (int64_t)*it;
         QString threadStr = QString("%1").arg(thread);
#ifdef DEBUG_StatsPanel_info
         printf("StatsPanel::getRankThreadList, thread=%ld, thread_count=%d, rt_list_of_threads.size()=%d\n", thread, thread_count, rt_list_of_threads.size() );
#endif
          
       } // end for list of threads

     } // end if threads.size

    } // end for rank

  }


 } else {

    rt_list_of_ranks.clear();

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::getRankThreadList, not valid exp_id=%d, no ranks with underlying threads\n", exp_id);
#endif

 }
}

void StatsPanel::getHostList(int exp_id)
{

// Now get the hosts

 QString command = QString::null;

#ifdef DEBUG_StatsPanel_info
 printf("StatsPanel::getHostList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadsStrENUM = UNKNOWN;

 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 ) {
    command = QString("list -v hosts -x %1").arg(exp_id);
  } else {
    command = QString("list -v hosts -x %1").arg(focusedExpID);
  }

//  currentThreadsStrENUM = RANK;

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getHostList-attempt to run (%s)\n", command.ascii() );
#endif

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  list_of_hosts.clear();
  bool list_is_cached = checkForExistingStringList( command.ascii(), list_of_hosts);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::getHostList, LIST_V_HOSTS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " list_of_hosts.size()=" << list_of_hosts.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_hosts.clear();
    InputLineObject *clip = NULL;

    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_hosts, clip, FALSE /* mark value for delete */ ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::getHostList, CACHING LIST_V_HOSTS, command=" << command.ascii() << " list_of_hosts.size()=" << list_of_hosts.size() << std::endl;

    addStringListForThisCommand(command.ascii(), list_of_hosts);
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getHostList, ran %s, list_of_hosts.size()=%d\n", command.ascii(), list_of_hosts.size() );
#endif

  if( list_of_hosts.size() > 1 ) {
    for( std::list<std::string>::const_iterator it = list_of_hosts.begin();
         it != list_of_hosts.end(); it++ ) {
      std::string host = *it;

#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::getHostList, host=(%s)\n", host.c_str() );
#endif

    }
  }
 } else {

  list_of_hosts.clear();

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getHostList, not valid exp_id=%d, no hosts\n", exp_id);
#endif

 }
}

void StatsPanel::getExecutableList(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getExecutableList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadsStrENUM = UNKNOWN;
 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 ) {
    command = QString("list -v executable -x %1").arg(exp_id);
  } else {
    command = QString("list -v executable -x %1").arg(focusedExpID);
  }
//  currentThreadsStrENUM = RANK;
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getExecutableList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_executables.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_executables, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getExecutableList, ran %s, list_of_executables.size()=%d\n", command.ascii(), list_of_executables.size() );
#endif

  if( list_of_executables.size() > 1 )
  {
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      std::string executable = *it;
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::getExecutableList, executable=(%s)\n", executable.c_str() );
#endif
    }
  }
 } else {
  list_of_executables.clear();
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getExecutableList, not valid exp_id=%d, no executables\n", exp_id);
#endif

 }
}


void StatsPanel::getApplicationCommand(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel_APPC
  printf("StatsPanel::getApplicationCommand exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 if( exp_id > 0 || focusedExpID > 0 ) {
//  currentThreadsStrENUM = UNKNOWN;
  if( focusedExpID == -1 ) {
    command = QString("list -v appcommand -x %1").arg(exp_id);
  } else {
    command = QString("list -v appcommand -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel_APPC
  printf("StatsPanel::getApplicationCommand-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_appcommands.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_appcommands, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel_APPC
  printf("StatsPanel::getApplicationCommand, ran %s, list_of_appcommands.size()=%d\n", command.ascii(), list_of_appcommands.size() );
#endif

  if( list_of_appcommands.size() >= 1 ) {
    for( std::list<std::string>::const_iterator it = list_of_appcommands.begin();
         it != list_of_appcommands.end(); it++ )
    {
      std::string appcommands = *it;
#ifdef DEBUG_StatsPanel_APPC
      printf("StatsPanel::getApplicationCommand, appcommands=(%s)\n", appcommands.c_str() );
#endif
    }
  }
 } else {
  list_of_appcommands.clear();
#ifdef DEBUG_StatsPanel_APPC
  printf("StatsPanel::getApplicationCommand, not valid exp_id=%d, no appcommand\n", exp_id);
#endif
 }
}

void StatsPanel::getExperimentType(int exp_id)
{

// Now get the executables
  QString command = QString::null;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 if( exp_id > 0 || focusedExpID > 0 ) {
//  currentThreadsStrENUM = UNKNOWN;
  if( focusedExpID == -1 ) {
    command = QString("list -v types -x %1").arg(exp_id);
  } else {
    command = QString("list -v types -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_types.clear();
  InputLineObject *clip = NULL;

  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_types, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType, ran %s, list_of_types.size()=%d\n", command.ascii(), list_of_types.size() );
#endif

  if( list_of_types.size() > 1 ) {
    for( std::list<std::string>::const_iterator it = list_of_types.begin();
         it != list_of_types.end(); it++ ) {
      std::string types = *it;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getExperimentType, types=(%s)\n", types.c_str() );
#endif

    } // end for

  } // end types size is greater than 1

 } else {
  // No valid experiment 
  list_of_types.clear();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType, not valid exp_id=%d, no types\n", exp_id);
#endif

 }
}

QString StatsPanel::getFullPathSrcFileName(QString noPathFileName, int exp_id)
{

  QString command = QString::null;
  std::string fullPathSrcFilename = "";

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFullPathSrcFileName noPathFileName=%s, exp_id=%d\n", noPathFileName.ascii(), exp_id);
#endif

  command = QString("list -v srcfullpath -x %1 -f %2").arg(exp_id).arg(noPathFileName);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFullPathSrcFileName-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_srcfilenames.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_srcfilenames, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFullPathSrcFileName, ran %s, list_of_srcfilenames.size()=%d\n", command.ascii(), list_of_srcfilenames.size() );
#endif

  if( list_of_srcfilenames.size() > 0 ) {
    for( std::list<std::string>::const_iterator it = list_of_srcfilenames.begin();
         it != list_of_srcfilenames.end(); it++ )
    {
      fullPathSrcFilename = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getFullPathSrcFileName, fullPathSrcFilename=(%s)\n", fullPathSrcFilename.c_str() );
#endif
    }
  }
  return(QString(fullPathSrcFilename.c_str()));
}



#ifdef DBNAMES
void StatsPanel::getDatabaseName(int exp_id, bool force_exp)
{

// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadsStrENUM = UNKNOWN;
  if( focusedExpID == -1 || force_exp) {
    command = QString("list -v database -x %1").arg(exp_id);
  } else {
    command = QString("list -v database -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_dbnames.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_dbnames, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName, ran %s, list_of_dbnames.size()=%d\n", command.ascii(), list_of_dbnames.size() );
#endif

  if( list_of_dbnames.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      std::string databaseName = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getDatabaseName, databaseName=(%s)\n", databaseName.c_str() );
#endif
    }
  }
}
#endif



#ifdef DBNAMES
QString StatsPanel::getDBName(int exp_id)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDBName() , list_of_dbnames.size()=%d\n", list_of_dbnames.size());
#endif
  bool force_use_of_exp_id = TRUE;
  getDatabaseName(exp_id, force_use_of_exp_id);
  if( list_of_dbnames.size() > 0 )
  {
    int dbnames_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      dbnames_count = dbnames_count + 1;
      std::string dbnames = *it;
      QString dbnamesStr = QString("%1").arg(dbnames.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getDBName, dbnames=(%s), dbnamesStr.ascii()=%s\n", dbnames.c_str(), dbnamesStr.ascii() );
#endif

      QSettings *settings = new QSettings();
      bool fullPathBool = settings->readBoolEntry( "/openspeedshop/general/viewFullPath");
#ifdef DEBUG_StatsPanel
      printf("CLI: /openspeedshop/general/viewFullPath == fullPathBool=(%d)\n", fullPathBool );
#endif

      QString db  = dbnamesStr;
      if (!fullPathBool) {
           int basename_index = dbnamesStr.findRev("/");
#ifdef DEBUG_StatsPanel
           printf("basename_index = %d\n", basename_index);
#endif
           if( basename_index != -1 ) {
#ifdef DEBUG_StatsPanel
             printf("dbnamesStr.length() = %d\n", dbnamesStr.length());
#endif
             db =  dbnamesStr.right((dbnamesStr.length()-basename_index)-1);
           } 
      }
      delete settings;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getDBName, returning db=%s\n", db.ascii());
#endif


      return(db);
    }

  }
}
#endif


void StatsPanel::updateMetadataForCompareIndication( QString compareStr )
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForCompareIndication, ENTER compareStr=(%s)\n", 
         compareStr.ascii() );
  printf("StatsPanel::updateMetadataForCompareIndication, ENTER infoSummaryStr.isEmpty=(%d)\n", 
         infoSummaryStr.isEmpty() );

  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateMetadataForCompareIndication, ENTER infoSummaryStr=(%s)\n", 
            infoSummaryStr.ascii() );
  }
#endif

  if (!infoSummaryStr.isEmpty()) {
      QString tempStr = QString("\nView consists of comparison columns %1").arg(compareStr);
      infoSummaryStr.append(tempStr);
  } else {
      infoSummaryStr = QString("View consists of comparison columns %1\n").arg(compareStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForCompareIndication, EXIT infoSummaryStr=(%s)\n", 
         infoSummaryStr.ascii() );
#endif
}


void StatsPanel::updateMetadataForTimeLineView( QString intervalStr )
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForTimeLineView, ENTER intervalStr=(%s)\n", 
         intervalStr.ascii() );
  printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoSummaryStr.isEmpty=(%d)\n", 
         infoSummaryStr.isEmpty() );

  printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoString.isEmpty=(%d)\n", 
         infoString.isEmpty() );

  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoSummaryStr=(%s)\n", 
            infoSummaryStr.ascii() );
  }
  if (!infoString.isEmpty()) {
    printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoString=(%s)\n", 
            infoString.ascii() );
  }
#endif

  if (!infoSummaryStr.isEmpty()) {
      QString tempStr = QString("\nView consists of a time segment covering this percentage range %1").arg(intervalStr);
      infoSummaryStr.append(tempStr);
  } else {
      infoSummaryStr = QString("View consists of a time segment covering this percentage range: %1 for\n").arg(intervalStr);
  }

  if (!infoString.isEmpty()) {
      QString tempStr = QString("\nView consists of a time segment covering this percentage range %1").arg(intervalStr);
      infoString.append(tempStr);
  } else {
      infoString = QString("View consists of a time segment covering this percentage range: %1 for\n").arg(intervalStr);
  }
    
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForTimeLineView, EXIT infoSummaryStr=(%s)\n", 
         infoSummaryStr.ascii() );
    
  printf("StatsPanel::updateMetadataForTimeLineView, EXIT infoString=(%s)\n", 
         infoString.ascii() );
#endif
}

void StatsPanel::checkForDashI()
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::checkForDashI, entered, lastCommand=(%s)\n", lastCommand.ascii() );
  printf("StatsPanel::checkForDashI, entered, infoAboutString=(%s)\n", infoAboutString.ascii() );
  printf("StatsPanel::checkForDashI, entered, infoSummaryStr=(%s)\n", infoSummaryStr.ascii() );
#endif

 int dashI_index = lastCommand.find("-I %");
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::checkForDashI dashI_index=%d\n", dashI_index);
#endif

 if( dashI_index != -1 ) {

   int new_index = findNextMajorToken(lastCommand, dashI_index+4, QString("\n"));

   QString intervalStr = lastCommand.mid(dashI_index+5, (new_index-(dashI_index+3)));
   
#ifdef DEBUG_StatsPanel
   printf("SP::checkForDashI -I case, dashI_index=%d, new_index=%d, (new_index-(dashI_index+4))=%d, intervalStr=%s\n",
           dashI_index, new_index, (new_index-(dashI_index+4)), intervalStr.ascii());
#endif

   updateMetadataForTimeLineView(intervalStr);
 }

}

QString StatsPanel::getPartialExperimentInfo2(int exp_id)
{

    QString returnString = QString::null;

#ifdef DEBUG_StatsPanel_info
    printf("getPartialExperimentInfo2, before add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

    aboutString += QString("for threads %1\n").arg(currentThreadsStr);
    infoAboutString += QString("Hosts/Threads %1\n").arg(currentThreadsStr);
    int ranks_present = currentThreadsStr.find("-r", 0, TRUE);
    QString local_ranks_command = QString::null;

    if (ranks_present > 0) {
#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::getPartialExperimentInfo2, PARTIAL EXP. INFO,list_of_pids.size()=%d\n", list_of_pids.size() );
#endif
#if 1

      if( focusedExpID == -1 ) {
        local_ranks_command = QString("list -v ranks -x %1 %2").arg(exp_id).arg(currentThreadsStr);
      } else {
        local_ranks_command = QString("list -v ranks -x %1 %2").arg(focusedExpID).arg(currentThreadsStr);
      }


#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::getPartialExperimentInfo2-attempt to run (%s)\n", local_ranks_command.ascii() );
#endif
      // Check if this command has been cached already, if so the list will be updated
      // If not, call into the CLI and Framework to get the list data required.
      partial_list_of_ranks.clear();
      bool list_is_cached = checkForExistingIntList( local_ranks_command.ascii(), partial_list_of_ranks);

#ifdef DEBUG_StatsPanel_cache
      std::cerr << "StatsPanel::getPartialExperimentInfo2, LIST_V_THREADS CHECK, local_ranks_command=" << local_ranks_command.ascii() 
                << " list_is_cached=" << list_is_cached 
                << " partial_list_of_ranks.size()=" << partial_list_of_ranks.size() << std::endl;
#endif

      if (!list_is_cached) {
        currentThreadsStrENUM = RANK;
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
        partial_list_of_ranks.clear();
        InputLineObject *clip = NULL;
        if( !cli->getIntListValueFromCLI( (char *) local_ranks_command.ascii(),
               &partial_list_of_ranks, clip, FALSE /* mark value for delete */ ) ) {
          printf("Unable to run %s local_ranks_command.\n", local_ranks_command.ascii() );
        }

        //std::cerr << "StatsPanel::getPartialExperimentInfo2, CACHING LIST_V_RANKS, local_ranks_command=" << local_ranks_command.ascii() 
        //          << " partial_list_of_ranks.size()=" << partial_list_of_ranks.size() << std::endl;

        addIntListForThisCommand(local_ranks_command.ascii(), partial_list_of_ranks);
      }

#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::getPartialExperimentInfo2, ran %s, partial_list_of_ranks.size()=%d\n", local_ranks_command.ascii(), partial_list_of_ranks.size() );
#endif
       int rank_cnt = partial_list_of_ranks.size();
#else
       int rank_cnt = 1;
#endif
       if (rank_cnt == 1) {
           // let us look for threads to present to the user in the partial experiment info.  These may be openMP threads
           currentThreadsStrENUM = THREAD;
           QString local_command = QString::null;
           if( focusedExpID == -1 ) {
             local_command = QString("list -v threads -x %1 %2").arg(exp_id).arg(currentThreadsStr);
           } else {
             local_command = QString("list -v threads -x %1 %2").arg(focusedExpID).arg(currentThreadsStr);
           }

// printf("attempt to run (%s)\n", local_command.ascii() );

           // Check if this command has been cached already, if so the list will be updated
           // If not, call into the CLI and Framework to get the list data required.
           partial_list_of_threads.clear();
           bool list_is_cached = checkForExistingIntList( local_command.ascii(), partial_list_of_threads);

#ifdef DEBUG_StatsPanel_cache
           std::cerr << "StatsPanel::getPartialExperimentInfo2, LIST_V_THREADS CHECK, local_command=" << local_command.ascii() << " list_is_cached=" << list_is_cached 
                     << " partial_list_of_threads.size()=" << partial_list_of_threads.size() << std::endl;
#endif

           if (!list_is_cached) {

             CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
             partial_list_of_threads.clear();
             InputLineObject *clip = NULL;

             if( !cli->getIntListValueFromCLI( (char *)local_command.ascii(), &partial_list_of_threads, clip, FALSE /* mark value for delete */ ) ) {
               printf("Unable to run %s command.\n", local_command.ascii() );
             }

             //std::cerr << "StatsPanel::getPartialExperimentInfo2, CACHING LIST_V_THREADS, local_command=" << local_command.ascii() 
             //          << " partial_list_of_threads.size()=" << partial_list_of_threads.size() << std::endl;

             addIntListForThisCommand(local_command.ascii(), partial_list_of_threads);
           }

#ifdef DEBUG_StatsPanel_info
         printf("StatsPanel::getPartialExperimentInfo2, ran %s, partial_list_of_threads.size()=%d\n", 
                local_command.ascii(), partial_list_of_threads.size() );
#endif
           if( partial_list_of_threads.size() > 0 ) {

#ifdef DEBUG_StatsPanel_info
           printf("StatsPanel::getPartialExperimentInfo2, inside if, partial_list_of_threads.size()=%d\n", partial_list_of_threads.size() );
#endif
             int local_thread_count = 0;
             int64_t local_thread = -1;
             for( std::list<int64_t>::const_iterator local_it = partial_list_of_threads.begin();
                  local_it != partial_list_of_threads.end(); local_it++ )
             {
               local_thread = (int64_t)*local_it;

#ifdef DEBUG_StatsPanel
               printf("StatsPanel::getPartialExperimentInfo2, inside for, partial_list_of_threads.size()=%d\n", partial_list_of_threads.size() );
#endif
               local_thread_count = local_thread_count + 1;
               QString local_threadStr = QString("%1").arg(local_thread);
               infoAboutString += QString("Thread: %1\n").arg(local_threadStr);
#ifdef DEBUG_StatsPanel_info
               printf("StatsPanel::getPartialExperimentInfo2, local_thread=%ld, local_thread_count=%d, partial_list_of_threads.size()=%d\n", local_thread, local_thread_count, partial_list_of_threads.size() );
#endif
          
             } // end for list of threads

           } // end if threads.size
       }
    }
    if (!infoAboutString.isEmpty()) {
      returnString = QString("\n%1\n").arg(infoAboutString);
    }

#ifdef DEBUG_StatsPanel_info
    printf("getPartialExperimentInfo2, after add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif
    return returnString;

}

// ----------------------------------------------
// ----------------------------------------------
// StatsPanel::getPartialExperimentInfo()
// ----------------------------------------------
// ----------------------------------------------

QString StatsPanel::getPartialExperimentInfo()
{
  QString returnString = QString::null;

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getPartialExperimentInfo, entered, aboutOutputString=(%s)\n", aboutOutputString.ascii() );
  printf("StatsPanel::getPartialExperimentInfo, entered, lastCommand=(%s)\n", lastCommand.ascii() );
  printf("StatsPanel::getPartialExperimentInfo, entered, infoAboutString=(%s)\n", infoAboutString.ascii() );
#endif

  int cviewinfo_index = lastCommand.find("cview ");

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getPartialExperimentInfo, cview-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif

  if( cviewinfo_index == -1 ) {
     cviewinfo_index = lastCommand.find("expView ");

#ifdef DEBUG_StatsPanel_info
     printf("StatsPanel::getPartialExperimentInfo, expview-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif

     // Need more than just the expview to continue, must be specifications that are only present
     // if a subset of the entire experiment is being displayed.  -h host is a check to make?
     if( cviewinfo_index != -1 ) {
        cviewinfo_index = lastCommand.find("-h");
#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::getPartialExperimentInfo, host-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif
     }
     if( cviewinfo_index == -1 ) {
        cviewinfo_index = lastCommand.find("-r");
#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::getPartialExperimentInfo, rank-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif
     }
  }
  if( cviewinfo_index != -1 ) {

#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::getPartialExperimentInfo, infoAboutComparingString.isEmpty()=(%d)\n", infoAboutComparingString.isEmpty() );
       if (!infoAboutComparingString.isEmpty()) {
          printf("StatsPanel::getPartialExperimentInfo, infoAboutComparingString.ascii()=(%s)\n", infoAboutComparingString.ascii() );
       }
        printf("StatsPanel::getPartialExperimentInfo, infoSummaryStr.isEmpty()=(%d)\n", infoSummaryStr.isEmpty() );
       if (!infoAboutComparingString.isEmpty()) {
          printf("StatsPanel::getPartialExperimentInfo, infoSummaryStr.ascii()=(%s)\n", infoSummaryStr.ascii() );
       }
#endif

    if (!infoAboutString.isEmpty()) {
      returnString = QString("\n%1\n").arg(infoAboutString);
    }

  }
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::getPartialExperimentInfo, return(%s)\n", returnString.ascii() );
#endif
  return (returnString);
}

void StatsPanel::updateStatsPanelInfoHeader(int exp_id)
{
  QString partialExperimentViewInfo;
  bool force_use_of_exp_id = TRUE;


//  if ( isHeaderInfoAlreadyProcessed(exp_id)) {
//
//#ifdef DEBUG_StatsPanel_info
//     printf("StatsPanel::updateStatsPanelInfoHeader, EXIT EARLY, exp_id=(%d) already processed\n", exp_id );
//#endif
//     
//     return; 
//  } else {
//     setHeaderInfoAlreadyProcessed(exp_id);
//  }

  int64_t previous_rank = -1;
  int64_t max_range_rank = -1;
  int64_t min_range_rank = -1;

  int64_t previous_thread = -1;
  int64_t max_range_thread = -1;
  int64_t min_range_thread = -1;

  int64_t previous_pid = -1;
  int64_t max_range_pid = -1;
  int64_t min_range_pid = -1;

  // Initialize the summary string
  infoSummaryStr = QString("");


#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader, entered, ++++++++++ this=0x%lx, lastCommand=(%s)\n", 
         this, lastCommand.ascii() );
#endif

  list_of_hosts.clear();
  list_of_pids.clear();
  list_of_ranks.clear();
  list_of_threads.clear();
  list_of_executables.clear();
  list_of_appcommands.clear();
  list_of_types.clear();

  getExperimentType(exp_id);
  getApplicationCommand(exp_id);
  getExecutableList(exp_id);
#ifdef DBNAMES

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader, calling getDatabaseName, exp_id=%d\n", exp_id );
#endif
  force_use_of_exp_id = TRUE;
  getDatabaseName(exp_id, force_use_of_exp_id);
#endif

  int cviewinfo_aux_index = -1;
  int cviewinfo_index = lastCommand.find("cview ");

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader, cview-check, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
#endif

  if( cviewinfo_index == -1 ) {
     cviewinfo_aux_index = lastCommand.find("expView ");

#ifdef DEBUG_StatsPanel_info
     printf("StatsPanel::updateStatsPanelInfoHeader, expview-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     // Need more than just the expview to continue, must be specifications that are only present
     // if a subset of the entire experiment is being displayed.  -h host is a check to make?
     if( cviewinfo_aux_index != -1 ) {
        cviewinfo_aux_index = lastCommand.find("-h");

#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::updateStatsPanelInfoHeader, host-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     }
     if( cviewinfo_aux_index == -1 ) {
        cviewinfo_aux_index = lastCommand.find("-r");

#ifdef DEBUG_StatsPanel_info
        printf("StatsPanel::updateStatsPanelInfoHeader, rank-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     }
  }
  
  if( cviewinfo_index != -1 || cviewinfo_aux_index != -1 ) {

    partialExperimentViewInfo = getPartialExperimentInfo();
    // Look at printing underlying threads under a paricular rank
    if( !currentThreadsStr.isEmpty() ) {
       getPartialPidList(exp_id);
       partialExperimentViewInfo = getPartialExperimentInfo2(exp_id);
    }
    if (!partialExperimentViewInfo.isEmpty()) {
      partialExperimentViewInfo.insert(0,"\n Partial Experiment View Information:");
    }
    partialExperimentViewInfo += QString("\n  Full Experiment Information:");

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, host-check, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
    printf("StatsPanel::updateStatsPanelInfoHeader, partialExperimentViewInfo.ascii()=(%s)\n", partialExperimentViewInfo.ascii() );
#endif
  }

  getSeparatePidList(exp_id);
  getSeparateRankList(exp_id);
  getSeparateThreadList(exp_id);
  getSeparateRanksAndThreadsList(exp_id);
  getSeparateRanksAndPidsList(exp_id);
  getSeparatePidsAndThreadsList(exp_id);

  getHostList(exp_id);
  getRankThreadPidList(exp_id);
#if SEPARATED_LISTS
  getRankThreadList(exp_id);
  getSeparatePidList(exp_id);
  getSeparateRankList(exp_id);
  getSeparateThreadList(exp_id);
#endif

  infoString += QString("Metadata for Experiment %1:").arg(exp_id);

  // Check for partial time segment
  checkForDashI();

  // Prepend partial results followed by the metadata for the whole experiment

  if( cviewinfo_index != -1 || cviewinfo_aux_index != -1  ) {
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, BEFORE ADDING partialExperimentViewInfo.ascii()=(%s)\n", partialExperimentViewInfo.ascii() );
      printf("StatsPanel::updateStatsPanelInfoHeader, infoString.ascii()=(%s)\n", infoString.ascii() );
#endif
      infoString += partialExperimentViewInfo;
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_appcommands.size()=%d\n", list_of_appcommands.size());
#endif
  if( list_of_appcommands.size() > 0 )
  {
    infoString += QString("\n  Application command: ");
    int appcommands_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_appcommands.begin();
         it != list_of_appcommands.end(); it++ )
    {
      appcommands_count = appcommands_count + 1;
      std::string appcommands = *it;
      QString appcommandsStr = QString("%1").arg(appcommands.c_str());
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, appcommands=(%s)\n", appcommands.c_str() );
#endif
      infoString += QString(" %1 ").arg(appcommandsStr);
//      if (appcommands_count > 6) break;
    }

  }

#ifdef DBNAMES
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_dbnames.size()=%d\n", list_of_dbnames.size());
#endif
  // Add the database name for this particular experiment into the info header    
  force_use_of_exp_id = TRUE;
  getDatabaseName(exp_id, force_use_of_exp_id);
  infoString += QString("\n  Database Name: ");
  QString dbnameStr = getDBName(exp_id);
  infoString += QString(" %1 ").arg(dbnameStr);
#endif

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_executables.size()=%d\n", list_of_executables.size());
#endif
  if( list_of_executables.size() > 0 )
  {
    infoString += QString("\n  Executables: ");
    int executable_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      executable_count = executable_count + 1;
      std::string executable = *it;
      QString executableStr = QString("%1").arg(executable.c_str());
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, executable=(%s)\n", executable.c_str() );
#endif
      infoString += QString(" %1 ").arg(executableStr);
//      if (executable_count > 6) break;
    }

  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_types.size()=%d\n", list_of_types.size());
#endif
  if( list_of_types.size() > 0 )
  {
    infoString += QString("\n  Experiment type: ");
    for( std::list<std::string>::const_iterator it = list_of_types.begin();
         it != list_of_types.end(); it++ )
    {
      std::string types = *it;
      QString typesStr = QString("%1").arg(types.c_str());
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, types=(%s)\n", types.c_str() );
#endif
      infoString += QString(" %1 ").arg(typesStr);
    }

  }


#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_hosts.size()=%d\n", list_of_hosts.size());
#endif
  if( list_of_hosts.size() > 0 )
  {
    infoString += QString("\n  Number of Host(s): ");
    infoString += QString(" %1 ").arg(list_of_hosts.size());
    infoString += QString("\n  Host(s): ");
    int host_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_hosts.begin();
         it != list_of_hosts.end(); it++ )
    {
      host_count = host_count + 1;
      std::string host = *it;
      QString infoHostStr = QString("%1").arg(host.c_str());
#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::updateStatsPanelInfoHeader, host=(%s)\n", host.c_str() );
#endif
      infoString += QString(" %1 ").arg(infoHostStr);
      if (host_count > 17) break;
    }

  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_pids.size()=%d\n", list_of_pids.size());
#endif

#if 0
  if( list_of_pids.size() > 0 )
  {
    infoString += QString("\n  Processes, Ranks or Threads: ");
    bool first_time = true;
    int pid_count = 0;
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin(); it != list_of_pids.end(); it++ )
    {
      pid_count = pid_count + 1;
      int64_t pid = (int64_t)*it;
      QString pidStr = QString("%1").arg(pid);
#ifdef DEBUG_StatsPanel_info_details
      printf("StatsPanel::updateStatsPanelInfoHeader, pid=%ld, pid_count=%d, list_of_pids.size()=%d\n", pid, pid_count, list_of_pids.size() );
#endif

      // Handle first time or only one pid cases
      if( list_of_pids.size() == 1 ) {
          infoString += QString(" %1 ").arg(pidStr);
          break;
      } else if (first_time) {
          max_range_pid = pid;
          min_range_pid = pid;
          previous_pid = pid;
          first_time = false;
#ifdef DEBUG_StatsPanel_info
          printf("StatsPanel::updateStatsPanelInfoHeader, FIRST TIME, min_range_pid=%ld, max_range_pid=%ld\n", 
                 min_range_pid, max_range_pid );
#endif
          continue;
      }

      if (pid > previous_pid ) {

#ifdef DEBUG_StatsPanel_info_details
         printf("StatsPanel::updateStatsPanelInfoHeader, pid>prev, previous_pid=%ld, pid=%ld\n", previous_pid, pid );
#endif

        if (pid == previous_pid + 1  && (pid_count != list_of_pids.size()) ) {

#ifdef DEBUG_StatsPanel_info_details
          printf("StatsPanel::updateStatsPanelInfoHeader, pid==prev+1, before(max_range_pid=%ld), pid=%ld\n", max_range_pid, pid );
#endif
          max_range_pid = pid;

        } else {

#ifdef DEBUG_StatsPanel_info
          printf("StatsPanel::updateStatsPanelInfoHeader, NOT pid==prev+1, max_range_pid=%ld, min_range_pid=%ld\n", max_range_pid, min_range_pid );
#endif
          if (max_range_pid != min_range_pid && (pid_count != list_of_pids.size()) ) {

            QString maxPidStr = QString("%1").arg(max_range_pid);
            infoString += QString(" %1 ").arg(maxPidStr);
#ifdef DEBUG_StatsPanel_info
            printf("StatsPanel::updateStatsPanelInfoHeader, NOT pid==prev+1, max_range_pid=%ld != min_range_pid=%ld\n", max_range_pid, min_range_pid );
            printf("StatsPanel::updateStatsPanelInfoHeader, NOT pid==prev+1, pid_count=%ld != list_of_pids.size()=%ld\n", pid_count, list_of_pids.size() );
#endif

          } else {

            // if in a range creation and you get to the end of the for
            // need to update the last item and output the range
            if  (pid_count == list_of_pids.size()) {
#ifdef DEBUG_StatsPanel_info
               printf("StatsPanel::updateStatsPanelInfoHeader, NOT pid==prev+1, pid_count=%d == list_of_pids.size(), setting max_range_pid=%ld = pid=%ld\n", pid_count, max_range_pid, pid );
#endif
               max_range_pid = pid;
            }

            // You've arrived here because you encountered a value that is
            // not consequitive or we have gotten to the end of the pids 
            // and need to output the range creation prior to leaving the loop.

            QString maxPidStr = QString("%1").arg(max_range_pid);
            QString minPidStr = QString("%1").arg(min_range_pid);
            if (min_range_pid != max_range_pid) {
              if (min_range_pid+1 == max_range_pid || previous_pid + 1 == max_range_pid) {
                infoString += QString(" %1-%2 ").arg(minPidStr).arg(maxPidStr);
              } else {
                // if end of the list put out both values, if not just the min
                if (pid_count == list_of_pids.size()) {
                  infoString += QString(" %1 ").arg(minPidStr);
                  infoString += QString(" %1 ").arg(maxPidStr);
                } else {
                  infoString += QString(" %1 ").arg(minPidStr);
                }
              }
            } else {
              infoString += QString(" %1 ").arg(minPidStr);
            }
#ifdef DEBUG_StatsPanel_info
            printf("StatsPanel::updateStatsPanelInfoHeader, arrived here because you encountered a value that is not consequitive\n" );
            printf("StatsPanel::updateStatsPanelInfoHeader, arrived .... infoString=%s\n", infoString.ascii() );
            printf("StatsPanel::updateStatsPanelInfoHeader, arrived .... pid=%ld, min_range_pid=%ld, max_range_pid=%ld\n", infoString.ascii(), min_range_pid, max_range_pid );
#endif
            min_range_pid = pid;
            max_range_pid = pid;
          } 

        } // end else -> not (pid == previous_pid + 1)



      } else {
#ifdef DEBUG_StatsPanel_info
       printf("ERROR - pids not ascending\n");
       break;
#endif
      } 


#ifdef DEBUG_StatsPanel_info_details
     printf("StatsPanel::updateStatsPanelInfoHeader, SET prev at end of for, previous_pid=%ld, pid=%ld\n", previous_pid, pid );
#endif
     previous_pid = pid;
    } // end for

  }
#else
   bool there_are_multiple_threads_per_rank = false;
   if (separate_list_of_ranksandthreads.size() > 0 && separate_list_of_ranks.size() > 0 &&
       separate_list_of_ranks.size() > separate_list_of_ranksandthreads.size() ) {
       there_are_multiple_threads_per_rank = true;
   }
   if (separate_list_of_ranksandthreads.size() > 0) {
    if (there_are_multiple_threads_per_rank) {
      infoString += QString("\n  Number of Rank:Thread Pairs: ");
      infoString += QString(" %1 ").arg(separate_list_of_ranksandthreads.size());
      infoString += QString("\n  Rank:Thread Pairs: ");
    } else {
      infoString += QString("\n  Number of Ranks: ");
      infoString += QString(" %1 ").arg(separate_list_of_ranks.size());
      infoString += QString("\n  Ranks: ");
    }
    bool first_time = true;
    int rank_count = 0;
    if (separate_list_of_ranksandthreads.size() > 0 && there_are_multiple_threads_per_rank) {
       for( std::list<std::string>::const_iterator it = separate_list_of_ranksandthreads.begin(); it != separate_list_of_ranksandthreads.end(); it++ )
      {
        rank_count = rank_count + 1;
        std::string rank_thread = (std::string)*it;
        infoString += QString(" %1 ").arg(rank_thread);
        if (rank_count > 16) {
           infoString += QString(" ... ");
           break;
        }
      }
    } else {
      // ranks only case
      if (separate_list_of_ranks.size() > 0) {
       for( std::list<int64_t>::const_iterator it = separate_list_of_ranks.begin(); it != separate_list_of_ranks.end(); it++ ) {
        rank_count = rank_count + 1;
        int64_t rank = (int64_t)*it;
        QString local_rankStr = QString("%1").arg(rank);
        infoString += QString(" %1 ").arg(local_rankStr);
        if (rank_count > 16) {
           infoString += QString(" ... ");
           break;
        }
       }
      }
    }
   } else if (separate_list_of_pidsandthreads.size() > 0) {
    infoString += QString("\n  Number of Process:Thread Pairs: ");
    infoString += QString(" %1 ").arg(separate_list_of_pidsandthreads.size());
    infoString += QString("\n  Process:Thread Pairs: ");
    bool first_time = true;
    int pid_count = 0;
    for( std::list<std::string>::const_iterator it = separate_list_of_pidsandthreads.begin(); it != separate_list_of_pidsandthreads.end(); it++ )
    {
      pid_count = pid_count + 1;
      std::string pid_thread = (std::string)*it;
      infoString += QString(" %1 ").arg(pid_thread);
      if (pid_count > 16) {
         infoString += QString(" ... ");
         break;
      }
    }
   }
#endif

// All the key data items have been processed into the infoString now...
// We now attempt to create a summary string that will always be shown
// in the stats panel.  This should contain the number of processes if > 1
// or the process if == 1.  Same with hosts, etc.


  // Process PID/RANK/THREAD information first

 if( list_of_executables.size() > 0 )
  {
    infoSummaryStr += QString("Executables: ");
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      std::string executable = *it;
      QString executableStr = QString("%1").arg(executable.c_str());
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, executable=(%s)\n", executable.c_str() );
#endif
      infoSummaryStr += QString(" %1 ").arg(executableStr);
    }

   }

  int index_host_start = -1;
  int index_host_end = -1;
#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
#endif

  if( cviewinfo_index != -1  || cviewinfo_aux_index != -1 ) {
  // if this is the case we have partial results or a comparison

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, infoString=(%s)\n", infoString.ascii() );
#endif

//    infoSummaryStr += QString("\nNot implemented yet ");
 
   // Look for number of hosts and a specific one to output for the summary string, 
   // find start/end character positions
   QString infoHostStr = QString::null;
   int host_count = infoString.contains("-h", TRUE);
#ifdef DEBUG_StatsPanel_info
   printf("StatsPanel::updateStatsPanelInfoHeader, after infoString.contains call, host_count=(%d)\n", host_count );
#endif
   if (host_count > 0) {  
    index_host_start = infoString.find("-h ");
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, index_host_start=(%d)\n", index_host_start );
#endif
    if (index_host_start != -1) {
     index_host_end = infoString.find("-r");
#ifdef DEBUG_StatsPanel_info
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,index_host_end=(%d)\n", index_host_end );
#endif

     if (index_host_end == -1) {
       index_host_end = infoString.find("-p");
#ifdef DEBUG_StatsPanel_info
       printf("StatsPanel::updateStatsPanelInfoHeader, process,index_host_end=(%d)\n", index_host_end );
#endif
     } 

     if (index_host_end == -1) {
       index_host_end = infoString.find("-t");
#ifdef DEBUG_StatsPanel_info
       printf("StatsPanel::updateStatsPanelInfoHeader, thread,index_host_end=(%d)\n", index_host_end );
#endif
     }

      infoHostStr = infoString.mid((index_host_start+3), (index_host_end-index_host_start-4) );

#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, index_host_start+3=(%d)\n", (index_host_start+3));
      printf("StatsPanel::updateStatsPanelInfoHeader, index_host_end-index_host_start-4=(%d)\n", (index_host_end-index_host_start-4));
      printf("StatsPanel::updateStatsPanelInfoHeader, infoHostStr=(%s)\n", infoHostStr.ascii() );
#endif
    }

    if (host_count == 1) {
      infoSummaryStr += QString("Host: ");
    } else {
      infoSummaryStr += QString("Hosts:(%1) ").arg(host_count);
    }

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, BFOR adding infoHostStr=(%s) to infoSummaryStr=(%s)\n", 
           infoHostStr.ascii(), infoSummaryStr.ascii()  );
#endif

    infoSummaryStr += QString(" %1 ").arg(infoHostStr);

#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, AFTER adding infoHostStr=(%s) to infoSummaryStr=(%s)\n", 
           infoHostStr.ascii(), infoSummaryStr.ascii()  );
#endif

    if (host_count > 1) {
      infoSummaryStr += QString("... ");
    }
  }
 
   // Look for number of pids/ranks/threads and a specific one to output for the summary string, 
   // find start/end character positions
   int index_pid_start = -1;
   int index_pid_end = -1;
   QString pidStr;
   QString searchStr =("-r ");
   int pid_count = infoString.contains("-r", TRUE);
   if (pid_count == 0) {  
      pid_count = infoString.contains("-t", TRUE);
      searchStr =("-t ");
      if (pid_count == 0) {  
        pid_count = infoString.contains("-p", TRUE);
        searchStr =("-p ");
      }
   }
   
#ifdef DEBUG_StatsPanel_info
   printf("StatsPanel::updateStatsPanelInfoHeader, pid_count=(%d)\n", pid_count );
   printf("StatsPanel::updateStatsPanelInfoHeader, searchStr.ascii()=(%s)\n", searchStr.ascii() );
#endif

   if (pid_count > 0) {  
    index_pid_start = infoString.find(searchStr.ascii(), 0, TRUE);
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_start=(%d)\n", index_pid_start );
#endif
    if (index_pid_start != -1) {
     index_pid_end = infoString.find('\n', index_pid_start+3, TRUE);
#ifdef DEBUG_StatsPanel_info
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,EOL search index_pid_end=(%d)\n", index_pid_end );
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,infoString.length()=(%d)\n", infoString.length() );
#endif
     if (index_pid_end == -1) {
       index_pid_end = infoString.find(" ", index_pid_start+3, TRUE);
#ifdef DEBUG_StatsPanel_info
       printf("StatsPanel::updateStatsPanelInfoHeader, rank, BLANKS search, index_pid_end=(%d)\n", index_pid_end );
#endif
     }

     if (index_pid_end != -1) {
      pidStr = infoString.mid((index_pid_start+3), (index_pid_end-index_pid_start-3) );
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_start+3=(%d)\n", (index_pid_start+3));
      printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_end-index_pid_start-3=(%d)\n", (index_pid_end-index_pid_start-3));
      printf("StatsPanel::updateStatsPanelInfoHeader, pidStr=(%s)\n", pidStr.ascii() );
#endif

      if( partial_list_of_pids.size() > 0 ) {
        infoSummaryStr += QString("Pids: (%1) ").arg(partial_list_of_pids.size());
      }

      if( partial_list_of_ranks.size() > 0 ) {
        infoSummaryStr += QString("Ranks: (%1) ").arg(partial_list_of_ranks.size());
      }

      if( partial_list_of_threads.size() > 0 ) {
        infoSummaryStr += QString("Threads: (%1) ").arg(partial_list_of_threads.size());
      }

#if 0
      if (pid_count == 1) {
        infoSummaryStr += QString("Process/Rank/Thread: ");
//        infoSummaryStr += QString("<b>Pid/Rank/Thread:</b> ");
      } else {
        infoSummaryStr += QString("Processes/Ranks/Threads:(%1) ").arg(pid_count);
//        infoSummaryStr += QString("<b>Processes/Ranks/Threads:</b>(%1) ").arg(pid_count);
      }
#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, BFOR adding pidStr=(%s) to infoSummaryStr=(%s)\n", 
             pidStr.ascii(), infoSummaryStr.ascii() );
#endif

      infoSummaryStr += QString(" %1 ").arg(pidStr);

#ifdef DEBUG_StatsPanel_info
      printf("StatsPanel::updateStatsPanelInfoHeader, AFTER adding pidStr=(%s) to infoSummaryStr=(%s)\n", 
             pidStr.ascii(), infoSummaryStr.ascii() );
#endif

      if (pid_count > 1) {
        infoSummaryStr += QString("... ");
      }
#endif

     } // have a valid end
    } // have a valid start
  } // have some pids/ranks/threads

  } else {
   // if this is the case we have full results and should be able to use
   // the list_.... items to create the summary.
   // Process HOST information first

   if (list_of_hosts.size() == 1) {
//    infoSummaryStr += QString("<b>Host:</b> ");
     infoSummaryStr += QString("Host: ");
   } else {
     infoSummaryStr += QString("Hosts:(%1) ").arg(list_of_hosts.size());
//    infoSummaryStr += QString("<b>Hosts:</b>(%1) ").arg(list_of_hosts.size());
   }
   std::list<std::string>::const_iterator host_it = list_of_hosts.begin();
   std::string host = *host_it;
   QString infoHostStr = QString("%1").arg(host.c_str());
#ifdef DEBUG_StatsPanel_info
   printf("StatsPanel::updateStatsPanelInfoHeader, host=(%s)\n", host.c_str() );
#endif
   infoSummaryStr += QString(" %1 ").arg(infoHostStr);

   if (list_of_hosts.size() > 1) {
     infoSummaryStr += QString("... ");
   }

  // Process PID/RANK/THREAD information first

   if (separate_list_of_ranksandpids.size() > 0) {
     infoSummaryStr += QString("Pids: %1").arg(separate_list_of_ranksandpids.size());
   } else if (separate_list_of_pids.size() > 0) {
     infoSummaryStr += QString("Pids: %1").arg(separate_list_of_pids.size());
   }
   if (separate_list_of_ranks.size() > 0) {
     infoSummaryStr += QString(" Ranks: %1").arg(separate_list_of_ranks.size());
   }
   if (separate_list_of_ranksandthreads.size() > 0) {
     infoSummaryStr += QString(" Threads: %1").arg(separate_list_of_ranksandthreads.size());
   } else if (separate_list_of_pidsandthreads.size() > 0) {
     infoSummaryStr += QString(" Threads: %1").arg(separate_list_of_pidsandthreads.size());
   } else {
     infoSummaryStr += QString(" Threads: %1").arg(separate_list_of_threads.size());
   }




#if 0
   if (list_of_pids.size() == 1) {
     infoSummaryStr += QString("Pid/Rank/Thread: ");
//    infoSummaryStr += QString("\n<b>Pid/Rank/Thread:</b> ");
   } else {
     infoSummaryStr += QString("Processes/Ranks/Threads:(%1) ").arg(list_of_pids.size());
//    infoSummaryStr += QString("\n<b>Processes/Ranks/Threads:</b>(%1) ").arg(list_of_hosts.size());
   }

   std::list<int64_t>::const_iterator pid_it = list_of_pids.begin();
   int64_t summary_pid = (int64_t)*pid_it;
   QString pidStr = QString("%1").arg(summary_pid);
   infoSummaryStr += QString(" %1 ").arg(pidStr);

   if (list_of_pids.size() > 1) {
     infoSummaryStr += QString("... ");
   }
#endif

 
 } // cviewinfo_index if/else

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoAboutComparingString.isEmpty()=%d\n", infoAboutComparingString.isEmpty());
  if (!infoAboutComparingString.isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , infoAboutComparingString.ascii()=%s\n", infoAboutComparingString.ascii());
  }
  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , infoSummaryStr.ascii()=%s\n", infoSummaryStr.ascii());
  }
#endif

  if (!infoAboutComparingString.isEmpty() || cviewinfo_index != -1) {
    // put out compare indication in the summary statement
    QString cStr = QString("click on the metadata icon \"I\" for details.");
#ifdef DEBUG_StatsPanel_info
    printf("StatsPanel::updateStatsPanelInfoHeader() , calling updateMetadataForCompareIndication\n");
#endif
    updateMetadataForCompareIndication( cStr );
  }

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoString.ascii()=%s\n", infoString.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoSummaryStr.ascii()=%s\n", infoSummaryStr.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , ADDING TO metaDataTextEdit->text().isEmpty()=%d\n", 
         metaDataTextEdit->text().isEmpty());
  if (!metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , ADDING TO metaDataTextEdit->text().ascii()=(%s)\n",
           metaDataTextEdit->text().ascii());
  }
#endif

  infoEditHeaderLabel->setText(infoSummaryStr);
  infoEditHeaderLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, infoEditHeaderLabel->sizePolicy().hasHeightForWidth() ) );


//#ifdef TEXT
  if (!infoAboutComparingString.isEmpty()) {
    metaDataTextEdit->setText(infoAboutComparingString);
    metaDataTextEdit->append(QString("\n"));
    infoAboutComparingString = QString::null;
  } 
  if (metaDataTextEdit->text().isEmpty()) {
    metaDataTextEdit->setText(infoString);
  } else {
    metaDataTextEdit->append(infoString);
  }
  infoString = QString("");
  metaDataTextEdit->setCursorPosition(0, 0);
  metaDataTextEdit->hide();

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader(), metadataToolButton=0x%lx, setIconSet\n",metadataToolButton);
#endif

#if MORE_BUTTON
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
#else
  metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
#endif
  metaDataTextEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, metaDataTextEdit->sizePolicy().hasHeightForWidth() ) );
  metaDataTextEditFLAG = FALSE;
//#endif

#ifdef DEBUG_StatsPanel_info
  printf("StatsPanel::updateStatsPanelInfoHeader() , exitting +++++++++++++++++++++++ with infoString.ascii()=%s\n",
          infoString.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , exitting metaDataTextEdit->text().isEmpty()=%d\n", 
          metaDataTextEdit->text().isEmpty());

  if (!metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , exitting metaDataTextEdit->text().ascii()=(%s)\n", 
            metaDataTextEdit->text().ascii());
  }
#endif

}

// processing_preference indicates whether the data should
// be forced to be updated or not.  If not, let the other
// parameters/situations determine whether the data should be updated.

void
StatsPanel::updateStatsPanelData(bool processing_preference, QString command)
{

#ifdef DEBUG_StatsPanel
  printf("ENTER StatsPanel::updateStatsPanelData, ENTERING -------------------------\n");
  printf("ENTER StatsPanel::updateStatsPanelData() entered., currentCollectorStr=%s, command=%s\n", 
         currentCollectorStr.ascii(), command.ascii() );
  printf("ENTER StatsPanel::updateStatsPanelData, currentThreadsStr=%s\n", currentThreadsStr.ascii() );
#endif

  levelsToOpen = getPreferenceLevelsToOpen().toInt();
  infoAboutString = QString("");
  infoAboutComparingString = QString("");

  SPListViewItem *splvi;
  columnHeaderList.clear();

  // Reinitialize these flags because of the "hiding of the no data message"
  // The no data message caused the splv (stats panel data) and cf (chart form) to be hidden
#ifdef DEBUG_StatsPanel_chart
  printf("ENTER StatsPanel::updateStatsPanelData, command.isEmpty()= %d\n", command.isEmpty() );
  printf("ENTER StatsPanel::updateStatsPanelData, command=%s, toolBarFLAG=%d\n", command.ascii() , toolBarFLAG);
  printf("ENTER StatsPanel::updateStatsPanelData, chartFLAG=%d\n", chartFLAG);
  printf("ENTER StatsPanel::updateStatsPanelData, currentUserSelectedReportStr=%s\n", currentUserSelectedReportStr.ascii() );
#endif

  // Turn off the chart for these views
  // and Turn back on if the chartFLAG is FALSE but the preference is for the chart to be on
  if ((currentUserSelectedReportStr.startsWith("Butterfly") || 
       currentUserSelectedReportStr.startsWith("CallTrees") || 
       currentCollectorStr.contains("iot") || 
       currentCollectorStr.contains("mem") || 
       currentCollectorStr.contains("mpit") || 
       currentUserSelectedReportStr.startsWith("TraceBacks") ||
       currentUserSelectedReportStr.startsWith("minMaxAverage") ||
       currentUserSelectedReportStr.startsWith("clusterAnalysis") ||
       currentUserSelectedReportStr.startsWith("HotCallPath") ||
       currentUserSelectedReportStr.startsWith("Comparison")) &&
      chartFLAG == TRUE) {
#ifdef DEBUG_StatsPanel_chart
    printf("IN StatsPanel::updateStatsPanelData, calling showChart from turn on checks, chartFLAG=%d\n", chartFLAG);
#endif
    showChart();
  } else if ((currentUserSelectedReportStr.startsWith("Butterfly") || 
       currentUserSelectedReportStr.startsWith("CallTrees") || 
       currentCollectorStr.contains("iot") || 
       currentCollectorStr.contains("mem") || 
       currentCollectorStr.contains("mpit") || 
       currentUserSelectedReportStr.startsWith("TraceBacks") ||
       currentUserSelectedReportStr.startsWith("minMaxAverage") ||
       currentUserSelectedReportStr.startsWith("clusterAnalysis") ||
       currentUserSelectedReportStr.startsWith("HotCallPath") ||
       currentUserSelectedReportStr.startsWith("Comparison")) &&
      chartFLAG == FALSE) {
#ifdef DEBUG_StatsPanel_chart
    printf("IN StatsPanel::updateStatsPanelData, do NOT call showChart from leave off checks, chartFLAG=%d\n", chartFLAG);
#endif
    // skip calling showChart, no need to call it.
  } else if (chartFLAG == FALSE &&  thisPC->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() ) {
    // for other views flip the chartFLAG back on
#ifdef DEBUG_StatsPanel_chart
    printf("IN StatsPanel::updateStatsPanelData, calling showChart from else clause, chartFLAG=%d\n", chartFLAG);
#endif
    showChart();
  }


  splv->show();
  if (chartFLAG) {
    cf->show();
  }
  sml->hide();
//  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );

  // Percent value list (for the chart)
  cpvl.clear();
  // Text value list (for the chart)
  ctvl.clear();
  color_names = NULL;

#ifdef DEBUG_StatsPanel_chart
  printf("updateStatsPanelData, CHART, currentUserSelectedReportStr=%s\n", currentUserSelectedReportStr.ascii() );
  printf("updateStatsPanelData, CHART, calling cf->init()\n" );
#endif

  cf->init();
  total_percent = 0.0;
  numberItemsToDisplayInStats = -1;

  if( !getPreferenceTopNLineEdit().isEmpty() ) {
    bool ok;
    numberItemsToDisplayInStats = getPreferenceTopNLineEdit().toInt(&ok);
  }

  if( !getPreferenceTopNTraceLineEdit().isEmpty() ) {
    bool ok;
    numberTraceItemsToDisplayInStats = getPreferenceTopNTraceLineEdit().toInt(&ok);
  }

  numberItemsToDisplayInChart = 5;

  if( !getPreferenceTopNChartLineEdit().isEmpty() ) {
    bool ok;
    numberItemsToDisplayInChart = getPreferenceTopNChartLineEdit().toInt(&ok);
  }

#ifdef DEBUG_StatsPanel_chart
  printf("updateStatsPanelData, CHART, numberItemsToDisplayInChart = %d\n", numberItemsToDisplayInChart );
#endif

  textENUM = getPreferenceShowTextInChart();

#ifdef DEBUG_StatsPanel_chart
  printf("updateStatsPanelData,textENUM=%d\n", textENUM );
#endif

  lastlvi = NULL;
  gotHeader = FALSE;
  gotColumns = FALSE;
//  fieldCount = 0;
  percentIndex = -1;


#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, lastC, command.isEmpty()= %d\n", command.isEmpty() );
  printf("updateStatsPanelData, lastC, lastCommand= %s\n", lastCommand.ascii() );
  printf("updateStatsPanelData, lastC, lastCommand.startsWith(\"cview -c\")=%d\n", lastCommand.startsWith("cview -c") );
  printf("updateStatsPanelData, lastC, currentCollectorStr=%s\n", currentCollectorStr.ascii() );
  printf("updateStatsPanelData, lastC, currentUserSelectedReportStr=%s\n", currentUserSelectedReportStr.ascii() );
  printf("updateStatsPanelData, lastC, lastUserSelectedReportStr=%s\n", lastUserSelectedReportStr.ascii() );
  printf("updateStatsPanelData, lastC, currentThreadsStr=%s\n", currentThreadsStr.ascii() );
  printf("updateStatsPanelData, lastC, lastCurrentThreadsStr=%s\n", lastCurrentThreadsStr.ascii() );
#endif

  // jeg 9/22/08 and refined 10/13/08
  if (lastCommand.startsWith("cview -c") && 
      command.isEmpty() && 
      lastCurrentThreadsStr == currentThreadsStr  &&
      currentUserSelectedReportStr.startsWith("Comparison") ) {
       command = lastCommand;
  }

#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, command.isEmpty()= %d\n", command.isEmpty() );
  printf("updateStatsPanelData, command=%s, toolBarFLAG=%d\n", command.ascii() , toolBarFLAG);
#endif

  if( command.isEmpty() ) {
#ifdef DEBUG_StatsPanel
    printf("In StatsPanel::updateStatsPanelData, calling generate_command with currentThreadsStr=%s\n", currentThreadsStr.ascii() );
#endif
    command = generateCommand();
  } else {
    aboutString = "Compare/Customize report for:\n  ";

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData, NOT command.isEmpty() aboutString.ascii()=%s\n", aboutString.ascii() );
#endif

  }

  if( command.isEmpty() ) {

#ifdef DEBUG_StatsPanel
    printf("  updateStatsPanelData, EXIT EARLY command is EMPTY\n" );
#endif

    return;
  }

  if( !command.startsWith("cview -c") ) {
    // better clear the compareExpIDs structure
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData, CLEAR compareExpIDs\n"); 
#endif
    compareExpIDs.clear();
  }



#ifdef DEBUG_StatsPanel
  printf("  updateStatsPanelData, this=0x%lx, currentCollectorStr = %s\n", this, currentCollectorStr.ascii() );
  printf("  updateStatsPanelData, lastCommand = %s  command = %s\n", lastCommand.ascii(), command.ascii() );
#endif

  //generateToolBar();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, getPreferenceShowToolbarCheckBox() == TRUE=%d\n",( getPreferenceShowToolbarCheckBox() == TRUE ));
#endif

//  if( toolBarFLAG == TRUE ) {
//    fileTools->show();
//  } else {
//    fileTools->hide();
//  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, about to append timeIntervalString = %s  to command = %s\n", 
          timeIntervalString.ascii(), command.ascii() );
#endif
  // ---------------------------------------------------------
  // ---------------------------------------------------------
  // There is some ordering issues with when the command 
  // was created and when the time segment is (can be) changed.
  // So, to rectify that issue, we look for the previous time
  // segment and replace it with the new time segment range.
  // ---------------------------------------------------------
  // ---------------------------------------------------------
  int timeIndex = -1;
  if (!prevTimeIntervalString.isEmpty() ) {
    timeIndex = command.find(prevTimeIntervalString);
  }
  if (timeIndex == -1) {
    // didn't find previous time interval (-I % nn:mm)
    command += timeIntervalString;
  } else {

    // did find previous time interval (-I % nn:mm)
    // so fix up by replacing with the new interval

    int prevLength = prevTimeIntervalString.length();
    command = command.replace( timeIndex, prevLength, timeIntervalString);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData, replaced prevTimeIntervalString =%s with timeIntervalString = %s  in command = %s\n", 
            prevTimeIntervalString.ascii(), timeIntervalString.ascii(), command.ascii() );
#endif

  }

  if( recycleFLAG == FALSE ) {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData,fire up a new stats panel and send (%s) to it.\n", command.ascii() );
#endif

    // fire up a new stats panel and send "command" to it.
    int exp_id = expID;
    if( expID == -1  ) {
      exp_id = groupID;
    }
     
    // JEG - might be able to do something here for rerun stats panels
    // maybe tag this string with the rerun count?
    // 8/7/2007

    QString name = QString("Stats Panel [%1]").arg(exp_id);
    Panel *sp = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

    if( !sp ) {
      char *panel_type = (char *) "Stats Panel";
      ArgumentObject *ao = new ArgumentObject("ArgumentObject", exp_id);
      sp = getPanelContainer()->dl_create_and_add_panel(panel_type, getPanelContainer(), ao);
      // remember the collector we are dealing with for the new instantiation.
      delete ao;
    } 

    if( sp ) {

      UpdateObject *msg = new UpdateObject((void *)NULL, -1, command.ascii(), 1);
      sp->listener( (void *)msg );

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, calling listener with UPDATEOBJECT,command.ascii()=%s\n", command.ascii() );
#endif

    } // end sp

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData,setting aboutString.ascii()=%s to lastAbout.ascii()=%s\n", 
           aboutString.ascii(), lastAbout.ascii() );
#endif

    aboutString = lastAbout;
    return;
  } // end recycleFLAG


  // processing_preference either forces and update of the data or not, depending
  // on it's value. false == no update, true == force update

  if( staticDataFLAG == TRUE && command == lastCommand && !processing_preference )
  {  // Then we really don't need to update.
#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,We really have static data and its the same command... Don't update.\n");
#endif
    return;
  }

#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,deleting the pd=0x%x\n", pd);
#endif
  if( pd ) {
    delete pd;
  }

#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,deleting the progressTimer=0x%x\n", progressTimer);
#endif
  if( progressTimer ) {
    delete progressTimer;
  }

  steps = 0;
  pd = new GenericProgressDialog(this, "Executing Command:", TRUE );
#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, creating the pd=0x%x\n", pd);
#endif
  pd->infoLabel->setText( QString("Running command") );
  progressTimer = new QTimer( this, "progressTimer" );
#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,create the progressTimer=0x%x\n", progressTimer);
#endif
  connect( progressTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
  pd->show();
//progressUpdate();
#ifdef DEBUG_Sorting
  printf("updateStatsPanelData,sort command?, command.ascii()=%s\n", command.ascii());
  fflush(stdout);
  fflush(stderr);
#endif
  progressTimer->start(0);
  pd->infoLabel->setText( QString("Running command - %1").arg(command) );
  qApp->flushX();
  qApp->processEvents(4000);

#ifdef DEBUG_Sorting
  printf("updateStatsPanelData,sort command?, command.ascii()=%s\n", command.ascii());
#endif

  if( command.contains("-v Butterfly") || 
      command.contains("-v CallTrees") || 
      command.contains("-v TraceBacks") )
  {
    // Don't sort these report types..  If you get a request to sort then only
    // sort on the last column.

#ifdef DEBUG_Sorting
    printf("updateStatsPanelData,butterfly, calltree, or tracebacks, Don't sort this display.\n");
#endif

    splv->setSorting ( -1 );

  } else if( command.startsWith("cview -c") && command.contains("-m ") )
  { // CLUSTER.. Don't sort this one...

#ifdef DEBUG_Sorting
    printf("updateStatsPanelData,cview type - Don't sort this display.\n");
#endif

    // jeg 10/10/11 - allow sorting for cluster analysis views
    // splv->setSorting ( -1 );
    splv->setSorting ( 0, FALSE );
    splv->setShowSortIndicator ( TRUE );

  } else {
    //
    // Set the re-sort to be the first column when a new report is requested.
    //
    if( command.contains("-v trace")) {
#ifdef DEBUG_Sorting
      printf("updateStatsPanelData, do not sort this display on column 0, contains -v trace.\n");
#endif
//JEG      splv->setSorting ( -1 );
      splv->setSorting ( 0, TRUE );
      splv->setShowSortIndicator ( TRUE );

    } else {

#ifdef DEBUG_Sorting
      printf("updateStatsPanelData,Sort this display on column 0.\n");
#endif
      splv->setSorting ( 0, FALSE );
      splv->setShowSortIndicator ( TRUE );
    }

  }

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  insertDiffColumnFLAG = FALSE;

  splv->clear();
  for(int i=splv->columns();i>=0;i--)
  {
    splv->removeColumn(i-1);
  }

  QApplication::setOverrideCursor(QCursor::WaitCursor);

  aboutString += "Command issued: " + command + "\n";

  if( lastCommand.isEmpty() )
  {
#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,lastCommand is empty, The original command = (%s)\n", command.ascii() );
    printf("updateStatsPanelData,lastCommand is empty, The aboutString string = (%s)\n", aboutString.ascii() );
#endif
    originalCommand = command;
  }
  lastCommand = command;
  lastCurrentThreadsStr = currentThreadsStr; 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before if,&statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip,statspanel_clip);
#endif

  bool cached_clip_processing = false;

  if( statspanel_clip ) { 

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,C: statspanel_clip->Set_Results_Used(), clearing statspanel_clip\n");
#endif

    //jeg statspanel_clip->Set_Results_Used();
    statspanel_clip = NULL;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData after set NULL,&statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  }

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel:updateStatsPanelData, about to issue a command via" 
            << " cli->run_Append_Input_String(): command=" <<  command.ascii() << std::endl;
#endif

  //InputLineObject* statspanel_clip = check_for_existing_clip(command.ascii());

  statspanel_clip = check_for_existing_clip(command.ascii());

  if (statspanel_clip) {

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:updateStatsPanelData, FOUND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE, command=" << command << std::endl;
#endif

    // call process_clip here?
    // maybe set some flags to skip the wait loop below
    cached_clip_processing = true;

  } else {

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:updateStatsPanelData, DID NOT FIND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE, ADDING CLIP AFTER ISSUING COMMAND, command=" << command << std::endl;
#endif

     statspanel_clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());

     cached_clip_processing = false;

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:updateStatsPanelData, after issuing command, statspanel_clip=" << statspanel_clip 
               << " sizeof(&statspanel_clip)=" << sizeof(&statspanel_clip) << std::endl;
#endif
     addClipForThisCommand(command.ascii(), statspanel_clip);
  }
  
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData after set with cli->run_App..,command.ascii()=%s,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", command.ascii(), &statspanel_clip, statspanel_clip);
#endif

#ifdef DEBUG_StatsPanel
  if( statspanel_clip == NULL )
     printf("StatsPanel:updateStatsPanelData, (statspanel_clip == NULL)=%d\n", (statspanel_clip == NULL) );
#endif

#if 0
  if( statspanel_clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
    progressTimer->stop();
    delete progressTimer;
    progressTimer = NULL;
    pd->hide();
    delete pd;
    pd = NULL;
    
//    return;
  }
#endif // 0

  Input_Line_Status status = ILO_UNKNOWN;

  if( !command.startsWith("cview -c")  ) {

#ifdef DEBUG_StatsPanel
  printf("SP::updateStatsPanelData not cview, calling updateStatsPanelInfoHeader, expID=%d, lastCommand=%s, infoAboutString=%s\n",
           expID, lastCommand.ascii(), infoAboutString.ascii());
  printf("StatsPanel::updateStatsPanelData(), CLEARING metaDataTextEdit->text().isEmpty()=%d\n", 
          metaDataTextEdit->text().isEmpty());
  if (metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelData(), CLEARING metaDataTextEdit->text().ascii()=(%s)\n", 
           metaDataTextEdit->text().ascii());
  }
#endif

   metaDataTextEdit->setText("");

#ifdef DEBUG_StatsPanel_toolbar
   printf("SP::updateStatsPanelData CALLING UPDATESTATSPANELINFOHEADER, expID=%d, toolBarFLAG=%d\n", expID, toolBarFLAG);
   printf("SP::updateStatsPanelData, calling generateToolBar(), calling GENERATETOOLBAR, this=0x%lx, currentCollectorStr = %s\n", 
          this, currentCollectorStr.ascii() );
#endif

   generateToolBar( command, expID );

   if (expID <= 0) {
     expID = findExperimentID( command);
#ifdef DEBUG_StatsPanel
     printf("SP::updateStatsPanelData after call to findExperimentID, SETTING expID=%d, toolBarFLAG=%d\n", expID, toolBarFLAG);
#endif
   }
   updateStatsPanelInfoHeader(expID);

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData this=0x%lx, toolBarFLAG=%d\n", this, toolBarFLAG);
#endif

   if( toolBarFLAG == TRUE ) {
     fileTools->show();
#ifdef DEBUG_StatsPanel_toolbar
     printf("StatsPanel::updateStatsPanelData, (5095) SHOW,fileTools=0x%lx)\n", fileTools);
#endif
   } else {
     fileTools->hide();
#ifdef DEBUG_StatsPanel_toolbar
     printf("StatsPanel::updateStatsPanelData, (5099) HIDE,fileTools=0x%lx)\n", fileTools);
#endif
   }

 } 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before ref with cli->Seman..,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  if (!cached_clip_processing ) {

    while( !statspanel_clip->Semantics_Complete() ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, pinging... while( !statspanel_clip->Semantics_Complete() ), qApp=0x%x\n", qApp);
#endif

      qApp->flushX();
      qApp->processEvents(4000);
      suspend();
      //sleep(1);
    }
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, done pinging... while( !statspanel_clip->Semantics_Complete() ), pd=0x%x\n", pd);
#endif

  pd->infoLabel->setText( tr("Processing information for display") );
  pd->show();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before call to process_clip..,&statspanel_clip=0x%x, statspanel_clip=0x%x, TotalTime=%f\n",
           &statspanel_clip, statspanel_clip, TotalTime);
#endif

  TotalTime = 0.0;
  maxEntryBasedOnTotalTime = 0;
  process_clip(statspanel_clip, NULL, FALSE);

//  process_clip(statspanel_clip, NULL, TRUE);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData after call to process_clip..,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  //jegstatspanel_clip->Set_Results_Used();
  statspanel_clip = NULL;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, Done processing the clip\n");
#endif

  pd->infoLabel->setText( tr("Analyze and sort the view.") );
  pd->show();

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData about to call analyzeTheCView, cview case, expID=%d, before clearing:infoAboutStringCompareExpIDs=%s, infoAboutString=%s\n",
           expID, infoAboutStringCompareExpIDs.ascii(), infoAboutString.ascii());
#endif
  infoAboutStringCompareExpIDs = QString::null;

#ifdef DEBUG_StatsPanel_cview
   printf("SP::updateStatsPanelData about to call analyzeTheCView, cview case, expID=%d\n", expID);
#endif

  analyzeTheCView();

  if( command.startsWith("cview -c")  ) {

//   recycleFLAG = FALSE; // let us not have to regenerate this.  keep the comparison views

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData call updateStatsPanelInfoHeader, cview case, expID=%d, lastCommand=%s, infoAboutString=%s\n",
           expID, lastCommand.ascii(), infoAboutString.ascii());
#endif

   int start_expID_index = 0;
   int expIDindex = 0;

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData BEFORE WHILE cview case, expIDindex=%d, start_expID_index=%d, infoAboutStringCompareExpIDs.length()=%d\n",
           expIDindex, start_expID_index, infoAboutStringCompareExpIDs.length());
   printf("SP::updateStatsPanelData BEFORE WHILE cview case, infoAboutStringCompareExpIDs.ascii()=%s\n",
           infoAboutStringCompareExpIDs.ascii());
   printf("SP::updateStatsPanelData() BEFORE WHILE cview case,  CLEARING metaDataTextEdit->text().isEmpty()=%d\n", metaDataTextEdit->text().isEmpty());
   if (!metaDataTextEdit->text().isEmpty()) {
     printf("SP::updateStatsPanelData() BEFORE WHILE cview case,  CLEARING metaDataTextEdit->text().ascii()=(%s)\n", metaDataTextEdit->text().ascii());
   }
#endif

   metaDataTextEdit->setText("");

   // Let the metadata for the compares be processed once only
   // If this isn't re-initialized here, then the updateStatsPanelInfoHeader would
   // return early, because the data was processed once already upon first presentation.
   setHeaderInfoAlreadyProcessed(-1);

   compareExpIDs.clear();
   compareExpDBNames.clear();
   int compareID_count = 0;
   bool alreadyGeneratedToolBar = FALSE;

   while (expIDindex != (infoAboutStringCompareExpIDs.length()-1) ) {

      expIDindex = infoAboutStringCompareExpIDs.find(",", start_expID_index);
      QString expIdStr = infoAboutStringCompareExpIDs.mid(start_expID_index, expIDindex-start_expID_index);

#ifdef DEBUG_StatsPanel
      printf("SP::updateStatsPanelData TOP OF WHILE cview case, start_expID_index=%d, (expIDindex-start_expID_index)=%d, expIDindex=%d, expIdStr=%s\n",
             start_expID_index, (expIDindex-start_expID_index), expIDindex, expIdStr.ascii());
#endif

     int compareExpID = expIdStr.toInt();
     if (compareExpID > 0)  {


          // See if the experiment ID is already in the list.  This happens with pthreaded/openMP applications
          //
          bool skip_this_one = false;
          for( std::vector<int>::const_iterator it = compareExpIDs.begin(); it != compareExpIDs.end(); it++ ) {
              int tmp_exp_id = *it;
              if (tmp_exp_id == compareExpID) {
                 skip_this_one = true;
              }
          }
       
          if (!skip_this_one) {
            compareID_count = compareID_count + 1;
            compareExpIDs.push_back (compareExpID);
            // Save the database name to pass to the choose experiment dialog panel
            QString tmpDBName = getDBName(compareExpID);
            compareExpDBNames.push_back (tmpDBName);
          }
          toolbar_status_label->setText("Showing Comparison Report...");
#ifdef DEBUG_StatsPanel
          printf("SP::updateStatsPanelData setting currentUserSelectedReportStr=%s to %s\n", currentUserSelectedReportStr.ascii(), "Comparison");
#endif
          currentUserSelectedReportStr = "Comparison";

#ifdef DEBUG_StatsPanel
         printf("SP::updateStatsPanelData compareExpID=%d, checking isHeaderInfoAlreadyProcessed(compareExpID)=%d\n", 
                compareExpID, isHeaderInfoAlreadyProcessed(compareExpID));
#endif


         if ( !isHeaderInfoAlreadyProcessed(compareExpID)) {
#ifdef DEBUG_StatsPanel
          printf("SP::updateStatsPanelData CALLING UPDATESTATSPANELINFOHEADER, compareExpID=%d, metadataToolButton=%d\n", 
                 compareExpID, metadataToolButton);
          printf("SP::updateStatsPanelData CALLING UPDATESTATSPANELINFOHEADER, command.ascii()=%s\n", command.ascii());
#endif
           updateStatsPanelInfoHeader(compareExpID);

         }

         // keep from repeating Metadata for the same experiment
         setHeaderInfoAlreadyProcessed(compareExpID);

     } // end compareExpID > 0

#if 1
#ifdef DEBUG_StatsPanel_toolbar
   printf("SP::updateStatsPanelData CVIEW2, calling generateToolBar, expID=%d, toolBarFLAG=%d\n", expID, toolBarFLAG);
   printf("SP::updateStatsPanelData, CVIEW2, calling generateToolBar, command=%s, this=0x%lx, currentCollectorStr = %s\n", 
          command.ascii(), this, currentCollectorStr.ascii() );
   printf("SP::updateStatsPanelData CVIEW2, calling generateToolBar, alreadyGeneratedToolBar=%d, expIDindex=%d, compareExpID=%d\n", alreadyGeneratedToolBar, expIDindex, compareExpID);
#endif

     // Only generate the toolbar once
     if (!alreadyGeneratedToolBar) {
       generateToolBar(command, expID);
       alreadyGeneratedToolBar = TRUE;
     }
#endif

     start_expID_index = expIDindex + 1;

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData BOTTOM OF WHILE cview case, compareExpID=%d, start_expID_index=%d, expIDindex=%d\n",
           compareExpID, start_expID_index, expIDindex);
#endif

   } // end while though cview experiment ids

 }

 // Let new view displays get their metadata
 setHeaderInfoAlreadyProcessed(-1);
 

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelData command.startsWith(cview -c)=%d, canWeDiff()=%d\n", 
          command.startsWith("cview -c"), canWeDiff());
#endif

  if( command.startsWith("cview -c") && canWeDiff() )
  {
    int insertColumn = 0;
    insertDiffColumn(insertColumn);
    insertDiffColumnFLAG = TRUE;
  
#ifdef DEBUG_Sorting
   printf("StatsPanel::updateStatsPanelData command.startsWith(cview -c), canWeDiff() is true, insertColumn=%d\n", insertColumn);
#endif
    // Force a re-sort in this case...
    splv->setSorting ( insertColumn, FALSE );
    splv->setShowSortIndicator ( TRUE );
    splv->sort();
  }

// Put out the chart if there is one...
   color_names = hotToCold_color_names;
   if( descending_sort != true ) {
    color_names = coldToHot_color_names;
   }

   if( textENUM == TEXT_NONE ) {
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelData, CHART textENUM=%d (TEXT_NONE)\n", textENUM );
#endif

     ctvl.clear();
   }

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelData, CHART Put out the chart!!!!\n");
   printf("StatsPanel::updateStatsPanelData, CHART numberItemsToDisplayInStats=(%d) cpvl.count()=(%d)\n", numberItemsToDisplayInStats, cpvl.count() );
#endif

  // Do we need an other?
#ifdef DEBUG_StatsPanel
printf("StatsPanel::updateStatsPanelData, CHART: total_percent=%f splv->childCount()=%d cpvl.count()=%d numberItemsToDisplayInStats=%d\n", total_percent, splv->childCount(), cpvl.count(), numberItemsToDisplayInStats );

printf("StatsPanel::updateStatsPanelData, CHART: cpvl.count()=%d numberItemsToDisplayInChart = %d\n", cpvl.count(), numberItemsToDisplayInChart );
#endif

/*
  if( ( total_percent > 0.0 &&
      cpvl.count() < numberItemsToDisplayInStats) ||
      ( total_percent > 0.0 && 
        cpvl.count() < numberItemsToDisplayInStats &&
        numberItemsToDisplayInChart < numberItemsToDisplayInStats) )
*/

  {
    if( total_percent < 100.00 ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, CHART, add other of %f\n", 100.00-total_percent );
#endif
      cpvl.push_back( (int)(100.00-total_percent) );
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, CHART, total_percent: textENUM=%d\n", textENUM );
#endif
      if( textENUM != TEXT_NONE ) {
        ctvl.push_back( "other" );
      }
    }
  }

  // Or were there no percents in the initial query.
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, total_percent = %f\n", total_percent );
#endif

  if( total_percent == 0.0 ) {
    cpvl.clear();
    ctvl.clear();
    cpvl.push_back(100);
    ctvl.push_back("N/A");
#ifdef DEBUG_StatsPanel_chart
    printf("StatsPanel::updateStatsPanelData, total_percent=0.0, chartFLAG=%d\n", chartFLAG);
#endif
    // jeg added 10-08-07
    if( thisPC->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() && chartFLAG) {
       showChart();
//       cf->hide();
//       chartFLAG = FALSE;
    }
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, cpvl.count()=%d ctvl.count()=%d\n", cpvl.count(), ctvl.count() );
#endif

  cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, now clean up the timer, progressTimer=0x%x\n", progressTimer);
  printf("StatsPanel::updateStatsPanelData, CHART, now clean up the timer, pd=0x%x\n", pd);
  if (progressTimer) {
    printf("StatsPanel::updateStatsPanelData, CHART, now clean up the timer, calling stop, progressTimer=0x%x\n", progressTimer);
    progressTimer->stop();
  }

  if (pd) {
    printf("StatsPanel::updateStatsPanelData, CHART, now clean up the timer, calling hide, pd=0x%x\n", pd);
    pd->hide();
  }
#endif

  if (progressTimer) {
    progressTimer->stop();
    delete progressTimer;
    progressTimer = NULL;
  }
  if (pd) {
    pd->hide();
    delete pd;
    pd = NULL;
  }

  QApplication::restoreOverrideCursor();
  if (qApp) qApp->flushX();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, EXITING toolBarFLAG=%d\n", toolBarFLAG);
#endif

}

void
StatsPanel::updateToolBarStatus(QString optionChosen)
{ 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateToolBarStatus, optionChosen.ascii()=%s\n", optionChosen.ascii());
#endif

 if (optionChosen.contains("Functions") ) {
  toolbar_status_label->setText("Showing Functions Report:");
 } else if (optionChosen.contains("LinkedObjects") ) {
  toolbar_status_label->setText("Showing Linked Objects Report:");
 } else if (optionChosen.contains("Statements by Function") ) {
  toolbar_status_label->setText("Showing Statements by Function Report:");
 } else if (optionChosen.contains("Statements") ) {
  toolbar_status_label->setText("Showing Statements Report:");
#if defined(HAVE_DYNINST)
 } else if (optionChosen.contains("Loops") ) {
  toolbar_status_label->setText("Showing Loops Report:");
#endif
 } else if (optionChosen.contains("CallTrees by Function") ) {
  toolbar_status_label->setText("Showing CallTrees by Function Report:");
 } else if (optionChosen.contains("CallTrees,FullStack Report") ) {
   toolbar_status_label->setText("Showing CallTrees,FullStack Report:");
 } else if (optionChosen.contains("CallTrees,FullStack by Function") ) {
   toolbar_status_label->setText("Showing CallTrees,FullStack by Function Report:");
 } else if (optionChosen.contains("CallTrees") ) {
  toolbar_status_label->setText("Showing CallTrees Report:");
 } else if (optionChosen.contains("TraceBacks by Function") ) {
   toolbar_status_label->setText("Showing TraceBacks by Function Report:");
 } else if (optionChosen.contains("TraceBacks,FullStack") ) {
   toolbar_status_label->setText("Showing TraceBacks,FullStack Report:");
 } else if (optionChosen.contains("TraceBacks,FullStack by Function") ) {
   toolbar_status_label->setText("Showing TraceBacks,FullStack by Function Report:");
 } else if (optionChosen.contains("TraceBacks") ) {
   toolbar_status_label->setText("Showing TraceBacks Report:");
 } else if (optionChosen.contains("Butterfly") ) {
   toolbar_status_label->setText("Showing Butterfly Report:");
 } else if (optionChosen.contains("Load Balance") ) {
   toolbar_status_label->setText("Showing Load Balance (min,max,ave) Report:");
 } else if (optionChosen.contains("Compare and Analyze") ) {
   toolbar_status_label->setText("Showing Comparative Analysis Report:");
 }

}

void
StatsPanel::threadSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("ENTER ---------------------- StatsPanel::threadSelected(%d)\n", val);
#endif

  currentMenuThreadsStr = threadMenu->text(val).ascii();

  currentThreadsStr = QString::null;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, threadMenu: selected text=(%s)\n", threadMenu->text(val).ascii() );
#endif


  bool FOUND_FLAG = FALSE;
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); 
                                      it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;

    if( ts == currentMenuThreadsStr )
    {   // Then it's already in the list... now remove it.
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::threadSelected, add the selected thread (%s).\n", ts.ascii() );
#endif
      currentThreadGroupStrList.remove(ts);
      FOUND_FLAG = TRUE;
      break;
    }
  }

  // We didn't find it to remove it, so this is a different thread... add it.
  if( FOUND_FLAG == FALSE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::threadSelected, We must need to add it (%s) then!\n", currentMenuThreadsStr.ascii() );
#endif
    currentThreadGroupStrList.push_back(currentMenuThreadsStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, Here's the string list...\n");
#endif
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::threadSelected, A: ts=(%s)\n", ts.ascii() );
#endif
  
    if( ts.isEmpty() ) {
      continue;
    }


if( currentThreadsStrENUM == RANK ) {
    if( currentThreadsStr.isEmpty() ) {
      currentThreadsStr = "-r "+ts;
    } else {
      currentThreadsStr += ","+ts;
    }
} else if( currentThreadsStrENUM == THREAD ) {
    if( currentThreadsStr.isEmpty() ) {
      currentThreadsStr = "-t "+ts;
    } else {
      currentThreadsStr += ","+ts;
    }
} else if( currentThreadsStrENUM == PID ) {
    if( currentThreadsStr.isEmpty() ) {
      currentThreadsStr = "-p "+ts;
    } else {
      currentThreadsStr += ","+ts;
    }
}
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, currentThreadsStr = %s call updateStatsPanelData.\n", currentThreadsStr.ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  // Now, try to focus the source panel on the first entry...
  QListViewItemIterator it( splv );
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::threadSelected, try to focus the source panel on the first entry, it.current=0x%x\n", it.current() );
#endif
  if( it.current() ) {
    int i = 0;
    QListViewItem *item = *it;
    StatsPanel::itemSelected(item);
  }
}

void
StatsPanel::collectorMetricSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::collectorMetricSelected val=%d\n", val);
#endif
// printf("collectorMetricSelected: currentCollectorStr=(%s)\n", popupMenu->text(val).ascii() );

  currentUserSelectedReportStr = QString::null;

  QString s = popupMenu->text(val).ascii();

// printf("A: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
//      collectorStrFromMenu = s.mid(13, index-13 );
      currentCollectorStr = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::collectorMetricSelected, BB1: s=(%s) currentCollectorStr=(%s) currentMetricStr=(%s)\n", s.ascii(), currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorMetricSelected, Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}

void
StatsPanel::collectorMPIReportSelected(int val)
{
  currentCollectorStr = "mpi";
  MPIReportSelected(val);
}

void
StatsPanel::collectorMPITReportSelected(int val)
{
  currentCollectorStr = "mpit";
  MPIReportSelected(val);
}

void
StatsPanel::MPIReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::MPIReportSelected val=%d\n", val);
  printf("StatsPanel::MPIReportSelected: mpi_menu=(%s)\n", mpi_menu->text(val).ascii() );
  printf("StatsPanel::MPIReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

  QString s = QString::null;
  s = mpi_menu->text(val).ascii();
  if( s.isEmpty() ) {
    s = contextMenu->text(val).ascii();
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::MPIReportSelected:B: s=%s\n", s.ascii() );
#endif

  int index = s.find(":");
  if( index != -1 ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::MPIReportSelected:D: NOW FIND ::\n");
#endif
    index = s.find(":");
    if( index > 0 ) { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::MPIReportSelected, MPI1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...
    } else { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") ) {
        selectedFunctionStr = QString::null;
      }
    }

    // The status for the tool bar needs to reflect what is 
    // going on when the same features are selected via the menu

    if( getPreferenceShowToolbarCheckBox() == TRUE ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::collectorMPIReportSelected, calling updateToolBarStatus(), currentUserSelectedReportStr = (%s)\n", 
             currentUserSelectedReportStr.ascii() );
#endif

      updateToolBarStatus( currentUserSelectedReportStr );
    } 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::MPIReportSelected, currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("StatsPanel::MPIReportSelected, Collector changed call updateStatsPanelData() \n");
#endif

  }

  updateStatsPanelData(DONT_FORCE_UPDATE);
}

void
StatsPanel::collectorIOReportSelected(int val)
{ 
  currentCollectorStr = "io";
  IOReportSelected(val);
}

void
StatsPanel::collectorIOTReportSelected(int val)
{ 
  currentCollectorStr = "iot";
  IOReportSelected(val);
}

void
StatsPanel::IOReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("IOReportSelected val=%d\n", val);
  printf("IOReportSelected: io_menu=(%s)\n", io_menu->text(val).ascii() );
  printf("IOReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = io_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("C: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("IO1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorIOReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorIOReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::collectorMEMReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorMEMReportSelected: val=(%d)\n", val);
  printf("collectorMEMReportSelected: mem_menu=(%s)\n", mem_menu->text(val).ascii() );
  printf("collectorMEMReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = mem_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("C: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("IO1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorMEMReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorMEMReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::collectorPTHREADSReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorPTHREADSReportSelected: val=(%d)\n", val);
  printf("collectorPTHREADSReportSelected: pthreads_menu=(%s)\n", pthreads_menu->text(val).ascii() );
  printf("collectorPTHREADSReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = pthreads_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("C: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("IO1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorPTHREADSReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorPTHREADSReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}

void
StatsPanel::collectorHWCReportSelected(int val)
{ 
  currentCollectorStr = "hwc";
  HWCReportSelected(val);
}


void
StatsPanel::collectorHWCSampReportSelected(int val)
{ 
  currentCollectorStr = "hwcsamp";
  HWCSampReportSelected(val);
}

void
StatsPanel::collectorHWCTimeReportSelected(int val)
{ 
  currentCollectorStr = "hwctime";
  HWCTimeReportSelected(val);
}


void
StatsPanel::HWCReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("HWCReportSelected: collectorMetricSelected val=%d\n", val);
  printf("HWCReportSelected: hwc_menu=(%s)\n", hwc_menu->text(val).ascii() );
  printf("HWCReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = hwc_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("D: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("DD1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
// printf("A: NULLING OUT selectedFunctionStr\n");
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorHWCReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorHWCReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::HWCSampReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("HWCSampReportSelected: collectorMetricSelected val=%d\n", val);
  printf("HWCSampReportSelected: hwcsamp_menu=(%s)\n", hwcsamp_menu->text(val).ascii() );
  printf("HWCSampReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = hwcsamp_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("D: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("DD1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
// printf("B: NULLING OUT selectedFunctionStr\n");
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorHWCSampReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorHWCSampReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::HWCTimeReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("HWCTimeReportSelected: collectorMetricSelected val=%d\n", val);
  printf("HWCTimeReportSelected: hwctime_menu=(%s)\n", hwctime_menu->text(val).ascii() );
  printf("HWCTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = hwctime_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("D: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("DD1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
// printf("B: NULLING OUT selectedFunctionStr\n");
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorHWCTimeReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorHWCTimeReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}

void
StatsPanel::collectorOMPTPReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("collectorOMPTPReportSelected: val=%d\n", val);
   printf("collectorOMPTPReportSelected: omptp_menu=(%s)\n", omptp_menu->text(val).ascii() );
   printf("collectorOMPTPReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "omptp";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = omptp_menu->text(val).ascii();
  if( s.isEmpty() ) {
    s = contextMenu->text(val).ascii();
  }

// printf("OMPTPReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 ) { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") ) {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorOMPTPReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorOMPTPReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);

}


void
StatsPanel::collectorUserTimeReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("collectorUserTimeReportSelected: val=%d\n", val);
   printf("collectorUserTimeReportSelected: usertime_menu=(%s)\n", usertime_menu->text(val).ascii() );
   printf("collectorUserTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "usertime";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = usertime_menu->text(val).ascii();
  if( s.isEmpty() ) {
    s = contextMenu->text(val).ascii();
  }

// printf("UserTimeReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 ) { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") ) {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorUsertimeReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorUsertimeReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);

}

void
StatsPanel::collectorIOPReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("collectorIOPReportSelected: val=%d\n", val);
   printf("collectorIOPReportSelected: iop_menu=(%s)\n", iop_menu->text(val).ascii() );
   printf("collectorIOPReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "iop";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = iop_menu->text(val).ascii();
  if( s.isEmpty() ) {
    s = contextMenu->text(val).ascii();
  }

// printf("IOPReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 ) { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") ) {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorIOPReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorIOPReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);

}


void
StatsPanel::collectorMPIPReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("collectorMPIPReportSelected: val=%d\n", val);
   printf("collectorMPIPReportSelected: mpip_menu=(%s)\n", mpip_menu->text(val).ascii() );
   printf("collectorMPIPReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "mpip";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = mpip_menu->text(val).ascii();
  if( s.isEmpty() ) {
    s = contextMenu->text(val).ascii();
  }

// printf("MPIPReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 ) { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") ) {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorMPIPReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorMPIPReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);

}

void
StatsPanel::collectorPCSampReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: val=%d\n", val);
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: pcsamp_menu=(%s)\n", pcsamp_menu->text(val).ascii() );
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "pcsamp";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = pcsamp_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

#ifdef DEBUG_StatsPanel
 printf("PCSampReport: (%s)\n", s.ascii() );
#endif

  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics

      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
      printf("PCSampReport: currentCollectorStr=(%s) currentMetricStr=(%s)\n", 
             currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...

    } else { // The user wants to do all the metrics on the selected threads...

      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
#ifdef DEBUG_StatsPanel
      printf("PCSampReport: selectedFunctionStr=(%s),currentUserSelectedReportStr.ascii()=(%s), currentCollectorStr=(%s) currentMetricStr=(%s)\n", 
             selectedFunctionStr.ascii(), currentUserSelectedReportStr.ascii(), 
             currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      if( !currentUserSelectedReportStr.contains("Statements by Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }
  }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorPCSampReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorPCSampReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::collectorPCSampReportSelected, currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("StatsPanel::collectorPCSampReportSelected, Collector changed call updateStatsPanelData() \n");
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

}


void
StatsPanel::collectorFPEReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorFPEReportSelected: val=%d\n", val);
  printf("collectorFPEReportSelected: fpe_menu=(%s)\n", fpe_menu->text(val).ascii() );
  printf("collectorFPEReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "fpe";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = fpe_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("FPE Report: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorFPEReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorFPEReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);


}


void
StatsPanel::collectorGenericReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorGenericReportSelected: val=%d\n", val);
  printf("collectorGenericReportSelected: generic_menu=(%s)\n", generic_menu->text(val).ascii() );
  printf("collectorGenericReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;

  currentMetricStr = QString::null;
  currentCollectorStr = QString::null;
  selectedFunctionStr = QString::null;
#ifdef DEBUG_StatsPanel
  printf("C: NULLING OUT selectedFunctionStr\n");
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  updateStatsPanelData(DONT_FORCE_UPDATE);
}


void
StatsPanel::modifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::modifierSelected val=%d\n", val);
  printf("StatsPanel::modifierSelected: (%s)\n", modifierMenu->text(val).ascii() );
#endif

  if( modifierMenu->text(val).isEmpty() )
  {
    return;
  }


  std::string s = modifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::modifierSelected: B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_modifiers.begin();
       it != current_list_of_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_modifiers.remove(modifier);
      modifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_modifiers.push_back(s);
    }
    modifierMenu->setItemChecked(val, TRUE);
  }

// Uncomment this line if the modifier selection to take place immediately.
// I used to do this, but it seemed wrong to make the use wait as they 
// selected each modifier.   Now, they select the modifier, then go out and
// reselect the Query...
//  updateStatsPanelData();
#ifdef DEBUG_StatsPanel
  printf("exit modifierSelected \n");
#endif

}


void
StatsPanel::mpiModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
 printf("mpiModifierSelected val=%d\n", val);
 printf("mpiModifierSelected: (%s)\n", mpiModifierMenu->text(val).ascii() );
 printf("mpiModifierSelected: (%d)\n", mpiModifierMenu->text(val).toInt() );
#endif


  if( mpiModifierMenu->text(val).isEmpty() )
  {
// printf("Do you want to add the \"duplicate\" submenus?\n");
    mpiModifierMenu->insertSeparator();
    if( mpi_menu )
    {
      delete mpi_menu;
    }
    mpi_menu = new QPopupMenu(this);
    mpiModifierMenu->insertItem(QString("Select mpi Reports:"), mpi_menu);
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMPIReportSelected(int)) );
    return;
  }


  std::string s = mpiModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mpi_modifiers.begin();
       it != current_list_of_mpi_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The %s modifier was in the list ... take it out!\n", s.c_str() );
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
// printf("The %s modifier was in the list ... remove it!\n", s.c_str() );
      current_list_of_mpi_modifiers.remove(modifier);
      mpiModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The %s modifier was not in the list ... add it!\n", s.c_str() );
    if( s != PTI )
    {
      current_list_of_mpi_modifiers.push_back(s);
    }
    mpiModifierMenu->setItemChecked(val, TRUE);
  }
#ifdef DEBUG_StatsPanel
  printf("exit mpiModifierSelected \n");
#endif
}


void
StatsPanel::mpitModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("mpitModifierSelected val=%d\n", val);
   printf("mpitModifierSelected: (%s)\n", mpitModifierMenu->text(val).ascii() );
#endif


  if( mpitModifierMenu->text(val).isEmpty() )
  {
    mpitModifierMenu->insertSeparator();
    if( mpi_menu )
    {
      delete mpi_menu;
    }
    mpi_menu = new QPopupMenu(this);
    mpitModifierMenu->insertItem(QString("Select mpit Reports:"), mpi_menu);
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMPITReportSelected(int)) );
    return;
  }


  std::string s = mpitModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mpit_modifiers.begin();
       it != current_list_of_mpit_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_mpit_modifiers.remove(modifier);
      mpitModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_mpit_modifiers.push_back(s);
    }
    mpitModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::ioModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("ioModifierSelected val=%d\n", val);
  printf("ioModifierSelected: (%s)\n", ioModifierMenu->text(val).ascii() );
#endif

  if( ioModifierMenu->text(val).isEmpty() )
  {
    ioModifierMenu->insertSeparator();
    if( io_menu )
    {
      delete io_menu;
    }
    io_menu = new QPopupMenu(this);
    ioModifierMenu->insertItem(QString("Select io Reports:"), io_menu);
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorIOReportSelected(int)) );
    return;
  }


  std::string s = ioModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_io_modifiers.begin();
       it != current_list_of_io_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_io_modifiers.remove(modifier);
      ioModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE ) {

// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI ) {
      current_list_of_io_modifiers.push_back(s);
    }
    ioModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::memModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("memModifierSelected val=%d\n", val);
  printf("memModifierSelected: (%s)\n", memModifierMenu->text(val).ascii() );
#endif

  if( memModifierMenu->text(val).isEmpty() )
  {
    memModifierMenu->insertSeparator();
    if( mem_menu )
    {
      delete mem_menu;
    }
    mem_menu = new QPopupMenu(this);
    memModifierMenu->insertItem(QString("Select mem Reports:"), mem_menu);
    addMEMReports(mem_menu);
    connect(mem_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMEMReportSelected(int)) );
    return;
  }


  std::string s = memModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mem_modifiers.begin();
       it != current_list_of_mem_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_mem_modifiers.remove(modifier);
      memModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE ) {

// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI ) {
      current_list_of_mem_modifiers.push_back(s);
    }
    memModifierMenu->setItemChecked(val, TRUE);
  }
}



void
StatsPanel::pthreadsModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("pthreadsModifierSelected val=%d\n", val);
  printf("pthreadsModifierSelected: (%s)\n", pthreadsModifierMenu->text(val).ascii() );
#endif

  if( pthreadsModifierMenu->text(val).isEmpty() )
  {
    pthreadsModifierMenu->insertSeparator();
    if( pthreads_menu )
    {
      delete pthreads_menu;
    }
    pthreads_menu = new QPopupMenu(this);
    pthreadsModifierMenu->insertItem(QString("Select pthreads Reports:"), pthreads_menu);
    addPTHREADSReports(pthreads_menu);
    connect(pthreads_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorPTHREADSReportSelected(int)) );
    return;
  }


  std::string s = pthreadsModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_pthreads_modifiers.begin();
       it != current_list_of_pthreads_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_pthreads_modifiers.remove(modifier);
      pthreadsModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE ) {

// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI ) {
      current_list_of_pthreads_modifiers.push_back(s);
    }
    pthreadsModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::iotModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("iotModifierSelected val=%d\n", val);
  printf("iotModifierSelected: (%s)\n", iotModifierMenu->text(val).ascii() );
#endif


  if( iotModifierMenu->text(val).isEmpty() ) {
    iotModifierMenu->insertSeparator();
    if( io_menu ) {
      delete io_menu;
    }
    io_menu = new QPopupMenu(this);
    iotModifierMenu->insertItem(QString("Select iot Reports:"), io_menu);
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorIOTReportSelected(int)) );
    return;
  }

  std::string s = iotModifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
   printf("iot,B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_iot_modifiers.begin();
       it != current_list_of_iot_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

#ifdef DEBUG_StatsPanel
      printf("Looping through the modifier list , modifier.c_str()=%s\n", modifier.c_str() );
#endif

    if( modifier ==  s )
    {   // It's in the list, so take it out...
#ifdef DEBUG_StatsPanel
      printf("The modifier was in the list ... take it out! (%s)\n", s.c_str());
#endif
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE ) {
      current_list_of_iot_modifiers.remove(modifier);
      iotModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE ) {
#ifdef DEBUG_StatsPanel
    printf("The modifier was not in the list ... add it!, (%s)\n", s.c_str());
    printf("iot,B1: modifierStr=(%s)\n", s.c_str() );
#endif
    if( s != PTI )
    {
      current_list_of_iot_modifiers.push_back(s);
    }
    iotModifierMenu->setItemChecked(val, TRUE);
  }

#ifdef DEBUG_StatsPanel
  printf("EXIT iot, (%s)\n", s.c_str());
  for( std::list<std::string>::const_iterator iit = current_list_of_iot_modifiers.begin();
       iit != current_list_of_iot_modifiers.end();  )
  {
    std::string x_modifier = (std::string)*iit;
    printf("EXITing iot, one of the modifiers that is in the list is:(%s)\n", x_modifier.c_str());
    iit++;
  }
#endif
}


void
StatsPanel::hwcModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("hwcModifierSelected val=%d\n", val);
  printf("hwcModifierSelected: (%s)\n", hwcModifierMenu->text(val).ascii() );
#endif

  if( hwcModifierMenu->text(val).isEmpty() )
  {
    hwcModifierMenu->insertSeparator();
    if( hwc_menu )
    {
      delete hwc_menu;
    }
    hwc_menu = new QPopupMenu(this);
    hwcModifierMenu->insertItem(QString("Select hwc Reports:"), hwc_menu);
    addHWCReports(hwc_menu);
    connect(hwc_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorHWCReportSelected(int)) );
    return;
  }


  std::string s = hwcModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_hwc_modifiers.begin();
       it != current_list_of_hwc_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_hwc_modifiers.remove(modifier);
      hwcModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_hwc_modifiers.push_back(s);
    }
    hwcModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::hwcsampModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("hwcsampModifierSelected val=%d\n", val);
  printf("hwcsampModifierSelected: (%s)\n", hwcsampModifierMenu->text(val).ascii() );
#endif

  if( hwcsampModifierMenu->text(val).isEmpty() )
  {
    hwcsampModifierMenu->insertSeparator();
    if( hwcsamp_menu )
    {
      delete hwcsamp_menu;
    }
    hwcsamp_menu = new QPopupMenu(this);
    hwcsampModifierMenu->insertItem(QString("Select hwcsamp Reports:"), hwcsamp_menu);
    addHWCSampReports(hwcsamp_menu);
    connect(hwcsamp_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorHWCSampReportSelected(int)) );
    return;
  }


  std::string s = hwcsampModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_hwcsamp_modifiers.begin(); it != current_list_of_hwcsamp_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_hwcsamp_modifiers.remove(modifier);
      hwcsampModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_hwcsamp_modifiers.push_back(s);
    }
    hwcsampModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::hwctimeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("hwctimeModifierSelected val=%d\n", val);
  printf("hwctimeModifierSelected: (%s)\n", hwctimeModifierMenu->text(val).ascii() );
#endif

  if( hwctimeModifierMenu->text(val).isEmpty() )
  {
    hwctimeModifierMenu->insertSeparator();
    if( hwctime_menu )
    {
      delete hwctime_menu;
    }
    hwctime_menu = new QPopupMenu(this);
    hwctimeModifierMenu->insertItem(QString("Select hwctime Reports:"), hwctime_menu);
    addHWCTimeReports(hwctime_menu);
    connect(hwctime_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorHWCTimeReportSelected(int)) );
    return;
  }


  std::string s = hwctimeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_hwctime_modifiers.begin();
       it != current_list_of_hwctime_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_hwctime_modifiers.remove(modifier);
      hwctimeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_hwctime_modifiers.push_back(s);
    }
    hwctimeModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::usertimeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("usertimeModifierSelected val=%d\n", val);
  printf("usertimeModifierSelected: (%s)\n", usertimeModifierMenu->text(val).ascii() );
#endif

  if( usertimeModifierMenu->text(val).isEmpty() )
  {
    usertimeModifierMenu->insertSeparator();
    if( usertime_menu )
    {
      delete usertime_menu;
    }
    usertime_menu = new QPopupMenu(this);
    usertimeModifierMenu->insertItem(QString("Select usertime Reports:"), usertime_menu);
    addUserTimeReports(usertime_menu);
    connect(usertime_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorUserTimeReportSelected(int)) );
    return;
  }


  std::string s = usertimeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_usertime_modifiers.begin();
       it != current_list_of_usertime_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_usertime_modifiers.remove(modifier);
      usertimeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_usertime_modifiers.push_back(s);
    }
    usertimeModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::omptpModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("omptpModifierSelected val=%d\n", val);
  printf("omptpModifierSelected: (%s)\n", omptpModifierMenu->text(val).ascii() );
#endif

  if( omptpModifierMenu->text(val).isEmpty() )
  {
    omptpModifierMenu->insertSeparator();
    if( omptp_menu )
    {
      delete omptp_menu;
    }
    omptp_menu = new QPopupMenu(this);
    omptpModifierMenu->insertItem(QString("Select omptp Reports:"), omptp_menu);
    addOMPTPReports(omptp_menu);
    connect(omptp_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorOMPTPReportSelected(int)) );
    return;
  }


  std::string s = omptpModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_omptp_modifiers.begin();
       it != current_list_of_omptp_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_omptp_modifiers.remove(modifier);
      omptpModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_omptp_modifiers.push_back(s);
    }
    omptpModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::iopModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("iopModifierSelected val=%d\n", val);
  printf("iopModifierSelected: (%s)\n", iopModifierMenu->text(val).ascii() );
#endif

  if( iopModifierMenu->text(val).isEmpty() )
  {
    iopModifierMenu->insertSeparator();
    if( iop_menu )
    {
      delete iop_menu;
    }
    iop_menu = new QPopupMenu(this);
    iopModifierMenu->insertItem(QString("Select iop Reports:"), iop_menu);
    addIOPReports(iop_menu);
    connect(iop_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorIOPReportSelected(int)) );
    return;
  }


  std::string s = iopModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_iop_modifiers.begin();
       it != current_list_of_iop_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_iop_modifiers.remove(modifier);
      iopModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_iop_modifiers.push_back(s);
    }
    iopModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::mpipModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("mpipModifierSelected val=%d\n", val);
  printf("mpipModifierSelected: (%s)\n", mpipModifierMenu->text(val).ascii() );
#endif

  if( mpipModifierMenu->text(val).isEmpty() )
  {
    mpipModifierMenu->insertSeparator();
    if( mpip_menu )
    {
      delete mpip_menu;
    }
    mpip_menu = new QPopupMenu(this);
    mpipModifierMenu->insertItem(QString("Select mpip Reports:"), mpip_menu);
    addMPIPReports(mpip_menu);
    connect(mpip_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMPIPReportSelected(int)) );
    return;
  }


  std::string s = mpipModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mpip_modifiers.begin();
       it != current_list_of_mpip_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_mpip_modifiers.remove(modifier);
      mpipModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_mpip_modifiers.push_back(s);
    }
    mpipModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::pcsampModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::pcsampModifierSelected val=%d\n", val);
  printf("StatsPanel::pcsampModifierSelected: (%s)\n", pcsampModifierMenu->text(val).ascii() );
#endif

  if( pcsampModifierMenu->text(val).isEmpty() )
  {
    if( pcsampModifierMenu->text(val).isEmpty() )
    {
      pcsampModifierMenu->insertSeparator();
      if( pcsamp_menu )
      {
        delete pcsamp_menu;
      }
      pcsamp_menu = new QPopupMenu(this);
      pcsampModifierMenu->insertItem(QString("Select pcsamp Reports:"), pcsamp_menu);
      addPCSampReports(pcsamp_menu);
      connect(pcsamp_menu, SIGNAL( activated(int) ),
        this, SLOT(collectorPCSampReportSelected(int)) );
      return;
    }

    return;
  }


  std::string s = pcsampModifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_pcsamp_modifiers.begin();
       it != current_list_of_pcsamp_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::The modifier was in the list ... take it out!\n");
#endif
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_pcsamp_modifiers.remove(modifier);
      pcsampModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::The modifier was not in the list ... add it!\n");
#endif
    if( s != PTI )
    {
      current_list_of_pcsamp_modifiers.push_back(s);
    }
    pcsampModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::fpeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::fpeModifierSelected val=%d\n", val);
  printf("StatsPanel::fpeModifierSelected: (%s)\n", fpeModifierMenu->text(val).ascii() );
#endif

  if( fpeModifierMenu->text(val).isEmpty() )
  {
    fpeModifierMenu->insertSeparator();
    if( fpe_menu )
    {
      delete fpe_menu;
    }
    fpe_menu = new QPopupMenu(this);
    fpeModifierMenu->insertItem(QString("Select fpe Reports:"), fpe_menu);
    addFPEReports(fpe_menu);
    connect(fpe_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorFPEReportSelected(int)) );
    return;
  }


  std::string s = fpeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_fpe_modifiers.begin();
       it != current_list_of_fpe_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_fpe_modifiers.remove(modifier);
      fpeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_fpe_modifiers.push_back(s);
    }
    fpeModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::genericModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::genericModifierSelected val=%d\n", val);
  printf("StatsPanel::genericModifierSelected: (%s)\n", genericModifierMenu->text(val).ascii() );
#endif

  if( genericModifierMenu->text(val).isEmpty() )
  {
    return;
  }


  std::string s = genericModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_generic_modifiers.begin();
       it != current_list_of_generic_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_generic_modifiers.remove(modifier);
      genericModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_generic_modifiers.push_back(s);
    }
    genericModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::raisePreferencePanel()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::raisePreferencePanel() \n");
#endif
  getPanelContainer()->getMainWindow()->filePreferences( statsPanelStackPage, QString(pluginInfo->panel_type) );
}

int
StatsPanel::getLineColor(double value)
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor(%f) double, descending_sort= %d TotalTime=%f, maxEntryBasedOnTotalTime=%f\n", value, descending_sort, TotalTime, maxEntryBasedOnTotalTime);
  printf("StatsPanel::getLineColor double, (maxEntryBasedOnTotalTime*.90)=%f, (maxEntryBasedOnTotalTime*.80)=%f, (maxEntryBasedOnTotalTime*.70)=%f\n", 
         (maxEntryBasedOnTotalTime*.90), (maxEntryBasedOnTotalTime*.80), (maxEntryBasedOnTotalTime*.70));
  printf("StatsPanel::getLineColor double, (maxEntryBasedOnTotalTime*.60)=%f, (maxEntryBasedOnTotalTime*.50)=%f, (maxEntryBasedOnTotalTime*.40)=%f\n", 
         (maxEntryBasedOnTotalTime*.60), (maxEntryBasedOnTotalTime*.50), (maxEntryBasedOnTotalTime*.40));
  printf("StatsPanel::getLineColor double, (maxEntryBasedOnTotalTime*.30)=%f, (maxEntryBasedOnTotalTime*.20)=%f, (maxEntryBasedOnTotalTime*.10)=%f\n", 
         (maxEntryBasedOnTotalTime*.30), (maxEntryBasedOnTotalTime*.20), (maxEntryBasedOnTotalTime*.10));
#endif

  if( (double) value >  0.0 ) {
    if( maxEntryBasedOnTotalTime*.90 <= value ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getLineColor(%f) in (maxEntryBasedOnTotalTime*.90=)%f, case block, returning 0==red color\n", value, (maxEntryBasedOnTotalTime*.90));
#endif
      return(0);
    } else if( maxEntryBasedOnTotalTime*.80 <= value ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getLineColor(%f) in (maxEntryBasedOnTotalTime*.80=)%f, case block, returning 1==xxx color\n", value, (maxEntryBasedOnTotalTime*.80));
#endif
      return(1);
    } else if( maxEntryBasedOnTotalTime*.70 <= value ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getLineColor(%f) in (maxEntryBasedOnTotalTime*.70=)%f, case block, returning 2==xxx color\n", value, (maxEntryBasedOnTotalTime*.70));
#endif
      return(2);
    } else if( maxEntryBasedOnTotalTime*.60 <= value ) {
      return(3);
    } else if( maxEntryBasedOnTotalTime*.50 <= value ) {
      return(4);
    } else if( maxEntryBasedOnTotalTime*.40 <= value ) {
      return(5);
    } else if( maxEntryBasedOnTotalTime*.30 <= value ) {
      return(6);
    } else if( maxEntryBasedOnTotalTime*.20 <= value ) {
      return(7);
    } else if( maxEntryBasedOnTotalTime*.10 <= value ) {
      return(8);
    } else if( maxEntryBasedOnTotalTime*0 <= value ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getLineColor(%f) in (maxEntryBasedOnTotalTime*0=)%f, case block, returning 10==xxx color\n", value, (maxEntryBasedOnTotalTime*0));
#endif
      return(9);
    } else {
      return(10);
    }
  }
  return(10);
}

int
StatsPanel::getLineColor(unsigned int value)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor, unsigned, (%u)\n", value);
  printf("StatsPanel::getLineColor(%f) unsigned, TotalTime=%f, maxEntryBasedOnTotalTime=%f\n", value, TotalTime, maxEntryBasedOnTotalTime);
#endif


  if( (int) value >  0.0 ) {
    if( maxEntryBasedOnTotalTime*.90 <= value ) {
      return(0);
    } else if( maxEntryBasedOnTotalTime*.80 <= value ) {
      return(1);
    } else if( maxEntryBasedOnTotalTime*.70 <= value ) {
      return(2);
    } else if( maxEntryBasedOnTotalTime*.60 <= value ) {
      return(3);
    } else if( maxEntryBasedOnTotalTime*.50 <= value ) {
      return(4);
    } else if( maxEntryBasedOnTotalTime*.40 <= value ) {
      return(5);
    } else if( maxEntryBasedOnTotalTime*.30 <= value ) {
      return(6);
    } else if( maxEntryBasedOnTotalTime*.20 <= value ) {
      return(7);
    } else if( maxEntryBasedOnTotalTime*.10 <= value ) {
      return(8);
    } else if( maxEntryBasedOnTotalTime*0 <= value ) {
      return(9);
    } else {
      return(10);
    }
  }
  return(10);

}


int
StatsPanel::getLineColor(uint64_t value)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor(%lld), unint64_t, TotalTime=%lf\n", value, TotalTime);
  printf("StatsPanel::getLineColor(%lld), unint64_t, (.90 * TotalTime)=%lf\n", value, (.90 *TotalTime));
  printf("StatsPanel::getLineColor(%lld) unint64_t,, TotalTime=%f, maxEntryBasedOnTotalTime=%f\n", value, TotalTime, maxEntryBasedOnTotalTime);
#endif

  if( (uint64_t) value >  0.0 ) {
    if( maxEntryBasedOnTotalTime*.90 <= value ) {
      return(0);
    } else if( maxEntryBasedOnTotalTime*.80 <= value ) {
      return(1);
    } else if( maxEntryBasedOnTotalTime*.70 <= value ) {
      return(2);
    } else if( maxEntryBasedOnTotalTime*.60 <= value ) {
      return(3);
    } else if( maxEntryBasedOnTotalTime*.50 <= value ) {
      return(4);
    } else if( maxEntryBasedOnTotalTime*.40 <= value ) {
      return(5);
    } else if( maxEntryBasedOnTotalTime*.30 <= value ) {
      return(6);
    } else if( maxEntryBasedOnTotalTime*.20 <= value ) {
      return(7);
    } else if( maxEntryBasedOnTotalTime*.10 <= value ) {
      return(8);
    } else if( maxEntryBasedOnTotalTime*0 <= value ) {
      return(9);
    } else {
      return(10);
    }
  }

  return(10);
}


std::list<std::string>
StatsPanel::findCollectors( int experimentID )
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::findCollectors() entered, experimentID=%d\n", experimentID);
#endif

  std::list<std::string> local_list_of_collectors;

  if( experimentGroupList.count() > 0 && focusedExpID > 0 ) {

    local_list_of_collectors.clear();
    QString command = QString("list -v expTypes -x %1").arg(focusedExpID);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::findCollectors-A: attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &local_list_of_collectors, clip, TRUE ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

  } else {

    // Now get the collectors... and their metrics...
    QString command = QString::null;

    if( focusedExpID == -1 ) {
      command = QString("list -v expTypes -x %1").arg(experimentID);
    } else {
      command = QString("list -v expTypes -x %1").arg(focusedExpID);
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::findCollectors-B: attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    local_list_of_collectors.clear();
    InputLineObject *clip = NULL;

    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &local_list_of_collectors, clip, TRUE ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::findCollectors-ran %s\n", command.ascii() );
    for( std::list<std::string>::const_iterator it = local_list_of_collectors.begin();
      it != local_list_of_collectors.end(); it++ ) {
      std::string collector_name = (std::string)*it;
      printf("StatsPanel::findCollectors-DEBUG:A: collector_name = (%s)\n", collector_name.c_str() );
    }
#endif // DEBUG_StatsPanel

  }
  return local_list_of_collectors;
}
void
StatsPanel::updateCollectorList()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorList() entered, experimentGroupList.count()=%d\n", experimentGroupList.count() );
#endif

  if( experimentGroupList.count() > 0 ) {
    list_of_collectors.clear();
    QString command = QString("list -v expTypes -x %1").arg(focusedExpID);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-A: attempt to run (%s)\n", command.ascii() );
#endif
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_collectors, clip, TRUE ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }
  } else {

    // Now get the collectors... and their metrics...
    QString command = QString::null;

    if( focusedExpID == -1 ) {
      command = QString("list -v expTypes -x %1").arg(expID);
    } else {
      command = QString("list -v expTypes -x %1").arg(focusedExpID);
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-B: attempt to run (%s)\n", command.ascii() );
#endif

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_collectors.clear();
    InputLineObject *clip = NULL;

    if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_collectors, clip, TRUE ) ) {
      printf("Unable to run %s command.\n", command.ascii() );
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-ran %s\n", command.ascii() );
    for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ ) {
      std::string collector_name = (std::string)*it;
      printf("StatsPanel::updateCollectorList-DEBUG:A: collector_name = (%s)\n", collector_name.c_str() );
    }
#endif // DEBUG_StatsPanel

  }
}


void
StatsPanel::updateCollectorMetricList()
{
  // Now get the collectors... and their metrics...
  QString command = QString::null;

  if( focusedExpID == -1 ) {
    command = QString("list -v metrics -x %1").arg(expID);
  } else {
    command = QString("list -v metrics -x %1").arg(focusedExpID);
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorMetricList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_collectors_metrics.clear();
  list_of_generic_modifiers.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), &list_of_collectors_metrics, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorMetricList-ran %s\n", command.ascii() );
#endif

  if( list_of_collectors_metrics.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors_metrics.begin();
         it != list_of_collectors_metrics.end(); it++ )
    {
      std::string collector_name = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateCollectorMetricList-collector_name/metric name=(%s)\n", collector_name.c_str() );
#endif
      if( currentCollectorStr.isEmpty() )
      {
        QString s = QString(collector_name.c_str());
        int index = s.find("::");
        currentCollectorStr = s.mid(0, index );
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::updateCollectorMetricList-Default the current collector to (%s)\n", collector_name.c_str());
#endif
      }
list_of_generic_modifiers.push_back(collector_name);
    }
  }
}

void
StatsPanel::updateCollectorParamsValList()
{
  // Now get the collectors... and their metrics...
  QString command = QString::null;

  if( focusedExpID == -1 ) {
    command = QString("list -v justparamvalues -x %1").arg(expID);
  } else {
    command = QString("list -v justparamvalues -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorParamsValList-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_collectors_paramsval.clear();
  list_of_generic_modifiers.clear();
  InputLineObject *clip = NULL;
  std::string cstring;

  //if( !cli->getStringValueFromCLI( (char *)command.ascii(), &list_of_collectors_paramsval, clip, TRUE ) )
  if( !cli->getStringValueFromCLI( (char *)command.ascii(), &cstring, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorParamsValList-ran %s, result=%s\n", command.ascii(), cstring.c_str() );
#endif

#if JIM
  if( list_of_collectors_paramsval.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors_paramsval.begin();
         it != list_of_collectors_paramsval.end(); it++ )
    {

      std::string collector_name = (std::string)*it;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateCollectorParamsValList-collector_name/paramsval name=(%s)\n", collector_name.c_str() );
#endif

      if( currentCollectorStr.isEmpty() )
      {
        QString s = QString(collector_name.c_str());
        int index = s.find("::");
        currentCollectorStr = s.mid(0, index );

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::updateCollectorParamsValList-Default the current collector to (%s)\n", collector_name.c_str());
#endif

      }

      list_of_generic_modifiers.push_back(collector_name);
    }
  }
#endif
}


void
StatsPanel::updateThreadsList()
{
// Now get the threads.
  QString command = QString::null;
  InputLineObject *clip = NULL;

  currentThreadsStrENUM = UNKNOWN;
  if( focusedExpID == -1 ) {
    command = QString("list -v ranks -x %1").arg(expID);
  } else {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }
  currentThreadsStrENUM = RANK;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList-attempt to run (%s)\n", command.ascii() );
#endif

  // Check if this command has been cached already, if so the list will be updated
  // If not, call into the CLI and Framework to get the list data required.
  list_of_pids.clear();
  bool list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
  std::cerr << "StatsPanel::updateThreadsList, LIST_V_RANKS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
            << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

  if (!list_is_cached) {

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_pids.clear();
    clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    //std::cerr << "StatsPanel::updateThreadsList, CACHING LIST_V_RANKS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;

    addIntListForThisCommand(command.ascii(), list_of_pids);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif

  if( list_of_pids.size() == 0 ) {
    currentThreadsStrENUM = THREAD;
    if( focusedExpID == -1 ) {
      command = QString("list -v threads -x %1").arg(expID);
    } else {
      command = QString("list -v threads -x %1").arg(focusedExpID);
    }

// printf("attempt to run (%s)\n", command.ascii() );

    // Check if this command has been cached already, if so the list will be updated
    // If not, call into the CLI and Framework to get the list data required.
    list_of_pids.clear();
    bool list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
    std::cerr << "StatsPanel::updateThreadsList, LIST_V_THREADS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
              << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

    if (!list_is_cached) {

      CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
      list_of_pids.clear();
      clip = NULL;

      if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
        printf("Unable to run %s command.\n", command.ascii() );
      }

      //std::cerr << "StatsPanel::updateThreadsList, CACHING LIST_V_THREADS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;

      addIntListForThisCommand(command.ascii(), list_of_pids);
    }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif

  } 

  if( list_of_pids.size() == 0 ) {
    currentThreadsStrENUM = PID;
    if( focusedExpID == -1 ) {
      command = QString("list -v pids -x %1").arg(expID);
    } else {
      command = QString("list -v pids -x %1").arg(focusedExpID);
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateThreadsList, attempt to run (%s)\n", command.ascii() );
#endif

    // Check if this command has been cached already, if so the list will be updated
    // If not, call into the CLI and Framework to get the list data required.
    list_of_pids.clear();
    bool list_is_cached = checkForExistingIntList( command.ascii(), list_of_pids);

#ifdef DEBUG_StatsPanel_cache
    std::cerr << "StatsPanel::updateThreadsList, LIST_V_PIDS CHECK, command=" << command.ascii() << " list_is_cached=" << list_is_cached 
              << " list_of_pids.size()=" << list_of_pids.size() << std::endl;
#endif

    if (!list_is_cached) {

      CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
      list_of_pids.clear();
      clip = NULL;
      if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &list_of_pids, clip, FALSE /* mark value for delete */ ) ) {
        printf("Unable to run %s command.\n", command.ascii() );
      }

      //std::cerr << "StatsPanel::updateThreadsList, CACHING LIST_V_PIDS, command=" << command.ascii() << " list_of_pids.size()=" << list_of_pids.size() << std::endl;

      addIntListForThisCommand(command.ascii(), list_of_pids);
    }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  } 

  if( list_of_pids.size() > 1 )
  {
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int64_t pid = (int64_t)*it;
#ifdef DEBUG_StatsPanel_details
      printf("StatsPanel::updateThreadsList, pid=(%ld)\n", pid );
#endif
    }
  }

  if( clip )
  {
    clip->Set_Results_Used();
  }
}

void
StatsPanel::setCurrentCollector()
{
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *fw_experiment = eo->FW();
      CollectorGroup cgrp = fw_experiment->getCollectors();
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::setCurrentCollector, Is says you have %d collectors.\n", cgrp.size() );
#endif
      if( cgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known collectors for this experiment.\n");
        return;
      }
      for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
      {
        Collector collector = *ci;
        Metadata cm = collector.getMetadata();
        QString name = QString(cm.getUniqueId().c_str());

// printf("Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
        if( currentCollectorStr.isEmpty() )
        {
          currentCollectorStr = name;
// printf("Assigning currentCollectorStr=%s\n", currentCollectorStr.ascii() );
        }
        if( name == currentCollectorStr )
        {
          if( currentCollector )
          {
// printf("delete the currentCollector\n");
            delete currentCollector;
          }
          currentCollector = new Collector(*ci);
// printf("Set a currentCollector!\n");
        }
      }
    }
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return;
  }
// printf("The currentCollector has been set. currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
}

void
StatsPanel::setCurrentThread()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::setCurrentThread() entered\n");
#endif
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *fw_experiment = eo->FW();
      // Evaluate the collector's time metric for all functions in the thread
      ThreadGroup tgrp = fw_experiment->getThreads();
// printf("tgrp.size() = (%d)\n", tgrp.size() );
      if( tgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known threads for this experiment.\n");
        return;
      }
      ThreadGroup::iterator ti = tgrp.begin();
      Thread t1 = *ti;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
      {
        Thread t = *ti;
        std::string host = t.getHost();
        pid_t pid = t.getProcessId();
        QString pidstr = QString("%1").arg(pid);
        if( currentMenuThreadsStr.isEmpty() )
        {
          currentMenuThreadsStr = pidstr;

#ifdef DEBUG_StatsPanel
          printf("Seting a currentMenuThreadsStr!, currentMenuThreadsStr=%s\n", pidstr.ascii());
#endif

          // set a default thread as well...
          t1 = *ti;
          if( currentThread )
          {
            delete currentThread;
          }
          currentThread = new Thread(*ti);
// printf("A: Set a currentThread\n");
        }
        if( pidstr == currentMenuThreadsStr )
        {
// printf("Using %s\n", currentMenuThreadsStr.ascii() );
          t1 = *ti;
          if( currentThread )
          {
            delete currentThread;
          }
          currentThread = new Thread(*ti);
// printf("B: Set a currentThread\n");
          break;
        }
      }
    }
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return;
  }


}

void
StatsPanel::setCurrentMetricStr()
{
// printf("StatsPanel::setCurrentMetricStr() entered\n");

  if( !currentMetricStr.isEmpty() )
  {
// printf("StatsPanel::setCurrentMetricStr() current metric = %s\n", currentMetricStr.ascii() );
    return;
  }


  // The cli (by default) focuses on the last metric.   We should to 
  // otherwise, when trying to focus on the related source panel, we 
  // don't get the correct statistics showing up.
  for( std::list<std::string>::const_iterator it = list_of_collectors_metrics.begin();
       it != list_of_collectors_metrics.end(); it++ )
  {
       std::string collector_name = (std::string)*it;
       QString s = QString(collector_name.c_str() );
// printf("collector_name=(%s)\n", collector_name.c_str() );

        
//      if( currentMetricStr.isEmpty() ) // See comment regarding which metric
                                         // to focus on by default. (above)
      {
// printf("Can you toggle this (currentCollector) menu?\n");
        int index = s.find("::");
// printf("index=%d\n", index );
        currentMetricStr = s.mid(index+2);
// printf("A: currentCollectorStr=(%s) currentMetricStr=(%s) currentMenuThreadsStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii(), currentMenuThreadsStr.ascii() );
      }
  }
// printf("metric=currentMetricStr=(%s)\n", currentMetricStr.ascii() );
}

void
StatsPanel::outputCLIAnnotation(QString xxxfuncName, QString xxxfileName, int xxxlineNumber)
{

#ifdef DEBUG_StatsPanel
   printf("ENTER StatsPanel::outputCLIAnnotation, xxxfuncName.ascii()=%s, xxxfileName.ascii()=%s\n", 
           xxxfuncName.ascii(), xxxfileName.ascii());
#endif

  SPListViewItem *highlight_item = NULL;
  bool highlight_line = FALSE;
  QColor highlight_color = QColor(Qt::blue);

  QString strippedString1 = QString::null; // MPI only.

  for( FieldList::Iterator it = columnFieldList.begin();
       it != columnFieldList.end();
       ++it)
  {
    QString s = ((QString)*it).stripWhiteSpace();

#ifdef DEBUG_StatsPanel
    printf("outputCLIAnnotation, PUT OUT ANNOTATION\n");
#endif

    QMessageBox::information( (QWidget *)this, tr("Info:"), tr(s), QMessageBox::Ok );
  }
  
#ifdef DEBUG_StatsPanel
   printf("outputCLIAnnotation, return from outputCLIAnnotation\n");
#endif

  return;
}

void
StatsPanel::outputCLIData(QString xxxfuncName, QString xxxfileName, int xxxlineNumber)
{


#ifdef DEBUG_StatsPanel
   printf("ENTER StatsPanel::outputCLIData, xxxfuncName.ascii()=%s, xxxfileName.ascii()=%s\n", 
           xxxfuncName.ascii(), xxxfileName.ascii());
#endif

  int i = 0;

  SPListViewItem *highlight_item = NULL;
  bool highlight_line = FALSE;
  QColor highlight_color = QColor(Qt::blue);

  QString strippedString1 = QString::null; // MPI only.

  QString *strings = NULL;

  if( gotHeader == FALSE ) {

#ifdef DEBUG_StatsPanel
    printf("outputCLIData, ATTEMPT TO PUT OUT HEADER gotHeader=%d, staticDataFLAG... = (%d)\n", 
            gotHeader, staticDataFLAG );
    printf("outputCLIData, (sml != NULL)... = (%d)\n", (sml != NULL) );
#endif

#if 0
    int columnWidth = 200;
#else
    int columnWidth = 160;
#endif
    for( FieldList::Iterator it = columnFieldList.begin();
       it != columnFieldList.end();
       ++it)
    {
      QString s = ((QString)*it).stripWhiteSpace();
      columnHeaderList.push_back(s);

#ifdef DEBUG_StatsPanel
      printf("outputCLIData, looking for Start or Stop Time, s=(%s), columnWidth=%d\n", s.ascii(), columnWidth);
#endif
      if ( (s.startsWith("Start Time") != -1) || (s.startsWith("Stop Time") != -1)) {

           columnWidth = 200;
#ifdef DEBUG_StatsPanel
           printf("outputCLIData, found Start or Stop Time, columnWidth=%d\n", columnWidth);
#endif
      } else {
#if 0
      columnWidth = 200;
#else
      columnWidth = 160;
#endif
     }

#ifdef DEBUG_StatsPanel
      printf("outputCLIData, columnHeaderList.push_back(s), outputCLIData, s=(%s), column i=%d\n",
             s.ascii(), i );
      printf("push_back(s), columnHeaderList.size()=%d\n", columnHeaderList.size() );
#endif

      // IF the string contains the no data samples message then
      // check to see if there is a StatsPanel Message Label available for use
      //      IF so, hide the stats panel list widget and chart form
      //             and view the message label 
      //      
      if( s.find("There were no data") >= 0 ) {
       if (sml != NULL) {
           splv->hide();
           cf->hide();
           sml->show();
           break;
#ifdef DEBUG_StatsPanel
           printf("outputCLIData, show sml hide splv (sml != NULL)... = (%d)\n", (sml != NULL) );
#endif
       }

      } else if( s.find("%") != -1 ) {
        if( percentIndex == -1 ) {
          percentIndex = i;

#ifdef DEBUG_StatsPanel
          printf("outputCLIData, Found a column with % in the header, percentIndex at %d\n", 
                 percentIndex);
#endif

        }

      } else if( (s.find("Function (defining location)") != -1) ||
                 (s.find("LinkedObject") != -1) ||
                 (s.find("Statement Location (Line Number)") != -1)  ) {

#ifdef DEBUG_StatsPanel
         printf("outputCLIData, Found a column with Function (defining..) in the header, percentIndex at %d\n", percentIndex);
#endif

        QSettings *settings = new QSettings();
        bool fullPathBool = settings->readBoolEntry( "/openspeedshop/general/viewFullPath");
        bool dynamicFieldSize = settings->readBoolEntry( "/openspeedshop/general/viewFieldSizeIsDynamic");
        if( fullPathBool  && !dynamicFieldSize ) {

#ifdef DEBUG_StatsPanel
        printf("fullPathBool=(%d)\n", fullPathBool );
#endif
           columnWidth = 1200;
         } else {
           columnWidth = 600;
         }
        delete settings;
      }
#ifdef DEBUG_StatsPanel
      printf("outputCLIData, adding column with columnWidth=%d\n", columnWidth);
#endif

      splv->addColumn( s, columnWidth );
      i++;
    }
  
    gotHeader = TRUE;

#ifdef DEBUG_StatsPanel
   printf("outputCLIData, return early from outputCLIData gotHeader=(%d)\n", gotHeader);
#endif

    return;

  } else {

#ifdef DEBUG_StatsPanel
    printf("outputCLIData, gotHeader=%d, staticDataFLAG... = (%d)\n", gotHeader, staticDataFLAG );
#endif

    strings = new QString[columnFieldList.count()];
    int i = 0;
    for( FieldList::Iterator it = columnFieldList.begin();
       it != columnFieldList.end();
       ++it)
    {
      QString s = ((QString)*it).stripWhiteSpace();
      strings[i] = s;
      i++;
    }
    fieldCount = i;
  }

#ifdef DEBUG_StatsPanel
   printf("outputCLIData, PUT OUT DATA, fieldCount=%d\n", fieldCount);
#endif

  if ( fieldCount == 0 )
  {
    QMessageBox::information( (QWidget *)this, tr("Info:"), tr("There were no data samples for this experiment execution.\nPossible reasons for this could be:\n   The executable being run didn't run long enough to record performance data.\n   The type of performance data being gathered may not be present in the executable being executed.\n"), QMessageBox::Ok );
  
#ifdef DEBUG_StatsPanel
   printf("outputCLIData, return early from outputCLIData fieldCount=(%d)\n", fieldCount);
#endif

    return;
  }

  SPListViewItem *splvi;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::outputCLIData, ATTEMPT TO PUT OUT DATA, More Function currentCollectorStr=%s, MPItraceFLAG=(%d)\n", currentCollectorStr.ascii(), MPItraceFLAG);
#endif

  if( (( currentCollectorStr == "mpi" || 
         currentCollectorStr == "mpit" || 
         currentCollectorStr == "mem" || 
         currentCollectorStr == "pthreads" || 
         currentCollectorStr == "io" || 
         currentCollectorStr == "iot" ) && 
        (MPItraceFLAG == FALSE &&
         !currentUserSelectedReportStr.startsWith("Functions")) && 
         ( currentUserSelectedReportStr.startsWith("CallTrees") ||
           currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || 
           currentUserSelectedReportStr.startsWith("Functions") || 
           currentUserSelectedReportStr.startsWith("TraceBacks") || 
           currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
           currentUserSelectedReportStr.startsWith("Butterfly") ) ) ||
        (currentCollectorStr == "iop" && 
        ( currentUserSelectedReportStr == "Butterfly" || 
          currentUserSelectedReportStr.startsWith("TraceBacks") || 
          currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
          currentUserSelectedReportStr.startsWith("CallTrees") || 
          currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
        ((currentCollectorStr == "usertime" ||
         currentCollectorStr == "omptp" ) && 
        ( currentUserSelectedReportStr == "Butterfly" || 
          currentUserSelectedReportStr.startsWith("TraceBacks") || 
          currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
          currentUserSelectedReportStr.startsWith("CallTrees") || 
          currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
        (currentCollectorStr == "fpe" &&
        (currentUserSelectedReportStr == "Butterfly" || 
         currentUserSelectedReportStr.startsWith("TraceBacks") || 
         currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
         currentUserSelectedReportStr.startsWith("CallTrees") || 
         currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
        (currentCollectorStr.startsWith("hwc") && 
        (currentUserSelectedReportStr == "Butterfly" || 
         currentUserSelectedReportStr.startsWith("TraceBacks") || 
         currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
         currentUserSelectedReportStr.startsWith("CallTrees") || 
         currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) ) )
  {
    QString indentChar = ">";

    if( currentUserSelectedReportStr.startsWith("TraceBacks") || 
        currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") ) {
      indentChar = "<";
    } 

    bool indented = strings[fieldCount-1].startsWith(indentChar);
    int indent_level = 0;

// Pretty the output up a bit.
    if( currentUserSelectedReportStr.startsWith("Butterfly") ) {
      if( indented ) {

        // Right side
        strings[fieldCount-1].insert(1,"    ");

// printf("RS: Field (%s)\n", strings[fieldCount-1].ascii() );

      } else if( strings[fieldCount-1].startsWith("<") ) {

        // Left side
        strings[fieldCount-1].remove("<");

// printf("LS: Field (%s)\n", strings[fieldCount-1].ascii() );

      } else {

// printf("Color this one: Field (%s)\n", strings[fieldCount-1].ascii() );

        // Focused Function
        strings[fieldCount-1].insert(0,"  ");
        highlight_line = TRUE;
      }

// printf("here (%s)\n", strings[fieldCount-1].ascii() );

    }


// printf("indented = (%d)\n", indented );
// printf("%d (%s) (%s)\n", indented, strings[0].ascii(), strings[fieldCount-1].ascii() );
  
    if( !indented ) {

// printf("indent_level=zero lastIndentLevel=%d\n", indent_level, lastIndentLevel );

      // If it's not indented, make sure if is put after the last
      // root node.

      if( lastlvi ) {

        SPListViewItem *topParent = (SPListViewItem *)lastlvi->parent();

        while( topParent ) {
          lastlvi = topParent;
          topParent = (SPListViewItem *)topParent->parent();
        }

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::outputCLIData, Put after (%s) \n", 
                lastlvi->text(fieldCount-1).ascii() );
        printf("StatsPanel::outputCLIData, xxxfileName.ascii()=%s, xxxfuncName.ascii()=%s\n", 
                xxxfileName.ascii(), xxxfuncName.ascii());
#endif

      } // end lastlvi

      lastlvi = splvi =  MYListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings);

      if( highlight_line ) {
        highlight_item = splvi;
      }

      lastIndentLevel = 0;

    } else {

      if( indented && lastlvi != NULL ) {

        QRegExp rxp = QRegExp( "[_,' ',@,A-Z,a-z,0-9,%]");
        indent_level = strings[fieldCount-1].find(rxp);
        strippedString1 = strings[fieldCount-1].mid(indent_level,9999);
        strings[fieldCount-1] = strippedString1;

        // Pretty up the format a bit.
        if( currentUserSelectedReportStr.startsWith("Butterfly") ) {
          strings[fieldCount-1].replace(0,0,QString("  ")); // This is the bad boy
        }

        if( indent_level == -1 ) {
          fprintf(stderr, "Error in determining depth for (%s).\n", strippedString1.ascii() );

        }

// printf("indent_level = %d lastIndentLevel = %d\n", indent_level, lastIndentLevel);

        if( indent_level > lastIndentLevel ) {

// printf("A: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), lastlvi->text(fieldCount-1).ascii() );

          lastlvi = splvi =  MYListViewItem(  this, 
                                              xxxfuncName, 
                                              xxxfileName, 
                                              xxxlineNumber, 
                                              lastlvi, 
                                              lastlvi, 
                                              strings);
          if( highlight_line ) {
            highlight_item = splvi;
          }

        } else {

// printf("Go figure out the right leaf to put this in...(%s) \n", strings[1].ascii() );

          SPListViewItem *mynextlvi = lastlvi;
          SPListViewItem *after = NULL;

          while( mynextlvi->parent() ) {
             mynextlvi = (SPListViewItem *)mynextlvi->parent();
          }

          for(int i=0;i<indent_level-1;i++)
          {
            SPListViewItem *lastChild = (SPListViewItem *)mynextlvi->firstChild();
            while( lastChild->nextSibling() ) {
              lastChild = (SPListViewItem *)lastChild->nextSibling();
            }
            mynextlvi = lastChild;
          }
  
          lastlvi = mynextlvi;
  
          // go to this head, count down the children of the indent level... Then add
          // this item.
          // after = (SPListViewItem *)lastlvi->parent()->firstChild();
          after = (SPListViewItem *)lastlvi->firstChild();
          while( after->nextSibling() ) {
            after = (SPListViewItem *)after->nextSibling();
          }

// printf("C: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), after->text(fieldCount-1).ascii() );

          lastlvi = splvi = MYListViewItem( this, 
                                            xxxfuncName, 
                                            xxxfileName, 
                                            xxxlineNumber, 
                                            lastlvi, 
                                            after, 
                                            strings );
          if( highlight_line ) {
            highlight_item = splvi;
          }

        }

      } else {
        fprintf(stderr, "Error in chaining child (%s) to tree.\n", strippedString1.ascii() );
      }
    }

    // Now try to open all the items.\n");
// printf("  indent_level=%d\n", indent_level );

    if( indent_level < levelsToOpen || levelsToOpen == -1 ) {
      if( lastlvi ) {
        lastlvi->setOpen(TRUE);
      }
    }

// printf("open lastlvi=(%s)\n", lastlvi->text(fieldCount-1).ascii() );

    lastIndentLevel = indent_level;
  } else {
    if( fieldCount == 2 ) {
      lastlvi = splvi =  new SPListViewItem( this, 
                                             xxxfuncName, 
                                             xxxfileName, 
                                             xxxlineNumber, 
                                             splv, 
                                             lastlvi, 
                                             strings[0], 
                                             strings[1] );

    } else if( fieldCount == 3 ) { // i.e. like pcsamp

      lastlvi = splvi =  new SPListViewItem( this, 
                                             xxxfuncName, 
                                             xxxfileName, 
                                             xxxlineNumber, 
                                             splv, 
                                             lastlvi, 
                                             strings[0], 
                                             strings[1], 
                                             strings[2] );

    } else { // i.e. like usertime

#if 0
      lastlvi = splvi =  new SPListViewItem( this, 
                                             xxxfuncName, 
                                             xxxfileName, 
                                             xxxlineNumber, 
                                             splv, 
                                             lastlvi, 
                                             strings[0], 
                                             strings[1], 
                                             strings[2], 
                                             strings[3] );
#endif

      lastlvi = splvi =  MYListViewItem( this, 
                                         xxxfuncName, 
                                         xxxfileName, 
                                         xxxlineNumber, 
                                         splv, 
                                         lastlvi, 
                                         strings);

      if( highlight_line ) {
        highlight_item = splvi;
      }
    }
  }

#ifdef DEBUG_StatsPanel
printf("Are there any cpvl values?\n");
for( ChartPercentValueList::Iterator it = cpvl.begin();
       it != cpvl.end();
       ++it)
{
  int v = (int)*it;
  printf("v=(%d)\n", v);
}
#endif 

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::outputCLIData, CHART, BEFORE if cpvl.count() <= numberItemsToDisplayInChart, total_percent=(%f) cpvl.count()=%d numberItemsToDisplayInStats=%d ctvl.count()=%d numberItemsToDisplayInChart=%d\n", 
          total_percent, cpvl.count(), numberItemsToDisplayInStats, ctvl.count(), numberItemsToDisplayInChart );
#endif



  if( cpvl.count() <= numberItemsToDisplayInChart &&
      ctvl.count() < numberItemsToDisplayInChart )
  {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, fieldCount=%d\n", fieldCount);
#endif

    int percent = 0;
    for( i = 0; i<fieldCount; i++)
    {
      QString value = strings[i];
      if( i == 0 ) // Grab the (some) default metric FIX
      {

        float f = value.toFloat();

        if (f > maxEntryBasedOnTotalTime) {
          maxEntryBasedOnTotalTime = f;
        }

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, add f=%f to TotalTime=%f, i=%d, maxEntryBasedOnTotalTime=%f\n", 
               f, TotalTime, i, maxEntryBasedOnTotalTime);
#endif

        TotalTime += f;
      }
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d, i=%d\n", percentIndex, i);
#endif

      if( percentIndex == i ) {

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d,EQUALS i=%d, value.isEmpty()=%d\n", 
                 percentIndex, i, value.isEmpty());
#endif

        if( !value.isEmpty() ) {

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d,EQUALS i=%d, value=%s\n", 
                 percentIndex, i, value.ascii());
#endif

          float f = value.toFloat();
          percent = (int)f;

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, ADDING percent=(%d) to total_percent=%d, percentIndex=%d, i=%d\n", percent, total_percent, percentIndex, i);
#endif
          total_percent += f;
        }
      }
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, after for fieldCount CHART, A: total_percent=%f\n", total_percent );
#endif

    if( textENUM == TEXT_BYVALUE ) { 

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, TEXT_BYVALUE: textENUM=%d (%s)\n", textENUM, strings[0].stripWhiteSpace().ascii()  );
#endif

      cf->setHeader( (QString)*columnHeaderList.begin() );
      ctvl.push_back( strings[0].stripWhiteSpace() );

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, TEXT_BYVALUE: ctvl.push_back -> (%s) \n", strings[0].stripWhiteSpace().ascii() );
#endif

    } else if( textENUM == TEXT_BYPERCENT ) {

#ifdef DEBUG_StatsPanel
       printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYPERCENT: textENUM=%d\n", textENUM );
#endif

      if( percentIndex > 0 ) {
        ctvl.push_back( strings[percentIndex].stripWhiteSpace() );
        ColumnList::Iterator hit = columnHeaderList.at(percentIndex);
        cf->setHeader( (QString)*hit );
      }

    } else if( textENUM == TEXT_BYLOCATION ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYLOCATION: textENUM=%d\n", textENUM );
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYLOCATION: ctvl.push_back -> (%s) \n", strings[columnHeaderList.count()-1].stripWhiteSpace().ascii() );
#endif
      ctvl.push_back( strings[columnHeaderList.count()-1].stripWhiteSpace() );

      if( columnHeaderList.count() > 0 ) {
        ColumnList::Iterator hit = columnHeaderList.at(columnHeaderList.count()-1);
        cf->setHeader( (QString)*hit );
      }
    }


#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, cpvl.push_back percent=%f\n", percent );
#endif

    // We always push back the percent for the value list.
    cpvl.push_back( percent );
  }

  if( highlight_line ) {

    //    highlight_item->setSelected(TRUE);
    for( int i=0;i<fieldCount;i++)
    {
      highlight_item->setBackground( i, QColor(Qt::red) );
    }
  }

  delete []strings;

#ifdef DEBUG_StatsPanel
   printf("Exit StatsPanel::outputCLIData\n");
#endif
}

void
StatsPanel::outputAboutData(QString *incoming_data)
{
  aboutOutputString += *incoming_data;
}


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, QListView *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 1 item (%s)\n", strings[0].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0] );
      break;
    case 2:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 2 item (%s) (%s)\n", strings[0].ascii(), strings[1].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 3 item (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 4 item (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 5 item (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 6 item (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 7 item (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 8 item, (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii(), strings[7].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
#ifdef DEBUG_StatsPanel
      printf("Warning: over 9 columns... Notify developer...\n");
#endif
      for( int i=8; i<fieldCount; i++ )
      {
        item->setText(i, strings[i]);
      }
      break;
  }

  return item;
}
#undef outputCLIData


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, SPListViewItem *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0] );
      break;
    case 2:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      for( int i=8; i<fieldCount; i++ )
      {
        item->setText(i, strings[i]);
      }
      break;
  }

  return item;
}

QString
StatsPanel::findSelectedFunction()
{
#ifdef DEBUG_StatsPanel
  printf("findSelectedFunction() entered\n");
#endif
  QString functionStr = QString::null;
  QListViewItem *selected_function_item = NULL;
  QListViewItemIterator it( splv, QListViewItemIterator::Selected );
  while( it.current() ) {
    int i = 0;
    selected_function_item = it.current();
    break;  // only select one for now...
    ++it;
  }

  if( selected_function_item ) {
    SPListViewItem *spitem = (SPListViewItem *)selected_function_item;
#ifdef DEBUG_StatsPanel
    std::cerr << "StatsPanel::findSelectedFunction, spitem->funcName=" << spitem->funcName.ascii() << std::endl;
    std::cerr << "StatsPanel::findSelectedFunction, spitem->fileName=" << spitem->fileName.ascii() << std::endl;; 
    std::cerr << "StatsPanel::findSelectedFunction, spitem->lineNumber=" << spitem->lineNumber << std::endl;
#endif

// 
// This section of code was causing std template library names to have (unsigned long) type
// information trimmed off, causing failures in per function commands like butterfly and calltree views 
// We are not sure what this was trying to clean 
// 9/20/2016
#if 0
    // Clean up the function name if it needs to be...
    int index = spitem->funcName.find("(");
    printf("StatsPanel::findSelectedFunction, index from spitem->funcName.find()=%d\n", index);
    if( index != -1 ) {
      QString clean_funcName = spitem->funcName.mid(0, index);
      return( clean_funcName );
    } else {
#endif
      return( spitem->funcName );
#if 0
    }
#endif
  } else {
    return( QString::null );
  }
  return( QString::null );
}

void
StatsPanel::resetRedirect()
{
// Just make sure any pending output goes "somewhere".
  Panel *cmdPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *) "&Command Panel");
  if( cmdPanel )
  {
    MessageObject *msg = new MessageObject("Redirect_Window_Output()");
    cmdPanel->listener((void *)msg);
    delete msg;
  } else
  {
    fprintf(stderr, "Unable to redirect output to the cmdpanel.\n");
  }
}

QString
StatsPanel::getFilenameFromString( QString selected_qstring )
{

// This is format of string: tdot_ (matmulMOD: matmulMOD.f90,5301)
// But must also consider this form: ggGridIterator<mrSurface*>::Next(mrSurface*&, double&, double&) (eon: ggGrid.h,259)

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFilenameFromString: Get filename from (%s)\n", selected_qstring.ascii() );
#endif
  QString filename = QString::null;

  int sfi = 0;

#if OLDWAY
  sfi = selected_qstring.find(" in ");
// printf("sfi=%d (Was there an \" in \"\n", sfi );

  if( sfi == -1 ) {
//    sfi = selected_qstring.find(" ");
//  } else {
    sfi = selected_qstring.find(": ");
    sfi += 1;
  }
  sfi++;

  int efi = selected_qstring.find(",");

// printf("sfi=(%d) efi=(%d) (Was there a \",\" in (%s)\n", sfi, efi, selected_qstring.ascii()  );

  filename = selected_qstring.mid(sfi, efi-sfi );

#else

// Here is the new algorithm

  int efi = selected_qstring.findRev(")");
#ifdef DEBUG_StatsPanel
    printf("efi=%d after findRev for right parens \n", efi );
#endif
  if( efi == -1 ) {
      // return null
      return(filename);
  } else {
    // This was part of the default path through this routine
    // Commenting this out because have not seen this happen and it is not a very safe check.
    // Leaving this in to see it this is really used anymore
#if 0
    sfi = selected_qstring.find(" in ");
#else
    sfi = -1;
#endif

#ifdef DEBUG_StatsPanel
    printf("sfi=%d (Was there an \" in \"\n", sfi );
#endif
    if ( sfi == -1 )  {
      sfi = selected_qstring.findRev(": ");
      if( sfi == -1 ) {
        // return null
        return(filename);
    } else {
      // bump once for :
      sfi++;
      // bump once for space
      sfi++;
      efi = selected_qstring.findRev(",", efi);
#ifdef DEBUG_StatsPanel
      printf("efi=%d after findRev for , \n", efi );
#endif

// This is format of string: tdot_ (matmulMOD: matmulMOD.f90,5301)
// But must also consider this form: ggGridIterator<mrSurface*>::Next(mrSurface*&, double&, double&) (eon: ggGrid.h,259)

#ifdef DEBUG_StatsPanel
      printf("new algorithm, sfi=(%d) efi=(%d) selected_qstring.ascii()=(%s), filename=%s\n", sfi, efi, selected_qstring.ascii(), selected_qstring.mid(sfi, efi-sfi ).ascii()  );
#endif

      filename = selected_qstring.mid(sfi, efi-sfi );
    }
   } else {
#ifdef DEBUG_StatsPanel
      printf("new algorithm, found - in - sfi=(%d) efi=(%d) (Was there a \",\" in (%s), filename=%s\n", sfi, efi, selected_qstring.ascii(), selected_qstring.mid(sfi, efi-sfi ).ascii()  );
#endif
     // sfi was not -1 on search for " in "
     filename = selected_qstring.mid(sfi, efi-sfi );
   }
 
  }

#endif
// printf("   returning filename=(%s)\n", filename.ascii() );
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFilenameFromString: returning filename=(%s)\n", filename.ascii() );
#endif

  return(filename);
}

QString
StatsPanel::getExperimentIdFromString( QString selected_qstring )
{

// This is format of string: cview -c 3, 5

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentIdFromString, from (%s)\n", selected_qstring.ascii() );
#endif
  QString experiment_id = QString::null;

  int sfi = 0;


  sfi = selected_qstring.find("-c ");
  sfi += 3;

  int efi = selected_qstring.find(",");

// printf("sfi=(%d) efi=(%d) (Was there a \",\" in (%s)\n", sfi, efi, selected_qstring.ascii()  );

  experiment_id = selected_qstring.mid(sfi, efi-sfi );

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::getLinenumberFromString, returning experiment_id=(%s)\n", experiment_id.ascii() );
#endif

  return(experiment_id);
}

QString
StatsPanel::getLinenumberFromString( QString selected_qstring )
{

// This is format of string: tdot_ (matmulMOD: matmulMOD.f90,5301)

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLinenumberFromString, Get linenumber from (%s)\n", selected_qstring.ascii() );
#endif

  QString linenumber = QString::null;

  int sfi = 0;

#if OLDWAY
  sfi = selected_qstring.find(" in ");
#ifdef DEBUG_StatsPanel
  printf("sfi=%d (Was there an \" in \"\n", sfi );
#endif

  if( sfi == -1 )
  {
//    sfi = selected_qstring.find(" ");
//  } else {
    sfi = selected_qstring.find(",");
//    sfi += 1;
  }
  sfi++;

  int efi = selected_qstring.find(")");

#ifdef DEBUG_StatsPanel
 printf("sfi=(%d) efi=(%d) (Was there a \",\" in (%s)\n", sfi, efi, selected_qstring.ascii()  );
#endif

  linenumber = selected_qstring.mid(sfi, efi-sfi );
#else

// Here is the new algorithm - jeg
// Look from the end of the string to avoid all the template and class special characters

  int efi = selected_qstring.findRev(")");
  if( efi == -1 ) {
      // return null
      return(linenumber);
  } else {


#if 0
    // This was part of the default path through this routine
    // Commenting this out because have not seen this happen and it is not a very safe check.
    // Leaving this in to see it this is really used anymore
    sfi = selected_qstring.find(" in ");
#else
    sfi = -1;
#endif
    

#ifdef DEBUG_StatsPanel
    printf("sfi=%d (Was there an \" in \"\n", sfi );
#endif
    if ( sfi == -1 )  {
      sfi = selected_qstring.findRev(",");
      if( sfi == -1 ) {
        // return null
        return(linenumber);
    } else {
      sfi++;

#ifdef DEBUG_StatsPanel
      printf("new algorithm, sfi=(%d) efi=(%d) (Was there a \",\" in (%s), linenumber=%s\n", sfi, efi, selected_qstring.ascii(), selected_qstring.mid(sfi, efi-sfi ).ascii()  );
#endif

      linenumber = selected_qstring.mid(sfi, efi-sfi );
    }
   } else {
#ifdef DEBUG_StatsPanel
      printf("new algorithm, found - in - sfi=(%d) efi=(%d) (Was there a \",\" in (%s), linenumber=%s\n", sfi, efi, selected_qstring.ascii(), selected_qstring.mid(sfi, efi-sfi ).ascii()  );
#endif
     // sfi was not -1 on search for " in "
     linenumber = selected_qstring.mid(sfi, efi-sfi );
   }
 
  }

#endif

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::getLinenumberFromString, returning linenumber=(%s)\n", linenumber.ascii() );
#endif

  return(linenumber);
}

QString
StatsPanel::getFunctionNameFromString( QString selected_qstring, QString &lineNumberStr )
{

// This is format of string: tdot_ (matmulMOD: matmulMOD.f90,5301)

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFunctionNameFromString, Get funcString from %s\n", selected_qstring.ascii() );
#endif

  QString funcString = QString::null;
  QString workString = selected_qstring;

  int sfi = 0;

  sfi = selected_qstring.find(" in ");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFunctionNameFromString, sfi=%d (Was there an \" in \"\n", sfi );
#endif

  if( sfi != -1 ) {
    workString = selected_qstring.mid(sfi+4);
  } else {
    workString = selected_qstring;
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFunctionNameFromString, Start your function lookup from (%s)\n", workString.ascii() );
#endif

  funcString = workString.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFunctionNameFromString, funcString=(%s)\n", funcString.ascii() );
#endif

  int efi = workString.find("(");
  QString function_name = workString.mid(0,efi);

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::getFunctionNameFromString, function_name=(%s)\n", function_name.ascii() );
#endif

  if( ( currentCollectorStr == "mpi" || 
        currentCollectorStr == "mpit" || 
        currentCollectorStr == "pthreads" || 
        currentCollectorStr == "mem" || 
        currentCollectorStr == "io" || 
        currentCollectorStr == "iot" ) && ( collectorStrFromMenu.startsWith("CallTrees") || 
                                            collectorStrFromMenu.startsWith("Functions") || 
                                            collectorStrFromMenu.startsWith("TraceBacks") || 
                                            collectorStrFromMenu.startsWith("TraceBacks,FullStack") ) )
  {
    int bof = -1;
    int eof = workString.find('(');

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::getFunctionNameFromString, eof=%d\n", eof);
#endif

    if( eof == -1 ) {
// printf("main:  you should never be here..\n");
      function_name = "main";
    } else {

      QString tempString = workString.mid(0,eof);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::getFunctionNameFromString, tempString=%s\n", tempString.ascii() );
#endif

      QRegExp rxp = QRegExp( "[ >]");
      bof = tempString.findRev(rxp, eof);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getFunctionNameFromString, bof=%d\n", bof);
#endif
      if( bof == -1 ) {
        bof = 0;
      } else {
        bof++;
      }
    }
    function_name = workString.mid(bof,eof-bof);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getFunctionNameFromString, bof=%d, eof-bof=%d, workString.mid(bof,eof-bof)=%s\n", bof, eof-bof, function_name.ascii());
#endif

    int boln = workString.find('@');
    boln++;
    int eoln = workString.find(" in ");
    lineNumberStr = workString.mid(boln,eoln-boln).stripWhiteSpace();

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::getFunctionNameFromString, lineNumberStr=(%s)\n", lineNumberStr.ascii() );
    printf("StatsPanel::getFunctionNameFromString, mpi: function_name=(%s)\n", function_name.ascii() );
#endif


  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getFunctionNameFromString, returning function_name=(%s) lineNumberStr=(%s)\n", function_name.ascii(), lineNumberStr.ascii() );
#endif

  return(function_name);
}

//
// Check for mangled name and return the mangled name, otherwise return the input QString
// as the function name to use in the CLI call.
//
QString StatsPanel::getMangledFunctionNameForCLI(QString inputFuncStr)
{
QString newMangledFuncStr = findMangledNameForCLIcommand(inputFuncStr.ascii());
if (newMangledFuncStr == QString::null) {
    newMangledFuncStr = inputFuncStr;
}

return newMangledFuncStr;

}

QString
StatsPanel::generateCommand()
{

  // Local that holds either the trace or non-trace preference for how many items to display in the stats panel
  int items_to_display = 0;

#ifdef DEBUG_StatsPanel
  printf("ENTER StatsPanel::generateCommand, expID=(%d), focusedExpID=%d\n",  expID, focusedExpID);
  if (!currentCollectorStr.isEmpty()) {
     printf("StatsPanel::generateCommand, currentCollectorStr=(%s), MPItraceFLAG=(%d), IOtraceFLAG=%d\n", 
         currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii(), MPItraceFLAG, IOtraceFLAG );
  }
  if (!currentUserSelectedReportStr.isEmpty()) {
     printf("StatsPanel::generateCommand, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii());
  }

  if (!currentThreadsStr.isEmpty()) {
    printf("StatsPanel::generateCommand, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
  }
#endif

  int exp_id = -1;
  if( focusedExpID == -1 ) {
    exp_id = expID;
  } else {
    exp_id = focusedExpID;
  }

#ifdef DEBUG_StatsPanel_toolbar
  printf("GENERATE_COMMAND, prior to method checks, compareExpIDs.size()=%d\n", compareExpIDs.size());
#endif

  if (lastCommand.startsWith("cview -c") && compareExpIDs.size() > 0) {

     // don't burden the user with a choice dialog, there is only one experiment being offered

     if (compareExpIDs.size() == 1) {
          exp_id = compareExpIDs[0];

#ifdef DEBUG_StatsPanel_toolbar
     printf("GENERATE_COMMAND, 1st method, compareExpIDs.size() == 1), exp_id=%d\n", exp_id);
     printf("GENERATE_COMMAND, 1st method, compareExpIDs.size() == 1),  lastCommand=(%s)\n", lastCommand.ascii());
#endif

     } else {

       exp_id = getValidExperimentIdForView();    

#ifdef DEBUG_StatsPanel_toolbar
       printf("GENERATE_COMMAND, 2nd method, exp_id=%d\n", exp_id);
       printf("GENERATE_COMMAND, 2nd method,  lastCommand=(%s)\n", lastCommand.ascii());
       printf("GENERATE_COMMAND, after getValidExperimentIdForView, exp_id=%d\n", exp_id);
#endif

     }

     focusedExpID = exp_id;

  } 
#ifdef DEBUG_StatsPanel_toolbar
     printf("GENERATE_COMMAND, after cview checks, currentCollectorStr=(%s)\n", currentCollectorStr.ascii());
     printf("GENERATE_COMMAND, after cview checks, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii());
#endif

  // If this is the first call to generateCommand then we want 
  // to change the user command variable to DefaultView 
  // so we get the optional Functions, Statements, Linked Object
  // options on the tool bar.
#ifdef DEBUG_StatsPanel_toolbar
     printf("GENERATE_COMMAND, firstGenerateCommandCall=(%d)\n", firstGenerateCommandCall);
#endif
  if (firstGenerateCommandCall) {
     originatingUserSelectedReportStr = "DefaultView";
     currentUserSelectedReportStr = "Functions";
#ifdef DEBUG_StatsPanel_toolbar
     printf("GENERATE_COMMAND, setting to FALSE, firstGenerateCommandCall=(%d)\n", firstGenerateCommandCall);
#endif
     firstGenerateCommandCall = false;
  }

  if( currentCollectorStr == "io" || 
      currentCollectorStr == "iot" ) {
    if( IOtraceFLAG == TRUE ) {
      traceAddition = " -v trace";
    }
  } else if( currentCollectorStr == "mpi" || 
             currentCollectorStr == "mpit" ) {
    if( MPItraceFLAG == TRUE ) {
      traceAddition = " -v trace";
    }
  }

  if( !traceAddition.isEmpty() ) {
     items_to_display = numberTraceItemsToDisplayInStats;
  } else {
     items_to_display = numberItemsToDisplayInStats;
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::generateCommand, traceAddition=(%s)\n", traceAddition.ascii() );
#endif

  QString modifierStr = QString::null;

  updateCollectorList();

  updateCollectorMetricList();

  if (currentCollectorStr == "hwcsamp" ) {
    updateCollectorParamsValList();
#if 0
  } else if (currentCollectorStr == "usertime" ) {
    updateCollectorParamsValList();
#endif
  }

  updateThreadsList();

  lastAbout = aboutString;

#ifdef DEBUG_StatsPanel_toolbar
     printf("GENERATE_COMMAND, after updateCollectorxxx calls, currentCollectorStr=(%s)\n", currentCollectorStr.ascii());
     printf("GENERATE_COMMAND, after updateCollectorxxx calls, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii());
     printf("GENERATE_COMMAND, Find_Experiment_Object() for %d\n", exp_id);
#endif

  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", exp_id);


  QString command = QString("expView -x %1").arg(exp_id);
  aboutString = QString("Experiment: %1\n").arg(exp_id);

  // For Hot Call Path view, just set the number of items to display to one and change the 
  // user requested report string to CallTrees with a FullStack.
  if (currentUserSelectedReportStr == "HotCallPath") {
     items_to_display = 5;
     currentUserSelectedReportStr = "CallTrees,FullStack";
  }
  // Show memory leaked view
  if (currentUserSelectedReportStr == "memLeakedPath") {
     currentUserSelectedReportStr = "Leaked,FullStack";
  }
  // Show memory highwater view
  if (currentUserSelectedReportStr == "memHighwaterPath") {
     currentUserSelectedReportStr = "Highwater,FullStack";
  }
  // Show memory unique call paths view
  if (currentUserSelectedReportStr == "memUniquePath") {
     currentUserSelectedReportStr = "Unique,FullStack";
  }

  if( currentCollectorStr.isEmpty() ) {
    if( items_to_display > 0 ) {

      command += QString(" %1%2").arg("stats").arg(items_to_display);
      aboutString += QString("Requested data for all collectors for top %1 items\n").arg(items_to_display);
      infoAboutString += QString("Experiment type: 'all' for top %1 items\n").arg(items_to_display);

    } else {

      command += QString(" %1").arg("stats");
      aboutString += QString("Requested data for all collectors\n");
      infoAboutString += QString("Data displayed is from all collectors\n");

    }
  } else {

   // Current collector string is not empty

    if( items_to_display > 0 ) {

      command += QString(" %1%2").arg(currentCollectorStr).arg(items_to_display);
      aboutString += QString("Requested data for collector %1 for top %2 items\n").arg(currentCollectorStr).arg(items_to_display);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::generateCommand() before setting, infoAboutString.ascii()=%s\n", infoAboutString.ascii());
#endif

      infoAboutString += QString("Experiment type: %1 for top %2 items\n").arg(currentCollectorStr).arg(items_to_display);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::generateCommand() after setting, infoAboutString.ascii()=%s\n", infoAboutString.ascii());
#endif
    } else {

      command += QString(" %1").arg(currentCollectorStr);
      aboutString += QString("Requested data for collector %1\n");
      infoAboutString += QString("Data displayed is from collector %1\n");

    }

  }

  if( !currentUserSelectedReportStr.isEmpty() && 
      !currentCollectorStr.isEmpty() ) {

    if( currentCollectorStr != currentUserSelectedReportStr ) {  

       // If these 2 are equal, we want the default display... not a 
       // specific metric.

#ifdef DEBUG_StatsPanel
       printf("generateCommand, (If these 2 are equal case block): command=(%s) currentCollectorStr=(%s) currentUserSelectedReportStr(%s) currentMetricStr=(%s)\n", 
        command.ascii(), currentCollectorStr.ascii(), 
        currentUserSelectedReportStr.ascii(), currentMetricStr.ascii() );
#endif
       if (!currentMetricStr.isEmpty()) {
         command += QString(" -m %1").arg(currentMetricStr);
         aboutString += QString("for metrics %1\n").arg(currentMetricStr);
        }
       infoAboutString += QString("Display options: %1\n").arg(currentUserSelectedReportStr);
    }
  }


#ifdef DEBUG_StatsPanel
 printf("generateCommand, so far: command=(%s) currentCollectorStr=(%s) currentUserSelectedReportStr(%s) currentMetricStr=(%s)\n", 
        command.ascii(), currentCollectorStr.ascii(), 
        currentUserSelectedReportStr.ascii(), currentMetricStr.ascii() );
#endif


  if( (currentCollectorStr == "pcsamp" || 
       currentCollectorStr == "hwcsamp" ) &&
      (currentUserSelectedReportStr == "Functions") || 
      (currentUserSelectedReportStr == "LinkedObjects") || 
#if defined(HAVE_DYNINST)
      (currentUserSelectedReportStr == "Loops") || 
#endif
      (currentUserSelectedReportStr == "minMaxAverage") || 
      (currentUserSelectedReportStr == "Statements by Function") ||
      (currentUserSelectedReportStr == "Statements") ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, inside pcsamp/hwcsamp specific processing, currentUserSelectedReportStr=(%s)\n",
              currentUserSelectedReportStr.ascii());
#endif

    if( currentUserSelectedReportStr.isEmpty() ) { 
      currentUserSelectedReportStr = "Functions";
    }

    if( currentUserSelectedReportStr == "Statements by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, pcsamp/hwcsamp, Statements by Function\n");
#endif

      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, pcsamp/hwcsamp, StatementsByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                 QString("Which filename or function?:"), 
                                 QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 

      }

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v statements -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "minMaxAverage" ) {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -m loadbalance ").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, pcsamp A: load balance command=(%s)\n", command.ascii() );
#endif
      } else {
        command = QString("expView -x %1 %2 -m loadbalance").arg(exp_id).arg(currentCollectorStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, pcsamp B: load balance, command=(%s)\n", command.ascii() );
#endif
      }
  } else {

    if( items_to_display > 0 ) {
      command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(items_to_display).arg(currentUserSelectedReportStr);
    } else {
      command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
    }
  }

#ifdef DEBUG_StatsPanel
  printf("end of pcsamp/hwcsamp generated command (%s)\n", command.ascii() );
#endif

  } else if( currentCollectorStr == "usertime" || 
             currentCollectorStr == "omptp" || 
             currentCollectorStr == "iop" || 
             currentCollectorStr == "mpip" || 
             currentCollectorStr == "fpe" || 
             currentCollectorStr == "io" || 
             currentCollectorStr == "iot" || 
             currentCollectorStr == "mem" || 
             currentCollectorStr == "pthreads" || 
             currentCollectorStr == "hwctime" || 
             currentCollectorStr == "mpi" || 
             currentCollectorStr == "mpit" ) {

#ifdef DEBUG_StatsPanel
    printf("generateCommand, usertime, omptp, iop, fpe,... to mpit, currentCollectorStr=(%s) currentUserSelectedReportStr=(%s)\n", 
           currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

    selectedFunctionStr = findSelectedFunction();

    if( currentUserSelectedReportStr.isEmpty() ) { 
        currentUserSelectedReportStr = "Functions";
    }

    if( currentUserSelectedReportStr == "Butterfly" ) {
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
  printf("generateCommand, Butterfly, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                    QString("Which filename or function?:"), 
                                    QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      }

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v Butterfly -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "Statements by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, Statements by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, StatementsByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                 QString("Which filename or function?:"), 
                                 QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 

      } 

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v statements -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);


    } else if( currentUserSelectedReportStr == "CallTrees by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, CallTrees by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, CallTreesByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      } 

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v CallTrees -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "CallTrees,FullStack by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("CallTrees,FullStack by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, CallTreesFSByFunction,A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      } 

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v CallTrees,FullStack -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "TraceBacks by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("generateCommand, TraceBacks by Function\n");
#endif
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      } 

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v Tracebacks -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "TraceBacks,FullStack by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("TraceBacks,FullStack by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, TracebacksFSByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      }

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      command = QString("expView -x %1 %4%2 -v Tracebacks,FullStack -f \"%3\"").arg(exp_id).arg(items_to_display).arg(newSelectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "minMaxAverage" ) {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -m loadbalance ").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, A: load balance command=(%s)\n", command.ascii() );
#endif
      } else {
        command = QString("expView -x %1 %2 -m loadbalance").arg(exp_id).arg(currentCollectorStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, B: load balance, command=(%s)\n", command.ascii() );
#endif
      }

    } else {

#ifdef DEBUG_StatsPanel
      printf("generateCommand, Here's the usertime menu work stuff.\n");
#endif

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(items_to_display).arg(currentUserSelectedReportStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, A: USERTIME! command=(%s)\n", command.ascii() );
#endif
      } else {
        command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, B: USERTIME! command=(%s)\n", command.ascii() );
#endif
      }
    }

#ifdef DEBUG_StatsPanel
   printf("USERTIME! command=(%s)\n", command.ascii() );
#endif

  } else if( ( ( currentCollectorStr == "hwc" || 
                 currentCollectorStr == "hwctime" || 
                 currentCollectorStr == "mpi" || 
                 currentCollectorStr == "mpit" ) && 
               ( currentUserSelectedReportStr.startsWith("CallTrees") || 
                 currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || 
                 currentUserSelectedReportStr.startsWith("Functions") || 
                 currentUserSelectedReportStr.startsWith("mpi") || 
                 currentUserSelectedReportStr.startsWith("io") || 
                 currentUserSelectedReportStr.startsWith("TraceBacks") || 
                 currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
                 currentUserSelectedReportStr.startsWith("Butterfly") ) )) {

#ifdef DEBUG_StatsPanel
    printf("generateCommand, It thinks we're mpi | io!\n");
#endif
    if( currentUserSelectedReportStr.isEmpty() || currentUserSelectedReportStr == "CallTrees" ) {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v CallTrees").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);
      } else {
        command = QString("expView -x %1 %2 -v CallTrees").arg(exp_id).arg(currentCollectorStr);
      }

    } else if ( currentUserSelectedReportStr == "CallTrees by Selected Function" ) {

      selectedFunctionStr = findSelectedFunction();

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      }

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v CallTrees -f %4").arg(exp_id).arg(currentCollectorStr).arg(items_to_display).arg(newSelectedFunctionStr);
      } else {
        command = QString("expView -x %1 %2 -v CallTrees -f %4").arg(exp_id).arg(currentCollectorStr).arg(newSelectedFunctionStr);
      }

    } else if ( currentUserSelectedReportStr == "TraceBacks" ) {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v TraceBacks").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);
      } else {
        command = QString("expView -x %1 %2%3 -v TraceBacks").arg(exp_id).arg(currentCollectorStr);
      }

    } else if ( currentUserSelectedReportStr == "TraceBacks,FullStack" ) {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v TraceBacks,FullStack").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);
      } else {
        command = QString("expView -x %1 %2 -v TraceBacks,FullStack").arg(exp_id).arg(currentCollectorStr);
      }

    } else if( currentUserSelectedReportStr == "Butterfly" ) {

      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;
#ifdef DEBUG_StatsPanel
        printf("generateCommand, Butterfly B: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Filename or Function Name Dialog:", 
                                  QString("Which filename or function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
        if ( !ok || selectedFunctionStr.isEmpty() ) {
          // user entered nothing or pressed Cancel
          return( QString::null );
        } 
      }

      QString newSelectedFunctionStr = getMangledFunctionNameForCLI(selectedFunctionStr);

      if( items_to_display > 0 ) {

        command = QString("expView -x %1 %2%3 -v Butterfly -f \"%4\"").arg(exp_id).arg(currentCollectorStr).arg(items_to_display).arg(newSelectedFunctionStr);

      } else {

        command = QString("expView -x %1 %2 -v Butterfly -f \"%4\"").arg(exp_id).arg(currentCollectorStr).arg(newSelectedFunctionStr);

      }

    } else {

      if( items_to_display > 0 ) {
        command = QString("expView -x %1 %2%3 -v Functions").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);
      } else {
        command = QString("expView -x %1 %2 -v Functions").arg(exp_id).arg(currentCollectorStr);
      }
    }

  } else if( (currentCollectorStr == "hwc" || 
              currentCollectorStr == "hwctime") &&
            (currentUserSelectedReportStr == "Butterfly") ||
            (currentUserSelectedReportStr == "Functions") ||
            (currentUserSelectedReportStr == "LinkedObjects") ||
#if defined(HAVE_DYNINST)
            (currentUserSelectedReportStr == "Loops") ||
#endif
            (currentUserSelectedReportStr == "Statements") ||
            (currentUserSelectedReportStr == "CallTrees") ||
            (currentUserSelectedReportStr == "CallTrees,FullStack") ||
            (currentUserSelectedReportStr == "TraceBacks") ||
            (currentUserSelectedReportStr == "TraceBacks,FullStack") ) {

    if( currentUserSelectedReportStr.isEmpty() ) { 
      currentUserSelectedReportStr = "Functions";
    }

   if( currentUserSelectedReportStr.startsWith("Statements") ) { 

    if( items_to_display > 0 ) {
      command = QString("expView -x %1 %2%3 -v statements").arg(exp_id).arg(currentCollectorStr).arg(items_to_display);
    } else {
      command = QString("expView -x %1 %2 -v statements").arg(exp_id).arg(currentCollectorStr);
    }

   } else {

    // report string does not start with Statements

    if( items_to_display > 0 ) {
      command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(items_to_display).arg(currentUserSelectedReportStr);
    } else {
      command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
    }

  }

#ifdef DEBUG_StatsPanel
  printf("generateCommand, hwc command=(%s)\n", command.ascii() );
#endif
  aboutString = command + "\n";
 } 

#ifdef DEBUG_StatsPanel
    printf("generateCommand, check before add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
    printf("generateCommand, check before add any focus, currentThreadsStr.isEmpty()=(%d)\n", currentThreadsStr.isEmpty() );
#endif

  // Add any focus.
 if( !currentThreadsStr.isEmpty() ) {

#ifdef DEBUG_StatsPanel
    printf("generateCommand, before add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

    command += QString(" %1").arg(currentThreadsStr);

    // Adding to infoAboutString, so ignore result.
    //QString ignore = getPartialExperimentInfo2(exp_id);

#ifdef DEBUG_StatsPanel
    printf("generateCommand, after add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

  }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, command sofar... =(%s)\n", command.ascii() );
   printf("generateCommand, add any modifiers...\n");
#endif

    std::list<std::string> *modifier_list = NULL;;


#ifdef DEBUG_StatsPanel
   printf("generateCommand: currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
   printf("generateCommand: currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
#endif
    if( currentCollectorStr == "hwc" ) {
      modifier_list = &current_list_of_hwc_modifiers;
    } else if( currentCollectorStr == "hwctime" ) {
      modifier_list = &current_list_of_hwctime_modifiers;
    } else if( currentCollectorStr == "hwcsamp" ) {
      modifier_list = &current_list_of_hwcsamp_modifiers;
    } else if( currentCollectorStr == "io" ) {
      modifier_list = &current_list_of_io_modifiers;
    } else if( currentCollectorStr == "iop" ) {
      modifier_list = &current_list_of_iop_modifiers;
    } else if( currentCollectorStr == "mpip" ) {
      modifier_list = &current_list_of_mpip_modifiers;
    } else if( currentCollectorStr == "mem" ) {
      modifier_list = &current_list_of_mem_modifiers;
    } else if( currentCollectorStr == "pthreads" ) {
      modifier_list = &current_list_of_pthreads_modifiers;
    } else if( currentCollectorStr == "omptp" ) {
      modifier_list = &current_list_of_omptp_modifiers;
    } else if( currentCollectorStr == "iot" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, &current_list_of_iot_modifiers=(%x)\n", &current_list_of_iot_modifiers);
#endif
       for( std::list<std::string>::const_iterator iot_it = current_list_of_iot_modifiers.begin();
          iot_it != current_list_of_iot_modifiers.end(); iot_it++ )
       {
         std::string iot_modifier = (std::string)*iot_it;
#ifdef DEBUG_StatsPanel
         printf("generateCommand, iot_modifier = (%s)\n", iot_modifier.c_str() );
#endif

      } // end for

      modifier_list = &current_list_of_iot_modifiers;

    } else if( currentCollectorStr == "mpi" ) {
      modifier_list = &current_list_of_mpi_modifiers;
    } else if( currentCollectorStr == "mpit" ) {
      modifier_list = &current_list_of_mpit_modifiers;
    } else if( currentCollectorStr == "pcsamp" ) {
      modifier_list = &current_list_of_pcsamp_modifiers;
    } else if( currentCollectorStr == "usertime" ) {
      modifier_list = &current_list_of_usertime_modifiers;
    } else if( currentCollectorStr == "fpe" ) {
      modifier_list = &current_list_of_fpe_modifiers;
    } else {
//      modifier_list = &current_list_of_modifiers;
      modifier_list = &current_list_of_generic_modifiers;
    }

    for( std::list<std::string>::const_iterator it = modifier_list->begin();
       it != modifier_list->end(); it++ )
    {

      std::string modifier = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("generateCommand, modifier = (%s)\n", modifier.c_str() );
#endif
      if( modifierStr.isEmpty() ) {

#ifdef DEBUG_StatsPanel
        printf("generateCommand, modifierStr is empty, A: modifer = (%s)\n", modifier.c_str() );
#endif

        modifierStr = QString(" -m %1").arg(modifier.c_str());
        currentMetricStr = modifier.c_str();
      } else {

#ifdef DEBUG_StatsPanel
        printf("generateCommand, before update modifierStr.ascii()=(%s) B: modifer = (%s)\n", 
               modifierStr.ascii(), modifier.c_str() );
#endif
        modifierStr += QString(",%1").arg(modifier.c_str());

#ifdef DEBUG_StatsPanel
        printf("generateCommand, after update modifierStr.ascii()=(%s) B: modifer = (%s)\n", 
               modifierStr.ascii(), modifier.c_str() );
#endif

      }
    }


#ifdef DEBUG_StatsPanel
     printf("generateCommand, before updating command=(%s) with modifierStr.ascii()=(%s)\n", 
             command.ascii(), modifierStr.ascii() );
#endif

   if( !modifierStr.isEmpty() ) {
     command += QString(" %1").arg(modifierStr);
   }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, after updating command=(%s) with modifierStr.ascii()=(%s)\n", 
           command.ascii(), modifierStr.ascii() );
   printf("generateCommand, before updating command=(%s) with traceAddition.ascii()=(%s)\n", 
           command.ascii(), traceAddition.ascii() );
#endif

   if( !traceAddition.isEmpty() ) {
      command += traceAddition;
   }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, before updating command=(%s) with traceAddition.ascii()=(%s)\n", 
          command.ascii(), traceAddition.ascii() );
   printf("generateCommand() EXIT returning command=(%s), currentCollectorStr=(%s)\n", 
          command.ascii(), currentCollectorStr.ascii() );
#endif

  return( command );
} // End generateCommand

void
StatsPanel::generateMPIMenu(QString collectorName)
{
#ifdef DEBUG_StatsPanel
  printf("generateMPIMenu(%s)\n", collectorName.ascii() );
#endif
  mpi_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  mpi_menu->setCheckable(TRUE);

  mpi_menu->insertSeparator();

  list_of_mpi_modifiers.clear();
  list_of_mpit_modifiers.clear();
  if( collectorName == "mpi" ) {

    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPIReportSelected(int)) );

    if( focusedExpID != -1 ) {
      contextMenu->insertItem(QString("Show Metrics (Exp: %1) : MPI").arg(focusedExpID), mpi_menu);
    } else {
      contextMenu->insertItem(QString("Display Options: MPI"), mpi_menu);
    }

    list_of_mpi_modifiers.push_back("mpi::exclusive_times");
    list_of_mpi_modifiers.push_back("mpi::inclusive_times");
//  list_of_mpi_modifiers.push_back("mpi::exclusive_details");
//  list_of_mpi_modifiers.push_back("mpi::inclusive_details");
    list_of_mpi_modifiers.push_back("min");
    list_of_mpi_modifiers.push_back("max");
    list_of_mpi_modifiers.push_back("average");
    list_of_mpi_modifiers.push_back("count");
    list_of_mpi_modifiers.push_back("percent");
    list_of_mpi_modifiers.push_back("stddev");

//  list_of_mpi_modifiers.push_back("start_time");
//  list_of_mpi_modifiers.push_back("stop_time");
//  list_of_mpi_modifiers.push_back("source");
//  list_of_mpi_modifiers.push_back("dest");
//  list_of_mpi_modifiers.push_back("size");
//  list_of_mpi_modifiers.push_back("tag");
//  list_of_mpi_modifiers.push_back("communicator");
//  list_of_mpi_modifiers.push_back("datatype");
//  list_of_mpi_modifiers.push_back("retval");

    if( mpiModifierMenu )
    {
      delete mpiModifierMenu;
    }
    mpiModifierMenu = new QPopupMenu(this);
    mpiModifierMenu->insertTearOffHandle();
    connect(mpiModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(mpiModifierSelected(int)) );

    generateModifierMenu(mpiModifierMenu, list_of_mpi_modifiers, current_list_of_mpi_modifiers);
    mpi_menu->insertItem(QString("Select mpi details:"), mpiModifierMenu);

    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( mpiModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(MPItraceFLAG);
    qaction->setOn(MPItraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(MPItraceSelected()) );
  } else if( collectorName == "mpit" ) {

    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPITReportSelected(int)) );

    if( focusedExpID != -1 ) {
      contextMenu->insertItem(QString("Show Metrics (Exp: %1) : MPIT").arg(focusedExpID), mpi_menu);
    } else {
      contextMenu->insertItem(QString("Display Options: MPIT"), mpi_menu);
    }

    list_of_mpit_modifiers.push_back("mpit::exclusive_times");
    list_of_mpit_modifiers.push_back("mpit::inclusive_times");
//  list_of_mpit_modifiers.push_back("mpit::exclusive_details");
//  list_of_mpit_modifiers.push_back("mpit::inclusive_details");
    list_of_mpit_modifiers.push_back("min");
    list_of_mpit_modifiers.push_back("max");
    list_of_mpit_modifiers.push_back("average");
    list_of_mpit_modifiers.push_back("count");
    list_of_mpit_modifiers.push_back("percent");
    list_of_mpit_modifiers.push_back("stddev");

    list_of_mpit_modifiers.push_back("start_time");
    list_of_mpit_modifiers.push_back("stop_time");
    list_of_mpit_modifiers.push_back("source");
    list_of_mpit_modifiers.push_back("dest");
    list_of_mpit_modifiers.push_back("size");
    list_of_mpit_modifiers.push_back("tag");
    list_of_mpit_modifiers.push_back("communicator");
    list_of_mpit_modifiers.push_back("datatype");
    list_of_mpit_modifiers.push_back("retval");

    if( mpitModifierMenu )
    {
      delete mpitModifierMenu;
    }
    mpitModifierMenu = new QPopupMenu(this);
    mpitModifierMenu->insertTearOffHandle();
    connect(mpitModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(mpitModifierSelected(int)) );

    generateModifierMenu(mpitModifierMenu, list_of_mpit_modifiers, current_list_of_mpit_modifiers);
    mpi_menu->insertItem(QString("Select mpit details:"), mpitModifierMenu);
  
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( mpitModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(MPItraceFLAG);
    qaction->setOn(MPItraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(MPItraceSelected()) );
  } else {
#ifdef DEBUG_StatsPanel
       printf("generateMPIMenu(%s) for mpiotf\n", collectorName.ascii() );
#endif
    // generate mpiotf menu
  }

}

void
StatsPanel::generatePCSAMPmodifiers()
{
  list_of_pcsamp_modifiers.clear();
  list_of_pcsamp_modifiers.push_back("pcsamp::time");
  list_of_pcsamp_modifiers.push_back("pcsamp::percent");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadAverage");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadMin");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadMax");
}

void
StatsPanel::generateUSERTIMEmodifiers()
{
  list_of_usertime_modifiers.clear();
  list_of_usertime_modifiers.push_back("usertime::exclusive_times");
  list_of_usertime_modifiers.push_back("usertime::inclusive_times");
  list_of_usertime_modifiers.push_back("usertime::percent");
  list_of_usertime_modifiers.push_back("usertime::count");
  list_of_usertime_modifiers.push_back("usertime::ThreadAverage");
  list_of_usertime_modifiers.push_back("usertime::ThreadMin");
  list_of_usertime_modifiers.push_back("usertime::ThreadMax");
}

void
StatsPanel::generateOMPTPmodifiers()
{
  list_of_omptp_modifiers.clear();
  list_of_omptp_modifiers.push_back("omptp::exclusive_times");
  list_of_omptp_modifiers.push_back("omptp::inclusive_times");
  list_of_omptp_modifiers.push_back("omptp::percent");
  list_of_omptp_modifiers.push_back("omptp::count");
  list_of_omptp_modifiers.push_back("omptp::ThreadAverage");
  list_of_omptp_modifiers.push_back("omptp::ThreadMin");
  list_of_omptp_modifiers.push_back("omptp::ThreadMax");
}

void
StatsPanel::generateMPIPmodifiers()
{
  list_of_mpip_modifiers.clear();
  list_of_mpip_modifiers.push_back("mpip::exclusive_times");
  list_of_mpip_modifiers.push_back("mpip::inclusive_times");
  list_of_mpip_modifiers.push_back("mpip::percent");
  list_of_mpip_modifiers.push_back("mpip::count");
  list_of_mpip_modifiers.push_back("mpip::ThreadAverage");
  list_of_mpip_modifiers.push_back("mpip::ThreadMin");
  list_of_mpip_modifiers.push_back("mpip::ThreadMax");
}

void
StatsPanel::generateIOPmodifiers()
{
  list_of_iop_modifiers.clear();
  list_of_iop_modifiers.push_back("iop::exclusive_times");
  list_of_iop_modifiers.push_back("iop::inclusive_times");
  list_of_iop_modifiers.push_back("iop::percent");
  list_of_iop_modifiers.push_back("iop::count");
  list_of_iop_modifiers.push_back("iop::ThreadAverage");
  list_of_iop_modifiers.push_back("iop::ThreadMin");
  list_of_iop_modifiers.push_back("iop::ThreadMax");
}

void
StatsPanel::generateHWCmodifiers()
{
  list_of_hwc_modifiers.clear();
  list_of_hwc_modifiers.push_back("hwc::overflows");
  list_of_hwc_modifiers.push_back("hwc::counts");
  list_of_hwc_modifiers.push_back("hwc::percent");
  list_of_hwc_modifiers.push_back("hwc::ThreadAverage");
  list_of_hwc_modifiers.push_back("hwc::ThreadMin");
  list_of_hwc_modifiers.push_back("hwc::ThreadMax");
}

void
StatsPanel::generateHWCSAMPmodifiers()
{
#ifdef DEBUG_StatsPanel
  printf("ENTER StatsPanel::generateHWCSAMPmodifiers\n" );
#endif
  list_of_hwcsamp_modifiers.clear();
  list_of_hwcsamp_modifiers.push_back("hwcsamp::time");
  list_of_hwcsamp_modifiers.push_back("hwcsamp::allEvents");
  list_of_hwcsamp_modifiers.push_back("hwcsamp::percent");
  list_of_hwcsamp_modifiers.push_back("hwcsamp::ThreadAverage");
  list_of_hwcsamp_modifiers.push_back("hwcsamp::ThreadMin");
  list_of_hwcsamp_modifiers.push_back("hwcsamp::ThreadMax");

  // Now get the the metrics for hwcsamp to pull off the event names to add to the metrics list
  QString command = QString::null;
    
  if( focusedExpID == -1 ) {
     command = QString("list -v justparamvalues -x %1").arg(expID);
  } else {
     command = QString("list -v justparamvalues -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::generateHWCSAMPmodifiers, -attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_collectors_paramsval.clear();
  list_of_generic_modifiers.clear();
  InputLineObject *clip = NULL;
  std::string cstring;
  std::string lastToken;


  if( !cli->getStringValueFromCLI( (char *)command.ascii(), &cstring, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::generateHWCSAMPmodifiers -ran %s, result=%s\n", command.ascii(), cstring.c_str() );
#endif
  std::vector<std::string> tokens;
  createTokens(cstring, tokens, ",");
  std::vector<std::string>::iterator k;

  for (k=tokens.begin();k != tokens.end(); k++) {
     lastToken = *k;
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::generateHWCSAMPmodifiers list_of_hwcsamp_modifiers.push_back, lastToken=%s\n", lastToken.c_str() );
#endif
     list_of_hwcsamp_modifiers.push_back(lastToken.c_str());
  }
  for( std::list<std::string>::const_iterator it = list_of_hwcsamp_modifiers.begin();
       it != list_of_hwcsamp_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::generateHWCSAMPmodifiers LOOP THROUGH, list_of_hwcsamp_modifiers, modifier=%s\n", modifier.c_str() );
#endif
     it++;
  }

//
}


void
StatsPanel::generateHWCTIMEmodifiers()
{
  list_of_hwctime_modifiers.clear();
  list_of_hwctime_modifiers.push_back("hwctime::exclusive_counts");
  list_of_hwctime_modifiers.push_back("hwctime::exclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::inclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::inclusive_counts");
  list_of_hwctime_modifiers.push_back("hwctime::percent");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadAverage");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadMin");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadMax");
}

void
StatsPanel::generateIOmodifiers()
{
    list_of_io_modifiers.clear();
    list_of_io_modifiers.push_back("io::exclusive_times");
    list_of_io_modifiers.push_back("min");
    list_of_io_modifiers.push_back("max");
    list_of_io_modifiers.push_back("average");
    list_of_io_modifiers.push_back("count");
    list_of_io_modifiers.push_back("percent");
    list_of_io_modifiers.push_back("stddev");
    list_of_io_modifiers.push_back("ThreadAverage");
    list_of_io_modifiers.push_back("ThreadMin");
    list_of_io_modifiers.push_back("ThreadMax");
}

void
StatsPanel::generateMEMmodifiers()
{
    list_of_mem_modifiers.clear();
    list_of_mem_modifiers.push_back("mem::exclusive_times");
    list_of_mem_modifiers.push_back("min");
    list_of_mem_modifiers.push_back("max");
    list_of_mem_modifiers.push_back("average");
    list_of_mem_modifiers.push_back("count");
    list_of_mem_modifiers.push_back("percent");
    list_of_mem_modifiers.push_back("stddev");
    list_of_mem_modifiers.push_back("ThreadAverage");
    list_of_mem_modifiers.push_back("ThreadMin");
    list_of_mem_modifiers.push_back("ThreadMax");
}

void
StatsPanel::generatePTHREADSmodifiers()
{
    list_of_pthreads_modifiers.clear();
    list_of_pthreads_modifiers.push_back("pthreads::exclusive_times");
    list_of_pthreads_modifiers.push_back("min");
    list_of_pthreads_modifiers.push_back("max");
    list_of_pthreads_modifiers.push_back("average");
    list_of_pthreads_modifiers.push_back("count");
    list_of_pthreads_modifiers.push_back("percent");
    list_of_pthreads_modifiers.push_back("stddev");
    list_of_pthreads_modifiers.push_back("ThreadAverage");
    list_of_pthreads_modifiers.push_back("ThreadMin");
    list_of_pthreads_modifiers.push_back("ThreadMax");
}

void
StatsPanel::generateIOTmodifiers()
{
    list_of_iot_modifiers.clear();
    list_of_iot_modifiers.push_back("iot::exclusive_times");
    list_of_iot_modifiers.push_back("iot::inclusive_times");
    list_of_iot_modifiers.push_back("min");
    list_of_iot_modifiers.push_back("max");
    list_of_iot_modifiers.push_back("average");
    list_of_iot_modifiers.push_back("count");
    list_of_iot_modifiers.push_back("percent");
    list_of_iot_modifiers.push_back("stddev");
    list_of_iot_modifiers.push_back("start_time");
    list_of_iot_modifiers.push_back("stop_time");
    list_of_iot_modifiers.push_back("syscallno");
    list_of_iot_modifiers.push_back("nsysargs");
    list_of_iot_modifiers.push_back("retval");
#if PATHNAME_READY
    list_of_iot_modifiers.push_back("pathname");
#endif
}

void
StatsPanel::generateMPImodifiers()
{
//  list_of_mpi_modifiers.push_back("mpi::exclusive_details");
//  list_of_mpi_modifiers.push_back("mpi::inclusive_details");
//
    list_of_mpi_modifiers.clear();
    list_of_mpi_modifiers.push_back("mpi::exclusive_times");
    list_of_mpi_modifiers.push_back("mpi::inclusive_times");
    list_of_mpi_modifiers.push_back("min");
    list_of_mpi_modifiers.push_back("max");
    list_of_mpi_modifiers.push_back("average");
    list_of_mpi_modifiers.push_back("count");
    list_of_mpi_modifiers.push_back("percent");
    list_of_mpi_modifiers.push_back("stddev");
}

void
StatsPanel::generateMPITmodifiers()
{
//  list_of_mpit_modifiers.push_back("mpit::exclusive_details");
//  list_of_mpit_modifiers.push_back("mpit::inclusive_details");
//
    list_of_mpit_modifiers.push_back("mpit::exclusive_times");
    list_of_mpit_modifiers.push_back("mpit::inclusive_times");
    list_of_mpit_modifiers.push_back("min");
    list_of_mpit_modifiers.push_back("max");
    list_of_mpit_modifiers.push_back("average");
    list_of_mpit_modifiers.push_back("count");
    list_of_mpit_modifiers.push_back("percent");
    list_of_mpit_modifiers.push_back("stddev");
    list_of_mpit_modifiers.push_back("start_time");
    list_of_mpit_modifiers.push_back("stop_time");
    list_of_mpit_modifiers.push_back("source");
    list_of_mpit_modifiers.push_back("dest");
    list_of_mpit_modifiers.push_back("size");
    list_of_mpit_modifiers.push_back("tag");
    list_of_mpit_modifiers.push_back("communicator");
    list_of_mpit_modifiers.push_back("datatype");
    list_of_mpit_modifiers.push_back("retval");
}

void
StatsPanel::generateFPEmodifiers()
{
  list_of_fpe_modifiers.clear();
  list_of_fpe_modifiers.push_back("fpe::time");
  list_of_fpe_modifiers.push_back("fpe::counts");
  list_of_fpe_modifiers.push_back("fpe::inclusive_counts");
  list_of_fpe_modifiers.push_back("fpe::percent");
  list_of_fpe_modifiers.push_back("fpe::ThreadAverage");
  list_of_fpe_modifiers.push_back("fpe::ThreadMin");
  list_of_fpe_modifiers.push_back("fpe::ThreadMax");
  list_of_fpe_modifiers.push_back("fpe::inexact_result_count");
  list_of_fpe_modifiers.push_back("fpe::underflow_count");
  list_of_fpe_modifiers.push_back("fpe::overflow_count");
  list_of_fpe_modifiers.push_back("fpe::division_by_zero_count");
  list_of_fpe_modifiers.push_back("fpe::unnormal_count");
  list_of_fpe_modifiers.push_back("fpe::invalid_count");
  list_of_fpe_modifiers.push_back("fpe::unknown_count");
}

void
StatsPanel::generateIOMenu(QString collectorName)
{
#ifdef DEBUG_StatsPanel
  printf("generateIOMenu(%s)\n", collectorName.ascii() );
#endif
  io_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  io_menu->insertSeparator();

  if( collectorName == "io" ) {

    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOReportSelected(int)) );

    if( focusedExpID != -1 ) {
      contextMenu->insertItem(QString("Display Options: (Exp: %1) IO").arg(focusedExpID), io_menu);
    } else {
      contextMenu->insertItem(QString("Display Options: IO"), io_menu);
    }

    // Build the static list of io modifiers.

    generateIOmodifiers();
  
    if( ioModifierMenu ) {
      delete ioModifierMenu;
    }
    ioModifierMenu = new QPopupMenu(this);
    ioModifierMenu->insertTearOffHandle();

    connect(ioModifierMenu, SIGNAL( activated(int) ), this, SLOT(ioModifierSelected(int)) );

    generateModifierMenu(ioModifierMenu, list_of_io_modifiers, current_list_of_io_modifiers);

    io_menu->insertItem(QString("Select io details:"), ioModifierMenu);
    io_menu->setCheckable(TRUE);
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( ioModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(IOtraceFLAG);
    qaction->setOn(IOtraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(IOtraceSelected()) );
  } else {

#ifdef DEBUG_StatsPanel
  printf("generateIOTMenu(%s)\n", collectorName.ascii() );
#endif

    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ), this, SLOT(collectorIOTReportSelected(int)) );

    if( focusedExpID != -1 ) {
      contextMenu->insertItem(QString("Display Options: (Exp: %1) IOT").arg(focusedExpID), io_menu);
    } else {
      contextMenu->insertItem(QString("Display Options: IOT"), io_menu);
    }

    // Build the static list of iot modifiers.
    generateIOTmodifiers();

    if( iotModifierMenu ) {
      delete iotModifierMenu;
    }

    iotModifierMenu = new QPopupMenu(this);
    iotModifierMenu->insertTearOffHandle();
    connect(iotModifierMenu, SIGNAL( activated(int) ), this, SLOT(iotModifierSelected(int)) );

    generateModifierMenu(iotModifierMenu, list_of_iot_modifiers, current_list_of_iot_modifiers);
    io_menu->insertItem(QString("Select iot details:"), iotModifierMenu);

    io_menu->setCheckable(TRUE);
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( iotModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(IOtraceFLAG);
    qaction->setOn(IOtraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(IOtraceSelected()) );
  }

// printf("We made an io_menu!!\n");


}


void
StatsPanel::generateHWCMenu(QString collectorName)
{
// printf("Collector hwc_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwc_menu = new QPopupMenu(this);

  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) hwc").arg(focusedExpID), hwc_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: hwc"), hwc_menu);
  }

  addHWCReports(hwc_menu);
  connect(hwc_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCReportSelected(int)) );

  generateHWCmodifiers();
  
  if( hwcModifierMenu )
  {
    delete hwcModifierMenu;
  }

  hwcModifierMenu = new QPopupMenu(this);
  hwcModifierMenu->insertTearOffHandle();
  connect(hwcModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(hwcModifierSelected(int)) );
  generateModifierMenu(hwcModifierMenu, list_of_hwc_modifiers, current_list_of_hwc_modifiers);
  hwc_menu->insertItem(QString("Select hwc details:"), hwcModifierMenu);
}


void
StatsPanel::generateHWCSampMenu(QString collectorName)
{
// printf("Collector hwcsamp_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwcsamp_menu = new QPopupMenu(this);

  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) hwcsamp").arg(focusedExpID), hwcsamp_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: hwcsamp"), hwcsamp_menu);
  }

  addHWCSampReports(hwcsamp_menu);
  connect(hwcsamp_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCSampReportSelected(int)) );
  generateHWCSAMPmodifiers();

  if( hwcsampModifierMenu ) {
    delete hwcsampModifierMenu;
  }
  hwcsampModifierMenu = new QPopupMenu(this);
  hwcsampModifierMenu->insertTearOffHandle();
  connect(hwcsampModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(hwcsampModifierSelected(int)) );
  generateModifierMenu(hwcsampModifierMenu, list_of_hwcsamp_modifiers, current_list_of_hwcsamp_modifiers);
  hwcsamp_menu->insertItem(QString("Select hwcsamp details:"), hwcsampModifierMenu);
}


void
StatsPanel::generateHWCTimeMenu(QString collectorName)
{
// printf("Collector hwctime_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwctime_menu = new QPopupMenu(this);

  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) hwctime").arg(focusedExpID), hwctime_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: hwctime"), hwctime_menu);
  }

  addHWCTimeReports(hwctime_menu);
  connect(hwctime_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCTimeReportSelected(int)) );
  generateHWCTIMEmodifiers();

  if( hwctimeModifierMenu )
  {
    delete hwctimeModifierMenu;
  }
  hwctimeModifierMenu = new QPopupMenu(this);
  hwctimeModifierMenu->insertTearOffHandle();
  connect(hwctimeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(hwctimeModifierSelected(int)) );
  generateModifierMenu(hwctimeModifierMenu, list_of_hwctime_modifiers, current_list_of_hwctime_modifiers);
  hwctime_menu->insertItem(QString("Select hwctime details:"), hwctimeModifierMenu);
}
void
StatsPanel::generateOMPTPMenu()
{
// printf("Collector omptp_menu is being created\n");

  omptp_menu = new QPopupMenu(this);
  connect(omptp_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorOMPTPReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) OMPTP").arg(focusedExpID), omptp_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: OMPTP"), omptp_menu);
  }

  generateOMPTPmodifiers();

  if( omptpModifierMenu )
  {
    delete omptpModifierMenu;
  }
  omptpModifierMenu = new QPopupMenu(this);
  addOMPTPReports(omptp_menu);
  omptpModifierMenu->insertTearOffHandle();
  connect(omptpModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(omptpModifierSelected(int)) );
  generateModifierMenu(omptpModifierMenu, list_of_omptp_modifiers, current_list_of_omptp_modifiers);
  omptp_menu->insertItem(QString("Select omptp Metrics:"), omptpModifierMenu);
}


void
StatsPanel::generateUserTimeMenu()
{
// printf("Collector usertime_menu is being created\n");

  usertime_menu = new QPopupMenu(this);
  connect(usertime_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorUserTimeReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) UserTime").arg(focusedExpID), usertime_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: UserTime"), usertime_menu);
  }

  generateUSERTIMEmodifiers();

  if( usertimeModifierMenu )
  {
    delete usertimeModifierMenu;
  }
  usertimeModifierMenu = new QPopupMenu(this);
  addUserTimeReports(usertime_menu);
  usertimeModifierMenu->insertTearOffHandle();
  connect(usertimeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(usertimeModifierSelected(int)) );
  generateModifierMenu(usertimeModifierMenu, list_of_usertime_modifiers, current_list_of_usertime_modifiers);
  usertime_menu->insertItem(QString("Select usertime Metrics:"), usertimeModifierMenu);
}

void
StatsPanel::generateIOPMenu()
{
// printf("Collector iop_menu is being created\n");

  iop_menu = new QPopupMenu(this);
  connect(iop_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOPReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) IOP").arg(focusedExpID), iop_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: IOP"), iop_menu);
  }

  generateIOPmodifiers();

  if( iopModifierMenu )
  {
    delete iopModifierMenu;
  }
  iopModifierMenu = new QPopupMenu(this);
  addIOPReports(iop_menu);
  iopModifierMenu->insertTearOffHandle();
  connect(iopModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(iopModifierSelected(int)) );
  generateModifierMenu(iopModifierMenu, list_of_iop_modifiers, current_list_of_iop_modifiers);
  iop_menu->insertItem(QString("Select iop Metrics:"), iopModifierMenu);
}

void
StatsPanel::generateMEMMenu(QString collectorName)
{
// printf("Collector mem_menu is being created\n");

  mem_menu = new QPopupMenu(this);
  connect(mem_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMEMReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) MEM").arg(focusedExpID), mem_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: MEM"), mem_menu);
  }

  generateMEMmodifiers();

  if( memModifierMenu )
  {
    delete memModifierMenu;
  }
  memModifierMenu = new QPopupMenu(this);
  addMEMReports(mem_menu);
  memModifierMenu->insertTearOffHandle();
  connect(memModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(memModifierSelected(int)) );
  generateModifierMenu(memModifierMenu, list_of_mem_modifiers, current_list_of_mem_modifiers);
  mem_menu->insertItem(QString("Select mem Metrics:"), memModifierMenu);
}

void
StatsPanel::generateMPIPMenu()
{
// printf("Collector mpip_menu is being created\n");

  mpip_menu = new QPopupMenu(this);
  connect(mpip_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPIPReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) MPIP").arg(focusedExpID), mpip_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: MPIP"), mpip_menu);
  }

  generateMPIPmodifiers();

  if( mpipModifierMenu )
  {
    delete mpipModifierMenu;
  }
  mpipModifierMenu = new QPopupMenu(this);
  addMPIPReports(mpip_menu);
  mpipModifierMenu->insertTearOffHandle();
  connect(mpipModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(mpipModifierSelected(int)) );
  generateModifierMenu(mpipModifierMenu, list_of_mpip_modifiers, current_list_of_mpip_modifiers);
  mpip_menu->insertItem(QString("Select mpip Metrics:"), mpipModifierMenu);
}

void
StatsPanel::generatePCSampMenu()
{
// printf("Collector pcsamp_menu is being created\n");

  pcsamp_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;


  addPCSampReports(pcsamp_menu);
  connect(pcsamp_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorPCSampReportSelected(int)) );
  
  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) pcsamp").arg(focusedExpID), pcsamp_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: pcsamp"), pcsamp_menu);
  }

  generatePCSAMPmodifiers();

  if( pcsampModifierMenu )
  {
    delete pcsampModifierMenu;
  }

  pcsampModifierMenu = new QPopupMenu(this);
  pcsampModifierMenu->insertTearOffHandle();
  connect(pcsampModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(pcsampModifierSelected(int)) );
  generateModifierMenu(pcsampModifierMenu, list_of_pcsamp_modifiers, current_list_of_pcsamp_modifiers);
  pcsamp_menu->insertItem(QString("Select pcsamp Metrics:"), pcsampModifierMenu);
}


void
StatsPanel::generateFPEMenu()
{
// printf("Collector fpe_menu is being created\n");

  fpe_menu = new QPopupMenu(this);
  connect(fpe_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorFPEReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 ) {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) FPE").arg(focusedExpID), fpe_menu);
  } else {
    contextMenu->insertItem(QString("Display Options: FPE"), fpe_menu);
  }

  generateFPEmodifiers();

  if( fpeModifierMenu )
  {
    delete fpeModifierMenu;
  }
  fpeModifierMenu = new QPopupMenu(this);
  addFPEReports(fpe_menu);
  fpeModifierMenu->insertTearOffHandle();
  connect(fpeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(fpeModifierSelected(int)) );
  generateModifierMenu(fpeModifierMenu, list_of_fpe_modifiers, current_list_of_fpe_modifiers);
  fpe_menu->insertItem(QString("Select fpe Metrics:"), fpeModifierMenu);
}


void
StatsPanel::generateGenericMenu()
{
// printf("generateGenericMenu is being created\n");

  generic_menu = new QPopupMenu(this);
  connect(generic_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorGenericReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;

  generic_menu->insertItem(QString("View Stats:"));

  contextMenu->insertItem(QString("Select %1 Metrics:").arg(currentCollectorStr), generic_menu);

//  list_of_generic_modifiers.clear();
  
  if( genericModifierMenu )
  {
    delete genericModifierMenu;
  }
  genericModifierMenu = new QPopupMenu(this);
  genericModifierMenu->insertTearOffHandle();
  connect(genericModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(genericModifierSelected(int)) );
  generateModifierMenu(genericModifierMenu, list_of_generic_modifiers, current_list_of_generic_modifiers);

// printf("Try to generate the genericModifierMenu()\n");

  generic_menu->insertItem(QString("Select %1 Metrics:").arg(currentCollectorStr)
, genericModifierMenu);
}


#if 0
static void
debugList(QListView *splv)
{
// Debug print
SPListViewItem *top = (SPListViewItem *)splv->firstChild();
  printf("Debug:\n");
while( top )
{
  printf("  %s, %s", top->text(0).ascii(), top->text(fieldCount-1).ascii() );
  SPListViewItem *level1 = (SPListViewItem *)top->firstChild();
  while( level1 )
  {
    printf("  --%s, %s", level1->text(0).ascii(), level1->text(fieldCount-1).ascii() );

    SPListViewItem *level2 = (SPListViewItem *)level1->firstChild();
    while( level2 )
    {
      printf("  ----%s, %s", level2->text(0).ascii(), level2->text(fieldCount-1).ascii() );
  
      SPListViewItem *level3 = (SPListViewItem *)level2->firstChild();
      while( level3 )
      {
        printf("  ------%s, %s", level3->text(0).ascii(), level3->text(fieldCount-1).ascii() );
        SPListViewItem *level4 = (SPListViewItem *)level3->firstChild();
        while( level4 )
        {
          printf("  --------%s, %s", level4->text(0).ascii(), level4->text(fieldCount-1).ascii() );
          SPListViewItem *level5 = (SPListViewItem *)level4->firstChild();
          while( level5 )
          {
            printf("  ----------%s, %s", level5->text(0).ascii(), level5->text(fieldCount-1).ascii() );
        
            level5 = (SPListViewItem *)level5->nextSibling();
          }
      
          level4 = (SPListViewItem *)level4->nextSibling();
        }
    
        level3 = (SPListViewItem *)level3->nextSibling();
      }

      level2 = (SPListViewItem *)level2->nextSibling();
    }

    level1 = (SPListViewItem *)level1->nextSibling();
  }
  
  top = (SPListViewItem *)top->nextSibling();
}
  printf("End Debug\n");
}
// endif Debug
#endif // 0

void
StatsPanel::addMPIReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for MPI Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to MPI Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to MPI Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each MPI Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show Call Trees, with full stacks, to each MPI Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
#endif // PULL
}

void
StatsPanel::addIOReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for IO Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to IO Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each IO Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
#endif // PULL
}

void
StatsPanel::addMEMReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for MEM Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to MEM Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to MEM Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to MEM functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each MEM Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
#endif // PULL
}


void
StatsPanel::addPTHREADSReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for PTHREADS Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to PTHREADS Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to PTHREADS Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to POSIX thread functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each PTHREADS Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to POSIX thread routine for selected function."));
#endif // PULL
}

void
StatsPanel::addOMPTPReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}

void
StatsPanel::addUserTimeReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}


void
StatsPanel::addIOPReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}


void
StatsPanel::addPCSampReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show LinkedObjects.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show Statements.") );

  qaction = new QAction(this, "showStatementsByFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements by Function") );
  qaction->setToolTip(tr("Show timings for statements by function. Select line containing function first."));

#if defined(HAVE_DYNINST)
  qaction = new QAction(this, "showLoops");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Loops") );
  qaction->setToolTip(tr("Show Loops.") );
#endif
}


void
StatsPanel::addFPEReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

  qaction = new QAction(this, "showStatementsByFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements by Function") );
  qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}

void
StatsPanel::addHWCReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show by Functions.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show by Statements.") );

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );

#if defined(HAVE_DYNINST)
  qaction = new QAction(this, "showLoops");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Loops") );
  qaction->setToolTip(tr("Show by Loops.") );
#endif
}

void
StatsPanel::addHWCSampReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show by Functions.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show by Statements.") );

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );

#if defined(HAVE_DYNINST)
  qaction = new QAction(this, "showLoops");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Loops") );
  qaction->setToolTip(tr("Show by Loops.") );
#endif
}

void
StatsPanel::addHWCTimeReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show by Functions.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show by Statements.") );

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );

#if defined(HAVE_DYNINST)
  qaction = new QAction(this, "showLoops");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Loops") );
  qaction->setToolTip(tr("Show by Loops.") );
#endif

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show by CallTrees.") );

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show by TraceBacks.") );

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTraceBacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show trace backs, with full stacks, to Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}



SourceObject *
StatsPanel::lookUpFileHighlights(QString filename, QString lineNumberStr, HighlightList *highlightList, QString highlightMetricStr)
{
  SourceObject *spo = NULL;
  HighlightObject *hlo = NULL;
  int localExpID = -1;

  nprintf(DEBUG_PANELS) ("lookUpFileHighlights: filename=(%s) lineNumberStr=(%s)\n", filename.ascii(), lineNumberStr.ascii() );

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, lfhA: expID=%d focusedExpID=%d\n", expID, focusedExpID );
  printf("StatsPanel::lookUpFileHighlights, localExpID=%d, highlightMetricStr=%s\n", localExpID, highlightMetricStr.ascii() );
  printf("StatsPanel::lookUpFileHighlights, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
  printf("StatsPanel::lookUpFileHighlights, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
  printf("StatsPanel::lookUpFileHighlights, timeIntervalString=(%s)\n", timeIntervalString.ascii() );
#endif

  QString command = QString::null;

  QFileInfo qfi(filename);
  QString _fileName  = qfi.fileName();


  QString fullPathFilename  = filename;
  QString fn  = filename;
  int basename_index = filename.findRev("/");
  if( basename_index != -1 ) {
    fn =  filename.right((filename.length()-basename_index)-1);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, file BaseName=(%s), highlightMetricStr.ascii()=%s\n", fn.ascii(), highlightMetricStr.ascii());
#endif

// test jeg  highlightMetricStr = "PAPI_TOT_CYC";
  if( highlightMetricStr.isEmpty() ) {

    if( _fileName.isEmpty() ) {
      return(spo);
    }

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::lookUpFileHighlights, lfhB: expID=%d focusedExpID=%d\n", 
            expID, focusedExpID );
#endif

    if( expID > 0 ) {


      if (focusedExpID != -1) {
        // use compare id instead of main experiment id for these commands
        localExpID = focusedExpID;
      } else {
        localExpID = expID;
      }

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::lookUpFileHighlights, localExpID=%d, highlightMetricStr=%s\n", localExpID, highlightMetricStr.ascii() );
#endif
      // Trace experiments do not have Statements metrics! why?
      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
          currentCollectorStr == "mem" ||
          currentCollectorStr == "pthreads" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {

        if (currentUserSelectedReportStr.startsWith("Loops" ) ) {
          command = QString("expView -x %1 -v loops -f %2 %3").arg(localExpID).arg(fn).arg(timeIntervalString);
        } else {
          command = QString("expView -x %1 -f %2 %3").arg(localExpID).arg(fn).arg(timeIntervalString);
        }
      } else {
        if (currentUserSelectedReportStr.startsWith("Loops" ) ) {
          command = QString("expView -x %1 -v loops -f %2 %3").arg(localExpID).arg(fn).arg(timeIntervalString);
        } else {
          command = QString("expView -x %1 -v statements -f %2 %3").arg(localExpID).arg(fn).arg(timeIntervalString);
        }
      }
    } else {

      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
          currentCollectorStr == "mem" ||
          currentCollectorStr == "pthreads" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {
          if (currentUserSelectedReportStr.startsWith("Loops" ) ) {
            command = QString("expView -x %1 -v loops -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
          } else {
            command = QString("expView -x %1 -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
          }
      } else {
          if (currentUserSelectedReportStr.startsWith("Loops" ) ) {
            command = QString("expView -x %1 -v loops -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
          } else {
            command = QString("expView -x %1 -v statements -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
          } 
      }

    }

  } else {

    if( expID > 0 ) {

        if (focusedExpID != -1) {
          // use compare id instead of main experiment id for these commands
          localExpID = focusedExpID;
        } else {
          localExpID = expID;
        }

        if (currentCollectorStr == "io" || 
            currentCollectorStr == "iot" ||
            currentCollectorStr == "mem" ||
            currentCollectorStr == "pthreads" ||
	    currentCollectorStr == "mpi" || 
            currentCollectorStr == "mpit" ) {

            command = QString("expView -x %1 -f %2 -m %3 %4").arg(localExpID).arg(fn).arg(highlightMetricStr).arg(timeIntervalString);

#ifdef DEBUG_StatsPanel
            printf("StatsPanel::lookUpFileHighlights, 33, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif

      } else {

#ifdef DEBUG_StatsPanel
            printf("StatsPanel::lookUpFileHighlights2, 33, lastCommand=(%s)\n", lastCommand.ascii() );
#endif

            int newExpId = expID;
            if( lastCommand.startsWith("cview -c") ) {

#ifdef DEBUG_StatsPanel
               printf("StatsPanel::lookUpFileHighlights2, 33, dosomething here, lastCommand=(%s)\n", lastCommand.ascii() );
#endif

              if (compareExpIDs.size() == 1) {
                   localExpID = compareExpIDs[0];
                   newExpId = compareExpIDs[0];
#ifdef DEBUG_StatsPanel
                   printf("StatsPanel::lookUpFileHighlights2, 33, one experiment , localExpID=(%d)\n", localExpID );
#endif
              } else if (compareExpIDs.size()  > 1) {

                  // We need to look up the full path name for each experiment and store the results in the global/class 
                  // vector structure for the full path source filename before calling getValidExperimentForView
                  // We need the full path to the source file.  So, generate the proper command to get that filename if we don't already have a path
                  for( std::vector<int>::const_iterator it = compareExpIDs.begin(); it != compareExpIDs.end(); it++ ) {
                      int tmp_exp_id = *it;
                      QString tmpFullPathFilename = getFullPathSrcFileName(fullPathFilename, tmp_exp_id);
                      compareSrcFilenames.push_back (tmpFullPathFilename);
                  }
//  int basename_index = srcFilename.findRev("/");
//  if( basename_index == -1 ) {
     // Use the non-fullpath filename as an argument to get the full pathname filename
//     yyyyygetFullPathSrcFileName(srcFilename, 
//  } 
  
  
                  
                   localExpID = getValidExperimentIdForView();    
#ifdef DEBUG_StatsPanel
                   printf("StatsPanel::lookUpFileHighlights2, 33, more than one experiment , localExpID=(%d), filename=%s, fn=%s\n", localExpID, filename.ascii(), fn.ascii() );
#endif
              }

#ifdef DEBUG_StatsPanel
               printf("StatsPanel::lookUpFileHighlights2, 33, dosomething here, newExpId=%d, focusedExpID=%d, localExpID=%d\n", 
                       newExpId, focusedExpID, localExpID);
#endif
          }

            if( fn.isEmpty() ) {
               command = QString("expView -x %1 -v statements -m %3 %4").arg(localExpID).arg(highlightMetricStr).arg(timeIntervalString);
            } else {
               command = QString("expView -x %1 -v statements -f %2 -m %3 %4").arg(localExpID).arg(fn).arg(highlightMetricStr).arg(timeIntervalString);
            } 

#ifdef DEBUG_StatsPanel
            printf("StatsPanel::lookUpFileHighlights2, 77jeg, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif

      } // end else not io, iot, mpi, mpit

    } else { // start else clause for expID > 0 check

        if (focusedExpID != -1) {
          // use compare id instead of main experiment id for these commands
          localExpID = focusedExpID;
        } 

        int newExpId = -1;

        if( lastCommand.startsWith("cview -c") ) {

#ifdef DEBUG_StatsPanel
            printf("StatsPanel::lookUpFileHighlights2, 44, dosomething here, lastCommand=(%s)\n", lastCommand.ascii() );
#endif
              if (compareExpIDs.size() == 1) {
                   localExpID = compareExpIDs[0];
                   newExpId = compareExpIDs[0];

#ifdef DEBUG_StatsPanel
                   printf("StatsPanel::lookUpFileHighlights2, 44, one experiment , localExpID=(%d)\n", localExpID );
#endif

              } else if (compareExpIDs.size()  > 1) {

                   // We need to look up the full path name for each experiment and store the results in the global/class 
                   // vector structure for the full path source filename before calling getValidExperimentForView
                   // We need the full path to the source file.  So, generate the proper command to get that filename if we don't already have a path
                   for( std::vector<int>::const_iterator it = compareExpIDs.begin(); it != compareExpIDs.end(); it++ ) {
                      int tmp_exp_id = *it;
                      QString tmpFullPathFilename = getFullPathSrcFileName(fullPathFilename, tmp_exp_id);
                      compareSrcFilenames.push_back (tmpFullPathFilename);
                   }
                   localExpID = getValidExperimentIdForView();    
                   newExpId = localExpID;
#ifdef DEBUG_StatsPanel
                   printf("StatsPanel::lookUpFileHighlights2, 44, more than one experiment , localExpID=(%d)\n", localExpID );
#endif
              }


#ifdef DEBUG_StatsPanel
            printf("StatsPanel::lookUpFileHighlights2, 44, dosomething here, newExpId=%d, focusedExpID=%d, localExpID=%d\n", 
                    newExpId, focusedExpID, localExpID);
#endif
          }

      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
          currentCollectorStr == "mem" ||
          currentCollectorStr == "pthreads" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {

          command = QString("expView -x %1 -f %2 -m %3 %4").arg(localExpID).arg(fn).arg(highlightMetricStr).arg(timeIntervalString);

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights, 44, fn=%s, command=(%s), lastCommand=(%s)\n", 
                  fn.ascii(), command.ascii(), lastCommand.ascii() );
#endif

      } else {

          command = QString("expView -x %1 -v statements -f %2 -m %3 %4").arg(localExpID).arg(fn).arg(highlightMetricStr).arg(timeIntervalString);

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights2, 44, fn=%s, command=(%s), lastCommand=(%s)\n", 
                  fn.ascii(), command.ascii(), lastCommand.ascii() );
#endif

      }
    } // end else clause for expID > 0 check
  }

  if( !currentThreadsStr.isEmpty() ) {
    command += QString(" %1").arg(currentThreadsStr);
  }

  bool cached_clip_processing = false;
  InputLineObject *clip;

#ifdef DEBUG_StatsPanel_cache
  printf("StatsPanel::lookUpFileHighlights, about to issue a command via cli->run_Append_Input_String(), command=(%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  clip = check_for_existing_clip(command.ascii());

  if (clip) {

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:lookUpFileHighlights, FOUND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE, command=" << command << std::endl;
#endif

    // call process_clip here?
    // maybe set some flags to skip the wait loop below
    cached_clip_processing = true;

  } else {

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:lookUpFileHighlights, DID NOT FIND AN EXISTING CLIP IN THE CLIP MAP STRUCTURE" 
               << ", ADDING CLIP AFTER ISSUING COMMAND, command=" << command << std::endl;
#endif

     clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());

     cached_clip_processing = false;

#ifdef DEBUG_StatsPanel_cache
     std::cerr << "StatsPanel:updateStatsPanelData, after issuing command, clip=" << clip 
               << " sizeof(&clip)=" << sizeof(&clip) << std::endl;
#endif
     addClipForThisCommand(command.ascii(), clip);

  }


  if( clip == NULL ) {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
  }

  Input_Line_Status status = ILO_UNKNOWN;

  if (!cached_clip_processing ) {
    while( !clip->Semantics_Complete() ) {
      status = cli->checkStatus(clip, command);
      if( !status || status == ILO_ERROR )
      { // An error occurred.... A message should have been posted.. return;
        QApplication::restoreOverrideCursor();
        if( clip ) {
          //clip->Set_Results_Used();
          clip = NULL;
        }
        break;
      }

      qApp->processEvents(4000);

      if( !cli->shouldWeContinue() )
      {
        QApplication::restoreOverrideCursor();
        if( clip ) {
          //clip->Set_Results_Used();
          clip = NULL;
        }
        break;
      }

      suspend();
      //sleep(1);
    }
  } // do not do if clip is cached

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, BEFORE calling process_clip, \n");
#endif

  process_clip(clip, highlightList, FALSE);

//  process_clip(clip, highlightList, TRUE);
//

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, AFTER calling process_clip, \n");
#endif

  clip->Set_Results_Used();
  clip = NULL;

  int lineNumber = lineNumberStr.toInt();
  QString value = QString::null;
  QString description = QString::null;
  QString value_description = QString::null;
  QString color = QString::null;
  HighlightObject *focusedHLO = NULL;

  for( HighlightList::Iterator it = highlightList->begin(); it != highlightList->end(); ++it) {
    hlo = (HighlightObject *)*it;
    if( hlo && focusedHLO == NULL ) {
      focusedHLO = hlo;
//      hlo->print();
    }

    if( value_description.isEmpty() ) {
      value_description = hlo->value_description;
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::lookUpFileHighlights, hlo->line=%d, lineNumber=%d\n", hlo->line, lineNumber);
#endif

    if( hlo->line == lineNumber ) {
      value = hlo->value;
      description = hlo->description;
      break;
    }
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, if true use hotToCold_color_names[2], value.isEmpty()=(%d)\n", value.isEmpty() );
  printf("StatsPanel::lookUpFileHighlights, lineNumberStr.ascii()=%s\n", lineNumberStr.ascii() );
#endif

  if( value.isEmpty() ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::lookUpFileHighlights, true is empty use hotToCold_color_names[2]=%s\n", hotToCold_color_names[2] );
#endif

    hlo = new HighlightObject(QString::null, filename, 
                              lineNumberStr.toInt(), 
                              hotToCold_color_names[2], 
                              ">>", 
                              "Callsite for this function", 
                              value_description);

  } else {

    int color_index = getLineColor((double)value.toDouble());

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::lookUpFileHighlights, false is NOT empty use value.ascii()=%s\n", value.ascii() );
    printf("StatsPanel::lookUpFileHighlights, false is NOT empty use hotToCold_color_names[color_index=%d]=%s\n", color_index, hotToCold_color_names[color_index] );
    printf("StatsPanel::lookUpFileHighlights, false is NOT empty use focusedHLO->fileName.ascii()=%s\n", focusedHLO->fileName.ascii() );
    printf("StatsPanel::lookUpFileHighlights, false is NOT empty use lineNumberStr.ascii()=%s\n", lineNumberStr.ascii() );
    printf("StatsPanel::lookUpFileHighlights, false is NOT empty use TotalTime=%f\n", TotalTime );
#endif

    highlightList->remove(hlo);
    hlo = new HighlightObject(QString::null, focusedHLO->fileName, 
                              lineNumberStr.toInt(), 
                              hotToCold_color_names[color_index], 
                              QString(">> %1").arg(value), 
                              QString("Callsite for this function.\n%1").arg(description), value_description);
  }

  highlightList->push_back(hlo);


#if DEBUG_StatsPanel
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    hlo->print();
  }
#endif // DEBUG_StatsPanel

  if( focusedHLO ) {

#if DEBUG_StatsPanel
    printf("lhfa: print focusedHLO:\n");
    focusedHLO->print();
#endif

    if( focusedHLO->fileName != filename ) {

      nprintf(DEBUG_PANELS) ("THE FILE NAMES %s != %s\n", focusedHLO->fileName.ascii(), filename.ascii() );

#if DEBUG_StatsPanel
      printf("lhfa: THE FILE NAMES %s != %s\n", focusedHLO->fileName.ascii(), filename.ascii() );
#endif

      if( !focusedHLO->fileName.isEmpty() ) {

	nprintf(DEBUG_PANELS) ("lhfa: focusedHLO->fileName.isEmpty, CHANGE THE FILENAME!!!\n");
#if DEBUG_StatsPanel
	printf("lhfa: focusedHLO->fileName.isEmpty, CHANGE THE FILENAME!!!\n");
#endif

        filename = focusedHLO->fileName;

      }
    }
  }

  int compareID = -1;
  if (groupID != expID) {
    compareID = expID;
  }
#if DEBUG_StatsPanel
  printf("lhfa: creating new source object with filename.ascii()=%s, expID=%d, compareID=%d\n", filename.ascii(), expID, compareID);
#endif
  spo = new SourceObject("functionName", filename.ascii(), lineNumberStr.toInt()-1, expID, TRUE, highlightList, compareID);

#if DEBUG_StatsPanel
  printf("lhfa: spo->fileName=(%s)\n", spo->fileName.ascii() );
#endif

  nprintf(DEBUG_PANELS) ("spo->fileName=(%s)\n", spo->fileName.ascii() );

#if DEBUG_StatsPanel
  printf("lhfa: calling spo->print, spo->fileName=(%s)\n", spo->fileName.ascii() );
// Begin debug
  spo->print();
// End debug
#endif // DEBUG_StatsPanel



  return spo;
}


void
StatsPanel::process_clip(InputLineObject *statspanel_clip, 
                         HighlightList *highlightList=NULL, 
                         bool dumpClipFLAG=FALSE)
{

#ifdef DEBUG_StatsPanel
    dumpClipFLAG = TRUE;
    printf("ENTER StatsPanel::process_clip,  &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  if( __internal_debug_setting & DEBUG_CLIPS ) {
    dumpClipFLAG = TRUE;
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::process_clip before NULL check &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  if( statspanel_clip == NULL ) {
    std::cerr << "No clip to process.\n";
  }

  QString valueStr = QString::null;
  QString xxxfileName = QString::null;
  QString xxxfuncName = QString::null;
  int xxxlineNumber = -1;
  HighlightObject *hlo = NULL;

  std::list<CommandObject *>::iterator coi;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::process_clip before NULL check &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip,statspanel_clip);
#endif

  coi = statspanel_clip->CmdObj_List().begin();

  CommandObject *co = (CommandObject *)(*coi);
  if( co == NULL ) {
    std::cerr << "No command object in clip to process.\n";
  }

  std::list<CommandResult *>::iterator cri;
  std::list<CommandResult *> cmd_result = co->Result_List();

#ifdef DEBUG_StatsPanel_reuse
  cri = cmd_result.begin();
  if (*cri) {
    std::cerr << "################ IN STATSPANEL::PROCESS_CLIP ###############, TYPE==(*cri)->Type()=" << (*cri)->Type() << std::endl;
  }
#endif


 // Process any annotations.
  bool issue_annotations = false;
  QString s;
  std::list<CommandResult_RawString *> cmd_annotation = co->Annotation_List ();
  for (std::list<CommandResult_RawString *>::iterator ari = cmd_annotation.begin();
       ari != cmd_annotation.end(); ari++) {
    if ((*ari)->Type() == CMD_RESULT_RAWSTRING) {
    }    

    std::string annotation_string = (*ari)->Form(0);
    QString vs = QString(annotation_string.c_str());

    if( vs.find("There were no data") >= 0 ) {
      // IF the string contains the no data samples message and there is no generated view then
      // check to see if there is a StatsPanel Message Label available for use
      //      IF so, hide the stats panel list widget and chart form
      //             and view the message label 
      //      
      if ( (cmd_result.begin() == cmd_result.end()) &&
           (sml != NULL) ) {
#ifdef DEBUG_StatsPanel
           printf("outputCLIData, show sml hide splv (sml != NULL)... = (%d)\n", (sml != NULL) );
#endif
          splv->hide();
          cf->hide();
          sml->show();
        continue;
      }
      try {
       // Need to create an 'info' panel for the no data samples mesage.
       // Save this message and output all of them at once.
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::process_clip, call outputCLIAnnotation: %s\n",annotation_string.c_str());
#endif
        if (!issue_annotations) {
          issue_annotations = true;
        } else if (!s.endsWith("\n")) {
          s += "\n";
        }
        s += vs;
      }
      catch(std::bad_alloc)
      { // Try to issue what might already have been placed in string.
        break;
      }
    }

  } // end - Process any annotations.
  if (issue_annotations) {
    // Create an 'info' panel for the no data samples messages.
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::process_clip, call outputCLIAnnotation %s\n",s.ascii());
#endif
    columnFieldList.push_back(s);
    outputCLIAnnotation( xxxfuncName, xxxfileName, xxxlineNumber );
    columnFieldList.clear();
  }

  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, TOP OF FOR cmd_result loop ----------------------------------\n");
#endif

    int skipFLAG = FALSE;
    if( dumpClipFLAG) std::cerr<< "DCLIP: " <<  "TYPE: " << (*cri)->Type() << "\n";
    if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, Here CMD_RESULT_COLUMN_VALUES:\n");
#endif

      std::list<CommandResult *> columns;
      CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
      ccp->Value(columns);

      std::list<CommandResult *>::iterator column_it;
      int i = 0;
      columnFieldList.clear();
      for (column_it = columns.begin(); column_it != columns.end(); column_it++)
      {
      
        if( dumpClipFLAG) {
          std::cerr << "DCLIP: " << (*column_it)->Form().c_str() << "\n";
        }
#ifdef DEBUG_StatsPanel
        std::cerr << "  " << (*column_it)->Form().c_str() << "\n";
#endif
        QString vs = (*column_it)->Form().c_str();
        columnFieldList.push_back(vs);

        CommandResult *cr = (CommandResult *)(*column_it);
        if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "cr->Type=" << cr->Type() << "\n";
        if( i == 0 && highlightList ) {
          valueStr = QString::null;
        }
        switch( cr->Type() )
        {
          case CMD_RESULT_NULL:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_NULL\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_UINT:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_UINT\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_INT:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_INT\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_FLOAT:
            {
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_FLOAT\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            }
            break;
          case CMD_RESULT_STRING:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_STRING\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_RAWSTRING:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_RAWSTRING\n";
            if( i == 0 && highlightList ) {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_FUNCTION:
          {
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_FUNCTION\n";
              CommandResult_Function *crf = (CommandResult_Function *)cr;
              std::string S = crf->getName();
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    S=" << S << "\n";
//            LinkedObject L = crf->getLinkedObject();
//            if( dumpClipFLAG) std::cerr << "    L.getPath()=" << L.getPath() << "\n";

              std::set<Statement> T = crf->getDefinitions();
              if( T.size() > 0 ) {
                std::set<Statement>::const_iterator ti = T.begin();
                Statement s = *ti;
                if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    s.getPath()=" << s.getPath() << "\n";
                if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

                xxxfuncName = S.c_str();
                xxxfileName = QString( s.getPath().c_str() );
//                xxxfileName = QString( s.getPath().getBaseName().c_str() );
                xxxlineNumber = s.getLine();
                if( dumpClipFLAG ) {
                  std::cerr << "DCLIP: " <<  "xxxfuncName=" << xxxfuncName << "\n";
                  std::cerr << "DCLIP: " <<  "xxxfileName=" << xxxfileName << "\n";
                  std::cerr << "DCLIP: " <<  "xxxlineNumber=" << xxxlineNumber << "\n";
                }
#ifdef DEBUG_StatsPanel
                std::cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxfuncName=" << xxxfuncName << "\n";
                std::cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxfileName=" << xxxfileName << "\n";
                std::cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxlineNumber=" << xxxlineNumber << "\n";
#endif
              }

              
          }
            break;
          case CMD_RESULT_STATEMENT:
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, ENTER CASEBLOCK FOR CMD_RESULT_STATEMENT\n");
#endif
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_STATEMENT\n";
            {


              CommandResult_Statement *T = (CommandResult_Statement *)cr;
              Statement s = (Statement)*T;
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    s.getPath()=" << s.getPath() << "\n";
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

              xxxfuncName = QString::null;
              xxxfileName = QString( s.getPath().c_str() );
//              xxxfileName = QString( s.getPath().getBaseName().c_str() );
              xxxlineNumber = s.getLine();
              if( dumpClipFLAG )
              {
                std::cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxfuncName=" << xxxfuncName << "\n";
                std::cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxfileName=" << xxxfileName << "\n";
                std::cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxlineNumber=" << xxxlineNumber << "\n";
              }
              if( highlightList ) {

                QString colheader = (QString)*columnHeaderList.begin();
                int color_index = getLineColor((double)valueStr.toDouble());
//                int color_index = getLineColor((unsigned int)valueStr.toUInt());
                hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                          hotToCold_color_names[color_index], 
//                                          hotToCold_color_names[currentItemIndex], 
                                          valueStr.stripWhiteSpace(), 
                                          QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), colheader);

                if( dumpClipFLAG ) hlo->print();
                highlightList->push_back(hlo);

              }

            }
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, EXIT CASEBLOCK FOR CMD_RESULT_STATEMENT\n");
#endif
            break;

          case CMD_RESULT_LOOP:
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, ENTER CASEBLOCK FOR CMD_RESULT_LOOP\n");
#endif
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_LOOP\n";
            {

              CommandResult_Loop *crl = (CommandResult_Loop *)cr;
              std::set<Statement> T = crl->getDefinitions();
              if( T.size() > 0 ) {
                std::set<Statement>::const_iterator ti = T.begin();
                Statement s = *ti;

                if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    s.getPath()=" << s.getPath() << "\n";
                if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

                xxxfuncName = QString::null;
                xxxfileName = QString( s.getPath().c_str() );
//              xxxfileName = QString( s.getPath().getBaseName().c_str() );
                xxxlineNumber = s.getLine();
                if( dumpClipFLAG )
                {
                  std::cerr << "DCLIP: CMD_RESULT_LOOP:" <<  "xxxfuncName=" << xxxfuncName << "\n";
                  std::cerr << "DCLIP: CMD_RESULT_LOOP:" <<  "xxxfileName=" << xxxfileName << "\n";
                  std::cerr << "DCLIP: CMD_RESULT_LOOP:" <<  "xxxlineNumber=" << xxxlineNumber << "\n";
                }
                if( highlightList ) {

                  QString colheader = (QString)*columnHeaderList.begin();
                  int color_index = getLineColor((double)valueStr.toDouble());
//                  int color_index = getLineColor((unsigned int)valueStr.toUInt());
                  hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                            hotToCold_color_names[color_index], 
//                                            hotToCold_color_names[currentItemIndex], 
                                            valueStr.stripWhiteSpace(), 
                                            QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), colheader);

                  if( dumpClipFLAG ) hlo->print();
                  highlightList->push_back(hlo);

                }
              }

            }
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, EXIT CASEBLOCK FOR CMD_RESULT_LOOP\n");
#endif
            break;

          case CMD_RESULT_LINKEDOBJECT:
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_LINKEDOBJECT\n";
            break;

          case CMD_RESULT_CALLTRACE:
          {
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, ENTER CASEBLOCK FOR CMD_RESULT_CALLTRACE\n");
#endif
            if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_CALLTRACE\n";
            CommandResult_CallStackEntry *CSE = (CommandResult_CallStackEntry *)cr;


            std::vector<CommandResult *> *CSV = CSE->Value();
            int64_t sz = CSV->size();
            std::vector<CommandResult *> *C1 = CSV;
	    if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "  CMD_RESULT_CALLTRACE: Form ="
				   << CSE->Form().c_str() << " sz= " << sz << "\n";

            CommandResult *CE = (*C1)[sz - 1];
#ifdef DEBUG_StatsPanel
            printf("StatsPanel::process_clip, IN CASEBLOCK FOR CMD_RESULT_CALLTRACE, CE->Type()=%d, CMD_RESULT_FUNCTION=%d\n", CE->Type(), CMD_RESULT_FUNCTION);
            printf("StatsPanel::process_clip, IN CASEBLOCK FOR CMD_RESULT_CALLTRACE, CMD_RESULT_STATEMENT=%d\n", CMD_RESULT_STATEMENT);
#endif
            if( CE->Type() == CMD_RESULT_FUNCTION )
            {
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "  CMD_RESULT_CALLTRACE with CE->Type() == CMD_RESULT_FUNCTION: sz=" << sz
				     << " and function ="
				     << CE->Form().c_str() << "\n";


              //std::string S = ((CommandResult_Function *)CE)->Value();
              std::string S;
              ((CommandResult_Function *)CE)->Value(S);

#ifdef DEBUG_StatsPanel
              std::cerr << "DCLIP: AFTER Assigning S to ((CommandResult_Function *)CE)->getName(), S=" << S << std::endl;
#endif

              xxxfuncName = S.c_str();

              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "((CommandResult_Function *)CE)->getName() == S=" << S << "\n";


#ifdef DEBUG_StatsPanel
              LinkedObject L = ((CommandResult_Function *)CE)->getLinkedObject(); 
              if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "    L.getPath()=" << L.getPath() << "\n";
#endif

	      // IMPORTANT: This will focus on the actual linenumber that
	      // corresponds to the address within the function!
	      // Overrides xxxlineNumber computed above.
              std::set<Statement> TT; 
              ((CommandResult_Function *)CE)->Value(TT);

#ifdef DEBUG_StatsPanel
              printf("StatsPanel::process_clip, FUNCTION: TT.begin() != TT.end()=%d, (TT.size() > 0)=%d\n",
                     (TT.begin() != TT.end()), (TT.size() > 0));
#endif


	      if (TT.begin() != TT.end() || TT.size() > 0) {

          	std::set<Statement>::const_iterator sti = TT.begin();;
          	Statement S = *sti;
          	char l[50];
          	sprintf( &l[0], "%lld", (int64_t)(S.getLine()));
                xxxfileName = QString( S.getPath().c_str() );
                xxxlineNumber = S.getLine();

#ifdef DEBUG_StatsPanel
		printf("StatsPanel::process_clip, FUNCTION: lineNumber via getValue is %d, fileName is %s\n",xxxlineNumber, xxxfileName.ascii());
#endif

		nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: lineNumber via getValue is %d, fileName is %s\n",xxxlineNumber, xxxfileName.ascii());

              } else {

#ifdef DEBUG_StatsPanel
		printf("StatsPanel::process_clip, FUNCTION: TT.begin == TT.end!, no statement info. reset xxxfileName and xxxlineNumber\n");
#endif
		nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: TT.begin == TT.end!, no statement info. reset xxxfileName and xxxlineNumber\n");
		xxxfileName = QString::null;
		xxxlineNumber = -1;
	      }

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, FUNCTION: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                      xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);
#endif

	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                                    xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);

            } else if( CE->Type() == CMD_RESULT_STATEMENT ) {

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, CMD_RESULT_STATEMENT: sz=%d, function=%s\n",
                     sz,CE->Form().c_str());
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip,   CMD_RESULT_STATEMENT: sz=%d, function=%s\n",
                                    sz,CE->Form().c_str());

              CommandResult_Statement *T = (CommandResult_Statement *)CE;
              Statement s = (Statement)*T;

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, STATEMENT: s.getPath()=%s, s.getLine()=%d\n",
                     s.getPath().c_str(),(int64_t)s.getLine());
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, STATEMENT: s.getPath()=%s, s.getLine()=%d\n",
                                    s.getPath().c_str(),(int64_t)s.getLine());

              xxxfuncName = CE->Form().c_str();
              xxxfileName = QString( s.getPath().c_str() );
//              xxxfileName = QString( s.getPath().getBaseName().c_str() );
              xxxlineNumber = s.getLine();

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, STATEMENT: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                     xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, STATEMENT: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                                     xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);

              if( highlightList ) {
                QString colheader = (QString)*columnHeaderList.begin();
#ifdef DEBUG_StatsPanel
 	        printf("StatsPanel::process_clip, STATEMENT: valueStr=%s, (unsigned int)valueStr.toUInt()=%d, currentItemIndex=%d\n",
                        valueStr.ascii(),(unsigned int)valueStr.toUInt(), currentItemIndex);
#endif
                int color_index = getLineColor((double)valueStr.toDouble());
#ifdef DEBUG_StatsPanel
 	        printf("StatsPanel::process_clip, STATEMENT: hotToCold_color_names[color_index]=%s\n", hotToCold_color_names[color_index]);
#endif
//                int color_index = getLineColor((unsigned int)valueStr.toUInt());
                hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                          hotToCold_color_names[color_index], 
                                          valueStr.stripWhiteSpace(), 
                                          QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), 
                                          colheader );

#ifdef DEBUG_StatsPanel
 	        printf("StatsPanel::process_clip, before hlo->print() call\n");
#endif
                if( dumpClipFLAG ) hlo->print();
#ifdef DEBUG_StatsPanel
 	        printf("StatsPanel::process_clip, after hlo->print() call\n");
#endif

                highlightList->push_back(hlo);
              }

            } else if( CE->Type() == CMD_RESULT_LOOP ) {

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, CMD_RESULT_LOOP: sz=%d, function=%s\n",
                     sz,CE->Form().c_str());
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip,   CMD_RESULT_LOOP: sz=%d, function=%s\n",
                                    sz,CE->Form().c_str());

              CommandResult_Loop *crl = (CommandResult_Loop *)CE;
              std::set<Statement> T = crl->getDefinitions();
              if( T.size() > 0 ) {
                std::set<Statement>::const_iterator ti = T.begin();
                Statement s = *ti;

#ifdef DEBUG_StatsPanel
	        printf("StatsPanel::process_clip, LOOP: s.getPath()=%s, s.getLine()=%d\n",
                       s.getPath().c_str(),(int64_t)s.getLine());
#endif
	        nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, LOOP: s.getPath()=%s, s.getLine()=%d\n",
                                    s.getPath().c_str(),(int64_t)s.getLine());

                xxxfuncName = CE->Form().c_str();
                xxxfileName = QString( s.getPath().c_str() );
//                xxxfileName = QString( s.getPath().getBaseName().c_str() );
                xxxlineNumber = s.getLine();

#ifdef DEBUG_StatsPanel
	        printf("StatsPanel::process_clip, LOOP: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                       xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);
#endif
	        nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, LOOP: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                                       xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);

                if( highlightList ) {
                  QString colheader = (QString)*columnHeaderList.begin();
#ifdef DEBUG_StatsPanel
 	          printf("StatsPanel::process_clip, LOOP: valueStr=%s, (unsigned int)valueStr.toUInt()=%d, currentItemIndex=%d\n",
                          valueStr.ascii(),(unsigned int)valueStr.toUInt(), currentItemIndex);
#endif
                  int color_index = getLineColor((double)valueStr.toDouble());
#ifdef DEBUG_StatsPanel
     	          printf("StatsPanel::process_clip, LOOP: hotToCold_color_names[color_index]=%s\n", hotToCold_color_names[color_index]);
#endif
//                  int color_index = getLineColor((unsigned int)valueStr.toUInt());
                  hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                            hotToCold_color_names[color_index], 
                                            valueStr.stripWhiteSpace(), 
                                            QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), 
                                            colheader );

#ifdef DEBUG_StatsPanel
 	          printf("StatsPanel::process_clip, before hlo->print() call\n");
#endif
                  if( dumpClipFLAG ) hlo->print();
#ifdef DEBUG_StatsPanel
 	          printf("StatsPanel::process_clip, after hlo->print() call\n");
#endif

                  highlightList->push_back(hlo);
                }
              }

            } else {

              if( dumpClipFLAG ) std::cerr << "DCLIP: " <<  "How do I handle this type? CE->Type() " << CE->Type() << "\n";

#ifdef DEBUG_StatsPanel
	       printf("StatsPanel::process_clip, CALLTRACE: How do I handle this type? %d\n", CE->Type());
#endif
	       nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, CALLTRACE: How do I handle this type? %d\n", CE->Type());
            }
          }
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::process_clip, EXIT CASEBLOCK FOR CMD_RESULT_CALLTRACE\n");
#endif
          break;
        case CMD_RESULT_TIME:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_TIME\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
          break;
        case CMD_RESULT_TITLE:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_TITLE\n";
          break;
        case CMD_RESULT_COLUMN_HEADER:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_HEADER\n";
          break;
        case CMD_RESULT_COLUMN_VALUES:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_VALUES\n";
          break;
        case CMD_RESULT_COLUMN_ENDER:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_ENDER\n";
          break;
        case CMD_RESULT_EXTENSION:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_EXTENSION\n";
          break;
        default:
          if( dumpClipFLAG) std::cerr << "DCLIP: " <<  "Got CMD_RESULT_EXTENSION\n";
          break;
        }

        i++;
      }

    } else if ((*cri)->Type() == CMD_RESULT_STRING) {
      // This looks to be a message we should display

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, Here CMD_RESULT_STRING:\n");
#endif
      QString s = QString((*cri)->Form().c_str());
        
      QMessageBox::information( (QWidget *)this, tr("Info:"), s, QMessageBox::Ok );
      skipFLAG = TRUE;
//      break;
    } else if( (*cri)->Type() == CMD_RESULT_COLUMN_HEADER ) {
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::process_clip, Here CMD_RESULT_COLUMN_HEADER:\n");
#endif
      std::list<CommandResult *> columns;
      CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
      ccp->Value(columns);

      std::list<CommandResult *>::iterator column_it;
      int i = 0;
      columnFieldList.clear();
      for (column_it = columns.begin(); column_it != columns.end(); column_it++)
      {

#ifdef DEBUG_StatsPanel
        std::cerr << "CMD_RESULT_COLUMN_HEADER:  " << (*column_it)->Form().c_str() << "\n";
#endif

        QString vs = (*column_it)->Form().c_str();
        columnFieldList.push_back(vs);

      }
    } else {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, Here OTHER:\n");
#endif
    }

    /* You have found the next row!! */
    // Here's a formatted row
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::process_clip, Here you have found the next row - here is the formatted row:\n");
#endif

    if( dumpClipFLAG) std::cerr << "DCLIP: " <<  (*cri)->Form().c_str() << "\n";

    // DUMP THIS TO OUR "OLD" FORMAT ROUTINE.
//    if( highlightList == NULL )
// printf("skipFLAG == FALSE\n");
//
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::process_clip, highlightList=%d, skipFLAG=%d\n", highlightList, skipFLAG);
#endif
    if( highlightList == NULL && skipFLAG == FALSE ) {

      QString s = QString((*cri)->Form().c_str());

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, printing the QString s as output:%s\n", s.ascii() );
      printf("StatsPanel::process_clip, calling outputCLIData, xxxfileName.ascii()=%s, printing the QString s as output: %s\n", 
              xxxfileName.ascii(), s.ascii() );
#endif

      outputCLIData( xxxfuncName, xxxfileName, xxxlineNumber );
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::process_clip, BOTTOM OF FOR cmd_result loop ----------------------------------\n");
#endif
  } // end for through results

}


static bool step_forward = TRUE;
void
StatsPanel::progressUpdate()
{
#ifdef DEBUG_StatsPanel
//  Lots of these occur so commenting this out unless really needed.
//  printf("StatsPanel::progressUpdate, progressUpdate() entered\n");
#endif
  pd->qs->setValue( steps );
  if( step_forward )
  {
    steps++;
  } else
  {
    steps--;
  }
  if( steps == 100 )
  {
//    step_forward = FALSE;
    step_forward = TRUE;
  } else if( steps == 0 )
  {
    step_forward = TRUE;
  }
}

void 
StatsPanel::insertDiffColumn(int insertAtIndex)
{
// The output out has been added to the StatsPanel.   Do you want to add
// the "Difference" column.
  QPtrList<QListViewItem> lst;
  QListViewItemIterator it( splv );
  int index = splv->addColumn("|Difference|", 200);
  int columnCount = splv->columns();

  int i=index;
  // First move the header columns.
  for(;i>insertAtIndex;i--)
  {
    splv->setColumnText( i, splv->columnText(i-1)  );
    splv->setColumnWidth( i, splv->columnWidth(i-1) );
  }
  splv->setColumnText( i, "|Difference|" );

  while ( it.current() )
  {
    QListViewItem *item = it.current();
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::insertDiffColumn,ls=%s rs=%s\n", item->text(0).ascii(), item->text(1).ascii() );
#endif
    QString ls = item->text(0);
    QString rs = item->text(1);
    double lsd = ls.toDouble();
    double rsd = rs.toDouble();
    double dd = lsd-rsd;
    double add = fabs(dd);

    for(i=index;i>insertAtIndex;i--)
    {
      item->setText(i,     item->text(i-1)  );
    }
    item->setText(i, QString("%1").arg(add));

    ++it;
  }
}


void 
StatsPanel::removeDiffColumn(int removeIndex)
{
  splv->removeColumn(removeIndex);
}

int
StatsPanel::getValidExperimentIdForView()
{
  int return_exp_id = -1;

#ifdef DEBUG_StatsPanel_cview
  printf("WE have a chooseExperimentDialog, chooseExperimentDialog=%d, compareExpIDs[0]=%d\n", 
         chooseExperimentDialog, compareExpIDs[0]);
  if (compareExpIDs[1] > 0) {
    printf("WE have a chooseExperimentDialog, chooseExperimentDialog=%d, compareExpIDs[1]=%d\n", 
           chooseExperimentDialog, compareExpIDs[1]);
  }
  if (compareExpIDs[2] > 0) {
    printf("WE have a chooseExperimentDialog, chooseExperimentDialog=%d, compareExpIDs[2]=%d\n", 
           chooseExperimentDialog, compareExpIDs[2]);
  }
#endif


  if( chooseExperimentDialog == NULL ) {
      chooseExperimentDialog = new ChooseExperimentDialog(getPanelContainer()->getMainWindow(), 
                                                          "Select Experiment For View:", 
                                                           &compareExpIDs, &compareExpDBNames, &compareSrcFilenames, focusedExpID);
      chooseExperimentDialog->show();
  } else {
      chooseExperimentDialog->show();
  }

#ifdef DEBUG_StatsPanel
  printf("WE have a chooseExperimentDialog, chooseExperimentDialog=%d\n", chooseExperimentDialog);
#endif

  if( chooseExperimentDialog->exec() == QDialog::Accepted ) { 
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::getValidExperimentIdForView, QDialog::Accepted\n");
     printf("StatsPanel::getValidExperimentIdForView, chooseExperimentDialog->focusExpID=%d\n", chooseExperimentDialog->focusExpID );
#endif
     return_exp_id = chooseExperimentDialog->focusExpID;
     // We could make this optional, so that we could keep using the result focusExpID without asking again
     chooseExperimentDialog = NULL;
  } else {
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::getValidExperimentIdForView, QDialog::Rejected\n");
#endif
  }


#ifdef DEBUG_StatsPanel
   printf("StatsPanel::getValidExperimentIdForView, entered, lastCommand=%s\n", lastCommand.ascii());
#endif

   if( lastCommand.startsWith("cview -c")) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getValidExperimentIdForView, we are in cview/compare situation, compareExpIDs.size()=%d\n", compareExpIDs.size());
#endif
     if (compareExpIDs.size() > 1) {
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::getValidExperimentIdForView, size >= 2, compareExpIDs[0]=(%d)\n", 
               compareExpIDs[0] );
       printf("StatsPanel::getValidExperimentIdForView, size >= 2, compareExpIDs[1]=(%d)\n", 
               compareExpIDs[1] );
#endif
       // for now return first experiment in the list
//       return_exp_id = compareExpIDs[0];
    }

   }
   return(return_exp_id);
}

void
StatsPanel::analyzeTheCView()
{

#ifdef DEBUG_StatsPanel_cview
   printf("analyzeTheCView(%s) entered: focusedExpID was=%d\n", lastCommand.ascii(), focusedExpID );
   printf("analyzeTheCView, RESETTING focusedExpID to -1\n");
#endif

  if( !lastCommand.startsWith("cview -c") ) {
    return;
  }

  QValueList<QString> cidList;

#ifdef DEBUG_StatsPanel_cview
  printf("analyzeTheCView, lastCommand =(%s)\n", lastCommand.ascii() );
#endif

  int vindex = lastCommand.find("-v");

#ifdef DEBUG_StatsPanel_cview
   printf("analyzeTheCView, vindex = %d\n", vindex);
#endif

  int mindex = lastCommand.find("-m");

#ifdef DEBUG_StatsPanel_cview
   printf("analyzeTheCView, mindex = %d\n", mindex);
#endif

  int end_index = vindex;
  if( vindex == -1 ) {
    end_index = mindex;
  }

  if( mindex != -1 && mindex < vindex ) {
    end_index = mindex;
  }

#ifdef DEBUG_StatsPanel_cview
  printf("analyzeTheCView, end_index= %d\n", end_index);
#endif

  QString ws = QString::null;
  if( end_index == -1 ) {
    ws = lastCommand.mid(9,999999);
  } else {
    ws = lastCommand.mid(9,end_index-10);
  }

#ifdef DEBUG_StatsPanel_cview
  printf("analyzeTheCView, ws=(%s)\n", ws.ascii() );
#endif

  int cnt = ws.contains(",");
  if( cnt > 0 ) {
    for(int i=0;i<=cnt;i++) {
      cidList.push_back( ws.section(",", i, i).stripWhiteSpace() );

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, cid list count=%d, compare id: cidList push back (%s)\n", cnt, ws.section(",", i, i).stripWhiteSpace().ascii() );
#endif

    }
  }

  if( cnt == 0 ) {

#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, We only have one cview... option (%s).\n", ws.ascii() );
#endif

    cidList.push_back( ws );

  }

  for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
  {
    CInfoClass *cic = (CInfoClass *)*it;
    delete(cic);
  }
  cInfoClassList.clear();

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

#ifdef DEBUG_StatsPanel_cview
  printf("analyzeTheCView, coming: currentMetricStr was %s\n", currentMetricStr.ascii() );
#endif
  currentMetricStr = QString::null;
  InputLineObject *clip = NULL;
  std::string cstring;
  infoAboutComparingString += QString("Comparing:");
  int max_cid_value = -1;
  int columnNum = 1;

  for( QValueList<QString>::Iterator it = cidList.begin(); it != cidList.end(); ++it)
  {
    QString cid_str = (QString)*it;

#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, TOP of QVALUELIST, cid_str=(%s)\n", cid_str.ascii() );
#endif

    QString command = QString("cviewinfo -c %1 %2").arg(cid_str).arg(timeIntervalString);
    if( !cli->getStringValueFromCLI( (char *)command.ascii(), &cstring, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }

    QString str = QString( cstring.c_str() );

#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, START-STRING ANALYSIS compare id string: str=(%s)\n", str.ascii() );
#endif

    int cid = -1;
    int expID = -1;
    QString collectorStr = QString::null;
    QString metricStr = QString::null;
    QString infoRankStr = QString::null;
    int start_host = -1;
    int start_expid = -1;
    int end_expid_index = -1;
    int end_host_index = -1;
    int start_process = -1;
    int end_process_index = -1;
    int start_rank = -1;
    int end_rank_index = -1;
    int start_metric = -1;
    int end_metric_index = -1;
 
    int str_length = str.length();

    int start_index = 3;
    int end_index = str.find(":");
    int colon_index = end_index;


    // Look up cid (-c)
    QString cidStr = str.mid(start_index, end_index-start_index);
    cid = cidStr.toInt();

    if (cid > max_cid_value) {
       max_cid_value = cid;
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, str_length=%d, cid=%d, max_cid_value=%d, start_index=%d, end_index=%d, colon_index=%d\n", 
             str_length, cid, max_cid_value, start_index, end_index, colon_index);
#endif

    // Look up collector name.
    start_index = end_index+1;
    end_index = str.find("-x");
    // Pretty sure this will always find -x
    if (end_index != -1) {
      collectorStr = str.mid(start_index, end_index-start_index).stripWhiteSpace();
    }

    // Look up expID (-x)
    start_index = end_index+3;
    start_expid = end_index+3;
    
    int new_index = findNextMajorToken(str, start_index, QString("-h") );

    if (new_index != -1) {
      end_expid_index = new_index;
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,return from findNextMajorToken, for -x end, new_index=%d, end_expid_index=%d\n", new_index, end_expid_index);
#endif

    end_index = str.find("-h", start_index);
    if (end_index != -1) {
       start_host = end_index;
       // look for the end of -h host string, add 2 for -h
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, about to call findNextMajorToken, for -h end, start_host+2=%d\n", start_host+2 );
#endif
       new_index = findNextMajorToken(str, start_host+2, QString("-r") );
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,return from findNextMajorToken, for -h end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_host_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, HOST INFO results start_host=%d, start_index=%d, end_host_index=%d\n", start_host, start_index, end_host_index);
#endif

    // Look up metric
    end_index = str.find("-m", start_index);
    if (end_index != -1) {
       start_metric = end_index;
       // look for the end of -m metric string, add 2 for -m
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,about to call findNextMajorToken, for -m end, start_metric+2=%d\n", start_metric+2 );
#endif
       new_index = findNextMajorToken(str, start_metric+2, QString("-r") );
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,return from findNextMajorToken, for -m end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_metric_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, METRIC INFO results start_metric=%d, start_index=%d, end_metric_index=%d\n", start_metric, start_index, end_metric_index);
#endif

    // Look up process
    end_index = str.find("-p", start_index);
    if( end_index != -1 ) {
        start_process = end_index;
       // look for the end of -p process string, add 2 for -p
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,about to call findNextMajorToken, for -p end, start_process+2=%d\n", start_process+2 );
#endif
       new_index = findNextMajorToken(str, start_process+2, QString(";") );
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,return from findNextMajorToken, for -p end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_process_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, process info results start_process=%d, start_index=%d, end_process_index=%d\n", start_process, start_index, end_process_index);
#endif

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, LOOKING FOR RANK, start_index=%d\n", start_index);
#endif

    // Look up rank info
    end_index = str.find("-r", start_index);
    if( end_index != -1 ) {
       start_rank = end_index;
       // look for the end of -r rank string, add 2 for -r
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, about to call findNextMajorToken, for -r end, start_rank+2=%d\n", start_rank+2 );
#endif
       new_index = findNextMajorToken(str, start_rank+2, QString(";") );
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView,return from findNextMajorToken, for -r end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_rank_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, RANK INFO RESULTS start_rank=%d, start_index=%d, end_rank_index=%d\n", start_rank, start_index, end_rank_index);
#endif

    if (start_rank != -1 && end_rank_index != -1) {
        infoRankStr = str.mid(start_rank+3, end_rank_index-start_rank+2);
#ifdef DEBUG_StatsPanel_cview
        printf("analyzeTheCView, RANK INFO VALUES: start_rank = %d, end_rank_index=%d, infoRankStr.ascii()=(%s)\n", start_rank, end_rank_index, infoRankStr.ascii());
#endif
    }


#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, END-STRING ANALYSIS compare id string: cid=%d, str=(%s)\n", cid, str.ascii() );
      printf("analyzeTheCView, cidStr=%s\n", cidStr.ascii() );
      printf("analyzeTheCView, start_host=%d, start_index=%d, end_index=%d\n", start_host, start_index, end_index);
      printf("analyzeTheCView, start_metric=%d, start_process=%d, start_rank=%d\n", start_metric, start_process, start_rank);
      printf("analyzeTheCView, start_expid=%d, end_expid_index=%d\n", start_expid, end_expid_index);
#endif

    if(!cidStr.isEmpty() ) {
      infoAboutComparingString += QString("\nColumn(s) labeled -c %1: ").arg(cidStr.ascii());
    }

    QString expIDStr = str.mid(start_expid, end_expid_index-start_expid);
    expID = expIDStr.toInt();

    infoAboutComparingString += QString("Experiment %1 ").arg(expID);
#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, A: expID = %d, expIDStr.ascii()=(%s)\n", expID, expIDStr.ascii());
#endif

    // Make a list of comma separated experiment ids for the stats panel info header
    infoAboutStringCompareExpIDs += QString("%1,").arg(expID);

    // Add the database name for this particular experiment into the info header    
    bool force_use_of_exp_id = TRUE;
    getDatabaseName(expID, force_use_of_exp_id);
    QString dbnamesStr = getDBName(expID);
    infoAboutComparingString += QString(" Database Name: %1 ").arg(dbnamesStr);

    QString host_pid_names = QString::null;

#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, start_host = (%d), infoAboutStringCompareExpIDs.ascii()=(%s)\n", start_host, infoAboutStringCompareExpIDs.ascii());
#endif

    if( start_host != -1 )
    {
      start_index = start_host;
      end_index = str.find("-m");
      if( end_index == -1 )
      {
        end_index = 999999;
      }
      host_pid_names = str.mid(start_index, end_index-start_index);
    }

    // Look up metricStr
#ifdef DEBUG_StatsPanel_cview
    printf("look up the metricStr in str? str=(%s)\n", str.ascii() );
#endif

    start_index = str.find("-m");
    if( start_index == -1 )
    {  // see if there's one on the original command?
      // HACK! HACK!   HACK!!

      str = lastCommand;
      start_index = str.find("-m");
    }

    if( start_index != -1 )
    {
      start_index += 3;  // Skip the -m
      // metricStr = str.right(start_index);
      metricStr = str.mid(start_index, 9999999);
    } else
    {
      metricStr = QString::null;
    }

    if( currentMetricStr.isEmpty() )
    {
      currentMetricStr = metricStr;
#ifdef DEBUG_StatsPanel_cview
      printf("StatsPanel::analyzeTheCView, currentMetricStr set to %s\n", currentMetricStr.ascii() );
      printf("StatsPanel::analyzeTheCView, new CInfoClass:cid=%d collectorStr=(%s) expID=(%d) host_pid_names=(%s) metricStr=(%s)\n",
             cid, collectorStr.ascii(), expID, host_pid_names.ascii(), metricStr.ascii() );
#endif
    }
    
    // There are at least two ways to come into this section of code
    // for ranks and hosts.
    // One way is with only ranks specified.  This is the group similar processes option.
    //    - for this option we separate out the ranks and report which ranks are
    //    - being compared for each respective column displayed
    // Another way is with hosts and ranks specified.  That is the else clause below.
    //    - we try to pretty this option up by changing the -h to "Host:" and the
    //    - "-r" to "Rank:" and remove line feeds and extra spaces to form a more
    //    - user friendly display.
#ifdef DEBUG_StatsPanel_cview
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, start_host=%d\n", start_host );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, start_index=%d\n", start_index );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, infoRankStr.isEmpty()=%d\n", infoRankStr.isEmpty() );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, str.find(-h, start_index)=%d\n", str.find("-h", start_index));
#endif
    
    if(!infoRankStr.isEmpty() && start_host == -1) {
      infoAboutComparingString += QString(": Rank(s) %1 ").arg( infoRankStr );
    } else {

      QString my_host_pid_names = "";

#ifdef DEBUG_StatsPanel_cview
     printf("StatsPanel::analyzeTheCView, before -h checks, host_pid_names=(%s)\n", host_pid_names.ascii() );
#endif

     if(!host_pid_names.isEmpty() ) {
       int h_search_start_index = 0;
       int minus_h_dx = -1;
       while (h_search_start_index != -1) {
         minus_h_dx = host_pid_names.find("-h", h_search_start_index);
         if (minus_h_dx != -1) {
           my_host_pid_names = host_pid_names.replace( minus_h_dx, 2, "Host:");
         }
         h_search_start_index = minus_h_dx;
       }
 
#ifdef DEBUG_StatsPanel_cview
       printf("StatsPanel::analyzeTheCView, after -h checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
       int r_search_start_index = 0;
       int minus_r_dx = -1;
       while (r_search_start_index != -1) {
         minus_r_dx = host_pid_names.find("-r", r_search_start_index);
         if (minus_r_dx != -1) {
           my_host_pid_names = host_pid_names.replace( minus_r_dx, 2, "Rank:");
         }
         r_search_start_index = minus_r_dx;
       }
#ifdef DEBUG_StatsPanel_cview
       printf("StatsPanel::analyzeTheCView, after -r checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       // Strip out the end of lines (line feeds), so the output come on one line
       int bslash_n_search_start_index = 0;
       int bslash_n_dx = -1;
       while (bslash_n_search_start_index != -1) {
         bslash_n_dx = host_pid_names.find("\n", bslash_n_search_start_index);
         if (bslash_n_dx != -1) {
           my_host_pid_names = host_pid_names.remove( bslash_n_dx, 2);
         }
         bslash_n_search_start_index = bslash_n_dx;
       }
 
#ifdef DEBUG_StatsPanel_cview
       printf("StatsPanel::analyzeTheCView, after bslash checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       // Strip out the spaces characters (5 spaces), so the output come on one line
       int spaces_search_start_index = 0;
       int spaces_dx = -1;
 
       while (spaces_search_start_index != -1) {
 
         spaces_dx = host_pid_names.find("     ", spaces_search_start_index);
 
#ifdef DEBUG_StatsPanel_cview
       printf("StatsPanel::analyzeTheCView, in spaces checks, spaces_search_start_index=%d, spaces_dx=%d, my_host_pid_names=(%s)\n", spaces_search_start_index, spaces_dx, my_host_pid_names.ascii() );
#endif
 
         if (spaces_dx != -1) {
           my_host_pid_names = host_pid_names.remove( spaces_dx, 5);
         }
 
         spaces_search_start_index = spaces_dx;
       }
#ifdef DEBUG_StatsPanel_cview
       printf("StatsPanel::analyzeTheCView, after 5 spaces checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       infoAboutComparingString += QString(": Showing %1 ").arg( my_host_pid_names );
     }
    }

    if(!currentMetricStr.isEmpty() ) {
      infoAboutComparingString += QString("\nfor performance data type: %1 ").arg( collectorStr );
    }

    if (!metricStr.isEmpty()) {
      infoAboutComparingString += QString("using display option: %1 ").arg( metricStr );
    }

    CInfoClass *cic = new CInfoClass( cid, collectorStr, expID, host_pid_names, metricStr );
    cInfoClassList.push_back( cic );
#ifdef DEBUG_StatsPanel_cview
    printf("StatsPanel::analyzeTheCView, push this out..,  infoAboutComparingString.ascii()=%s\n", infoAboutComparingString.ascii());
    cic->print();
#endif

    columnNum += 1;
#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, BOTTOM of QVALUELIST, infoAboutComparingString.ascii=(%s)\n", infoAboutComparingString.ascii() );
#endif
  } // end major loop through QValueList 
 
  if (max_cid_value != -1) {
    // do some adjustment for compare groups
  }

  experimentGroupList.clear();

// I eventually want a info class per column of cview data...

  for(int i=0;i < splv->columns(); i++ ) {

    QString header = splv->columnText(i);
    int end_index = header.find(":");
    int commaIndex = header.find(",");

#ifdef DEBUG_StatsPanel_cview
    printf("analyzeTheCView, B: commaIndex=%d, header.ascii()=%s\n", 
           commaIndex, header.ascii() );
#endif

    if(commaIndex != -1 ) {
      end_index = commaIndex;
#ifdef DEBUG_StatsPanel_cview
      printf("analyzeTheCView, B: end_index=%d, header.ascii()=%s\n", 
             end_index, header.ascii() );
#endif
    }

    if( header.startsWith("-c ") && end_index > 0 ) {
      int start_index = 3;
      QString cviewIDStr = header.mid(start_index, end_index-start_index);
      int cid = cviewIDStr.toInt();
      for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
      {
        CInfoClass *cic = (CInfoClass *)*it;
        if( cic->cid == cid ) {

          experimentGroupList.push_back( QString("Experiment: %1").arg(cic->expID) );

#ifdef DEBUG_StatsPanel_cview
          printf("StatsPanel::analyzeTheCView, PUSH_BACK experiment id=%d\n", cic->expID);
          printf("StatsPanel::analyzeTheCView, cic->expId=%d, focusedExpID=%d\n", cic->expID, focusedExpID );
#endif

          if( focusedExpID == -1 ) {
            focusedExpID = cic->expID;
          }

        }
      }

      // Log this into the Column
      for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
      {
        CInfoClass *cic = (CInfoClass *)*it;
        if( cic->cid == cid ) {
          columnValueClass[i].cic = cic;
#ifdef DEBUG_StatsPanel_cview
          printf("StatsPanel::analyzeTheCView, printing columnValueClass[i]\n" );
          columnValueClass[i].print();
#endif
          break;
        }
      }

    } // end startwith -c
  } // end for

  if( experimentGroupList.count() > 0 ) {
    updateCollectorList();
  }

#ifdef DEBUG_StatsPanel_cview
 printf("Exit StatsPanel::analyzeTheCView, focusedExpID=%d\n", focusedExpID);
#endif
}

bool
StatsPanel::canWeDiff()
{
// This broke with the changes to the headers for the report.
// A new approach needs to be done.

return (FALSE);

#if 0

#ifdef DEBUG_StatsPanel_diff
  printf("canWeDiff() entered\n");
#endif

  int diffIndex = -1;
  if( splv->columns() < 2 ) {

#ifdef DEBUG_StatsPanel_diff
    printf("canWeDiff() return FALSE(A)\n");
#endif

    return( FALSE );

  }

#ifdef CLUSTERANALYSIS

#ifdef DEBUG_StatsPanel_diff
  printf("canWeDiff:  lastCommand (%s)\n", lastCommand.ascii() );
#endif

  if( lastCommand.startsWith("cview -c") && lastCommand.contains("-m ") ) {

#ifdef DEBUG_StatsPanel_diff
    printf("lastCommand was (%s) and we're not going to sort!\n", lastCommand.ascii() );
    printf("canWeDiff() return FALSE(B)\n");
#endif

    return( FALSE );
  }

#endif // CLUSTERANALYSIS

  QString c1header = splv->columnText(0);
  QString c2header = splv->columnText(1);

  if( c1header == "|Difference|" ) {

#ifdef DEBUG_StatsPanel_diff
    printf("A: return TRUE\n");
#endif

    return(TRUE);
  }

#ifdef DEBUG_StatsPanel_diff
  printf("c1header=(%s) c2header=(%s)\n", c1header.ascii(), c2header.ascii() );
#endif

  if( c1header == c2header ) {

#ifdef DEBUG_StatsPanel_diff
    printf("c1header==c2header\n");
    printf("B: return TRUE\n");
#endif

    return(TRUE);
  }

  diffIndex = c1header.find(":");

#ifdef DEBUG_StatsPanel_diff
  printf("A: diffInde=%d\n", diffIndex);
#endif

int commaIndex = c1header.find(",");

#ifdef DEBUG_StatsPanel_diff
  printf("B: commaIndex=%d\n", commaIndex);
#endif

  if(commaIndex != -1 ) {
    diffIndex = commaIndex;
  }

  if( diffIndex > 0 ) {
    c1header = c1header.mid(diffIndex+1);
  }

  diffIndex = c2header.find(":");

#ifdef DEBUG_StatsPanel_diff
  printf("B: diffInde=%d\n", diffIndex);
#endif

  commaIndex = c2header.find(",");

#ifdef DEBUG_StatsPanel_diff
  printf("B: commaIndex=%d\n", commaIndex);
#endif

  if(commaIndex != -1 ) {
    diffIndex = commaIndex;
  }

  if( diffIndex > 0 ) {
    c2header = c2header.mid(diffIndex+1);
  }
    
#ifdef DEBUG_StatsPanel_diff
  printf("B: c1header=(%s) c2header=(%s)\n", c1header.ascii(), c2header.ascii() );
#endif

  if( c1header == c2header ) {

#ifdef DEBUG_StatsPanel_diff
    printf("new c1header==c2header\n");
    printf("C: return TRUE\n");
#endif

    return(TRUE);
  }


#ifdef DEBUG_StatsPanel_diff
  printf("canWeDiff() return FALSE(C)\n");
#endif

  return(FALSE);

#endif // 0
}


void
StatsPanel::generateBaseToolBar( QString command )
{

#ifdef DEBUG_StatsPanel
 printf("ENTER StatsPanel::generateBaseToolBar, this=0x%lx, currentCollectorStr.ascii()=%s\n", this, currentCollectorStr.ascii() );
 printf("ENTER StatsPanel::generateBaseToolBar, lastCollectorStr.ascii()=%s\n", lastCollectorStr.ascii() );
 printf("ENTER StatsPanel::generateBaseToolBar, recycleFLAG=%d\n", recycleFLAG );
 printf("ENTER StatsPanel::generateBaseToolBar, fileTools=0x%lx, command=%s\n", fileTools, command.ascii()  );
#endif

  // ----------------- Start of the Information Icons
  MoreMetadata_icon = new QPixmap(meta_information_plus_xpm);
  metadataToolButton = new QToolButton(*MoreMetadata_icon, "Show More Experiment Metadata", 
                                        QString::null, this, SLOT( infoEditHeaderMoreButtonSelected()), 
                                        fileTools, "show more experiment metadata");
#ifdef DEBUG_StatsPanel
  printf("StatsPanel() generateBaseToolBar CREATING metadataToolButton=0x%lx, currentCollectorStr.ascii()=%s, lastCollectorStr.ascii()=%s, recycleFLAG=%d\n", 
          metadataToolButton, currentCollectorStr.ascii(), lastCollectorStr.ascii(), recycleFLAG);
#endif

  QToolTip::add( metadataToolButton, tr( "SHOW MORE: Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
// should not need this--  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));

  LessMetadata_icon = new QPixmap(meta_information_minus_xpm);
  // ----------------- End of the Information Icons

  // ----------------- Start of the Panel Administration Icons
  QPixmap *update_icon = new QPixmap( update_icon_xpm );
  new QToolButton(*update_icon, "UPDATE: Update the statistics panel.  Make viewing option changes\nand then click on this icon to display the new view.", QString::null, this, SLOT( updatePanel()), fileTools, "Update the statistics panel to show updated view");

  QPixmap *clear_auxiliary = new QPixmap( clear_auxiliary_xpm );
  new QToolButton(*clear_auxiliary, "CLEAR: Clear all auxiliary \"sticky\" view settings, such as, specific function setting,\ntime segment settings, per event display settings, specific ranks, threads,\nor processes that were focused from the Manage Process Panel, etc..", QString::null, this, SLOT( clearAuxiliarySelected()), fileTools, "clear auxiliary settings");
  // ----------------- End of the Panel Administration Icons

  toolbar_status_label = new QLabel(fileTools,"toolbar_status_label");
  // default setting to match default views
  toolbar_status_label->setText("Initial Status");
  fileTools->setStretchableWidget(toolbar_status_label);

#ifdef DEBUG_StatsPanel
 printf("Exit StatsPanel::generateBaseToolBar\n");
#endif
}

void
StatsPanel::generateToolBar( QString command, int expID )
{

#ifdef DEBUG_StatsPanel_toolbar
 printf("ENTER StatsPanel::generateToolBar, this=0x%lx, currentUserSelectedReportStr.ascii()=%s\n", this, currentUserSelectedReportStr.ascii() );
 printf("ENTER StatsPanel::generateToolBar, this=0x%lx, currentCollectorStr.ascii()=%s\n", this, currentCollectorStr.ascii() );
 printf("ENTER StatsPanel::generateToolBar, lastCollectorStr.ascii()=%s\n", lastCollectorStr.ascii() );
 printf("ENTER StatsPanel::generateToolBar, recycleFLAG=%d, expID=%d\n", recycleFLAG, expID );
 printf("ENTER StatsPanel::generateToolBar, fileTools=0x%lx, command=%s\n", fileTools, command.ascii()  );
#endif


// If this invocation is related to a new StatsPanel from reuse
// the necessary info is not usually available at this point.
// So, we try to recreate it...

if (currentCollectorStr == NULL && lastCollectorStr == NULL ) {

    std::string collector_name = "";
    int experiment_id = -1;
    if( command.startsWith("cview") ) {
      for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it) {
          CInfoClass *cic = (CInfoClass *)*it;
          experiment_id = cic->expID;
#ifdef DEBUG_StatsPanel_toolbar
          printf("StatsPanel::generateToolBar, from c_view, experiment_id=%d\n", experiment_id );
#endif
      }
    } else {
      experiment_id = findExperimentID( command );
#ifdef DEBUG_StatsPanel_toolbar
      printf("StatsPanel::generateToolBar, from findExperimentID, experiment_id=%d\n", experiment_id );
#endif
    }
#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::generateToolBar, experiment_id=%d\n", experiment_id );
#endif
    if (experiment_id > 0) {
      std::list<std::string> collectors_list = findCollectors(experiment_id);
      for( std::list<std::string>::const_iterator it = collectors_list.begin();
           it != collectors_list.end(); it++ ) {
               collector_name = (std::string)*it;
#ifdef DEBUG_StatsPanel_toolbar
               printf("StatsPanel::generateToolBar, collector_name=%s, collector_name.size()=%d\n", 
                      collector_name.c_str(), collector_name.size()  );
#endif
      }

      if (collector_name.size() > 0 ) {
         currentCollectorStr = QString(collector_name.c_str() );
#ifdef DEBUG_StatsPanel_toolbar
         printf("StatsPanel::generateToolBar, currentCollectorStr.ascii()=%s\n", currentCollectorStr.ascii() );
#endif
      }

    } // end experiment_id > 0

} // end NULL collectorStr check

#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::generateToolBar, IFCHECK, recycleFLAG=%d\n", recycleFLAG);
  printf("StatsPanel::generateToolBar, IFCHECK, currentCollectorStr.ascii()=%s\n", currentCollectorStr.ascii() );
  printf("StatsPanel::generateToolBar, IFCHECK, lastCollectorStr.ascii()=%s\n", lastCollectorStr.ascii() );
  printf("StatsPanel::generateToolBar, IFCHECK, lastUserSelectedReportStr.ascii()=%s\n", lastUserSelectedReportStr.ascii() );
  printf("StatsPanel::generateToolBar, IFCHECK, currentUserSelectedReportStr.ascii()=%s\n", currentUserSelectedReportStr.ascii() );
#endif

if (currentCollectorStr != lastCollectorStr ||
    (currentCollectorStr == NULL && lastCollectorStr == NULL) ||
    (lastUserSelectedReportStr != currentUserSelectedReportStr) ||
    (originatingUserSelectedReportStr != NULL) ||
    recycleFLAG == FALSE ) {

  // Clear the initial setting and regenerate the toolbar
#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::generateToolBar, IFCHECK PASSES, before calling fileTools->clear() fileTools=0x%lx\n", fileTools);
#endif
  fileTools->clear();

#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::generateToolBar, after calling fileTools->clear() fileTools=0x%lx\n", fileTools);
#endif

  // ----------------- Start of the Information Icons
  MoreMetadata_icon = new QPixmap(meta_information_plus_xpm);
  metadataToolButton = new QToolButton(*MoreMetadata_icon, "Show More Experiment Metadata", 
                                        QString::null, this, SLOT( infoEditHeaderMoreButtonSelected()), 
                                        fileTools, "show more experiment metadata");
#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel() generateToolBar CREATING metadataToolButton=0x%lx, currentCollectorStr.ascii()=%s, lastCollectorStr.ascii()=%s, recycleFLAG=%d\n", 
          metadataToolButton, currentCollectorStr.ascii(), lastCollectorStr.ascii(), recycleFLAG);
#endif

  QToolTip::add( metadataToolButton, tr( "SHOW MORE: Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
// should not need this--  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));

  LessMetadata_icon = new QPixmap(meta_information_minus_xpm);
  // ----------------- End of the Information Icons

  // ----------------- Start of the Panel Administration Icons
  QPixmap *update_icon = new QPixmap( update_icon_xpm );
  new QToolButton(*update_icon, "UPDATE: Update the statistics panel.  Make viewing option changes\nand then click on this icon to display the new view.", QString::null, this, SLOT( updatePanel()), fileTools, "Update the statistics panel to show updated view");

  QPixmap *clear_auxiliary = new QPixmap( clear_auxiliary_xpm );
  new QToolButton(*clear_auxiliary, "CLEAR: Clear all auxiliary \"sticky\" view settings, such as, specific function setting,\ntime segment settings, per event display settings, specific ranks, threads,\nor processes that were focused from the Manage Process Panel, etc..", QString::null, this, SLOT( clearAuxiliarySelected()), fileTools, "clear auxiliary settings");
  // ----------------- End of the Panel Administration Icons


  // ----------------- Start of the View Generation Icons
  QPixmap *functions_icon = new QPixmap( defaultView_xpm );
  new QToolButton(*functions_icon, "SHOW DEFAULT VIEW: Generate a performance statistics report\nshowing the performance data delineated by functions (default) and optionally\nstatements or linked objects by using the View/Display Choice options.\n\nNOTE: To clear \"sticky\" view settings, such as, specific function setting,\ntime segment settings, per event display settings, specific ranks, threads,\nor processes that were focused from the Manage Process Panel, etc..\nuse the CL (Clear auxiliary settings) icon.  The CL icon will set\nthe view back to the original aggregated data view.", QString::null, this, SLOT( defaultViewSelected()), fileTools, "show default view");

#ifdef DEBUG_StatsPanel_toolbar
 printf("StatsPanel::generateToolBar, currentCollectorStr.ascii()=%s\n", currentCollectorStr.ascii() );
#endif

  if(  currentCollectorStr != "mpi" && 
       currentCollectorStr != "io" && 
       currentCollectorStr != "iot" && 
       currentCollectorStr != "iop" && 
       currentCollectorStr != "mpip" && 
       currentCollectorStr != "mem" &&
       currentCollectorStr != "pthreads" &&
       currentCollectorStr != "mpit" ) {

#if 0
    QPixmap *linkedObjects_icon = new QPixmap( linkedObjects_xpm );
    new QToolButton(*linkedObjects_icon, "SHOW LINKED OBJECTS: Generate a performance statistics report\nshowing the performance data delineated by the linked objects\nthat are involved in the execution of the executable(s).", QString::null, this, SLOT( linkedObjectsSelected()), fileTools, "show linked objects");

    QPixmap *statements_icon = new QPixmap( statements_xpm );
    new QToolButton(*statements_icon, "SHOW STATEMENTS: Generate a performance statistics report\nshowing the performance data delineated by the source line\nstatements in your program.", QString::null, this, SLOT( statementsSelected()), fileTools, "show statements");

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::generateToolBar, statements_icon=0x%lx\n", statements_icon );
#endif

#endif

    QPixmap *statementsByFunction_icon = new QPixmap( statementsByFunction_xpm );
    new QToolButton(*statementsByFunction_icon, "SHOW STATEMENTS BY FUNCTION: Generate a performance statistics\nreport showing the performance data delineated by the\nsource line statements from a selected function in your program.\nSelect a function by selecting output line in the display below and\nthen clicking this icon.  This report will only show the\nperformance information for the statements in the selected function.", QString::null, this, SLOT( statementsByFunctionSelected()), fileTools, "show statements by function");

  }

  if(  currentCollectorStr != "pcsamp" && currentCollectorStr != "hwc" && currentCollectorStr != "hwcsamp" )
  {

  if ( getPreferenceAdvancedToolbarCheckBox() == TRUE ) {

    QPixmap *calltrees_icon = new QPixmap( calltrees_xpm );
    new QToolButton(*calltrees_icon, "SHOW CALL PATHS: This view displays all the call paths in your program.\nPortions of common paths are suppressed, they are displayed from the\nprogram start function down to the user function.", QString::null, this, SLOT( calltreesSelected()), fileTools, "show calltrees");

    QPixmap *calltreesByFunction_icon = new QPixmap( calltreesByFunction_xpm );
    new QToolButton(*calltreesByFunction_icon, "SHOW CALL PATHS BY FUNCTION: This view displays all the call paths in your program that include the selected function.\nTo use click on one of the functions and then click the Calltrees By Function icon.", QString::null, this, SLOT( calltreesByFunctionSelected()), fileTools, "show calltrees by function");
   } // advanced toolbar

    QPixmap *calltreesfull_icon = new QPixmap( calltreesfull_xpm );
    new QToolButton(*calltreesfull_icon, "SHOW CALL PATHS WITH FULL STACK:  This view displays all the call paths in\nyour program.  None of the common portions of the call paths are suppressed,\nthis is indicated by the name FullStack.", QString::null, this, SLOT( calltreesFullStackSelected()), fileTools, "calltrees,fullstack");

    QPixmap *calltreesfullByFunction_icon = new QPixmap( calltreesfullByFunction_xpm );
    new QToolButton(*calltreesfullByFunction_icon, "SHOW CALL PATHS WITH FULL STACK BY FUNCTION:  This view displays all\nthe call paths in your program that include the selected function.\nNone of the common portions of the call paths are suppressed.  To use click\non one of the functions and then click the Calltrees By Function FullStack icon.", QString::null, this, SLOT( calltreesFullStackByFunctionSelected()), fileTools, "calltrees,fullstack by function");

    QPixmap *hotcallpath_icon = new QPixmap( hotcallpath_icon_xpm );
    new QToolButton(*hotcallpath_icon, "SHOW HOT CALL PATH: Show the top five (5) time taking callpaths in this program:\nThis view displays the most expensive call paths in your program.\n", QString::null, this, SLOT( hotCallpathSelected()), fileTools, "hot call path");
  if(  currentCollectorStr == "mem" ) {
    QPixmap *memLeaked_icon = new QPixmap( memLeaked_icon_xpm );
    new QToolButton(*memLeaked_icon, "SHOW MEMORY LEAKED PATH: Show the call paths where memory leaked in this program:\nThis view displays the call paths in your program where memory leaked.\n", QString::null, this, SLOT( memLeakedpathSelected()), fileTools, "memory leaked call path");

    QPixmap *memHighwater_icon = new QPixmap( memHighwater_icon_xpm );
    new QToolButton(*memHighwater_icon, "SHOW MEMORY HIGHWATER PATHS: Show the call paths where memory changed to the highwater mark in this program:\nThis view displays the call paths in your program where memory changed to the highwater point.\n", QString::null, this, SLOT( memHighwaterpathSelected()), fileTools, "memory highwater call path");

    QPixmap *memUnique_icon = new QPixmap( memUnique_icon_xpm );
    new QToolButton(*memUnique_icon, "SHOW MEMORY UNIQUE CALL PATHS: Show where unique memory call paths existed in this program:\nThis view displays the call paths in your program where unique memory call paths existed.\n", QString::null, this, SLOT( memUniquepathSelected()), fileTools, "memory unique call paths");

  }

  if ( getPreferenceAdvancedToolbarCheckBox() == TRUE ) {
    QPixmap *tracebacks_icon = new QPixmap( tracebacks_xpm );
    new QToolButton(*tracebacks_icon, "SHOW TRACEBACKS:  This view displays all the call paths in your program,\nthe call paths are displayed from the user function down to the program\nstart function.  Portions of common paths are suppressed.", QString::null, this, SLOT( tracebacksSelected()), fileTools, "show tracebacks");

    QPixmap *tracebacksByFunction_icon = new QPixmap( tracebacksByFunction_xpm );
    new QToolButton(*tracebacksByFunction_icon, "SHOW TRACEBACKS BY FUNCTION: This view displays all the call paths in your\nprogram that include the selected function. The paths are displayed from\nthe user function down to the program start function.  Portions of common\npaths are suppressed.  To use click on one of the functions and then click\nthe Tracebacks By Function icon", QString::null, this, SLOT( tracebacksByFunctionSelected()), fileTools, "show tracebacks by function");

    QPixmap *tracebacksfull_icon = new QPixmap( tracebacksfull_xpm );
    new QToolButton(*tracebacksfull_icon, "SHOW TRACEBACKS WITH FULL STACK: This view displays all the call paths in\nyour program, the call paths are displayed from the user function down\nto the program start function.  None of the portions of common paths are\nsuppressed.  All call paths are displayed.", QString::null, this, SLOT( tracebacksFullStackSelected()), fileTools, "show tracebacks,fullstack");

    QPixmap *tracebacksfullByFunction_icon = new QPixmap( tracebacksfullByFunction_xpm );
    new QToolButton(*tracebacksfullByFunction_icon, " SHOW TRACEBACKS WITH FULL STACK BY FUNCTION: This view displays all the call paths in your program that include\nthe selected function. The paths are displayed from the user function down to the program start function.\nNone of the portions of common paths are suppressed.\nTo use click on one of the functions and then click the Traceback,FullStack By Function icon", QString::null, this, SLOT( tracebacksFullStackByFunctionSelected()), fileTools, "show tracebacks,fullstack by function");
  } // advanced toolbar

    QPixmap *butterfly_icon = new QPixmap( butterfly_xpm );
    new QToolButton(*butterfly_icon, "SHOW BUTTERFLY:  This view shows the callers of the function selected\n(shown above the function) and the callees of the function selected\n(shown below the function).  Selecting one of the displayed functions\nand clicking on the Butterfly icon will make that function, the pivot \nfunction for the callers and callees display.  To use click on one of\nthe functions and then click the Butterfly icon.", QString::null, this, SLOT( butterflySelected()), fileTools, "show butterfly");

    QPixmap *timeSegment_icon = new QPixmap( timeSegment_xpm );
    new QToolButton(*timeSegment_icon, "SELECT TIME SEGMENT:  This view shows a section of the performance\nexperiments data that was chosen by selecting a segment of time from the\ntime segment dialog slider.  To return to viewing the complete experiment\ndata, click on the Clear icon followed by the Update icon.  This will\nrestore viewing 100 percent of the experiment data.", QString::null, this, SLOT( timeSliceSelected()), fileTools, "show time segment");



  }
  // ----------------- End of the View Generatin Icons
  //
#if OPTIONAL_VIEW
    QPixmap *optional_views_icon = new QPixmap( optional_views_icon_xpm );
    new QToolButton(*optional_views_icon, "SHOW OPTIONAL VIEW: Select icon to launch dialog box which will present\na number of optional fields/columns to include in the creation of an\noptional view of the existing data.", QString::null, this, SLOT( optionalViewsCreationSelected()), fileTools, "create optional view ");
#endif

#if 1
  if( currentCollectorStr == "hwcsamp" || 
      currentCollectorStr == "usertime" ) {
#else
  if( currentCollectorStr == "hwcsamp" ) {
#endif
#if 1
    QPixmap *sourceAnnotation_icon = new QPixmap( sourceAnnotation_icon_xpm );
    new QToolButton(*sourceAnnotation_icon, "CHANGE SOURCE ANNOTATION METRIC: Select icon to launch dialog box which will present\na number of optional metrics to view in the creation of\nthe source panel.", QString::null, this, SLOT( sourcePanelAnnotationCreationSelected()), fileTools, "create source annotation view ");
#endif
  }

  // ----------------- Start of the Analysis Icons
  if( currentCollectorStr == "iot" || 
      currentCollectorStr == "mpit" || 
      currentCollectorStr == "pthreads" || 
      currentCollectorStr == "mem" ) {
    QPixmap *event_list_icon = new QPixmap( event_list_icon_xpm );
    new QToolButton(*event_list_icon, "SHOW EVENT BY EVENT LIST: Show a per event list display.  There will be\none event (call to a function that was specified to be traced) per line.\nThis view is a event by event list of the occurances of each call to a MPI\nor I/O function being traced.", QString::null, this, SLOT( showEventListSelected()), fileTools, "Show per event display");
  }

#ifdef DEBUG_StatsPanel_toolbar
  printf("GGGGGGG ---------- StatsPanel::generateToolBar, list_of_pids.size()=%d\n", list_of_pids.size() );
  printf("GGGGGGG ---------- StatsPanel::generateToolBar, separate_list_of_threads.size()=%d\n", list_of_threads.size() );
#endif

  if ( list_of_pids.size() == 1 ) {
    getSeparateThreadList(expID);
  }

  if ( ( list_of_pids.size() > 1 ) || ( list_of_pids.size() == 1 && separate_list_of_threads.size() > 1) ) {

    QPixmap *load_balance_icon = new QPixmap( load_balance_icon_xpm );
    new QToolButton(*load_balance_icon, "LOAD BALANCE: Show minimum, maximum, and average statistics across ranks,\nthreads, processes: generate a performance statistics report for these metric\nvalues, creating comparison columns for each value.\nUse the View/Display Choice options to see the data for statements or linked objects.\n\nNOTE: To clear \"sticky\" view settings, such as, specific function setting,\ntime segment settings, per event display settings, specific ranks, threads,\nor processes that were focused from the Manage Process Panel, etc..\nuse the CL (Clear auxiliary settings) icon.  The CL icon will set\nthe view back to the original aggregated data view.", QString::null, this, SLOT( minMaxAverageSelected()), fileTools, "Show min, max, average statistics across ranks, threads, processes.");

    QPixmap *compare_and_analyze_icon = new QPixmap( compare_and_analyze_xpm );
    new QToolButton(*compare_and_analyze_icon, "COMPARE AND ANALYZE: Show Comparison and Analysis across ranks, threads,\nprocesses: generate a performance statistics report as the result of a \ncluster analysis algorithm to group ranks, threads or processes that have\nsimilar performance statistic characteristics.\nUse the View/Display Choice options to see the data for statements or linked objects.\n\nNOTE: To clear \"sticky\" view settings, such as, specific function setting,\ntime segment settings, per event display settings, specific ranks, threads,\nor processes that were focused from the Manage Process Panel, etc..\nuse the CL (Clear auxiliary settings) icon.  The CL icon will set\nthe view back to the original aggregated data view.", QString::null, this, SLOT( clusterAnalysisSelected()), fileTools, "show comparison analysis");
  }

  QPixmap *custom_comparison_icon = new QPixmap( custom_comparison_xpm );
  new QToolButton(*custom_comparison_icon, "CUSTOM COMPARISON: Show Custom Comparison report as built by user input:\ngenerate a performance statistics report as the result of the user\ncreating comparison columns and then selecting which experiments, ranks,\nthreads, or processes will comprise each column.", QString::null, this, SLOT( customizeExperimentsSelected()), fileTools, "show comparison analysis");
  // ----------------- End of the Analysis Icons

  toolbar_status_label = new QLabel(fileTools,"toolbar_status_label");
  // default setting to match default views
  toolbar_status_label->setText("Showing Functions Report:");
  fileTools->setStretchableWidget(toolbar_status_label);


#ifdef DEBUG_StatsPanel_toolbar
 printf("StatsPanel::generateToolBar, fileTools=0x%lx, toolBarFLAG=%d\n", fileTools, toolBarFLAG );
 printf("StatsPanel::generateToolBar, HIDE if toolBarFLAG is FALSE, SHOW if toolBarFLAG is TRUE, toolbar_status_label=0x%lx\n", toolbar_status_label );
 printf("ENTER StatsPanel::generateToolBar,before minMaxAverage check recycleFLAG=%d\n", recycleFLAG );
 printf("ENTER StatsPanel::generateToolBar,before minMaxAverage check this=0x%lx, currentUserSelectedReportStr.ascii()=%s\n", this, currentUserSelectedReportStr.ascii() );
 printf("ENTER StatsPanel::generateToolBar,before minMaxAverage check this=0x%lx, originatingUserSelectedReportStr.ascii()=%s\n", this, originatingUserSelectedReportStr.ascii() );
#endif

#if 1
  if (originatingUserSelectedReportStr.startsWith("minMaxAverage") ||
      originatingUserSelectedReportStr.startsWith("DefaultView") ||
     (originatingUserSelectedReportStr.startsWith("clusterAnalysis") &&
      currentUserSelectedReportStr.startsWith("Comparison")) ) {

    // Create a compare type button group
    QButtonGroup *vDisplayTypeBG = new QButtonGroup( 1, QGroupBox::Vertical, "View/Display Choice", fileTools);
    QToolTip::add(vDisplayTypeBG, tr("Select either Function, Statement, or Library view type.\nThen select an icon representing a display type.\nFor example: D for Default view, LB for Load Balance View, etc.\nTypical action is to select Statements as the view type and\nclick D to display the Default view with statements as the view level.") );

    vDisplayTypeBG->setExclusive( TRUE );

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::generateToolBar, in compare type button group, creation, fileTools=0x%lx)\n", fileTools);
#endif

    // insert 1 or 4 radiobuttons
    vDisplayTypeFunctionRB = new QRadioButton( "Functions", vDisplayTypeBG );
    connect( vDisplayTypeFunctionRB, SIGNAL( clicked() ), this, SLOT( displayUsingFunction() ) );

    bool full_display_by_menu = TRUE;
    if (currentCollectorStr.contains("mpi") || 
        currentCollectorStr.contains("io") || 
        currentCollectorStr.contains("pthreads") || 
        currentCollectorStr.contains("mem")) {
      full_display_by_menu = FALSE;
      vDisplayTypeStatementRB = NULL;
      vDisplayTypeLinkedObjectRB = NULL;
#if defined(HAVE_DYNINST)
      vDisplayTypeLoopRB = NULL;
#endif
    }

#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::generateToolBar, in compare type button group, creation, full_display_by_menu=%d)\n", full_display_by_menu);
#endif

    if (full_display_by_menu) {

      vDisplayTypeStatementRB = new QRadioButton( "Statements", vDisplayTypeBG );
      connect( vDisplayTypeStatementRB, SIGNAL( clicked() ), this, SLOT( displayUsingStatement() ) );

      vDisplayTypeLinkedObjectRB = new QRadioButton( "Linked Objects", vDisplayTypeBG );
      connect( vDisplayTypeLinkedObjectRB, SIGNAL( clicked() ), this, SLOT( displayUsingLinkedObject() ) );

#if defined(HAVE_DYNINST)
      vDisplayTypeLoopRB = new QRadioButton( "Loops", vDisplayTypeBG );
      connect( vDisplayTypeLoopRB, SIGNAL( clicked() ), this, SLOT( displayUsingLoop() ) );
#endif

#ifdef DEBUG_StatsPanel_toolbar
      printf("StatsPanel::generateToolBar, in compare type button group, creation, currentDisplayUsingType=%d)\n", currentDisplayUsingType);
#endif
      if (currentDisplayUsingType == displayUsingFunctionType) {
         vDisplayTypeFunctionRB->setChecked(TRUE);
         vDisplayTypeStatementRB->setChecked(FALSE);
         vDisplayTypeLinkedObjectRB->setChecked(FALSE);
#if defined(HAVE_DYNINST)
         vDisplayTypeLoopRB->setChecked(FALSE);
#endif

#ifdef DEBUG_StatsPanel_toolbar
         printf("StatsPanel::generateToolBar, in compare type button group, displayUsingFunctionType, currentDisplayUsingType=%d)\n", currentDisplayUsingType);
#endif
      } else if (currentDisplayUsingType == displayUsingStatementType) {
         vDisplayTypeFunctionRB->setChecked(FALSE);
         vDisplayTypeStatementRB->setChecked(TRUE);
         vDisplayTypeLinkedObjectRB->setChecked(FALSE);
#if defined(HAVE_DYNINST)
         vDisplayTypeLoopRB->setChecked(FALSE);
#endif

#ifdef DEBUG_StatsPanel_toolbar
         printf("StatsPanel::generateToolBar, in compare type button group, displayUsingStatementType, currentDisplayUsingType=%d)\n", currentDisplayUsingType);
#endif
      } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
         vDisplayTypeFunctionRB->setChecked(FALSE);
         vDisplayTypeStatementRB->setChecked(FALSE);
         vDisplayTypeLinkedObjectRB->setChecked(TRUE);
#if defined(HAVE_DYNINST)
         vDisplayTypeLoopRB->setChecked(FALSE);
      } else if (currentDisplayUsingType == displayUsingLoopType) {
         vDisplayTypeFunctionRB->setChecked(FALSE);
         vDisplayTypeStatementRB->setChecked(FALSE);
         vDisplayTypeLinkedObjectRB->setChecked(FALSE);
         vDisplayTypeLoopRB->setChecked(TRUE);
#endif
      }
    } else {
         vDisplayTypeFunctionRB->setChecked(TRUE);
#ifdef DEBUG_StatsPanel_toolbar
         printf("StatsPanel::generateToolBar, in compare type button group, default, currentDisplayUsingType=%d)\n", currentDisplayUsingType);
#endif
    }
#ifdef DEBUG_StatsPanel_toolbar
    printf("StatsPanel::generateToolBar, in compare type button group, displayUsingLOType, currentDisplayUsingType=%d)\n", currentDisplayUsingType);
#endif

#if 1
    if (vDisplayTypeFunctionRB) {
      vDisplayTypeFunctionRB->show();
    }
    if (vDisplayTypeStatementRB) {
      vDisplayTypeStatementRB->show();
    }
    if (vDisplayTypeLinkedObjectRB) {
      vDisplayTypeLinkedObjectRB->show();
    }
#if defined(HAVE_DYNINST)
    if (vDisplayTypeLoopRB) {
      vDisplayTypeLoopRB->show();
    }
#endif
#else
    if (vDisplayTypeBG) {
      vDisplayTypeBG->show();
    }
#endif
  }
#endif

  if( toolBarFLAG == TRUE ) {
   fileTools->show();
#ifdef DEBUG_StatsPanel_toolbar
   printf("StatsPanel::generateToolBar, (11985) SHOW,fileTools=0x%lx)\n", fileTools);
#endif
  } else {
   fileTools->hide();
#ifdef DEBUG_StatsPanel_toolbar
   printf("StatsPanel::generateToolBar, (11990) HIDE,fileTools=0x%lx)\n", fileTools);
#endif
  }

//    fileTools->hide();
} else {
#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::generateToolBar, IFCHECK FAILS, before calling fileTools->clear() fileTools=%d\n", fileTools);
#endif
} 

 lastCollectorStr = currentCollectorStr;
 lastUserSelectedReportStr = currentUserSelectedReportStr;

#ifdef DEBUG_StatsPanel_toolbar
 printf("EXIT StatsPanel::generateToolBar, lastCollectorStr.ascii()=%s\n", lastCollectorStr.ascii() );
 printf("EXIT StatsPanel::generateToolBar, fileTools=0x%lx)\n", fileTools);
#endif


}

void
StatsPanel::defaultViewSelected()
{
  QString displayType;

  originatingUserSelectedReportStr = "DefaultView";

#ifdef DEBUG_StatsPanel_toolbar
  printf("ENTER defaultViewSelected(), currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
  printf("ENTER defaultViewSelected(), originatingUserSelectedReportStr(%s)\n", originatingUserSelectedReportStr.ascii() );
  printf("ENTER defaultViewSelected(), traceAddition=(%s), currentThreadsStr=(%s)\n", traceAddition.ascii(), currentThreadsStr.ascii() );
#endif

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;

// This was changed on 10/13/2011 in order to make the GUI more flexible.
// The Default view will now honor previous metric selections, time segments, specific thread or rank selections
// Users must now use the CL (clear icon) to clear these selections away.
#if 0
  // Clear all the -m modifiers (metrics)
  clearModifiers();
#endif

  if (currentDisplayUsingType == displayUsingFunctionType) {
      displayType = "functions";
      currentUserSelectedReportStr = "Functions";
      toolbar_status_label->setText("Generating Functions Report...");
  } else if (currentDisplayUsingType == displayUsingStatementType) {
      displayType = "statements";
      currentUserSelectedReportStr = "Statements";
      toolbar_status_label->setText("Generating Statements Report...");
  } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
      displayType = "linkedobjects";
      currentUserSelectedReportStr = "LinkedObjects";
      toolbar_status_label->setText("Generating Linked Objects Report...");
#if defined(HAVE_DYNINST)
  } else if (currentDisplayUsingType == displayUsingLoopType) {
      displayType = "loops";
      currentUserSelectedReportStr = "Loops";
      toolbar_status_label->setText("Generating Loops Report...");
#endif
  }
  // Clear all thread specific options
//  currentThreadsStr = QString::null;

#ifdef DEBUG_StatsPanel_toolbar
  printf("StatsPanel::defaultViewSelected(), calling updateStatsPanelData\n" );
  printf("StatsPanel::defaultViewSelected(), calling updateStatsPanelData, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

  updateStatsPanelData(DO_FORCE_UPDATE);

  if (currentDisplayUsingType == displayUsingFunctionType) {
      toolbar_status_label->setText("Showing Functions Report:");
  } else if (currentDisplayUsingType == displayUsingStatementType) {
      toolbar_status_label->setText("Showing Statements Report...");
  } else if (currentDisplayUsingType == displayUsingLinkedObjectType) {
      toolbar_status_label->setText("Showing Linked Objects Report...");
#if defined(HAVE_DYNINST)
  } else if (currentDisplayUsingType == displayUsingLoopType) {
      toolbar_status_label->setText("Showing Loops Report...");
#endif
  }
}


void
StatsPanel::functionsSelected()
{

#ifdef DEBUG_StatsPanel
  printf("functionsSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
  printf("  traceAddition=(%s), currentThreadsStr=(%s)\n", traceAddition.ascii(), currentThreadsStr.ascii() );
#endif

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;

  // Clear all the -m modifiers (metrics)
  clearModifiers();

  originatingUserSelectedReportStr = "Functions";
  currentUserSelectedReportStr = "Functions";
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Functions Report...");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::functionsSelected(), calling updateStatsPanelData\n" );
  printf("StatsPanel::functionsSelected(), calling updateStatsPanelData, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Functions Report:");
}

void
StatsPanel::linkedObjectsSelected()
{
#ifdef DEBUG_StatsPanel
  printf("linkedObjectsSelected()\n");
  printf("linkedObjectsSelected, currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  // Clear all the -m modifiers (metrics)
  clearModifiers();

  originatingUserSelectedReportStr = "LinkedObjects";
  currentUserSelectedReportStr = "LinkedObjects";

  toolbar_status_label->setText("Generating Linked Objects Report...");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::linkedObjectsSelected(), calling updateStatsPanelData\n" );
#endif

  toolbar_status_label->setText("Showing Linked Objects Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Linked Objects Report:");
}


void
StatsPanel::statementsSelected()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::statementsSelected()\n");
  printf("StatsPanel::currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "Statements";
  currentUserSelectedReportStr = "Statements";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  // Clear all the -m modifiers (metrics)
  clearModifiers();

  toolbar_status_label->setText("Generating Statements Report...");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::statementsSelected(), calling updateStatsPanelData\n" );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Statements Report:");
}

void
StatsPanel::statementsByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("Enter StatsPanel::statementsByFunctionSelected(), currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "Statements by Function";
  currentUserSelectedReportStr = "Statements by Function";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Statements by Function Report...");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::statementsByFunctionSelected, calling updateStatsPanelData, currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", 
         currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Statements by Function Report:");
}

void
StatsPanel::calltreesSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "CallTrees";
  currentUserSelectedReportStr = "CallTrees";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  // Clear all the -m modifiers (metrics)
  clearModifiers();

  toolbar_status_label->setText("Generating CallTrees Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees Report:");
}

void
StatsPanel::calltreesByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "CallTrees by Function";
  currentUserSelectedReportStr = "CallTrees by Function";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating CallTrees by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees by Function Report:");
}

void
StatsPanel::calltreesFullStackSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesFullStackSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "CallTrees,FullStack";
  currentUserSelectedReportStr = "CallTrees,FullStack";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating CallTrees,FullStack Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees,FullStack Report:");
}

void
StatsPanel::calltreesFullStackByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesFullStackByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "CallTrees,FullStack by Function";
  currentUserSelectedReportStr = "CallTrees,FullStack by Function";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating CallTrees,FullStack by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees,FullStack by Function Report:");
}

void
StatsPanel::tracebacksSelected()
{
#ifdef DEBUG_StatsPanel
  printf("tracebacksSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "TraceBacks";
  currentUserSelectedReportStr = "TraceBacks";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  // Clear all the -m modifiers (metrics)
  clearModifiers();

  toolbar_status_label->setText("Generating TraceBacks Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks Report:");
}

void
StatsPanel::tracebacksByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("tracebacksByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  originatingUserSelectedReportStr = "TraceBacks by Function";
  currentUserSelectedReportStr = "TraceBacks by Function";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating TraceBacks by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks by Function Report:");
}

void
StatsPanel::tracebacksFullStackSelected()
{
#ifdef DEBUG_StatsPanel
  printf("tracebacksFullStackSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "TraceBacks,FullStack";
  currentUserSelectedReportStr = "TraceBacks,FullStack";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating TraceBacks,FullStack Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks,FullStack Report:");
}

void
StatsPanel::tracebacksFullStackByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("tracebacksFullStackByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "TraceBacks,FullStack by Function";
  currentUserSelectedReportStr = "TraceBacks,FullStack by Function";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating TraceBacks,FullStack by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks,FullStack by Function Report:");
}

void
StatsPanel::butterflySelected()
{
#ifdef DEBUG_StatsPanel
 printf("butterflySelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "Butterfly";
  currentUserSelectedReportStr = "Butterfly";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Butterfly Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Butterfly Report:");
}

void
StatsPanel::hotCallpathSelected()
{
#ifdef DEBUG_StatsPanel
 printf("hotCallpathSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "HotCallPath";
  currentUserSelectedReportStr = "HotCallPath";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Hot Callpath Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Hot Callpath Report:");
}

void
StatsPanel::memLeakedpathSelected()
{
#ifdef DEBUG_StatsPanel
 printf("memLeakedpathSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "memLeakedPath";
  currentUserSelectedReportStr = "memLeakedPath";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Memory Leaked Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Memory Leaked Report:");
}

void
StatsPanel::memHighwaterpathSelected()
{
#ifdef DEBUG_StatsPanel
 printf("memHighwaterpathSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "memHighwaterPath";
  currentUserSelectedReportStr = "memHighwaterPath";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Memory Highwater Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Memory Highwater Report:");
}


void
StatsPanel::memUniquepathSelected()
{
#ifdef DEBUG_StatsPanel
 printf("memUniquepathSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  originatingUserSelectedReportStr = "memUniquePath";
  currentUserSelectedReportStr = "memUniquePath";

  // Clear all trace display - this should be a purely function view
  traceAddition = QString::null;
  // Clear all thread specific options
  currentThreadsStr = QString::null;

  toolbar_status_label->setText("Generating Memory Unique Paths Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Memory Unique Paths Report:");
}

