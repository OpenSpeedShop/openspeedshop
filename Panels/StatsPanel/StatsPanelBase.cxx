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


#include "StatsPanelBase.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove

#include <qvaluelist.h>
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;


#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"
#include "preference_plugin_info.hxx" // Do not remove


#include "MetricInfo.hxx" // dummy data only...
// This is only hear for the debugging tables....
static char *color_name_table[10] =
  { "red", "orange", "yellow", "skyblue", "green" };


// #include "ToolAPI.hxx"
#include "SS_Input_Manager.hxx"
using namespace OpenSpeedShop::Framework;



StatsPanelBase::StatsPanelBase(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
printf("StatsPanelBase() entered\n");

  setCaption("StatsPanelBase");

  groupID = (int)argument;

  metricHeaderTypeArray = NULL;

  bool ok;
  numberItemsToRead = getPreferenceTopNLineEdit().toInt(&ok);
  if( !ok )
  {
printf("Invalid \"number of items\" in preferences.   Resetting to default.\n");
    numberItemsToRead = 5;
  }

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  lv = NULL;
  
  getBaseWidgetFrame()->setCaption("StatsPanelBaseBaseWidget");

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), groupID);
  setName(name_buffer);
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanelBase::~StatsPanelBase()
{
  // Delete anything you new'd from the constructor.
}

void
StatsPanelBase::updateStatsPanelBaseData(void *expr, int expID, QString experiment_name)
{
   // Read the new data, destroy the old data, and update the StatsPanelBase with
   // the new data.


  dprintf("updateStatsPanelBaseData() enterd.\n");
// printf("updateStatsPanelBaseData(0x%x %d, %s) enterd.\n", expr, expID, experiment_name.ascii() );

  if( lv != NULL )
  {
    delete lv;
    lv = NULL;
  }

  if( lv == NULL )
  {
    lv = new SPListView( this, getBaseWidgetFrame(), getName(), 0 );
 
    connect( lv, SIGNAL(clicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

    lv->setAllColumnsShowFocus(TRUE);

    // If there are subitems, then indicate with root decorations.
    lv->setRootIsDecorated(TRUE);

    // If there should be sort indicators in the header, show them here.
    lv->setShowSortIndicator(TRUE);

  }

  // Sort in decending order
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


  lv->clear();

  SPListViewItem *lvi;
  columnList.clear();
printf("This should be overloaded in pcStatsPanel....\n");

ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
if( eo && eo->FW() )
{
  Experiment *fw_experiment = eo->FW();
// Evaluate the collector's time metric for all functions in the thread
SmartPtr<std::map<Function, double> > data;
ThreadGroup tgrp = fw_experiment->getThreads();
ThreadGroup::iterator ti = tgrp.begin();
Thread t1 = *ti;
CollectorGroup cgrp = fw_experiment->getCollectors();
CollectorGroup::iterator ci = cgrp.begin();
Collector c1 = *ci;

Queries::GetMetricByFunctionInThread(c1, "time", t1, data);

// Display the results
  MetricHeaderInfoList metricHeaderInfoList;
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Time"), FLOAT_T));
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

  
  char timestr[50];
  for(std::map<Function, double>::const_iterator
        item = data->begin(); item != data->end(); ++item)
  {
    sprintf(timestr, "%f", item->second);
    lvi =  new SPListViewItem( this, lv, timestr,  item->first.getName() );
  }
}

  frameLayout->addWidget(lv);

  lv->show();
}
void
StatsPanelBase::languageChange()
{
  // Set language specific information here.
}

/*! This calls the user 'menu()' function
    if the user provides one.   The user can attach any specific panel
    menus to the passed argument and they will be displayed on a right
    mouse down in the panel.
    /param  contextMenu is the QPopupMenu * that use menus can be attached.
 */
bool
StatsPanelBase::menu(QPopupMenu* contextMenu)
{
printf("StatsPanelBase::menu() requested.\n");
  contextMenu->insertSeparator();

//  contextMenu->insertItem("Number visible entries...", this, SLOT(setNumberVisibleEntries()), CTRL+Key_1, 0, -1);
  contextMenu->insertItem("Number visible entries...", this, SLOT(setNumberVisibleEntries()));

  contextMenu->insertSeparator();

//  contextMenu->insertItem("Compare...", this, SLOT(compareSelected()), CTRL+Key_1, 0, -1);
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
  contextMenu->insertItem("Export Report Data...");

  return( TRUE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
StatsPanelBase::save()
{
  dprintf("StatsPanelBase::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
StatsPanelBase::saveAs()
{
  dprintf("StatsPanelBase::saveAs() requested.\n");
}

void
StatsPanelBase::preferencesChanged()
{ 
//  printf("StatsPanelBase::preferencesChanged\n");

  bool thereWasAChangeICareAbout = FALSE;

  SortOrder old_sortOrder = lv->sortOrder();
  bool new_sortOrder = getPreferenceSortDecending();

  if( old_sortOrder != new_sortOrder )
  {
    if( new_sortOrder == TRUE )
    {
      lv->setSortOrder ( Qt::Descending );
    } else
    {
      lv->setSortOrder ( Qt::Ascending );
    }
    thereWasAChangeICareAbout = TRUE;
  }


  bool ok;
  int new_numberItemsToRead = getPreferenceTopNLineEdit().toInt(&ok);
  if( ok )
  {
    if( new_numberItemsToRead != numberItemsToRead )
    {
      numberItemsToRead = new_numberItemsToRead;
      thereWasAChangeICareAbout = TRUE;
    }
  }

  int new_columnToSort = getPreferenceColumnToSortLineEdit().toInt(&ok);
  if( ok )
  {
    int old_columnToSort = lv->sortColumn();
    if( old_columnToSort != new_columnToSort )
    {
      thereWasAChangeICareAbout = TRUE;
    }
  }

  if( thereWasAChangeICareAbout )
  {
// printf("  thereWasAChangeICareAbout!!!!\n");
    updateStatsPanelBaseData();
  }
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
StatsPanelBase::listener(void *msg)
{
  dprintf("StatsPanelBase::listener() requested.\n");

  return 0;  // 0 means, did not want this message and did not act on anything.
}

/*! Create the context senstive menu for the report. */
bool
StatsPanelBase::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  dprintf ("StatsPanelBase: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");
  return( FALSE );
}

void
StatsPanelBase::compareSelected()
{
  printf("compareSelected()\n");
}

void
StatsPanelBase::setNumberVisibleEntries()
{
  printf("setNumberVisibleEntries()\n");
{
  bool ok;
  QString s = QString("%1").arg(numberItemsToRead);
  QString text = QInputDialog::getText(
          "Visible Lines", "Enter number visible lines:", QLineEdit::Normal,
          s, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    numberItemsToRead = atoi(text.ascii());
    dprintf ("numberItemsToRead=%d\n", numberItemsToRead);
    updateStatsPanelBaseData();
  } else
  {
    // user entered nothing or pressed Cancel
  }
}
}

static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanelBase::doOption(int id)
{
  dprintf("doOption() id=%d\n", id);

  if( lv->columnWidth(id) )
  {
    cwidth = lv->columnWidth(id);
    lv->hideColumn(id);
  } else
  {
    lv->setColumnWidth(id, cwidth);
  }
}


/*! This is just a utility routine to truncate_name long names. */
char *
StatsPanelBase::truncateCharString(char *str, int length)
{
  char *newstr = NULL;
//  newstr = new char( length );
  newstr = (char *)calloc( length, sizeof(char)+3+1 );

  if( length > strlen(str) )
  {
    strcpy(newstr, str);
  } else
  {
    strcpy(newstr, "...");
    int extra = strlen(str)-length;
    strcat(newstr, str+extra);
    strcat(newstr, "");
  }

  return newstr;
}
