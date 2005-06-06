////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

#include "qapplication.h" // For qApp->processEvent() below.

// QString prompt = QString("openss-> ");
extern char *Current_OpenSpeedShop_Prompt;
QString prompt = QString::null;

#include "SS_Input_Manager.hxx"


/*! \class CmdPanel
  The CmdPanel class is designed to accept command line input from the user.
  As if they were typing in commands in cli only.   

  This is prototype code to show proof of concept.
  */
static  QTextEdit *output;

class OutputClass : public ss_ostream
{
   private:
    virtual void output_string (std::string s) {
// This goes to the text stream...
// fprintf(stderr,"A: %s",s.c_str());
      output->append(s.c_str());
    }
    virtual void flush_ostream () {
// This flushes the text stream...
//      fflush(stderr);
      qApp->flushX();
    }
};

CmdPanel::CmdPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CmdPanel::CmdPanel() constructor called.\n");

  // grab the prompt from the cli.
//  prompt = QString(Current_OpenSpeedShop_Prompt)+"-> ";
  prompt = QString(Current_OpenSpeedShop_Prompt);

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

// int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
oclass = new OutputClass();
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
  nprintf(DEBUG_PANELS)  ("CmdPanel::returnPressed()\n");

  int current_para;
  int current_index;

  output->scrollToBottom();

  output->getCursorPosition(&current_para, &current_index);
  output->setSelection(last_para, last_index, current_para, current_index);

  int i = 0;
  CommandList commandList;
  commandList.clear();
  for( i = last_para;i<=current_para;i++ )
  {
    nprintf(DEBUG_PANELS) ("i=%d last_para=%d current_para=%d\n", i, last_para, current_para );
    QString text = output->text(i);
    char *buffer = strdup(text.stripWhiteSpace().ascii());
    nprintf(DEBUG_PANELS) ("buffer=(%s)\n", buffer);
    if( text.stripWhiteSpace() == "" || text.stripWhiteSpace() == prompt )
    {
      free(buffer);
//      return;
      continue;
    }
    char *start_ptr = buffer;
    if( text.startsWith(prompt+" ") )
    {
      start_ptr += prompt.length()+1;
    } else if( text.startsWith(prompt) )
    {
      start_ptr += prompt.length();
    }
    QString command_string = QString(start_ptr).stripWhiteSpace();

    commandList.push_back(command_string);
    nprintf(DEBUG_PANELS) ("Put command_string on the list(%s)\n", command_string.ascii() );

    free( buffer );
  }

  nprintf(DEBUG_PANELS) ("commandList.count()=%d\n", commandList.count() );

  textDisabled = TRUE;
  for( CommandList::Iterator ci = commandList.begin(); ci != commandList.end(); ci++ )
  {
    QString command = (QString) *ci;
    nprintf(DEBUG_PANELS) ("Send down (%s)\n", command.ascii());
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
    Redirect_Window_Output( wid, oclass, oclass );

    InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii());

    // Push the command onto the history list.
    cmdHistoryListIterator = cmdHistoryList.end();
    cmdHistoryList.push_back(command);

    output->moveCursor(QTextEdit::MoveEnd, FALSE);

    // Try to check the status an print some 'bogus, but real' cli text.
    Input_Line_Status status = ILO_UNKNOWN;
    if( clip )
    {
      status = clip->What();
    } else
    {
      // We treat a null clip as a complete.   It was a special
      // command handled only by the cli.  (i.e. It wasn't passed
      // to the parser.    The cli handled it, cleaned up, and 
      // returned NULL.
      status = ILO_COMPLETE;
    }
    int rough_second_count = 0;
    while( status != ILO_COMPLETE )
    {
      nprintf(DEBUG_PANELS) ("status = %d\n", status );
      if( status == ILO_ERROR )
      {
#ifdef OLDDWAY
        int64_t val = 0;
        std::list<CommandObject *>::iterator coi;
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);
        std::list<CommandResult *>::iterator crl;
        crl = co->Result_List().begin();
        CommandResult_Int *cr_int = (CommandResult_Int *)(*crl);
        cr_int->Value(&val);

        fprintf(stderr, "ILO_ERROR val=(%d)!!!\n", val);
#endif //  OLDDWAY
        break;
      }
      sleep(1);
      qApp->processEvents(3);
      status = clip->What();
      rough_second_count++;
      if( rough_second_count > 30 )
      {
        fprintf(stdout, "Fake an error!\n");
        output->append("Unable to process command.");
        break;
       }
    }

#ifdef OLDWAY
    // This in only a cludge for now!!! FIX
    if( clip && status == ILO_COMPLETE )
    {
//      fprintf(stderr, "ILO_COMPLETE!\n");
      char *fname = tempnam("/tmp", "__oss");
      FILE *fp = fopen(fname, "w+");
      clip->Print_Results(fp, "\n", "");
      fclose(fp);
      QFile f( fname );
      QString line = NULL;
      if( f.open( IO_ReadOnly ) )
      {
        QTextStream ts(&f);
        while( !ts.atEnd() )
        {
          line = ts.readLine();  // line of text excluding '\n'
          output->append(line);
        }
        output->moveCursor(QTextEdit::MoveEnd, FALSE);
        output->getCursorPosition(&history_start_para, &history_start_index);
        history_start_index = prompt.length();
        output->moveCursor(QTextEdit::MoveEnd, FALSE);
        output->getCursorPosition(&last_para, &last_index);
      }
      unlink(fname);
    }
#else //OLDWAY
    Default_TLI_Line_Output(clip);

    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&history_start_para, &history_start_index);
    history_start_index = prompt.length();
    output->moveCursor(QTextEdit::MoveEnd, FALSE);
    output->getCursorPosition(&last_para, &last_index);
#endif //OLDWAY

  } 
  textDisabled = FALSE;

  nprintf(DEBUG_PANELS) ("Set the positions for the next command.\n");
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&history_start_para, &history_start_index);
  history_start_index = prompt.length();
  output->moveCursor(QTextEdit::MoveEnd, FALSE);
  output->getCursorPosition(&last_para, &last_index);
  nprintf(DEBUG_PANELS) ("Try to get another command.\n");
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

  nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() getName(%s)\n", getName() );
  MessageObject *messageObject = (MessageObject *)msg;
  if( messageObject->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("CmdPanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
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
