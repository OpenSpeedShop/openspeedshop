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


#include "ComparePanel.hxx"
#include "CompareClass.hxx"
#include "PreferencesChangedObject.hxx"
#include "UpdateObject.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "ArgumentObject.hxx"

#include "preference_plugin_info.hxx"

#include <qtextedit.h>  // For QTextEdit in example below...
ComparePanel::ComparePanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  expID = ao->int_data;
  openComparePaneFLAG = FALSE;

  setCaption("ComparePanel");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  QSplitter *compareSplitter = new QSplitter(getBaseWidgetFrame(), "compareSplitter");
  compareSplitter->setOrientation(QSplitter::Vertical);


  mcc = new CompareClass( this, compareSplitter);

  mcc1 = new CompareClass( this, compareSplitter, "CompareClass", FALSE, 0, ao->int_data );

  frameLayout->addWidget(compareSplitter);
  mcc->show();
  mcc1->hide();
  mcc->expID = ao->int_data;
  groupID = mcc->expID;
  getBaseWidgetFrame()->setCaption("ComparePanelBaseWidget");

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);

  preferencesChanged();
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
ComparePanel::~ComparePanel()
{
  // Delete anything you new'd from the constructor.
// printf("ComparePanel::destructor called.\n");
  delete mcc;
}

void
ComparePanel::languageChange()
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
ComparePanel::menu(QPopupMenu* contextMenu)
{
  Panel::menu(contextMenu);

  QAction *qaction = new QAction( this,  "openComparePane");
  qaction->addTo( contextMenu );
  if( openComparePaneFLAG )
  {
    qaction->setText( tr("Hide Compare Pane") );
  } else 
  {
    qaction->setText( tr("Show Compare Pane") );
  }
  connect( qaction, SIGNAL( activated() ), this, SLOT( openComparePane() ) );
  qaction->setStatusTip( tr("Open pane to set the compare properties.") );

  if( mcc1->hasMouse() )
  {
    return( mcc1->menu(contextMenu) );
  } else
  {
    return( mcc->menu(contextMenu) );
  }

  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
ComparePanel::save()
{
  dprintf("ComparePanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
ComparePanel::saveAs()
{
  dprintf("ComparePanel::saveAs() requested.\n");
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
ComparePanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

  MessageObject *msgObject = (MessageObject *)msg;
//  if( msgObject->msgType == getName() )
// printf("ComparePanel::listener() getName=%s\n", getName());
// msgObject->print();
  if( msgObject->msgType == getName() && recycleFLAG == TRUE )
  {
    nprintf(DEBUG_MESSAGES) ("ComparePanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }

  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
    nprintf(DEBUG_MESSAGES) ("ComparePanel::listener() UpdateExperimentDataObject!\n");

#ifdef PULL
    mcc->expID = msg->expID;
    mcc->updatePanel();
printf("mcc1->expID=%d msg->expID=%d\n", mcc1->expID, msg->expID );
if( mcc1->expID != msg->expID )
{
  mcc1->expID = msg->expID;
  mcc1->addNewCSet();
}
#endif // PULL

    if( msg->raiseFLAG )
    {
    if( msg->raiseFLAG )
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
    nprintf(DEBUG_MESSAGES) ("ComparePanel::listener() PreferencesChangedObject!\n");
    pco = (PreferencesChangedObject *)msgObject;
// Currently ignored.
    preferencesChanged();
  } else if( msgObject->msgType == "SaveAsObject" )
  {
//    SaveAsObject *sao = (SaveAsObject *)msg;
  dprintf("ComparePanel!!!!! Save as!\n");
//    if( !sao )
//    {
//      return 0;  // 0 means, did not act on message.
//    }
//    exportData(sao->f, sao->ts);
// Currently you're not passing the file descriptor down... you need to.sao->f, sao->ts);
//    f = sao->f;
  dprintf("Attempt to call (unexistent) exportData() routine\n");
//    exportData();
  }


  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
ComparePanel::broadcast(char *msg)
{
  dprintf("ComparePanel::broadcast() requested.\n");
  return 0;
}

void
ComparePanel::raisePreferencePanel()
{
// printf("ComparePanel::raisePreferencePanel() \n");
  getPanelContainer()->getMainWindow()->filePreferences( manageProcessesPanelStackPage, QString(pluginInfo->panel_type) );
}

void
ComparePanel::openComparePane()
{
  if( openComparePaneFLAG )
  {
    openComparePaneFLAG = FALSE;
    mcc1->hide();
  } else
  {
    openComparePaneFLAG = TRUE;
    mcc1->show();
  }
}


void
ComparePanel::preferencesChanged()
{
  // Place holder for eventual preferenceChange callback.
}
