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

#include <qvaluelist.h>
#include <qmessagebox.h>
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;


#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
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
// StatsPanel::StatsPanel(PanelContainer *pc, const char *n, void *argument) : StatsPanelBase(pc, n, argument)
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
// printf("StatsPanel() entered\n");
  setCaption("StatsPanel");


  f = NULL;
  metricMenu = NULL;
  metricStr = QString::null;
  metricHeaderTypeArray = NULL;
  collectorStr = QString::null;
  groupID = (int)argument;
  expID = -1;
  ascending_sort = false;

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
  if( msgObject->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }

  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() UpdateExperimentDataObject!\n");

  if( expID == -1 || expID != msg->expID )
  {
    // Initialize the collector list....
    clo = new CollectorListObject(msg->expID);

    // Now that you have the list initialize the default metricStr 
    // and default collector string (if it's the first time..)

    if( collectorStr.isEmpty() && metricStr.isEmpty() )
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
          metricStr = cpe->name;
// printf("Initialize collectorStr=(%s) metricStr=(%s)\n", collectorStr.ascii(), metricStr.ascii() );
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
    if( getPanelContainer()->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() )
    {
      chartFLAG = TRUE;
      cf->show();
    } else
    {
      chartFLAG = FALSE;
      cf->hide();
    }
    splv->show();
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
#ifdef TRIED   // FIX - Add context sensitive help to these entries.
    QAction *qaction = new QAction( this,  "columnsMenuActions");
    qaction->addTo( columnsMenu );
    qaction->setText( s );
//    connect( qaction, SIGNAL( activated(int) ), this, SLOT( doOption(int) ) );
    qaction->setStatusTip( tr("Select which columns to display.") );
#endif  // TRIED
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
StatsPanel::gotoSource()
{
// printf("gotoSource() menu selected.\n");
  QListViewItem *lvi = splv->selectedItem();

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
StatsPanel::itemSelected(QListViewItem *item)
{
// printf("StatsPanel::itemSelected(clicked) entered\n");

  if( item )
  {
// printf("  item->depth()=%d\n", item->depth() );

    SPListViewItem *nitem = (SPListViewItem *)item;
    while( nitem->parent() )
    {
// printf("looking for 0x%x\n", nitem->parent() );
      nitem = (SPListViewItem *)nitem->parent();
    } 
  
    
    if( nitem )
    {
// printf("here's the parent! 0x%x\n", nitem);
//      printf("  here's the rank of that parent: function name = %s\n",
//        nitem->text(1).ascii() );
//      matchSelectedItem( atoi( nitem->text(1).ascii() ) );

// printf("YOU NEED TO LOOP THROUGH AND FIND THE FUNCTION FIELD!!!\n");
      matchSelectedItem( std::string(nitem->text(2).ascii()) );
    }
  }
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



void
StatsPanel::matchSelectedItem(std::string selected_function )
{
  bool foundFLAG = FALSE;
// printf ("StatsPanel::matchSelectedItem() = %s\n", selected_function.c_str() );

  try
  {
    std::vector<Function_double_pair>::const_iterator it = sorted_items.begin();
    std::set<Statement> definitions = it->first.getDefinitions();
    for( ; it != sorted_items.end(); ++it)
    {
// printf("%s %f\n", it->first.getName().c_str(), it->second );
      if( selected_function == it->first.getName()  )
      {
// printf("FOUND IT!\n");
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
          return;
        }
      }
    }

    SourceObject *spo = NULL;
    if( definitions.size() > 0 )
    {
      std::set<Statement>::const_iterator di = definitions.begin();
      spo = new SourceObject(it->first.getName().c_str(), di->getPath(), di->getLine()-1, expID, TRUE, NULL);
    }



    if( spo )
    {
      if( broadcast((char *)spo, NEAREST_T) == 0 )
      { // No source view up...
// printf("place the source panel.\n");
        char *panel_type = "Source Panel";
  //Find the nearest toplevel and start placement from there...
        PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(getPanelContainer());
        Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, (void *)groupID);
        if( p != NULL ) 
        {
          p->listener((void *)spo);
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
StatsPanel::updateStatsPanelData()
{
  nprintf( DEBUG_PANELS) ("StatsPanel::updateStatsPanelData() entered.\n");

  int index = 0;
  int count = 0;
  int values[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  char *color_names[] = { "red", "green", "cyan", "gray", "darkGreen", "darkCyan", "magenta", "blue", "yellow", "black", "darkRed", "darkBlue", "darkMagenta", "darkYellow", "darkGray", "lightGray" };
  char *strings[] = { "", "", "", "", "", "", "", "", "", "" };

  
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
          nprintf( DEBUG_PANELS) ("GetMetricByFunctionInThread()\n");
// printf("GetMetricByFunction(%s  %s %s)\n", name.ascii(), metricStr.ascii(), QString("%1").arg(t1.getProcessId()).ascii() );
          Queries::GetMetricByFunctionInThread(collector, metricStr.ascii(), t1, orig_data);

          if( !orig_data.isNull() )
          {
           sorted_items.clear();
           // sort by the time metric
            // Now we can sort the data.
            for(std::map<Function, double>::const_iterator
                item = orig_data->begin(); item != orig_data->end(); ++item)
            {
              sorted_items.push_back( *item );
            }
          
            if (ascending_sort) {
              std::sort(sorted_items.begin(), sorted_items.end(), sort_ascending<Function_double_pair>());
            } else {
              std::sort(sorted_items.begin(), sorted_items.end(), sort_descending<Function_double_pair>());
            }
          
          }
  
          // Display the results
          MetricHeaderInfoList metricHeaderInfoList;
          metricHeaderInfoList.push_back(new MetricHeaderInfo(QString(metricStr.ascii() ), FLOAT_T));
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
      splv->setSorting ( columnToSort, FALSE );
  
      // Figure out which way to sort
      bool sortOrder = getPreferenceSortDecending();
      if( sortOrder == TRUE )
      {
        splv->setSortOrder ( Qt::Descending );
        ascending_sort = false;
      } else
      {
        splv->setSortOrder ( Qt::Ascending );
        ascending_sort = false;
      }
  
      // How many rows should we display?
      int numberItemsToDisplay = -1;
      if( !getPreferenceTopNLineEdit().isEmpty() )
      {
        numberItemsToDisplay = getPreferenceTopNLineEdit().toInt(&ok);
        if( !ok )
        {
          numberItemsToDisplay = 5; // Default to top5.
        }
      }


if( orig_data.isNull() )
{
  printf("no data read.\n");
  return;
}

      nprintf( DEBUG_PANELS) ("Put the data out...\n");

      double TotalTime = Get_Total_Time();

      char cputimestr[50];
      char a_percent_str[50];
      a_percent_str[0] = '\0';
      // convert time to %
      double percent_factor = 100.0 / TotalTime;
      double a_percent = 0; // accumulated percent
      double c_percent = 0; // accumulated percent
  
      for(std::vector<Function_double_pair>::const_iterator
              it = sorted_items.begin(); it != sorted_items.end(); ++it)
      {
        c_percent = it->second*percent_factor;  // current item's percent of total time
        sprintf(cputimestr, "%f", it->second);
        sprintf(a_percent_str, "%f", c_percent);
        lvi =  new SPListViewItem( this, splv, cputimestr,  a_percent_str, it->first.getName().c_str() );

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
    
//      splv->sort();

        // Set the values for the top 5 pie chart elements...
      QListViewItemIterator it( splv );
      while( it.current() )
      {
        QListViewItem *item = *it;
        if( index < 5 )
        {
          values[index] = (int)item->text(1).toFloat();
// printf("values[%d] = (%d)\n", index, values[index] );
          strings[index] = (char *)item->text(1).ascii();
          count = index+1;
        }
        index++;
      
        ++it;
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

// Begin: Try putting out the graph here...
// Loop through splv and print out the top 5....
{
  int total_percent = 0;
int i = 0;
  for(i =0;i<count;i++)
  {
    total_percent += values[i];
  }
if( total_percent < 100 )
{
  values[i] = 100-total_percent;
  strings[i] = "other";
  count++;
}
  cf->setValues(values, color_names, strings, count+1);
}
// End: Try putting out the graph here...

#ifdef SYNTAX
  // Update header information
  headerLabel->setText(QString("Report for collector type \"%1\" with metric selection \"%2\" for thread \"%3\"").arg(collectorStr).arg(metricStr).arg(threadStr) );
#else // SYNTAX
//  printf("Set the header.\n");
#endif // SYNTAX
}

double
StatsPanel::Get_Total_Time()
{
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;

  if( orig_data.isNull() )
  {
    return TotalTime;
  }

  for(std::map<Function, double>::const_iterator
            it = orig_data->begin(); it != orig_data->end(); ++it)
  {
    TotalTime += it->second;
  }
  return TotalTime;
}


void
StatsPanel::metricSelected()
{ 

// metricStr = metricMenu->text(val);
// printf("metricStr = (%s)\n", metricStr.ascii() );
 

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

   metricStr = popupMenu->text(val).ascii();
}

void
StatsPanel::threadMenuHighlighted(int val)
{ 
// printf("threadMenuHighlighted val=%d\n", val);
// printf("threadMenuHighlighted: Full collectorStr=(%s)\n", popupMenu->text(val).ascii() );
   threadStr = popupMenu->text(val).ascii();
// printf("threadStr=(%s)\n", threadStr.ascii() );
}
