#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#include <qlayout.h>
#include <qhbox.h>

#define PANEL_CLASS_NAME UserTimePanel   // Change the value of the define
                                         // to the name of your new class.
//! A simple place holder for a future UserTimePanel.
class UserTimePanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! UserTimePanel() - A default constructor the the Panel Class.
  UserTimePanel();  // Default construct

  //! UserTimePanel(PanelContainer *pc, const char *name)
  UserTimePanel(PanelContainer *pc, const char *n); // Active constructor

  //! ~UserTimePanel() - The default destructor.
  ~UserTimePanel();  // Active destructor

  //! Adds use panel menus (if any).
  bool menu(QPopupMenu* contextMenu);

  //! Calls the user panel function save() request.
  void save();

  //! Calls the user panel function saveas() request.
  void saveAs();

  //! Calls the user panel function listener() request.
  int listener(char *msg);

  //! Calls the panel function broadcast() message request.
  int broadcast(char *msg);

  QHBoxLayout * frameLayout;
public slots:

protected:

private:
};
#endif // TEMPLATE_PANEL_H
