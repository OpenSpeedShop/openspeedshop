#ifndef CMD_PANEL_H
#define CMD_PANEL_H
#include <qobject.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qtextedit.h>

#include "Panel.hxx"           // Do not remove

#include "SPTextEdit.hxx"

class PanelContainer;   // Do not remove

#include "Commander.hxx"   // Contains the ResultObject definition.

#define PANEL_CLASS_NAME CmdPanel   // Change the value of the define
                                         // to the name of your new class.

//! The command panel that fields input from the user.
class CmdPanel  : public Panel
{
  Q_OBJECT
public:
  CmdPanel(PanelContainer *pc, const char *n, char *argument);
  ~CmdPanel();  // Active destructor

#ifdef PULL
  void clicked(int, int);
  void selectionChanged();
#endif // PULL
protected slots:
  void menu1callback();
  void menu2callback();
  void returnPressed();
  void textChanged();

protected:


private:
  QHBoxLayout * frameLayout;
#ifdef SPTEXTEDIT
  SPTextEdit *output;
#else // SPTEXTEDIT
  QTextEdit *output;
#endif // SPTEXTEDIT
  int last_para;   // The from portion of the last cursor position.
  int last_index;  // The from portion of the last cursor position.

  bool menu(QPopupMenu* contextMenu);
  void save();
  void saveAs();
  int listener(void *msg);
  int broadcast(char *msg, BROADCAST_TYPE bt);

  bool textDisabled;
};
#endif // CMD_PANEL_H
