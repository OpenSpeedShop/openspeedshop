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

class PerformancePanel  : public Panel
{
public:
  PerformancePanel();  // Default construct
  PerformancePanel(PanelContainer *pc, const char *n); // Active constructor
  ~PerformancePanel();  // Active destructor

  QHBoxLayout * frameLayout; 
  QWidget *performancePanelContainerWidget;
protected:

private: 
  PanelContainerList *lpcl;
};
#endif // PERFORMANCE_PANEL_H
