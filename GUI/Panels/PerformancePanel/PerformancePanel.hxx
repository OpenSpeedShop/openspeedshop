#ifndef PERFORMANCE_PANEL_H
#define PERFORMANCE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include <qwidget.h>
#include <qhbox.h>
#include <qlayout.h>

class PanelContainer;   // Do not remove

#define PANEL_CLASS_NAME PerformancePanel   // Change the value of the define
                                         // to the name of your new class.
#include "PanelContainer.hxx"

//! Simple example of a vertically split top level PanelContainer.
class PerformancePanel  : public Panel
{
public:
  //! Default construct
  PerformancePanel(); 
  //! Work construct
  PerformancePanel(PanelContainer *pc, const char *n); // Active constructor
  //! Desstructor
  ~PerformancePanel();

  //! The layout that controls resizing.
  QHBoxLayout * frameLayout; 

  //! The child widget of frameLayout... All widgets are placed here.
  QWidget *performancePanelContainerWidget;
protected:

private: 
  //! The list of child PanelContainer for this PanelContainer.
  PanelContainerList *lpcl;
};
#endif // PERFORMANCE_PANEL_H
