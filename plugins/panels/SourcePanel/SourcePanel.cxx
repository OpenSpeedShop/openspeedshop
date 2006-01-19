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


/*! \class SourcePanel
 The SourcePanel is responsible for managing source files.

 This class will manage all source details.  Including highlighting/clearing
 lines, positioning source, searching the source, fielding context sensitive
 menus, basically all functions dealing with source file manipulation.
 
 */
#include "SourcePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "preference_plugin_info.hxx"


#include <stdlib.h>  // for atoi()

#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qheader.h>
#include <qaction.h>

#include <qmessagebox.h>

#include <qcursor.h>

#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"

#include "debug.hxx"

/*! Unused constructor. */
SourcePanel::SourcePanel()
{ // Unused... Here for completeness...
}


/*! This constructor creates a title label and a QTextEdit.
*/
SourcePanel::SourcePanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SourcePanel::SourcePanel() constructor called\n");
//printf ( "SourcePanel::SourcePanel() constructor called\n");

  last_spo = NULL;

  frameLayout = NULL;
  textEditLayoutFrame = NULL;
  textEditHeaderLayout = NULL;
  textEditLayout = NULL;
  splitter = NULL;
  label = NULL;
  canvasForm = NULL;
  textEdit = NULL;
  vscrollbar = NULL;
  hscrollbar = NULL;
  vbar = NULL;
  hbar = NULL;

  lineCount = 0;
  lastTop = 0;
  lastScrollBarValue = 0;
  lastLineHeight = 1;
  lastVisibleLines = 0;
  line_numbersFLAG = FALSE;
  statsFLAG = FALSE;
  firstTimeShowingStatAreaFLAG = TRUE;
  highlightList = NULL;
  fileName = QString::null;
  last_para = -1;   // For last find command.
  last_index = -1;   // For last find command.
  lastSearchText = QString::null;
  expID = 0;

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  if( ao )
  {
    expID = ao->int_data;
  }
  groupID = expID;

  splitter = new QSplitter(getBaseWidgetFrame(), "SourcePanel: splitter");
  splitter->setOrientation( QSplitter::Horizontal );
//  splitter->setHandleWidth(1);
  splitter->setHandleWidth(3);


// Create a label so I can get's it's height...
  label = new QLabel( getBaseWidgetFrame(), "text label", 0 );

  canvasForm = new SPCanvasForm( label->height(), splitter, "Stats" );
  canvasForm->hide();
  canvasForm->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, canvasForm->sizePolicy().hasHeightForWidth() ) );


#ifdef CANVAS_FORM_2
  canvasForm2 = new SPCanvasForm( label->height(), splitter, "stats" );
  canvasForm2->hide();

#endif // CANVAS_FORM_2

  delete label;

  textEditLayoutFrame = new QFrame( splitter, "New QFrame" );
  textEditLayout = new QVBoxLayout( textEditLayoutFrame );

  textEditHeaderLayout = new QHBoxLayout( textEditLayout, 2, "textEditHeaderLayout" );
  textEditHeaderLayout->setMargin(1);

  QSpacerItem *spacerItem = new QSpacerItem(1,label->height(), QSizePolicy::Fixed, QSizePolicy::Minimum );
  textEditHeaderLayout->addItem( spacerItem );

  label = new QLabel( textEditLayoutFrame, "text label", 0 );
  label->setCaption("SourcePanel: text label");
//  label->setFrameStyle( QFrame::NoFrame );
  QString label_text = "No source file specified.";
  label->setText(label_text);
  label->setMinimumSize( QSize(0,0) );
//  label->setSizePolicy(QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)5, 0, 0, FALSE ) );
  label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, label->sizePolicy().hasHeightForWidth() ) );


  textEditHeaderLayout->addWidget( label );


  textEdit = new SPTextEdit( this, textEditLayoutFrame );
  textEdit->setCaption("SourcePanel: SPTextEdit");
  textEdit->setHScrollBarMode( QScrollView::AlwaysOn );
  textEditLayout->addWidget( textEdit );

  addWhatsThis(textEdit, this);

  textEdit->setTextFormat(PlainText);  // This makes one para == 1 line.
  textEdit->setReadOnly(TRUE);
  textEdit->setWordWrap(QTextEdit::NoWrap);
  vscrollbar = textEdit->verticalScrollBar();
  hscrollbar = textEdit->horizontalScrollBar();
  if( vscrollbar )
  {
    connect( vscrollbar, SIGNAL(valueChanged(int)),
           this, SLOT(valueChanged(int)) );
  }

  defaultColor = textEdit->color();

  connect( textEdit, SIGNAL(clicked(int, int)),
           this, SLOT(clicked(int, int)) );

  QValueList<int> sizeList;
  sizeList.clear();
  int width = pc->width();
  int left_side_size = (int)(width/4);
  if( DEFAULT_CANVAS_WIDTH < left_side_size )
  {
    left_side_size = DEFAULT_CANVAS_WIDTH;
  }
  sizeList.push_back( left_side_size );
  sizeList.push_back( width-left_side_size );
  splitter->setSizes(sizeList);

  // Note: It seems you need to resize before adding the 
  //       splitter to the layout.   I guess that makes sense
  //       as the layout manager needs to know the size.
  frameLayout->addWidget(splitter);

  splitter->show();


  textEdit->show();
  label->show();

  textEdit->setFocus();
// printf("Now, getShowStatistics()= %d\n", getShowStatistics() );
  if( getShowStatistics() == TRUE )
  {
// printf("show canvas form!\n");
    showCanvasForm();
  }
// printf("Now, getShowLineNumbers()= %d\n", getShowLineNumbers() );
  if( getShowLineNumbers() == TRUE )
  {
// printf("show line numbers!\n");
    showLineNumbers();
  }

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);
}


/*!
 *  Destroys the object and frees any allocated resources
 */
SourcePanel::~SourcePanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  SourcePanel::~SourcePanel() destructor called\n");
#ifdef ABORTS
  delete textEdit;
  delete label;
  delete frameLayout;
#endif // ABORTS
}

/*!
 * Add local panel options here..
 */
bool
SourcePanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("SourcePanel::menu() requested.\n");

  Panel::menu(contextMenu);

  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "saveAs");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Export Data...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( saveAs() ) );
  qaction->setToolTip( tr("Save panel to ascii file.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "openNew");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Open New...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( chooseFile() ) );
  qaction->setToolTip( tr("Open a new file and load it into panel.") );

  qaction = new QAction( this,  "gotoLine");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Goto Line...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( goToLine() ) );
  qaction->setToolTip( tr("Goto a specific source line.") );

  if( expID > 0 )
  {
    qaction = new QAction( this,  "gotoFunction");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Goto Function...") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( goToFunction() ) );
    qaction->setToolTip( tr("Goto a specific function.") );
  }
  if( line_numbersFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideLineNumbers");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Hide Line Numbers") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showLineNumbers() ) );
    qaction->setToolTip( tr("Hide the line numbers.") );
  } else
  {
    qaction = new QAction( this,  "showLineNumbers");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Show Line Numbers") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showLineNumbers() ) );
    qaction->setToolTip( tr("Show line numbers.") );
  }
  if( statsFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Hide Statistics") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showCanvasForm() ) );
    qaction->setToolTip( tr("Hide statistics canvas.") );
  } else
  {
    qaction = new QAction( this,  "showStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Show Statistics") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showCanvasForm() ) );
    qaction->setToolTip( tr("Show statistics canvas.") );
  }

  qaction = new QAction( this,  "findString");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Find...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( findString() ) );
  qaction->setToolTip( tr("Search for a string in this source panel.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "zoomIn");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Zoom In") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( zoomIn() ) );
  qaction->setToolTip( tr("Makes the font larger.") );

  qaction = new QAction( this,  "zoomOut");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Zoom Out") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( zoomOut() ) );
  qaction->setToolTip( tr("Makes the font smaller.") );

  return( TRUE );
}

/*! This routine is called to create a context sensitive dynamic menu 
    base on where the mouse is located at the time of the request. */
void
SourcePanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  nprintf(DEBUG_PANELS) ("Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");

  textEdit->setCursorPosition(textEdit->paragraphAt(pos), 0);
  if( whatIsAtPos(pos) )
  {
    contextMenu->insertItem("Tell Me MORE!!!", this, SLOT(details()), CTRL+Key_1 );
    contextMenu->insertItem("Who calls this routine?", this, SLOT(whoCallsMe()), CTRL+Key_2 );
    contextMenu->insertItem("What routines are called from here?", this, SLOT(whoDoICall()), CTRL+Key_2 );
  }
}

/*!
 * Add local save() functionality here.
 */
void
SourcePanel::save()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::save() requested.\n");
}

/*! 
 * Add local saveAs() functionality here.
 */
void
SourcePanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::saveAs() requested.\n");

  QFileDialog *sfd = NULL;
  QString dirName = QString::null;
  if( sfd == NULL )
  {
    sfd = new QFileDialog(this, "file dialog", TRUE );
    sfd->setCaption( QFileDialog::tr("Enter filename:") );
    sfd->setMode( QFileDialog::AnyFile );
    sfd->setSelection(QString("SourcePanel.txt"));
    QString types(
                  "Html files (*.txt);;"
                  );
    sfd->setFilters( types );
    sfd->setDir(dirName);
  }

  QString fileName = QString::null;
  if( sfd->exec() == QDialog::Accepted )
  {
    fileName = sfd->selectedFile();

    if( !fileName.isEmpty() )
    {
      QFile file(fileName);

      if( file.open( IO_WriteOnly ) ) 
      {
        QTextStream stream(&file);

        doSaveAs(&stream);

        file.close();
      }
    }
  }
}

void
SourcePanel::preferencesChanged()
{
// printf("SourcePanel::preferencesChanged()\n");

  bool new_show_stats_val = getShowStatistics();
// printf("  show_stats_val=%d\n", show_stats_val );
  if( statsFLAG != new_show_stats_val )
  {
    showCanvasForm();
  }
  bool show_line_numbers_val = getShowLineNumbers();
// printf("  show_line_numbers_val=%d\n", show_line_numbers_val );
  if( line_numbersFLAG != show_line_numbers_val )
  {
    showLineNumbers();
  }

  // Ignore return value.
  refresh();
}

/*! 
 * The listener function fields requests to load and position source files.
 */
#include "SaveAsObject.hxx"
int 
SourcePanel::listener(void *msg)
{
  SourceObject *spo = NULL;
  SaveAsObject *sao = NULL;
  PreferencesChangedObject *pco = NULL;
  nprintf(DEBUG_PANELS) ("SourcePanel::listener() requested.\n");
//printf("SourcePanel::listener() requested.\n");

  MessageObject *msgObject = (MessageObject *)msg;

//printf("msgObject->msgType-%s getName()=%s\n", msgObject->msgType.ascii(), getName() );

  // Check the message type to make sure it's our type...
  if( msgObject->msgType == getName() && recycleFLAG == TRUE )
  {
    // Someone's looking for us... \n");
    return 1;
  } else 
  if( msgObject->msgType == "SourceObject" && recycleFLAG == TRUE )
  {
    nprintf(DEBUG_PANELS)  ("Its a SourceObject\n");
// printf("Its a SourceObject\n");
    spo = (SourceObject *)msg;
    if( !spo )
    {
      return 0;  // 0 means, did not act on message.
    }
    // If there's a specific group_id that is to be raised,
    // only raise it....   All other source panels should
    // ignore this request...
    if( spo->group_id > 0 )
    {
      if( spo->group_id != groupID )
      {
        return 0; // 0 means, did not act on message.
      }
    }

    last_spo = spo;


#if OLDWAY
    lineCount = 0;
//printf ("load the file spo->fileName=%s\n", spo->fileName.ascii() );
    if( loadFile(spo->fileName) == FALSE )
    {
      // We didn't find or load the file, but we did attempt
      // to handle this message.   Return 1.
      return 1;
    }


    highlightList = spo->highlightList;
    doFileHighlights();

    // Try to highlight the line...
    hscrollbar->setValue(0);

    if( spo->raiseFLAG == TRUE )
    {
      this->getPanelContainer()->raisePanel(this);
    }

    nprintf(DEBUG_PANELS) ("Try to position at line %d\n", spo->line_number);

    positionLineAtCenter(spo->line_number);

    // Now just make sure everything is lined up and resized correctly.
    valueChanged(-1);
#else // OLDWAY
    refresh();
#endif // OLDWAY
  } else if( msgObject->msgType == "SaveAsObject" )
  {
    sao = (SaveAsObject *)msg;
    if( !sao )
    {
      return 0;  // 0 means, did not act on message.
    }
    if( sao->f != NULL )
    {
      doSaveAs(sao->ts);
      sao->f->flush();
    }
  } else if( msgObject->msgType == "PreferencesChangedObject" )
  {
//    printf("SourcePanel:  The preferences changed.\n");
    pco = (PreferencesChangedObject *)msg;
    preferencesChanged();
  } else
  {
    return 0; // 0 means, did not act on message.
  }

  
  return 1;
}

/*!
 * Add message broadcaster() functionality here.
 */
int
SourcePanel::broadcast(char *msg, BROADCAST_TYPE bt)
{
  nprintf(DEBUG_PANELS) ("SourcePanel::broadcast() requested.\n");
  return 0;
}

void
SourcePanel::info(QPoint p, QObject *target)
{
  nprintf(DEBUG_PANELS) ("SourcePanel::info() called.\n");
  int tew = 0;
  int vbw = 0;

//  QPoint pos = textEdit->mapFromGlobal( p );
  QPoint pos = textEdit->mapFromGlobal( QCursor::pos() );

  // If we have a vertical scrollbar, see if the event was generated 
  // in the scrollbar area.
  if( textEdit->vbar )
  {
//    printf("we have a vbar\n");
    tew = textEdit->width();
    vbw = textEdit->vbar->width();
  }

  // If it was a scrollbar event then convert the pos.y location...
  if( textEdit->vbar && pos.x() > tew - vbw )
  { // This is a scrollbar pos.  Convert to a source location then continue
    // as usual.
    int vbh = textEdit->vbar->height() - (textEdit->vbar->width()*2);
    int y = pos.y()-textEdit->vbar->width();

// printf("first: The nominalized y=%d\n", y);
    if( y < 0 )
    {
//      y = 0;
      return;
    }
    if( y > vbh )
    {
//      y = vbh;
      return;
    }

    float ratio = (float)y/(float)vbh;
//    printf("second: The ratio=%f\n", ratio);

    int new_y = int (ratio * (float)textEdit->vbar->maxValue());
//    printf("finally: The new_y=%d (maxValue=%d)\n", new_y, textEdit->vbar->maxValue() );
  
    // set this to a textEdit.document() value...
    pos.setY( new_y );
  } else
  {
    pos.setY( pos.y() + vscrollbar->value() );
  }
//  printf("tew=%d vbw=%d pos.x()=%d pos.y()=%d\n", tew, vbw, pos.x(), pos.y() );

  // Now, based on the pos (location) see if there's anything interesting 
  // we can detail.
  int line = whatIsAtPos( pos );
  nprintf(DEBUG_PANELS) ("SourcePanel::info() line=%d\n", line);
  if( line <= 0 )
  {
//    printf("NOTHING TO DISPLAY\n");
    return;
  }

  QString msg = getDescription( line );
  displayWhatsThis(msg);
}

/*! This routine pops up a dialog box to select a file to be loaded. */
void
SourcePanel::chooseFile()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::chooseFile() entered\n");

  QString fn = QFileDialog::getOpenFileName( QString::null, QString::null,
                           this);
  if( !fn.isEmpty() )
  {
    loadFile( fn );
  } else
  {
//    statusBar()->message( "Loading aborted", 2000 );
  }
}

/*! Go to a particalar line and position it at the center of the display. */
void
SourcePanel::goToLine()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::goToLine() entered\n");
  bool ok;
  QString text = QInputDialog::getText(
          "Goto Line", "Enter line number:", QLineEdit::Normal,
          QString::null, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    int line = atoi(text.ascii());
    nprintf(DEBUG_PANELS) ("goto line %d\n", line);
    positionLineAtCenter(line);
//    positionLineAtTop(line);
  } else
  {
    // user entered nothing or pressed Cancel
  }
}


void
SourcePanel::goToFunction()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::goToFunction() entered\n");
  bool ok;
  QString text = QInputDialog::getText(
          "Goto Function", "Enter function name:", QLineEdit::Normal,
          QString::null, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    nprintf(DEBUG_PANELS) ("goto function %s\n", text.ascii());

    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
      if( experiment != NULL );
      {
        ThreadGroup tgrp = experiment->getThreads();
        ThreadGroup::iterator ti = tgrp.begin();
        if( tgrp.size() == 0 )
        {
          return;
        }
        Thread thread = *ti;
//        Time time = Time::Now();
        const std::string func_string = std::string(text.ascii());
//        std::pair<bool, Function>  function = thread.getFunctionByName(func_string, time);
        std::pair<bool, Function>  function = thread.getFunctionByName(func_string);
        std::set<Statement> statement_definition = function.second.getDefinitions();
        if( statement_definition.size() > 0 )
        {
          std::set<Statement>::const_iterator i = statement_definition.begin();
          std::string fileName = i->getPath();
          int line = i->getLine()-1;
      
          loadFile( fileName.c_str() );
          positionLineAtCenter(line);
        }
      }
    }

    

//    positionLineAtTop(line);
  } else
  {
    // user entered nothing or pressed Cancel
  }
}

/*! Display/Undisplay line numbers in the display. */
void
SourcePanel::showCanvasForm()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::showCanvasForm() entered\n");
  if( statsFLAG == TRUE )
  {
    statsFLAG = FALSE;
    canvasForm->hide();
#ifdef CANVAS_FORM_2
    canvasForm2->hide();
#endif // CANVAS_FORM_2
  } else
  {
    statsFLAG = TRUE;
    canvasForm->show();
#ifdef CANVAS_FORM_2
    canvasForm2->show();
#endif // CANVAS_FORM_2
  }

  // Make sure the scrollbar is sync'd with everyone..
  valueChanged(-1);
}

/*! Display/Undisplay line numbers in the display. */
void
SourcePanel::showLineNumbers()
{
  if( line_numbersFLAG == TRUE )
  {
    line_numbersFLAG = FALSE;
  } else
  {
    line_numbersFLAG = TRUE;
  }
  nprintf(DEBUG_PANELS) ("SourcePanel::showLineNumbers() entered\n");
  loadFile( fileName );
}

/*! Pops up a dialog to find a particular string in the currently displayed
    source. */
void
SourcePanel::findString()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::findString() entered\n");
  int para = last_para == -1 ? lastTop : last_para;
  int index = last_index == -1 ? 0 : last_index;
  bool ok;
  QString text = QInputDialog::getText(
          "Find String", "Enter search expression:", QLineEdit::Normal,
          lastSearchText, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    if(  textEdit->find(text, TRUE, TRUE, TRUE, &para, &index ) )
    {
      positionLineAtTop(para);
      last_para = para;
      last_index = index+1;
      lastSearchText = text;
    } else
    {
       nprintf(DEBUG_PANELS) ("DIDN'T FIND THE STRING ANYMORE.\n");
      QString msg;
      msg = QString("No string %1 found from line %2 until the end of file").arg(lastSearchText).arg(para);
      QMessageBox::information( (QWidget *)this, "Search Results...",
                               msg, QMessageBox::Ok );
    }
  } else
  {
    // user entered nothing or pressed Cancel
  }
}

/*! If font has a larger  pointSize, bump up one size. */
void
SourcePanel::zoomIn()
{
  textEdit->zoomIn();

  calculateLastParameters();

  valueChanged(-1);
}

/*! If font has a maller  pointSize, bump down one size. */
void
SourcePanel::zoomOut()
{
  textEdit->zoomOut();
  
  calculateLastParameters();

  valueChanged(-1);
}

/* Load a given file in the display. */
bool
SourcePanel::loadFile(const QString &_fileName)
{
  nprintf(DEBUG_PANELS) ("SourcePanel::loadFile() entered\n");

// printf("attempt to remap the path to _fileName=(%s)\n", _fileName.ascii() );
  QString remapped_fileName = remapPath(_fileName);

// printf("attempt to remap the path to remapped_fileName=(%s)\n", remapped_fileName.ascii() );

  canvasForm->hide();
  canvasForm->clearAllItems();
  canvasForm->show();


  bool sameFile = FALSE;
  if( fileName == remapped_fileName )
  {
    sameFile = TRUE;
    nprintf(DEBUG_PANELS) ("loadFile:: sameFile: lastTop=%d\n", lastTop );
  } else
  {
    if( highlightList )
    {
      clearHighlightList();
    }
    lastTop = 0;
    last_para = -1;   // For last find command.
    last_index = -1;   // For last find command.

  }
  if( remapped_fileName.isEmpty() )
  { // Just clear the Source Panel and return.
    textEdit->clear();
    textEdit->clearScrollBar();
    label->setText(tr("No file found."));
    if( !fileName.isEmpty() )
    {
      QString msg;
      msg = QString("Unable to open file: %1\n-or-\n%2").arg(fileName).arg(remapped_fileName);
      QMessageBox::information( (QWidget *)this, tr("Details..."),
                               msg, QMessageBox::Ok );
    }
    return FALSE;
  }
  fileName = remapped_fileName;

  QFile f( fileName );
  if( !f.open( IO_ReadOnly ) )
  {
    QString msg;
    msg = QString("Unable to open file: %1").arg(fileName);
    QMessageBox::information( (QWidget *)this, tr("Details..."),
                               msg, QMessageBox::Ok );
    textEdit->clear();
    textEdit->clearScrollBar();
    label->setText(tr("No file found."));
    textEdit->clear();
    textEdit->append(msg);
    return FALSE;
  }

  textEdit->hide();
  // Disabling highlights, makes updating the source much quicker and cleaner.
  textEdit->setUpdatesEnabled( FALSE );

  QString line = NULL;
  QString blank_line("");
  QTextStream ts( &f );
  QString line_number;
  char line_number_buffer[10];
  textEdit->clear();
  lineCount = 0;
  if( line_numbersFLAG )
  {
    while( !ts.atEnd() )
    {
      line = ts.readLine();  // line of text excluding '\n'
      sprintf(line_number_buffer, "%6d ", lineCount+1);
      line_number = QString(line_number_buffer);
      textEdit->append(line_number+line);
      lineCount++;
    }
//printf("line_numbersFLAG lineCount=%d\n", lineCount);
  } else
  {
// As Qt documentation suggests, but it always adds an extra line...
    textEdit->setText( ts.read() );
    lineCount = textEdit->paragraphs();
    lineCount--;
//printf("no line_numbersFLAG lineCount=%d\n", lineCount);
  }
  textEdit->setCursorPosition(0, 0);
  nprintf(DEBUG_PANELS) ("lineCount=%d paragraphs()=%d\n", lineCount, textEdit->paragraphs() );

//  printf("The last line was: %s\n", line_number_buffer );


  nprintf(DEBUG_PANELS) ("lineCount=%d\n", lineCount);
  textEdit->setModified( FALSE );
  label->setText(fileName);

#ifdef SLOWS_ME_DOWN
  clearAllHighlights();

  clearAllSelections();
#endif // SLOWS_ME_DOWN

  textEdit->setUpdatesEnabled( TRUE );

  textEdit->show();

  textEdit->clearScrollBar();

  calculateLastParameters();

  if( sameFile == FALSE ) // Then position at top.
  {
    textEdit->moveCursor(QTextEdit::MoveHome, FALSE);
    canvasForm->clearAllItems();
    canvasForm->setHighlights(textEdit->font(), lastLineHeight, lastTop, lineCount, lastVisibleLines, -2, highlightList);

    QString header_label = "Stats";
    canvasForm->header->setLabel(0, header_label);
  } else
  {
    // Redisplay the high lights.
// printf("loadFile() now call doFileHighlights()\n");
    doFileHighlights();

    positionLineAtTop(lastTop);
    nprintf(DEBUG_PANELS) ("loadFile:: down here sameFile: lastTop=%d\n", lastTop);
  }

  return TRUE;
}

/*! Get more information about the current posotion (if any). */
void
SourcePanel::details()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::details() entered\n");

  int line = 0;
  int index = 0;
  textEdit->getCursorPosition(&line, &index);
  line++;

  QString msg = getDescription(line);
  QMessageBox::information( (QWidget *)this, tr("Details..."),
    msg, QMessageBox::Ok );

}

/*! prototype: Display the who calls me information. */
void
SourcePanel::whoCallsMe()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::whoCallsMe() entered\n");

  int para = 0;
  int index = 0;
  textEdit->getCursorPosition(&para, &index);
  para++;

  QString msg = getDescription(para);
  QMessageBox::information( (QWidget *)this, "Who Calls Me...",
    msg, QMessageBox::Ok );
}

/*! prototype: Display the who do I call information. */
void
SourcePanel::whoDoICall()
{
  nprintf(DEBUG_PANELS) ("SourcePanel::whoDoICall() entered\n");

  int para = 0;
  int index = 0;
  textEdit->getCursorPosition(&para, &index);
  para++;

  QString msg = getDescription(para);

  QMessageBox::information( (QWidget *)this, "Who Do I Call...",
    msg, QMessageBox::Ok );
}


/*! Highlight a line with the given color. */
void
SourcePanel::highlightLine(int line, QString color, bool inverse)
{
  // para == line when QTextEdit is in PlainText mode
  line--;
  nprintf(DEBUG_PANELS) ("highlightLine(%d, %s, %d)\n", line, color.ascii(), inverse);
  if( inverse )
  {
    textEdit->setParagraphBackgroundColor(line, QColor(color) );
  } else
  {
    textEdit->setSelection(line, 0, line, textEdit->paragraphLength(line));
    textEdit->setColor( color );
  }

  // Annotate the scrollbar for this highlight....
  textEdit->annotateScrollBarLine(line, QColor(color));
}

/*! Clear the highlight at the give line. */
void
SourcePanel::clearHighlightedLine(int line)
{
  // para == line when QTextEdit is in PlainText mode
  textEdit->setSelection(line, 0, line, textEdit->paragraphLength(line));
  textEdit->setColor( defaultColor );
}

/*! Highlight a segment in a give color. */
void
SourcePanel::highlightSegment(int from_para, int from_index, int to_para, int to_index, char *color)
{
  from_para--;
  to_para--;
  textEdit->setSelection(from_para, from_index, to_para, to_index);
  textEdit->setColor( color );

  // Annotate the scrollbar for this highlight....   (from_para == the line.)
  textEdit->annotateScrollBarLine(from_para, QColor(color));
}

/*! Clear the highlighted segment. */
void
SourcePanel::clearHighlightSegment(int from_para, int from_index, int to_para, int to_index )
{
  textEdit->setSelection(from_para, from_index, to_para, to_index);
  textEdit->setColor( defaultColor );
}

/*! Clears all user selections. */
void
SourcePanel::clearAllSelections()
{
  // Clears the black background box(es) on the screen.
  textEdit->selectAll(FALSE);
}

/*! Clear all highlights. */
void
SourcePanel::clearAllHighlights()
{
  textEdit->selectAll(TRUE);
  textEdit->setColor(defaultColor);
  textEdit->selectAll(FALSE);
}

/*! Clear the highlight list. */
void
SourcePanel::clearHighlightList()
{
  HighlightObject *hlo = NULL;
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    delete hlo;
  }
  delete highlightList;
  highlightList = NULL;
}

/*! Center the line in the display. */
void
SourcePanel::positionLineAtCenter(int center_line)
{
  nprintf(DEBUG_PANELS) ("positionLineAtCenter(%d)\n", center_line);

   int top_line = center_line - (lastVisibleLines/2);
   if( top_line < 1 )
   {
     top_line = 1;
   }

   positionLineAtTop(top_line);
}

/*! Position the line at the top of the display. */
void
SourcePanel::positionLineAtTop(int top_line)
{

  lastTop = top_line;
  nprintf(DEBUG_PANELS) ("positionLineAtTop(top_line=%d)\n", top_line);
  top_line--; // We subtract 1 as textEdit is 0 based.

  // Clears the black background box(es) on the screen.

  int value = (int) top_line * (int)lastLineHeight;
 
  nprintf(DEBUG_PANELS) ("So I think the value is %d\n", value);
  vscrollbar->setValue(value);

  // This forces a screen position.   Otherwise, some reposition, when the 
  // screen was not yet realized, were not being reposition at all.
  textEdit->setCursorPosition(top_line, 0);
}

/*! If the position is within a highlight, return true. */
int
SourcePanel::whatIsAtPos(const QPoint &pos)
{
  // remember, lines (para) and characters are 0 based.
  nprintf(DEBUG_PANELS) ("SourcePanel::whatIsAtPos() length=%d\n", textEdit->length() );

  if( textEdit->length() == 0 )
  {
    return 0;
  }
  int para = 0;
  int line = textEdit->paragraphAt(pos);
// pos is 0 base, we bump line by one to get it sync'd with file line numbers.
  line++;
  int c = textEdit->charAt(pos, &para);

  nprintf(DEBUG_PANELS) ("whatIsAtPos() line=%d para=%d c=(%d)\n", line, para, c );
  if( !highlightList )
  {
    return(0);
  }

  HighlightObject *hlo = NULL;
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    if( hlo->line == line )
    {
      nprintf(DEBUG_PANELS) ("We have data at that line!!!\n");
      return( hlo->line );
    }
  }

  return(0);  // Return nothing highlighted.
}

/*! The user clicked.  -unused. */
void
SourcePanel::clicked(int para, int offset)
{
  nprintf(DEBUG_PANELS) ("You clicked?\n");
}

/*! The value changed... That means we've scrolled.   Recalculate the
    top line (top_line) and set the lastTop. */
void
SourcePanel::valueChanged(int passed_in_value)
{
// printf("SourcePanel::valueChanged(%d)\n", passed_in_value );
  if( textEdit->isUpdatesEnabled() == FALSE )
  {
    return;
  }

// This is not correct, but it's gets close enough for right now.  FIX
  nprintf(DEBUG_PANELS) ("Your valueChanged - passed_in_value=%d\n", passed_in_value);
  int max_value = vscrollbar->maxValue();
  int value = 0;
 
  if( passed_in_value >= 0 )
  {
    value = passed_in_value;
  } else
  {
    value = vscrollbar->value();
  }
  nprintf(DEBUG_PANELS) ("valueChanged:: (%d)\n", value );
  nprintf(DEBUG_PANELS) ("passed in value=%d\n", passed_in_value );

  nprintf(DEBUG_PANELS) ("value=%d lastLineHeight=%d max_value=%d\n", value, lastLineHeight, max_value);
  int top_line = (int)(value/lastLineHeight);
  int remainder = value-(top_line*lastLineHeight);
  nprintf(DEBUG_PANELS) ("remainder = %d\n", remainder );
  remainder-=3; // For the textEdit margin.
  nprintf(DEBUG_PANELS) ("new remainder = %d\n", remainder );
  top_line++;
  lastTop = top_line;

  nprintf(DEBUG_PANELS) ("top_line =%d\n", top_line);
  canvasForm->clearAllItems();
  canvasForm->setHighlights(textEdit->font(), lastLineHeight, lastTop, lastVisibleLines, lineCount, (int)remainder, highlightList);
}

/*! If there's a highlight list.... highlight the lines. */
void
SourcePanel::doFileHighlights()
{
// printf("doFileHighlights() entered\n");

  if( !highlightList || highlightList->empty() )
  {
// printf("no highlights, return.\n");
    return;
  }

  HighlightObject *hlo = NULL;
  textEdit->setUpdatesEnabled( FALSE );
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    if( hlo->fileName == fileName )
    {
      highlightLine(hlo->line, hlo->color, TRUE);
    }
  }
  // If there was a value description, put it out.
  if( hlo )
  {
    QString header_label = hlo->value_description;
    canvasForm->header->setLabel(0, header_label);
  }

  // Don't forget to turn the refreshing (for resize etc) back on...
  textEdit->setUpdatesEnabled( TRUE );
}

/*! If theres a description field, return it. */
QString
SourcePanel::getDescription(int line)
{
  HighlightObject *hlo = NULL;
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it) {
    hlo = (HighlightObject *)*it;
    if( hlo->line == line )
    {
      return( hlo->fileName+
              ":"+
              QString::number(hlo->line)+
              "\n"+
              QString(hlo->description)
              );
    }
  }

  return(0);  // Return nothing highlighted.
}

void SourcePanel::handleSizeEvent(QResizeEvent *e)
{
  nprintf(DEBUG_PANELS) ("SourcePanel::handleSizeEvent(e) entered\n");

  calculateLastParameters();

  textEdit->clearScrollBar();
  doFileHighlights();
}

  
void
SourcePanel::calculateLastParameters()
{
//printf("calculateLastParameters()\n");
  int lineHeight = 1;
  int height = textEdit->height();
  int heightForWidth = textEdit->heightForWidth(80);
//printf("lineHeight=%d height=%d heightForWidth=%d lineCount=%d\n", lineHeight, height, heightForWidth );
  if( lineCount == 0 )
  {
    lastLineHeight = 1;
    lastVisibleLines = 0;
    return;
  } else
  {
    lineHeight = heightForWidth/lineCount;
  }
  lastLineHeight = lineHeight;
  nprintf(DEBUG_PANELS) ("height=%d lineHeight=%d\n", height, lineHeight );

//printf("height=%d lineHeight=%d\n", height, lineHeight );

  lastVisibleLines = (int)(height/lineHeight);

  // We always add one to last visible lines, just incase there's a partial
  // line on the top paired with a partial line on the bottom.    Otherwise
  // a dynamic calculation needs to be done on each valueChange.... which may
  // be the way to go if this causes problems.
  lastVisibleLines++;

  nprintf(DEBUG_PANELS) ("calculate: maxValue=%d lineHeight=%d lineCount=%d\n", vscrollbar->maxValue(), lineHeight, lineCount );
  nprintf(DEBUG_PANELS) ("calculate: heightForWidth=%d\n", heightForWidth);
  nprintf(DEBUG_PANELS) ("calculate: lastLineHeight=%d lastVisibleLines=%d\n", lastLineHeight, lastVisibleLines );
}

void
SourcePanel::doSaveAs(QTextStream *ts)
{
// printf("doSaveAs() entered\n");
  *ts << fileName;
  *ts << textEdit->text();
}

void
SourcePanel::raisePreferencePanel()
{
// printf("StatsPanel::raisePreferencePanel() \n");
  getPanelContainer()->getMainWindow()->filePreferences( sourcePanelStackPage, QString(pluginInfo->panel_type) );
}

QString
SourcePanel::remapPath(QString _fileName)
{
// printf("SourcePanel::remapPath(%s\n", _fileName.ascii() );
  QString fromStr = QString::null;
  QString toStr = QString::null;

  LineEditList *lslel = getLeftSideLineEditList();
  LineEditList *rslel = getRightSideLineEditList();

  QString newStr = _fileName;

  // Look up the matching fromStr
  int lscnt = 0;
  int rscnt = 0;
  LineEditList::Iterator lsit = lslel->begin();
  LineEditList::Iterator rsit = rslel->begin();
  for(  ; lsit != lslel->end(); ++lsit, ++rsit )
  {
    QLineEdit *lsle = (QLineEdit *)*lsit;
    QLineEdit *rsle = (QLineEdit *)*rsit;

    QString fromStr = lsle->text();
    QString toStr = rsle->text();

    if( fromStr.isEmpty() )
    {
      continue;
    }
    if( _fileName.startsWith(fromStr) )
    {
      int i = _fileName.find(fromStr);
      if( i > -1 )
      {
        newStr = toStr + _fileName.right( _fileName.length()-fromStr.length() );
      }
      break;
    }
  }

// printf("Return newStr=(%s)\n", newStr.ascii() );
  return( newStr );
}

void
SourcePanel::refresh()
{
  lineCount = 0;
//printf ("load the file last_spo->fileName=%s\n", last_spo->fileName.ascii() );
  if( loadFile(last_spo->fileName) == FALSE )
  {
    // We didn't find or load the file, but we did attempt
    // to handle this message.   Return 1.
    return;
  }

  highlightList = last_spo->highlightList;
  doFileHighlights();

  // Try to highlight the line...
  hscrollbar->setValue(0);

  if( last_spo->raiseFLAG == TRUE )
  {
    this->getPanelContainer()->raisePanel(this);
  }

  nprintf(DEBUG_PANELS) ("Try to position at line %d\n", last_spo->line_number);

  positionLineAtCenter(last_spo->line_number);

  // Now just make sure everything is lined up and resized correctly.
  valueChanged(-1);
}
