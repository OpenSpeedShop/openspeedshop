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


#include "DebugPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


#include <qapplication.h>
#include <qwidgetlist.h> // For debugging loop of allWidgets

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlabel.h>

/*! \class DebugPanel
  This is a internal Panel.   It's sole purpose is to turn on/off debug
  flags.
*/

/*! The default constructor.   Unused. */
DebugPanel::DebugPanel()
{ // Unused... Here for completeness...
  fprintf(stderr, "DebugPanel::DebugPanel() should not be called.\n");
  fprintf(stderr, "see: DebugPanel::DebugPanel(PanelContainer *pc, const char *n)\n");
}


/*! This constructor.
 */
DebugPanel::DebugPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  setCaption("DebugPanel");
  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );


  debugAllOnButton = new QPushButton( tr("Debug All"), getBaseWidgetFrame(), "debugAllOnButton" );
  debugAllOnButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, debugAllOnButton->sizePolicy().hasHeightForWidth() ) );
  frameLayout->addWidget( debugAllOnButton );

  debugAllOffButton = new QPushButton( tr("Debug Off"), getBaseWidgetFrame(), "debugAllOffButton" );
  debugAllOffButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, debugAllOffButton->sizePolicy().hasHeightForWidth() ) );
  frameLayout->addWidget( debugAllOffButton );

//  topLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, "topLayout" );
  topLayout = new QHBoxLayout( 0, 0, 2, "topLayout" );
  frameLayout->addLayout(topLayout);

//  guiLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "guiLayout" );
  guiLayout = new QVBoxLayout( 0, 0, 2, "guiLayout" );
  topLayout->addLayout(guiLayout);

//  cliLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "cliLayout" );
  cliLayout = new QVBoxLayout( 0, 0, 2, "cliLayout" );
  topLayout->addLayout(cliLayout);

//  frameWorkLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "frameWorkLayout" );
  frameWorkLayout = new QVBoxLayout( 0, 0, 2, "frameWorkLayout" );
  topLayout->addLayout(frameWorkLayout);

//  backendLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "backendLayout" );
  backendLayout = new QVBoxLayout( 0, 0, 2, "backendLayout" );
  topLayout->addLayout(backendLayout);

//  bottomLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, "bottomLayout" );
  bottomLayout = new QHBoxLayout( 0, 0, 2, "bottomLayout" );
  frameLayout->addLayout(bottomLayout);

// GUI DEBUG DEFINITIONS
  label = new QLabel( tr("GUI Debug Settings"), getBaseWidgetFrame(), "(gui) label" );
  label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, label->sizePolicy().hasHeightForWidth() ) );
  guiLayout->addWidget( label );

  debugGUIOnButton = new QPushButton( tr("Debug GUI All"), getBaseWidgetFrame(), "debugGUIOnButton" );
  guiLayout->addWidget( debugGUIOnButton );

  debugGUIOffButton = new QPushButton( tr("Debug GUI Off"), getBaseWidgetFrame(), "debugGUIOffButton" );
  guiLayout->addWidget( debugGUIOffButton );


  debugPanelsCheckBox = new QCheckBox( tr("Debug Panels"), getBaseWidgetFrame(), "debugPanelsCheckBox");
  guiLayout->addWidget( debugPanelsCheckBox );

  debugFramesCheckBox = new QCheckBox( tr("Debug Frames"), getBaseWidgetFrame(), "debugFramesCheckBox");
  guiLayout->addWidget( debugFramesCheckBox );

  debugPanelContainersCheckBox = new QCheckBox( tr("Debug Panel Containers"), getBaseWidgetFrame(), "debugPanelContainersCheckBox");
  guiLayout->addWidget( debugPanelContainersCheckBox );

  debugDNDCheckBox = new QCheckBox( tr("Debug Drag-n-Drop"), getBaseWidgetFrame(), "debugDNDCheckBox");
  guiLayout->addWidget( debugDNDCheckBox );

  debugMessagesCheckBox = new QCheckBox( tr("Debug Messages"), getBaseWidgetFrame(), "debugMessagesCheckBox");
  guiLayout->addWidget( debugMessagesCheckBox );

  debugPrintingCheckBox = new QCheckBox( tr("Debug Printing"), getBaseWidgetFrame(), "debugPrintingCheckBox");
  guiLayout->addWidget( debugPrintingCheckBox );

  debugClipCheckBox = new QCheckBox( tr("Debug Clip"), getBaseWidgetFrame(), "debugClipCheckBox");
  guiLayout->addWidget( debugClipCheckBox );

  debugCommandsCheckBox = new QCheckBox( tr("Debug Commands"), getBaseWidgetFrame(), "debugCommandsCheckBox");
  guiLayout->addWidget( debugCommandsCheckBox );

  debugPanelContainerTreeButton = new QPushButton( tr("Dump PanelContainer Tree"), getBaseWidgetFrame(), "debugPanelContainerTreeButton" );
  guiLayout->addWidget( debugPanelContainerTreeButton );

  debugAllWidgetsButton = new QPushButton( tr("Dump All Widgets"), getBaseWidgetFrame(), "debugAllWidgetsButton" );
  guiLayout->addWidget( debugAllWidgetsButton );


// CLI DEBUG DEFINITIONS
  label = new QLabel( tr("CLI Debug Settings"), getBaseWidgetFrame(), "(cli) label" );
  label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, label->sizePolicy().hasHeightForWidth() ) );
  cliLayout->addWidget( label );

  debugCLIOnButton = new QPushButton( tr("Debug CLI All"), getBaseWidgetFrame(), "debugAllCLIPushButton" );
  cliLayout->addWidget( debugCLIOnButton );

  debugCLIOffButton = new QPushButton( tr("Debug CLI Off"), getBaseWidgetFrame(), "debugOffCLIPushButton" );
  cliLayout->addWidget( debugCLIOffButton );

  debugCLICheckBox = new QCheckBox( tr("Debug CLI"), getBaseWidgetFrame(), "debugCLICheckBox");
  cliLayout->addWidget( debugCLICheckBox );

// FrameWork DEBUG DEFINITIONS
  label = new QLabel( tr("FrameWork Debug Settings"), getBaseWidgetFrame(), "(FrameWork) label" );
  label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, label->sizePolicy().hasHeightForWidth() ) );
  frameWorkLayout->addWidget( label );

  debugFrameWorkOnButton = new QPushButton( tr("Debug FrameWork All"), getBaseWidgetFrame(), "debugAllFrameWorkPushButton" );
  frameWorkLayout->addWidget( debugFrameWorkOnButton );

  debugFrameWorkOffButton = new QPushButton( tr("Debug FrameWork Off"), getBaseWidgetFrame(), "debugOffFrameWorkPushButton" );
  frameWorkLayout->addWidget( debugFrameWorkOffButton );

  debugFrameWorkCheckBox = new QCheckBox( tr("Debug FrameWork"), getBaseWidgetFrame(), "debugFrameWorkCheckBox");
  frameWorkLayout->addWidget( debugFrameWorkCheckBox );

// Backend DEBUG DEFINITIONS
  label = new QLabel( tr("Backend Debug Settings"), getBaseWidgetFrame(), "(Backend) label" );
  label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, label->sizePolicy().hasHeightForWidth() ) );
  backendLayout->addWidget( label );

  debugBackendOnButton = new QPushButton( tr("Debug Backend All"), getBaseWidgetFrame(), "debugAllBackendPushButton" );
  backendLayout->addWidget( debugBackendOnButton );

  debugBackendOffButton = new QPushButton( tr("Debug Backend Off"), getBaseWidgetFrame(), "debugOffBackendPushButton" );
  backendLayout->addWidget( debugBackendOffButton );


  debugBackendCheckBox = new QCheckBox( tr("Debug Backend"), getBaseWidgetFrame(), "debugBackendCheckBox");
  backendLayout->addWidget( debugBackendCheckBox );




  spacerItem = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  bottomLayout->addItem( spacerItem );
  
  applyButton = new QPushButton( tr("Apply"), getBaseWidgetFrame(), "applyButton");
  applyButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, applyButton->sizePolicy().hasHeightForWidth() ) );
  bottomLayout->addWidget( applyButton );

  getBaseWidgetFrame()->setCaption("DebugPanelBaseWidget");


  // signals and slots connetions
  connect( debugAllOnButton, SIGNAL( clicked() ), this,
           SLOT( debugAllOnButtonSelected() ) );
  connect( debugAllOffButton, SIGNAL( clicked() ), this,
           SLOT( debugAllOffButtonSelected() ) );

  connect( debugGUIOnButton, SIGNAL( clicked() ), this,
           SLOT( debugGUIOnButtonSelected() ) );
  connect( debugGUIOffButton, SIGNAL( clicked() ), this,
           SLOT( debugGUIOffButtonSelected() ) );

  connect( debugCLIOnButton, SIGNAL( clicked() ), this,
           SLOT( debugCLIOnButtonSelected() ) );
  connect( debugCLIOffButton, SIGNAL( clicked() ), this,
           SLOT( debugCLIOffButtonSelected() ) );

  connect( debugFrameWorkOnButton, SIGNAL( clicked() ), this,
           SLOT( debugFrameWorkOnButtonSelected() ) );
  connect( debugFrameWorkOffButton, SIGNAL( clicked() ), this,
           SLOT( debugFrameWorkOffButtonSelected() ) );

  connect( debugBackendOnButton, SIGNAL( clicked() ), this,
           SLOT( debugBackendOnButtonSelected() ) );
  connect( debugBackendOffButton, SIGNAL( clicked() ), this,
           SLOT( debugBackendOffButtonSelected() ) );

  connect( debugPanelContainerTreeButton, SIGNAL( clicked() ), this,
           SLOT( debugPanelContainerTreeButtonSelected() ) );
  connect( debugAllWidgetsButton, SIGNAL( clicked() ), this,
           SLOT( debugAllWidgetsButtonSelected() ) );

  connect( applyButton, SIGNAL( clicked() ), this,
           SLOT( applyButtonSelected() ) );

  //Now look up the initial settings and set the check boxes correctly.
  if( __internal_debug_setting & DEBUG_PANELS )
  {
    debugPanelsCheckBox->setChecked( TRUE );
  }

  if( __internal_debug_setting & DEBUG_FRAMES )
  {
    debugFramesCheckBox->setChecked( TRUE );
  }

  if( __internal_debug_setting & DEBUG_PANELCONTAINERS )
  {
    debugPanelContainersCheckBox->setChecked( TRUE );
  }

  if( __internal_debug_setting & DEBUG_DND )
  {
    debugDNDCheckBox->setChecked( TRUE );
  }

  // debugMessagesCheckBox->setChecked( TRUE );

  // debugPrintingCheckBox->setChecked( TRUE );

  if( __internal_debug_setting & DEBUG_CLIPS )
  {
    debugClipCheckBox->setChecked( TRUE );
  }
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
DebugPanel::~DebugPanel()
{
  // Delete anything you new'd from the constructor.
}

void
DebugPanel::languageChange()
{
}


/*! This calls the user 'menu()' function
    if the user provides one.   The user can attach any specific panel
    menus to the passed argument and they will be displayed on a right
    mouse down in the panel.
    /param  contextMenu is the QPopupMenu * that use menus can be attached.
 */
bool
DebugPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("DebugPanel::menu() requested.\n");

  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
DebugPanel::save()
{
  dprintf("DebugPanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
DebugPanel::saveAs()
{
  dprintf("DebugPanel::saveAs() requested.\n");
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
DebugPanel::listener(void *msg)
{
  dprintf("DebugPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
DebugPanel::broadcast(char *msg)
{
  dprintf("DebugPanel::broadcast() requested.\n");
  return 0;
}

void
DebugPanel::debugAllOnButtonSelected()
{
  printf("debugAllOnButtonSelected()\n");
  debugGUIOnButtonSelected();
  debugCLIOnButtonSelected();
  debugFrameWorkOnButtonSelected();
  debugBackendOnButtonSelected();
}

void
DebugPanel::debugAllOffButtonSelected()
{
  printf("debugAllOffButtonSelected()\n");
  debugGUIOffButtonSelected();
  debugCLIOffButtonSelected();
  debugFrameWorkOffButtonSelected();
  debugBackendOffButtonSelected();
}

void
DebugPanel::debugGUIOnButtonSelected()
{
  printf("debugGUIOnButtonSelected()\n");

  debugPanelsCheckBox->setChecked(TRUE);;
  debugFramesCheckBox->setChecked(TRUE);;
  debugPanelContainersCheckBox->setChecked(TRUE);;
  debugDNDCheckBox->setChecked(TRUE);;
  debugMessagesCheckBox->setChecked(TRUE);;
  debugPrintingCheckBox->setChecked(TRUE);;
  debugClipCheckBox->setChecked(TRUE);;
  debugCommandsCheckBox->setChecked(TRUE);;
}

void
DebugPanel::debugGUIOffButtonSelected()
{
  printf("debugGUIOffButtonSelected()\n");
  debugPanelsCheckBox->setChecked(FALSE);;
  debugFramesCheckBox->setChecked(FALSE);;
  debugPanelContainersCheckBox->setChecked(FALSE);;
  debugDNDCheckBox->setChecked(FALSE);;
  debugMessagesCheckBox->setChecked(FALSE);;
  debugPrintingCheckBox->setChecked(FALSE);;
  debugClipCheckBox->setChecked(FALSE);;
  debugCommandsCheckBox->setChecked(FALSE);;
}

void
DebugPanel::debugCLIOnButtonSelected()
{
  printf("debugCLIOnButtonSelected()\n");
  debugCLICheckBox->setChecked(TRUE);
}

void
DebugPanel::debugCLIOffButtonSelected()
{
  printf("debugCLIOffButtonSelected()\n");
  debugCLICheckBox->setChecked(FALSE);
}

void
DebugPanel::debugFrameWorkOnButtonSelected()
{
  printf("debugFrameWorkOnButtonSelected()\n");
  debugFrameWorkCheckBox->setChecked(TRUE);
}

void
DebugPanel::debugFrameWorkOffButtonSelected()
{
  printf("debugFrameWorkOffButtonSelected()\n");
  debugFrameWorkCheckBox->setChecked(FALSE);
}

void
DebugPanel::debugBackendOnButtonSelected()
{
  printf("debugBackendOnButtonSelected()\n");
  debugBackendCheckBox->setChecked(TRUE);
}

void
DebugPanel::debugBackendOffButtonSelected()
{
  printf("debugBackendOffButtonSelected()\n");
  debugBackendCheckBox->setChecked(FALSE);
}

void
DebugPanel::applyButtonSelected()
{
  printf("applyButtonSelected()\n");

  // DEBUG GUI SETTINGS
  if( debugPanelsCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_PANELS;
  } else
  {
    __internal_debug_setting &= ~DEBUG_PANELS;
  }

  if( debugFramesCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_FRAMES;
  } else
  {
    __internal_debug_setting &= ~DEBUG_FRAMES;
  }

  if( debugPanelContainersCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_PANELCONTAINERS;
  } else
  {
    __internal_debug_setting &= ~DEBUG_PANELCONTAINERS;
  }

  if( debugDNDCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_DND;
  } else
  {
    __internal_debug_setting &= ~DEBUG_DND;
  }

  if( debugMessagesCheckBox->isOn() )
  {
  }

  if( debugPrintingCheckBox->isOn() )
  {
  }

  if( debugClipCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_CLIPS;
  } else
  {
    __internal_debug_setting &= ~DEBUG_CLIPS;
  }

  if( debugCommandsCheckBox->isOn() )
  {
    __internal_debug_setting |= DEBUG_COMMANDS;
  } else
  {
    __internal_debug_setting &= ~DEBUG_COMMANDS;
  }

}

void
DebugPanel::debugPanelContainerTreeButtonSelected()
{
  getPanelContainer()->getMasterPC()->debugPanelContainerTree();
}

void
DebugPanel::debugAllWidgetsButtonSelected()
{
  getPanelContainer()->getMasterPC()->debugAllWidgets();
}
