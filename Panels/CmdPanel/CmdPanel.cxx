#include "CmdPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

QString prompt = QString("cli> ");

/*! \class CmdPanel
  The CmdPanel class is designed to accept command line input from the user.
  As if they were typing in commands in cli only.   

  This is prototype code to show proof of concept.
  */

CmdPanel::CmdPanel(PanelContainer *pc, const char *n, char *argument) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CmdPanel::CmdPanel() constructor called.\n");

  textDisabled = FALSE;

  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

#ifdef SPTEXTEDIT
  output = new SPTextEdit( this, getBaseWidgetFrame() );
#else // SPTEXTEDIT
  output = new QTextEdit( getBaseWidgetFrame() );
#endif // SPTEXTEDIT
  output->setTextFormat(PlainText);
  connect( output, SIGNAL(returnPressed()),
                this, SLOT(returnPressed()) );
  connect( output, SIGNAL(textChanged()),
                this, SLOT(textChanged()) );
#ifdef PULL
  connect( output, SIGNAL(selectionChanged()),
                this, SLOT(selectionChanged()) );
  connect( output, SIGNAL(clicked(int, int)),
                this, SLOT(clicked(int, int)) );
#endif // PULL

  frameLayout->addWidget(output);

  output->show();

  output->append( prompt );
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->setFocus();
}


/*
 *  Destroys the object and frees any allocated resources
 */
CmdPanel::~CmdPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "  CmdPanel::~CmdPanel() destructor called.\n");

//  delete frameLayout;
//  delete output;
}

#ifdef PULL
void
CmdPanel::selectionChanged()
{
printf("selectionChanged\n");
//  output->moveCursor(QTextEdit::MoveEnd, FALSE);
//  output->getCursorPosition(&last_para, &last_index);
}

void
CmdPanel::clicked(int, int)
{
printf("clicked\n");
/*
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
*/
}
#endif // PULL

void
CmdPanel::returnPressed()
{
//  printf ("CmdPanel::returnPressed()\n");

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
    if( text.stripWhiteSpace() == "" || text.stripWhiteSpace() == "cli>" )
    {
      free(buffer);
      return;
    }
    char *start_ptr = buffer;
    if( text.startsWith("cli> ") )
    {
      start_ptr += 5;
    } else if( text.startsWith("cli>") )
    {
      start_ptr += 4;
    }
//    printf("Send down (%s)\n", start_ptr);
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
    InputLineObject *ilp = Append_Input_String( wid, start_ptr);
    free( buffer );
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
  } 
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
}

void
CmdPanel::textChanged()
{
//  printf ("CmdPanel::textChanged()\n");
  if( textDisabled == TRUE )
  { 
//    printf("textDisabled return\n");
    return;
  }

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);

// printf("last_para=%d current_para=%d last_index=%d current_index=%d\n", last_para, current_para, last_index, current_index );

  if( current_para == last_para && current_index < last_index )
  {
//    printf("They're back spacing!!!!  Undo the backspace!\n");
    output->undo();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    return;
  }


  if( current_para < last_para )
  {
//   printf("They're wacking a previous line!\n");
    output->undo();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    return;
  }

  output->moveCursor(QTextEdit::MoveEnd, FALSE);


// printf("current_para=%d last_para=%d\n", current_para, last_para );
  if( last_para > -1 && current_para != last_para )
  {
    textDisabled = TRUE;
    returnPressed();
// printf("OUTPUT PROMPT!\n");
    output->append( prompt );
    output->scrollToBottom();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&last_para, &last_index);
    textDisabled = FALSE;
  }

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
