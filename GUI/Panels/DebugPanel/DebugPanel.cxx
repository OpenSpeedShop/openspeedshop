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
    This class is intended to be used as a starting point to create
    user defined panels.   There's a script: mknewpanel that takes this
    template panel and creates a panel for the user to work with.    (See:
    mknewpanel in this directory.  i.e. type: mknewpanel --help)

    \par $ mknewpanel
    \b usage: mknewpanel directory panelname "menu header" "menu label" "menu accel" "show immediate" "grouping"
    \par where:
    \par directory:
    Is the path to the directory to put the new panel code.
    \par panelname:
    Is the name of the new panel.
    \par menu header:
    Is the Menu named to be put on the menu bar.
    \par menu label:
    Is the menu label under the menu header.
    \par menu accel:
    Is the menu accelerator to be used.   
    \par
    <b>   Note:</b> It's the panel developer's reponsiblity to resolve any conflicts.
    \par show immediate:
    Default is 0.  Setting this to 1 will display the panel upon initialization.
    \par grouping:
    Which named panel container should this menu item drop this panel by default.


    \par
    An exmple would be to cd to this DebugPanel directory and issue the
    following command:
    \par
    mknewpanel ../NewPanelName "NewPanelName" "New Panel Menu Heading" "New Panel Label" 0 "Performance"
    \par

    That command would create a new panel directory, with the necessary
    structure for the user to create a new panel.   The user's new panel would
    be in the NewPanelName directory.   The future panel would be called,
    "NewPanelName".   A toplevel menu heading will be created called "New 
    Panel Menu Heading".   An entry under that topleve menu would read "New
    Panel Label".    The panel would not be displayed upon initialization of
    the tool, but only upon menu selection.    The final argument hints to the 
    tool that this panel belongs to the group of other Performance related 
    panels.
*/


/*! The default constructor.   Unused. */
DebugPanel::DebugPanel()
{ // Unused... Here for completeness...
  fprintf(stderr, "DebugPanel::DebugPanel() should not be called.\n");
  fprintf(stderr, "see: DebugPanel::DebugPanel(PanelContainer *pc, const char *n)\n");

  panelContainer = NULL;
}


/*! This constructor is the work constructor.   It is called to
    create the new Panel and attach it to a PanelContainer.
    \param pc is a pointer to PanelContainer
      the Panel will be initially attached.
    \param name is the name give to the Panel.
      This is where the user would create the panel specific Qt code
      to do whatever functionality the user wanted the panel to perform.
 */
DebugPanel::DebugPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  panelContainer = pc;

  setCaption("DebugPanel");
  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );


  debugAllOnButton = new QPushButton( tr("Debug All"), getBaseWidgetFrame(), "debugAllOnButton" );
  debugAllOnButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, debugAllOnButton->sizePolicy().hasHeightForWidth() ) );
  frameLayout->addWidget( debugAllOnButton );

  debugAllOffButton = new QPushButton( tr("Debug Off"), getBaseWidgetFrame(), "debugAllOffButton" );
  debugAllOffButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, debugAllOffButton->sizePolicy().hasHeightForWidth() ) );
  frameLayout->addWidget( debugAllOffButton );

  topLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, "topLayout" );
  frameLayout->addLayout(topLayout);

  guiLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "guiLayout" );
  topLayout->addLayout(guiLayout);

  cliLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "cliLayout" );
  topLayout->addLayout(cliLayout);

  frameWorkLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "frameWorkLayout" );
  topLayout->addLayout(frameWorkLayout);

  backendLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, "backendLayout" );
  topLayout->addLayout(backendLayout);

  bottomLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, "bottomLayout" );
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

  debugSaveAsCheckBox = new QCheckBox( tr("Debug SaveAs"), getBaseWidgetFrame(), "debugSaveAsCheckBox");
  guiLayout->addWidget( debugSaveAsCheckBox );

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

  baseWidgetFrame->setCaption("DebugPanelBaseWidget");


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

  // debugSaveAsCheckBox->setChecked( TRUE );
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
DebugPanel::listener(char *msg)
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
  debugSaveAsCheckBox->setChecked(TRUE);;
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
  debugSaveAsCheckBox->setChecked(FALSE);;
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

  if( debugSaveAsCheckBox->isOn() )
  {
  }
}

void
DebugPanel::debugPanelContainerTreeButtonSelected()
{
  panelContainer->debugPanelContainerTree();
}

void
DebugPanel::debugAllWidgetsButtonSelected()
{
  panelContainer->debugAllWidgets();
}
