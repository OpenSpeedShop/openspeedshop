#include "PerformancePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

PerformancePanel::PerformancePanel()
{ // Unused... Here for completeness...
}


PerformancePanel::PerformancePanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  setCaption("PerformancePanel");
  printf( "PerformancePanel::PerformancePanel() constructor called.\n");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  performancePanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "performancePanelContainerWidget" );
  topPC = createPanelContainer( performancePanelContainerWidget,
                              "Performance", NULL, pc->_masterPanelContainerList );
  frameLayout->addWidget( performancePanelContainerWidget );


  topPC->splitVertical();

  performancePanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;
}

/*
 *  Destroys the object and frees any allocated resources
 */
PerformancePanel::~PerformancePanel()
{
  printf("  PerformancePanel::~PerformancePanel() destructor called.\n");

  delete performancePanelContainerWidget;
  delete frameLayout;
  delete baseWidgetFrame;
}
