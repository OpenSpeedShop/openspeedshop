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


#ifndef PCSTATSPANEL_H
#define PCSTATSPANEL_H
#include "SPListView.hxx"           // Do not remove
#include "Panel.hxx"           // Do not remove
#include "CollectorListObject.hxx"
#include "GenericProgressDialog.hxx"

#include "ToolAPI.hxx"
#include "Queries.hxx"

using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;

class PanelContainer;   // Do not remove
class QVBoxLayout;
class QHBoxLayout;
class QFile;
#include "SPChartForm.hxx"


#include <qlistview.h>
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qinputdialog.h>

#include "CollectorInfo.hxx"   // For dummied up data...

#include "SPListViewItem.hxx"


#include "ArgumentObject.hxx"
#include "SS_Input_Manager.hxx"

#include <qvaluelist.h>
typedef QValueList<QString> ColumnList;

typedef std::pair<std::string, double> item_type;
typedef std::pair<Function, double> Function_double_pair;

#include <qsettings.h>

#define PANEL_CLASS_NAME StatsPanel   // Change the value of the define
//! StatsPanel Class
class StatsPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! StatsPanel(PanelContainer *pc, const char *name)
    StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);

    //! ~StatsPanel() - The default destructor.
    ~StatsPanel();  // Active destructor

    //! Calls the user panel function listener() request.
    int listener(void *msg);

    //! Adds use panel menus (if any).
    virtual bool menu(QPopupMenu* contextMenu);

    //! Routine to popup dynamic menu.
    bool createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos );

    SPListView *splv;

    QHBoxLayout *frameLayout;
    QSplitter *splitterA;
    SPChartForm *cf;
    ColumnList columnList;
    int *metricHeaderTypeArray;  // matches the QListView # of column entries.


  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

    //! Holds the current thread that is in focus
    Thread *currentThread;

    //! Holds the current collector that is in focus
    Collector *currentCollector;

    QString currentMetricStr;
    QString currentMetricTypeStr;

    //! Current selected chart item.
    SPListViewItem *currentItem;

    QPopupMenu *threadMenu;
    QPopupMenu *metricMenu;
    QPopupMenu *popupMenu;   // Pointer to the contextMenu

    QString threadStr;
    QString collectorStr;
    QString collectorStrFromMenu;

    QFile *f;
  public slots:
    void itemSelected( QListViewItem * );
    void itemSelected( int );
    void sortColumn( int );
    void doOption(int id);
    void exportData();
    void details();
    void gotoSource(bool use_current_item = FALSE);
    void compareSelected();

  private slots:
    void threadSelected();
    void metricSelected();
    void threadMenuHighlighted(int);
    void metricMenuHighlighted(int);
    void contextMenuHighlighted(int);
    void showStats();
    void showChart();
    void setOrientation();

  private:
    bool matchSelectedItem( std::string function_name );

    void clearSourceFile(int expID);

    void updateStatsPanelData();

    void raisePreferencePanel();

    SmartPtr<std::map<Function, double> > orig_data;
    std::vector<Function_double_pair> sorted_items;
    SmartPtr<std::map<int, double> > orig_statement_data;
    bool ascending_sort;

    double Get_Total_Time();

    int expID;

    //! Flag setting, indicating if we should be displaying the statistics.
    bool statsFLAG;
  
    //! Flag setting, indicating if we should be displaying the chart.
    bool chartFLAG;

    CollectorListObject *clo;

};
#endif // PCSTATSPANEL_H
