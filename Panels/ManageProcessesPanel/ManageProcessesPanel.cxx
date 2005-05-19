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


#include "ManageProcessesPanel.hxx"   // Change this to your new class header file name
#include "ManageCollectorsClass.hxx"   // Change this to your new class header file name
#include "PreferencesChangedObject.hxx"
#include "UpdateObject.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qtextedit.h>  // For QTextEdit in example below...
ManageProcessesPanel::ManageProcessesPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  setCaption("ManageProcessesPanel");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );


  mcc = new ManageCollectorsClass( pc, getBaseWidgetFrame() );
  frameLayout->addWidget(mcc);
  mcc->show();
  mcc->expID = (int)argument;
  groupID = mcc->expID;

  getBaseWidgetFrame()->setCaption("ManageProcessesPanelBaseWidget");
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
ManageProcessesPanel::~ManageProcessesPanel()
{
  // Delete anything you new'd from the constructor.
}

void
ManageProcessesPanel::languageChange()
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
ManageProcessesPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("ManageProcessesPanel::menu() requested.\n");

  return( mcc->menu(contextMenu) );

  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
ManageProcessesPanel::save()
{
  dprintf("ManageProcessesPanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
ManageProcessesPanel::saveAs()
{
  dprintf("ManageProcessesPanel::saveAs() requested.\n");
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
ManageProcessesPanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

  MessageObject *msgObject = (MessageObject *)msg;
  if( msgObject->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("ManageProcessesPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }

  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
    nprintf(DEBUG_MESSAGES) ("ManageProcessesPanel::listener() UpdateExperimentDataObject!\n");
  dprintf("ManageProcessesPanel::listener() UpdateExperimentDataObject!\n");

//  expID = msg->expID;
    mcc->expID = msg->expID;
    mcc->updateAttachedList();

    if( msg->raiseFLAG )
    {
    if( msg->raiseFLAG )
      getPanelContainer()->raisePanel((Panel *)this);
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
    nprintf(DEBUG_MESSAGES) ("ManageProcessesPanel::listener() PreferencesChangedObject!\n");
  dprintf ("ManageProcessesPanel::listener() PreferencesChangedObject!\n");
  dprintf ("HANDLE THIS!!!!\n");
    pco = (PreferencesChangedObject *)msgObject;
//    preferencesChanged();
  } else if( msgObject->msgType == "SaveAsObject" )
  {
//    SaveAsObject *sao = (SaveAsObject *)msg;
  dprintf("ManageProcessesPanel!!!!! Save as!\n");
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
ManageProcessesPanel::broadcast(char *msg)
{
  dprintf("ManageProcessesPanel::broadcast() requested.\n");
  return 0;
}
