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


#include "pcStatsPanel.hxx"   // Change this to your new class header file name
#include "StatsPanelBase.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

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

// #include "MetricInfo.hxx" // dummy data only...
// This is only hear for the debugging tables....
static char *color_name_table[10] =
  { "red", "orange", "yellow", "skyblue", "green" };


#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;


/*! Create a pc Sampling Specific Stats Panel.   This panel is derived
    from the StatsPanelBase class.  
*/
pcStatsPanel::pcStatsPanel(PanelContainer *pc, const char *n, void *argument) : StatsPanelBase(pc, n, argument)
{
//  printf("pcStatsPanel() entered\n");
  setCaption("pcStatsPanel");
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
pcStatsPanel::~pcStatsPanel()
{
  // Delete anything you new'd from the constructor.
}

void
pcStatsPanel::languageChange()
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
int 
pcStatsPanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

  MessageObject *msgObject = (MessageObject *)msg;
  if( msgObject->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("pcStatsPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }

  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
    nprintf(DEBUG_MESSAGES) ("pcStatsPanel::listener() UpdateExperimentDataObject!\n");
    updateStatsPanelBaseData(msg->fw_expr, msg->expID, msg->experiment_name);
    if( msg->raiseFLAG )
    {
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
    nprintf(DEBUG_MESSAGES) ("pcStatsPanel::listener() PreferencesChangedObject!\n");
    pco = (PreferencesChangedObject *)msgObject;
    preferencesChanged();
  }

  return 0;  // 0 means, did not want this message and did not act on anything.
}

bool
pcStatsPanel::menu( QPopupMenu* contextMenu)
{
//  printf("pcStatsPanel::menu() entered.\n");


  contextMenu->insertSeparator();

  contextMenu->insertItem("Compare...", this, SLOT(compareSelected()) );

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
    if( lv->columnWidth(id) )
    {
      columnsMenu->setItemChecked(id, TRUE);
    } else
    {
      columnsMenu->setItemChecked(id, FALSE);
    }
    id++;
  }

//  contextMenu->insertItem("Export Report Data...", this, NULL, NULL);
  contextMenu->insertItem("Export Report Data...", this, SLOT(exportData()));

  if( lv->selectedItem() )
  {
    contextMenu->insertItem("Tell Me MORE about %d!!!", this, SLOT(details()), CTRL+Key_1 );
    contextMenu->insertItem("Go to source location...", this, SLOT(gotoSource()), CTRL+Key_1 );
    return( TRUE );
  }

  return( TRUE );
}

/*! Create the context senstive menu for the report. */
bool
pcStatsPanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
printf("pcStatsPanel::createPopupMenu(contextMenu=0x%x) entered\n", contextMenu);
#ifdef OLDWAY
  QPopupMenu *panelMenu = new QPopupMenu(this);
  panelMenu->setCaption("Panel Menu");
  contextMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_C);
  panelMenu->insertSeparator();
  menu(panelMenu);

  if( lv->selectedItem() )
  {
  //  contextMenu->insertItem("Tell Me MORE about %d!!!", this, SLOT(details()), CTRL+Key_1 );
    contextMenu->insertItem("Go to source location...", this, SLOT(gotoSource()), CTRL+Key_1 );
    return( TRUE );
  }
#else // OLDWAY
  menu(contextMenu);
  return( TRUE );
#endif // OLDWAY
  
  return( FALSE );
}

/*! Go to source menu item was selected. */
void
pcStatsPanel::details()
{
  printf("details() menu selected.\n");
}

void
pcStatsPanel::exportData()
{
  printf("exportData() menu selected.\n");
  QPtrList<QListViewItem> lst;
  QListViewItemIterator it( lv );
  int cols =  lv->columns();
  int i=0;
  QString fileName = "pcStatsPanel.txt";
  QString dirName = QString::null;

  QFileDialog *fd = new QFileDialog(this, "save_pcStatsPanelData:", TRUE );
  fd->setCaption( QFileDialog::tr("Save pcStatsPanel data:") );
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
  
    if( !fileName.isEmpty() )
    {
      QFile f(fileName);
      f.open(IO_WriteOnly );

      // Write out the header info
      QString line = QString("  ");
      for(i=0;i<cols;i++)
      {
        for(i=0;i<cols;i++)
        {
          line += QString(lv->columnText(i))+" ";
        }
        line += QString("\n");
      }
      f.writeBlock( line, qstrlen(line) );

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
        f.writeBlock( line, qstrlen(line) );
        ++it;
      }
      f.close();
    }
  }
}

/*! Go to source menu item was selected. */
void
pcStatsPanel::gotoSource()
{
  printf("gotoSource() menu selected.\n");
}

void
pcStatsPanel::itemSelected(QListViewItem *item)
{
//  printf("pcStatsPanel::itemSelected(clicked) entered\n");

  if( item )
  {
//    printf("  item->depth()=%d\n", item->depth() );

    SPListViewItem *nitem = (SPListViewItem *)item;
    while( nitem->parent() )
    {
//      printf("looking for 0x%x\n", nitem->parent() );
      nitem = (SPListViewItem *)nitem->parent();
    } 
  
    
    if( nitem )
    {
//      printf("here's the parent! 0x%x\n", nitem);
//      printf("  here's the rank of that parent: function name = %s\n",
//        nitem->text(1).ascii() );
//      matchSelectedItem( atoi( nitem->text(1).ascii() ) );
      matchSelectedItem( std::string(nitem->text(3).ascii()) );
    }
  }
}


void
pcStatsPanel::matchSelectedItem(std::string selected_function )
{
  bool foundFLAG = FALSE;
// printf ("pcStatsPanel::matchSelectedItem() = %s\n", selected_function.c_str() );

  try
  {
    std::map<Function, double>::const_iterator it = orig_data->begin();
    std::set<Statement> definitions = it->first.getDefinitions();
    for( ; it != orig_data->end(); ++it)
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
// fprintf(stderr, "No function definition for this entry.   Unable to position source.\n");
          QMessageBox::information(this, "Open|SpeedShop", "No function definition for this entry.\nUnable to position source. (No symbols.)\n", "Ok");
          return;
        }
      }
    }

    SourceObject *spo = NULL;
    if( definitions.size() > 0 )
    {
      std::set<Statement>::const_iterator di = definitions.begin();
      spo = new SourceObject(it->first.getName().c_str(), di->getPath(), di->getLine()-1, TRUE, NULL);
    }



    if( spo )
    {
      if( broadcast((char *)spo, NEAREST_T) == 0 )
      { // No source view up...
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
pcStatsPanel::updateStatsPanelBaseData(void *expr, int expID, QString experiment_name)
{
  nprintf( DEBUG_PANELS) ("pcStatsPanel::updateStatsPanelBaseData() entered.\n");

  StatsPanelBase::updateStatsPanelBaseData(expr, expID, experiment_name);

  
  SPListViewItem *lvi;
  columnList.clear();

  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", expID);
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
    CollectorGroup cgrp = fw_experiment->getCollectors();
    if( cgrp.size() == 0 )
    {
      fprintf(stderr, "There are no known collectors for this experiment.\n");
      return;
    }
    CollectorGroup::iterator ci = cgrp.begin();
    Collector c1 = *ci;

    nprintf( DEBUG_PANELS) ("GetMetricByFunctionInThread()\n");
    Queries::GetMetricByFunctionInThread(c1, "time", t1, orig_data);

    // Display the results
    MetricHeaderInfoList metricHeaderInfoList;
    metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("CPU Time (Seconds)"), FLOAT_T));
metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("% of Time"), FLOAT_T));
metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Cumulative %"), FLOAT_T));
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
      lv->addColumn( s );
      metricHeaderTypeArray[i] = mhi->type;
    
      columnList.push_back( s );
      i++;
    }

    // Look up the latest of the preferences and apply them...
    // Which column should we sort?
    bool ok;
    int columnToSort = getPreferenceColumnToSortLineEdit().toInt(&ok);
    if( !ok )
    {
      columnToSort = 0;
    }
    lv->setSorting ( columnToSort, FALSE );

    // Figure out which way to sort
    bool sortOrder = getPreferenceSortDecending();
    if( sortOrder == TRUE )
    {
      lv->setSortOrder ( Qt::Descending );
    } else
    {
      lv->setSortOrder ( Qt::Ascending );
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


    nprintf( DEBUG_PANELS) ("Put the data out...\n");

    double TotalTime = Get_Total_Time();

    char cputimestr[50];
    char a_percent_str[50];
    a_percent_str[0] = '\0';
    char c_percent_str[50];
    // convert time to %
    double percent_factor = 100.0 / TotalTime;
    double a_percent = 0; // accumulated percent
    double c_percent = 0.0;

    for(std::map<Function, double>::const_iterator
            it = orig_data->begin(); it != orig_data->end(); ++it)
    {
      c_percent = it->second*percent_factor;  // current item's percent of total time
      sprintf(cputimestr, "%f", it->second);
      sprintf(a_percent_str, "%f", c_percent);
      sprintf(c_percent_str, "%f", a_percent);
      lvi =  new SPListViewItem( this, lv, cputimestr,  a_percent_str, c_percent_str, it->first.getName().c_str() );

#ifdef OLDWAY
      if(numberItemsToDisplay >= 0 )
      {
        numberItemsToDisplay--;
        if( numberItemsToDisplay == 0)
        {
          // That's all the user requested...
          break;  
        }
      }
#endif // OLDWAY
    }
    
    lv->sort();


    sortCalledRecalculateCumulative(0);

  }
}

double
pcStatsPanel::Get_Total_Time()
{
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;
  for(std::map<Function, double>::const_iterator
            it = orig_data->begin(); it != orig_data->end(); ++it)
  {
    TotalTime += it->second;
  }
  return TotalTime;
}


void
pcStatsPanel::sortCalledRecalculateCumulative(int val)
{
  // Now calculate the cumulative %
  double a_percent = 0.0;
  QPtrList<QListViewItem> lst;
  QListViewItemIterator it( lv );
  while( it.current() )
  {
    QListViewItem *item = *it;
    QString val_str = item->text(1);
    double val = val_str.toDouble();
    a_percent += val;
    item->setText( 2, QString("%1").arg(a_percent) );
    ++it;
  }
}
