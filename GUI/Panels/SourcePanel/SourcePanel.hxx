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
#ifdef OLDWAY
class HighlightObject;
typedef QValueList<HighlightObject *>HighlightList;
#else // OLDWAY
#include "HighlightObject.hxx"
#endif // OLDWAY

#define PANEL_CLASS_NAME SourcePanel   // Change the value of the define
                                         // to the name of your new class.
class SourcePanel  : public Panel
{
  Q_OBJECT
public:
  SourcePanel();  // Default construct
  SourcePanel(PanelContainer *pc, const char *n); // Active constructor
  ~SourcePanel();  // Active destructor

  bool menu(QPopupMenu* contextMenu);
  void save();
  void saveAs();
  int listener(char *msg);
  int broadcast(char *msg, BROADCAST_TYPE bt);

  
  QVBoxLayout * frameLayout;
  QLabel *label;
  MyQTextEdit *textEdit;
  QScrollBar *vscrollbar;
  QScrollBar *hscrollbar;
  int lineCount;
  int lastTop;
int lastScrollBarValue;
  bool line_numbersFLAG;
  HighlightList *highlightList;
  QString fileName;
  void loadFile(const QString &fileName);

  QColor defaultColor;  // Used to clear the text and set everything back to normal
  void highlightLine(int line, char *color="red", bool inverse=FALSE);
  void clearHighlightedLine(int line);
  void highlightSegment(int from_para, int start_index,
                        int to_para, int to_index, char *color="red");
  void clearHighlightSegment(int from_para, int start_index,
                             int to_para, int to_index);
  void clearAllSelections();
  void clearAllHighlights();
  void clearHighlightList();

  void positionLineAtCenter(int line);
  void positionLineAtTop(int line);
  int whatIsAtPos(const QPoint &pos);
  char *getDescription(int line);

  int last_para;   // For last find command.
  int last_index;   // For last find command.
  QString lastSearchText;

  void doFileHighlights();

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

protected:

private:
};
#endif // SOURCE_PANEL_H
