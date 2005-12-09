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
typedef QValueList<QString> ThreadGroupStringList;

typedef std::pair<Function, double> Function_double_pair;
typedef std::pair<Function, unsigned int> Function_uint_pair;
typedef std::pair<Function, uint64_t> Function_uint64_pair;

#include <qsettings.h>

class SPOutputClass;

class ColumnValueClass
{
  public:
    void init() { start_index = -1; end_index = -1; };

    int start_index;
    int end_index;

    void print()
    {
      cout << start_index << " " << end_index << "\n";
    }
};


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

    QString lastAbout;

    QHBoxLayout *frameLayout;
    QSplitter *splitterA;
    SPChartForm *cf;
    ColumnList columnHeaderList;
ColumnValueClass columnValueClass[10];
    int *metricHeaderTypeArray;  // matches the QListView # of column entries.

    std::list<std::string> list_of_collectors;
    std::list<int64_t> list_of_pids;
    void updateThreadsList();
    void updateCollectorMetricList();
    void outputCLIData(QString *data);
bool mpiFLAG;

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

    //! Holds the current thread that is in focus
    Thread *currentThread;
    void setCurrentThread();

ThreadGroupStringList currentThreadGroupStrList;

    //! Holds the current collector that is in focus
    Collector *currentCollector;
    void setCurrentCollector();

    QString currentMetricStr;
    void setCurrentMetricStr();
    QString currentUserSelectedMetricStr;

    //! Current selected chart item.
    SPListViewItem *currentItem;
    int currentItemIndex;

    int showPercentageID;
    bool showPercentageFLAG;

    QPopupMenu *threadMenu;
    QPopupMenu *metricMenu;
    QPopupMenu *popupMenu;   // Pointer to the contextMenu

    QString currentThreadStr;
    QString currentThreadsStr;
    QString currentCollectorStr;
    QString collectorStrFromMenu;

    QFile *f;

    SPOutputClass *spoclass;

  public slots:
    void itemSelected( QListViewItem * );
    void itemSelected( int );
    void doOption(int id);
    void exportData();
    void details();
    void updatePanel();
    void gotoSource(bool use_current_item = FALSE);
    void showPercentageSelected();
    void aboutSelected();
    void compareSelected();
    void manageProcessesSelected();

  private slots:
    void threadSelected(int);
    void collectorMetricSelected(int);
    void showStats();
    void showChart();
    void setOrientation();

  private:
    bool matchSelectedItem( QListViewItem *item, std::string function_name );
    bool matchDoubleSelectedItem( std::string function_name );
    bool matchUIntSelectedItem( std::string function_name );
    bool matchUInt64SelectedItem( std::string function_name );

    void updateStatsPanelData();

    SPListViewItem *lastlvi;
int lastIndentLevel;
    bool gotHeader;
    bool gotColumns;
    int fieldCount;
    int percentIndex;
    void raisePreferencePanel();

    double TotalTime;
    double total_percent;
    int numberItemsToDisplayInStats;
    int numberItemsToDisplayInChart;
    ChartTextValueList ctvl;
    ChartPercentValueList cpvl;

    double double_minTime;
    double double_maxTime;
    unsigned int ui_minTime;
    unsigned int ui_maxTime;
    uint64_t uint64_minTime;
    uint64_t uint64_maxTime;

    int getLineColor(double value);
    int getLineColor(unsigned int value);
    int getLineColor(uint64_t value);


    bool descending_sort;

    double Get_Double_Total_Time();
    double Get_UInt_Total_Time();
    double Get_UInt64_Total_Time();

    int expID;

    //! Flag setting, indicating if we should be displaying the statistics.
    bool statsFLAG;

    //! Flag setting, indicating if we should be displaying the text in graphics.
    bool textFLAG;
  
    //! Flag setting, indicating if we should be displaying the chart.
    bool chartFLAG;

    CollectorListObject *clo;

};
#endif // PCSTATSPANEL_H
