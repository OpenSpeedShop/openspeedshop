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


#include "CPTextEdit.hxx"

#include <qpopupmenu.h>
#include "CmdPanel.hxx"

#include <qscrollbar.h>
#include <qaction.h>

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

CPTextEdit::CPTextEdit( )
{
}

CPTextEdit::CPTextEdit( CmdPanel *cp, QWidget *parent, const char *n )
    : QTextEdit( parent, n )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "CPTextEdit::CPTextEdit( ) constructor called\n");
  cmdPanel = cp;

  connect( this, SIGNAL(clicked(int, int)),
           this, SLOT(clicked(int, int)) );
  connect( this, SIGNAL(doubleClicked(int, int)),
           this, SLOT(doubleClicked(int, int)) );
//  connect( this, SIGNAL(returnPressed()),
//           this, SLOT(returnPressed()) );
  connect( this, SIGNAL(selectionChanged()),
           this, SLOT(selectionChanged()) );
  connect( this, SIGNAL(textChanged()),
           this, SLOT(textChanged()) );
  connect( this, SIGNAL(copyAvailable(bool)),
           this, SLOT(copyAvailable(bool)) );
  connect( this, SIGNAL(modificationChanged(bool)),
           this, SLOT(modificationChanged(bool)) );
  connect( this, SIGNAL(cursorPositionChanged(int,int)),
           this, SLOT(cursorPositionChanged(int,int)) );

}

CPTextEdit::~CPTextEdit( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  CPTextEdit::~CPTextEdit( ) destructor called\n");
}

void
CPTextEdit::copy()
{
// printf("CPTextEdit::copy()\n");
 QTextEdit::copy();
}

void
CPTextEdit::cut()
{
// printf("CPTextEdit::cut()\n");
  int current_paragraph, current_index;
  getCursorPosition(&current_paragraph, &current_index);
  int _paragraphs = paragraphs();

// printf("paragraphs=%d current_paragraph=%d current_index=%d\n", _paragraphs, current_paragraph, current_index );
  current_paragraph++;
  
  if( current_paragraph == _paragraphs && current_index > cmdPanel->prompt.length() )
  {
    QTextEdit::cut();
  }
}

void
CPTextEdit::paste()
{
  int current_paragraph, current_index;
  getCursorPosition(&current_paragraph, &current_index);
  int _paragraphs = paragraphs();

// printf("paragraphs=%d current_paragraph=%d current_index=%d\n", _paragraphs, current_paragraph, current_index );
  current_paragraph++;

  if( current_paragraph != _paragraphs && current_index > cmdPanel->prompt.length() )
  {
// printf("positionToEnd()\n");
   cmdPanel->positionToEnd();
  }

// printf("CPTextEdit::paste()\n");

  // Squirrel away for multiline input check (to be done after the paste).
  int sh_para = cmdPanel->start_history_para;
  int sh_index = cmdPanel->start_history_index;

//printf("sh_para=(%d) sh_index=(%d)\n", sh_para, sh_index );

  cmdPanel->editingHistory = TRUE;
  QTextEdit::paste();


  // Now check for multiline input... and handle it.
  getCursorPosition(&current_paragraph, &current_index);
//  current_paragraph++;
// printf("paragraphs=%d current_paragraph=%d current_index=%d\n", _paragraphs, current_paragraph, current_index );

// printf("current_paragraph=(%d) sh_para=(%d)\n", current_paragraph, sh_para );
  if( current_paragraph > sh_para )
  {
// printf("ARE YOU MULTI LINE???\n");
    for( int i = sh_para; i < current_paragraph; i++ )
    {
      QString buffer = text(i);
      if( buffer.startsWith(cmdPanel->prompt) )
      {
        buffer = text(i).mid(cmdPanel->prompt.length());
      }
// printf("buffer[%d]=(%s)\n", i, buffer.ascii() );
      cmdPanel->user_line_buffer = buffer;
      cmdPanel->editingHistory = FALSE;
      cmdPanel->returnPressed();
    }
  }

}

QPopupMenu *
CPTextEdit::createPopupMenu ( const QPoint & pos )
{
// printf("begin createPopupMenu\n");
  QPopupMenu *popupMenu = new QPopupMenu(this);

  QAction *qaction = new QAction( this,  "Copy");
  qaction->addTo( popupMenu );
  qaction->setText( tr("Copy") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( copy() ) );
  qaction->setToolTip( tr("Copy text.") );

  qaction = new QAction( this,  "Cut");
  qaction->addTo( popupMenu );
  qaction->setText( tr("Cut") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( cut() ) );
  qaction->setToolTip( tr("Cut text.") );

  qaction = new QAction( this,  "Paste");
  qaction->addTo( popupMenu );
  qaction->setText( tr("Paste") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( paste() ) );
  qaction->setToolTip( tr("Paste text.") );

// printf("returning from createPopupMenu\n");


  return( popupMenu );
}

void 
CPTextEdit::textChanged ()
{
// printf("CPTextEdit::textChanged ()\n");
// HERE>>>
//  cmdPanel->editingHistory = TRUE;
}

void 
CPTextEdit::selectionChanged ()
{
// printf("CPTextEdit::selectionChanged ()\n");
}

void 
CPTextEdit::copyAvailable ( bool yes )
{
// printf("CPTextEdit::copyAvailable ( bool yes )\n");
}

void 
CPTextEdit::cursorPositionChanged ( int para, int pos )
{
// printf("CPTextEdit::cursorPositionChanged ( int para, int pos )\n");
}

void 
CPTextEdit::returnPressed ()
{
// printf("CPTextEdit::returnPressed ()\n");

//  cmdPanel->positionToEnd();
//  cmdPanel->returnPressed();
//  cmdPanel->user_line_buffer = QString::null;
}

void 
CPTextEdit::modificationChanged ( bool m )
{
// printf("CPTextEdit::modificationChanged ( bool m )\n");
}

void 
CPTextEdit::clicked ( int para, int pos )
{
// printf("CPTextEdit::clicked ( int para, int pos )\n");
}

void 
CPTextEdit::doubleClicked ( int para, int pos )
{
// printf("CPTextEdit::doubleClicked ( int para, int pos )\n");
}


