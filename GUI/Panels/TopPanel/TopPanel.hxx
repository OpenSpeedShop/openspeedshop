#ifndef TOPPANEL_H
#define TOPPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class ExprInfo;

#include "MyQTextEdit.hxx"
#include <qsplitter.h>
#include <qtextedit.h>
#include <qlistview.h>
#include <qcolor.h>

class ChartForm;
class QHBoxLayout;

#define PANEL_CLASS_NAME TopPanel   // Change the value of the define

                                         // to the name of your new class.
//! TopPanel displays the top 'n' metrics.
class TopPanel  : public Panel
{
  Q_OBJECT
public:
  //! Default construct - unused.
  TopPanel();

  //! Constructs a TopPanel consisting of a left side graph and a right side text.
  TopPanel(PanelContainer *pc, const char *n);

  //! Destructor
  ~TopPanel();  // Active destructor


  //! Create a context sensitive menu.
  bool menu(QPopupMenu* contextMenu);

  //! Save the contents of the TopPanel
  void save();

  //! Listener routine for the TopPanel
  int listener(char *msg);

  //! prototype: get the top 'n' values
  int getValues(int values[], char *colors[], char *strings[], int n);

  //! prototype: get the top 5 data.
  void getTopFiveData();

  //! Item at index 'i' was selected.
  void itemSelected(int i);

  //! Create a context senstive menu for the TextEdit (right side).
  void createTextEditPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );

  //! Create a context sensitive menu for the chart (left side).
  void createChartPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );

  //! truncate the character string.
  char *truncate(char *str, int length);

  //! Highlight a line in the TextEdit.
  void highlightLine(int line, char *color, bool inverse=FALSE);


  //! The main layout manager for the panel internals.
  QHBoxLayout * frameLayout;

  //! A splitter.
  QSplitter *splitterA; 

  //! The chart (left side)
  ChartForm *cf;

  //! The data list (right side)
  QTextEdit *textEdit;

public slots:
  void setOrientation();

  void saveAs();
  void aboutExperiment();
  void listClicked(int para, int offset);
  void details();

  void zoomIn();
  void zoomOut();

protected:
  //! prototype: dummy data.
  ExprInfo *topFiveObject;

private:
};
#endif // TOPPANEL_H
