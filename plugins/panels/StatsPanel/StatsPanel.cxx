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



/*! Create a pc Sampling Specific Stats Panel.   This panel is derived
    from the StatsPanelBase class.  
*/
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
// printf("StatsPanel() entered\n");
  setCaption("StatsPanel");

  currentThread = NULL;
  currentCollector = NULL;
  currentItem = NULL;
  currentItemIndex = 0;
  lastlvi = NULL;
  gotHeader = FALSE;
  fieldCount = 0;
  percentIndex = -1;
  gotColumns = FALSE;
  lastAbout = QString::null;
// printf("currentItemIndex initialized to 0\n");

  f = NULL;
  metricMenu = NULL;
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
  showPercentageID = -1;
  showPercentageFLAG = TRUE;

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");

  splitterA->setOrientation( QSplitter::Horizontal );

  cf = new SPChartForm(this, splitterA, getName(), 0);
  cf->setCaption("SPChartFormIntoSplitterA");

  splv = new SPListView(this, splitterA, getName(), 0);

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
    showPercentageFLAG = TRUE;
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

  if( currentCollector )
  {
// printf("Destructor delete the currentCollector\n");
    delete currentCollector;
  }
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
// printf("StatsPanel::listener() msg->msgType = (%s)\n", msgObject->msgType.ascii() );
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
  } else if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
// printf("UpdateExperimentDataObject\n");

    UpdateObject *msg = (UpdateObject *)msgObject;
    expID = msg->expID;
    updateStatsPanelData();
    if( msg->raiseFLAG )
    {
    if( msg->raiseFLAG )
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
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

  if( metricMenu != NULL )
  {
    delete metricMenu;
  }
  metricMenu = NULL;

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
  metricMenu = new QPopupMenu(this);
  contextMenu->setCheckable(TRUE);
  int mid = -1;
  QString defaultStatsReportStr = QString::null;
  if( list_of_collectors.size() > 1 || list_of_pids.size() > 1 )
  {
// printf("We only have more than one collector... one metric\n");
    defaultStatsReportStr = QString("Show Metric: %1").arg(currentCollectorStr);
    mid = contextMenu->insertItem(defaultStatsReportStr);
// printf("mid=%d for %s\n", mid, defaultStatsReportStr.ascii() );
  }
  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
  {
     std::string collector_name = (std::string)*it;
// printf("collector_name=(%s)\n", collector_name.c_str() );
    QString s = QString("Show Metric: %1").arg(collector_name.c_str());
    mid = contextMenu->insertItem(s);
// printf("mid=%d for %s\n", mid, s.ascii() );
    if( currentMetricStr.isEmpty() || currentCollectorStr.isEmpty() )
    {
// printf("Can you toggle this (currentCollector) menu?\n");
//        contextMenu->setItemChecked(mid, TRUE);
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

  showPercentageID = contextMenu->insertItem("Show Percentages", this, SLOT(showPercentageSelected()) );
  contextMenu->setItemChecked(showPercentageID, showPercentageFLAG);

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
StatsPanel::showPercentageSelected()
{
  if( showPercentageFLAG == TRUE )
  {
    showPercentageFLAG = FALSE;
  } else
  {
    showPercentageFLAG = TRUE;
  }
  if( contextMenu )
  {
    contextMenu->setItemChecked(showPercentageID, showPercentageFLAG);
  }
  updateStatsPanelData();
}

void
StatsPanel::aboutSelected()
{

QString aboutString = lastAbout;
QMessageBox::information(this, "About stats information", aboutString, "Ok");
}

/*! Compare item was selected. */
void
StatsPanel::compareSelected()
{
printf("compareSelected() menu selected.\n");
QMessageBox::information(this, "compareSelected() unimplemented", "This functionality is currently unimplement.\nIt will eventually bring up a dialog that will\nallow many different comparisons.\n  - Compare one run to another.\n  - Compare on thread to another.\n  - Compare one thread against all others.\n  - ...\n", "Ok");
}

/*! Compare item was selected. */
void
StatsPanel::manageProcessesSelected()
{
// printf("manageProcessesSelected() menu selected.\n");
  QString name = QString("ManageProcessesPanel [%1]").arg(expID);


  Panel *manageProcessPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() found ManageProcessesPanel found.. raise it.\n");
    getPanelContainer()->raisePanel(manageProcessPanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() no ManageProcessesPanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    manageProcessPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", startPC, ao);
    delete ao;
  }

  if( manageProcessPanel )
  {
//    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", manageProcessPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, "pcsamp", 1);
      manageProcessPanel->listener( (void *)msg );
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
  
    
    if( nitem )
    {
// printf("YOU NEED TO LOOP THROUGH AND FIND THE FUNCTION FIELD!!!\n");
      currentItem = (SPListViewItem *)nitem;
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

// First lets try to find the function/file pair.

  SourceObject *spo = NULL;
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();
  QString selected_function_qstring = QString(sf).stripWhiteSpace();
  QString funcString = selected_function_qstring.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

// printf("funcString=(%s)\n", funcString.ascii() );

  int end_func_name_index = selected_function_qstring.find("(");
  QString function_name = selected_function_qstring.mid(0,end_func_name_index);

// printf("function_name=(%s)\n", function_name.ascii() );

  int start_file_index = selected_function_qstring.find(" ");
  QString intermediate_string = selected_function_qstring.mid(start_file_index+1);
  int end_file_index = intermediate_string.find(",");
  QString filename = intermediate_string.mid(0, end_file_index );

// printf("A: filename=(%s)\n", filename.ascii() );


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

          hlo = new HighlightObject(di->getPath(), di->getLine(), hotToCold_color_names[currentItemIndex], QString::null, QString("Beginning of function %1").arg(function_name.ascii()) );
          highlightList->push_back(hlo);
// printf("push_back function entry\n");
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
                  color_index = getLineColor(v);
                  hlo->color = hotToCold_color_names[color_index];
                  FOUND = TRUE;
                  break;
                }
              }

              if( !FOUND )
              {
                hlo = new HighlightObject(di->getPath(), sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 %2.").arg(currentMetricStr).arg(sit->second) );
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
                  color_index = getLineColor(v);
                  hlo->color = hotToCold_color_names[color_index];
                  FOUND = TRUE;
                  break;
                }
              }
              if( !FOUND )
              {
                hlo = new HighlightObject(di->getPath(), sit->first, hotToCold_color_names[color_index], QString("%1").arg(sit->second), QString("\nMetric %1 was %2.").arg(currentMetricStr).arg(sit->second) );
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
          spo = new SourceObject(function_name.ascii(), di->getPath(), di->getLine()-1, expID, TRUE, highlightList);

// End try to highlight source for doubles....
        } else
        { // Clear the highlight list.
// printf("No definitioin for thread's function\n");
        }
      }
    }
    // If no spo, make one so the source panel is placed correctly.
    if( !spo )
    {
//printf("NO SOURCE PANEL OBJECT to update existing source. Create null one.\n");
      spo = new SourceObject(NULL, NULL, -1, expID, TRUE, NULL);
    }
    if( spo )
    {
      QString name = QString("Source Panel [%1]").arg(expID);
//printf("Find a SourcePanel named %s\n", name.ascii() );
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
StatsPanel::updateStatsPanelData()
{
// printf("StatsPanel::updateStatsPanelData() entered.\n");


  SPListViewItem *splvi;
  columnHeaderList.clear();
  splv->clear();

  // Percent value list (for the chart)
  cpvl.clear();
  // Text value list (for the chart)
  ctvl.clear();
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



  lastlvi = NULL;
  gotHeader = FALSE;
  gotColumns = FALSE;
  fieldCount = 0;
  percentIndex = -1;

  for(int i=splv->columns();i>=0;i--)
  {
    splv->removeColumn(i-1);
  }

  updateCollectorMetricList();

  updateThreadsList();


  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", expID);

  splv->setSorting ( -1 );
  QString command = QString("expView -x %1").arg(expID);
  lastAbout = QString("Experiment: %1\n").arg(expID);
  if( currentCollectorStr.isEmpty() || showPercentageFLAG == FALSE )
  {
    command += QString(" %1%2").arg("stats").arg(numberItemsToDisplayInStats);
    lastAbout += QString("Requested data for all collectors for top %1 items\n").arg(numberItemsToDisplayInStats);
  } else
  {
    command += QString(" %1%2").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
    lastAbout += QString("Requested data for collector %1 for top %2 items\n").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);

  }
  if( !currentUserSelectedMetricStr.isEmpty() )
  {
     command += QString(" -m %1").arg(currentUserSelectedMetricStr);
     lastAbout += QString("for metrics %1\n").arg(currentUserSelectedMetricStr);
  }
  if( !currentThreadsStr.isEmpty() )
  {
     command += QString(" %1").arg(currentThreadsStr);
     lastAbout += QString("for threads %1\n").arg(currentThreadsStr);
  }

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  Redirect_Window_Output( cli->wid, spoclass, spoclass );
// printf("command: (%s)\n", command.ascii() );
  lastAbout += "Command issued: " + command;
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
// printf("done pinging...\n");

  //Test putting the output to statspanel stream.
  Default_TLI_Line_Output(clip);



// Put out the chart if there is one...
   char **color_names = hotToCold_color_names;
   if( descending_sort != true )
   {
    color_names = coldToHot_color_names;
   }
   textFLAG = getPreferenceShowTextInChart();
   if( !textFLAG )
   {
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
      if( textFLAG )
      {
        ctvl.push_back( "other" );
      }
    }
  }

  // Or were there no percents in the initial query.
// printf("total_percent = %f showPercentageFLAG = %d\n", total_percent, showPercentageFLAG );
  if( total_percent == 0.0 || showPercentageFLAG == FALSE )
  {
    cpvl.clear();
    ctvl.clear();
    cpvl.push_back(100);
    ctvl.push_back("N/A");
  }
// printf("cpvl.count()=%d ctvl.count()=%d\n", cpvl.count(), ctvl.count() );

  cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);

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

// printf("currentThreadsStr = %s\n", currentThreadsStr.ascii() );


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
      index = s.find(":");
      currentCollectorStr = s.mid(13, index-13 );
      currentMetricStr = QString::null;
currentUserSelectedMetricStr = QString::null;
// printf("B2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }
    updateStatsPanelData();
  }
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
  QString command = QString("listMetrics -x %1").arg(expID);
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
  QString command = QString("listPids -x %1").arg(expID);
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
// printf("The currentCollector has been set.\n");
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

  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
       it != list_of_collectors.end(); it++ )
  {
       std::string collector_name = (std::string)*it;
       QString s = QString(collector_name.c_str() );
// printf("collector_name=(%s)\n", collector_name.c_str() );
      if( currentMetricStr.isEmpty() )
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
StatsPanel::outputCLIData(QString *data)
{
// printf("%s", data->ascii() );
  // Skip any blank lines.
  if( *data == QString("\n") )
  {
    return;
  }

  // The first line is a header ... mine this for the column headers.
  QString stripped_data = data->stripWhiteSpace();
  if( gotHeader == FALSE )
  {
    QRegExp rxp = QRegExp( "  [A-Z,a-z,0-9,%]");
    fieldCount = stripped_data.contains( rxp );
    fieldCount++;

    int start_index = 0;
    int end_index = 99999;
    for(int i=0;i<fieldCount;i++)
    {
      end_index = stripped_data.find(rxp, start_index);

      if( end_index == -1 )
      {
        columnHeaderList.push_back(stripped_data.mid(start_index).stripWhiteSpace());
        splv->addColumn( stripped_data.mid(start_index).stripWhiteSpace() );
        break;
      }
      columnHeaderList.push_back(stripped_data.mid(start_index, end_index-start_index));
      splv->addColumn( stripped_data.mid(start_index, end_index-start_index) );
// Find the percent column
      if( stripped_data.find("%") != -1 )
      {
        percentIndex = i;
      }
  
      start_index = end_index+2;
    }
  
    gotHeader = TRUE;
    return;
  }


//  if( gotColumns == FALSE )
  {
    QRegExp rxp = QRegExp( " [A-Z,a-z,0-9,%,_]");
// printf("Figure out where the fields start and stop.\n");
    int start_index = 0;
    int end_index = 99999;
    int MAX_COLUMN_COUNT = 10;
// printf("stripped_data=(%s)\n", stripped_data.ascii() );
    for(int i=0;i<fieldCount && i < MAX_COLUMN_COUNT;i++)
    {
      end_index = stripped_data.find(rxp, start_index);

// printf(" end_index = (%d)\n", end_index );
      if( end_index == -1 || i == fieldCount-1 )
      {
        columnValueClass[i].start_index = start_index;
        columnValueClass[i].end_index = 99999;
// columnValueClass[i].print();
        break;
      }
      columnValueClass[i].start_index = start_index;
      columnValueClass[i].end_index = end_index;
  
      start_index = end_index+1;
    }
    gotColumns = TRUE;
  }

  QString *strings = new QString[fieldCount];
 
  int percent = 0;
  for( int i = 0; i<fieldCount; i++)
  {
    int si = columnValueClass[i].start_index;
    int l = columnValueClass[i].end_index-columnValueClass[i].start_index;
    QString value = stripped_data.mid(si,l).stripWhiteSpace();
    if( i == 0 ) // Grab the (some) default metric FIX
    {
      float f = value.toFloat();
      TotalTime += f;
    }
    if( percentIndex == i )
    {
      float f = value.toFloat();
      percent = (int)f;
// printf("percent=(%d)\n", percent);
      total_percent += f;
    }
    strings[i] = value;
  }
// printf("total_percent=%f\n", total_percent );


  SPListViewItem *splvi;
  if( fieldCount == 2 )
  {
    lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1] );
  } else if( fieldCount == 3 )
  { // i.e. like pcsamp
    lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1], strings[2] );
  } else
  { // i.e. like usertime
    lastlvi = splvi =  new SPListViewItem( this, splv, lastlvi, strings[0], strings[1], strings[2], strings[3] );
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
    ctvl.push_back( strings[percentIndex].stripWhiteSpace() );
  }

}
