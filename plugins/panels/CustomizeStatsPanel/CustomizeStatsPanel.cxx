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

#include <qtoolbutton.h>

#include "update_icon.xpm"
#include "add_processes_icon.xpm"
#include "remove_processes_icon.xpm"
#include "add_column_icon.xpm"
#include "remove_column_icon.xpm"
#include "focus_stats_icon.xpm"
#include "load_experiment.xpm"



#include <qtextedit.h>  // For QTextEdit in example below...
CustomizeStatsPanel::CustomizeStatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");
  splitterA->setOrientation( QSplitter::Vertical );

#ifdef DEBUG_StatsPanel
  printf("CustomizeStatsPanel::CustomizeStatsPanel:: splitterA created as Vertical\n");
#endif

  expID = ao->int_data;

#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::constructor called, expID=%d\n", expID);
#endif

  setCaption("CustomizeStatsPanel");



  fileTools = new QToolBar(QString("label"), getPanelContainer()->getMainWindow(), (QWidget *)getBaseWidgetFrame(), "file operations" );
  fileTools->setOrientation( Qt::Horizontal );
  fileTools->setLabel( "File Operations" );
  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );
  frameLayout->addWidget(fileTools);


#ifdef DEBUG_StatsPanel
  printf("CustomizeStatsPanel::CustomizeStatsPanel:: fileTools created as QToolBar\n");
#endif


#if NOSPLITTER
  mcc1 = new CustomizeClass( this, getBaseWidgetFrame(), "CustomizeClass", FALSE, 0, ao->int_data, ao->qstring_data );
#else
  mcc1 = new CustomizeClass( this, splitterA, "CustomizeClass", FALSE, 0, ao->int_data, ao->qstring_data );
#endif

#ifdef DEBUG_CSP
  printf("CustomizeStatsPanel::constructor called, mcc1=%x\n", mcc1);
#endif

#if NOSPLITTER
  frameLayout->addWidget(mcc1);
#else
//  splitterA->addWidget(mcc1);
#endif

  mcc1->expID = ao->int_data;
  groupID = mcc1->expID;
  getBaseWidgetFrame()->setCaption("CustomizeStatsPanelBaseWidget");

  QPixmap *update_icon = new QPixmap( update_icon_xpm );
  new QToolButton(*update_icon, "Update the customize stats panel.", QString::null, this, SLOT( updatePanel() ), fileTools, "Update the custimized statistics panel");

  QPixmap *add_processes_icon = new QPixmap( add_processes_icon_xpm );
  new QToolButton(*add_processes_icon, "Add new processes to the compare process set.", QString::null, this, SLOT( addProcessesSelected() ), fileTools, "Add new processes to the compare process set.");

  QPixmap *remove_processes_icon = new QPixmap( remove_processes_icon_xpm );
  new QToolButton(*remove_processes_icon, "Remove new processes to the compare process set.", QString::null, this, SLOT( removeUserPSet() ), fileTools, "Remove new processes to the compare process set.");

  QPixmap *add_column_icon = new QPixmap( add_column_icon_xpm );
  new QToolButton(*add_column_icon, "Add new column to the compare set.", QString::null, this, SLOT( addColumn() ), fileTools, "Add a new column to the compare set.");

  QPixmap *remove_column_icon = new QPixmap( remove_column_icon_xpm );
  new QToolButton(*remove_column_icon, "Remove the raised tab column from the compare set.", QString::null, this, SLOT( removeColumn() ), fileTools, "Remove the raised tab column.");

  QPixmap *load_experiment_icon = new QPixmap( load_experiment_xpm );
  new QToolButton(*load_experiment_icon, "Load another experiment into the customize stats panel.", QString::null, this, SLOT( loadExperiment() ), fileTools, "Load another experiment into the customize stats panel.");

  QPixmap *focus_stats_icon = new QPixmap( focus_stats_icon_xpm );
  new QToolButton(*focus_stats_icon, "Focus the StatsPanel on this information.  Generate/Display the view from the currently requested settings.", QString::null, this, SLOT( focusStatsPanel() ), fileTools, "Generate the StatsPanel view from the current settings.");

  toolbar_status_label = new QLabel(fileTools,"toolbar_status_label");
  // default setting to match default views
  toolbar_status_label->setText("Use these icons or the menu under CustomizeStatsPanel tab.");
  fileTools->setStretchableWidget(toolbar_status_label);




  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);

  frameLayout->addWidget( splitterA );

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
CustomizeStatsPanel::focusStatsPanel()
{
   mcc1->focusOnCSetSelected();
}

void
CustomizeStatsPanel::updatePanel()
{
   mcc1->updatePanel();
}

void
CustomizeStatsPanel::addColumn()
{
   mcc1->addNewColumn();
}

void
CustomizeStatsPanel::removeColumn()
{
   mcc1->removeRaisedTab();
}

void
CustomizeStatsPanel::addProcessesSelected()
{
   mcc1->addProcessesSelected();
}

void
CustomizeStatsPanel::removeUserPSet()
{
   mcc1->removeUserPSet();
}

void
CustomizeStatsPanel::loadExperiment()
{
   mcc1->loadAdditionalExperimentSelected();
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
