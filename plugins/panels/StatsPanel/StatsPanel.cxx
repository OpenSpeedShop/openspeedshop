////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;
#include "CLIInterface.hxx"

#include "ManageProcessesPanel.hxx"



// These are the pie chart colors..
static char *hotToCold_color_names[] = { 
  "red", 
  "magenta",
  "blue",
  "cyan",
  "green",
  "yellow",
  "gray",
  "lightGray"
  "darkRed",
  "darkMagenta",
  "darkBlue",
  "darkCyan",
  "darkGreen",
  "darkYellow",
};
static char *coldToHot_color_names[] = { 
  "darkYellow",
  "darkGreen",
  "darkCyan",
  "darkBlue",
  "darkMagenta",
  "darkRed",
  "lightGray"
  "gray",
  "yellow",
  "green",
  "cyan",
  "blue",
  "magenta",
  "red", 
};
#define MAX_COLOR_CNT 14


#define PTI "Present Trace Information"

#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "FocusObject.hxx"
#include "FocusCompareObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"

#include "preference_plugin_info.hxx"
#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;


template <class T>
struct sort_ascending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second < y.second;
    }
};
template <class T>
struct sort_descending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second > y.second;
    }
};

#include <map>

template <typename T>
void GetMetricByStatementOfFileInThread(
    const Collector& collector,
    const std::string& metric,
    const TimeInterval& interval,
    const Thread& thread,
    const Path& file,
    SmartPtr<std::map<int, T > >& result)
{
    // Allocate (if necessary) a new map of statement line numbers to values
    if(result.isNull())
        result = SmartPtr<std::map<int, T > >(new std::map<int, T >());
    Assert(!result.isNull());

    // Get the set of statements for the specified file in this thread
    std::set<Statement> objects = thread.getStatementsBySourceFile(file);

    // Get the summation reduced metric values for these statements
    SmartPtr<std::map<Statement, std::map<Thread, T > > > individual;
#if 0
// Begin debug
Metadata cm = collector.getMetadata();
QString name = QString(cm.getUniqueId().c_str());
printf("collector name=(%s)\n", name.ascii() );
printf("metric=(%s)\n", metric.c_str() );
// End debug
#endif // 0
// printf("query: metric=(%s)\n", metric.c_str() );
    Queries::GetMetricValues(collector, metric, interval,
			     Queries::MakeThreadGroup(thread), 
			     objects, individual);
    SmartPtr<std::map<Statement, T > > reduced =
	Queries::Reduction::Apply(individual, Queries::Reduction::Summation);
    individual = SmartPtr<std::map<Statement, std::map<Thread, T > > >();

    // Merge the temporary reduction into the actual results while stripping
    // out everything but the line numbers for the statments
    for(typename std::map<Statement, T >::const_iterator
	    i = reduced->begin(); i != reduced->end(); ++i)
	if(result->find(i->first.getLine()) == result->end())
	    result->insert(std::make_pair(i->first.getLine(), i->second));
	else
	    (*result)[i->first.getLine()] += i->second;
}



// This is the stream the cli will write the output from the 
// expView commands that are sent its way.
class SPOutputClass : public ss_ostream
{
  public:
    StatsPanel *sp;
    void setSP(StatsPanel *_sp) { sp = _sp;line_buffer = QString::null; };
    QString line_buffer;
  private:
    virtual void output_string (std::string s)
    {
       line_buffer += s.c_str();
       if( QString(s).contains("\n") )
       {
         QString *data = new QString(line_buffer);
//         sp->postCustomEvent(data);
           sp->outputCLIData(data);
         line_buffer = QString::null;
       }
    }
    virtual void flush_ostream ()
    {
      qApp->flushX();
    }
};


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
       if( QString(s).contains("\n") )
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



/*! Create a Stats Panel.
*/
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
// printf("StatsPanel() entered\n");
  setCaption("StatsPanel");

  IOtraceFLAG = FALSE;
  MPItraceFLAG = FALSE;

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
  about = QString::null;
  lastAbout = QString::null;
  // In an attempt to optimize the update of this panel;
  // If the data file is static (i.e. read from a file or 
  // the processes status is terminated) and the command is
  // the same, don't update this panel. 
  lastCommand = QString::null;
  staticDataFLAG = false;
// printf("currentItemIndex initialized to 0\n");

  f = NULL;
  modifierMenu = NULL;

  mpiModifierMenu = NULL;
  mpitModifierMenu = NULL;
  ioModifierMenu = NULL;
  iotModifierMenu = NULL;
  hwcModifierMenu = NULL;
  hwctimeModifierMenu = NULL;
  pcsampModifierMenu = NULL;
  usertimeModifierMenu = NULL;

  mpi_menu = NULL;
  io_menu = NULL;
  hwc_menu = NULL;
  hwctime_menu = NULL;
  pcsamp_menu = NULL;
  usertime_menu = NULL;

  list_of_modifiers.clear(); // This is the global known list of modifiers.

  list_of_mpi_modifiers.clear();
  current_list_of_mpi_modifiers.clear();  // This is this list of user selected modifiers.
  current_list_of_mpit_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_io_modifiers.clear();
  current_list_of_io_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_iot_modifiers.clear();
  current_list_of_iot_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_hwc_modifiers.clear();
  current_list_of_hwc_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_hwctime_modifiers.clear();
  current_list_of_hwctime_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_pcsamp_modifiers.clear();
  current_list_of_pcsamp_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_usertime_modifiers.clear();
  current_list_of_usertime_modifiers.clear();  // This is this list of user selected modifiers.

  current_list_of_modifiers.clear();  // This is this list of user selected modifiers.
  selectedFunctionStr = QString::null;
  threadMenu = NULL;
  currentMetricStr = QString::null;
  currentUserSelectedReportStr = QString::null;
  metricHeaderTypeArray = NULL;
  currentThreadStr = QString::null;
  currentCollectorStr = QString::null;
  collectorStrFromMenu = QString::null;
  groupID = ao->int_data;
  expID = -1;
  descending_sort = true;
  TotalTime = 0;

  if( ao->loadedFromSavedFile == TRUE )
  {
//printf("StatsPanel:: static data!!!\n");
    staticDataFLAG = TRUE;
  }

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");

  splitterA->setOrientation( QSplitter::Horizontal );

  cf = new SPChartForm(this, splitterA, getName(), 0);
  cf->setCaption("SPChartFormIntoSplitterA");

  splv = new SPListView(this, splitterA, getName(), 0);
  splv->setSorting ( -1 );

  connect( splv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

  connect( splv, SIGNAL(returnPressed(QListViewItem *)), this, SLOT( returnPressed( QListViewItem* )) );

  int width = pc->width();
  int height = pc->height();
  QValueList<int> sizeList;
  sizeList.clear();
  if( splitterA->orientation() == QSplitter::Vertical )
  {
    sizeList.push_back((int)(height/4));
    sizeList.push_back(height-(int)(height/4));
  } else
  {
    sizeList.push_back((int)(width/4));
    sizeList.push_back(width-(int)(width/4));
  }
  splitterA->setSizes(sizeList);


  frameLayout->addWidget( splitterA );

  if( pc->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() )
  {
    chartFLAG = TRUE;
    cf->show();
  } else
  {
    chartFLAG = FALSE;
    cf->hide();
  }
  statsFLAG = TRUE;
  splv->show();

  splitterA->show();

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), groupID);
  setName(name_buffer);

  spoclass = new SPOutputClass();
  spoclass->setSP(this);




}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanel::~StatsPanel()
{
  // Delete anything you new'd from the constructor.
  nprintf( DEBUG_CONST_DESTRUCT ) ("  StatsPanel::~StatsPanel() destructor called\n");
// printf("  StatsPanel::~StatsPanel() destructor called\n");


  // We must reset the directing of output, otherwise the cli goes nuts
  // trying to figure out where the output is suppose to go.
  resetRedirect();


  // We allocated a Collect, we must delete it.   Otherwise the framework
  // issues a warning on exit.
  if( currentCollector )
  {
// printf("Destructor delete the currentCollector\n");
    delete currentCollector;
  }

  delete spoclass;

// printf("  StatsPanel::~StatsPanel() destructor finished\n");
}

void
StatsPanel::languageChange()
{
  // Set language specific information here.
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
  nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() msg->msgType = (%s)\n", msgObject->msgType.ascii() );
  if( msgObject->msgType == getName() && recycleFLAG == TRUE )
  {
    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }

// printf("StatsPanel::listener(%s)\n", msgObject->msgType.ascii() );

  if(  msgObject->msgType  == "FocusObject" && recycleFLAG == TRUE )
  {
// printf("StatsPanel got a new FocusObject\n");
    FocusObject *msg = (FocusObject *)msgObject;
// msg->print();
    expID = msg->expID;

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
    } else
    {
//printf("Here in StatsPanel::listener()\n");
//msg->print();
      currentThreadGroupStrList.clear();
      currentThreadsStr = QString::null;
      std::vector<HostPidPair>::const_iterator sit = msg->host_pid_vector.begin();
      for(std::vector<HostPidPair>::const_iterator
                      sit = msg->host_pid_vector.begin();
                      sit != msg->host_pid_vector.end(); ++sit)
      {
        if( sit->first.size() )
        {
          currentThreadsStr += QString(" -h %1 -p %2").arg(sit->first.c_str()).arg(sit->second.c_str() );
        } else 
        {
          currentThreadsStr += QString(" -p %1").arg(sit->second.c_str() );
        }
        currentThreadGroupStrList.push_back( sit->second.c_str() );
      }
    }
//printf("currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
// Begin determine if there's mpi stats
try
{
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    Experiment *fw_experiment = eo->FW();
    CollectorGroup cgrp = fw_experiment->getCollectors();
// printf("Is says you have %d collectors.\n", cgrp.size() );
    if( cgrp.size() == 0 )
    {
      fprintf(stderr, "There are no known collectors for this experiment.\n");
    }
    for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
    {
      Collector collector = *ci;
      Metadata cm = collector.getMetadata();
      QString name = QString(cm.getUniqueId().c_str());

// printf("B: Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
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

// printf("StatsPanel::listener call updateStatsPanelData  Do we need to update?\n");
    updateStatsPanelData();
    if( msg->raiseFLAG == TRUE )
    {
      getPanelContainer()->raisePanel(this);
    }
// now focus a source file that's listening....
// printf("Now focus the source panel, if it's up..\n");
    //First get the first item...
    QListViewItemIterator it( splv );
    QListViewItem *item = *it;

    // Now call the match routine, this should focus any source panels.
    if( item && matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) ) )
    {
    //printf("match\n");
        return 1;
    } else
    {
    //printf("no match\n");
        return 0;
    }
  } else if(  msgObject->msgType  == "FocusCompareObject" && recycleFLAG == TRUE )
  {
// printf("StatsPanel got a new FocusCompareObject\n");
    FocusCompareObject *msg = (FocusCompareObject *)msgObject;
// msg->print();
    expID = msg->expID;

    if( !msg->compare_command.isEmpty()  )
    {
// printf("StatsPanel::listener() call updatestastPanelData(%s)\n", msg->compare_command.ascii() );
      updateStatsPanelData(msg->compare_command);
// printf("StatsPanel::listener() called \n");
    }

    if( msg->raiseFLAG == TRUE )
    {
// printf("StatsPanel::listener() raise this panel.. \n");
      getPanelContainer()->raisePanel(this);
    }
#ifdef LATER
// now focus a source file that's listening....
// printf("Now focus the source panel, if it's up..\n");
    //First get the first item...
    QListViewItemIterator it( splv );
    QListViewItem *item = *it;

    // Now call the match routine, this should focus any source panels.
    if( item && matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) ) )
    {
    //printf("match\n");
        return 1;
    } else
    {
    //printf("no match\n");
        return 0;
    }
#endif // LATER
  } else if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
// printf("UpdateExperimentDataObject\n");

    UpdateObject *msg = (UpdateObject *)msgObject;
    if( msg->expID == -1 )
    {
// printf("We got the command=(%s)\n", msg->experiment_name.ascii() );
      QString command = msg->experiment_name;
      updateStatsPanelData(command);
      return(1);
    }
    
    expID = msg->expID;

// Begin determine if there's mpi stats
try
{
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    Experiment *fw_experiment = eo->FW();
    CollectorGroup cgrp = fw_experiment->getCollectors();
// printf("Is says you have %d collectors.\n", cgrp.size() );
    if( cgrp.size() == 0 )
    {
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

// printf("Call updateStatsPanelData() \n");
    updateStatsPanelData();
    if( msg->raiseFLAG )
    {
    if( msg->raiseFLAG )
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
// printf("Call (make this one smarter) updateStatsPanelData() \n");
    updateStatsPanelData();
  } else if( msgObject->msgType == "SaveAsObject" )
  {
    SaveAsObject *sao = (SaveAsObject *)msg;
// printf("StatsPanel!!!!! Save as!\n");
    if( !sao )
    {
      return 0;  // 0 means, did not act on message.
    }
//    exportData(sao->f, sao->ts);
// Currently you're not passing the file descriptor down... you need to.sao->f, sao->ts);
    f = sao->f;
    exportData();
  }


  return 0;  // 0 means, did not want this message and did not act on anything.
}

bool
StatsPanel::menu( QPopupMenu* contextMenu)
{
// printf("StatsPanel::menu() entered.\n");

  Panel::menu(contextMenu);

// printf("B: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
// printf("B: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );


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
  // Over all the collectors....
  // Round up the metrics ....
  // Create a menu of metrics....
  contextMenu->setCheckable(TRUE);
  int mid = -1;
  QString defaultStatsReportStr = QString::null;

  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
  {
     std::string collector_name = (std::string)*it;
    if( QString(collector_name).startsWith("mpi") )
    {
// printf("Generate an mpi* menu\n");
      generateMPIMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("io") )
    {
// printf("Generate an io* menu\n");
      generateIOMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("hwctime") )
    {
// printf("Generate an hwctime menu\n");
      generateHWCTimeMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("hwc") )
    {
// printf("Generate an hwc menu\n");
      generateHWCMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("usertime") )
    {
// printf("Generate an usertime menu.\n");
      generateUserTimeMenu();
    } else if( QString(collector_name).startsWith("pcsamp") )
    {
// printf("Generate a pcsamp menu\n");
      generatePCSampMenu();
    } else
    {
// printf("Generate an other (%s) menu\n", collector_name.c_str() );
      generateGenericMenu();
    }

  }

  if( threadMenu )
  {
    delete threadMenu;
  }
  threadMenu = new QPopupMenu(this);
  int MAX_PROC_MENU_DISPLAY = 8;
  if( list_of_pids.size() <= MAX_PROC_MENU_DISPLAY )
  {
    contextMenu->insertItem(QString("Show Thread/Process"), threadMenu);
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int pid = (int64_t)*it;
// printf("pid=(%d)\n", pid );
      QString pidStr = QString("%1").arg(pid);
      int mid = threadMenu->insertItem(pidStr);
      threadMenu->setCheckable(TRUE);
      if( currentThreadStr.isEmpty() || currentThreadStr == pidStr )
      {
        currentThreadStr = pidStr;
//        threadMenu->setItemChecked(mid, TRUE);
      }
      for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
      {
        QString ts = (QString)*it;
        if( ts == pidStr )
        {
          threadMenu->setItemChecked(mid, TRUE);
        }
      }
    }
    connect(threadMenu, SIGNAL( activated(int) ),
        this, SLOT(threadSelected(int)) );
  } else
  {
#ifdef EXPERIMENT_PANEL_AND_STATSPANEL
    qaction = new QAction( this,  "manageProcessesMenu");
    qaction->addTo( contextMenu );
    qaction->setText( "Manage Processes..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( manageProcessesSelected() ) );
    qaction->setStatusTip( tr(QString("There are over %1 processes to manage.  This brings up the Manage Processes\nPanel which is designed to handle large number of procesess/threads.").arg(MAX_PROC_MENU_DISPLAY)) );
#endif // EXPERIMENT_PANEL_AND_STATSPANEL
  }

  contextMenu->insertSeparator();

#ifdef EXPERIMENT_PANEL_AND_STATSPANEL
  qaction = new QAction( this,  "compareAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Compare Experiments/Customize StatsPanel..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( compareSelected() ) );
  qaction->setStatusTip( tr("Compare one experiment to another, one thread to another, or customize the StatsPanel report.") );

  contextMenu->insertSeparator();
#endif // EXPERIMENT_PANEL_AND_STATSPANEL

  int id = 0;
  QPopupMenu *columnsMenu = new QPopupMenu(this);
  columnsMenu->setCaption("Columns Menu");
  contextMenu->insertItem("&Columns Menu", columnsMenu, CTRL+Key_C);

  for( ColumnList::Iterator pit = columnHeaderList.begin();
           pit != columnHeaderList.end();
           ++pit )
  { 
    QString s = (QString)*pit;
    columnsMenu->insertItem(s, this, SLOT(doOption(int)), CTRL+Key_1, id, -1);
    if( splv->columnWidth(id) )
    {
      columnsMenu->setItemChecked(id, TRUE);
    } else
    {
      columnsMenu->setItemChecked(id, FALSE);
    }
    id++;
  }

  qaction = new QAction( this,  "exportDataAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Report Data..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( exportData() ) );
  qaction->setStatusTip( tr("Save the report's data to an ascii file.") );

  if( splv->selectedItem() )
  {
//    contextMenu->insertItem("Tell Me MORE about %d!!!", this, SLOT(details()), CTRL+Key_1 );
    qaction = new QAction( this,  "gotoSource");
    qaction->addTo( contextMenu );
    qaction->setText( "Go to source location..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSource() ) );
    qaction->setStatusTip( tr("Position at source location of this item.") );
  }


  contextMenu->insertSeparator();

  qaction = new QAction( this,  "re-orientate");
  qaction->addTo( contextMenu );
  qaction->setText( "Re-orientate" );
  connect( qaction, SIGNAL( activated() ), this, SLOT( setOrientation() ) );
  qaction->setStatusTip( tr("Display chart/statistics horizontal/vertical.") );

  if( chartFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are shown, hide the graphic chart.") );
  } else
  {
    qaction = new QAction( this,  "showChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are available, show the graphic chart.") );
  }
  if( statsFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Hide the statistics display.") );
  } else
  {
    qaction = new QAction( this,  "showStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Show the statistics display.") );
  }

  return( TRUE );
}

void
StatsPanel::generateModifierMenu(QPopupMenu *menu, std::list<std::string> modifier_list, std::list<std::string> current_list)
{
// printf("StatsPanel::generateModifierMenu() entered\n");
  menu->setCheckable(TRUE);
  for( std::list<std::string>::const_iterator it = modifier_list.begin();
          it != modifier_list.end(); it++ )
  {
    std::string modifier = (std::string)*it;

// printf("modifier = (%s)\n", modifier.c_str() );

    QString s = QString(modifier.c_str() );
     int mid = menu->insertItem(s);
    for( std::list<std::string>::const_iterator it = current_list.begin();
         it != current_list.end(); it++ )
    {
      std::string current_modifier = (std::string)*it;
// printf("building menu : current_list here's one (%s)\n", current_modifier.c_str() );
      if( modifier == current_modifier )
      {
// printf("WE have a match to check\n");
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
// printf("StatsPanel::showChart() entered\n");
  if( chartFLAG == TRUE )
  {
    chartFLAG = FALSE;
    cf->hide();
  } else
  {
    chartFLAG = TRUE;
// printf("lastCommand=(%s)\n", lastCommand.ascii() );
    cf->show();
    if( !lastCommand.startsWith("cview") )
    {
      lastCommand = QString::null;  // This will force a redraw of the data.
      // I'm not sure why, but the text won't draw unless the 
      // piechart is visible.
      updatePanel();
    } else
    {
      cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);
      updatePanel();
    }
    cf->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( chartFLAG == FALSE && statsFLAG == FALSE )
  {
    statsFLAG = TRUE;
  }
}


void
StatsPanel::showStats()
{
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
}


/*! Reset the orientation of the graph/text relationship with setOrientation */
void
StatsPanel::setOrientation()
{
// printf("StatsPanel::setOrientation() entered\n");
  Orientation o = splitterA->orientation();
  if( o == QSplitter::Vertical )
  {
    splitterA->setOrientation(QSplitter::Horizontal);
  } else
  {
    splitterA->setOrientation(QSplitter::Vertical);
  }
}


/*! Go to source menu item was selected. */
void
StatsPanel::details()
{
  printf("details() menu selected.\n");
}

void
StatsPanel::exportData()
{
// printf("exportData() menu selected.\n");
  Orientation o = splitterA->orientation();
  QListViewItemIterator it( splv );
  int cols =  splv->columns();
  int i=0;
  QString fileName = "StatsPanel.txt";
  QString dirName = QString::null;

  if( f == NULL)
  {
    QFileDialog *fd = new QFileDialog(this, "save_StatsPanelData:", TRUE );
    fd->setCaption( QFileDialog::tr("Save StatsPanel data:") );
    fd->setMode( QFileDialog::AnyFile );
    fd->setSelection(fileName);
    QString types( 
                      "Data files (*.dat);;"
                      "Text files (*.txt);;"
                      "Any File (*.*);;"
                      );
    fd->setFilters( types );
    // Pick the initial default types to put out.
    const QString mask = QString("*.txt");
    fd->setSelectedFilter( mask );
    fd->setDir(dirName);
  
    if( fd->exec() == QDialog::Accepted )
    {
      fileName = fd->selectedFile();
    }
    
    if( !fileName.isEmpty() )
    {
        f = new QFile(fileName);
        f->open(IO_WriteOnly );
    }
  }

  bool datFLAG = FALSE;
  if( fileName.endsWith(".dat") )
  {
    datFLAG = TRUE;
  }

  if( f != NULL )
  {
    // Write out the header info
    QString line = QString("  ");
    for(i=0;i<cols;i++)
    {
      for(i=0;i<cols;i++)
      {
        if( datFLAG == TRUE )
        {
          if( i < cols-1 )
          {
            line += QString(splv->columnText(i))+"; ";
          } else
          {
            line += QString(splv->columnText(i))+" ";
          }
        } else
        {
          line += QString(splv->columnText(i))+" ";
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
          if( i < cols-1 )
          {
            line += QString(item->text(i))+"; ";
          } else
          {
            line += QString(item->text(i))+" ";
          }
        } else
        {
          line += QString(item->text(i))+" ";
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
// printf("updatePanel() about to call updateStatsPanelData()\n");
  updateStatsPanelData(lastCommand);
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSource(bool use_current_item)
{
  QListViewItem *lvi = NULL;

  if( use_current_item )
  {
// printf("gotoSource() menu selected.\n");
    lvi = currentItem;
  } else 
  {
    lvi =  splv->selectedItem();
  }

  itemSelected(lvi);
}

#include "AboutDialog.hxx"
void
StatsPanel::aboutSelected()
{
  aboutOutputString = QString("%1\n\n").arg(about);

  QString command = QString::null;

  AboutOutputClass *aboutOutputClass = NULL;

  int cviewinfo_index = lastCommand.find("cview ");
  if( cviewinfo_index != -1 )
  {
    cviewinfo_index += 6;
    command = QString("cviewinfo ")+lastCommand.mid(cviewinfo_index);
// printf("fire off new command (%s)\n", command.ascii() );
    QString info_str = QString::null;
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();

    aboutOutputClass = new AboutOutputClass();
    aboutOutputClass->setSP(this);

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    Redirect_Window_Output(cli->wid, aboutOutputClass, aboutOutputClass);

    QApplication::setOverrideCursor(QCursor::WaitCursor);
    InputLineObject *clip = cli->run_Append_Input_String( wid, (char *)command.ascii());

    Input_Line_Status status = ILO_UNKNOWN;

    while( !clip->Semantics_Complete() )
    {
      status = cli->checkStatus(clip);
      if( !status || status == ILO_ERROR )
      { // An error occurred.... A message should have been posted.. return;
        QApplication::restoreOverrideCursor();
        if( clip ) 
        {
          clip->Set_Results_Used();
        }
        return;
      }
  
      qApp->processEvents(1000);
  
      if( !cli->shouldWeContinue() )
      {
// printf("RETURN FALSE!   COMMAND FAILED!\n");
        QApplication::restoreOverrideCursor();
        if( clip ) 
        {
          clip->Set_Results_Used();
        }
        return;
      }

      sleep(1);
    }
    QApplication::restoreOverrideCursor();

    //Test putting the output to statspanel stream.
    Default_TLI_Line_Output(clip);
  
    if( clip )
    {
      clip->Set_Results_Used();
    }
  }

  AboutDialog *aboutDialog = new AboutDialog(this, "StatsPanel Context:", FALSE, 0, aboutOutputString);
  aboutDialog->show();

  if( aboutOutputClass )
  {
    resetRedirect();
    delete aboutOutputClass;
  }
}

void
StatsPanel::MPItraceSelected()
{
  if( MPItraceFLAG == TRUE )
  {
    MPItraceFLAG = FALSE;
  } else
  {
    MPItraceFLAG = TRUE;
  }
}

void
StatsPanel::IOtraceSelected()
{
  if( IOtraceFLAG == TRUE )
  {
    IOtraceFLAG = FALSE;
  } else
  {
    IOtraceFLAG = TRUE;
  }
}


/*! Compare item was selected. */
void
StatsPanel::compareSelected()
{
  QString name = QString("CustomizeStatsPanel [%1]").arg(expID);

  Panel *comparePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( comparePanel )
  { 
    nprintf( DEBUG_PANELS ) ("comparePanel() found comparePanel found.. raise it.\n");
    getPanelContainer()->raisePanel(comparePanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("comparePanel() no comparePanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    comparePanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("CustomizeStatsPanel", startPC, ao);
    delete ao;
  }
}

/*! Compare item was selected. */
void
StatsPanel::manageProcessesSelected()
{
// printf("manageProcessesSelected() menu selected.\n");
  QString name = QString("ManageProcessesPanel [%1]").arg(expID);


  Panel *manageProcessesPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessesPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() found ManageProcessesPanel found.. raise it.\n");
    getPanelContainer()->raisePanel(manageProcessesPanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() no ManageProcessesPanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    manageProcessesPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", startPC, ao);
    delete ao;
  }

  if( manageProcessesPanel )
  {
//    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", manageProcessesPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, "pcsamp", 1);
      manageProcessesPanel->listener( (void *)msg );
    }
  }

}

void
StatsPanel::itemSelected(int index)
{
// printf("StatsPanel::itemSelected(%d)\n", index);
  QListViewItemIterator it( splv );
  int i = 0;
  while( it.current() )
  {
    QListViewItem *item = *it;
// printf("%d == %d\n", i, index);
    if( i == index )
    {
      currentItem = (SPListViewItem *)item;
      currentItemIndex = index;
// printf("A: currentItemIndex set to %d\n", currentItemIndex);
// highlight the list item
// Now call the action routine.
      splv->setSelected((QListViewItem *)item, TRUE);
      itemSelected(item);
      break;
    }
    i++;
    it++;
  }
    
}

void
StatsPanel::returnPressed(QListViewItem *item)
{
  if( lastCommand.contains("Butterfly") )
  {
    updateStatsPanelData();
  } else
  {
    itemSelected( item );
  }
}

void
StatsPanel::itemSelected(QListViewItem *item)
{
// printf("StatsPanel::itemSelected(QListViewItem *) item=%s\n", item->text(0).ascii() );

  if( item )
  {
// printf("  item->depth()=%d\n", item->depth() );

    SPListViewItem *nitem = (SPListViewItem *)item;
    int index = 0;

    if( currentUserSelectedReportStr != "Butterfly" )
    {
      while( nitem->parent() )
      {
// printf("looking for 0x%x\n", nitem->parent() );
        nitem = (SPListViewItem *)nitem->parent();
        index++;
      } 
    }

  
    
    // Now set the currentIndexItem so we can look up the correctl
    // color for the function later.
    if( nitem )
    {
      currentItem = (SPListViewItem *)nitem;

      index = 0;
      QListViewItemIterator it( splv );
      while( it.current() )
      {
        QListViewItem *item = *it;
        if( item->isSelected() )
        {
          currentItemIndex = index;
          break;
        }
        index++;
        it++;
      }

      matchSelectedItem( nitem, std::string(nitem->text(fieldCount-1).ascii()) );
    }
  }
}


static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanel::doOption(int id)
{
// printf("doOption() id=%d\n", id);

  if( splv->columnWidth(id) )
  {
    cwidth = splv->columnWidth(id);
    splv->hideColumn(id);
  } else
  {
    splv->setColumnWidth(id, cwidth);
  } 
}

bool
StatsPanel::matchSelectedItem(QListViewItem *item, std::string sf )
{
// printf("matchSelectedItem() entered. sf=%s\n", sf.c_str() );

// printf("A: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
// printf("A: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );

  QString lineNumberStr = "-1"; // MPI* and IO* only

  if( currentUserSelectedReportStr.contains("CallTrees") || currentUserSelectedReportStr.contains("TraceBacks") )
  {
    // text directly.
    SPListViewItem *selectedItem = (SPListViewItem *)splv->selectedItem();
    if( selectedItem )
    {
      QString ret_value = selectedItem->text(fieldCount-1).stripWhiteSpace();
      sf = ret_value.ascii();
// printf("         (%s)\n", sf.c_str() );
      int index = ret_value.find("@");
      if( index == 0 )
      {
        int sfi = 0;

        sfi = ret_value.find(" in ");
        if( sfi != -1 )
        {
// printf("sfi=(%d)\n", sfi);
          lineNumberStr = ret_value.mid(2, sfi-2);
        }
// printf("It think we have a line number (%s) just after the @ \n", lineNumberStr.ascii() );

      }
    }
  }

// First lets try to find the function/file pair.

  SourceObject *spo = NULL;
  QString ssf = QString(sf).stripWhiteSpace();

  QString selected_function_qstring = QString(sf).stripWhiteSpace();

  QString filename = QString::null;
  QString function_name = QString::null;


// printf("ssf=(%s)\n", ssf.ascii() );
  if( currentCollectorStr == "hwc" || currentCollectorStr == "hwctime" )
  {
    filename = getFilenameFromString( ssf ); 
    function_name = "";
    int index = ssf.find(",");
// printf("index=%d \n", index);
    if( index != -1 )
    {
      lineNumberStr = ssf.mid(index+1,ssf.length()-(index+2));
// printf("lineNumberStr=(%s)\n", lineNumberStr.ascii() );
      filename = filename.mid(0, index);
    } 
  } else
  {
    QString lns = QString::null;
    filename = getFilenameFromString( ssf ); 
    function_name = getFunctionNameFromString( ssf, lns );
    if( currentCollectorStr != "mpi" && currentCollectorStr != "mpit" &&
        currentCollectorStr != "io" && currentCollectorStr != "iot" )
    {
      lineNumberStr = lns;
    }
  }
  int index = filename.find("(");
  if( index != -1 )
  {
    lineNumberStr = filename.mid(index+1,filename.length()-(index+2));
    filename = filename.mid(0, index);
  } 

  if( currentUserSelectedReportStr == "Statements" )
  {
    function_name = "";
  }

// printf("AA: filename=(%s)\n", filename.ascii() );
// printf("AA: function_name=(%s) lineNumberStr=(%s)\n", function_name.ascii(), lineNumberStr.ascii() );

  // Explicitly make sure the highlightList is clear.
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();


  QApplication::setOverrideCursor(QCursor::WaitCursor);

  // Begin Find the file/function pair.
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo != NULL )
    {
      Experiment *fw_experiment = eo->FW();
      ThreadGroup tgrp = fw_experiment->getThreads();
      ThreadGroup::iterator ti = tgrp.begin();
      if( tgrp.size() == 0 )
      { // No threads to look up the data...
        return FALSE;
      }

      for( ; ti != tgrp.end(); ti++ )
      {
        Thread thread = *ti;
        //  Check to see if we're in the focused group!
        bool foundFLAG = FALSE;
        if( currentThreadGroupStrList.size() == 0 )
        {
          foundFLAG = TRUE;
        }
// printf("Is %d one of us\n", thread.getProcessId() );
        for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
        {
          QString ts = (QString)*it;
// printf("Is it ts=(%s)\n", ts.ascii() );
          if( QString("%1").arg(thread.getProcessId()) == ts )
          {
// printf("Got it!!!!\n");
            foundFLAG = TRUE;
            break;
          }
        }
        if( foundFLAG == FALSE )
        {
          continue;
        }
// printf("We've got a match, now can we look up the function (%s)?\n", function_name.ascii() );

        spo = lookUpFileHighlights(function_name, thread, ti, item, filename, lineNumberStr, highlightList);

      }
    }
    // If no spo, make one so the source panel is placed correctly.
    if( !spo )
    {
// printf("NO SOURCE PANEL OBJECT to update existing source. Create null one.\n");
      spo = new SourceObject(NULL, NULL, -1, expID, TRUE, NULL);
    }
    if( spo )
    {
      QString name = QString("Source Panel [%1]").arg(expID);
// printf("A: Find a SourcePanel named %s\n", name.ascii() );
      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel )
      {
// printf("no source view up, place the source panel.\n");
        char *panel_type = "Source Panel";
        PanelContainer *startPC = NULL;
        if( getPanelContainer()->parentPanelContainer != NULL )
        {
          startPC = getPanelContainer()->parentPanelContainer;
        } else
        {
          startPC = getPanelContainer();
        }
        PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);
        sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
      }
      if( sourcePanel )
      {
// printf("send the spo to the source panel.\n");
// spo->print();
       sourcePanel->listener((void *)spo);
      }
    }
    QApplication::restoreOverrideCursor( );
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
// End Find the file/function pair.

}


void
StatsPanel::updateStatsPanelData(QString command)
{
// printf("StatsPanel::updateStatsPanelData() entered.\n");


  levelsToOpen = getPreferenceLevelsToOpen().toInt();

  SPListViewItem *splvi;
  columnHeaderList.clear();

  // Percent value list (for the chart)
  cpvl.clear();
  // Text value list (for the chart)
  ctvl.clear();
  color_names = NULL;
  cf->init();
  total_percent = 0.0;
  numberItemsToDisplayInStats = 10;
  if( !getPreferenceTopNLineEdit().isEmpty() )
  {
    bool ok;
    numberItemsToDisplayInStats = getPreferenceTopNLineEdit().toInt(&ok);
  }
  numberItemsToDisplayInChart = 5;
  if( !getPreferenceTopNChartLineEdit().isEmpty() )
  {
    bool ok;
    numberItemsToDisplayInChart = getPreferenceTopNChartLineEdit().toInt(&ok);
  }
// printf("numberItemsToDisplayInChart = %d\n", numberItemsToDisplayInChart );

  textENUM = getPreferenceShowTextInChart();
// printf("textENUM=%d\n", textENUM );

  lastlvi = NULL;
  gotHeader = FALSE;
  gotColumns = FALSE;
//  fieldCount = 0;
  percentIndex = -1;


  if( command.isEmpty() )
  {
    command = generateCommand();
  } else
  {
    about = "Compare/Customize report for:\n  ";
  }

  if( command.isEmpty() )
  {
    return;
  }

// printf("  lastCommand = %s  command = %s\n", lastCommand.ascii(), command.ascii() );
  if( staticDataFLAG == TRUE && command == lastCommand )
  {  // Then we really don't need to update.
// printf("We really have static data and its the same command... Don't update.\n");
    return;
  }

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  splv->clear();
  for(int i=splv->columns();i>=0;i--)
  {
    splv->removeColumn(i-1);
  }

  splv->setSorting ( -1 );

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  Redirect_Window_Output( cli->wid, spoclass, spoclass );
// printf("command: (%s)\n", command.ascii() );
  about += "Command issued: " + command + "\n";
  lastCommand = command;
  InputLineObject *clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());

  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
//    return;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  while( !clip->Semantics_Complete() )
  {
// printf("ping!\n");
    status = cli->checkStatus(clip, command);
    if( !status || status == ILO_ERROR )
    { // An error occurred.... A message should have been posted.. return;
      QApplication::restoreOverrideCursor();
      if( clip )
      {
        clip->Set_Results_Used();
      }
      splv->addColumn( "No data available:" );
      resetRedirect();
      return;
    }

    qApp->processEvents(1000);

    if( !cli->shouldWeContinue() )
    {
// printf("RETURN FALSE!   COMMAND FAILED!\n");
      QApplication::restoreOverrideCursor();
      if( clip )
      {
        clip->Set_Results_Used();
      }
      splv->addColumn( "Command failed to complete." );
      resetRedirect();
      return;
    }

    sleep(1);
  }
// printf("done pinging...\n");

  //Test putting the output to statspanel stream.
  Default_TLI_Line_Output(clip);

  // make sure you redirect the output back to the cli...\n");
  // We have a strange (temporary problem) because we're currently using
  // Redirect_Window_Output( cli->wid, spoclass, spoclass ); to push the 
  // cli output to a parser that then generates the 
  // This is (obviously) paired with the CmdPanel.   When you pull this,
  // pull that one too.
  Panel *cmdPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), "&Command Panel");
  if( cmdPanel )
  {
    MessageObject *msg = new MessageObject("Redirect_Window_Output()");
    cmdPanel->listener((void *)msg);
    delete msg;
  } else
  {
    fprintf(stderr, "Unable to redirect output to the cmdpanel.\n");
  }



// Put out the chart if there is one...
   color_names = hotToCold_color_names;
   if( descending_sort != true )
   {
    color_names = coldToHot_color_names;
   }
   if( textENUM == TEXT_NONE )
   {
// printf("textENUM=%d (TEXT_NONE)\n", textENUM );
     ctvl.clear();
   }

// printf("Put out the chart!!!!\n");
// printf("numberItemsToDisplayInStats=(%d) cpvl.count()=(%d)\n", numberItemsToDisplayInStats, cpvl.count() );

  // Do we need an other?
// printf("total_percent=%f splv->childCount()=%d cpvl.count()=%d numberItemsToDisplayInStats=%d\n", total_percent, splv->childCount(), cpvl.count(), numberItemsToDisplayInStats );

// printf("A: cpvl.count()=%d numberItemsToDisplayInChart = %d\n", cpvl.count(), numberItemsToDisplayInChart );
  if( ( total_percent > 0.0 &&
      cpvl.count() < numberItemsToDisplayInStats) ||
      ( total_percent > 0.0 && 
        cpvl.count() < numberItemsToDisplayInStats &&
        numberItemsToDisplayInChart < numberItemsToDisplayInStats) )
  {
    if( total_percent < 100.00 )
    {
// printf("add other of %f\n", 100.00-total_percent );
      cpvl.push_back( (int)(100.00-total_percent) );
// printf("total_percent: textENUM=%d\n", textENUM );
      if( textENUM != TEXT_NONE )
      {
        ctvl.push_back( "other" );
      }
    }
  }

  // Or were there no percents in the initial query.
// printf("total_percent = %f\n", total_percent );
  if( total_percent == 0.0 )
  {
    cpvl.clear();
    ctvl.clear();
    cpvl.push_back(100);
    ctvl.push_back("N/A");
  }
// printf("cpvl.count()=%d ctvl.count()=%d\n", cpvl.count(), ctvl.count() );

  cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);
  cf->setHeader( (QString)*columnHeaderList.begin() );

  QApplication::restoreOverrideCursor();

  if( clip )
  {
    clip->Set_Results_Used();
  }


}


void
StatsPanel::threadSelected(int val)
{ 
// printf("threadSelected(%d)\n", val);


  currentThreadStr = threadMenu->text(val).ascii();

  currentThreadsStr = QString::null;

// printf("threadMenu: selected text=(%s)\n", threadMenu->text(val).ascii() );


  bool FOUND_FLAG = FALSE;
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;

    if( ts == currentThreadStr )
    {   // Then it's already in the list... now remove it.
// printf("add the selected thread (%s).\n", ts.ascii() );
      currentThreadGroupStrList.remove(ts);
      FOUND_FLAG = TRUE;
      break;
    }
  }

  // We didn't find it to remove it, so this is a different thread... add it.
  if( FOUND_FLAG == FALSE )
  {
// printf("We must need to add it (%s) then!\n", currentThreadStr.ascii() );
    currentThreadGroupStrList.push_back(currentThreadStr);
  }

// printf("Here's the string list...\n");
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;
// printf("A: ts=(%s)\n", ts.ascii() );
  
    if( ts.isEmpty() )
    {
      continue;
    }

    if( currentThreadsStr.isEmpty() )
    {
      currentThreadsStr = "-p "+ts;
    } else
    {
      currentThreadsStr += ","+ts;
    }
  }

// printf("currentThreadsStr = %s call update.\n", currentThreadsStr.ascii() );


  updateStatsPanelData();


  // Now, try to focus the source panel on the first entry...
  QListViewItemIterator it( splv );
  if( it.current() )
  {
    int i = 0;
    QListViewItem *item = *it;
    StatsPanel::itemSelected(item);
  }
}

void
StatsPanel::collectorMetricSelected(int val)
{ 
// printf("collectorMetricSelected val=%d\n", val);
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
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
// printf("BB1: s=(%s) currentCollectorStr=(%s) currentMetricStr=(%s)\n", s.ascii(), currentCollectorStr.ascii(), currentMetricStr.ascii() );
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

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
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
// printf("MPIReportSelected val=%d\n", val);
// printf("MPIReportSelected: mpi_menu=(%s)\n", mpi_menu->text(val).ascii() );
//  printf("MPIReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

  QString s = QString::null;
  s = mpi_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("B: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("D: NOW FIND ::\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
// printf("MPI1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
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

// printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
  }
  updateStatsPanelData();
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
// printf("IOReportSelected val=%d\n", val);
// printf("IOReportSelected: io_menu=(%s)\n", io_menu->text(val).ascii() );
// printf("IOReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

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
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
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

// printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
  }
}

void
StatsPanel::collectorHWCReportSelected(int val)
{ 
  currentCollectorStr = "hwc";
  HWCReportSelected(val);
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
// printf("HWCReportSelected: collectorMetricSelected val=%d\n", val);
// printf("HWCReportSelected: hwc_menu=(%s)\n", hwc_menu->text(val).ascii() );
// printf("HWCReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

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
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
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


// printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
  }
}


void
StatsPanel::HWCTimeReportSelected(int val)
{ 
// printf("HWCTimeReportSelected: collectorMetricSelected val=%d\n", val);
// printf("HWCTimeReportSelected: hwctime_menu=(%s)\n", hwctime_menu->text(val).ascii() );
// printf("HWCTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

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
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
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


// printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
  }
}


void
StatsPanel::collectorUserTimeReportSelected(int val)
{ 
// printf("collectorUserTimeReportSelected: val=%d\n", val);
// printf("collectorUserTimeReportSelected: usertime_menu=(%s)\n", usertime_menu->text(val).ascii() );
// printf("collectorUserTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "usertime";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = usertime_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("UserTimeReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
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
    }

// printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
  }
  updateStatsPanelData();


}


void
StatsPanel::collectorPCSampReportSelected(int val)
{ 
// printf("collectorPCSampReportSelected: val=%d\n", val);
// printf("collectorPCSampReportSelected: pcsamp_menu=(%s)\n", pcsamp_menu->text(val).ascii() );
// printf("collectorPCSampReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

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

// printf("PCSampReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentMetricStr = s.mid(index+2);
      currentUserSelectedReportStr = currentMetricStr;
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
    }

// printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );

// printf("Collector changed call updateStatsPanelData() \n");
  }
  updateStatsPanelData();

}


void
StatsPanel::collectorGenericReportSelected(int val)
{ 
// printf("collectorGenericReportSelected: val=%d\n", val);
// printf("collectorGenericReportSelected: generic_menu=(%s)\n", generic_menu->text(val).ascii() );
// printf("collectorGenericReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );

  currentUserSelectedReportStr = QString::null;

  currentMetricStr = QString::null;
  currentCollectorStr = QString::null;
  selectedFunctionStr = QString::null;
// printf("C: NULLING OUT selectedFunctionStr\n");

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
}


void
StatsPanel::modifierSelected(int val)
{ 
// printf("modifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", modifierMenu->text(val).ascii() );

  if( modifierMenu->text(val).isEmpty() )
  {
    return;
  }


  std::string s = modifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

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
}


void
StatsPanel::mpiModifierSelected(int val)
{ 
// printf("mpiModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", mpiModifierMenu->text(val).ascii() );
// printf("modifierSelected: (%d)\n", mpiModifierMenu->text(val).toInt() );


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
}


void
StatsPanel::mpitModifierSelected(int val)
{ 
// printf("mpitModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", mpitModifierMenu->text(val).ascii() );


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
// printf("ioModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", ioModifierMenu->text(val).ascii() );

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

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_io_modifiers.push_back(s);
    }
    ioModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::iotModifierSelected(int val)
{ 
// printf("iotModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", iotModifierMenu->text(val).ascii() );


  if( iotModifierMenu->text(val).isEmpty() )
  {
    iotModifierMenu->insertSeparator();
    if( io_menu )
    {
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
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_iot_modifiers.begin();
       it != current_list_of_iot_modifiers.end();  )
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
      current_list_of_iot_modifiers.remove(modifier);
      iotModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_iot_modifiers.push_back(s);
    }
    iotModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::hwcModifierSelected(int val)
{ 
// printf("hwcModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", hwcModifierMenu->text(val).ascii() );

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
StatsPanel::hwctimeModifierSelected(int val)
{ 
// printf("hwctimeModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", hwctimeModifierMenu->text(val).ascii() );

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
// printf("usertimeModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", usertimeModifierMenu->text(val).ascii() );

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
StatsPanel::pcsampModifierSelected(int val)
{ 
// printf("pcsampModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", pcsampModifierMenu->text(val).ascii() );

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
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_pcsamp_modifiers.begin();
       it != current_list_of_pcsamp_modifiers.end();  )
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
      current_list_of_pcsamp_modifiers.remove(modifier);
      pcsampModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_pcsamp_modifiers.push_back(s);
    }
    pcsampModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::genericModifierSelected(int val)
{ 
// printf("genericModifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", genericModifierMenu->text(val).ascii() );

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
// printf("StatsPanel::raisePreferencePanel() \n");
  getPanelContainer()->getMainWindow()->filePreferences( statsPanelStackPage, QString(pluginInfo->panel_type) );
}

int
StatsPanel::getLineColor(double value)
{
// printf("getLineColor(%f) descending_sort= %d TotalTime=%f\n", value, descending_sort, TotalTime);
  if( (int) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }
  return(10);
}

int
StatsPanel::getLineColor(unsigned int value)
{
// printf("getLineColor(%u)\n", value);


  if( (int) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }
  return(10);

}


int
StatsPanel::getLineColor(uint64_t value)
{
// printf("getLineColor(%lld)\n", value);

  if( (uint64_t) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }

  return(10);
}


void
StatsPanel::updateCollectorList()
{
  // Now get the collectors... and their metrics...
  QString command = QString("list -v expTypes -x %1").arg(expID);
// printf("attempt to run (%s)\n", command.ascii() );
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
list_of_collectors.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_collectors, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
// printf("ran %s\n", command.ascii() );
}


void
StatsPanel::updateCollectorMetricList()
{
  // Now get the collectors... and their metrics...
//  command = QString("listTypes -x %1").arg(expID);
//  QString command = QString("listMetrics -x %1").arg(expID);
  QString command = QString("list -v metrics -x %1").arg(expID);
// printf("attempt to run (%s)\n", command.ascii() );
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_collectors_metrics.clear();
list_of_generic_modifiers.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_collectors_metrics, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
// printf("ran %s\n", command.ascii() );

  if( list_of_collectors_metrics.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors_metrics.begin();
         it != list_of_collectors_metrics.end(); it++ )
    {
      std::string collector_name = (std::string)*it;
// printf("collector_name/metric name=(%s)\n", collector_name.c_str() );
      if( currentCollectorStr.isEmpty() )
      {
        QString s = QString(collector_name.c_str());
        int index = s.find("::");
        currentCollectorStr = s.mid(0, index );
// printf("Default the current collector to (%s)\n", collector_name.c_str());
      }
list_of_generic_modifiers.push_back(collector_name);
    }
  }
}


void
StatsPanel::updateThreadsList()
{
// Now get the threads.
//  QString command = QString("listPids -x %1").arg(expID);
  QString command = QString("list -v pids -x %1").arg(expID);
// printf("attempt to run (%s)\n", command.ascii() );
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_pids.clear();
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
         &list_of_pids, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
// printf("ran %s\n", command.ascii() );

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
// printf("Is says you have %d collectors.\n", cgrp.size() );
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
// printf("setCurrentThread() entered\n");
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
        if( currentThreadStr.isEmpty() )
        {
// printf("Seting a currentThreadStr!\n");
          currentThreadStr = pidstr;
          // set a default thread as well...
          t1 = *ti;
          if( currentThread )
          {
            delete currentThread;
          }
          currentThread = new Thread(*ti);
// printf("A: Set a currentThread\n");
        }
        if( pidstr == currentThreadStr )
        {
// printf("Using %s\n", currentThreadStr.ascii() );
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
// printf("A: currentCollectorStr=(%s) currentMetricStr=(%s) currentThreadStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii(), currentThreadStr.ascii() );
      }
  }
// printf("metric=currentMetricStr=(%s)\n", currentMetricStr.ascii() );
}

void
StatsPanel::outputCLIData(QString *incoming_data)
{
// printf("StatsPanel::outputCLIData\n");
// printf("%s", incoming_data->ascii() );

  SPListViewItem *highlight_item = NULL;
  bool highlight_line = FALSE;
  QColor highlight_color = QColor("blue");

  QString strippedString1 = QString::null; // MPI only.

  // Skip any blank lines.
  if( *incoming_data == QString("\n") )
  {
    return;
  }

  QString data = QString("  ")+(*incoming_data);
// printf("%s", data.ascii() );

  int start_index = 0;
  QString stripped_data = data.stripWhiteSpace();
  QRegExp start_rxp = QRegExp( "  [A-Z,a-z,\\-,0-9,%]");
  QRegExp end_rxp = QRegExp( "[A-Z,a-z,\\-,0-9,%]  ");

  start_index = data.find( start_rxp, start_index );
  start_index += 2;
  if( gotHeader == FALSE )
  {
    fieldCount = data.contains( start_rxp );
// printf("fieldCount... hot off the wire = (%d) start_index=(%d)\n", fieldCount, start_index );

    int end_index = 99999;
    for(int i=0;i<fieldCount;i++)
    {
      QString headerStr = QString::null;
      end_index = data.find(end_rxp, start_index);
      end_index++;  // Need to include the last letter...
      if( i == 0 )  // For this first field we always start from zero.
      {
        start_index = 0;  
      }
      columnValueClass[i].start_index = start_index;
      columnValueClass[i].end_index = end_index;

      if( end_index == -1 )
      {
        columnValueClass[i].end_index = 99999;
//        headerStr = data.mid(start_index, end_index-start_index).stripWhiteSpace();
        headerStr = data.mid(start_index).stripWhiteSpace();
// printf("A: headerStr=(%s)\n", headerStr.ascii() );
        columnHeaderList.push_back(headerStr);
        splv->addColumn( data.mid(start_index).stripWhiteSpace() );
        break;
      } else
      {
        int header_end_index = data.find( start_rxp, start_index );
        if( header_end_index == -1 )
        {
          header_end_index = 99999;
        }
//        headerStr = data.mid(start_index, header_end_index-start_index).stripWhiteSpace();
        headerStr = data.mid(start_index, end_index-start_index).stripWhiteSpace();
// printf("B: headerStr=(%s)\n", headerStr.ascii() );
    
      }
//      columnHeaderList.push_back(data.mid(start_index, end_index-start_index).stripWhiteSpace());
      columnHeaderList.push_back(headerStr);
      splv->addColumn( data.mid(start_index, end_index-start_index).stripWhiteSpace() );
      // Find the percent column
// printf("find percent: headerStr=(%s) (%s)\n", headerStr.ascii(), data.mid(start_index, end_index-start_index).stripWhiteSpace().ascii() );
//      if( headerStr.find("%") != -1 )
      if( headerStr.find("%") != -1 )
      {
        if( percentIndex == -1 )
        {
          percentIndex = i;
// printf("Found the percentIndex at %d\n", percentIndex);
        }
      }
  
      start_index = end_index+2;
    }
    columnValueClass[fieldCount-1].end_index = 99999;

#if 0
// Begin debug.
printf("fieldCount=%d\n", fieldCount);
for(int i=0;i<fieldCount;i++)
{
  printf("columnValueClass[%d].start_index=%d end=%d\n", i, columnValueClass[i].start_index, columnValueClass[i].end_index );
}
// End debug.
#endif // 0
  
    gotHeader = TRUE;
    return;
  }

  QString *strings = new QString[fieldCount];
 
  int percent = 0;
  for( int i = 0; i<fieldCount; i++)
  {
    int si = columnValueClass[i].start_index;
    int l = columnValueClass[i].end_index-columnValueClass[i].start_index;
    QString value = data.mid(si,l).stripWhiteSpace();
// printf("si=%d ei=%d (%s)\n", columnValueClass[i].start_index, columnValueClass[i].end_index, value.ascii() );
    if( i == 0 ) // Grab the (some) default metric FIX
    {
      float f = value.toFloat();
      TotalTime += f;
    }
    if( percentIndex == i )
    {
      if( !value.isEmpty() )
      {
        float f = value.toFloat();
        percent = (int)f;
// printf("percent=(%d)\n", percent);
        total_percent += f;
      }
    }
    strings[i] = value;
// printf("        strings[%d]=(%s)\n", i, strings[i].ascii() );
  }
// printf("A: total_percent=%f\n", total_percent );


  if( fieldCount == 0 )
  {
    QMessageBox::information( (QWidget *)this, tr("Info:"), tr("There are no data samples yet available.   This could be a timing issue.\nTry an update of the StatsPanel."), QMessageBox::Ok );
  
    return;
  }

  SPListViewItem *splvi;
// printf("More Function MPItraceFLAG=(%d)\n", MPItraceFLAG);
  if( (( currentCollectorStr == "mpi" || currentCollectorStr == "mpit" || currentCollectorStr == "io" || currentCollectorStr == "iot" ) && (MPItraceFLAG == FALSE && !currentUserSelectedReportStr.startsWith("Functions")) &&  ( currentUserSelectedReportStr.startsWith("CallTrees") || currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || currentUserSelectedReportStr.startsWith("Functions") || currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || currentUserSelectedReportStr.startsWith("Butterfly") ) ) ||
      (currentCollectorStr == "usertime" && (currentUserSelectedReportStr == "Butterfly" || currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || currentUserSelectedReportStr.startsWith("CallTrees") || currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
      (currentCollectorStr.startsWith("hwc") && (currentUserSelectedReportStr == "Butterfly" || currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || currentUserSelectedReportStr.startsWith("CallTrees") || currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) ) )
  {
    QString indentChar = ">";

    if( currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") )
    {
      indentChar = "<";
    } 
    bool indented = strings[fieldCount-1].startsWith(indentChar);
    int indent_level = 0;

// Pretty the output up a bit.
    if( currentUserSelectedReportStr.startsWith("Butterfly") )
    {
      if( indented )
      {
        // Right side
        strings[fieldCount-1].insert(1,"    ");
// printf("RS: Field (%s)\n", strings[fieldCount-1].ascii() );
      } else if( strings[fieldCount-1].startsWith("<") )
      {
        // Left side
        strings[fieldCount-1].remove("<");
// printf("LS: Field (%s)\n", strings[fieldCount-1].ascii() );
      } else
      {
// printf("Color this one: Field (%s)\n", strings[fieldCount-1].ascii() );
        // Focused Function
        strings[fieldCount-1].insert(0,"  ");
        highlight_line = TRUE;
      }
// printf("here (%s)\n", strings[fieldCount-1].ascii() );
    }


// printf("indented = (%d)\n", indented );
// printf("%d (%s) (%s)\n", indented, strings[0].ascii(), strings[fieldCount-1].ascii() );
  
    if( !indented )
    {
// printf("indent_level=zero lastIndentLevel=%d\n", indent_level, lastIndentLevel );
      // If it's not indented, make sure if is put after the last
      // root node.
      if( lastlvi )
      {
        SPListViewItem *topParent = (SPListViewItem *)lastlvi->parent();
        while( topParent )
        {
          lastlvi = topParent;
          topParent = (SPListViewItem *)topParent->parent();
        }
// printf("Put after (%s) \n", lastlvi->text(fieldCount-1).ascii() );
      }
      lastlvi = splvi =  MYListViewItem( this, splv, lastlvi, strings);
if( highlight_line ) highlight_item = splvi;
      lastIndentLevel = 0;
    } else
    {
      if( indented && lastlvi != NULL )
      {
        QRegExp rxp = QRegExp( "[_,' ',@,A-Z,a-z,0-9,%]");
        indent_level = strings[fieldCount-1].find(rxp);
        strippedString1 = strings[fieldCount-1].mid(indent_level,9999);
        strings[fieldCount-1] = strippedString1;

        // Pretty up the format a bit.
        if( currentUserSelectedReportStr.startsWith("Butterfly") )
        {
          strings[fieldCount-1].replace(0,0,QString("  ")); // This is the bad boy
        }

        if( indent_level == -1 )
        {
          fprintf(stderr, "Error in determining depth for (%s).\n", strippedString1.ascii() );

        }
// printf("indent_level = %d lastIndentLevel = %d\n", indent_level, lastIndentLevel);
        if( indent_level > lastIndentLevel )
        {
// printf("A: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), lastlvi->text(fieldCount-1).ascii() );
          lastlvi = splvi =  MYListViewItem( this, lastlvi, lastlvi, strings);
if( highlight_line ) highlight_item = splvi;
        } else
        {
// printf("Go figure out the right leaf to put this in...(%s) \n", strings[1].ascii() );

          SPListViewItem *mynextlvi = lastlvi;
          SPListViewItem *after = NULL;
          while( mynextlvi->parent() )
          {
             mynextlvi = (SPListViewItem *)mynextlvi->parent();
          }
          for(int i=0;i<indent_level-1;i++)
          {
            SPListViewItem *lastChild = (SPListViewItem *)mynextlvi->firstChild();
            while( lastChild->nextSibling() )
            {
              lastChild = (SPListViewItem *)lastChild->nextSibling();
            }
            mynextlvi = lastChild;
          }
  
          lastlvi = mynextlvi;
  
          // go to this head, count down the children of the indent level... Then add
          // this item.
          // after = (SPListViewItem *)lastlvi->parent()->firstChild();
          after = (SPListViewItem *)lastlvi->firstChild();
          while( after->nextSibling() )
          {
            after = (SPListViewItem *)after->nextSibling();
          }
// printf("C: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), after->text(fieldCount-1).ascii() );
          lastlvi = splvi = MYListViewItem( this, lastlvi, after, strings );
if( highlight_line ) highlight_item = splvi;
        }
      } else
      {
        fprintf(stderr, "Error in chaining child (%s) to tree.\n", strippedString1.ascii() );
      }
    }

    // Now try to open all the items.\n");
// printf("  indent_level=%d\n", indent_level );
    if( indent_level < levelsToOpen || levelsToOpen == -1 )
    {
      if( lastlvi )
      {
        lastlvi->setOpen(TRUE);
      }
    }
// printf("open lastlvi=(%s)\n", lastlvi->text(fieldCount-1).ascii() );

    lastIndentLevel = indent_level;
  } else
  {
    if( fieldCount == 2 )
    {
      lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1] );
    } else if( fieldCount == 3 )
    { // i.e. like pcsamp
      lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1], strings[2] );
    } else
    { // i.e. like usertime
//      lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1], strings[2], strings[3] );
      lastlvi = splvi =  MYListViewItem( this, splv, lastlvi, strings);
if( highlight_line ) highlight_item = splvi;
    }
  }


  if( total_percent > 0.0 && cpvl.count() < numberItemsToDisplayInStats  &&
      ctvl.count() < numberItemsToDisplayInChart )
  {
// printf("put out data for the chart. %d %s\n", percent, strings[percentIndex].stripWhiteSpace().ascii() );
    cpvl.push_back( percent );
  } 
  if( total_percent > 0.0 && cpvl.count() <= numberItemsToDisplayInChart &&
      ctvl.count() < numberItemsToDisplayInChart )
  {
// printf("Push back another one!(%s)\n", strings[percentIndex].stripWhiteSpace().ascii());
    if( textENUM == TEXT_BYVALUE )
    { 
// printf("TEXT_BYVALUE: textENUM=%d (%s)\n", textENUM, strings[0].stripWhiteSpace().ascii()  );
      ctvl.push_back( strings[0].stripWhiteSpace() );
    } else if( textENUM == TEXT_BYPERCENT )
    {
// printf("A: TEXT_BYPERCENT: textENUM=%d\n", textENUM );
      ctvl.push_back( strings[percentIndex].stripWhiteSpace() );
    }
  }

  if( highlight_line )
  {
    //    highlight_item->setSelected(TRUE);
    for( int i=0;i<fieldCount;i++)
    {
      highlight_item->setBackground( i, QColor("red") );
    }
  }
}

void
StatsPanel::outputAboutData(QString *incoming_data)
{
  aboutOutputString += *incoming_data;
}


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, QListView *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
// printf("Put out SPListViewItem with 1 item (%s)\n", strings[0].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0] );
      break;
    case 2:
// printf("Put out SPListViewItem with 2 item (%s) (%s)\n", strings[0].ascii(), strings[1].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
// printf("Put out SPListViewItem with 3 item (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
// printf("Put out SPListViewItem with 4 item (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
// printf("Put out SPListViewItem with 5 item (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
// printf("Put out SPListViewItem with 6 item (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
// printf("Put out SPListViewItem with 7 item (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
// printf("Put out SPListViewItem with 8 item, (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii(), strings[7].ascii() );
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
// printf("Warning: over 9 columns... Notify developer...\n");
      for( int i=8; i<fieldCount; i++ )
      {
        item->setText(i, strings[i]);
      }
      break;
  }

  return item;
} 


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, SPListViewItem *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0] );
      break;
    case 2:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
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
// printf("findSelectedFunction() entered\n");
  QString functionStr = QString::null;
  QListViewItem *selected_function_item = NULL;
  QListViewItemIterator it( splv, QListViewItemIterator::Selected );
  while( it.current() )
  {
    int i = 0;
    selected_function_item = it.current();
    break;  // only select one for now...
    ++it;
  }
  if( selected_function_item == NULL || selected_function_item->text(fieldCount-1).isEmpty() )
  {
// printf("Whoa!  No function selected.\n");
    about = lastAbout;
    return QString::null;
  }
// printf("selected_function_item->text(%d)=(%s)\n", fieldCount-1, selected_function_item->text(fieldCount-1).ascii() );
  if( selected_function_item && !selected_function_item->text(fieldCount-1).isEmpty() )
  {
    QString tstr = selected_function_item->text(fieldCount-1);
    int eof = tstr.find('(');
    functionStr = tstr.mid(0,eof);
  }


  // Now clean up any known noise on the line.
  if( functionStr.stripWhiteSpace().startsWith("@ ") )
  {
    int i = functionStr.find(" in ");
    if( i != -1 )
    {
      QString fstr = functionStr.mid(i+4,9999);
      functionStr = fstr;
    }
  }


// Before we return... make sure this is really a function and not a filename.
QFileInfo fi(functionStr);
if( fi.exists() )
{
  functionStr = QString::null;
}
// Well that doesn't always work... If the source moved... Then we 
// need to apply an additional heuristic... FIX (i.e. Make this better!)
if( functionStr.contains(".") || functionStr.contains("/") )
{
  functionStr = QString::null;
}


// printf("Return functionStr=(%s)\n", functionStr.stripWhiteSpace().ascii() );

  return( functionStr.stripWhiteSpace() );
}

void
StatsPanel::resetRedirect()
{
// Just make sure any pending output goes "somewhere".
  Panel *cmdPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), "&Command Panel");
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

// printf("Get filename from (%s)\n", selected_qstring.ascii() );
  QString filename = QString::null;

  int sfi = 0;

  sfi = selected_qstring.find(" in ");
// printf("sfi=%d (Was there an \" in \"\n", sfi );

  if( sfi == -1 )
  {
    sfi = selected_qstring.find(" ");
  } else
  {
    sfi = selected_qstring.find(": ");
    sfi += 1;
  }
  sfi++;

  int efi = selected_qstring.find(",");

// printf("sfi=(%d) efi=(%d) (Was there a \",\" in (%s)\n", sfi, efi, selected_qstring.ascii()  );

  filename = selected_qstring.mid(sfi, efi-sfi );

// printf("   returning filename=(%s)\n", filename.ascii() );

  return(filename);
}

QString
StatsPanel::getFunctionNameFromString( QString selected_qstring, QString &lineNumberStr )
{
// printf("Get funcString from %s\n", selected_qstring.ascii() );
  QString funcString = QString::null;
  QString workString = selected_qstring;

  int sfi = 0;

  sfi = selected_qstring.find(" in ");
// printf("sfi=%d (Was there an \" in \"\n", sfi );
  if( sfi != -1 )
  {
    workString = selected_qstring.mid(sfi+4);
  } else
  {
    workString = selected_qstring;
  }

// printf("Start you function lookup from (%s)\n", workString.ascii() );

  funcString = workString.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

// printf("funcString=(%s)\n", funcString.ascii() );

  int efi = workString.find("(");
  QString function_name = workString.mid(0,efi);

// printf("function_name=(%s)\n", function_name.ascii() );

  if( ( currentCollectorStr == "mpi" || currentCollectorStr == "mpit" || currentCollectorStr == "io" || currentCollectorStr == "iot" ) && ( collectorStrFromMenu.startsWith("CallTrees") || collectorStrFromMenu.startsWith("Functions") || collectorStrFromMenu.startsWith("TraceBacks") || collectorStrFromMenu.startsWith("TraceBacks,FullStack") ) )
  {
    int bof = -1;
    int eof = workString.find('(');
// printf("eof=%d\n", eof);
    if( eof == -1 )
    {
// printf("main:  you should never be here..\n");
      function_name = "main";
    } else
    {

      QString tempString = workString.mid(0,eof);
// printf("tempString=%s\n", tempString.ascii() );

      QRegExp rxp = QRegExp( "[ >]");
      bof = tempString.findRev(rxp, eof);
// printf("bof=%d\n", bof);
      if( bof == -1 )
      {
        bof = 0;
      } else
      {
        bof++;
      }
    }
    function_name = workString.mid(bof,eof-bof);

    int boln = workString.find('@');
    boln++;
    int eoln = workString.find(" in ");
    lineNumberStr = workString.mid(boln,eoln-boln).stripWhiteSpace();
// printf("lineNumberStr=(%s)\n", lineNumberStr.ascii() );


// printf("mpi: function_name=(%s)\n", function_name.ascii() );
  }

// printf("returning function_name=(%s) lineNumberStr=(%s)\n", function_name.ascii(), lineNumberStr.ascii() );

  return(function_name);
}

QString
StatsPanel::generateCommand()
{
  QString traceAddition = QString::null;
// printf("GenerateCommand(%s) MPItraceFLAG = (%d) currentUserSelectedReportStr=(%s) IOtraceFLAG == %d\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii(), MPItraceFLAG, IOtraceFLAG );

  if( currentCollectorStr == "io" || currentCollectorStr == "iot" )
  {
    if( IOtraceFLAG == TRUE )
    {
      traceAddition = " -v trace";
    }
  } else if( currentCollectorStr == "mpi" || currentCollectorStr == "mpit" )
  {
    if( MPItraceFLAG == TRUE )
    {
      traceAddition = " -v trace";
    }
  }
// printf("traceAddition=(%s)\n", traceAddition.ascii() );

  QString modifierStr = QString::null;

  updateCollectorList();

  updateCollectorMetricList();

  updateThreadsList();

  lastAbout = about;

  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", expID);

  QString command = QString("expView -x %1").arg(expID);
  about = QString("Experiment: %1\n").arg(expID);

  if( currentCollectorStr.isEmpty() )
  {
    command += QString(" %1%2").arg("stats").arg(numberItemsToDisplayInStats);
    about += QString("Requested data for all collectors for top %1 items\n").arg(numberItemsToDisplayInStats);
  } else
  {
    command += QString(" %1%2").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
    about += QString("Requested data for collector %1 for top %2 items\n").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);

  }
  if( !currentUserSelectedReportStr.isEmpty() && !currentCollectorStr.isEmpty() )
  {
    if( currentCollectorStr != currentUserSelectedReportStr )
    {  // If these 2 are equal, we want the default display... not a 
       // specific metric.
       command += QString(" -m %1").arg(currentUserSelectedReportStr);
       about += QString("for metrics %1\n").arg(currentUserSelectedReportStr);
    }
  }
  if( !currentThreadsStr.isEmpty() )
  {
     command += QString(" %1").arg(currentThreadsStr);
     about += QString("for threads %1\n").arg(currentThreadsStr);
  }


// printf("so far: command=(%s) currentCollectorStr=(%s) currentUserSelectedReportStr(%s) currentMetricStr=(%s)\n", command.ascii(), currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii(), currentMetricStr.ascii() );


  if( currentCollectorStr == "pcsamp" && (currentUserSelectedReportStr
== "Functions") || (currentUserSelectedReportStr == "LinkedObjects") || (currentUserSelectedReportStr == "Statements") )
  {
    if( currentUserSelectedReportStr.isEmpty() )
    { 
      currentUserSelectedReportStr = "Functions";
    }
    command = QString("expView -x %1 %4%2 -v %5").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
// printf("start of pcsamp generated command (%s)\n", command.ascii() );
  } else if( currentCollectorStr == "usertime" &&
            (currentUserSelectedReportStr == "Butterfly") ||
            (currentUserSelectedReportStr == "Functions") ||
            (currentUserSelectedReportStr == "LinkedObjects") ||
            (currentUserSelectedReportStr == "Statements") ||
            (currentUserSelectedReportStr == "CallTrees") ||
            (currentUserSelectedReportStr == "CallTrees,FullStack") ||
            (currentUserSelectedReportStr == "TraceBacks") ||
            (currentUserSelectedReportStr == "TraceBacks,FullStack") )
  {
    if( currentUserSelectedReportStr.isEmpty() )
    { 
      currentUserSelectedReportStr = "Functions";
    }
    if( currentUserSelectedReportStr == "Butterfly" )
    {
      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() )
      {
        bool ok = FALSE;
// printf("A: NO FUNCTION SELECTED Prompt for one!\n");
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", QString("Which function?:"), QLineEdit::Normal, QString::null, &ok, this);
      }
      if( selectedFunctionStr.isEmpty() )
      {
        return( QString::null );
      }
      command = QString("expView -x %1 %4%2 -v Butterfly -f \"%3\"").arg(expID).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
     } else
     {
        command = QString("expView -x %1 %4%2 -v %5").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
     }
// printf("USERTIME! command=(%s)\n", command.ascii() );
  } else if( ( ( currentCollectorStr == "hwc" || currentCollectorStr == "hwctime" || currentCollectorStr == "mpi" || currentCollectorStr == "mpit" || currentCollectorStr == "io" || currentCollectorStr == "iot" ) && ( currentUserSelectedReportStr.startsWith("CallTrees") || currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || currentUserSelectedReportStr.startsWith("Functions") || currentUserSelectedReportStr.startsWith("mpi") || currentUserSelectedReportStr.startsWith("io") || currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || currentUserSelectedReportStr.startsWith("Butterfly") ) ))
  { 
// printf("It thinks we're mpi | io!\n");
    if( currentUserSelectedReportStr.isEmpty() || currentUserSelectedReportStr == "CallTrees" )
    {
      command = QString("expView -x %1 %3%2 -v CallTrees").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
    } else if ( currentUserSelectedReportStr == "CallTrees by Selected Function" )
    {
//      if( selectedFunctionStr.isEmpty() )
      {
        selectedFunctionStr = findSelectedFunction();
        bool ok = FALSE;
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", QString("Which function?:"), QLineEdit::Normal, QString::null, &ok, this);
      }
      if( selectedFunctionStr.isEmpty() )
      {
        return( QString::null );
      }
      command = QString("expView -x %1 %4%2 -v CallTrees -f %3").arg(expID).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
    } else if ( currentUserSelectedReportStr == "TraceBacks" )
    {
      command = QString("expView -x %1 %3%2 -v TraceBacks").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
    } else if ( currentUserSelectedReportStr == "TraceBacks,FullStack" )
    {
      command = QString("expView -x %1 %3%2 -v TraceBacks,FullStack").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
    } else if( currentUserSelectedReportStr == "Butterfly" )
    {
//      if( selectedFunctionStr.isEmpty() )
      {
        selectedFunctionStr = findSelectedFunction();
      }
      if( selectedFunctionStr.isEmpty() )
      {
        bool ok = FALSE;
// printf("B: NO FUNCTION SELECTED Prompt for one!\n");
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", QString("Which function?:"), QLineEdit::Normal, QString::null, &ok, this);
      }
      if( selectedFunctionStr.isEmpty() )
      {
        return( QString::null );
      }
      command = QString("expView -x %1 %4%2 -v Butterfly -f \"%3\"").arg(expID).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
    } else
    {
      command = QString("expView -x %1 %3%2 -v Functions").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
    }
    if( !currentThreadsStr.isEmpty() )
    {
       command += QString(" %1").arg(currentThreadsStr);
       about += QString("for threads %1\n").arg(currentThreadsStr);
    }
  } else if( (currentCollectorStr == "hwc" || currentCollectorStr == "hwctime") &&
            (currentUserSelectedReportStr == "Butterfly") ||
            (currentUserSelectedReportStr == "Functions") ||
            (currentUserSelectedReportStr == "LinkedObjects") ||
            (currentUserSelectedReportStr == "Statements") ||
            (currentUserSelectedReportStr == "CallTrees") ||
            (currentUserSelectedReportStr == "CallTrees,FullStack") ||
            (currentUserSelectedReportStr == "TraceBacks") ||
            (currentUserSelectedReportStr == "TraceBacks,FullStack") )
{
    if( currentUserSelectedReportStr.isEmpty() )
    { 
      currentUserSelectedReportStr = "Functions";
    }
  if( currentUserSelectedReportStr.startsWith("Statements") )
  { 
    command = QString("expView -x %1 %3%2 -v Statements").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
  } else
  {
//    command = QString("expView -x %1 %3%2 -v Functions").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr);
      command = QString("expView -x %1 %4%2 -v %5").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
  }
// printf("hwc command=(%s)\n", command.ascii() );
  about = command + "\n";
} 

// printf("command sofar... =(%s)\n", command.ascii() );
// printf("add any modifiers...\n");
    std::list<std::string> *modifier_list = NULL;;
// printf("generateCommand: currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    if( currentCollectorStr == "hwc" )
    {
      modifier_list = &current_list_of_hwc_modifiers;
    } else if( currentCollectorStr == "hwctime" )
    {
      modifier_list = &current_list_of_hwctime_modifiers;
    } else if( currentCollectorStr == "io" || currentCollectorStr == "iot" )
    {
      modifier_list = &current_list_of_io_modifiers;
    } else if( currentCollectorStr == "mpi" )
    {
      modifier_list = &current_list_of_mpi_modifiers;
    } else if( currentCollectorStr == "mpit" )
    {
      modifier_list = &current_list_of_mpit_modifiers;
    } else if( currentCollectorStr == "pcsamp" )
    {
      modifier_list = &current_list_of_pcsamp_modifiers;
    } else if( currentCollectorStr == "usertime" )
    {
      modifier_list = &current_list_of_usertime_modifiers;
    } else
    {
//      modifier_list = &current_list_of_modifiers;
      modifier_list = &current_list_of_generic_modifiers;
    }

    for( std::list<std::string>::const_iterator it = modifier_list->begin();
       it != modifier_list->end(); it++ )
    {
      std::string modifier = (std::string)*it;
// printf("modifer = (%s)\n", modifier.c_str() );
      if( modifierStr.isEmpty() )
      {
// printf("A: modifer = (%s)\n", modifier.c_str() );

        modifierStr = QString(" -m %1").arg(modifier.c_str());
        currentMetricStr = modifier.c_str();
      } else
      {
// printf("B: modifer = (%s)\n", modifier.c_str() );
        modifierStr += QString(",%1").arg(modifier.c_str());
      }
    }
    if( !modifierStr.isEmpty() )
    {
      command += QString(" %1").arg(modifierStr);
    }

    if( !traceAddition.isEmpty() )
    {
      command += traceAddition;
    }

// printf("generateCommand() returning (%s) currentCollectorStr=(%s)\n", command.ascii(), currentCollectorStr.ascii() );
  return( command );
} // End generateCommand

void
StatsPanel::generateMPIMenu(QString collectorName)
{
// printf("generateMPIMenu(%s)\n", collectorName.ascii() );
  mpi_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  mpi_menu->setCheckable(TRUE);

  mpi_menu->insertSeparator();



  list_of_mpi_modifiers.clear();
  list_of_mpit_modifiers.clear();
if( collectorName == "mpi" )
{
  addMPIReports(mpi_menu);
  connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPIReportSelected(int)) );
  contextMenu->insertItem(QString("Show Metrics: MPI"), mpi_menu);
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
//  list_of_mpi_modifiers.push_back("commuinicator");
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
} else
{
  addMPIReports(mpi_menu);
  connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPITReportSelected(int)) );
  contextMenu->insertItem(QString("Show Metrics: MPIT"), mpi_menu);
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
  list_of_mpit_modifiers.push_back("commuinicator");
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
}

}

void
StatsPanel::generateIOMenu(QString collectorName)
{
// printf("generateIOMenu(%s)\n", collectorName.ascii() );
  io_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  io_menu->insertSeparator();

  if( collectorName == "io" )
  {
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOReportSelected(int)) );
    contextMenu->insertItem(QString("Show Metrics: IO"), io_menu);
    // Build the static list of io modifiers.
    list_of_io_modifiers.clear();
    list_of_io_modifiers.push_back("io::exclusive_times");
    list_of_io_modifiers.push_back("io::inclusive_times");
//    list_of_io_modifiers.push_back("io::exclusive_details");
//    list_of_io_modifiers.push_back("io::inclusive_details");
    list_of_io_modifiers.push_back("min");
    list_of_io_modifiers.push_back("max");
    list_of_io_modifiers.push_back("average");
    list_of_io_modifiers.push_back("count");
    list_of_io_modifiers.push_back("percent");
    list_of_io_modifiers.push_back("stddev");

//    list_of_io_modifiers.push_back("start_time");
//    list_of_io_modifiers.push_back("stop_time");
//    list_of_io_modifiers.push_back("source");
//    list_of_io_modifiers.push_back("dest");
//    list_of_io_modifiers.push_back("size");
//    list_of_io_modifiers.push_back("tag");
//    list_of_io_modifiers.push_back("commuinicator");
//    list_of_io_modifiers.push_back("datatype");
//    list_of_io_modifiers.push_back("retval");
  
    if( ioModifierMenu )
    {
      delete ioModifierMenu;
    }
    ioModifierMenu = new QPopupMenu(this);
    ioModifierMenu->insertTearOffHandle();
    connect(ioModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(ioModifierSelected(int)) );
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
  } else 
  {
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOTReportSelected(int)) );
    contextMenu->insertItem(QString("Show Metrics: IOT"), io_menu);
    // Build the static list of iot modifiers.
    list_of_iot_modifiers.clear();
    list_of_iot_modifiers.push_back("iot::exclusive_times");
    list_of_iot_modifiers.push_back("iot::inclusive_times");
//    list_of_iot_modifiers.push_back("iot::exclusive_details");
//    list_of_iot_modifiers.push_back("iot::inclusive_details");
    list_of_iot_modifiers.push_back("min");
    list_of_iot_modifiers.push_back("max");
    list_of_iot_modifiers.push_back("average");
    list_of_iot_modifiers.push_back("count");
    list_of_iot_modifiers.push_back("percent");
    list_of_iot_modifiers.push_back("stddev");

    list_of_iot_modifiers.push_back("start_time");
    list_of_iot_modifiers.push_back("stop_time");
    list_of_iot_modifiers.push_back("source");
    list_of_iot_modifiers.push_back("dest");
    list_of_iot_modifiers.push_back("size");
    list_of_iot_modifiers.push_back("tag");
    list_of_iot_modifiers.push_back("commuinicator");
    list_of_iot_modifiers.push_back("datatype");
    list_of_iot_modifiers.push_back("retval");

    if( iotModifierMenu )
    {
      delete iotModifierMenu;
    }
    iotModifierMenu = new QPopupMenu(this);
    iotModifierMenu->insertTearOffHandle();
    connect(iotModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(iotModifierSelected(int)) );
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

  contextMenu->insertItem(QString("Show Metrics: hwc"), hwc_menu);

  addHWCReports(hwc_menu);
  connect(hwc_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCReportSelected(int)) );

  list_of_hwc_modifiers.clear();
  list_of_hwc_modifiers.push_back("hwc::hwc_overflows");
  
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
StatsPanel::generateHWCTimeMenu(QString collectorName)
{
// printf("Collector hwctime_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwctime_menu = new QPopupMenu(this);
  contextMenu->insertItem(QString("Show Metrics: hwctime"), hwctime_menu);

  addHWCTimeReports(hwctime_menu);
  connect(hwctime_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCTimeReportSelected(int)) );
  list_of_hwctime_modifiers.clear();
  list_of_hwctime_modifiers.push_back("hwctime::exclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::inclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::count");
  list_of_hwctime_modifiers.push_back("hwctime::percent");

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
StatsPanel::generateUserTimeMenu()
{
// printf("Collector usertime_menu is being created\n");

  usertime_menu = new QPopupMenu(this);
  connect(usertime_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorUserTimeReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;

//  usertime_menu->insertItem(QString("Show Metric: %1").arg(currentCollectorStr));

  contextMenu->insertItem(QString("Show Metrics: UserTime"), usertime_menu);

  list_of_usertime_modifiers.clear();
  list_of_usertime_modifiers.push_back("usertime::exclusive_times");
  list_of_usertime_modifiers.push_back("usertime::inclusive_times");
  list_of_usertime_modifiers.push_back("usertime::exclusive_details");
  list_of_usertime_modifiers.push_back("usertime::inclusive_details");
  list_of_usertime_modifiers.push_back("usertime::count");
  list_of_usertime_modifiers.push_back("usertime::percent");

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
StatsPanel::generatePCSampMenu()
{
// printf("Collector pcsamp_menu is being created\n");

  pcsamp_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;


  addPCSampReports(pcsamp_menu);
  connect(pcsamp_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorPCSampReportSelected(int)) );
  
  contextMenu->insertItem(QString("Show Metrics: pcsamp"), pcsamp_menu);

  list_of_pcsamp_modifiers.clear();
  list_of_pcsamp_modifiers.push_back("pcsamp::time");

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
StatsPanel::generateGenericMenu()
{
// printf("generateGenericMenu is being created\n");

  generic_menu = new QPopupMenu(this);
  connect(generic_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorGenericReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;

//  generic_menu->insertItem(QString("Show Metric: %1").arg(currentCollectorStr));
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

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to MPI Functions."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each MPI Functions."));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show Call Trees, with full stacks, to each MPI Functions."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
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

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each IO Functions."));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
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

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

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
  qaction->setToolTip(tr("Show timings for IO Functions."));

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show LinkedObjects.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show Statements.") );
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

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show by CallTrees.") );

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show by TraceBacks.") );

  qaction = new QAction(this, "showTraceBacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show trace backs, with full stacks, to Functions."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}


SourceObject *
StatsPanel::lookUpFileHighlights(QString function_name, Thread thread, ThreadGroup::iterator ti, QListViewItem *item, QString filename, QString lineNumberStr, HighlightList *highlightList)
{

// printf("lookUpFileHighlights: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
// printf("lookUpFileHighlights: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );

  SourceObject *spo = NULL;
  HighlightObject *hlo = NULL;
  
  std::set<Statement>::const_iterator di;

  Time time = Time::Now();
  const std::string lookup_string = std::string(function_name.ascii());
  std::pair<bool, Function> function = thread.getFunctionByName(lookup_string);
  std::set<Statement> statement_definition;
  statement_definition.clear();
  if( function.first )
  {
    statement_definition = function.second.getDefinitions();
  }
  if( statement_definition.size() > 0 )
  {
    di = statement_definition.begin();
// printf("FOUND THE FUNCTION in FILE (%s) line=%d\n", di->getPath().c_str(), di->getLine() );
    filename = di->getPath();
    lineNumberStr = QString("%1").arg(di->getLine());
  }

// printf("Try to query the metrics.\n");

// printf("currentItemIndex=%d\n", currentItemIndex);

  hlo = new HighlightObject(filename, lineNumberStr.toInt(), hotToCold_color_names[currentItemIndex], QString::null, QString("Beginning of function %1").arg(function_name.ascii()), (QString)*columnHeaderList.begin() );
  highlightList->push_back(hlo);
// printf("push_back function entry (currentItemIndex=%d\n", currentItemIndex);
// hlo->print();

// printf("Query:\n");
// printf("  %s\n", !currentCollectorStr.isEmpty() ? currentCollectorStr.ascii() : "NULL");
// printf("  %s\n", !currentThreadStr.isEmpty() ? currentThreadStr.ascii() : "NULL");
// printf("  %s\n", !currentMetricStr.isEmpty() ? currentMetricStr.ascii() : "NULL");

 
  // First, determine if we can simply set the defaults to the only
  // possible settings.
  if( list_of_collectors_metrics.size() == 1 && list_of_pids.size() == 1 )
  {
// printf("There's no confusion (and there's not defaults) simply set the defaults.\n");
    setCurrentCollector();
    setCurrentThread();
    setCurrentMetricStr();
  }

  setCurrentCollector();
  setCurrentMetricStr();
  if( currentThread )
  {
    delete currentThread;
  }
  currentThread = new Thread(*ti);
// printf("Getting the next currentThread (%d)\n", currentThread->getProcessId() );

  Metadata m = Find_Metadata(*currentCollector, std::string(currentMetricStr.ascii()) );

#if 0
// Begin DEBUG
printf("lookUpFileHighlights: metric=(%s)\n", currentMetricStr.ascii() );
std::string  id = m.getUniqueId();

if( m.isType(typeid(unsigned int)) )
{
printf("type: unsigned int\n");
} else if( m.isType(typeid(uint64_t)) )
{ 
printf("type: uint64_t\n");
} else if( m.isType(typeid(int)) ) 
{
printf("type: int\n");
} else if( m.isType(typeid(int64_t)) )
{
printf("type: int64_t\n");
} else if( m.isType(typeid(float)) )
{
printf("type: float\n");
} else if( m.isType(typeid(double)) )
{
printf("type: double\n");
} else if( m.isType(typeid(string)) )
{
printf("type: string\n");
} else
{
printf("UNknown type.\n");
}
// END DEBUG
#endif // 0

  if( m.isType(typeid(double)) )
  {
// printf("DOUBLE\n");
    // If double
    SmartPtr<std::map<int, double> > double_statement_data = Framework::SmartPtr<std::map<int, double> >(new std::map<int, double>() );;

// printf("GetMetric... %s:%s %d %s\n", currentCollectorStr.ascii(), currentMetricStr.ascii(), currentThread->getProcessId(), Path(filename.ascii()).c_str() );

    GetMetricByStatementOfFileInThread(*currentCollector, currentMetricStr.ascii(), TimeInterval(Time::TheBeginning(),Time::TheEnd()), *currentThread, filename.ascii(), double_statement_data);

    // Begin try to highlight source for doubles....
// printf("Build/append to a list of highlights for the source panel to update.\n");
      for(std::map<int, double>::const_iterator
          sit = double_statement_data->begin();
          sit != double_statement_data->end(); ++sit)
      {

        int64_t line = 1;
        int color_index = getLineColor(sit->second);
  
        // first check to see if there's already a hlo for this line number.
        // If there is, bump the value... Otherwise, push back a new one.
        bool FOUND = FALSE;
// printf("Do we have a duplicate? (%d) %f \n", sit->first, sit->second );
        for( HighlightList::Iterator it = highlightList->begin();
               it != highlightList->end();
               ++it)
        {
          hlo = (HighlightObject *)*it;
// printf("\thlo->line=(%d)\n", hlo->line );
          if( hlo->line == sit->first )
          {
// printf("We have a duplicate at line (%d)\n", sit->first );
            float v = hlo->value.toFloat();
// printf("%f + %f =%f\n", v, sit->second, v+sit->second );
            v += sit->second;
            hlo->value = QString("%1").arg(v);
// printf("  new value=(%s)\n", hlo->value.ascii() );
            hlo->description = QString("\nMetric %1 was %2.").arg(currentMetricStr).arg(v);
            hlo->value_description = (QString)*columnHeaderList.begin();
            color_index = getLineColor(v);
            hlo->color = hotToCold_color_names[color_index];
            FOUND = TRUE;
            break;
          }
        }

        if( !FOUND )
        {
          hlo = new HighlightObject(filename, sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 %2.").arg(currentMetricStr).arg(sit->second), (QString)*columnHeaderList.begin() );
          highlightList->push_back(hlo);
// printf("A: Push_back a hlo for %d %f (%s)\n", sit->first, sit->second, hlo->description.ascii() );
        }
// hlo->print();
      }
    } else
    {
      // Not a double value...
// printf("NOT DOUBLE\n");
      if( !m.isType(typeid(uint64_t)) )
      {
// printf("WARNING: ATTEMPTING TOO LOOK UP TYPE uint64_t WHEN METRIC IS NOT uint64_t\n");
//        fprintf(stderr, "report name=(%s)\n", currentUserSelectedReportStr.ascii() );
//        fprintf(stderr, "metric name=(%s)\n", currentCollectorStr.ascii() );
  
std::string  id = m.getUniqueId();
fprintf(stderr, "Unsupported type: Unable to display per statement source metrics for type:  ");
if( m.isType(typeid(unsigned int)) )
{
fprintf(stderr, " unsigned int\n");
} else if( m.isType(typeid(uint64_t)) )
{ 
fprintf(stderr, " uint64_t\n");
} else if( m.isType(typeid(int)) ) 
{
fprintf(stderr, " int\n");
} else if( m.isType(typeid(int64_t)) )
{
fprintf(stderr, "type: int64_t\n");
} else if( m.isType(typeid(float)) )
{
fprintf(stderr, " float\n");
} else if( m.isType(typeid(double)) )
{
fprintf(stderr, " double\n");
} else if( m.isType(typeid(string)) )
{
fprintf(stderr, " string\n");
} else
{
fprintf(stderr, "UNknown type.\n");
}
      }

      SmartPtr<std::map<int, uint64_t> > uint64_statement_data = Framework::SmartPtr<std::map<int, uint64_t> >(new std::map<int, uint64_t>() );;

      if( m.isType(typeid(uint64_t)) )
      {
        GetMetricByStatementOfFileInThread(*currentCollector, currentMetricStr.ascii(), TimeInterval(Time::TheBeginning(),Time::TheEnd()), *currentThread, Path(filename.ascii()), uint64_statement_data);
      }
      
// printf("uint64_statement_data->size(%d)\n", uint64_statement_data->size() );

      // Begin try to highlight source for doubles....
      for(std::map<int, uint64_t>::const_iterator
            sit = uint64_statement_data->begin();
            sit != uint64_statement_data->end(); ++sit)
      {
// printf("Build a list of highlights for the source panel to update.\n");
        int64_t line = 1;

        int color_index = getLineColor(sit->second);

        // first check to see if there's already a hlo for this line number.
        // If there is, bump the value... Otherwise, push back a new one.
        bool FOUND = FALSE;
// printf("Do we have a duplicate? (%d)\n", sit->first );
        for( HighlightList::Iterator it = highlightList->begin();
               it != highlightList->end();
               ++it)
        {
          hlo = (HighlightObject *)*it;
// printf("\thlo->line=(%d)\n", hlo->line );
          if( hlo->line == sit->first )
          {
// printf("We have a duplicate at line (%d)\n", sit->first );
            uint64_t v = hlo->value.toUInt();
// printf("v=%f\n", v );
            v += sit->second;
            hlo->value = QString("%1").arg(v);
            hlo->description = QString("\nMetric %1 was %2.").arg(currentMetricStr).arg(v);

            hlo->value_description = (QString)*columnHeaderList.begin();
            color_index = getLineColor(v);
            hlo->color = hotToCold_color_names[color_index];
            FOUND = TRUE;
            break;
          }
        }
        if( !FOUND )
        {
          hlo = new HighlightObject(filename, sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 was %2.").arg(currentMetricStr).arg(sit->second), (QString)*columnHeaderList.begin() );
          highlightList->push_back(hlo);
// printf("B: Push_back a hlo for %d %f\n", sit->first, sit->second);
// hlo->print();
        }
      }
    }



  currentItemIndex = 0;
  QListViewItemIterator lvit = (splv);
  while( lvit.current() )
  {
    QListViewItem *this_item = lvit.current();
  
    if( this_item == item )
    {
      break;
    }
  
    currentItemIndex++;
    lvit++;
  }
// printf("PREPARE the hlo and spo.  filename=(%s)\n", filename.ascii() );
  if( lineNumberStr != "-1" &&
    ( collectorStrFromMenu.startsWith("CallTrees") ||
      collectorStrFromMenu.startsWith("CallTrees,FullStack") ||
      collectorStrFromMenu.startsWith("Functions") ||
      collectorStrFromMenu.startsWith("TraceBacks") ||
      collectorStrFromMenu.startsWith("TraceBacks,FullStack") ) )
  {
      hlo = new HighlightObject(NULL, lineNumberStr.toInt(), hotToCold_color_names[2], ">>", "Callsite", "N/A");
    highlightList->push_back(hlo);
// printf("spo A: lineNumberStr=(%s)\n", lineNumberStr.ascii() );
    spo = new SourceObject(function_name.ascii(), filename.ascii(), lineNumberStr.toInt()-1, expID, TRUE, highlightList);
  } else
  {
// printf("spo B: lineNumberStr=(%s)\n", lineNumberStr.ascii() );
    spo = new SourceObject(function_name.ascii(), filename, lineNumberStr.toInt()-1, expID, TRUE, highlightList);
  }


#if 0
// Begin debug
  for( HighlightList::Iterator it = spo->highlightList->begin();
       it != spo->highlightList->end();
       ++it)
  {
    HighlightObject *dhlo = (HighlightObject *)*it;
printf("A: (%d)\n", dhlo->line );
  }
// End debug
#endif // 0



  return spo;
}
