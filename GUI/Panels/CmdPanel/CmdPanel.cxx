#include "CmdPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include "cli.hxx"

QString prompt = QString("ftcli> ");

CmdPanel::CmdPanel()
{ // Unused... Here for completeness...
}


CmdPanel::CmdPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf( "CmdPanel::CmdPanel() constructor called.\n");
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
  printf( "  CmdPanel::~CmdPanel() destructor called.\n");

  delete frameLayout;
  delete output;

  delete baseWidgetFrame;
}

void
CmdPanel::returnPressed()
{
  dprintf("CmdPanel::returnPressed()\n");

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);
  output->setSelection(last_para, last_index, current_para, current_index);

  QString text = output->selectedText();
  dprintf("The user entered (%s)\n", text.ascii() );

  OutputObject *oo = process_command(text.stripWhiteSpace().ascii());
  if( oo )
  {
    output->append( oo->outputBuffer );
  }

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
  dprintf("CmdPanel::menu() requested.\n");

  contextMenu->insertItem("CmdPanel Menu Item &1", this, SLOT(menu1callback()), CTRL+Key_1 );
  contextMenu->insertItem("CmdPanel Menu Item &2", this, SLOT(menu2callback()), CTRL+Key_2 );

  return( TRUE );
} /* * Add local save() functionality here.  */
void 
CmdPanel::save()
{
  dprintf("CmdPanel::save() requested.\n");
}

/* 
 * Add local saveAs() functionality here.
 */
void 
CmdPanel::saveAs()
{
  dprintf("CmdPanel::saveAs() requested.\n");
}

/* 
 * Add message listener() functionality here.
 */
int 
CmdPanel::listener(char *msg)
{
  dprintf("CmdPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}

/* 
 * Add message broadcaster() functionality here.
 */
int
CmdPanel::broadcast(char *msg, BROADCAST_TYPE bt)
{
  dprintf("CmdPanel::broadcast() requested.\n");
  return 0;
}

void
CmdPanel::menu1callback()
{
  dprintf("CmdPanel::menu1callback() entered\n");
}

void CmdPanel::menu2callback()
{
  dprintf("CmdPanel::menu2callback() entered\n");
}
