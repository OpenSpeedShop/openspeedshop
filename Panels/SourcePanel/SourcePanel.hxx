#ifndef SOURCE_PANEL_H
#define SOURCE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include <qtextedit.h>
#include "SPCanvasForm.hxx"
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qscrollbar.h>
#include <qsplitter.h>

#include "SourceObject.hxx"
#include "HighlightObject.hxx"

#include "SPTextEdit.hxx"

class PanelContainer;   // Do not remove
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>

#include <qvaluelist.h>
#include "HighlightObject.hxx"

#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME SourcePanel   // Change the value of the define
                                         // to the name of your new class.
//! Source display widget.
class SourcePanel  : public Panel
{
  Q_OBJECT
public:
  //! Default constructor.  Unused.
  SourcePanel();

  //! Constructor for the SourcePanel.
  SourcePanel(PanelContainer *pc, const char *n, char *argument);

  //! SourcePanel destructor.
  ~SourcePanel();

  //! Context menu for the source.
  bool menu(QPopupMenu* contextMenu);

  //! Save the source to a file.
  void save();

//! Does the actual writing of the data.
void doSaveAs(QTextStream *ts);

  //! The listener routine for fielding messages.
  int listener(void *msg);

  //! The broadcaster for sending messages.
  int broadcast(char *msg, BROADCAST_TYPE bt);

  void info(QPoint pos, QObject *target = NULL);
  
  //! Layout for managing child widgets.
  QVBoxLayout * frameLayout;

  //! Layout for managing child widgets.
  QFrame *textEditLayoutFrame;

  //! Layout for managing text header size widgets.
  QHBoxLayout * textEditHeaderLayout;

  //! Layout for managing child widgets.
  QVBoxLayout * textEditLayout;

  //! Sliter for the stat/text area.
  QSplitter *splitter;

  //! Label for displaying current file displayed.
  QLabel *label;

  SPCanvasForm *canvasForm;

  //! The QTextEdit for managing the actual text.
  SPTextEdit *textEdit;

  //! A pointer to the textEdit vertical scrollbar.
  QScrollBar *vscrollbar;

  //! A pointer to the textEdit horizontal scrollbar.
  QScrollBar *hscrollbar;

  //! A pointer to the statArea (QListView) vertical scrollbar.
  QScrollBar *vbar;

  //! A pointer to the statArea (QListView) horizontal scrollbar.
  QScrollBar *hbar;

  //! The current file lineCount.
  int lineCount;

  //! The last top line.
  int lastTop;

  //! The last scrollbar value.
  int lastScrollBarValue;

  //! The last line height
  int lastLineHeight;
  
  //! The last calculated visible lines...
  int lastVisibleLines;
  
  //! Should line numbers be displayed flag.
  bool line_numbersFLAG;

  //! Should statistics be displayed flag.
  bool statsFLAG;

//! Is this the first time displaying stats FLAG.
bool firstTimeShowingStatAreaFLAG;

  //! The highlight list for this file.
  HighlightList *highlightList;

  //! The file name being displayed.
  QString fileName;

  //! Routine to load a file.
  void loadFile(const QString &fileName);

  //! Used to clear the text and set everything back to normal
  QColor defaultColor;

  //! Highlight a specific line.
  void highlightLine(int line, char *color="red", bool inverse=FALSE);

  //! Clear the highlight of a specific line.
  void clearHighlightedLine(int line);

  //! Highlight a segment of the display.
  void highlightSegment(int from_para, int start_index,
                        int to_para, int to_index, char *color="red");

  //! Clear the Highlight of a segment of the display.
  void clearHighlightSegment(int from_para, int start_index,
                             int to_para, int to_index);

  //! Clear all user highlighted selections.
  void clearAllSelections();

  //! Clear all highlights.
  void clearAllHighlights();

  //! Clear the high list.
  void clearHighlightList();

  //! Position line at center of display.
  void positionLineAtCenter(int line);

  //! Position line at top of display.
  void positionLineAtTop(int line);

  //! Returns the line if the line at pos is highlighted.
  int whatIsAtPos(const QPoint &pos);

  //! If the line is highlighted, return its' description.
  QString getDescription(int line);

  //! Holds the last line for the find command.
  int last_para;
  //! Holds the last index for the find command.
  int last_index;

  //! Holds the last text searched for by user.
  QString lastSearchText;

  //! Highlight the lines in the highlight list.
  void doFileHighlights();

  //! Creates the context popup menu.
  void createPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );

public slots:
  void clicked(int para, int offset);
  void valueChanged(int = -1);
  void chooseFile();
  void goToLine();
  void showLineNumbers();
  void showCanvasForm();
  void findString();
  void details();
  void whoCallsMe();
  void whoDoICall();
  void saveAs();

  void zoomIn();
  void zoomOut();

protected:

private:
  //! Handles the resize event to redraw the scrollbar's pixmap ...
  void handleSizeEvent(QResizeEvent *e=NULL);

  //! Set the lastLineHeight and lastVisibleLines variables.
  void calculateLastParameters();
  
};
#endif // SOURCE_PANEL_H
