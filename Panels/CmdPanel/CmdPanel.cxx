#include "CmdPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#ifdef OLDWAY
#include "cli.hxx"
#endif // OLDWAY

QString prompt = QString("cli> ");

/*! \class CmdPanel
  The CmdPanel class is designed to accept command line input from the user.
  As if they were typing in commands in cli only.   

  This is prototype code to show proof of concept.
  */
CmdPanel::CmdPanel()
{ // Unused... Here for completeness...
}


CmdPanel::CmdPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CmdPanel::CmdPanel() constructor called.\n");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  output = new QTextEdit( getBaseWidgetFrame() );
  output->setTextFormat(PlainText);
  connect( output, SIGNAL(returnPressed()),
                this, SLOT(returnPressed()) );

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

  delete frameLayout;
  delete output;
}

void
CmdPanel::returnPressed()
{
  nprintf(DEBUG_PANELS) ("CmdPanel::returnPressed()\n");

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);
  output->setSelection(last_para, last_index, current_para, current_index);

  QString text = output->selectedText();
  nprintf(DEBUG_PANELS) ("The user entered (%s)\n", text.ascii() );

#ifdef OLDWAY
  OutputObject *oo = process_command(text.stripWhiteSpace().ascii());
  if( oo )
  {
    output->append( oo->outputBuffer );
  }
#endif // OLDWAY

  output->append( prompt );
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
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
