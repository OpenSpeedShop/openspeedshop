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
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;


#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"

// #include "preference_plugin_info.hxx" // Do not remove

#include "MetricInfo.hxx" // dummy data only...
// This is only hear for the debugging tables....
static char *color_name_table[10] =
  { "red", "orange", "yellow", "skyblue", "green" };


#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;


pcStatsPanel::pcStatsPanel(PanelContainer *pc, const char *n, void *argument) : StatsPanelBase(pc, n, argument)
{
printf("pcStatsPanel() entered\n");
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
  printf("pcStatsPanel::listener() requested.\n");
//  StatsPanelBase::listener(msg);


  PreferencesChangedObject *pco = NULL;

// BUG - BIG TIME KLUDGE.   This should have a message type.
  MessageObject *msgObject = (MessageObject *)msg;
  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
msg->print();
PrintView(msg->expID);
    updateStatsPanelBaseData(msg->fw_expr, msg->expID, msg->experiment_name);
    if( msg->raiseFLAG )
    {
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
//    printf("StatsPanelBase:  The preferences changed.\n");
    pco = (PreferencesChangedObject *)msgObject;
    preferencesChanged();
  }

  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! Create the context senstive menu for the report. */
bool
pcStatsPanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  printf ("pcStatsPanel: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");
  
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
  

  return( FALSE );
}


void
pcStatsPanel::gotoSource()
{
  printf("gotoSource() menu selected.\n");
}

void
pcStatsPanel::itemSelected(QListViewItem *item)
{
  printf("pcStatsPanel::itemSelected(clicked) entered\n");

  if( item )
  {
    printf("  item->depth()=%d\n", item->depth() );

    SPListViewItem *nitem = (SPListViewItem *)item;
    while( nitem->parent() )
    {
      printf("looking for 0x%x\n", nitem->parent() );
      nitem = (SPListViewItem *)nitem->parent();
    } 
  
    
    if( nitem )
    {
      printf("here's the parent! 0x%x\n", nitem);
      printf("  here's the rank of that parent: rank = %s\n",
        nitem->text(1).ascii() );
      matchSelectedItem( atoi( nitem->text(1).ascii() ) );
    }
  }
}


void
pcStatsPanel::matchSelectedItem(int element)
{
  printf ("pcStatsPanel::matchSelectedItem() = %d\n", element );


#ifdef OLDWAY

  int i = 0;
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();
  HighlightObject *hlo = NULL;

  MetricInfo *fi = NULL;
  MetricInfoList::Iterator it = NULL;

  i = 0;
  for( it = collectorData->metricInfoList.begin();
       it != collectorData->metricInfoList.end();
       it++ )
  {
    fi = (MetricInfo *)*it;
    for( int line=fi->start; line <= fi->end; line++)
    {
      if( i >= 5 )
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[4], "exclusive time");
      } else
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[i], "exclusive time");
      }
// fprintf(stderr, "  pushback hlo: line=%d in color (%s)\n", line, hlo->color);
      highlightList->push_back(hlo);
    }
    i++;
  }


  i = 0;
  for( it = collectorData->metricInfoList.begin();
       it != collectorData->metricInfoList.end();
       it++ )
  {
     fi = (MetricInfo *)*it;
     if( i == element )
     {
       break;
     }
     i++;
  }

  dprintf ("%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  
  char msg[1024];
  sprintf(msg, "%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  

  SourceObject *spo = new SourceObject(fi->functionName, fi->fileName, fi->function_line_number, TRUE, highlightList);



  if( broadcast((char *)spo, NEAREST_T) == 0 )
  { // No source view up...
    char *panel_type = "Source Panel";
//Find the nearest toplevel and start placement from there...
    Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type, NULL, (void *)groupID);
    if( p != NULL ) 
    {
      p->listener((void *)spo);
    }
  }
#endif // OLDWAY
}

#include "SS_Input_Manager.hxx"
void
pcStatsPanel::PrintView(int expID)
{

printf("pcStatsPanel::PrintView(%d) entered\n", expID );
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
  
    for(std::map<Function, double>::const_iterator
          item = data->begin(); item != data->end(); ++item)
    {
      printf("%20f %20s \n", item->second,  item->first.getName().c_str() );
    }
  }

printf("pcStatsPanel::PrintView(%d) finished\n", expID );
}


