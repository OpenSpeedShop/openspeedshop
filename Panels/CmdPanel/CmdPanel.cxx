#include "CmdPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

// QString prompt = QString("openss-> ");
extern char *Current_OpenSpeedShop_Prompt;
QString prompt = QString::null;


/*! \class CmdPanel
  The CmdPanel class is designed to accept command line input from the user.
  As if they were typing in commands in cli only.   

  This is prototype code to show proof of concept.
  */

CmdPanel::CmdPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CmdPanel::CmdPanel() constructor called.\n");

  // grab the prompt from the cli.
  prompt = QString(Current_OpenSpeedShop_Prompt)+"-> ";

  textDisabled = FALSE;

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  output = new QTextEdit( getBaseWidgetFrame() );
  output->setTextFormat(PlainText);
  connect( output, SIGNAL(returnPressed()),
                this, SLOT(returnPressed()) );
  connect( output, SIGNAL(textChanged()),
                this, SLOT(textChanged()) );

  frameLayout->addWidget(output);

  output->show();

  output->append( prompt );
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->setFocus();
  history_start_para = last_para;
  history_start_index = prompt.length();

  KeyEventFilter *keyEventFilter = new KeyEventFilter(output, this);
  output->installEventFilter( keyEventFilter );

  cmdHistoryListIterator = cmdHistoryList.begin();
}


/*
 *  Destroys the object and frees any allocated resources
 */
CmdPanel::~CmdPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "  CmdPanel::~CmdPanel() destructor called.\n");
}

void
CmdPanel::returnPressed()
{
  nprintf(DEBUG_PANELS)  ("CmdPanel::returnPressed()\n");

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);
  output->setSelection(last_para, last_index, current_para, current_index);

  int i = 0;
  for( i = last_para;i<=current_para;i++ )
  {
    QString text = output->text(i);
    char *buffer = strdup(text.stripWhiteSpace().ascii());
    if( text.stripWhiteSpace() == "" || text.stripWhiteSpace() == "openss->" )
    {
      free(buffer);
      return;
    }
    char *start_ptr = buffer;
//    if( text.startsWith("openss-> ") )
    if( text.startsWith(prompt+" ") )
    {
      start_ptr += prompt.length();
    } else if( text.startsWith(prompt) )
    {
      start_ptr += prompt.length()-1;
    }
    nprintf(DEBUG_PANELS) ("Send down (%s)\n", start_ptr);
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
    InputLineObject *ilp = Append_Input_String( wid, start_ptr);

    // Push the command onto the history list.
    cmdHistoryListIterator = cmdHistoryList.end();
    cmdHistoryList.push_back(start_ptr);

    free( buffer );
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&history_start_para, &history_start_index);
    history_start_index = prompt.length();
  } 
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
}

void
CmdPanel::textChanged()
{
  nprintf(DEBUG_PANELS)  ("CmdPanel::textChanged()\n");
  if( textDisabled == TRUE )
  { 
    nprintf(DEBUG_PANELS) ("textDisabled return\n");
    return;
  }

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);

  nprintf(DEBUG_PANELS) ("last_para=%d current_para=%d last_index=%d current_index=%d\n", last_para, current_para, last_index, current_index );

  if( current_para == last_para && current_index < last_index )
  {
    nprintf(DEBUG_PANELS) ("They're back spacing!!!!  Undo the backspace!\n");
    output->undo();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    return;
  }


  if( current_para < last_para )
  {
    nprintf(DEBUG_PANELS) ("They're wacking a previous line!\n");
    output->undo();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    return;
  }

  // If we're editting a line of history, don't move to the end..
  if( current_para != history_start_para ||
      current_index < history_start_index )
  {
    nprintf(DEBUG_PANELS) ("Right, or wrong.  Move cursor to the end.\n");
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
  }


  nprintf(DEBUG_PANELS) ("current_para=%d last_para=%d\n", current_para, last_para );

  if( last_para > -1 && current_para != last_para )
  {
    textDisabled = TRUE;
    returnPressed();
    nprintf(DEBUG_PANELS) ("OUTPUT PROMPT!\n");
    output->append( prompt );
    output->scrollToBottom();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&last_para, &last_index);
    textDisabled = FALSE;
  }

}

void
CmdPanel::upKey()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::upKey()\n");

  if( cmdHistoryListIterator != cmdHistoryList.begin() ) 
  {
    cmdHistoryListIterator--;
  }
  QString str = (QString)*cmdHistoryListIterator;

  if( str )
  {
    nprintf(DEBUG_PANELS) ("upKey() str=(%s)\n", str.ascii() );

    appendHistory(str);
  }
}

void
CmdPanel::downKey()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::downKey()\n");
  if( cmdHistoryListIterator != cmdHistoryList.end() ) 
  {
    cmdHistoryListIterator++;
  }
  
  QString str = (QString)*cmdHistoryListIterator;

  if( str )
  {
    nprintf(DEBUG_PANELS) ("downKey() str=(%s)\n", str.ascii() );
    appendHistory(str);
  }
}

void
CmdPanel::positionToEndForReturn()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::positionToEndForReturn()\n");
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
}

void
CmdPanel::appendHistory(QString str)
{
  int para;
  int index;

  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&para, &index);
  output->setSelection(history_start_para, history_start_index, para, index);
  output->removeSelectedText();

  nprintf(DEBUG_PANELS) ("history_start_para=%d history_start_index=%d para=%d index=%d\n", history_start_para, history_start_index, para, index);

  output->insertAt(str, history_start_para, history_start_index);

  // Now position the cursor...
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
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
  nprintf(DEBUG_SAVEAS) ("CmdPanel::saveAs() requested.\n");
}

/* 
 * Add message listener() functionality here.
 */
#include "MessageObject.hxx"
int 
CmdPanel::listener(void *msg)
{
  nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() requested.\n");

  MessageObject *messageObject = (MessageObject *)msg;
  if( messageObject->msgType == "&Command Panel" )
  {
    nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() interested!\n");
    return 1;
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
