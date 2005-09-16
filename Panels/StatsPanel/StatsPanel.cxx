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

#include <qvaluelist.h>
#include <qmessagebox.h>
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;

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
// printf("currentItemIndex initialized to 0\n");

  f = NULL;
  metricMenu = NULL;
  currentMetricStr = QString::null;
  metricHeaderTypeArray = NULL;
  collectorStr = QString::null;
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

  connect( splv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

  connect( splv->header(), SIGNAL( clicked(int) ), this, SLOT( sortColumn(int) ) );


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
//printf("StatsPanel got a new FocusObject\n");
    FocusObject *msg = (FocusObject *)msgObject;
// msg->print();
    expID = msg->expID;
    threadStr = msg->pid_name;
// Currently this causes a second update when loading from a saved file. FIX
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
if( item && matchSelectedItem( std::string(item->text(2).ascii()) ) )
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
    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() UpdateExperimentDataObject!\n");

  if( expID == -1 || expID != msg->expID )
  {
    // Initialize the collector list....
    clo = new CollectorListObject(msg->expID);

    // Now that you have the list initialize the default currentMetricStr 
    // and default collector string (if it's the first time..)

    if( collectorStr.isEmpty() && currentMetricStr.isEmpty() )
    {
      CollectorEntry *ce = NULL;
      CollectorEntryList::Iterator it;
      for( it = clo->collectorEntryList.begin();
             it != clo->collectorEntryList.end();
             ++it )
      {
        ce = (CollectorEntry *)*it;
        CollectorMetricEntryList::Iterator pit = NULL;
        for( pit =  ce->metricList.begin();pit != ce->metricList.end();  pit++)
        {
          CollectorMetricEntry *cpe = (CollectorMetricEntry *)*pit;
// printf("\t%s\n", ce->name.ascii() );
// printf("\t%s\n", cpe->name.ascii() );
          collectorStr = ce->name;
          currentMetricStr = cpe->name;
          currentMetricTypeStr = cpe->type;
// printf("Initialize collectorStr=(%s) currentMetricType=(%s) currentMetricStr=(%s)\n", collectorStr.ascii(), currentMetricTypeStr.ascii(), currentMetricStr.ascii() );
          if( currentMetricTypeStr != "double" &&
              currentMetricStr != "unsigned int" )
          {
            // HACK!  FIX FIXME 
//            if( currentMetricTypeStr == "unknown" )
//            {
//              printf("Flag this as a uint64_t\n");
//            }
            // Once we've passed the MS converge on the template
            // approach the cli is using.
            currentMetricTypeStr = "uint64_t";
          }
          break;
        }
      }
    }
  }
  expID = msg->expID;
// I know this is a problem.  -FIX
// If a user adds a collector, after creating the StatsPanel,
// we'll somehow need to update this list... (Maybe via a message
// to the listener...
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

  connect(contextMenu, SIGNAL( highlighted(int) ),
        this, SLOT(contextMenuHighlighted(int)) );

  contextMenu->insertSeparator();
  // Over all the collectors....
  // Round up the metrics ....
  // Create a menu of metrics....
  // (Don't forget to do this for threads as well...)
  CollectorEntry *ce = NULL;
  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
         it != clo->collectorEntryList.end();
         ++it )
  {
    ce = (CollectorEntry *)*it;
    metricMenu = new QPopupMenu(this);
    connect(metricMenu, SIGNAL( highlighted(int) ),
               this, SLOT(metricMenuHighlighted(int)) );
    contextMenu->insertItem(QString("Show Metric : %1").arg(ce->name), metricMenu);

    CollectorMetricEntryList::Iterator pit = NULL;
    for( pit =  ce->metricList.begin();pit != ce->metricList.end();  pit++)
    {
      CollectorMetricEntry *cpe = (CollectorMetricEntry *)*pit;
// printf("\t%s\n", cpe->name.ascii() );
//      metricMenu->insertItem(cpe->name);
//      metricMenu->insertItem(cpe->name);
      QAction *qaction = new QAction( this,  "metric item");
      qaction->addTo( metricMenu );
      qaction->setText( cpe->name );
if( currentMetricStr.isEmpty() || currentMetricStr == cpe->name )
{
  currentMetricStr = cpe->name;
  qaction->setToggleAction(TRUE);
  qaction->setOn(TRUE);
} 
      connect( qaction, SIGNAL( activated() ), this, SLOT( metricSelected() ) );
      qaction->setStatusTip( tr("Query metric values %1.").arg(cpe->name) );
    }
  }
  // Begin try a pid/thread menu hook 
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);

    if( eo->FW() != NULL )
    {
  // The following bit of code was snag and modified from SS_View_exp.cxx
      ThreadGroup tgrp = eo->FW()->getThreads();
      ThreadGroup::iterator ti;
      bool atleastone = false;
      threadMenu = new QPopupMenu(this);
      connect(threadMenu, SIGNAL( highlighted(int) ),
                 this, SLOT(threadMenuHighlighted(int)) );
      contextMenu->insertItem(QString("Show Thread/Process"), threadMenu);
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
      {
        Thread t = *ti;
        std::string host = t.getHost();
        pid_t pid = t.getProcessId();
        if (!atleastone)
        {
          atleastone = true;
        }
        QString pidstr = QString("%1").arg(pid);
        QAction *qaction = new QAction( this,  "pidThreadRank");
        qaction->addTo( threadMenu );
        qaction->setText( pidstr );
        if( currentThread == NULL || *currentThread == t )
        {
          if( currentThread == NULL )
          {
            currentThread = new Thread(*ti);
          }
          qaction->setToggleAction(TRUE);
          qaction->setOn(TRUE);
        }
        connect( qaction, SIGNAL( activated() ), this, SLOT( threadSelected() ) );
        qaction->setStatusTip( tr("Query metric values for %1.").arg(pidstr) );
      }
    }
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return 0;
  }

  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "compareAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Compare..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( compareSelected() ) );
  qaction->setStatusTip( tr("Compare one experiment to another, one thread to another, ... currently unimplemented.") );

  contextMenu->insertSeparator();

  int id = 0;
  QPopupMenu *columnsMenu = new QPopupMenu(this);
  columnsMenu->setCaption("Columns Menu");
  contextMenu->insertItem("&Columns Menu", columnsMenu, CTRL+Key_C);

  for( ColumnList::Iterator pit = columnList.begin();
           pit != columnList.end();
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
  contextMenu->insertItem("&Re-orientate", this, SLOT(setOrientation()), CTRL+Key_R );
  if( chartFLAG == TRUE )
  {
    contextMenu->insertItem("Hide &Chart...", this,
    SLOT(showChart()), CTRL+Key_L );
  } else
  {
    contextMenu->insertItem("Show &Chart...", this,
    SLOT(showChart()), CTRL+Key_L );
  }
  if( statsFLAG == TRUE )
  {
    contextMenu->insertItem("Hide &Statistics...", this,
    SLOT(showStats()), CTRL+Key_L );
  } else
  {
    contextMenu->insertItem("Show &Statistics...", this,
    SLOT(showStats()), CTRL+Key_L );
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
  nprintf(DEBUG_PANELS) ("StatsPanel::showChart() entered\n");
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
  nprintf(DEBUG_PANELS) ("StatsPanel::showStats() entered\n");
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
  nprintf(DEBUG_PANELS) ("StatsPanel::setOrientation() entered\n");
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
    QString types( "Any Files (*);;"
                      "Text files (*.txt);;"
                      );
    fd->setFilters( types );
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

  if( f != NULL )
  {
    // Write out the header info
    QString line = QString("  ");
    for(i=0;i<cols;i++)
    {
      for(i=0;i<cols;i++)
      {
        line += QString(splv->columnText(i))+" ";
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
        line += QString(item->text(i))+" ";
      }
      line += QString("\n");
      f->writeBlock( line, qstrlen(line) );
      ++it;
    }
    f->close();
  }

  f = NULL;

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

/*! Go to source menu item was selected. */
void
StatsPanel::compareSelected()
{
//  printf("compareSelected() menu selected.\n");
QMessageBox::information(this, "compareSelected() unimplemented", "This functionality is currently unimplement.\nIt will eventually bring up a dialog that will\nallow many different comparisons.\n  - Compare one run to another.\n  - Compare on thread to another.\n  - Compare one thread against all others.\n  - ...\n", "Ok");
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
// printf("here's the parent! 0x%x\n", nitem);
//      printf("  here's the rank of that parent: function name = %s\n",
//        nitem->text(1).ascii() );
//      matchSelectedItem( atoi( nitem->text(1).ascii() ) );

// printf("YOU NEED TO LOOP THROUGH AND FIND THE FUNCTION FIELD!!!\n");
      currentItem = (SPListViewItem *)nitem;
      matchSelectedItem( std::string(nitem->text(2).ascii()) );
    }
  }
}

void
StatsPanel::sortColumn(int column)
{
// printf("C: Display the results currentMetricTypeStr=(%s)\n", currentMetricTypeStr.ascii() );
  if( currentMetricTypeStr == "unsigned int" )
  {
    sortUintColumn(column);
  } else if( currentMetricTypeStr == "uint64_t" )
  {
    sortUInt64Column(column);
  } else
  {
    sortDoubleColumn(column);
  }
}

void
StatsPanel::sortDoubleColumn(int column)
{
// printf("StatsPanel::sortDoubleColumn(%d) entered\n", column);

  // For now we only allow column 0 to be sorted.
  if( column > 0 )
  {
    return;
  }

  int index = 0;
  int count = 0;
  ChartTextValueList ctvl;
  ctvl.clear();
  ChartPercentValueList cpvl;
  cpvl.clear();
  SPListViewItem *lvi;

  // How many rows should we display?
  bool ok;
  int numberItemsToDisplay = -1;
  if( !getPreferenceTopNLineEdit().isEmpty() )
  {
    numberItemsToDisplay = getPreferenceTopNLineEdit().toInt(&ok);
  }
// printf("numberItemsToDisplay=%d\n", numberItemsToDisplay );

  if( column >= 0 )
  {
    if( descending_sort == true )
    { 
// printf("Toggle to ascending...\n");
      splv->header()->setSortIndicator( column, Qt::Ascending );
      descending_sort = false;
    } else
    {
// printf("Toggle to descending...\n");
      splv->header()->setSortIndicator( column, Qt::Descending );
      descending_sort = true;
    }
  }

// printf("Now really sort the items: descending_sort=%d\n", descending_sort);

if( column == -1 )
{
// printf("Resort all the orig_double_data items\n");
  sorted_double_items.clear();
  // sort by the time metric
  // Now we can sort the data.
  for(std::map<Function, double>::const_iterator
                item = orig_double_data->begin(); item != orig_double_data->end(); ++item)
  {
    sorted_double_items.push_back( *item );
  }
          
  if (descending_sort == true )
  {
    std::sort(sorted_double_items.begin(), sorted_double_items.end(), sort_descending<Function_double_pair>());
  } else
  {
    std::sort(sorted_double_items.begin(), sorted_double_items.end(), sort_ascending<Function_double_pair>());
  }
}


  splv->clear();

if( column == -1 )
{
  TotalTime = Get_Double_Total_Time();
}
  


if( column == -1 )
{
// printf("Push to a separate list, the topN functions.\n");
  topNsorted_double_items.clear();
  // Now, for the sorted items, just grab the top 'N'.
  for(std::vector<Function_double_pair>::const_iterator
                it = sorted_double_items.begin(); it != sorted_double_items.end(); ++it)
  {
    topNsorted_double_items.push_back( *it );
    if(numberItemsToDisplay >= 0 )
    {
      numberItemsToDisplay--;
      if( numberItemsToDisplay == 0)
      {
        // That's all the user requested...
        break;  
      }
    }
  }
}

  nprintf( DEBUG_PANELS) ("Put the data out...\n");
  
if( descending_sort == true )
{
// printf("Put out the descending list.\n");
  for(std::vector<Function_double_pair>::reverse_iterator
                it = topNsorted_double_items.rbegin(); it != topNsorted_double_items.rend(); ++it)
  {
//    std::vector<Function_double_pair> item = it;
// putItem(it);
    char cputimestr[50];
    char a_percent_str[50];
    a_percent_str[0] = '\0';
    // convert time to %
    double percent_factor = 100.0 / TotalTime;
    double a_percent = 0; // accumulated percent
    double c_percent = 0; // accumulated percent
    QString funcInfo = QString::null;
  
    c_percent = it->second*percent_factor;  // current item's percent of total time
      sprintf(cputimestr, "%f", it->second);
      sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
  
      std::set<Statement> T = it->first.getDefinitions();
      if( T.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = T.begin(); ti != T.end(); ti++)
        {
          if (ti != T.begin())
          {
            funcInfo += "  &...";
              break;
          }
            Statement s = *ti;
          char l[20];
          sprintf( &l[0], " %lld", (int64_t)s.getLine());
          funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
        }
      }
      funcInfo += QString(")");
  
      lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
//    printf("A: Put out (%s)\n", cputimestr);
  } // end for
} else
{
// printf("Put out the ascending list.\n");
  for(std::vector<Function_double_pair>::const_iterator
                it = sorted_double_items.begin(); it != sorted_double_items.end(); ++it)
  {
//    std::vector<Function_double_pair> item = *it;
// putItem(it);
    char cputimestr[50];
    char a_percent_str[50];
    a_percent_str[0] = '\0';
    // convert time to %
    double percent_factor = 100.0 / TotalTime;
    double a_percent = 0; // accumulated percent
    double c_percent = 0; // accumulated percent
    QString funcInfo = QString::null;
  
    c_percent = it->second*percent_factor;  // current item's percent of total time
      sprintf(cputimestr, "%f", it->second);
      sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
  
      std::set<Statement> T = it->first.getDefinitions();
      if( T.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = T.begin(); ti != T.end(); ti++)
        {
          if (ti != T.begin())
          {
            funcInfo += "  &...";
              break;
          }
            Statement s = *ti;
          char l[20];
          sprintf( &l[0], " %lld", (int64_t)s.getLine());
          funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
        }
      }
      funcInfo += QString(")");
  
      lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
//    printf("B: Put out (%s)\n", cputimestr);
  }
}



//  splv->setSorting ( -1, descending_sort );
  splv->setSorting ( -1 );
  splv->header()->setSortIndicator( column == -1 ? 0 : column, descending_sort );

// printf("Set the chart elements\n");
  // Set the values for the top 5 pie chart elements...
  QListViewItemIterator it( splv );
  int total_percent = 0;
  while( it.current() )
  {
    QListViewItem *item = *it;
    {
      cpvl.push_back( (int)item->text(1).toFloat() );
// printf("values[%d] = (%d)\n", index, values[index] );
      total_percent += (int)item->text(1).toFloat();
      ctvl.push_back( (char *)item->text(1).ascii() );
      count = index+1;
    }
    index++;
      
    ++it;
  }

  // Now put out the graph
  int i = 0;
  if( total_percent < 100 )
  {
    cpvl.push_back( 100-total_percent );
    ctvl.push_back( "other" );
    count++;
  }
// printf("put out %d number of pie slices.\n", count);
char **color_names = hotToCold_color_names;
if( descending_sort != true )
{
 color_names = coldToHot_color_names;
}
if( !getPreferenceShowTextInChart() )
{
  ctvl.clear();
} 
cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);



}


void
StatsPanel::sortUintColumn(int column)
{
// printf("StatsPanel::sortUintColumn(%d) entered\n", column);

  // For now we only allow column 0 to be sorted.
  if( column > 0 )
  {
    return;
  }

  int index = 0;
  int count = 0;
  ChartTextValueList ctvl;
  ctvl.clear();
  ChartPercentValueList cpvl;
  cpvl.clear();
  SPListViewItem *lvi;

  // How many rows should we display?
  bool ok;
  int numberItemsToDisplay = -1;
  if( !getPreferenceTopNLineEdit().isEmpty() )
  {
    numberItemsToDisplay = getPreferenceTopNLineEdit().toInt(&ok);
  }

  if( column >= 0 )
  {
    if( descending_sort == true )
    { 
// printf("Toggle to ascending...\n");
      splv->header()->setSortIndicator( column, Qt::Ascending );
      descending_sort = false;
    } else
    {
// printf("Toggle to descending...\n");
      splv->header()->setSortIndicator( column, Qt::Descending );
      descending_sort = true;
    }
  }

// printf("Now really sort the items: descending_sort=%d\n", descending_sort);

if( column == -1 )
{
// printf("Resort all the orig_uint_data items\n");
  sorted_uint_items.clear();
  // sort by the time metric
  // Now we can sort the data.
  for(std::map<Function, unsigned int>::const_iterator
                item = orig_uint_data->begin(); item != orig_uint_data->end(); ++item)
  {
    sorted_uint_items.push_back( *item );
  }
          
  if (descending_sort == true )
  {
    std::sort(sorted_uint_items.begin(), sorted_uint_items.end(), sort_descending<Function_uint_pair>());
  } else
  {
    std::sort(sorted_uint_items.begin(), sorted_uint_items.end(), sort_ascending<Function_uint_pair>());
  }
}


  splv->clear();

if( column == -1 )
{
  TotalTime = Get_UInt_Total_Time();
}
  


if( column == -1 )
{
// printf("Push to a separate list, the topN functions.\n");
  topNsorted_uint_items.clear();
  // Now, for the sorted items, just grab the top 'N'.
  for(std::vector<Function_uint_pair>::const_iterator
                it = sorted_uint_items.begin(); it != sorted_uint_items.end(); ++it)
  {
    topNsorted_uint_items.push_back( *it );
    if(numberItemsToDisplay >= 0 )
    {
      numberItemsToDisplay--;
      if( numberItemsToDisplay == 0)
      {
        // That's all the user requested...
        break;  
      }
    }
  }
}

  nprintf( DEBUG_PANELS) ("Put the data out...\n");
  
if( descending_sort == true )
{
// printf("Put out the descending list.\n");
  for(std::vector<Function_uint_pair>::reverse_iterator
                it = topNsorted_uint_items.rbegin(); it != topNsorted_uint_items.rend(); ++it)
  {
    char cputimestr[50];
    char a_percent_str[50];
    a_percent_str[0] = '\0';
    // convert time to %
    double percent_factor = 100.0 / TotalTime;
    double a_percent = 0; // accumulated percent
    double c_percent = 0; // accumulated percent
    QString funcInfo = QString::null;
  
    c_percent = it->second*percent_factor;  // current item's percent of total time
      sprintf(cputimestr, "%u", it->second);
// printf("Here!\n");
// printf("str=%u name=(%s)\n", it->second, it->first.getName().c_str() );
// cout << "str: " << it->second << "\n";
      sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
  
      std::set<Statement> T = it->first.getDefinitions();
      if( T.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = T.begin(); ti != T.end(); ti++)
        {
          if (ti != T.begin())
          {
            funcInfo += "  &...";
              break;
          }
            Statement s = *ti;
          char l[20];
          sprintf( &l[0], " %lld", (int64_t)s.getLine());
          funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
        }
      }
      funcInfo += QString(")");
  
      lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
//    printf("A: Put out (%s)\n", cputimestr);
  } // end for
} else
{
// printf("Put out the ascending list.\n");
  for(std::vector<Function_uint_pair>::const_iterator
                it = sorted_uint_items.begin(); it != sorted_uint_items.end(); ++it)
  {
//    std::vector<Function_uint_pair> item = *it;
// putItem(it);
    char cputimestr[50];
    char a_percent_str[50];
    a_percent_str[0] = '\0';
    // convert time to %
    double percent_factor = 100.0 / TotalTime;
    double a_percent = 0; // accumulated percent
    double c_percent = 0; // accumulated percent
    QString funcInfo = QString::null;
  
    c_percent = it->second*percent_factor;  // current item's percent of total time
      sprintf(cputimestr, "%f", it->second);
      sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
  
      std::set<Statement> T = it->first.getDefinitions();
      if( T.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = T.begin(); ti != T.end(); ti++)
        {
          if (ti != T.begin())
          {
            funcInfo += "  &...";
              break;
          }
            Statement s = *ti;
          char l[20];
          sprintf( &l[0], " %lld", (int64_t)s.getLine());
          funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
        }
      }
      funcInfo += QString(")");
  
      lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
//    printf("B: Put out (%s)\n", cputimestr);
  }
}



//  splv->setSorting ( -1, descending_sort );
  splv->setSorting ( -1 );
  splv->header()->setSortIndicator( column == -1 ? 0 : column, descending_sort );

  // Set the values for the top 5 pie chart elements...
  QListViewItemIterator it( splv );
  int total_percent = 0;
  while( it.current() )
  {
    QListViewItem *item = *it;
    if( index < 5 )
    {
      cpvl.push_back( (int)item->text(1).toFloat() );
// printf("values[%d] = (%d)\n", index, values[index] );
      total_percent += (int)item->text(1).toFloat();
      ctvl.push_back( (char *)item->text(1).ascii() );
      count = index+1;
    }
    index++;
      
    ++it;
  }

  // Now put out the graph
  int i = 0;
  if( total_percent < 100 )
  {
    cpvl.push_back( 100-total_percent );
    ctvl.push_back( "other" );
    count++;
  }
if( descending_sort == true )
{
  cf->setValues(cpvl, ctvl, hotToCold_color_names, MAX_COLOR_CNT);
} else
{
  cf->setValues(cpvl, ctvl, coldToHot_color_names, MAX_COLOR_CNT);
}

}


void
StatsPanel::sortUInt64Column(int column)
{
// printf("StatsPanel::sortUInt64Column(%d) entered\n", column);

  // For now we only allow column 0 to be sorted.
  if( column > 0 )
  {
    return;
  }

  int index = 0;
  int count = 0;
  ChartTextValueList ctvl;
  ctvl.clear();
  ChartPercentValueList cpvl;
  cpvl.clear();
  SPListViewItem *lvi;

  // How many rows should we display?
  bool ok;
  int numberItemsToDisplay = -1;
  if( !getPreferenceTopNLineEdit().isEmpty() )
  {
    numberItemsToDisplay = getPreferenceTopNLineEdit().toInt(&ok);
    if( !ok )
    {
      numberItemsToDisplay = 5; // Default to top5.
    }
  }

  if( column >= 0 )
  {
    if( descending_sort == true )
    { 
// printf("Toggle to ascending...\n");
      splv->header()->setSortIndicator( column, Qt::Ascending );
      descending_sort = false;
    } else
    {
// printf("Toggle to descending...\n");
      splv->header()->setSortIndicator( column, Qt::Descending );
      descending_sort = true;
    }
  }

// printf("Now really sort the items: descending_sort=%d\n", descending_sort);

  if( column == -1 )
  {
// printf("Resort all the orig_uint64_data items\n");
    sorted_uint64_items.clear();
    // sort by the time metric
    // Now we can sort the data.
    for(std::map<Function, uint64_t>::const_iterator
                  item = orig_uint64_data->begin(); item != orig_uint64_data->end(); ++item)
    {
      sorted_uint64_items.push_back( *item );
    }
// printf("Okay ... sorted them..\n");
          
    if (descending_sort == true )
    {
      std::sort(sorted_uint64_items.begin(), sorted_uint64_items.end(), sort_descending<Function_uint64_pair>());
    } else
    {
      std::sort(sorted_uint64_items.begin(), sorted_uint64_items.end(), sort_ascending<Function_uint64_pair>());
    }
  }

// printf("Are you still okay?\n");


  splv->clear();

  if( column == -1 )
  {
    TotalTime = Get_UInt64_Total_Time();
  }

// printf("You got the TotalTime\n");
  


  if( column == -1 )
  {
// printf("Push to a separate list, the topN functions.\n");
    topNsorted_uint64_items.clear();
    // Now, for the sorted items, just grab the top 'N'.
    for(std::vector<Function_uint64_pair>::const_iterator
                  it = sorted_uint64_items.begin(); it != sorted_uint64_items.end(); ++it)
    {
      topNsorted_uint64_items.push_back( *it );
      if(numberItemsToDisplay >= 0 )
      {
        numberItemsToDisplay--;
        if( numberItemsToDisplay == 0)
        {
          // That's all the user requested...
          break;  
        }
      }
    }
  }

// printf("Put the data out...\n");
  
  if( descending_sort == true )
  {
// printf("Put out the descending list.\n");
    for(std::vector<Function_uint64_pair>::reverse_iterator
                it = topNsorted_uint64_items.rbegin(); it != topNsorted_uint64_items.rend(); ++it)
    {
      char cputimestr[50];
      char a_percent_str[50];
      a_percent_str[0] = '\0';
      // convert time to %
      double percent_factor = 100.0 / TotalTime;
      double a_percent = 0; // accumulated percent
      double c_percent = 0; // accumulated percent
      QString funcInfo = QString::null;
    
      c_percent = it->second*percent_factor;  // current item's percent of total time
        sprintf(cputimestr, "%lld", it->second);
// printf("Blob: Here!\n");
// printf("A:\tstr=%lld name=(%s)\n", it->second, it->first.getName().c_str() );
// cout << "A:\tstr: " << it->second << "\n";

        sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
    
        std::set<Statement> T = it->first.getDefinitions();
        if( T.size() > 0 )
        {
          std::set<Statement>::const_iterator ti;
          for (ti = T.begin(); ti != T.end(); ti++)
          {
            if (ti != T.begin())
            {
              funcInfo += "  &...";
                break;
            }
              Statement s = *ti;
            char l[20];
            sprintf( &l[0], " %lld", (int64_t)s.getLine());
            funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
          }
        }
        funcInfo += QString(")");
    
        lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
// printf("A: Put out (%s)\n", cputimestr);
    } // end for
  } else
  {
// printf("Put out the ascending list.\n");
    for(std::vector<Function_uint64_pair>::const_iterator
                it = sorted_uint64_items.begin(); it != sorted_uint64_items.end(); ++it)
    {
//    std::vector<Function_uint64_pair> item = *it;
// putItem(it);
      char cputimestr[50];
      char a_percent_str[50];
      a_percent_str[0] = '\0';
      // convert time to %
      double percent_factor = 100.0 / TotalTime;
      double a_percent = 0; // accumulated percent
      double c_percent = 0; // accumulated percent
      QString funcInfo = QString::null;
    
      c_percent = it->second*percent_factor;  // current item's percent of total time
        sprintf(cputimestr, "%lld", it->second);
// printf("\tcputimestr=(%s)\n", cputimestr);
// cout << "str: " << it->second << "\n";
        sprintf(a_percent_str, "%f", c_percent); funcInfo = it->first.getName().c_str() + QString(" (") + it->first.getLinkedObject().getPath().getBaseName();
    
        std::set<Statement> T = it->first.getDefinitions();
        if( T.size() > 0 )
        {
          std::set<Statement>::const_iterator ti;
          for (ti = T.begin(); ti != T.end(); ti++)
          {
            if (ti != T.begin())
            {
              funcInfo += "  &...";
                break;
            }
              Statement s = *ti;
            char l[20];
            sprintf( &l[0], " %lld", (int64_t)s.getLine());
            funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
          }
        }
        funcInfo += QString(")");
    
        lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
// printf("B: Put out (%s)\n", cputimestr);
    }
  }



//  splv->setSorting ( -1, descending_sort );
  splv->setSorting ( -1 );
  splv->header()->setSortIndicator( column == -1 ? 0 : column, descending_sort );

  // Set the values for the top 5 pie chart elements...
  QListViewItemIterator it( splv );
  int total_percent = 0;
  while( it.current() )
  {
    QListViewItem *item = *it;
    if( index < 5 )
    {
      cpvl.push_back( (int)item->text(1).toFloat() );
// printf("values[%d] = (%d)\n", index, values[index] );
      total_percent += (int)item->text(1).toFloat();
      ctvl.push_back( (char *)item->text(1).ascii() );
      count = index+1;
    }
    index++;
      
    ++it;
  }

  // Now put out the graph
  int i = 0;
  if( total_percent < 100 )
  {
    cpvl.push_back( 100-total_percent );
    ctvl.push_back( "other" );
    count++;
  }
if( descending_sort == true )
{
  cf->setValues(cpvl, ctvl, hotToCold_color_names, MAX_COLOR_CNT);
} else
{
  cf->setValues(cpvl, ctvl, coldToHot_color_names, MAX_COLOR_CNT);
}

// printf("finished up putting out values.\n");
}

void
StatsPanel::putItem(std::vector<Function_double_pair> *item)
{
#ifdef SYNTAX
  char cputimestr[50];
  char a_percent_str[50];
  a_percent_str[0] = '\0';
  // convert time to %
  double percent_factor = 100.0 / TotalTime;
  double a_percent = 0; // accumulated percent
  double c_percent = 0; // accumulated percent
  QString funcInfo = QString::null;

  c_percent = item->second*percent_factor;  // current item's percent of total time
    sprintf(cputimestr, "%f", item->second);
    sprintf(a_percent_str, "%f", c_percent); funcInfo = item->first.getName().c_str() + QString(" (") + item->first.getLinkedObject().getPath().getBaseName();

    std::set<Statement> T = item->first.getDefinitions();
    if( T.size() > 0 )
    {
      std::set<Statement>::const_iterator ti;
      for (ti = T.begin(); ti != T.end(); ti++)
      {
        if (ti != T.begin())
        {
          funcInfo += "  &...";
          break;
        }
        Statement s = *ti;
        char l[20];
        sprintf( &l[0], " %lld", (int64_t)s.getLine());
        funcInfo = funcInfo + ": " + s.getPath().getBaseName() + "," + &l[0];
      }
    }
    funcInfo += QString(")");

    lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, funcInfo.ascii() );
// printf("Put out (%s)\n", cputimestr);
#endif // SYNTAX
}


static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanel::doOption(int id)
{
  dprintf("doOption() id=%d\n", id);

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
StatsPanel::matchSelectedItem(std::string sf )
{
// printf("A: Display the results currentMetricTypeStr=(%s)\n", currentMetricTypeStr.ascii() );
  if( currentMetricTypeStr == "unsigned int" )
  {
    return( matchUIntSelectedItem(sf) );
  } else if( currentMetricTypeStr == "double" )
  {
    return( matchDoubleSelectedItem(sf) );
  } else
  {
    return( matchUInt64SelectedItem(sf) );
  }
}


bool
StatsPanel::matchDoubleSelectedItem(std::string sf )
{
  bool foundFLAG = FALSE;
  SourceObject *spo = NULL;
// printf ("StatsPanel::matchDoubleSelectedItem() = %s\n", sf.c_str() );

  QString selected_function_qstring = QString(sf);
  QString funcString = selected_function_qstring.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

  try
  {
    std::vector<Function_double_pair>::const_iterator it = sorted_double_items.begin();
    std::set<Statement> definitions = it->first.getDefinitions();
int index = 0;
    for( ; it != sorted_double_items.end(); ++it)
    {
// printf("%s %f\n", it->first.getName().c_str(), it->second );
      if( selected_function == it->first.getName()  || sf == NULL )
      {
// printf("FOUND IT!\n");
        foundFLAG = TRUE;
        currentItemIndex = index;

        definitions = it->first.getDefinitions();
        if(definitions.size() > 0 )
        {
//        for( std::set<Statement>::const_iterator i = definitions.begin(); i != definitions.end(); ++i)
//        {
//          std::cout << " (" << i->getPath().baseName()
//              << ", " << i->getLine() << ")";
//        }
//        std::cout << std::endl;
          break;
        } else
        {
fprintf(stderr, "No function definition for this entry.   Unable to position source.\n");
          QMessageBox::information(this, "Open|SpeedShop", "No function definition for this entry.\nUnable to position source. (No symbols.)\n", "Ok");


          clearSourceFile(expID);

          return foundFLAG;
        }
      }
      index++;
    }
// printf("FOUND? foundFLAG=%d\n", foundFLAG);

    if( definitions.size() > 0 )
    {
      std::set<Statement>::const_iterator di = definitions.begin();
      HighlightList *highlightList = new HighlightList();
      highlightList->clear();
      HighlightObject *hlo = NULL;
// cout << "name: " << di->getPath() << " line: " << di->getLine()-1 << "red" << "HighlightInfo note." << "\n";
      int64_t line = 1;
      if( definitions.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = definitions.begin(); ti != definitions.end(); ti++)
        {
          if (ti != definitions.begin())
          {
            break;
          }
          Statement s = *ti;
          line = (int64_t)s.getLine();
        }
      }
      // Put out statment metrics for this file.
//      if( getPanelContainer()->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() )
      {
        QApplication::setOverrideCursor(QCursor::WaitCursor);

// printf("Look up metrics by statement in file.\n");
        if( currentMetricTypeStr == "unsigned int" )
        {
// printf("You shouldn't be here.  double function for an unsigned int???\n");
          Queries::GetUIntMetricByStatementInFileForThread(*currentCollector, currentMetricStr.ascii(), di->getPath(), *currentThread, orig_uint_statement_data);

          for(std::map<int, unsigned int>::const_iterator
                item = orig_uint_statement_data->begin();
                item != orig_uint_statement_data->end(); ++item)
          {
// printf("item->first=%d\n", item->first);
// printf("item->second=%f\n", item->second );
// printf("A: TotalTime=%f\n", TotalTime);
// printf("A: item->second=%f\n", item->second);
            int color_index = getLineColor(item->second);
//            hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[color_index], item->second, (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
            hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[color_index], QString("%1").arg(item->second), (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
            highlightList->push_back(hlo);
// printf("Push_back a hlo for %d %f\n", item->first, item->second);
          }
        } else
        {
          Queries::GetMetricByStatementInFileForThread(*currentCollector, currentMetricStr.ascii(), di->getPath(), *currentThread, orig_double_statement_data);
//printf("Looked up metrics by statement in file.\n");

          for(std::map<int, double>::const_iterator
                item = orig_double_statement_data->begin();
                item != orig_double_statement_data->end(); ++item)
          {
// printf("item->first=%d\n", item->first);
// printf("item->second=%f\n", item->second );
//printf("B: TotalTime=%f\n", TotalTime);
// printf("B: item->second=%f\n", item->second);
            int color_index = getLineColor(item->second);
//            hlo = new HighlightObject(di->getPath(), item->first,  hotToCold_color_names[color_index], item->second, (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
            hlo = new HighlightObject(di->getPath(), item->first,  hotToCold_color_names[color_index], QString("%1").arg(item->second), (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
            highlightList->push_back(hlo);
// printf("Push_back a hlo for %d %f\n", item->first, item->second);
          }
        }
      }

//      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], -1, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], QString::null, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      highlightList->push_back(hlo);
      spo = new SourceObject(it->first.getName().c_str(), di->getPath(), di->getLine()-1, expID, TRUE, highlightList);

    } else
    {
      clearSourceFile(expID);
// printf("No file found.\n");
      return foundFLAG;
    }

    if( spo )
    {
      QString name = QString("Source Panel [%1]").arg(expID);
// printf("Find a SourcePanel named %s\n", name.ascii() );
      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel )
      {
//printf("no source view up, place the source panel.\n");
        char *panel_type = "Source Panel";
//        PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(getPanelContainer()->parentPanelContainer);
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
//printf("send the spo to the source panel.\n");
        sourcePanel->listener((void *)spo);
//printf("sent the spo to the source panel.\n");
      }
    }
    qApp->restoreOverrideCursor( );
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return foundFLAG;
  }


  return( foundFLAG );
}


bool
StatsPanel::matchUIntSelectedItem(std::string sf )
{
  bool foundFLAG = FALSE;
  SourceObject *spo = NULL;
// printf ("StatsPanel::matchUIntSelectedItem() = %s\n", sf.c_str() );

  QString selected_function_qstring = QString(sf);
  QString funcString = selected_function_qstring.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

  try
  {
int index = 0;
    std::vector<Function_uint_pair>::const_iterator it = sorted_uint_items.begin();
    std::set<Statement> definitions = it->first.getDefinitions();
    for( ; it != sorted_uint_items.end(); ++it)
    {
// printf("%s %f\n", it->first.getName().c_str(), it->second );
      if( selected_function == it->first.getName()  || sf == NULL )
      {
        currentItemIndex = index;
// printf("FOUND IT!\n");
        foundFLAG = TRUE;

        definitions = it->first.getDefinitions();
        if(definitions.size() > 0 )
        {
//        for( std::set<Statement>::const_iterator i = definitions.begin(); i != definitions.end(); ++i)
//        {
//          std::cout << " (" << i->getPath().baseName()
//              << ", " << i->getLine() << ")";
//        }
//        std::cout << std::endl;
          break;
        } else
        {
fprintf(stderr, "No function definition for this entry.   Unable to position source.\n");
          QMessageBox::information(this, "Open|SpeedShop", "No function definition for this entry.\nUnable to position source. (No symbols.)\n", "Ok");


          clearSourceFile(expID);

          return foundFLAG;
        }
      }
      index++;
    }
// printf("FOUND? foundFLAG=%d\n", foundFLAG);

    if( definitions.size() > 0 )
    {
      std::set<Statement>::const_iterator di = definitions.begin();
      HighlightList *highlightList = new HighlightList();
      highlightList->clear();
      HighlightObject *hlo = NULL;
// cout << "name: " << di->getPath() << " line: " << di->getLine()-1 << "red" << "HighlightInfo note." << "\n";
      int64_t line = 1;
      if( definitions.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = definitions.begin(); ti != definitions.end(); ti++)
        {
          if (ti != definitions.begin())
          {
            break;
          }
          Statement s = *ti;
          line = (int64_t)s.getLine();
        }
      }
      // Put out statment metrics for this file.
//      if( getPanelContainer()->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() )
      {
        QApplication::setOverrideCursor(QCursor::WaitCursor);

// printf("Look up metrics by statement in file.\n");
if( currentMetricTypeStr == "unsigned int" )
{
        Queries::GetUIntMetricByStatementInFileForThread(*currentCollector, currentMetricStr.ascii(), di->getPath(), *currentThread, orig_uint_statement_data);
//printf("Looked up metrics by statement in file.\n");

        for(std::map<int, unsigned int>::const_iterator
              item = orig_uint_statement_data->begin();
              item != orig_uint_statement_data->end(); ++item)
        {
// printf("item->first=%d\n", item->first);
// printf("item->second=%u\n", item->second );
//          hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[(int)(TotalTime/item->second)], item->second, (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
          hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[(int)(TotalTime/item->second)], QString("%1").arg(item->second), (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
          highlightList->push_back(hlo);
// printf("Push_back a hlo for %d %f\n", item->first, item->second);
        }
} else
{
// printf("You shouldn't be here.  unsigned int function for an double???\n");
        Queries::GetMetricByStatementInFileForThread(*currentCollector, currentMetricStr.ascii(), di->getPath(), *currentThread, orig_double_statement_data);
//printf("Looked up metrics by statement in file.\n");

        for(std::map<int, double>::const_iterator
              item = orig_double_statement_data->begin();
              item != orig_double_statement_data->end(); ++item)
        {
// printf("item->first=%d\n", item->first);
// printf("item->second=%f\n", item->second );
//          hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[(int)(TotalTime/item->second)], item->second, (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
          hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[(int)(TotalTime/item->second)], QString("%1").arg(item->second), (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
          highlightList->push_back(hlo);
// printf("Push_back a hlo for %d %f\n", item->first, item->second);
        }
}
      }

//      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], -1, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], QString::null, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      highlightList->push_back(hlo);
      spo = new SourceObject(it->first.getName().c_str(), di->getPath(), di->getLine()-1, expID, TRUE, highlightList);

    } else
    {
      clearSourceFile(expID);
// printf("No file found.\n");
      return foundFLAG;
    }

    if( spo )
    {
      QString name = QString("Source Panel [%1]").arg(expID);
// printf("Find a SourcePanel named %s\n", name.ascii() );
      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel )
      {
//printf("no source view up, place the source panel.\n");
        char *panel_type = "Source Panel";
//        PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(getPanelContainer()->parentPanelContainer);
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
//printf("send the spo to the source panel.\n");
        sourcePanel->listener((void *)spo);
//printf("sent the spo to the source panel.\n");
      }
    }
    qApp->restoreOverrideCursor( );
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return foundFLAG;
  }


  return( foundFLAG );
}


bool
StatsPanel::matchUInt64SelectedItem(std::string sf )
{
  bool foundFLAG = FALSE;
  SourceObject *spo = NULL;
// printf ("StatsPanel::matchUInt64SelectedItem() = %s\n", sf.c_str() );

  QString selected_function_qstring = QString(sf);
  QString funcString = selected_function_qstring.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

  try
  {
int index = 0;
    std::vector<Function_uint64_pair>::const_iterator it = sorted_uint64_items.begin();
    std::set<Statement> definitions = it->first.getDefinitions();
    for( ; it != sorted_uint64_items.end(); ++it)
    {
// printf("%s %f\n", it->first.getName().c_str(), it->second );
      if( selected_function == it->first.getName()  || sf == NULL )
      {
        currentItemIndex = index;
// printf("FOUND IT!\n");
        foundFLAG = TRUE;

        definitions = it->first.getDefinitions();
        if(definitions.size() > 0 )
        {
          break;
        } else
        {
// fprintf(stderr, "No function definition for this entry.   Unable to position source.\n");
          QMessageBox::information(this, "Open|SpeedShop", "No function definition for this entry.\nUnable to position source. (No symbols.)\n", "Ok");


          clearSourceFile(expID);

          return foundFLAG;
        }
      }
      index++;
    }
// printf("FOUND? foundFLAG=%d\n", foundFLAG);

    if( definitions.size() > 0 )
    {
      std::set<Statement>::const_iterator di = definitions.begin();
      HighlightList *highlightList = new HighlightList();
      highlightList->clear();
      HighlightObject *hlo = NULL;
// cout << "name: " << di->getPath() << " line: " << di->getLine()-1 << "red" << "HighlightInfo note." << "\n";
      int64_t line = 1;
      if( definitions.size() > 0 )
      {
        std::set<Statement>::const_iterator ti;
        for (ti = definitions.begin(); ti != definitions.end(); ti++)
        {
          if (ti != definitions.begin())
          {
            break;
          }
          Statement s = *ti;
          line = (int64_t)s.getLine();
        }
      }
      // Put out statment metrics for this file.
//      if( getPanelContainer()->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() )
      {
        QApplication::setOverrideCursor(QCursor::WaitCursor);

// printf("Look up metrics by statement in file.\n");
        Queries::GetUIntMetricByStatementInFileForThread(*currentCollector, currentMetricStr.ascii(), di->getPath(), *currentThread, orig_uint64_statement_data);
// printf("Looked up metrics by statement in file.\n");

        for(std::map<int, uint64_t>::const_iterator
              item = orig_uint64_statement_data->begin();
              item != orig_uint64_statement_data->end(); ++item)
        {
// printf("item->first=%d\n", item->first);
// printf("item->second=%lld\n", item->second );
          char uint64_value_buffer[100];
          sprintf(uint64_value_buffer, "%lld", item->second);
// printf("uint64_value_buffer = (%s)\n", uint64_value_buffer);
          int color_index = getLineColor(item->second);
          hlo = new HighlightObject(di->getPath(), item->first, hotToCold_color_names[color_index], QString(uint64_value_buffer), (char *)QString("\n%1: This line took %2 seconds.").arg(threadStr).arg(item->second).ascii());
          highlightList->push_back(hlo);
// printf("Push_back a hlo for %d %lld\n", item->first, item->second);
// printf("Push_back a hlo for %lld\n", item->second);
        }
      }

// printf("now push back the beginning of function...\n");

//      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], -1, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      hlo = new HighlightObject(di->getPath(), line, hotToCold_color_names[currentItemIndex], QString::null, (char *)QString("Beginning of function %1").arg(it->first.getName().c_str()).ascii() );
      highlightList->push_back(hlo);
// printf("create an spo \n");
      spo = new SourceObject(it->first.getName().c_str(), di->getPath(), di->getLine()-1, expID, TRUE, highlightList);

    } else
    {
      clearSourceFile(expID);
// printf("No file found.\n");
      return foundFLAG;
    }

    if( spo )
    {
      QString name = QString("Source Panel [%1]").arg(expID);
// printf("Find a SourcePanel named %s\n", name.ascii() );
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
        sourcePanel->listener((void *)spo);
// printf("sent the spo to the source panel.\n");
      }
    }
    qApp->restoreOverrideCursor( );
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return foundFLAG;
  }


// printf("return %d from StatsPanel::matchUInt64SelectedItem()\n", foundFLAG );
  return( foundFLAG );
}

void
StatsPanel::updateStatsPanelData()
{
  nprintf( DEBUG_PANELS) ("StatsPanel::updateStatsPanelData() entered.\n");
printf("StatsPanel::updateStatsPanelData() entered.\n");

  SPListViewItem *lvi;
  columnList.clear();
  splv->clear();
  for(int i=splv->columns();i>=0;i--)
  {
    splv->removeColumn(i-1);
  }

  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", expID);

  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *fw_experiment = eo->FW();
      // Evaluate the collector's time metric for all functions in the thread
      ThreadGroup tgrp = fw_experiment->getThreads();
      if( tgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known threads for this experiment.\n");
        return;
      }
      ThreadGroup::iterator ti = tgrp.begin();
      Thread t1 = *ti;
// Begin: If the user requested a specific thread, use it instead.
// The following bit of code was snag and modified from SS_View_exp.cxx
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
    {
      Thread t = *ti;
      std::string host = t.getHost();
      pid_t pid = t.getProcessId();
      QString pidstr = QString("%1").arg(pid);
      if( threadStr.isEmpty() )
      {
        threadStr = pidstr;
      }
      if( pidstr == threadStr )
      {
// printf("Using %s\n", threadStr.ascii() );
        t1 = *ti;
        if( currentThread )
        {
          delete currentThread;
        }
        currentThread = new Thread(*ti);
        break;
      }
    }
// End: If the user requested a specific thread, use it instead.
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

// printf("Try to match: name.ascii()=%s collectorStr.ascii()=%s\n", name.ascii(), collectorStr.ascii() );
        if( name == collectorStr )
        {
          if( currentCollector )
          {
// printf("delete the currentCollector\n");
            delete currentCollector;
          }
          currentCollector = new Collector(*ci);
// printf("Set a currentCollector!\n");

          nprintf( DEBUG_PANELS) ("GetMetricByFunctionInThread()\n");
          nprintf( DEBUG_PANELS ) ("GetMetricByFunction(%s  %s %s)\n", name.ascii(), currentMetricStr.ascii(), QString("%1").arg(t1.getProcessId()).ascii() );
// printf("GetMetricByFunction(%s  %s %s)\n", name.ascii(), currentMetricStr.ascii(), QString("%1").arg(t1.getProcessId()).ascii() );
          QApplication::setOverrideCursor(QCursor::WaitCursor);
// printf("A: Display the results currentMetricTypeStr=(%s)\n", currentMetricTypeStr.ascii() );
          if( currentMetricTypeStr == "unsigned int" )
          {
            Queries::GetUIntMetricByFunctionInThread(collector, currentMetricStr.ascii(), t1, orig_uint_data);
          } else if( currentMetricTypeStr == "double" )
          {
            Queries::GetMetricByFunctionInThread(collector, currentMetricStr.ascii(), t1, orig_double_data);
          } else
          {
// printf("about to call the new GetUInt64MetricByFunctionInThread() routine.\n");
            Queries::GetUInt64MetricByFunctionInThread(collector, currentMetricStr.ascii(), t1, orig_uint64_data);
// printf("called the new GetUInt64MetricByFunctionInThread() routine.\n");
          }
          qApp->restoreOverrideCursor( );

          // Display the results
          MetricHeaderInfoList metricHeaderInfoList;
          if( currentMetricTypeStr == "double" )
          {
            metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(currentMetricStr.ascii() ), FLOAT_T));
          } else if( currentMetricTypeStr == "int" )
          {
            metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(currentMetricStr.ascii() ), INT_T));
          } else if( currentMetricTypeStr == "unsigned int" )
          {
            metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(currentMetricStr.ascii() ), UNSIGNED_INT_T));
          } else if( currentMetricTypeStr == "uint64_t" )
          {
            metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(currentMetricStr.ascii() ), UINT64_T));
          } else
          {
            metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(currentMetricStr.ascii() ), UNKNOWN_T));
          }


          metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("% of Time"), FLOAT_T));
          metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Function"), CHAR_T));
          if( metricHeaderTypeArray != NULL )
          {
            delete []metricHeaderTypeArray;
          }
          int header_count = metricHeaderInfoList.count();
          metricHeaderTypeArray = new int[header_count];
    
          int i=0;
          for( MetricHeaderInfoList::Iterator pit = metricHeaderInfoList.begin(); pit != metricHeaderInfoList.end(); ++pit )
          { 
            MetricHeaderInfo *mhi = (MetricHeaderInfo *)*pit;
            QString s = mhi->label;
if( collectorStr == "hwc" && pit == metricHeaderInfoList.begin() )
{
  if( currentCollector )
  {
// printf("hwc and currentCollector.\n");
    Metadata cm = currentCollector->getMetadata();
    std::set<Metadata> md =currentCollector->getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++)
    {
      Metadata m = *mi;
// printf("%s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
// printf("%s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
// printf("%s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );
    }
    unsigned int sampling_rate = 0;
    currentCollector->getParameterValue("sampling_rate", sampling_rate);
// printf("sampling_rate=%d\n", sampling_rate);
    std::string PAPIDescriptionStr;
    currentCollector->getParameterValue("event", PAPIDescriptionStr);
// printf("PAPIDescriptionStr=(%s)\n", PAPIDescriptionStr.c_str() );
  if( strlen(PAPIDescriptionStr.c_str()) <= 1 )
  {
    optionalMetricStr = "Unknown metric";
// printf("optionalMetricStr=(%s)\n", optionalMetricStr.ascii() );
  } else
  {
    optionalMetricStr = PAPIDescriptionStr.c_str();
// printf("optionalMetricStr=(%s)\n", optionalMetricStr.ascii() );
  }
  }
  s = optionalMetricStr;
}
if( pit == metricHeaderInfoList.begin() )
{
  s += " for pid:" + threadStr;
}
            splv->addColumn( s );
            metricHeaderTypeArray[i] = mhi->type;
          
            columnList.push_back( s );
            i++;
          }
          break;
        }
      }
  
      // Look up the latest of the preferences and apply them...
      // Which column should we sort?
      bool ok;
      int columnToSort = getPreferenceColumnToSortLineEdit().toInt(&ok);
      if( !ok )
      {
        columnToSort = 0;
      }
  
      // Figure out which way to sort
      bool sortOrder = getPreferenceSortDecending();
      if( sortOrder == TRUE )
      {
        splv->setSortOrder ( Qt::Descending );
        descending_sort = true;
        splv->header()->setSortIndicator( columnToSort, Qt::Descending );
// printf("set sort order Decending.\n");
      } else
      {
        splv->setSortOrder ( Qt::Ascending );
        descending_sort = false;
        splv->header()->setSortIndicator( columnToSort, Qt::Ascending );
// printf("set sort order Ascending.\n");
      }
// printf("columnToSort=%d\n", columnToSort);
      splv->setSorting ( columnToSort, !descending_sort );
      sortColumn(-1);

// printf("B: Display the results currentMetricTypeStr=(%s)\n", currentMetricTypeStr.ascii() );
if( currentMetricTypeStr == "unsigned int" )
{
      if( orig_uint_data.isNull() )
      {
// printf("no data read.\n");
        return;
      }
} else if( currentMetricTypeStr == "double" )
{
      if( orig_double_data.isNull() )
      {
// printf("no data read.\n");
        return;
      }
} else
{
      if( orig_uint64_data.isNull() )
      {
// printf("no data read.\n");
        return;
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

#ifdef SYNTAX
  // Update header information
  headerLabel->setText(QString("Report for collector type \"%1\" with metric selection \"%2\" for thread \"%3\"").arg(collectorStr).arg(currentMetricStr).arg(threadStr) );
#else // SYNTAX
//  printf("Set the header.\n");
#endif // SYNTAX
}

double
StatsPanel::Get_Double_Total_Time()
{
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;

  if( orig_double_data.isNull() )
  {
    return TotalTime;
  }

double_minTime = 0.0;
double_maxTime = 0.0;
  for(std::map<Function, double>::const_iterator
            it = orig_double_data->begin(); it != orig_double_data->end(); ++it)
  {
    TotalTime += it->second;
// Snag the min time...
if( it->second < double_minTime )
{
  double_minTime = it->second;
}
// Snag the max time...
if( it->second > double_maxTime )
{
  double_maxTime = it->second;
}
  }
  return TotalTime;
}


double
StatsPanel::Get_UInt_Total_Time()
{
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;

  if( orig_uint_data.isNull() )
  {
    return TotalTime;
  }

ui_minTime = 0;
ui_maxTime = 0;
  for(std::map<Function, unsigned int>::const_iterator
            it = orig_uint_data->begin(); it != orig_uint_data->end(); ++it)
  {
    TotalTime += it->second;
// Snag the min time...
if( it->second < ui_minTime )
{
  ui_minTime = it->second;
}
// Snag the max time...
if( it->second > ui_maxTime )
{
  ui_maxTime = it->second;
}
  }
  return TotalTime;
}


double
StatsPanel::Get_UInt64_Total_Time()
{
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;

  if( orig_uint64_data.isNull() )
  {
    return TotalTime;
  }

uint64_minTime = 0;
uint64_maxTime = 0;
  for(std::map<Function, uint64_t>::const_iterator
            it = orig_uint64_data->begin(); it != orig_uint64_data->end(); ++it)
  {
    TotalTime += it->second;
// Snag the min time...
if( it->second < ui_minTime )
{
  uint64_minTime = it->second;
}
// Snag the max time...
if( it->second > ui_maxTime )
{
  uint64_maxTime = it->second;
}
  }
  return TotalTime;
}


void
StatsPanel::metricSelected()
{ 

// currentMetricStr = metricMenu->text(val);
// printf("currentMetricStr = (%s)\n", currentMetricStr.ascii() );
 

// printf("collectorStrFromMenu=(%s)\n", collectorStrFromMenu.ascii() );
 int loc = collectorStrFromMenu.find(':');
 collectorStr = collectorStrFromMenu.right(collectorStrFromMenu.length()-(loc+2));
// printf("collectorStr=(%s)\n", collectorStr.ascii() );

  updateStatsPanelData();
}

void
StatsPanel::threadSelected()
{ 

// printf("threadStr = (%s)\n", threadStr.ascii() );
 

// printf("collectorStrFromMenu=(%s)\n", collectorStrFromMenu.ascii() );
 int loc = collectorStrFromMenu.find(':');
 collectorStr = collectorStrFromMenu.right(collectorStrFromMenu.length()-(loc+2));
// printf("collectorStr=(%s)\n", collectorStr.ascii() );

  updateStatsPanelData();
}

void
StatsPanel::contextMenuHighlighted(int val)
{ 
// printf("contextMenuHighlighted val=%d\n", val);
// printf("contextMenuHighlighted: Full collectorStr=(%s)\n", popupMenu->text(val).ascii() );
  QString s = popupMenu->text(val).ascii();

  if( s.find("Show Metric :") != -1 )
  {
    collectorStrFromMenu = popupMenu->text(val).ascii();
  }
}

void
StatsPanel::metricMenuHighlighted(int val)
{ 
// printf("metricMenuHighlighted val=%d\n", val);
// printf("metricMenuHighlighted: Full collectorStr=(%s)\n", popupMenu->text(val).ascii() );

//   currentMetricStr = popupMenu->text(val).ascii();
   currentMetricStr = popupMenu->text(val);
// printf("currentMetricStr=(%s) \n", currentMetricStr.ascii() );
// printf("optionalMetricStr=(%s) \n", optionalMetricStr ? optionalMetricStr.ascii() : "" );
}

void
StatsPanel::threadMenuHighlighted(int val)
{ 
// printf("threadMenuHighlighted val=%d\n", val);
// printf("threadMenuHighlighted: Full collectorStr=(%s)\n", popupMenu->text(val).ascii() );
   threadStr = popupMenu->text(val).ascii();
// printf("threadStr=(%s)\n", threadStr.ascii() );
}

void
StatsPanel::raisePreferencePanel()
{
// printf("StatsPanel::raisePreferencePanel() \n");
  getPanelContainer()->getMainWindow()->filePreferences( statsPanelStackPage, QString(pluginInfo->panel_type) );
}

void 
StatsPanel::clearSourceFile(int expID)
{
  SourceObject *spo = NULL;
  spo = new SourceObject(NULL, NULL, -1, expID, TRUE, NULL);
  if( broadcast((char *)spo, NEAREST_T) == 0 )
  { // No source view up...
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

int
StatsPanel::getLineColor(double value)
{
// printf("getLineColor(%f) descending_sort= %d\n", value, descending_sort);

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
// printf("getLineColor(%d)\n", value);


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
// printf("getLineColor(%l)\n", value);


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
