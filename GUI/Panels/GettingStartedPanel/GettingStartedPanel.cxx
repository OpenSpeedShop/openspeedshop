#include "GettingStartedPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include "GettingStartedClass/.ui/gettingstartedclass.h"

GettingStartedPanel *gettingStartedPanel = NULL;

GettingStartedPanel::GettingStartedPanel()
{ // Unused... Here for completeness...
}


GettingStartedPanel::GettingStartedPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf( "GettingStartedPanel::GettingStartedPanel() constructor called.\n");

  gettingStartedPanel= this;

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  gsc = new GettingStartedClass(getBaseWidgetFrame(), getName() );
  frameLayout->addWidget( gsc );

  gsc->show();
}

/*
 *  Destroys the object and frees any allocated resources
 */
GettingStartedPanel::~GettingStartedPanel()
{
  printf("  GettingStartedPanel::~GettingStartedPanel() destructor called.\n");

  delete gsc;
  delete frameLayout;

  delete baseWidgetFrame;
}

void
GettingStartedPanel::experimentListSlot( )
{
  printf("GettingStartedPanel::experimentListSlot() entered.\n");

  panelContainer->_masterPC->dl_create_and_add_panel("Performance");
  panelContainer->_masterPC->dl_create_and_add_panel("Top Five Panel");
  panelContainer->_masterPC->dl_create_and_add_panel("Command Panel");
}
