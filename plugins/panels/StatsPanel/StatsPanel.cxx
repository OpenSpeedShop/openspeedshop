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

#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "FocusObject.hxx"
#include "FocusCompareObject.hxx"
#include "HighlightObject.hxx"
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



/*! Create a Stats Panel.
*/
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
// printf("StatsPanel() entered\n");
  setCaption("StatsPanel");

  traceFLAG == FALSE;

  mpi_io_FLAG = FALSE;
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
  about = QString::null;
  lastAbout = QString::null;
// printf("currentItemIndex initialized to 0\n");

  f = NULL;
  modifierMenu = NULL;
  list_of_modifiers.clear(); // This is the global known list of modifiers.
  current_list_of_modifiers.clear();  // This is this list of user selected modifiers.
  selectedFunctionStr = QString::null;
  threadMenu = NULL;
  currentMetricStr = QString::null;
  currentUserSelectedMetricStr = QString::null;
  metricHeaderTypeArray = NULL;
  currentThreadStr = QString::null;
  currentCollectorStr = QString::null;
  groupID = ao->int_data;
  expID = -1;
  descending_sort = true;
  TotalTime = 0;

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");

  splitterA->setOrientation( QSplitter::Horizontal );

  cf = new SPChartForm(this, splitterA, getName(), 0);
  cf->setCaption("SPChartFormIntoSplitterA");

  splv = new SPListView(this, splitterA, getName(), 0);
  splv->setSorting ( -1 );

  connect( splv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

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
      int i = name.find("mpi");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("mpit");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("io");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("iot");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
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
  QApplication::restoreOverrideCursor( );
  return FALSE;
}
// End determine if there's mpi stats

// printf("currentThreadStr=(%s)\n", currentThreadStr.ascii() );
// Currently this causes a second update when loading from a saved file. FIX
// printf("Currently this causes a second update when loading from a saved file. FIX\n");
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

// printf("A: Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
      int i = name.find("mpi");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("mpit");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("io");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
      }
      i = name.find("iot");
      if( i == 0 )
      {
        mpi_io_FLAG = TRUE;
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

  popupMenu = contextMenu; // So we can look up the text easily later.

  connect(contextMenu, SIGNAL( activated(int) ),
        this, SLOT(collectorMetricSelected(int)) );

  QAction *qaction = NULL;

//  contextMenu->insertItem("About...", this, SLOT(aboutSelected()) );
  qaction = new QAction( this,  "_aboutStatsPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "About..." );
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
  if( list_of_collectors.size() > 1 || list_of_pids.size() > 1 )
  {
// printf("We have more than one collector... one metric\n");
    defaultStatsReportStr = QString("Show Metric: %1").arg(currentCollectorStr);
    if( mpi_io_FLAG == FALSE )
    {
      mid = contextMenu->insertItem(defaultStatsReportStr);
    }
// printf("mid=%d for %s\n", mid, defaultStatsReportStr.ascii() );
  }
  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
  {
     std::string collector_name = (std::string)*it;
// printf("collector_name=(%s)\n", collector_name.c_str() );
// Only do this once for the mpi collector.  The mpi menus are not 
// driven of the metric names, but rather are static.
    if( QString(collector_name).startsWith("mpi") ||
        QString(collector_name).startsWith("io") )
    {
      // Build the static list of mpi modifiers.
      list_of_modifiers.clear();
      list_of_modifiers.push_back("exclusive_times");
      list_of_modifiers.push_back("min");
      list_of_modifiers.push_back("max");
      list_of_modifiers.push_back("average");
      list_of_modifiers.push_back("count");
      list_of_modifiers.push_back("percent");
      list_of_modifiers.push_back("stddev");

if( QString(collector_name).startsWith("mpit::exclusive_times") )
{
  list_of_modifiers.push_back("start_time");
  list_of_modifiers.push_back("stop_time");
  list_of_modifiers.push_back("source");
  list_of_modifiers.push_back("destination");
  list_of_modifiers.push_back("size");
  list_of_modifiers.push_back("tag");
  list_of_modifiers.push_back("comminicator");
  list_of_modifiers.push_back("datatype");
  list_of_modifiers.push_back("retval");
}

// printf("currentCollectorStr=%s\n", currentCollectorStr.ascii() );
      if( QString(collector_name).startsWith("mpi::exclusive_times") ||
          QString(collector_name).startsWith("mpit::exclusive_times") ||
          QString(collector_name).startsWith("io::exclusive_times") ||
          QString(collector_name).startsWith("iot::exclusive_times") )
      {
        QPopupMenu *mpi_io_Menu = new QPopupMenu(this);
        connect(mpi_io_Menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMetricSelected(int)) );

        QString s = QString::null;

        QAction *qaction = NULL;

        mpi_io_Menu->setCheckable(TRUE);
        qaction = new QAction(this, "showTraceInfo");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Present Trace Information") );
        qaction->setToggleAction(traceFLAG);
        qaction->setOn(traceFLAG);
        qaction->setToolTip(tr("When available, show traced timings."));
        connect( qaction, SIGNAL( activated() ), this, SLOT(traceSelected()) );

        mpi_io_Menu->insertSeparator();

        qaction = new QAction(this, "showFunctions");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Show Metric: Functions") );
        if( QString(collector_name).startsWith("mpi::exclusive_times") || 
            QString(collector_name).startsWith("mpit::exclusive_times") )
        {
          qaction->setToolTip(tr("Show timings for MPI Functions."));
        } else
        {
          qaction->setToolTip(tr("Show timings for IO Functions."));
        }

        qaction = new QAction(this, "showTracebacks");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Show Metric: TraceBacks") );
        if( QString(collector_name).startsWith("mpi::exclusive_times") ||
            QString(collector_name).startsWith("mpit::exclusive_times") )
        {
          qaction->setToolTip(tr("Show tracebacks to MPI Functions."));
        } else
        {
          qaction->setToolTip(tr("Show tracebacks to IO Functions."));
        }


        qaction = new QAction(this, "showTracebacks/FullStack");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Show Metric: TraceBacks/FullStack") );
        if( QString(collector_name).startsWith("mpi::exclusive_times")  ||
            QString(collector_name).startsWith("mpit::exclusive_times") )
        {
          qaction->setToolTip(tr("Show tracebacks, with full stacks, to MPI Functions."));
        } else
        {
          qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));
        }

        qaction = new QAction(this, "showCallTrees");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Show Metric: CallTrees") );
        if( QString(collector_name).startsWith("mpi::exclusive_times") ||
            QString(collector_name).startsWith("mpit::exclusive_times") )
        {
          qaction->setToolTip(tr("Show Call Trees to each MPI Functions."));
        } else
        {
          qaction->setToolTip(tr("Show Call Trees to each IO Functions."));
        }

        qaction = new QAction(this, "showButterfly");
        qaction->addTo( mpi_io_Menu );
        qaction->setText( tr("Show Metric: Butterfly") );
        qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

        if( !currentCollectorStr.isEmpty() && 
            (currentCollectorStr == "Functions" ||
             currentCollectorStr == "mpi" ||
             currentCollectorStr == "mpit" ||
             currentCollectorStr == "iot" ||
             currentCollectorStr == "io" ) )
        {
          qaction = new QAction(this, "showCallTreesBySelectedFunction");
          qaction->addTo( mpi_io_Menu );
          qaction->setText( tr("Show Metric: CallTrees by Selected Function") );
          if( QString(collector_name).startsWith("mpi::exclusive_times") ||
              QString(collector_name).startsWith("mpit::exclusive_times") )
          {
            qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
          } else
          {
            qaction->setToolTip(tr("Show Call Tree to IO routine for selected function."));
          }
        }
        if( modifierMenu )
        {
          delete modifierMenu;
        }
        modifierMenu = new QPopupMenu(this);
        modifierMenu->setCheckable(TRUE);
        connect(modifierMenu, SIGNAL( activated(int) ),
          this, SLOT(modifierSelected(int)) );
        for( std::list<std::string>::const_iterator it = list_of_modifiers.begin();
                it != list_of_modifiers.end(); it++ )
        {
          std::string modifier = (std::string)*it;
    
          QString s = QString(modifier.c_str() );
          mid = modifierMenu->insertItem(s);
          for( std::list<std::string>::const_iterator it = current_list_of_modifiers.begin();
               it != current_list_of_modifiers.end(); it++ )
          {
            std::string current_modifier = (std::string)*it;
// printf("building menu : current_list_of_modifier here one (%s)\n", current_modifier.c_str() );
            if( modifier == current_modifier )
            {
// printf("WE have a match to check\n");
              modifierMenu->setItemChecked(mid, TRUE);
            }
          }
        }
if( QString(collector_name).startsWith("mpi::exclusive_times") ||
    QString(collector_name).startsWith("mpit::exclusive_times") )
{
        mpi_io_Menu->insertItem(QString("Show mpi modifiers:"), modifierMenu);

        contextMenu->insertItem(QString("Show metrics: MPI"), mpi_io_Menu);
} else
{
        mpi_io_Menu->insertItem(QString("Show io modifiers:"), modifierMenu);

        contextMenu->insertItem(QString("Show metrics: IO"), mpi_io_Menu);
}
      }
    } else
    {
      QString s = QString("Show Metric: %1").arg(collector_name.c_str());
      mid = contextMenu->insertItem(s);
// printf("mid=%d for %s\n", mid, s.ascii() );

      if( currentMetricStr.isEmpty() && currentCollectorStr.isEmpty() )
      {
        int index = s.find("Show Metric:");
// printf("s=(%s)\n", s.ascii() );
        if( index != -1 )
        {
          index = s.find("::");
// printf("index=%d\n", index );
          currentCollectorStr = s.mid(13, index-13 );
          currentMetricStr = s.mid(index+2);
// printf("A: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
        }
      }
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
    qaction = new QAction( this,  "manageProcessesMenu");
    qaction->addTo( contextMenu );
    qaction->setText( "Manage Processes..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( manageProcessesSelected() ) );
    qaction->setStatusTip( tr(QString("There are over %1 processes to manage.  This brings up the Manage Processes\nPanel which is designed to handle large number of procesess/threads.").arg(MAX_PROC_MENU_DISPLAY)) );
  }

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "compareAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Compare..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( compareSelected() ) );
  qaction->setStatusTip( tr("Compare one experiment to another, one thread to another, ... currently unimplemented.") );

  contextMenu->insertSeparator();

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
    cf->show();
    // I'm not sure why, but the text won't draw unless the 
    // piechart is visible.
    updatePanel();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( chartFLAG == FALSE && statsFLAG == FALSE )
  {
    statsFLAG = TRUE;
    splv->show();
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
  updateStatsPanelData();
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

void
StatsPanel::aboutSelected()
{

  QString aboutString = about;
  // QMessageBox::information(this, "About stats information", aboutString, "Ok");
  QMessageBox about( "About stats information", aboutString, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this, "aboutStats", FALSE);

  about.show();
  about.exec();
}

void
StatsPanel::traceSelected()
{
  if( traceFLAG == TRUE )
  {
    traceFLAG = FALSE;
  } else
  {
    traceFLAG = TRUE;
  }
}


/*! Compare item was selected. */
void
StatsPanel::compareSelected()
{
  QString name = QString("ComparePanel [%1]").arg(expID);

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
    comparePanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ComparePanel", startPC, ao);
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
StatsPanel::itemSelected(QListViewItem *item)
{
// printf("StatsPanel::itemSelected(QListViewItem *) entered\n");

  if( item )
  {
// printf("  item->depth()=%d\n", item->depth() );

    SPListViewItem *nitem = (SPListViewItem *)item;
    int index = 0;
    while( nitem->parent() )
    {
// printf("looking for 0x%x\n", nitem->parent() );
      nitem = (SPListViewItem *)nitem->parent();
      index++;
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


  if( mpi_io_FLAG )
  { // The mpi tree is different.   We need to look up the highlighted
    // text directly.
  // printf("Here\n");
    SPListViewItem *selectedItem = (SPListViewItem *)splv->selectedItem();
    if( selectedItem )
    {
// printf("Got an ITEM!\n");
      QString ret_value = selectedItem->text(fieldCount-1);
      sf = ret_value.ascii();
// printf("         (%s)\n", sf.c_str() );
    }
  }

// First lets try to find the function/file pair.

  SourceObject *spo = NULL;
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();

  QString selected_function_qstring = QString(sf).stripWhiteSpace();
  QString lineNumberStr = "-1"; // MPI only

  QString filename = getFilenameFromString( QString(sf).stripWhiteSpace() ); 

  QString function_name = getFunctionNameFromString( QString(sf).stripWhiteSpace(), lineNumberStr );

// printf("AA: filename=(%s)\n", filename.ascii() );
// printf("AA: function_name=(%s) lineNumberStr=(%s)\n", function_name.ascii(), lineNumberStr.ascii() );

// printf("mpi_io_FLAG=(%d) currentCollectorStr=(%s)\n", mpi_io_FLAG, currentCollectorStr.ascii() );


  QApplication::setOverrideCursor(QCursor::WaitCursor);

  // Explicitly make sure the highlightList is clear.
  highlightList->clear();
  // Begin Find the file/function pair.
  try
  {
    std::set<Statement>::const_iterator di = NULL;
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
#ifdef PULL
          if( ts.isEmpty() )
          { // Temporary hack to get initial view to get the right numbers.
// printf("NULL! !!!!\n");
            foundFLAG = TRUE;
            break;
          }
#endif // PULL
          if( QString("%1").arg(thread.getProcessId()) == ts )
          {
// printf("Got it!!!!\n");
            foundFLAG = TRUE;
            break;
          }
        }
        if( foundFLAG == FALSE )
        {
// printf("foundFLAG == FALSE.. continue\n");
          continue;
        }
// printf("We've got a match, now can we look up the function (%s)?\n", function_name.ascii() );
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

// printf("Try to query the metrics.\n");
          HighlightObject *hlo = NULL;

// printf("currentItemIndex=%d\n", currentItemIndex);

          hlo = new HighlightObject(di->getPath(), di->getLine(), hotToCold_color_names[currentItemIndex], QString::null, QString("Beginning of function %1").arg(function_name.ascii()), (QString)*columnHeaderList.begin() );
          highlightList->push_back(hlo);
// printf("push_back function entry (currentItemIndex=%d\n", currentItemIndex);
// hlo->print();

// printf("Query:\n");
// printf("  %s\n", !currentCollectorStr.isEmpty() ? currentCollectorStr.ascii() : "NULL");
// printf("  %s\n", !currentThreadStr.isEmpty() ? currentThreadStr.ascii() : "NULL");
// printf("  %s\n", !currentMetricStr.isEmpty() ? currentMetricStr.ascii() : "NULL");

 
          // First, determine if we can simply set the defaults to the only
          // possible settings.
          if( list_of_collectors.size() == 1 && list_of_pids.size() == 1 )
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
          if( !mpi_io_FLAG )
          {
            if( item->text(0).contains(".") )
            {
// printf("DOUBLE\n");
              // If double
//  SmartPtr<std::map<int, double> > double_statement_data;
              SmartPtr<std::map<int, double> > double_statement_data = Framework::SmartPtr<std::map<int, double> >(new std::map<int, double>() );;

// printf("GetMetric... %s:%s %d %s\n", currentCollectorStr.ascii(), currentMetricStr.ascii(), currentThread->getProcessId(), Path(di->getPath()).c_str() );

              Queries::GetMetricByStatementOfFileInThread(*currentCollector, currentMetricStr.ascii(), TimeInterval(Time::TheBeginning(),Time::TheEnd()), *currentThread, Path(di->getPath()), double_statement_data);

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
                  hlo = new HighlightObject(di->getPath(), sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 %2.").arg(currentMetricStr).arg(sit->second), (QString)*columnHeaderList.begin() );
                  highlightList->push_back(hlo);
// printf("A: Push_back a hlo for %d %f (%s)\n", sit->first, sit->second, hlo->description.ascii() );
                }
// hlo->print();
              }
            } else
            {
// printf("NOT DOUBLE\n");
              // Not a double value...
              SmartPtr<std::map<int, uint64_t> > uint64_statement_data = Framework::SmartPtr<std::map<int, uint64_t> >(new std::map<int, uint64_t>() );;
            Queries::GetMetricByStatementOfFileInThread(*currentCollector, currentMetricStr.ascii(), TimeInterval(Time::TheBeginning(),Time::TheEnd()), *currentThread, Path(di->getPath()), uint64_statement_data);
      
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
                  hlo = new HighlightObject(di->getPath(), sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 was %2.").arg(currentMetricStr).arg(sit->second), (QString)*columnHeaderList.begin() );
                  highlightList->push_back(hlo);
// printf("B: Push_back a hlo for %d %f\n", sit->first, sit->second);
// hlo->print();
                }
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
// ADD CHECK HERE TO SEE IF filename == di->getPath() 
if( filename.isEmpty() )
{
  filename = di->getPath();
// printf("filename was empty.   setting to di->getPath((%s)\n", filename.ascii() ); 
}
if( filename != di->getPath() )
{
//  QString msg;
//  msg = QString("Filename mismatch: Confusion exists over which file to use.\nThe choices are %1 and %2.\nUsing the former").arg(di->getPath().arg(filename);
//  QMessageBox::information( (QWidget *)this, tr("Details..."),
//                               msg, QMessageBox::Ok );
filename = di->getPath();
}
// printf("PREPARE the hlo and spo.  filename=(%s)\n", filename.ascii() );
        if( mpi_io_FLAG && lineNumberStr != "-1" &&
            ( currentCollectorStr.startsWith("CallTrees") ||
              currentCollectorStr.startsWith("Functions") ||
              currentCollectorStr.startsWith("TraceBacks") ||
              currentCollectorStr.startsWith("TraceBacks/FullStack") ) )
        {
          hlo = new HighlightObject(NULL, lineNumberStr.toInt(), hotToCold_color_names[2], ">>", "Callsite", "N/A");
          highlightList->push_back(hlo);
// printf("spo A:\n");
//          spo = new SourceObject(function_name.ascii(), di->getPath(), lineNumberStr.toInt()-1, expID, TRUE, highlightList);
          spo = new SourceObject(function_name.ascii(), filename.ascii(), lineNumberStr.toInt()-1, expID, TRUE, highlightList);
        } else
        {
// printf("spo B:\n");
//          spo = new SourceObject(function_name.ascii(), di->getPath(), di->getLine()-1, expID, TRUE, highlightList);
          spo = new SourceObject(function_name.ascii(), filename, di->getLine()-1, expID, TRUE, highlightList);
}
        } else
        {
// printf("No definitioin for thread's function\n");
        }
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
// printf("sent the spo to the source panel.\n");
      }
    }
// printf("Now restore the cursor and return.\n");
    QApplication::restoreOverrideCursor( );
// delete highlightList;
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
  }

  if( command.isEmpty() )
  {
    return;
  }

// printf("command=(%s)\n", command.ascii() );

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
  about += "Command issued: " + command;
  InputLineObject *clip = Append_Input_String( cli->wid, (char *)command.ascii());

  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
//    return;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  while( status != ILO_COMPLETE )
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
//  if( splv->childCount() > 0 && total_percent > 0.0 &&
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
// printf("collectorMetricSelected: Full currentCollectorStr=(%s)\n", popupMenu->text(val).ascii() );


  currentUserSelectedMetricStr = QString::null;

  QString s = popupMenu->text(val).ascii();

  int index = s.find("Show Metric:");
  if( index != -1 )
  {
    index = s.find("::");
    if( index > 0 )
    { // The user selected one of the metrics
      currentCollectorStr = s.mid(13, index-13 );
      currentMetricStr = s.mid(index+2);
      currentUserSelectedMetricStr = currentMetricStr;
// printf("B1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
//      currentCollectorStr = s.mid(13, index-13 );
      currentUserSelectedMetricStr = s.mid(13, index-13);
// printf("B2: currentCollectorStr=(NULL) currentUserSelectedMetricStr=(%s)\n", currentCollectorStr.ascii(), currentUserSelectedMetricStr.ascii() );
      if( currentUserSelectedMetricStr != "Show Metric: CallTrees by Selected Function" )
      {
        selectedFunctionStr = QString::null;
      }
    }

// printf("Collector changed call updateStatsPanelData() \n");
    updateStatsPanelData();
  }
}


void
StatsPanel::modifierSelected(int val)
{ 
// printf("modifierSelected val=%d\n", val);
// printf("modifierSelected: (%s)\n", modifierMenu->text(val).ascii() );


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
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    current_list_of_modifiers.push_back(s);
  }

// Uncomment this line if the modifier selection to take place immediately.
// I used to do this, but it seemed wrong to make the use wait as they 
// selected each modifier.   Now, they select the modifier, then go out and
// reselect the Query...
//  updateStatsPanelData();
}

void
StatsPanel::raisePreferencePanel()
{
// printf("StatsPanel::raisePreferencePanel() \n");
  getPanelContainer()->getMainWindow()->filePreferences( statsPanelStackPage, QString(pluginInfo->panel_type) );
}

#ifdef PULL
void 
StatsPanel::clearSourceFile(int expID)
{
// printf("clearSourceFile() entered\n");
  SourceObject *spo = NULL;
  QString name = QString("Source Panel [%1]").arg(expID);
  Panel *sp = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if( !sp )
  {
    char *panel_type = "Source Panel";
    //Find the nearest toplevel and start placement from there...
    PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(getPanelContainer());
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);
//printf("StatsPanel::clearSourceFile() creating Source Panel\n");
    Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
    delete ao;
    if( p != NULL ) 
    {
      p->listener((void *)spo);
    }
  }
}
#endif // PULL

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
StatsPanel::updateCollectorMetricList()
{
  // Now get the collectors... and their metrics...
//  command = QString("listTypes -x %1").arg(expID);
//  QString command = QString("listMetrics -x %1").arg(expID);
  QString command = QString("list -v metrics -x %1").arg(expID);
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

  if( list_of_collectors.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
         it != list_of_collectors.end(); it++ )
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

  // The cli (by default) focuses on the last metric.   We should to 
  // otherwise, when trying to focus on the related source panel, we 
  // don't get the correct statistics showing up.
  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
       it != list_of_collectors.end(); it++ )
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
        headerStr = data.mid(start_index, end_index-start_index).stripWhiteSpace();
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
        headerStr = data.mid(start_index, header_end_index-start_index).stripWhiteSpace();
    
      }
      columnHeaderList.push_back(data.mid(start_index, end_index-start_index).stripWhiteSpace());
      splv->addColumn( data.mid(start_index, end_index-start_index).stripWhiteSpace() );
      // Find the percent column
// printf("headerStr=(%s)\n", headerStr.ascii() );
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

// printf("fieldCount=(%d)\n", fieldCount);
// printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );


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
  if( mpi_io_FLAG && ( currentUserSelectedMetricStr.startsWith("CallTrees") || currentUserSelectedMetricStr.startsWith("Functions") || currentUserSelectedMetricStr.startsWith("TraceBacks") || currentUserSelectedMetricStr.startsWith("TraceBacks/FullStack") || currentUserSelectedMetricStr.startsWith("Butterfly") ) )
  {
    QString indentChar = ">";

    if( currentUserSelectedMetricStr.startsWith("TraceBacks") || currentUserSelectedMetricStr.startsWith("TraceBacks/FullStack") )
    {
      indentChar = "<";
    } 
    bool indented = strings[fieldCount-1].startsWith(indentChar);
    int indent_level = 0;

// Pretty the output up a bit.
if( currentUserSelectedMetricStr.startsWith("Butterfly") )
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
        if( currentUserSelectedMetricStr.startsWith("Butterfly") )
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
// printf("Set the highlighted line!\n");
    highlight_item->setSelected(TRUE);
  }
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

  if( mpi_io_FLAG && ( currentCollectorStr.startsWith("CallTrees") || currentCollectorStr.startsWith("Functions") || currentCollectorStr.startsWith("TraceBacks") || currentCollectorStr.startsWith("TraceBacks/FullStack") ) )
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
// printf("GenerateCommand() traceFLAG == %d\n", traceFLAG );
  if( traceFLAG == TRUE )
  {
    traceAddition = "-v trace";
  }

  QString modifierStr = QString::null;

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
  if( !currentUserSelectedMetricStr.isEmpty() && !currentCollectorStr.isEmpty() )
  {
    if( currentCollectorStr != currentUserSelectedMetricStr )
    {  // If these 2 are equal, we want the default display... not a 
       // specific metric.
// printf("A: adding currentUserSelectedMetricStr=(%s) currentCollectorStr=(%s)\n", currentUserSelectedMetricStr.ascii(), currentCollectorStr.ascii()  );
       command += QString(" -m %1").arg(currentUserSelectedMetricStr);
       about += QString("for metrics %1\n").arg(currentUserSelectedMetricStr);
    }
  }
//  if( !mpi_io_FLAG )
  { 
    if( !currentThreadsStr.isEmpty() )
    {
       command += QString(" %1").arg(currentThreadsStr);
       about += QString("for threads %1\n").arg(currentThreadsStr);
    }
  }


// printf("so far: command=(%s) currentCollectorStr=(%s) currentUserSelectedMetricStr(%s) currentMetricStr=(%s)\n", command.ascii(), currentCollectorStr.ascii(), currentUserSelectedMetricStr.ascii(), currentMetricStr.ascii() );

  if( mpi_io_FLAG && ( currentUserSelectedMetricStr.startsWith("CallTrees") || currentUserSelectedMetricStr.startsWith("Functions") || currentUserSelectedMetricStr.startsWith("mpi") || currentUserSelectedMetricStr.startsWith("io") || currentUserSelectedMetricStr.startsWith("TraceBacks") || currentUserSelectedMetricStr.startsWith("TraceBacks/FullStack") || currentUserSelectedMetricStr.startsWith("Butterfly") ) )
  { 
    if( currentUserSelectedMetricStr.isEmpty() || currentUserSelectedMetricStr == "CallTrees" )
    {
      command = QString("expView -x %1 %3%2 -v CallTrees %4").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(traceAddition);
    } else if ( currentUserSelectedMetricStr == "CallTrees by Selected Function" )
    {
      if( selectedFunctionStr.isEmpty() )
      {
        selectedFunctionStr = findSelectedFunction();
      }
      if( selectedFunctionStr.isEmpty() )
      {
        return( QString::null );
      }
      command = QString("expView -x %1 %4%2 -v CallTrees -f %3 %4").arg(expID).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr).arg(traceAddition);
    } else if ( currentUserSelectedMetricStr == "TraceBacks" )
    {
      command = QString("expView -x %1 %3%2 -v TraceBacks %4").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(traceAddition);
    } else if ( currentUserSelectedMetricStr == "TraceBacks/FullStack" )
    {
      command = QString("expView -x %1 %3%2 -v TraceBacks,FullStack %4").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(traceAddition);
    } else if( currentUserSelectedMetricStr == "Butterfly" )
    {
      if( selectedFunctionStr.isEmpty() )
      {
        selectedFunctionStr = findSelectedFunction();
      }
      if( selectedFunctionStr.isEmpty() )
      {
        return( QString::null );
      }
      command = QString("expView -x %1 %4%2 -v Butterfly -f %3").arg(expID).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
    } else
    {
      command = QString("expView -x %1 %3%2 -v Functions %4").arg(expID).arg(numberItemsToDisplayInStats).arg(currentCollectorStr).arg(traceAddition);
    }
    if( !currentThreadsStr.isEmpty() )
    {
       command += QString(" %1").arg(currentThreadsStr);
       about += QString("for threads %1\n").arg(currentThreadsStr);
    }
  } 

// printf("add any modifiers...\n");
    for( std::list<std::string>::const_iterator it = current_list_of_modifiers.begin();
       it != current_list_of_modifiers.end(); it++ )
    {
      std::string modifier = (std::string)*it;
      if( modifierStr.isEmpty() )
      {
// printf("A: modifer = (%s)\n", modifier.c_str() );
        modifierStr = QString(" -m %1").arg(modifier.c_str());
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

// printf("generateCommand() returning (%s)\n", command.ascii() );
  return( command );
} // End generateCommand

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
