/*! \class TopPanel
 This class creates and manages a Panel that displays the top n metrics of
 information.  It contains a split display with a chart (pie, by default) 
 in the left side and a list of details on the right side.

 A selection on either side will either reposition and raise an existing
 SourcePanel or create and position one for you.  Requesting a context 
 sensitve menu will bring up information, or possible actions, one can
 do to each item.
 */
#include "TopPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qvaluelist.h>
#include <qmessagebox.h>
#include <qbrush.h>

#include <qapplication.h>

#include "chartform.hxx"
#include "TPChartForm.hxx"

#include <stdlib.h>  // for calloc

#include "ExprInfo.hxx"   // For dummied up data...
#include "SourceObject.hxx"

#include <qfiledialog.h>  // For the file dialog box.
#include <qfile.h>  // For the file dialog box.
#include <qtextstream.h>

#define DEMO 1

// This is only hear for the debugging tables....
static char *color_name_table[10] =
//  { "red", "purple", "papayawhip", "violet", "green" };
  { "red", "orange", "yellow", "skyblue", "green" };


/*! This is just a utility routine to truncate long names. */
char *
TopPanel::truncate(char *str, int length)
{
  char *newstr = NULL;
//  newstr = new char( length );
  newstr = (char *)calloc( length, sizeof(char)+3+1 );

  if( length > strlen(str) )
  {
    strcpy(newstr, str);
  } else
  {
    strcpy(newstr, "...");
    int extra = strlen(str)-length;
    strcat(newstr, str+extra);
    strcat(newstr, "");
  }

  return newstr;
}

/*! Unused constructor. */
TopPanel::TopPanel()
{ // Unused... Here for completeness...
  nprintf(DEBUG_CONST_DESTRUCT) ( "TopPanel::TopPanel() default constructor called\n");
}


// #define ALL_SPLIT_UP 1
/*! Creaates a TopPanel with a left and right side.   The left side
    contains an interactive chart (pie chart by default) and the right
    side contains an interactive list of metrics.
 */
TopPanel::TopPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "TopPanel::TopPanel() constructor called\n");


  setCaption("TopPanel");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("TopPanelSplitterA");

  splitterA->setOrientation( QSplitter::Horizontal );

  chartFLAG = TRUE;
  statsFLAG = TRUE;

  cf = new TPChartForm(this, splitterA, getName(), 0);
  cf->setCaption("TPChartFormIntoSplitterA");

  textEdit = new TPTextEdit( this, splitterA );
  textEdit->setCaption("TPTextEditIntoSplitterA");
  textEdit->setTextFormat(PlainText);  // This makes one para == 1 line.
  textEdit->setReadOnly(TRUE);
  textEdit->setWordWrap(QTextEdit::NoWrap);

  addWhatsThis(textEdit, this);
addWhatsThis(cf, this);

#ifndef OVERRIDE_FONT
  // Set to a fixed size font so the columns line up nicely.
  QFont font = QFont("fixed");
  textEdit->setCurrentFont(font);
#endif // OVERRIDE_FONT

  // Set the background of the textEdit to the same background as the graph.
  QColor backgroundColor = cf->backgroundColor();
  QBrush brush = QBrush(backgroundColor);
  textEdit->setPaper(brush);

  connect( textEdit, SIGNAL(clicked(int, int)),
           this, SLOT(listClicked(int, int)) );

  int width = pc->width();
  int height = pc->height();
  QValueList<int> sizeList;
  sizeList.clear();
  if( splitterA->orientation() == QSplitter::Vertical )
  {
    sizeList.push_back((int)(height/4));
    sizeList.push_back(height-(int)(height/4));
  } else
  {
    sizeList.push_back((int)(width/4));
    sizeList.push_back(width-(int)(width/4));
  }
  splitterA->setSizes(sizeList);

  frameLayout->addWidget( splitterA );

  splitterA->show();

  // Here's the hook to go get a TopFiveObject
  getTopFiveData();

  getBaseWidgetFrame()->setCaption("TopPanelBaseWidget");
}
// End Fill The QTextEdit with the values


/*!
 *  Destroys the object and frees any allocated resources
 */
TopPanel::~TopPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  TopPanel::~TopPanel() destructor called\n");

  delete textEdit;
  delete cf;
  delete splitterA;
  delete frameLayout;
}

/*!
 * Add local panel options here..
 */
bool
TopPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("TopPanel::menu() requested.\n");

  contextMenu->insertSeparator();
  contextMenu->insertItem("&Save As ...", this, SLOT(saveAs()), CTRL+Key_S );
  contextMenu->insertItem("&About Experiment...", this, SLOT(aboutExperiment()), CTRL+Key_A );
  contextMenu->insertSeparator();
  contextMenu->insertItem("&Re-orientate", this, SLOT(setOrientation()), CTRL+Key_R );
  if( chartFLAG == TRUE )
  {
    contextMenu->insertItem("Hide &Chart...", this,
    SLOT(showChart()), CTRL+Key_L );
  } else
  {
    contextMenu->insertItem("Show &Chart...", this,
    SLOT(showChart()), CTRL+Key_L );
  }
  if( statsFLAG == TRUE )
  {
    contextMenu->insertItem("Hide &Statistics...", this,
    SLOT(showStats()), CTRL+Key_L );
  } else
  {
    contextMenu->insertItem("Show &Statistics...", this,
    SLOT(showStats()), CTRL+Key_L );
  }

  return( TRUE );
}

/*!
 * Add local save() functionality here.
 */
void 
TopPanel::save()
{
  nprintf(DEBUG_PANELS) ("TopPanel::save() requested.\n");
}

/*!
 * Saves the data away to a give file.   The left side is displayed as
   a png file and the right side is re-represented in html format.
 */
void 
TopPanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("TopPanel::saveAs() requested.\n");

  QFileDialog *sfd = NULL;
  QString dirName = QString::null;
  if( sfd == NULL )
  {
    sfd = new QFileDialog(this, "file dialog", TRUE );
    sfd->setCaption( QFileDialog::tr("Enter filename:") );
    sfd->setMode( QFileDialog::AnyFile );
    sfd->setSelection(QString("topPanel.html"));
    QString types(
                  "Html files (*.html *.htm);;"
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
        stream << "<html>";
        stream << "<head>";
        stream << "<meta content=\"text/html; charset=ISO-8859-1\" \
                         http-equiv=\"content-type\"> ";
        stream << "<title>TopPanel</title>";
        stream << "</head>";
  
        doSaveAs(&file);
  
        file.close();
      }
    }
  }

}

/*!
 * Add message listener() functionality here.
 */
#include "SaveAsObject.hxx"
int 
TopPanel::listener(void *msg)
{
  nprintf(DEBUG_MESSAGES) ("TopPanel::listener() requested.\n");
  SaveAsObject *sao = NULL;

#ifdef DEMO
// Just force a position to the top line....  Big Kludge... for demo.
if( msg == NULL )
{
  int element = 0;
  nprintf(DEBUG_MESSAGES) ("attempt to position at %d\n", element );
  itemSelected( element );
}
#endif // DEMO

  if( msg == NULL )
  {
    return 0;
  }

  MessageObject *msgObject = (MessageObject *)msg;

  if( msgObject->msgType == "SaveAsObject" )
  {
    sao = (SaveAsObject *)msg;
    if( !sao )
    {
      return 0;  // 0 means, did not act on message.
    }
    doSaveAs(sao->f);
  }

  
  return 0;  // 0 means, did not want this message and did not act on anything.
}

/*! Displays information about the experiment. */
void
TopPanel::aboutExperiment()
{
  nprintf(DEBUG_PANELS) ("TopPanel::aboutExperiment() entered\n");

  QString msg = QString("Target Program: %1\nExperiment Name: %2\nMarching Orders: %3\nExperiment Notes: %4").arg(topFiveObject->targetProgram).arg(topFiveObject->experimentName).arg(topFiveObject->marchingOrders).arg(topFiveObject->experimentNotes);
  QMessageBox::information( (QWidget *)this, "About The Experiment...",
    msg, QMessageBox::Ok );
}

/*! Reset the orientation of the graph/text relationship with setOrientation */
void
TopPanel::setOrientation()
{
  nprintf(DEBUG_PANELS) ("TopPanel::setOrientation() entered\n");
  Orientation o = splitterA->orientation();
  if( o == QSplitter::Vertical )
  {
    splitterA->setOrientation(QSplitter::Horizontal);
  } else
  {
    splitterA->setOrientation(QSplitter::Vertical);
  }
}

enum LABEL_TYPE  { PERCENT_T, FUNCTION_NAME_T, RANK_T, NONE_T };
/*! Get the values from the topFiveObject. */
int
TopPanel::getValues(int values[], char *color_names[], char *strings[], int n)
{
// LABEL_TYPE lt = PERCENT_T;
LABEL_TYPE lt = NONE_T;

  int i=0;
  float sum = 0.0;

  FuncInfo *fi = NULL;

  if( !topFiveObject )
  {
    return 0;
  }

  for( FuncInfoList::Iterator it = topFiveObject->funcInfoList.begin();
       it != topFiveObject->funcInfoList.end();
       it++ )
  {
values[i] = 0;
color_names[i] = "";
strings[i] = "";
    if( i == n-1 )
//    if( i == n )
    {
      break;
    }
    if( i >= 5 )
    {
      color_names[i] = color_name_table[4];
    } else
    {
      color_names[i] = color_name_table[i];
    }
    fi = (FuncInfo *)*it;
    values[i] = (int)fi->percent;

    if( lt == FUNCTION_NAME_T )
    {
      char *ptr = strchr(fi->functionName, ':');
     if( ptr )
      {
        strings[i] = ptr+=2;
      } else
      {
        strings[i] = fi->functionName;
      }
    } else if( lt == RANK_T )
    {
      char rank_buffer[10];
      sprintf(rank_buffer, "%d", i+1);
      strings[i] = strdup(rank_buffer);   // leak   FIX
    } else if( lt == PERCENT_T )
    {
      char percent_buffer[10];
      sprintf(percent_buffer, "%.2f", fi->percent);
      strings[i] = strdup(percent_buffer);   // leak   FIX
    } else   // NONE_T
    {
      strings[i] = strdup(""); // leak   FIX
    }
    sum+= values[i];
    i++;
  }
  values[i] = (int)(100-sum);
  if( lt != NONE_T )
  {
    strings[i] = strdup("other...");   // leak   FIX
  }
  if( i > 5 )
  {
    color_names[i] = color_name_table[4];
  } else
  {
    color_names[i] = color_name_table[i];
  }

  return(i+1);
}

/*! An element was selected.  Determine its associated highlight information,
    then act upon it.   In this case create a SourceObject message and
    call the SourcePanel's listener function with it.
*/
void
TopPanel::itemSelected(int element)
{
  nprintf(DEBUG_PANELS) ("TopPanel::itemSelected() = %d\n", element );

  int i = 0;
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();
  HighlightObject *hlo = NULL;

  FuncInfo *fi = NULL;
  FuncInfoList::Iterator it = NULL;

  i = 0;
  for( it = topFiveObject->funcInfoList.begin();
       it != topFiveObject->funcInfoList.end();
       it++ )
  {
    fi = (FuncInfo *)*it;
    for( int line=fi->start; line <= fi->end; line++)
    {
      if( i >= 5 )
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[4], "exclusive time");
      } else 
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[i], "exclusive time");
      }
// fprintf(stderr, "  pushback hlo: line=%d in color (%s)\n", line, hlo->color);
      highlightList->push_back(hlo);
    }
    i++;
  }

  i = 0;
  for( it = topFiveObject->funcInfoList.begin();
       it != topFiveObject->funcInfoList.end();
       it++ )
  {
     fi = (FuncInfo *)*it;
     if( i == element )
     {
       break;
     }
     i++;
  }

  nprintf(DEBUG_PANELS) ("%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  
  char msg[1024];
  sprintf(msg, "%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  

  nprintf(DEBUG_PANELS) ("highlightList=0x%x\n", highlightList);
  SourceObject *spo = new SourceObject(fi->functionName, fi->fileName, fi->function_line_number, TRUE, highlightList);



  if( broadcast((char *)spo, NEAREST_T) == 0 )
  { // No source view up...
    char *panel_type = "Source Panel";
    Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type);
//    if( i > 0 ) 
    if( p != NULL ) 
    {
//      broadcast((char *)spo);
      p->listener((void *)spo);
    }
  }
}

/*! Create the context senstive menu for the chart. */
void
TopPanel::createChartPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  nprintf(DEBUG_PANELS) ("Chart: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");

  contextMenu->insertItem("Tell Me MORE about Chart!!!", this, SLOT(details()), CTRL+Key_1 );
}

/*! Create the context senstive menu for the TextEdit. */
void
TopPanel::createTextEditPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  nprintf(DEBUG_PANELS) ("TextEdit: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");

  textEdit->setCursorPosition(textEdit->paragraphAt(pos), 0);

  contextMenu->insertItem("Tell Me MORE!!!", this, SLOT(details()), CTRL+Key_1 );

#ifdef LATER
  if( whatIsAtPos(pos) )
  {
    contextMenu->insertItem("Tell Me MORE!!!", this, SLOT(details()), CTRL+Key_1
 );
    contextMenu->insertItem("Who calls this routine?", this, SLOT(whoCallsMe()),
 CTRL+Key_2 );
    contextMenu->insertItem("What routines are called from here?", this, SLOT(wh
oDoICall()), CTRL+Key_2 );
  }
#endif // LATER

  contextMenu->insertSeparator();
  contextMenu->insertItem("Zoom In", this, SLOT(zoomIn()), CTRL+Key_Plus);
  contextMenu->insertItem("Zoom Out", this, SLOT(zoomOut()), CTRL+Key_Minus);
}

/*! There have been details requested.   Put the details out to the 
    screen in the form of an information dialog. */
void
TopPanel::details()
{
  nprintf(DEBUG_PANELS) ("TopPanel::details() entered\n");

  int para = 0;
  int index = 0;
  textEdit->getCursorPosition(&para, &index);

  QString msg = getDescription(para);

//  displayWhatsThis(msg);
  QMessageBox::information( (QWidget *)this, tr("Details..."),
    msg, QMessageBox::Ok );
}

#include <qscrollbar.h>
void
TopPanel::info(QPoint p, QObject *o)
{
  nprintf(DEBUG_PANELS) ("TopPanel::info() called.\n");

  if( o == textEdit )
  {
//    printf("o == textEdit\n");
  } else
  {
//    printf("o == chartForm\n");
  
    int item = cf->getItemFromPos( QCursor::pos() );
    QString msg = getDescription(item+1);
    displayWhatsThis(msg);
  
    return;
  }

  QScrollBar *vscrollbar = textEdit->verticalScrollBar();
  QPoint pos = textEdit->mapFromGlobal( QCursor::pos() );
  pos.setY( pos.y() + vscrollbar->value() );

  int line = textEdit->paragraphAt( pos );

  nprintf(DEBUG_PANELS) ("info() line=%d\n", line );

  QString msg = getDescription(line);

  displayWhatsThis(msg);
}

/*! If font has a larger  pointSize, bump up one size. */
void
TopPanel::zoomIn()
{
  nprintf(DEBUG_PANELS) ("TopPanel::zoomIn() entered\n");
  textEdit->zoomIn();
}

/*! If font has a smaller pointSize, bump down one size. */
void
TopPanel::zoomOut()
{
  nprintf(DEBUG_PANELS) ("TopPanel::zoomOut() entered\n");
  textEdit->zoomOut();
}

/*! The list was selected.   Figure out the line then 
    let itemSelected do the work. */
void
TopPanel::listClicked(int para, int offset)
{
  para-=1;
  nprintf(DEBUG_PANELS) ("You clicked? %d\n", para);

  if(para >= 0 && para <= 4 )
  {
    itemSelected(para);
  }
}

/*! Highlight a line of text, with a color.
 \note This could likely be abstrated out to a class that is inherited by 
 anyone using a QTextEdit.   (i.e. TPTextEdit in SourcePanel may
 include this functionality. */
void
TopPanel::highlightLine(int line, char *color, bool inverse)
{
  // para == line when QTextEdit is in PlainText mode
  line--;
// printf("highlightLine() line = %d color = (%s)\n", line, color);
  if( inverse )
  {
    textEdit->setParagraphBackgroundColor(line, QColor(color) );
  } else
  {
    textEdit->setSelection(line, 0, line, textEdit->paragraphLength(line));
    textEdit->setColor( color );
  }
}

/*! prototype: Get the top Five data. */
void
TopPanel::getTopFiveData()
{
  int NUMBER_ENTRIES = 5;

#ifdef OLDWAY
  int values[10];
  char *color_names[10];
  char *strings[10];
#else // OLDWAY
  int values[1024];
  char *color_names[1024];
  char *strings[1024];
#endif // OLDWAY

  topFiveObject = new ExprInfo();

  int number_returned = getValues(values, color_names, strings, NUMBER_ENTRIES);
  if( number_returned == 0 )
  {
    return;
  }
// number_returned++;
  cf->setValues(values, color_names, strings, number_returned);

// Fill The QTextEdit with the values
  char buffer[1024];
  char rankstr[10];
  char filestr[21];
  char funcstr[21];
  sprintf(buffer, "%-9s %-15s  %-6s   %-20s   %-s\n", "Rank", "Function", "Percent", "Filename", "Line #");
  textEdit->append(buffer);
  FuncInfo *fi;
float sum=0;
int line = 2;
int i = 0;
  for( FuncInfoList::Iterator it = topFiveObject->funcInfoList.begin();
       it != topFiveObject->funcInfoList.end();
       it++ )
  {
if( i >= number_returned )
{
  break;
}
     fi = (FuncInfo *)*it;

     char *ptr = NULL;
     ptr = truncate(fi->functionName, 17);
     strcpy(funcstr, ptr);
     free(ptr);
     ptr = truncate(fi->fileName, 17);
     strcpy(filestr, ptr);
     free(ptr);
     sprintf(rankstr, "%d", line-1);
     sprintf(buffer, "%-9s %-15s  %3.3f   %-20s  %d\n", rankstr, funcstr, fi->percent, filestr, fi->function_line_number);
sum+= fi->percent;
     textEdit->append(buffer);
     if( i >= 5 )
     {
       highlightLine(line, color_name_table[4], TRUE);
     } else
     {
       highlightLine(line, color_name_table[line-2], TRUE);
     }
     line++;
     strcpy(buffer, "");
i++;
  }
  if( sum < 100 )
  {
     sprintf(rankstr, "%d", number_returned);
     sprintf(buffer, "%4s %-15s  %-2.3f    %-15s  %d\n", rankstr, "Other...", 100-sum, "...", -1);
     textEdit->append(buffer);
     if( i >= 5 )
     {
       highlightLine(line, color_name_table[4], TRUE);
     } else
     {
       highlightLine(line, color_name_table[line-3], TRUE);
     }
  }

  // left justify everything.
  textEdit->moveCursor(QTextEdit::MoveHome, FALSE);
}

void
TopPanel::showStats()
{
  nprintf(DEBUG_PANELS) ("TopPanel::showStats() entered\n");
  if( statsFLAG == TRUE )
  {
    statsFLAG = FALSE;
    textEdit->hide();
  } else
  {
    statsFLAG = TRUE;
    textEdit->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( statsFLAG == FALSE && chartFLAG == FALSE )
  {
    chartFLAG = TRUE;
    cf->show();
  }
}

void
TopPanel::showChart()
{
  nprintf(DEBUG_PANELS) ("TopPanel::showChart() entered\n");
  if( chartFLAG == TRUE )
  {
    chartFLAG = FALSE;
    cf->hide();
  } else
  {
    chartFLAG = TRUE;
    cf->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( chartFLAG == FALSE && statsFLAG == FALSE )
  {
    statsFLAG = TRUE;
    textEdit->show();
  }
}

/*! If theres a description field, return it. */
QString
TopPanel::getDescription(int line)
{
  int i = 1;
  for( FuncInfoList::Iterator it = topFiveObject->funcInfoList.begin();
       it != topFiveObject->funcInfoList.end();
       it++, i++ )
  {
    FuncInfo *fi = (FuncInfo *)*it;

    if( i == line )
    {
      return( fi->fileName+
              QString(":")+
              QString::number(fi->start)+
              "-"+
              QString::number(fi->end)+
              "\n"+
              QString("Exclusive time.")
              );
    }
  }

  
  return(0);  // Return nothing highlighted.
}

void
TopPanel::doSaveAs(QFile *f)
{
    QString filename = f->name()+ ".png";
    cf->fileSaveAsPixmap(filename);

    QTextStream stream( f );
    stream << "<body>";
//    stream << "<h2>Report Header Line</h2><br><br>\n";

    stream << "<img style=\"width: 130px; height: 165px;\" alt=\"pie chart diagram.\" src=\"";
    stream << filename;
    stream << "\"><br>\n";

    stream << "<br>\n";
    stream << "<pre>\n";

    int lineCount = textEdit->paragraphs();
    for( int i=0; i<lineCount; i++ )
    {
        stream << textEdit->text(i);;
        stream << "\n";
    }
    stream << "<br>\n";
    stream << "<br>\n";
    stream << "</pre>\n";
    stream << "End of report\n";
    stream << "</body>";
    stream << "</html>";
}
