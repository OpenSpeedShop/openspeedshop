////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2016 Krell Institute All Rights Reserved.
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


#ifndef STATSPANEL_H
#define STATSPANEL_H
#include "SPListView.hxx"           // Do not remove
#include "Panel.hxx"           // Do not remove
#include "CollectorListObject.hxx"
#include "GenericProgressDialog.hxx"
#include "SelectTimeSegmentDialog.hxx"
#include "OptionalViewsDialog.hxx"
#include "SourcePanelAnnotationDialog.hxx"
#include "ChooseExperimentDialog.hxx"

#include "ToolAPI.hxx"
#include "Queries.hxx"
#include <vector>


#define DEFAULT_CANVAS_WIDTH 100
#define DEFAULT_CANVAS_MIN 20


using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;

class PanelContainer;   // Do not remove
class QVBoxLayout;
class QHBoxLayout;
class QFile;
class GenericProgressDialog;
class SourceObject;
class QToolBar;
class QPushButton;
class QRadioButton;
class QToolBox;


#include "SPChartForm.hxx"
#include "HighlightObject.hxx"

#include "SPTextEdit.hxx"


#include <qlistview.h>
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qinputdialog.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qradiobutton.h>


#include "CollectorInfo.hxx"   // For dummied up data...

#include "SPListViewItem.hxx"


#include "ArgumentObject.hxx"
#include "SS_Input_Manager.hxx"


#include <qvaluelist.h>
typedef QValueList<QString> FieldList;
typedef QValueList<QString> ColumnList;
typedef QValueList<QString> ThreadGroupStringList;
typedef QValueList<QString> ExperimentGroupList;

typedef std::pair<Function, double> Function_double_pair;
typedef std::pair<Function, unsigned int> Function_uint_pair;
typedef std::pair<Function, uint64_t> Function_uint64_pair;


typedef QValueList <QListView *> CompareList;

#include <qsettings.h>
#include <qtimer.h>

enum CURRENTTHREADSTR_ENUM { UNKNOWN, RANK,  THREAD, PID };
enum EXPORT_TYPE_ENUM { EXPORT_TEXT, EXPORT_CSV };


class CInfoClass
{
  public:
    CInfoClass( ) { };
    CInfoClass( int _cid, QString _collector_name, int _expID, QString _host_pid_names, QString _metricStr) { cid = _cid; collector_name = _collector_name, expID = _expID; host_pid_names = _host_pid_names; metricStr = _metricStr; };
//    ~CInfoClass() { printf("CInfoClass() Destructor called!\n"); } ;
    ~CInfoClass() { } ;

    void print() { printf("%d %s %d %s %s\n", cid, collector_name.ascii(), expID, host_pid_names.ascii(), metricStr.ascii() ); };

    int cid;
    QString collector_name;
    int expID;
    QString host_pid_names;
    QString metricStr;
};
typedef QValueList<CInfoClass *> CInfoClassList;

class ColumnValueClass
{
  public:
    ColumnValueClass( ) { };
    ~ColumnValueClass( ) { };
    void init() { start_index = -1; end_index = -1; cic = NULL; };

    int start_index;
    int end_index;

    CInfoClass *cic;

    void print()
    {
      std::cout << start_index << " " << end_index << "\n";
      cic->print();
    }
};
typedef QValueList<ColumnValueClass *> ColumnValueClassList;



#define PANEL_CLASS_NAME StatsPanel   // Change the value of the define
#define DONT_FORCE_UPDATE false
#define DO_FORCE_UPDATE true

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
 
    void generatePCSAMPmodifiers();
    void generateUSERTIMEmodifiers();
    void generateOMPTPmodifiers();
    void generateHWCmodifiers();
    void generateHWCTIMEmodifiers();
    void generateHWCSAMPmodifiers();
    void generateIOmodifiers();
    void generateIOPmodifiers();
    void generateIOTmodifiers();
    void generateMEMmodifiers();
    void generateMPImodifiers();
    void generateMPIPmodifiers();
    void generateMPITmodifiers();
    void generatePTHREADSmodifiers();
    void generateFPEmodifiers();

    void generatePCSAMPAnnotationmodifiers();
    void generateUSERTIMEAnnotationmodifiers();
    void generateOMPTAnnotationmodifiers();
    void generateHWCAnnotationmodifiers();
    void generateHWCTIMEAnnotationmodifiers();
    void generateHWCSAMPAnnotationmodifiers();
    void generateIOAnnotationmodifiers();
    void generateIOPAnnotationmodifiers();
    void generateIOTAnnotationmodifiers();
    void generateMEMAnnotationmodifiers();
    void generateMPIAnnotationmodifiers();
    void generateMPIPAnnotationmodifiers();
    void generateMPITAnnotationmodifiers();
    void generatePTHREADSAnnotationmodifiers();
    void generateFPEAnnotationmodifiers();

    void updateCurrentModifierList( std::list<std::string> genericModifierList,
                                    std::list<std::string> *currentSelectedModifierList,
                                    std::map<std::string, bool> newDesiredModifierList);


    //! Adds use panel menus (if any).
    virtual bool menu(QPopupMenu* contextMenu);

    //! Routine to popup dynamic menu.
    bool createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos );

    //! Data structure that maps the expview like command to the clip that holds the command objects for the view
    std::map < std::string, InputLineObject *> cmdToClipMap;

    //! Data structure that maps the cview cluster command to the cview -c nn command, used to cache cviewcluster views
    std::map < std::string, std::string > associatedCommandMap;

    //! Data structure that maps the list -v command to the actual int list output
    std::map < std::string, std::list<int64_t> > cmdToIntListMap;

    //! Data structure that maps the list -v command to the actual std::string list output
    std::map < std::string, std::list<std::string> > cmdToStringListMap;

    //! Routine to checks to see if the expview like command has a clip associated with it.
    InputLineObject* check_for_existing_clip(std::string new_command);

    //! Routine to pair or associate the expview like command with the clip that holds the performance data
    void addClipForThisCommand(std::string command, InputLineObject* new_clip);

    //! Routine to pair or associate the cview cluster command with the cview with -c number commands
    void associateTheseCommands( std::string command, std::string cview_cluster_command );

    //! Routine to check to see if the cview cluster command is associated with the cview with -c number commands
    bool isCommandAssociatedWith( std::string input_cview_cluster_command);

    //! Routine to return the command that is associated with the cview cluster command
    std::string getAssociatedCommand( std::string input_cview_cluster_command);



    //! Routine to checks to see if the list -v command already has a list of int's associated with it.
    bool checkForExistingIntList(std::string command, std::list<int64_t>  &return_list);
    //std::list<int64_t> checkForExistingIntList(std::string new_command);

    //! Routine to pair or associate the list -v command with the list of ints that holds the list command results
    void addIntListForThisCommand(std::string command, std::list<int64_t> listToSave);


    //! Routine to checks to see if the list -v command already has a list of strings associated with it.
    //std::list<std::string> checkForExistingStringList(std::string new_command);
    bool checkForExistingStringList(std::string new_command, std::list<std::string> &return_list);

    //! Routine to pair or associate the list -v command with the list of strings that holds the list command results
    void addStringListForThisCommand(std::string command, std::list<std::string> listToSave);


    InputLineObject *statspanel_clip;
    void process_clip(InputLineObject *statspanel_clip, HighlightList *highlightList, bool dumpClipFLAG);
    GenericProgressDialog *pd;
    SelectTimeSegmentDialog *timeSegmentDialog;
    ChooseExperimentDialog *chooseExperimentDialog;
    OptionalViewsDialog *optionalViewsDialog;
    SourcePanelAnnotationDialog *sourcePanelAnnotationDialog;
    QString timeIntervalString;
    QString prevTimeIntervalString;
    QTimer *progressTimer;
    bool insertDiffColumnFLAG;
    bool absDiffFLAG;
    void insertDiffColumn(int insertAtIndex=0);
    void removeDiffColumn(int removeIndex);


    int focusedExpID;
    int focusedCompareExpID;
    std::vector<int> compareExpIDs;
    std::vector<QString> compareExpDBNames;
    std::vector<QString> compareSrcFilenames;
    ExperimentGroupList experimentGroupList;
    CInfoClassList cInfoClassList;

    void analyzeTheCView();
    int getValidExperimentIdForView();
    QString getFullPathSrcFileName(QString srcFilename, int exp_id);
    bool canWeDiff();

    SPListView *splv;

    QString lastAbout;
    QString aboutOutputString;
    QString aboutString;
    QString infoString;
    QString infoAboutString;
    QString infoAboutComparingString;
    QString infoAboutStringCompareExpIDs;
    QString originalCommand;
    QString lastCommand;
    QString lastCurrentThreadsStr;
    bool staticDataFLAG;

    QVBoxLayout *frameLayout;

    //! Layout for managing child widgets.
    QFrame *metadataAllSpaceFrame;

    //! Labels for managing information about the displayed stats
    QLabel* infoLabel;
    QLabel* infoEditHeaderLabel;
    QPushButton* infoEditHeaderMoreButton;

    //! Layout for managing information about the displayed stats
    QHBoxLayout * metadataOneLineInfoLayout;

    //! Layout for managing button and summary label information about the displayed stats
    QHBoxLayout * infoButtonAndLabelLayout;

    //! Layout for managing child widgets.
    QVBoxLayout * metadataAllSpaceLayout;

    //! The QTextEdit for managing the actual text.
    SPTextEdit *metaDataTextEdit;

    //! A pointer to the metaDataTextEdit vertical scrollbar.
    QScrollBar *vscrollbar;

    //! A pointer to the metaDataTextEdit horizontal scrollbar.
    QScrollBar *hscrollbar;

    //! A pointer to the statArea (QListView) vertical scrollbar.
    QScrollBar *vbar;

    //! A pointer to the statArea (QListView) horizontal scrollbar.
    QScrollBar *hbar;

    //! Used to clear the text and set everything back to normal
    QColor defaultColor;

    QString currentDisplayUsingTypeStr;
    enum enumDisplayType { displayUsingFunctionType, 
                           displayUsingStatementType,  
//#ifdef HAVE_DYNINST
                           displayUsingLinkedObjectType,  
                           displayUsingLoopType };
//#else
//                           displayUsingLinkedObjectType };
//#endif
    enumDisplayType  currentDisplayUsingType;
    QString getCollectorName();


    QRadioButton *vDisplayTypeFunctionRB;
    QRadioButton *vDisplayTypeStatementRB;
    QRadioButton *vDisplayTypeLinkedObjectRB;
//#ifdef HAVE_DYNINST
    QRadioButton *vDisplayTypeLoopRB;
//#endif
    QButtonGroup *vDisplayTypeBG;


    QToolBar * fileTools;
    QToolBar * metaToolBar;
    QToolButton *metadataToolButton;
    QPixmap *MoreMetadata_icon;
    QPixmap *LessMetadata_icon;
    QSplitter *splitterA;
    QSplitter *splitterB;
    SPChartForm *cf;
    ColumnList columnHeaderList;
    ChartPercentValueList skylineValues;
    ChartTextValueList skylineText;
    FieldList columnFieldList;
    ColumnValueClass columnValueClass[80];
    int *metricHeaderTypeArray;  // matches the QListView # of column entries.

    std::vector< std::pair<std::string, std::string> > demangled_mangled_vector;

    std::list<std::string> list_of_hosts;
    std::list<std::string> list_of_executables;
    std::list<std::string> list_of_appcommands;
    std::list<std::string> list_of_types;
    std::list<std::string> list_of_dbnames;
    std::list<std::string> list_of_srcfilenames;
    std::list<std::string> list_of_collectors_metrics;
    std::list<std::string> list_of_collectors_paramsval;
    std::list<std::string> list_of_collectors;
    std::list<int64_t> list_of_pids;
    std::list<int64_t> list_of_ranks;
    std::list<int64_t> list_of_threads;
    std::list<int64_t> separate_list_of_pids;
    std::list<int64_t> separate_list_of_ranks;
    std::list<int64_t> separate_list_of_threads;
    std::list<std::string> separate_list_of_ranksandpids;
    std::list<std::string> separate_list_of_ranksandthreads;
    std::list<std::string> separate_list_of_pidsandthreads;
    std::list<int64_t> rt_list_of_ranks;
    std::list<int64_t> rt_list_of_threads;
    std::list<int64_t> partial_list_of_pids;
    std::list<int64_t> partial_list_of_threads;
    std::list<int64_t> partial_list_of_ranks;
    std::list<std::string> list_of_modifiers;

    std::list<int> current_list_of_cview_exp_ids;
    std::list<std::string> current_list_of_modifiers;

    std::list<std::string> list_of_mpi_modifiers;
    std::list<std::string> current_list_of_mpi_modifiers;
    std::list<std::string> list_of_mpip_modifiers;
    std::list<std::string> current_list_of_mpip_modifiers;
    std::list<std::string> list_of_mpit_modifiers;
    std::list<std::string> current_list_of_mpit_modifiers;
    std::list<std::string> list_of_io_modifiers;
    std::list<std::string> current_list_of_io_modifiers;
    std::list<std::string> list_of_iop_modifiers;
    std::list<std::string> current_list_of_iop_modifiers;
    std::list<std::string> list_of_iot_modifiers;
    std::list<std::string> current_list_of_iot_modifiers;
    std::list<std::string> list_of_hwc_modifiers;
    std::list<std::string> current_list_of_hwc_modifiers;
    std::list<std::string> list_of_hwcsamp_modifiers;
    std::list<std::string> current_list_of_hwcsamp_modifiers;
    std::list<std::string> list_of_hwctime_modifiers;
    std::list<std::string> current_list_of_hwctime_modifiers;
    std::list<std::string> list_of_pcsamp_modifiers;
    std::list<std::string> current_list_of_pcsamp_modifiers;
    std::list<std::string> list_of_usertime_modifiers;
    std::list<std::string> current_list_of_usertime_modifiers;
    std::list<std::string> list_of_omptp_modifiers;
    std::list<std::string> current_list_of_omptp_modifiers;
    std::list<std::string> list_of_mem_modifiers;
    std::list<std::string> current_list_of_mem_modifiers;
    std::list<std::string> list_of_pthreads_modifiers;
    std::list<std::string> current_list_of_pthreads_modifiers;
    std::list<std::string> list_of_fpe_modifiers;
    std::list<std::string> current_list_of_fpe_modifiers;
    std::list<std::string> list_of_generic_modifiers;
    std::list<std::string> current_list_of_generic_modifiers;

    void setLastCommand(QString value) { lastCommand = value; } ;
    void updateThreadsList();
    void getApplicationCommand(int expID);
    void getExperimentType(int expID);
    void getDatabaseName(int expID, bool force_use_of_exp_id);
    QString getDBName(int);
    void getRankThreadPidList(int expID);
    void getRankThreadList(int expID);
    void getPartialPidList(int expID);
    void getSeparatePidList(int expID);
    void getSeparateRankList(int expID);
    void getSeparateThreadList(int expID);
    void getSeparateRanksAndPidsList(int expID);
    void getSeparateRanksAndThreadsList(int expID);
    void getSeparatePidsAndThreadsList(int expID);
    void getHostList(int expID);
    void getDemangledMangledNames(int expID);
    QString getMangledFunctionNameForCLI(QString inputFuncStr);
    QString findMangledNameForCLIcommand(std::string inputFunctionStr);
    void getExecutableList(int expID);
    void updateStatsPanelInfoHeader(int expID);
    void updateToolBarStatus(QString optionChosen);
    void updateCollectorList();
    std::list<std::string> findCollectors(int expID);
    int findExperimentID(QString command);
    void updateCollectorMetricList();
    void updateCollectorParamsValList();
    void outputAboutData(QString *data);
    void outputCLIAnnotation(QString xxxfuncName, QString xxxfileName, int xxxlineNumber);
    void outputCLIData(QString xxxfuncName, QString xxxfileName, int xxxlineNumber);
    void exportData( EXPORT_TYPE_ENUM exportTypeParam = EXPORT_CSV );
    QString getPartialExperimentInfo();
    QString getPartialExperimentInfo2(int);
    bool MPItraceFLAG;
    bool IOtraceFLAG;
    PanelContainer* thisPC;

    bool getAdvancedToolBar()
    {
       return advancedToolBar;
    }

    bool setAdvancedToolBar(bool flag)
    {
       advancedToolBar = flag;
    }


    bool isHeaderInfoAlreadyProcessed(int exp_id)
    {
#if DEBUG_StatsPanel
      printf("isHeaderInfoAlreadyProcessed, exp_id=%d, headerInfoProcessedExpID=%d\n", exp_id, headerInfoProcessedExpID);
#endif

      if (exp_id == headerInfoProcessedExpID) {
        return true;
      } else {
        return false;
      }
    }
    void setHeaderInfoAlreadyProcessed(int exp_id) {

#if DEBUG_StatsPanel
       printf("Enter setHeaderInfoAlreadyProcessed, exp_id=%d, headerInfoProcessedExpID=%d\n", exp_id, headerInfoProcessedExpID);
#endif

       headerInfoProcessedExpID = exp_id;

#if DEBUG_StatsPanel
       printf("Exit setHeaderInfoAlreadyProcessed, exp_id=%d, headerInfoProcessedExpID=%d\n", exp_id, headerInfoProcessedExpID);
#endif

    }

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
    QString sourcePanelMetricStr;
    void setCurrentMetricStr();
    QString currentUserSelectedReportStr;
    bool firstGenerateCommandCall;

    QString lastUserSelectedReportStr;
    QString originatingUserSelectedReportStr;
    QString traceAddition;

    //! Current selected chart item.
    SPListViewItem *currentItem;
    int currentItemIndex;

    QPopupMenu *threadMenu;
    QPopupMenu *modifierMenu;

    QPopupMenu *mpiModifierMenu;
    QPopupMenu *mpipModifierMenu;
    QPopupMenu *mpitModifierMenu;
    QPopupMenu *ioModifierMenu;
    QPopupMenu *iopModifierMenu;
    QPopupMenu *memModifierMenu;
    QPopupMenu *pthreadsModifierMenu;
    QPopupMenu *iotModifierMenu;
    QPopupMenu *hwcModifierMenu;
    QPopupMenu *hwcsampModifierMenu;
    QPopupMenu *hwctimeModifierMenu;
    QPopupMenu *pcsampModifierMenu;
    QPopupMenu *usertimeModifierMenu;
    QPopupMenu *omptpModifierMenu;
    QPopupMenu *fpeModifierMenu;
    QPopupMenu *genericModifierMenu;

    QPopupMenu *popupMenu;   // Pointer to the contextMenu
    QPopupMenu *mpi_menu;
    QPopupMenu *io_menu;
    QPopupMenu *iop_menu;
    QPopupMenu *mpip_menu;
    QPopupMenu *mem_menu;
    QPopupMenu *pthreads_menu;
    QPopupMenu *hwc_menu;
    QPopupMenu *hwcsamp_menu;
    QPopupMenu *hwctime_menu;
    QPopupMenu *usertime_menu;
    QPopupMenu *omptp_menu;
    QPopupMenu *pcsamp_menu;
    QPopupMenu *fpe_menu;
    QPopupMenu *generic_menu;

    QPopupMenu *experimentsMenu;

    CURRENTTHREADSTR_ENUM currentThreadsStrENUM;
    EXPORT_TYPE_ENUM exportType;
    QString currentThreadsStr;
    QString currentMenuThreadsStr;
    QString currentCollectorStr;
    QString lastCollectorStr;
    QString infoSummaryStr;
    QString collectorStrFromMenu;

    QFile *f;
    void generateToolBar(QString command, int expID);
    void generateBaseToolBar(QString command);
    QLabel *toolbar_status_label;

    // Status Panel Message Label - put out messages when data is not available
    QLabel *sml;

    int headerInfoProcessedExpID;

    CompareList compareList;
    QToolBox *csetTB;


#ifdef OLDWAY // move back to public slots:
    void headerSelected( int );
#endif // OLDWAY
  public slots:
    void displayUsingFunction();
    void displayUsingStatement();
    void displayUsingLinkedObject();
//#ifdef HAVE_DYNINST
    void displayUsingLoop();
//#endif

    void itemSelected( QListViewItem * );
    void returnPressed( QListViewItem * );
    void itemSelected( int );
    void doOption(int id);
    void exportCSVData();
    void exportTextData();
    void details();
    void originalQuery();
    void cviewQueryStatements();
    void updatePanel();
    void focusOnExp(int);
    void gotoSourceCompare1Selected(bool use_current_item = FALSE);
    void gotoSourceCompare2Selected(bool use_current_item = FALSE);
    void gotoSourceCompare3Selected(bool use_current_item = FALSE);
    void gotoSourceCompare4Selected(bool use_current_item = FALSE);
    void gotoSourceCompare5Selected(bool use_current_item = FALSE);
    void gotoSource(bool use_current_item = FALSE);
    void aboutSelected();
    void MPItraceSelected();
    void MEMtraceSelected();
    void PTHREADStraceSelected();
    void IOtraceSelected();
    void customizeExperimentsSelected();
    void clusterAnalysisSelected();
    void clearAuxiliarySelected();
    void showEventListSelected();
//#ifdef MIN_MAX_ENABLED
    void minMaxAverageSelected();
//#endif
    void manageProcessesSelected();
    void raiseManageProcessesPanel();
    void progressUpdate();
    void valueChanged(int);
    void clicked(int, int);
    void clearModifiers();

// TOOLBAR SLOTS
    void defaultViewSelected();
    void functionsSelected();
    void linkedObjectsSelected();
    void statementsSelected();
    void statementsByFunctionSelected();
    void calltreesSelected();
    void calltreesByFunctionSelected();
    void calltreesFullStackSelected();
    void calltreesFullStackByFunctionSelected();
    void tracebacksSelected();
    void tracebacksByFunctionSelected();
    void tracebacksFullStackSelected();
    void tracebacksFullStackByFunctionSelected();
    void butterflySelected();
    void infoEditHeaderMoreButtonSelected();
    void hotCallpathSelected();
    void memLeakedpathSelected();
    void memHighwaterpathSelected();
    void memUniquepathSelected();

  private slots:
    void threadSelected(int);
    void modifierSelected(int);
    void mpiModifierSelected(int);
    void mpipModifierSelected(int);
    void mpitModifierSelected(int);
    void ioModifierSelected(int);
    void iopModifierSelected(int);
    void iotModifierSelected(int);
    void hwcModifierSelected(int);
    void hwcsampModifierSelected(int);
    void hwctimeModifierSelected(int);
    void genericModifierSelected(int);
    void pcsampModifierSelected(int);
    void usertimeModifierSelected(int);
    void omptpModifierSelected(int);
    void memModifierSelected(int);
    void pthreadsModifierSelected(int);
    void fpeModifierSelected(int);
    void collectorMetricSelected(int);
    void collectorMPIReportSelected(int);
    void collectorMPIPReportSelected(int);
    void collectorMPITReportSelected(int);
    void collectorIOReportSelected(int);
    void collectorIOPReportSelected(int);
    void collectorMEMReportSelected(int);
    void collectorPTHREADSReportSelected(int);
    void collectorIOTReportSelected(int);
    void collectorHWCReportSelected(int);
    void collectorHWCSampReportSelected(int);
    void collectorHWCTimeReportSelected(int);
    void collectorUserTimeReportSelected(int);
    void collectorOMPTPReportSelected(int);
    void collectorPCSampReportSelected(int);
    void collectorFPEReportSelected(int);
    void collectorGenericReportSelected(int);
    void showStats();
    void showToolBar();
    void showInfoHeader();
    void showDiff();
    void showChart();
    void setOrientation();

    void timeSliceSelected();
    void optionalViewsCreationSelected();
    void sourcePanelAnnotationCreationSelected();

  private:
    QString getMostImportantClusterMetric(QString collector_name);

    bool matchSelectedItem( QListViewItem *item, std::string function_name );

    void updateStatsPanelData(bool processing_preference, QString command = QString::null);
    void checkForDashI();
    void updateMetadataForTimeLineView(QString intervalStr);
    void updateMetadataForCompareIndication( QString compareStr );
    void generateMPIMenu(QString collectorName);
    void generateMPIPMenu();
    void addMPIReports(QPopupMenu *menu);
    void addMPIPReports(QPopupMenu *menu);
    void generateIOMenu(QString collectorName);
    void addIOReports(QPopupMenu *menu);

    void generateIOPMenu();
    void addIOPReports(QPopupMenu *menu);

    void generateMEMMenu(QString collectorName);
    void addMEMReports(QPopupMenu *menu);
    void generatePTHREADSMenu(QString collectorName);
    void addPTHREADSReports(QPopupMenu *menu);
    void generateHWCMenu(QString collectorName);
    void addHWCReports(QPopupMenu *menu);
    void generateHWCSampMenu(QString collectorName);
    void addHWCSampReports(QPopupMenu *menu);
    void generateHWCTimeMenu(QString collectorName);
    void addHWCTimeReports(QPopupMenu *menu);
    void generateUserTimeMenu();
    void addUserTimeReports(QPopupMenu *menu);
    void generateOMPTPMenu();
    void addOMPTPReports(QPopupMenu *menu);
    void generatePCSampMenu();
    void addPCSampReports(QPopupMenu *menu);
    void generateFPEMenu();
    void addFPEReports(QPopupMenu *menu);
    void generateGenericMenu();
    QString generateCommand();
    void generateModifierMenu(QPopupMenu *, std::list<std::string>current_list, std::list<std::string>current_local_list);


    void MPIReportSelected(int);
    void IOReportSelected(int);
    void HWCReportSelected(int);
    void HWCSampReportSelected(int);
    void HWCTimeReportSelected(int);
    void resetRedirect();

    SPListViewItem *lastlvi;
    int lastIndentLevel;
    bool gotHeader;
    bool gotColumns;
    int fieldCount;
    int percentIndex;
    void raisePreferencePanel();

    int levelsToOpen;
    bool advancedToolBar;

    int steps;

    double TotalTime;
    double maxEntryBasedOnTotalTime;

    double total_percent;
    int numberItemsToDisplayInStats;
    int numberTraceItemsToDisplayInStats;
    int numberItemsToDisplayInChart;
    const char **color_names;
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


    QString getFilenameFromString( QString ); 
    QString getFunctionNameFromString( QString, QString & );
    QString getLinenumberFromString( QString );
    QString getExperimentIdFromString( QString );

    bool descending_sort;

    int expID;

    //! Flag setting, indicating if we should be displaying the toolbar for display viewing options.
    bool toolBarFLAG;

    //! Flag setting, indicating if we should be displaying the information header about the statistics.
    bool infoHeaderFLAG;

    //! Flag setting, indicating if we should be displaying the information header about the statistics.
    bool metaDataTextEditFLAG;

    //! Flag setting, indicating if we should be displaying the statistics.
    bool statsFLAG;

    //! Flag setting, indicating if we should be displaying the text in graphics.
    int textENUM;
  
    //! Flag setting, indicating if we should be displaying the chart.
    bool chartFLAG;

    CollectorListObject *clo;


    QString findSelectedFunction();
    QString selectedFunctionStr;

    SPListViewItem *MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, SPListViewItem *arg2, SPListViewItem *arg3, QString *strings);

    SPListViewItem *MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, QListView *arg2, SPListViewItem *arg3, QString *strings);

    SourceObject *lookUpFileHighlights(QString filename, QString lineNumberStr, HighlightList *highlightList, QString highlightMetricStr);

};
#endif // STATSPANEL_H
