////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 The Krell Institute All Rights Reserved.
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


#include "CmdPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qapplication.h>
#include <qevent.h>

#include "CPTextEdit.hxx"

extern const char *Current_OpenSpeedShop_Prompt;
extern const char *Alternate_Current_OpenSpeedShop_Prompt;

#include "SS_Input_Manager.hxx"

/*! \class CmdPanel
  The CmdPanel class is designed to accept command line input from the user.
  As if they were typing in commands in cli only.   

  This is prototype code to show proof of concept.
  */
static  QTextEdit *output;

class OutputClass : public ss_ostream
{
  public:
    CmdPanel *cp;
    void setCP(CmdPanel *_cp) { cp = _cp;line_buffer = QString::null; };
    QString line_buffer;
  private:
    virtual void output_string (std::string s)
    {
      if( cp->closingDown == TRUE )
      {
        return;
      }
       line_buffer += s.c_str();
// printf("line_buffer=(%s)\n", line_buffer.ascii());
 
       if( strchr(line_buffer, '\n') )
       {
         QString *data = new QString(line_buffer);
         cp->postCustomEvent(data);
         line_buffer = QString::null;
       }

       int pos = s.find(Current_OpenSpeedShop_Prompt, 0);
       if( pos != 0 )
       {
         pos = s.find(Alternate_Current_OpenSpeedShop_Prompt, 0);
       }
       if( pos == 0 )
       {
// printf("We got a openss>>prompt\n");
         QString *data = new QString(line_buffer);
         cp->postCustomEvent(data);
         line_buffer = QString::null;
       }
    }
    virtual void flush_stream ()
    {
      qApp->flushX();
    }
};

CmdPanel::CmdPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CmdPanel::CmdPanel() constructor called.\n");

  closingDown = FALSE;

  prompt = QString::null;
  prompt2 = QString::null;

  editingHistory = FALSE;
  start_history_para = 0;
  start_history_index = 0;

  // grab the prompt from the cli.
  prompt = QString(Current_OpenSpeedShop_Prompt);
  prompt2 = QString(Alternate_Current_OpenSpeedShop_Prompt);
// printf("prompt=(%s)\n", prompt.ascii() );

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

//  output = new QTextEdit( getBaseWidgetFrame() );
  output = new CPTextEdit( this, getBaseWidgetFrame() );
  output->setTextFormat(PlainText);

#ifdef OLDWAY
  connect( output, SIGNAL(clicked(int, int)),
           this, SLOT(clicked(int, int)) );
#endif // OLDWAY

  frameLayout->addWidget(output);

  output->show();

  output->setFocus();

  KeyEventFilter *keyEventFilter = new KeyEventFilter(output, this);
  output->installEventFilter( keyEventFilter );

  output->installEventFilter( keyEventFilter );

  cmdHistoryListIterator = cmdHistoryList.begin();

  oclass = new OutputClass();
  oclass->setCP(this);
  oclass->Set_Issue_Prompt (true);
// printf("Got a mainprompt\n");


  show();

// printf("CmdPanel.   Redirect all output here...\n");

  int wid = getPanelContainer()->getMainWindow()->widStr.toInt();

//  printf("CmdPanel::CmdPanel, calling Redirect_Window_Output(wid=%d, ...)\n", wid);

  Redirect_Window_Output( wid, oclass, oclass );
  putOutPrompt();
  output->getCursorPosition(&start_history_para, &start_history_index);
}


/*
 *  Destroys the object and frees any allocated resources
 */
CmdPanel::~CmdPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "  CmdPanel::~CmdPanel() destructor called.\n");
}

typedef QValueList<QString> CommandList;
void
CmdPanel::returnPressed()
{
// printf("CmdPanel::returnPressed(editingHistory=%d)\n", editingHistory);

  if( !user_line_buffer.stripWhiteSpace().isEmpty() )
  {
    output->append("\n");
  }

// printf("returnPressed() entered user_line_buffer=(%s)\n", user_line_buffer.ascii() );

  if( editingHistory == TRUE )
  {
    QString history_buffer = output->text(start_history_para);
// printf("edittingHistory== TRUE: history_buffer = (%s) prompt=(%s)\n", output->text(start_history_para).ascii(), prompt.ascii() );
    if( history_buffer.startsWith(prompt) )
    {
      user_line_buffer = output->text(start_history_para).mid(prompt.length());
    } else if( history_buffer.startsWith(prompt2) )
    {
      user_line_buffer = output->text(start_history_para).mid(prompt2.length());
    } else
    {
      user_line_buffer = output->text(start_history_para);
    }
  }

  output->scrollToBottom();

// printf("user_line_buffer.c_str()=(%s)\n", user_line_buffer.ascii() );
  if( user_line_buffer.stripWhiteSpace() == "exit" || 
      user_line_buffer.stripWhiteSpace() == "quit" )
  {
// printf("call cmd_exit() \n");
    cmd_exit();
  }


  int i = 0;
  CommandList commandList;
  commandList.clear();

  QString command_with_newline = user_line_buffer + "\n";
  commandList.push_back(user_line_buffer);

// printf("commandList.count()=%d\n", commandList.count() );

  for( CommandList::Iterator ci = commandList.begin(); ci != commandList.end(); ci++ )
  {
    QString command = (QString) *ci;
    nprintf(DEBUG_PANELS) ("Send down (%s)\n", command.ascii());
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();

//    printf("CmdPanel::returnPressed, calling Redirect_Window_Output(wid=%d, ...)\n", wid);

    Redirect_Window_Output( wid, oclass, oclass );

    InputLineObject clip;
// printf("PUSH BACK (%s) to history.\n", command.ascii() );
    cmdHistoryList.push_back(command);
    cmdHistoryListIterator = cmdHistoryList.end();
    oclass->Set_Issue_Prompt (true);
// printf("send down command=(%s)\n", command.ascii() );
    if( user_line_buffer.stripWhiteSpace().isEmpty() )
    {
      Append_Input_String( wid, "\n", NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
    } else 
    {
      Append_Input_String( wid, (char *)command.ascii(), NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
    }

    output->moveCursor(QTextEdit::MoveEnd, FALSE);
  } 

  nprintf(DEBUG_PANELS) ("Set the positions for the next command.\n");
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  nprintf(DEBUG_PANELS) ("Try to get another command.\n");

 
  user_line_buffer = QString::null;

  editingHistory = FALSE;

}


void
CmdPanel::upKey()
{

  nprintf(DEBUG_PANELS) ("CmdPanel::upKey()\n");
// printf("upkey() entered\n");


  if( cmdHistoryListIterator != cmdHistoryList.begin() ) 
  {
    cmdHistoryListIterator--;
  }

  QString str = (QString)*cmdHistoryListIterator;

  if( str )
  {
    nprintf(DEBUG_PANELS) ("upKey() str=(%s)\n", str.ascii() );
// printf("upKey() str=(%s)\n", str.ascii() );

if( editingHistory )
{
  int end_para; int end_index;
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&end_para, &end_index);
  output->setSelection(start_history_para, start_history_index, end_para, end_index);
  output->insert(str);
} else
{
  clearCurrentLine();
  output->getCursorPosition(&start_history_para, &start_history_index);
}
  editingHistory = TRUE;


    clearCurrentLine();

    appendHistory(prompt+str);
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    user_line_buffer = str;
  }
}

void
CmdPanel::downKey()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::downKey()\n");
// printf("downkey() entered\n");

  if( cmdHistoryListIterator != cmdHistoryList.end() ) 
  {
    cmdHistoryListIterator++;
  }
  
  QString str = (QString)*cmdHistoryListIterator;

  if( str )
  {
    nprintf(DEBUG_PANELS) ("downKey() str=(%s)\n", str.ascii() );
// printf("downKey() str=(%s)\n", str.ascii() );
if( editingHistory )
{
  int end_para; int end_index;
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&end_para, &end_index);
  output->setSelection(start_history_para, start_history_index, end_para, end_index);
  output->insert(str);
} else
{
  clearCurrentLine();
  output->getCursorPosition(&start_history_para, &start_history_index);
}
  editingHistory = TRUE;

    clearCurrentLine();

    appendHistory(prompt+str);
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    user_line_buffer = str;
  }
}

void 
CmdPanel::positionToEnd()
{
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&start_history_para, &start_history_index);
}

void
CmdPanel::clearCurrentLine()
{
  positionToEnd();
  int para, index;
  output->getCursorPosition(&para, &index);
  output->setSelection(para, 0, para, index);
  output->removeSelectedText();
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&start_history_para, &start_history_index);
}

void
CmdPanel::putOutPrompt()
{
// printf("PutOutPrompt() enterd\n");
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  oclass->acquireLock();
  oclass->Issue_Prompt();
  oclass->releaseLock();
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&start_history_para, &start_history_index);
// printf("putOutPrompt() finished. %d, %d\n", start_history_para, start_history_index);
}

void
CmdPanel::appendHistory(QString str)
{
// printf("appendHistory entered (%s)\n", str.ascii() );

  output->append(str);
}

/*
 * Add local panel options here..
 */
bool
CmdPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("CmdPanel::menu() requested.\n");
  return( TRUE );
} /* * Add local save() functionality here.  */
void 
CmdPanel::save()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::save() requested.\n");
}

/* 
 * Add local saveAs() functionality here.
 */
void 
CmdPanel::saveAs()
{
}

/* 
 * Add message listener() functionality here.
 */
#include "MessageObject.hxx"
int 
CmdPanel::listener(void *msg)
{
  nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() requested.\n");


  nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() getName(%s)\n", getName() );
  MessageObject *messageObject = (MessageObject *)msg;
  nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() messageObject->msgType = (%s)\n", messageObject->msgType.ascii() );
  if( messageObject->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  } else if( messageObject->msgType == "ClosingDownObject" )
  {
// printf("CmdPanel::listener knows we're about to close down.\n");
    closingDown = TRUE;
  }

  // make sure you redirect the output back to the cli...\n");
  // We have a strange (temporary problem) because we're currently using
  // Redirect_Window_Output( cli->wid, spoclass, spoclass ); to push the
  // cli output to a parser that then generates the
  // This is paired up with the StatsPanel.  When you pull this, pull that one
  // too.
  if( messageObject->msgType == "Redirect_Window_Output()" )
  {
    // ----------------------------- 
    // ----------------------------- REDIRECT-WINDOW-OUTPUT
    // ----------------------------- 
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
//    printf("CmdPanel::listener, calling Redirect_Window_Output(wid=%d, ...)\n", wid);
    Redirect_Window_Output( wid, oclass, oclass );
  }
  
  return 0;  // 0 means, did not want this message and did not act on anything.
}

/* 
 * Add message broadcaster() functionality here.
 */
int
CmdPanel::broadcast(char *msg, BROADCAST_TYPE bt)
{
  nprintf(DEBUG_MESSAGES) ("CmdPanel::broadcast() requested.\n");
  return 0;
}

void
CmdPanel::menu1callback()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::menu1callback() entered\n");
}

void CmdPanel::menu2callback()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::menu2callback() entered\n");
}

void CmdPanel::CntrlC()
{
  user_line_buffer = QString::null;
  clearCurrentLine();
  output->insert(prompt);
}

#define CMDPANELEVENT 10000
#define CMDPANELPROMPTEVENT 10001
void
CmdPanel::postCustomEvent(QString *data)
{
  QCustomEvent *event;
  event = new QCustomEvent(CMDPANELEVENT);
  event->setData(data);
  QApplication::postEvent(this, event);
}

void
CmdPanel::postCustomPromptEvent(QString *data)
{
  QCustomEvent *event;
  event = new QCustomEvent(CMDPANELPROMPTEVENT);
  event->setData(data);
  QApplication::postEvent(this, event);
}

void CmdPanel::customEvent(QCustomEvent *e)
{
  if( e->type() == CMDPANELEVENT )
  {
    QString *data = static_cast<QString *>(e->data());
    // This goes to the text stream...
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->append(*data);
//   output->insert(*data);
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&start_history_para, &start_history_index);
  } else if( e->type() == CMDPANELPROMPTEVENT )
  {
    QString *data = static_cast<QString *>(e->data());
    // This goes to the text stream...
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
//   output->append(*data);
    output->insert(*data);
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&start_history_para, &start_history_index);
  }
}

void
CmdPanel::cmd_exit()
{
// printf("CmdPanel exit|quit received.\n");
  closingDown = TRUE;
  
  getPanelContainer()->getMainWindow()->fileExit();
}
