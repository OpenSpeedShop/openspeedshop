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
class TopPanel  : public Panel
{
  Q_OBJECT
public:
  TopPanel();  // Default construct
  TopPanel(PanelContainer *pc, const char *n); // Active constructor
  ~TopPanel();  // Active destructor

  bool menu(QPopupMenu* contextMenu);
  void save();
  int listener(char *msg);

  int getValues(int values[], char *colors[], char *strings[], int n);
  void getTopFiveData();
  void itemSelected(int i);

  void createTextEditPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );
  void createChartPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );
  char *truncate(char *str, int length);
  void highlightLine(int line, char *color, bool inverse=FALSE);


  QHBoxLayout * frameLayout;
  QSplitter *splitterA; 
  ChartForm *cf;
  QTextEdit *textEdit;
public slots:
  void menu1callback();
  void menu2callback();

  void saveAs();
  void aboutExperiment();
  void listClicked(int para, int offset);
  void details();

protected:
  ExprInfo *topFiveObject;  // dummy data.

private:
};
#endif // TOPPANEL_H
