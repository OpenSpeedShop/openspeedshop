#ifndef CMD_PANEL_H
#define CMD_PANEL_H
#include <qobject.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qtextedit.h>

#include "Panel.hxx"           // Do not remove

#include "KeyEventFilter.hxx"

class PanelContainer;   // Do not remove

#include "Commander.hxx"   // Contains the ResultObject definition.

#define PANEL_CLASS_NAME CmdPanel   // Change the value of the define
                                         // to the name of your new class.

#include <qstring.h>
#include <qvaluelist.h>

typedef QValueList<QString> CmdHistoryList;

//! The command panel that fields input from the user.
class CmdPanel  : public Panel
{
  Q_OBJECT
public:
  CmdPanel(PanelContainer *pc, const char *n, char *argument);
  ~CmdPanel();  // Active destructor

  void upKey();
  void downKey();

void positionToEndForReturn();

#ifdef PULL
  void clicked(int, int);
  void selectionChanged();
#endif // PULL
protected slots:
  void menu1callback();
  void menu2callback();
  void textChanged();
  void returnPressed();
  void clicked(int, int);

protected:


private:
  QHBoxLayout * frameLayout;
  QTextEdit *output;
  int last_para;   // The from portion of the last cursor position.
  int last_index;  // The from portion of the last cursor position.

int history_start_para;
int history_start_index;

  bool menu(QPopupMenu* contextMenu);
  void save();
  void saveAs();
  int listener(void *msg);
  int broadcast(char *msg, BROADCAST_TYPE bt);

  CmdHistoryList::Iterator cmdHistoryListIterator;
  CmdHistoryList cmdHistoryList;

  void appendHistory(QString str);
  

  bool textDisabled;
};
#endif // CMD_PANEL_H
