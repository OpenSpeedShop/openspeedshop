////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 The Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef CMD_PANEL_H
#define CMD_PANEL_H
#include <stddef.h>
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

class OutputClass;


class CPTextEdit;

//! The command panel that fields input from the user.
class CmdPanel  : public Panel
{
  Q_OBJECT
public:
  CmdPanel(PanelContainer *pc, const char *n, void *argument);
  ~CmdPanel();  // Active destructor

  void upKey();
  void downKey();
  void clearCurrentLine();
  void CntrlC();

  void positionToEnd();
  void putOutPrompt();
  bool closingDown;
  bool editingHistory;
  int start_history_para;
  int start_history_index;
  void cmd_exit();
  OutputClass *oclass;

  void postCustomEvent(QString *data);
  void postCustomPromptEvent(QString *data);
  void customEvent(QCustomEvent *event);

  QString user_line_buffer;
  void returnPressed();

  QString prompt;
  QString prompt2;

#ifdef OLDWAY // Move back to protected slots:
  void clicked(int, int);
#endif // OLDWAY

protected slots:
  void menu1callback();
  void menu2callback();


protected:


private:
  QHBoxLayout * frameLayout;

  bool menu(QPopupMenu* contextMenu);
  void save();
  void saveAs();
  int listener(void *msg);
  int broadcast(char *msg, BROADCAST_TYPE bt);

  CmdHistoryList::Iterator cmdHistoryListIterator;
  CmdHistoryList cmdHistoryList;

  void appendHistory(QString str);
  

};
#endif // CMD_PANEL_H
