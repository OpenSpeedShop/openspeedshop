#ifndef CMD_PANEL_H
#define CMD_PANEL_H
#include <qobject.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qtextedit.h>

#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#define PANEL_CLASS_NAME CmdPanel   // Change the value of the define
                                         // to the name of your new class.

//! The command panel that fields input from the user.
class CmdPanel  : public Panel
{
  Q_OBJECT
public:
  CmdPanel();  // Default construct
  CmdPanel(PanelContainer *pc, const char *n); // Active constructor
  ~CmdPanel();  // Active destructor

protected slots:
  void menu1callback();
  void menu2callback();
  void returnPressed();

protected:

private:
  QHBoxLayout * frameLayout;
  QTextEdit *output;
  int last_para;   // The from portion of the last cursor position.
  int last_index;  // The from portion of the last cursor position.

  bool menu(QPopupMenu* contextMenu);
  void save();
  void saveAs();
  int listener(void *msg);
  int broadcast(char *msg, BROADCAST_TYPE bt);

};
#endif // CMD_PANEL_H
