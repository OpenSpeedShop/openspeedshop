#ifndef STATS_PANEL_H
#define STATS_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QVBoxLayout;

#include <qlistview.h>
#include <qinputdialog.h>

#include "ExprInfo.hxx"   // For dummied up data...

#include <qvaluelist.h>
typedef QValueList<QString> ColumnList;

#define PANEL_CLASS_NAME StatsPanel   // Change the value of the define
                                         // to the name of your new class.
//! StatsPanel Class
class StatsPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! StatsPanel(PanelContainer *pc, const char *name)
    StatsPanel(PanelContainer *pc, const char *n, char *argument);

    //! ~StatsPanel() - The default destructor.
    ~StatsPanel();  // Active destructor

    //! Adds use panel menus (if any).
    bool menu(QPopupMenu* contextMenu);

    //! Calls the user panel function save() request.
    void save();

    //! Calls the user panel function saveas() request.
    void saveAs();

    //! Calls the user panel function listener() request.
    int listener(void *msg);

    //! Controls the base layout of the Panel's widgets.
    QVBoxLayout * frameLayout;

    QListView *lv;

    bool createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos );

    void updateStatsPanelData();

    //! truncate the character string.
    char *truncateCharString(char *str, int length);

private:
int numberItemsToRead;
    ColumnList columnList;
ExprInfo *experimentData;
    int getUpdatedData(int num_entries_to_read);
    int getValues(int values[], char *colors[], char *strings[], int n);
void matchSelectedItem( int );

//    void selectionChanged();
//    void selectionChanged( QListViewItem* );
//    void myContextMenuEvent( QListViewItem* );
  public slots:
    void gotoSource();
    void setNumberVisibleEntries();
    void doOption(int id);

    void itemSelected( QListViewItem * );

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

  private:
};
#endif // STATS_PANEL_H
