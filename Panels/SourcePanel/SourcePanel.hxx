#ifndef SOURCE_PANEL_H
#define SOURCE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include <qtextedit.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qscrollbar.h>

#include "SourceObject.hxx"
#include "HighlightObject.hxx"

#include "MyQTextEdit.hxx"

class PanelContainer;   // Do not remove
#include <qlayout.h>

#include <qvaluelist.h>
#include "HighlightObject.hxx"

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
  SourcePanel(PanelContainer *pc, const char *n);

  //! SourcePanel destructor.
  ~SourcePanel();

  //! Context menu for the source.
  bool menu(QPopupMenu* contextMenu);

  //! Save the source to a file.
  void save();

  //! Save the source to a named file.
  void saveAs();

  //! The listener routine for fielding messages.
  int listener(void *msg);

  //! The broadcaster for sending messages.
  int broadcast(char *msg, BROADCAST_TYPE bt);

  
  //! Layout for managing child widgets.
  QVBoxLayout * frameLayout;

  //! Label for displaying current file displayed.
  QLabel *label;

  //! The QTextEdit for managing the actual text.
  MyQTextEdit *textEdit;

  //! A pointer to the vertical scrollbar.
  QScrollBar *vscrollbar;

  //! A pointer to the horizontal scrollbar.
  QScrollBar *hscrollbar;

  //! The current file lineCount.
  int lineCount;

  //! The last top line.
  int lastTop;

  //! The last scrollbar value.
  int lastScrollBarValue;

  //! Should line numbers be displayed flag.
  bool line_numbersFLAG;

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
  char *getDescription(int line);

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
  void valueChanged();
  void chooseFile();
  void goToLine();
  void showLineNumbers();
  void findString();
  void details();
  void whoCallsMe();
  void whoDoICall();

  void zoomIn();
  void zoomOut();

protected:

private:
};
#endif // SOURCE_PANEL_H
