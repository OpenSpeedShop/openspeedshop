////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
//
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

// Debug Flag
//#define DEBUG_CSP 1
// 

#include "CustomizeStatsPanel.hxx"
#include "CustomizeClass.hxx"
#include "CompareProcessesDialog.hxx"
#include "PreferencesChangedObject.hxx"
#include "UpdateObject.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "ArgumentObject.hxx"

#include <qtextedit.h>  // For QTextEdit in example below...
CustomizeStatsPanel::CustomizeStatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  expID = ao->int_data;

#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::constructor called, expID=%d\n", expID);
#endif

  setCaption("CustomizeStatsPanel");

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );


  mcc1 = new CustomizeClass( this, getBaseWidgetFrame(), "CustomizeClass", FALSE, 0, ao->int_data, ao->qstring_data );

#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::constructor called, mcc1=%x\n", mcc1);
#endif

  frameLayout->addWidget(mcc1);
  mcc1->expID = ao->int_data;
  groupID = mcc1->expID;
  getBaseWidgetFrame()->setCaption("CustomizeStatsPanelBaseWidget");

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);

  preferencesChanged();
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
CustomizeStatsPanel::~CustomizeStatsPanel()
{
  // Delete anything you new'd from the constructor.
#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::destructor called.\n");
#endif
    // jeg 10-12-07 had to comment this out as it causes
    // an abort on exit that I'm not able to figure out
    // Al had some comments about deleting mcc1->dialog
    // that suggest this has been a problem in the past.
    // Look for TERMINATE in the source to find these comments
//  delete mcc1;
}

void
CustomizeStatsPanel::languageChange()
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
CustomizeStatsPanel::menu(QPopupMenu* contextMenu)
{
  Panel::menu(contextMenu);

  return( mcc1->menu(contextMenu) );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
CustomizeStatsPanel::save()
{
  dprintf("CustomizeStatsPanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
CustomizeStatsPanel::saveAs()
{
  dprintf("CustomizeStatsPanel::saveAs() requested.\n");
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
CustomizeStatsPanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

  MessageObject *msgObject = (MessageObject *)msg;
//  if( msgObject->msgType == getName() )
#ifdef DEBUG_CSP
   printf("CustomizeStatsPanel::listener() getName=%s\n", getName());
#endif
// msgObject->print();

  if( msgObject->msgType == "ClosingDownObject" )
  {
    // When we close down, delete the dialog. (Optionally we could
    // disconnect the SIGNAL/SLOT connection for the update when 
    // the current tab changes.  Regardless, destroying the 
    // dialog (then setting the dialog pointer to null, prevents
    // updates from occurring as the destructors are being called
    // to clean up the column information.   This prevents and 
    // abort from occuring when the following sequence takes
    // place:  openss pcsamp -f executable; GUI:RUN, GUI:CustomizeStatsPanel,
    // GUI:CustomizeStatsPanel->Dialog, GUI:CustomizeStatsPanel->Dialog->close,
    // GUI:TERMINATE (experiment's execution), GUI:Close Experiment.
#ifdef DEBUG_CSP
   printf("CustomizeStatsPanel::listener(), ClosingDownObject, mcc1=%x\n", mcc1 );
   if (mcc1) {
     printf("CustomizeStatsPanel::listener(), ClosingDownObject, mcc1->dialog=%x\n", mcc1->dialog );
   }
#endif
    if( mcc1 && mcc1->dialog )
    {
      mcc1->dialog->hide();
// jeg 10-12-07 Comment this out as it causes an abort I haven't figured
//              out how to fix.  Look for TERMINATE for a corresponding
//              related issue.
//      delete mcc1->dialog;
      mcc1->dialog = NULL;
    }
    return 1;
  }

  if( msgObject->msgType == getName() && recycleFLAG == TRUE )
  {
    nprintf(DEBUG_MESSAGES) ("CustomizeStatsPanel::listener() interested!\n");
#ifdef DEBUG_CSP
    printf("CustomizeStatsPanel::listener() interested!\n");
#endif
    getPanelContainer()->raisePanel(this);
    return 1;
  }

  if(  msgObject->msgType  == "UpdateExperimentDataObject" )
  {
    UpdateObject *msg = (UpdateObject *)msgObject;
    nprintf(DEBUG_MESSAGES) ("CustomizeStatsPanel::listener() UpdateExperimentDataObject!\n");
#ifdef DEBUG_CSP
    printf("CustomizeStatsPanel::listener() UpdateExperimentDataObject!\n");
#endif

    if( msg->raiseFLAG )
    {
    if( msg->raiseFLAG )
      getPanelContainer()->raisePanel((Panel *)this);
    }
    mcc1->updateInfo();
  } else if( msgObject->msgType == "PreferencesChangedObject" ) {

    nprintf(DEBUG_MESSAGES) ("CustomizeStatsPanel::listener() PreferencesChangedObject!\n");

#ifdef DEBUG_CSP
    printf("CustomizeStatsPanel::listener() PreferencesChangedObject!\n");
#endif

    pco = (PreferencesChangedObject *)msgObject;

// Currently ignored.

    preferencesChanged();
  } else if( msgObject->msgType == "SaveAsObject" ) {

//    SaveAsObject *sao = (SaveAsObject *)msg;

  dprintf("CustomizeStatsPanel!!!!! Save as!\n");

#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel!!!!! Save as!\n");
#endif

//    if( !sao )
//    {
//      return 0;  // 0 means, did not act on message.
//    }
//    exportData(sao->f, sao->ts);
// Currently you're not passing the file descriptor down... you need to.sao->f, sao->ts);
//    f = sao->f;
  dprintf("Attempt to call (unexistent) exportData() routine\n");
#ifdef DEBUG_CSP
  printf("Attempt to call (unexistent) exportData() routine\n");
#endif
//    exportData();
  }


  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
CustomizeStatsPanel::broadcast(char *msg)
{
  dprintf("CustomizeStatsPanel::broadcast() requested.\n");
#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::broadcast() requested.\n");
#endif
  return 0;
}

void
CustomizeStatsPanel::preferencesChanged()
{
  // Place holder for eventual preferenceChange callback.
}
