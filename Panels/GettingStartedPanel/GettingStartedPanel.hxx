#ifndef GETTINGSTARTED_PANEL_H
#define GETTINGSTARTED_PANEL_H
#include "Panel.hxx"           // Do not remove

#include <qlayout.h>
#include <qhbox.h>

class PanelContainer;   // Do not remove
class GettingStartedPanel;
class GettingStartedClass;
extern GettingStartedPanel *gettingStartedPanel;

#define PANEL_CLASS_NAME GettingStartedPanel   // Change the value of the define
                                         // to the name of your new class.

//! An initial prototype of a getting started panel.
class GettingStartedPanel  : public Panel
{
public:
  GettingStartedPanel();  // Default construct
  GettingStartedPanel(PanelContainer *pc, const char *n); // Active constructor
  ~GettingStartedPanel();  // Active destructor

  void experimentListSlot( );

  QHBoxLayout * frameLayout;
  GettingStartedClass *gsc;
protected:

private: 
};
#endif // GETTINGSTARTED_PANEL_H
