////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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

//
// To enable debuging uncomment define DEBUG_StatsPanel statement
//
//#define DEBUG_StatsPanel 1
//


#include "StatsPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include "CollectorListObject.hxx"
#include "CollectorMetricEntryClass.hxx"

#include <qapplication.h>
#include <qheader.h>
#include <qregexp.h>

#include <qvaluelist.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qdockwindow.h>
#include <qdockarea.h>
#include <qbutton.h>
#include <qpushbutton.h>

#include <qpixmap.h>
#include "functions.xpm"
#include "linkedObjects.xpm"
#include "statements.xpm"
#include "statementsByFunction.xpm"
#include "calltrees.xpm"
#include "calltreesByFunction.xpm"
#include "calltreesfull.xpm"
#include "calltreesfullByFunction.xpm"
#include "tracebacks.xpm"
#include "tracebacksByFunction.xpm"
#include "tracebacksfull.xpm"
#include "tracebacksfullByFunction.xpm"
#include "butterfly.xpm"
#include "meta_information_plus.xpm"
#include "meta_information_minus.xpm"
#include "compare_and_analyze.xpm"
#include "custom_comparison.xpm"
#include "update_icon.xpm"
#include "load_balance_icon.xpm"
#include "event_list_icon.xpm"
#include "clear_auxiliary.xpm"

class MetricHeaderInfo;
class QPushButton;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;
#include "CLIInterface.hxx"

#include "ManageProcessesPanel.hxx"

#define CLUSTERANALYSIS 1

#include "GenericProgressDialog.hxx"

// These are the pie chart colors..
static char *hotToCold_color_names[] = { 
  "red", 
  "magenta",
  "skyblue",
  "cyan",
  "green",
  "yellow",
  "gray",
  "lightGray",
  "pink",
  "orange"
  "lightblue",
  "lightred",
  "lightcyan",
  "lightgreen",
};
static char *coldToHot_color_names[] = { 
  "lightgreen",
  "lightcyan",
  "lightred",
  "lightblue",
  "orange",
  "pink",
  "lightGray"
  "gray",
  "yellow",
  "green",
  "cyan",
  "skyblue",
  "magenta",
  "red", 
};
#define MAX_COLOR_CNT 14
static char *blue_color_names[] = { 
  "blue", 
  "blue", 
  "blue", 
};


#define PTI "Present Trace Information"

#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "PrepareToRerunObject.hxx"
#include "FocusObject.hxx"
#include "FocusCompareObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"

#include "preference_plugin_info.hxx"
#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;


class AboutOutputClass : public ss_ostream
{
  public:
    StatsPanel *sp;
    void setSP(StatsPanel *_sp) { sp = _sp;line_buffer = QString::null; };
    QString line_buffer;
  private:
    virtual void output_string (std::string s)
    {
       line_buffer += s.c_str();
       if( QString(s).contains("\n") )
       {
         QString *data = new QString(line_buffer);
         sp->outputAboutData(data);
         line_buffer = QString::null;
       }
    }
    virtual void flush_ostream ()
    {
      qApp->flushX();
    }
};


/*! Create a Stats Panel.
*/
StatsPanel::StatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor entered\n");
#endif
  thisPC = pc;
  setCaption("StatsPanel");
  timeSegmentDialog = NULL;;

  metadataAllSpaceFrame = NULL;
  metadataAllSpaceLayout = NULL;
  metadataOneLineInfoLayout = NULL;

  statspanel_clip = NULL;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif
  progressTimer = NULL;
  pd = NULL;

  IOtraceFLAG = FALSE;
  MPItraceFLAG = FALSE;

  insertDiffColumnFLAG = FALSE;
  focusedExpID = -1;
  experimentGroupList.clear();

  currentThread = NULL;
  currentCollector = NULL;
  currentItem = NULL;
  currentItemIndex = 0;
  lastlvi = NULL;
  lastIndentLevel = 0;
  gotHeader = FALSE;
  fieldCount = 0;
  percentIndex = -1;
  gotColumns = FALSE;
  aboutOutputString = QString::null;
  aboutString = QString::null;

  // for the metadata information header
  // uses some of what is in the aboutString string but format is different
  infoString = QString::null;
  infoAboutString = QString::null;
  infoAboutComparingString = QString::null;
  infoAboutStringCompareExpIDs = QString::null;

  lastAbout = QString::null;
  timeIntervalString = QString::null;
  prevTimeIntervalString = QString::null;

  // In an attempt to optimize the update of this panel;
  // If the data file is static (i.e. read from a file or 
  // the processes status is terminated) and the command is
  // the same, don't update this panel. 
  originalCommand = QString::null;
  lastCommand = QString::null;
  staticDataFLAG = false;

#ifdef DEBUG_StatsPanel
  printf("currentItemIndex initialized to 0\n");
#endif

  f = NULL;
  modifierMenu = NULL;
  experimentsMenu = NULL;

  mpiModifierMenu = NULL;
  mpitModifierMenu = NULL;
  ioModifierMenu = NULL;
  iotModifierMenu = NULL;
  hwcModifierMenu = NULL;
  hwctimeModifierMenu = NULL;
  pcsampModifierMenu = NULL;
  usertimeModifierMenu = NULL;
  fpeModifierMenu = NULL;

  mpi_menu = NULL;
  io_menu = NULL;
  hwc_menu = NULL;
  hwctime_menu = NULL;
  pcsamp_menu = NULL;
  usertime_menu = NULL;
  fpe_menu = NULL;

  list_of_modifiers.clear(); // This is the global known list of modifiers.

  list_of_mpi_modifiers.clear();
  current_list_of_mpi_modifiers.clear();  // This is this list of user selected modifiers.
  current_list_of_mpit_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_io_modifiers.clear();
  current_list_of_io_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_iot_modifiers.clear();
  current_list_of_iot_modifiers.clear();  // This is this list of user selected modifiers.
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel() constructor CLEARING current_list_of_iot_modifiers\n");
#endif
  list_of_hwc_modifiers.clear();
  current_list_of_hwc_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_hwctime_modifiers.clear();
  current_list_of_hwctime_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_pcsamp_modifiers.clear();
  current_list_of_pcsamp_modifiers.clear();  // This is this list of user selected modifiers.
  list_of_usertime_modifiers.clear();
  current_list_of_usertime_modifiers.clear();  // This is this list of user selected modifiers.

  list_of_fpe_modifiers.clear();
  current_list_of_fpe_modifiers.clear();  // This is this list of user selected modifiers.

  current_list_of_modifiers.clear();  // This is this list of user selected modifiers.
  selectedFunctionStr = QString::null;
  threadMenu = NULL;
  currentMetricStr = QString::null;
  currentUserSelectedReportStr = QString::null;
  traceAddition = QString::null;
  metricHeaderTypeArray = NULL;
  currentThreadStr = QString::null;
  currentCollectorStr = QString::null;
  lastCollectorStr = QString::null;
  infoSummaryStr = QString::null;
  collectorStrFromMenu = QString::null;
  groupID = ao->int_data;
  expID = -1;
  descending_sort = true;
  TotalTime = 0;

  if( ao->loadedFromSavedFile == TRUE ) {

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel static data!!!\n");
#endif

    staticDataFLAG = TRUE;
  }

  splitterA = new QSplitter( getBaseWidgetFrame(), "splitterA");
  splitterA->setCaption("StatsPanelSplitterA");
  splitterA->setOrientation( QSplitter::Vertical );
//  splitterA->setOrientation( QSplitter::Horizontal );

  int Awidth = pc->width();
  int Aheight = pc->height();

#ifdef DEBUG_StatsPanel
  printf("in Splitter section of code, Awidth=%d, Aheight=%d\n", Awidth, Aheight);
#endif

  QValueList<int> AsizeList;
  AsizeList.clear();
  if( splitterA->orientation() == QSplitter::Vertical ) {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is vertical, (Aheight-(int)(Aheight/5))=%d, (Aheight/5)=%d\n", (Aheight-(int)(Aheight/5)), (Aheight/5));
#endif
    AsizeList.push_back((int)(Aheight/5));
    AsizeList.push_back(Aheight-(int)(Aheight/5));
  } else {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is horizontal, (Awidth/5)=%d, (Awidth-(int)(Awidth/5))=%d\n", (Awidth/5), (Awidth-(int)(Awidth/5)));
#endif
    AsizeList.push_back((int)(Awidth/5));
    AsizeList.push_back(Awidth-(int)(Awidth/5));
  }
  splitterA->setSizes(AsizeList);
// NEW HEADER CODE

  metadataAllSpaceFrame = new QFrame( splitterA, "metadataAllSpaceFrame" );
  metadataAllSpaceLayout = new QVBoxLayout( metadataAllSpaceFrame );

  metadataOneLineInfoLayout = new QHBoxLayout( metadataAllSpaceLayout, 0, "metadataOneLineInfoLayout" );
//  metadataOneLineInfoLayout->setMargin(1);

  metadataAllSpaceFrame->setMinimumSize( QSize(0,0) );
  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
#ifdef DEBUG_StatsPanel
  printf("stats panel metadataAllSpaceFrame->height()=%d\n", metadataAllSpaceFrame->height());
#endif
  metadataAllSpaceFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

#if MORE_BUTTON
  infoEditHeaderMoreButton = new QPushButton( metadataAllSpaceFrame, "infoEditHeaderMoreButton" );
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
  infoEditHeaderMoreButton->setEnabled(TRUE);
  infoEditHeaderMoreButton->setMinimumSize( QSize(0,0) );
  infoEditHeaderMoreButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, infoEditHeaderMoreButton->sizePolicy().hasHeightForWidth() ) );
#else
  metaToolBar = new QToolBar( QString("label"), getPanelContainer()->getMainWindow(), metadataAllSpaceFrame, "file operations" );
  metaToolBar->setOrientation( Qt::Horizontal );
  metaToolBar->setLabel( "Metadata Operations" );

//  QPixmap *MoreMetadata_icon = new QPixmap(meta_information_plus_xpm);
//  new QToolButton(*MoreMetadata_icon, "Show More Experiment Metadata", 
//                  QString::null, this, SLOT( infoEditHeaderMoreButtonSelected()), 
//                  metaToolBar, "show more experiment metadata");
#endif

  infoEditHeaderLabel = new QLabel( metadataAllSpaceFrame, "info label", 0 );
  infoEditHeaderLabel->setCaption("StatsPanel: info label");
  infoEditHeaderLabel->setPaletteBackgroundColor( QColor("skyblue").light(145) );
//  infoEditHeaderLabel->setPaletteBackgroundColor( QColor("blue").light(185));
#ifdef DEBUG_StatsPanel
  printf("stats panel 2nd infoEditHeaderLabel->height()=%d\n", infoEditHeaderLabel->height());
#endif

  QString label_text = QString("There is no known information about this experiment's statistics available\n");

  infoEditHeaderLabel->setText(label_text);
  infoEditHeaderLabel->setMinimumSize( QSize(0,0) );
  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
  infoEditHeaderLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );
  QToolTip::add( infoEditHeaderLabel, tr( "This quick summary line shows partial information about the executable(s),\nhost(s), and process(es) used in creating the experiment performance\ndata displayed below.  To see more complete information use the\nexperiment metadata icon above to toggle between the\nsummary and expanded information." ) );

#ifdef DEBUG_StatsPanel
  printf("stats panel 3rd infoEditHeaderLabel->height()=%d\n", infoEditHeaderLabel->height());
#endif

  infoButtonAndLabelLayout = new QHBoxLayout( 0, 0, 0, "infoButtonAndLabelLayout");
#if MORE_BUTTON
  infoButtonAndLabelLayout->addWidget(infoEditHeaderMoreButton);
#endif
  infoButtonAndLabelLayout->addWidget(infoEditHeaderLabel);
  metadataOneLineInfoLayout->addLayout( infoButtonAndLabelLayout );

//#ifdef TEXT
  metaDataTextEdit = new SPTextEdit( this, metadataAllSpaceFrame );
  metaDataTextEdit->setCaption("SourcePanel: SPTextEdit");
  metaDataTextEdit->setHScrollBarMode( QScrollView::AlwaysOn );
  metaDataTextEdit->setVScrollBarMode( QScrollView::AlwaysOn );
  metadataAllSpaceLayout->addWidget( metaDataTextEdit );
  metaDataTextEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  addWhatsThis(metaDataTextEdit, this);

  metaDataTextEdit->setTextFormat(PlainText);  // This makes one para == 1 line.
  metaDataTextEdit->setReadOnly(TRUE);
  metaDataTextEdit->setWordWrap(QTextEdit::NoWrap);
  vscrollbar = metaDataTextEdit->verticalScrollBar();
  hscrollbar = metaDataTextEdit->horizontalScrollBar();
  if( vscrollbar )
  {
    connect( vscrollbar, SIGNAL(valueChanged(int)),
           this, SLOT(valueChanged(int)) );
  }

  defaultColor = metaDataTextEdit->color();

  connect( metaDataTextEdit, SIGNAL(clicked(int, int)),
           this, SLOT(clicked(int, int)) );

  QValueList<int> metaDataTextEditsizeList;
  metaDataTextEditsizeList.clear();
  int metaDataTextEditwidth = pc->width();
  int metaDataTextEditleft_side_size = (int)(metaDataTextEditwidth/4);
  if( DEFAULT_CANVAS_WIDTH < metaDataTextEditleft_side_size )
  {
    metaDataTextEditleft_side_size = DEFAULT_CANVAS_WIDTH;
  }
  metaDataTextEditsizeList.push_back( metaDataTextEditleft_side_size );
  metaDataTextEditsizeList.push_back( metaDataTextEditwidth-metaDataTextEditleft_side_size );
//jeg 9-1807  metaDataTextEdit->show();
  metaDataTextEdit->setFocus();
  metaDataTextEdit->hide();
  metaDataTextEditFLAG = FALSE;
//#endif

//#endif

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 0, 0, getName() );
//  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

// END NEW HEADER CODE

  splitterB = new QSplitter( splitterA, "splitterB");
  splitterB->setCaption("StatsPanelSplitterB");
  splitterB->setOrientation( QSplitter::Horizontal );
#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB created as Horizontal\n");
#endif

  cf = new SPChartForm(this, splitterB, getName(), 0);
  cf->setCaption("SPChartFormIntoSplitterA");

  splv = new SPListView(this, splitterB, getName(), 0);
  splv->setSorting ( 0, FALSE );

  sml =new QLabel(splitterB,"stats_message_label");
  sml->setText("There were no data samples for this experiment execution.\nPossible reasons for this could be:\n   The executable being run didn't run long enough to record performance data.\n   The type of performance data being gathered may not be present in the executable being executed.\n   The executable was not compiled with debug symbols enabled (-g option or variant).\n");
  // Hide this and only show it when we don't see any performance data samples
  sml->hide();


#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB being use as chart form holder\n");
#endif


#ifdef OLDWAY
  QHeader *header = splv->header();
  header->setMovingEnabled(FALSE);
  header->setClickEnabled(TRUE);
  connect( header, SIGNAL(clicked(int)), this, SLOT( headerSelected( int )) );
#endif // OLDWAY

  connect( splv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

  connect( splv, SIGNAL(returnPressed(QListViewItem *)), this, SLOT( returnPressed( QListViewItem* )) );

  int Bwidth = pc->width();
  int Bheight = pc->height();

#ifdef DEBUG_StatsPanel
  printf("in Splitter section of code, Bwidth=%d, Bheight=%d\n", Bwidth, Bheight);
#endif

  QValueList<int> BsizeList;
  BsizeList.clear();
  if( splitterB->orientation() == QSplitter::Vertical ) {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is vertical, (Bheight-(int)(Bheight/4)=%d, (Bheight/4)=%d\n", (Bheight-(int)(Bheight/4), (Bheight/4)));
#endif
    BsizeList.push_back((int)(Bheight/4));
    BsizeList.push_back(Bheight-(int)(Bheight/4));
  } else {
#ifdef DEBUG_StatsPanel
    printf("in Splitter section of code, splitter is horizontal, (Bwidth-(int)(Bwidth/4)=%d, (Bwidth/4)=%d\n", (Bwidth-(int)(Bwidth/4), (Bwidth/4)));
#endif
    BsizeList.push_back((int)(Bwidth/4));
    BsizeList.push_back(Bwidth-(int)(Bwidth/4));
  }
  splitterB->setSizes(BsizeList);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel:: splitterB is set to setSizes(BsizeList)\n");
#endif

  bool toolbarFLAG = getPreferenceShowToolbarCheckBox();

// Begin - Move to Panel.cxx
  fileTools = new QToolBar( QString("label"), getPanelContainer()->getMainWindow(), (QWidget *)getBaseWidgetFrame(), "file operations" );
  fileTools->setOrientation( Qt::Horizontal );
  fileTools->setLabel( "File Operations" );
//  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );
  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );
// End - Move to Panel.cxx

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel, addWidget(fileTools=0x%x)\n", fileTools);
#endif

  frameLayout->addWidget(fileTools);
//  frameLayout->addWidget(metadataAllSpaceFrame);

  // Show statistics metadata information initially

  if( getPreferenceShowMetadataCheckBox() == TRUE ) {
    infoHeaderFLAG = TRUE;
    metadataAllSpaceFrame->show();
  } else {
    infoHeaderFLAG = FALSE;
    metadataAllSpaceFrame->hide();
  }

  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
    toolBarFLAG = TRUE;
    fileTools->show();
  } else {
    toolBarFLAG = FALSE;
    fileTools->hide();
  }

//  frameLayout->addLayout( metadataAllSpaceLayout );
  frameLayout->addWidget( splitterA );

  if( pc->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() ) {
    chartFLAG = TRUE;
    cf->show();
  } else {
    chartFLAG = FALSE;
    cf->hide();
  }

if( !getChartTypeComboBox() ) {
  cf->setChartType((ChartType)0);
} else {
  cf->setChartType((ChartType)getChartTypeComboBox());
}
  statsFLAG = TRUE;
  splv->show();

  splitterA->show();

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), groupID);
  setName(name_buffer);

  // Initialize the experiment id for the info header already processed
  // to something that can't occur as the first value.
  setHeaderInfoAlreadyProcessed(-1);

  // ----------------------------------------------------
  // SIGNAL SETUP AREA
  // ---------------------------------------------------

#if DEBUG_INTRO
  printf("StatsPanel::StatsPanel() constructor, start connect section of code\n");
#endif
#if MORE_BUTTON
  connect( infoEditHeaderMoreButton, SIGNAL( clicked() ), this, SLOT( infoEditHeaderMoreButtonSelected() ) );
#endif

}



/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanel::~StatsPanel()
{
  // Delete anything you new'd from the constructor.
  nprintf( DEBUG_CONST_DESTRUCT ) ("  StatsPanel::~StatsPanel() destructor called\n");
#ifdef DEBUG_StatsPanel
  printf("  StatsPanel::~StatsPanel() destructor called\n");
#endif


  // We must reset the directing of output, otherwise the cli goes nuts
  // trying to figure out where the output is suppose to go.
  resetRedirect();


  // We allocated a Collect, we must delete it.   Otherwise the framework
  // issues a warning on exit.
  if( currentCollector ) {

#ifdef DEBUG_StatsPanel
   printf("Destructor delete the currentCollector\n");
#endif

    delete currentCollector;
  }

#ifdef DEBUG_StatsPanel
  printf("  StatsPanel::~StatsPanel() destructor finished\n");
#endif

}

/*! The user clicked.  -unused. */
void
StatsPanel::clicked(int para, int offset)
{
  nprintf(DEBUG_PANELS) ("You clicked?\n");
}


/*! The value changed... That means we've scrolled.   
    Should not have to do anything for the statspanel */
void
StatsPanel::valueChanged(int passed_in_value)
{
#ifdef DEBUG_SourcePanel
   printf("StatsPanel::valueChanged(%d), ignoring\n", passed_in_value );
#endif
   return;

}


void StatsPanel::infoEditHeaderMoreButtonSelected()
{

#if DEBUG_INTRO
  printf("Enter StatsPanel::infoEditHeaderMoreButtonSelected()\n");
#endif
//#ifdef TEXT
  if (metaDataTextEditFLAG) {
    metaDataTextEdit->hide();
    metaDataTextEditFLAG = FALSE;
#if MORE_BUTTON
    infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
    infoEditHeaderMoreButton->setEnabled(TRUE);
#else
    metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
    metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
    QToolTip::add( metadataToolButton, tr( "Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
#endif
  } else {
    metaDataTextEdit->setCursorPosition(0, 0);
    metaDataTextEdit->show();
    metaDataTextEditFLAG = TRUE;
#if MORE_BUTTON
    infoEditHeaderMoreButton->setText( tr( "Less Metadata" ) );
    infoEditHeaderMoreButton->setEnabled(TRUE);
#else
    metadataToolButton->setIconSet( QIconSet(*LessMetadata_icon));
    metadataToolButton->setIconText(QString("Show Less Experiment Metadata"));
    QToolTip::add( metadataToolButton, tr( "Push to hide the expanded experiment metadata information." ) );
#endif
  } 
//#endif
  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
#if MORE_BUTTON
// maybe move the above into here - I did but commented out for now
//  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );
//  infoEditHeaderLabel->resize( infoEditHeaderLabel->sizeHint() );
#else
  metaToolBar->update();
#endif

}



void
StatsPanel::raiseManageProcessesPanel()
{
#ifdef DEBUG_MPPanel
  printf("StatsPanel::raiseManageProcessesPanel(), expID=%d\n", expID);
#endif

  QString name = QString("ManageProcessesPanel [%1]").arg(expID);

  Panel *manageProcessesPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessesPanel ) {
    getPanelContainer()->raisePanel(manageProcessesPanel);
  }

}



void
StatsPanel::languageChange()
{
  // Set language specific information here.

#if MORE_BUTTON
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
  infoEditHeaderMoreButton->setEnabled(TRUE);
  QToolTip::add( infoEditHeaderMoreButton, tr( "Push for more or less complete experiment metadata (information about the experiment performance data)." ) );
#else
  metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
  QToolTip::add( metadataToolButton, tr( "Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
  metaToolBar->update();
#endif
}



/*! When a message has been sent (from anyone) and the message broker is
    notifying panels that they may want to know about the message, this is the
    function the broker notifies.   The listener then needs to determine
    if it wants to handle the message.
    \param msg is the incoming message.
    \return 0 means you didn't do anything with the message.
    \return 1 means you handled the message.
 */
#include "SaveAsObject.hxx"
int 
StatsPanel::listener(void *msg)
{
  PreferencesChangedObject *pco = NULL;

#ifdef DEBUG_StatsPanel
   printf("Enter StatsPanel::listener(&msg = 0x%x)\n", &msg );
#endif

  MessageObject *msgObject = (MessageObject *)msg;
  nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() msg->msgType = (%s)\n", msgObject->msgType.ascii() );

  if( msgObject->msgType == getName() && recycleFLAG == TRUE )
  {
    nprintf(DEBUG_MESSAGES) ("StatsPanel::listener() interested!\n");
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener-1-(%s)\n", msgObject->msgType.ascii() );
   printf("StatsPanel::listener-1-(getName()=%s)\n", getName() );
#endif

    getPanelContainer()->raisePanel(this);
    return 1;
  }
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener-2-(%s)\n", msgObject->msgType.ascii() );
#endif

 if(  msgObject->msgType  == "PrepareToRerun" )
  {
   // ----------------------------
   // ---------------------------- PREPARE-TO-RERUN
   // ----------------------------
   // In an attempt to optimize the update of this panel;
   // If the data file is static (i.e. read from a file or
   // the processes status is terminated) and the command is
   // the same, don't update this panel.
   // FOR RERUN - need to clear lastCommand so it does update
#ifdef DEBUG_StatsPanel
    printf("StatsPanel in PrepareToRerun\n");
#endif // DEBUG_StatsPanel
   recycleFLAG = FALSE;
   originalCommand = QString::null;
   lastCommand = QString::null;
   staticDataFLAG = false;
   raiseManageProcessesPanel();
   ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
   if( eo && eo->FW() ) {
     Experiment *fw_experiment = eo->FW();
#ifdef DEBUG_StatsPanel
     printf("StatsPanel in PrepareToRerun, fw-experiment->rerun_count=%d\n", fw_experiment->getRerunCount());
#endif // DEBUG_StatsPanel
   }

  } else if(  msgObject->msgType  == "FocusObject" && recycleFLAG == TRUE ) {
   // ---------------------------- 
   // ---------------------------- FOCUS-OBJECT
   // ----------------------------
#ifdef DEBUG_StatsPanel
    printf("StatsPanel got a new FocusObject\n");
#endif // DEBUG_StatsPanel
    FocusObject *msg = (FocusObject *)msgObject;
#ifdef DEBUG_StatsPanel
// msg->print();
#endif // DEBUG_StatsPanel
    expID = msg->expID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, FocusObject, B: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

    if( msg->host_pid_vector.size() == 0 && !msg->pidString.isEmpty() )
    { // Soon to be obsoleted
      currentThreadsStr = msg->pidString;
      currentThreadGroupStrList.clear();
      QString ws = msg->pidString;
      int cnt = ws.contains(",");
      if( cnt > 1 )
      {
        for(int i=0;i<=cnt;i++)
        {
          currentThreadGroupStrList.push_back( ws.section(",", i, i) ); 
        }
      } else
      {
        if( !ws.isEmpty() )
        {
          currentThreadGroupStrList.push_back( ws );
        }
      }
    } else
    {
      currentThreadGroupStrList.clear();
      currentThreadsStr = QString::null;
if( msg->descriptionClassList.count() > 0 ) {

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::listener, FocusObject, Focusing with the new (more robust) syntax.\n");
#endif // DEBUG_StatsPanel

  for( QValueList<DescriptionClassObject>::iterator it = msg->descriptionClassList.begin(); it != msg->descriptionClassList.end(); it++)
    {
      DescriptionClassObject dco = (DescriptionClassObject)*it;
      if( !dco.rid_name.isEmpty() )
      {
        currentThreadsStr += QString(" -h %1 -r %2").arg(dco.host_name).arg(dco.rid_name);
      } else if( !dco.tid_name.isEmpty() )
      {
        currentThreadsStr += QString(" -h %1 -t %2").arg(dco.host_name).arg(dco.tid_name);
      } else // pid...
      {
        currentThreadsStr += QString(" -h %1 -p %2").arg(dco.host_name).arg(dco.pid_name);
      }
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener, FocusObject, Building currentThreadStr.ascii()=%s\n", currentThreadsStr.ascii() );
#endif // DEBUG_StatsPanel
     
    }
} else
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener, FocusObject, Here in StatsPanel::listener()\n");
  msg->print();
#endif // DEBUG_StatsPanel

      currentThreadGroupStrList.clear();
      currentThreadsStr = QString::null;
      std::vector<HostPidPair>::const_iterator sit = msg->host_pid_vector.begin();
      for(std::vector<HostPidPair>::const_iterator
                      sit = msg->host_pid_vector.begin();
                      sit != msg->host_pid_vector.end(); ++sit)
      {
        if( sit->first.size() ) {
          currentThreadsStr += QString(" -h %1 -p %2").arg(sit->first.c_str()).arg(sit->second.c_str() );
        } else {
          currentThreadsStr += QString(" -p %1").arg(sit->second.c_str() );
        }
        currentThreadGroupStrList.push_back( sit->second.c_str() );
      }
}
    }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener, FocusObject, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif // DEBUG_StatsPanel

// Begin determine if there's mpi stats
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() )
      {
        Experiment *fw_experiment = eo->FW();
        CollectorGroup cgrp = fw_experiment->getCollectors();

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusObject, Is says you have %d collectors.\n", cgrp.size() );
#endif // DEBUG_StatsPanel

        if( cgrp.size() == 0 )
        {
          fprintf(stderr, "There are no known collectors for this experiment.\n");
        }
        for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
        {
          Collector collector = *ci;
          Metadata cm = collector.getMetadata();
          QString name = QString(cm.getUniqueId().c_str());

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusObject, B: Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
#endif // DEBUG_StatsPanel

        }
      }
    }
    catch(const std::exception& error)
    { 
      std::cerr << std::endl << "Error: "
                << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                "Unknown runtime error." : error.what()) << std::endl
                << std::endl;
      QApplication::restoreOverrideCursor( );
      return FALSE;
    }
// End determine if there's mpi stats

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener FocusObject, call updateStatsPanelData  Do we need to update?\n");
#endif // DEBUG_StatsPanel

    updateStatsPanelData(DONT_FORCE_UPDATE);
    if( msg->raiseFLAG == TRUE ) {
      getPanelContainer()->raisePanel(this);
    }
// now focus a source file that's listening....

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusObject, Now focus the source panel, if it's up..\n");
#endif // DEBUG_StatsPanel

    //First get the first item...
    QListViewItemIterator it( splv );
    QListViewItem *item = *it;

  // Now call the match routine, this should focus any source panels, if the 
  // focus source panel preference is set

  if( getPreferencesFocusSourcePanel() &&  
      item && matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) )) {

#ifdef DEBUG_StatsPanel
         printf("StatsPanel::listener, FocusObject, match\n");
#endif // DEBUG_StatsPanel

        return 1;
    } else {
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::listener, FocusObject, no match\n");
#endif // DEBUG_StatsPanel
        return 0;
    }

   // ---------------------------- 
   // ---------------------------- FOCUS-COMPARE-OBJECT
   // ----------------------------
  } else if(  msgObject->msgType  == "FocusCompareObject" && recycleFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel got a new FocusCompareObject expID was %d\n", expID);
#endif // DEBUG_StatsPanel

    FocusCompareObject *msg = (FocusCompareObject *)msgObject;

#ifdef DEBUG_StatsPanel
   msg->print();
#endif // DEBUG_StatsPanel

    if( !msg->compare_command.startsWith("cview -c") ) {
      expID = msg->expID;

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, FocusCompareObject, C: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

    }

    if( !msg->compare_command.isEmpty()  ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener() FocusCompareObject, calling updateStatsPanelData, command=(%s)\n", msg->compare_command.ascii() );
#endif // DEBUG_StatsPanel

      updateStatsPanelData(DONT_FORCE_UPDATE, msg->compare_command);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, FocusCompareObject, StatsPanel::listener() called \n");
#endif // DEBUG_StatsPanel

    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener() FocusCompareObject, raise this panel? msg->raiseFLAG=%d\n",msg->raiseFLAG);
#endif // DEBUG_StatsPanel

    if( msg->raiseFLAG == TRUE ) {

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener() FocusCompareObject, raise this panel.. \n");
#endif // DEBUG_StatsPanel

      getPanelContainer()->raisePanel(this);

    }
   // ---------------------------- 
   // ---------------------------- UPDATE-EXPERIMENT-DATA-OBJECT
   // ----------------------------
  } else if(  msgObject->msgType  == "UpdateExperimentDataObject" ) {

    UpdateObject *msg = (UpdateObject *)msgObject;

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT msgType == UpdateExperimentDataObject\n");
   printf("StatsPanel::listener,  UPDATE-EXPERIMENT-DATA-OBJECTmsg->expID=%d\n, msg->expID");
#endif // DEBUG_StatsPanel

    if( msg->expID == -1 ) {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener,  UPDATE-EXPERIMENT-DATA-OBJECT We got the command=(%s)\n", msg->experiment_name.ascii() );
#endif // DEBUG_StatsPanel

      QString command = msg->experiment_name;

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT We got the command=(%s), calling updateStatsPanelData\n", command.ascii() );
#endif // DEBUG_StatsPanel

      updateStatsPanelData(DONT_FORCE_UPDATE, command);

//Hack - NOTE: You may have to snag the expID out of the command.
#ifdef OLDWAY
expID = groupID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, D: expID = %d\n", expID);
#endif // DEBUG_StatsPanel
updateCollectorList();
#else // OLDWAY
int start_index = command.find("-x");
if( start_index != -1 )
{
  QString s = command.mid(start_index+3);
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT Got a -x in the command s=(%s)\n", s.ascii() );
#endif // DEBUG_StatsPanel
  int end_index = s.find(" ");
  if( end_index == -1 )
  {
    end_index == 99999;
  }

  QString exp_x = s.mid(0, end_index);

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT exp_x=%s\n", exp_x.ascii() );
#endif // DEBUG_StatsPanel

  expID = exp_x.toInt();

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT E: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

   updateCollectorList();
} else {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT no -x in the command\n");
    expID = groupID;
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT G: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

}
#endif // OLDWAY
      return(1);
    }
    

    expID = msg->expID;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT H: expID = %d\n", expID);
#endif // DEBUG_StatsPanel

    // Begin determine if there's mpi stats
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() ) {

        Experiment *fw_experiment = eo->FW();
        CollectorGroup cgrp = fw_experiment->getCollectors();

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT It says you have %d collectors.\n", cgrp.size() );
#endif // DEBUG_StatsPanel

        if( cgrp.size() == 0 ) {
          fprintf(stderr, "There are no known collectors for this experiment.\n");
        }

        for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
        {
          Collector collector = *ci;
          Metadata cm = collector.getMetadata();
          QString name = QString(cm.getUniqueId().c_str());
        }
      }
    }
    catch(const std::exception& error)
    { 
      std::cerr << std::endl << "Error: "
                << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                "Unknown runtime error." : error.what()) << std::endl
                << std::endl;
      QApplication::restoreOverrideCursor( );
      return FALSE;
    }
// End determine if there's mpi stats

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT Call updateStatsPanelData() \n");
#endif // DEBUG_StatsPanel

    // Raise or Create a new manage processes panel for loading saved data files
    manageProcessesSelected();

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT SAVED DATA LOAD, msg->raiseFLAG =%d, calling updateStatsPanelData \n", msg->raiseFLAG );
#endif // DEBUG_StatsPanel

    updateStatsPanelData(DONT_FORCE_UPDATE);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, UPDATE-EXPERIMENT-DATA-OBJECT msg->raiseFLAG =%d \n", msg->raiseFLAG );
#endif // DEBUG_StatsPanel

    if( msg->raiseFLAG )
    {
      getPanelContainer()->raisePanel((Panel *)this);
    }
   // ---------------------------- 
   // ---------------------------- PREFERENCE-CHANGED-OBJECT
   // ----------------------------
  } else if( msgObject->msgType == "PreferencesChangedObject" ) {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getPreferenceShowToolbarCheckBox()=%d\n",
           getPreferenceShowToolbarCheckBox() );
#endif // DEBUG_StatsPanel

    if( getPreferenceShowToolbarCheckBox() == TRUE ) {
      fileTools->show();
    } else {
      fileTools->hide();
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getPreferenceShowMetadataCheckBox()=%d\n",
           getPreferenceShowMetadataCheckBox() );
#endif // DEBUG_StatsPanel

    if( getPreferenceShowMetadataCheckBox() == TRUE ) {
      infoHeaderFLAG = TRUE;
    } else {
      infoHeaderFLAG = FALSE;
    }


#ifdef DEBUG_StatsPanel
    printf("StatsPanel::listener, PREFERENCE-CHANGED-OBJECT, getChartTypeComboBox()=%d\n",
           getChartTypeComboBox() );
#endif // DEBUG_StatsPanel

    if( !getChartTypeComboBox() ) {
      cf->setChartType((ChartType)0);
    } else {
      cf->setChartType((ChartType)getChartTypeComboBox());
    }


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::listener() PREFERENCE-CHANGED-OBJECT, about to call updateStatsPanelData\n"  );
#endif
    updateStatsPanelData(DO_FORCE_UPDATE);

   // ---------------------------- 
   // ---------------------------- SAVE-AS-OBJECT
   // ----------------------------
  } else if( msgObject->msgType == "SaveAsObject" )
  {
    SaveAsObject *sao = (SaveAsObject *)msg;
#ifdef DEBUG_StatsPanel
    printf("StatsPanel!!!!! Save as!\n");
#endif // DEBUG_StatsPanel
    if( !sao ) {
      return 0;  // 0 means, did not act on message.
    }

//    exportData(sao->f, sao->ts);
// Currently you're not passing the file descriptor down... you need to.sao->f, sao->ts);

    f = sao->f;
    exportData();
  }


  return 0;  // 0 means, did not want this message and did not act on anything.
}

bool
StatsPanel::menu( QPopupMenu* contextMenu)
{


#ifdef DEBUG_StatsPanel
 printf("StatsPanel::menu() entered.\n");
 printf("StatsPanel::menu(),  focusedExpID=%d\n", focusedExpID );
#endif

  Panel::menu(contextMenu);

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::menu, B: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
 printf("StatsPanel::menu, B: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif


  popupMenu = contextMenu; // So we can look up the text easily later.

  QAction *qaction = NULL;

  qaction = new QAction( this,  "_aboutStatsPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Context..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( aboutSelected() ) );
  qaction->setStatusTip( tr("Shows information about what is currently being displayed in the StatsPanel.") );

  qaction = new QAction( this,  "_updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Update Panel..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Attempt to update this panel's display with fresh data.") );

  contextMenu->insertSeparator();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::menu, expID=(%d) focusedExpID=(%d)\n", expID, focusedExpID );
#endif
  if( focusedExpID > 0 )
  {
    qaction = new QAction( this,  "_originalQuery");
    qaction->addTo( contextMenu );
    qaction->setText( QString("Original Query (%1) ...").arg(originalCommand) );
    connect( qaction, SIGNAL( activated() ), this, SLOT( originalQuery() ) );
    qaction->setStatusTip( tr("Update this panel with the data from the initial query.") );

    qaction = new QAction( this,  "_cviewQueryStatemnts");
    qaction->addTo( contextMenu );
    qaction->setText( QString("Query Statements (%1) ...").arg(originalCommand) );
    connect( qaction, SIGNAL( activated() ), this, SLOT( cviewQueryStatements() ) );
    qaction->setStatusTip( tr("Update this panel with the statements related to the initial query.") );
  }

  // Over all the collectors....
  // Round up the metrics ....
  // Create a menu of metrics....
  contextMenu->setCheckable(TRUE);
  int mid = -1;
  QString defaultStatsReportStr = QString::null;

// printf("Do you have a list of collectors?\n");
  qaction = new QAction(this, "selectTimeSlice");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Select: Time Segment") );
  qaction->setToolTip(tr("Select a time segment to limiting future reports.") );
  connect( qaction, SIGNAL( activated() ), this, SLOT(timeSliceSelected()) );

  for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
  {
     std::string collector_name = (std::string)*it;
#ifdef DEBUG_StatsPanel
     printf("collector_name = (%s)\n", collector_name.c_str() );
#endif
    if( QString(collector_name).startsWith("mpi") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an mpi* menu\n");
#endif
      generateMPIMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("io") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an io* menu\n");
#endif
      generateIOMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("hwctime") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an hwctime menu\n");
#endif
      generateHWCTimeMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("hwc") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an hwc menu\n");
#endif
      generateHWCMenu(QString(collector_name));
    } else if( QString(collector_name).startsWith("usertime") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an usertime menu.\n");
#endif
      generateUserTimeMenu();
    } else if( QString(collector_name).startsWith("pcsamp") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate a pcsamp menu\n");
#endif
      generatePCSampMenu();
    } else if( QString(collector_name).startsWith("fpe") )
    {
#ifdef DEBUG_StatsPanel
      printf("Generate a fpe menu\n");
#endif
      generateFPEMenu();
    } else
    {
#ifdef DEBUG_StatsPanel
      printf("Generate an other (%s) menu\n", collector_name.c_str() );
#endif
      generateGenericMenu();
    }

  }

  if( threadMenu ) {
    delete threadMenu;
  }

  threadMenu = new QPopupMenu(this);

  int MAX_PROC_MENU_DISPLAY = 8;
  if( list_of_pids.size() > 1 && list_of_pids.size() <= MAX_PROC_MENU_DISPLAY )
  {
    contextMenu->insertItem(QString("Show Rank/Thread/Process"), threadMenu);
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int pid = (int64_t)*it;
#ifdef DEBUG_StatsPanel
      printf("Inside threadMenu generation, pid=(%d)\n", pid );
#endif
      QString pidStr = QString("%1").arg(pid);
      int mid = threadMenu->insertItem(pidStr);
      threadMenu->setCheckable(TRUE);
      if( currentThreadGroupStrList.count() == 0 )
      {
        threadMenu->setItemChecked(mid, TRUE);
      }
      if( currentThreadStr.isEmpty() || currentThreadStr == pidStr )
      {
        currentThreadStr = pidStr;
      }
      for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); 
                                          it != currentThreadGroupStrList.end(); ++it)
      {
        QString ts = (QString)*it;
        if( ts == pidStr )
        {
          threadMenu->setItemChecked(mid, TRUE);
#ifdef DEBUG_StatsPanel
          printf("Inside threadMenu generation, setting mid=(%d) item checked\n", mid );
#endif
        }
      }
    }
    connect(threadMenu, SIGNAL( activated(int) ),
        this, SLOT(threadSelected(int)) );
    // By default select them all...
    if( currentThreadGroupStrList.count() == 0 )
    {
      for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
           it != list_of_pids.end(); it++ )
      {
        int pid = (int64_t)*it;
        currentThreadGroupStrList.push_back(QString("%1").arg(pid));
      }
    }
  }
#ifdef DEBUG_StatsPanel
  printf("After threadMenu generation, currentThreadGroupStrList.count()=(%d)\n", currentThreadGroupStrList.count() );
#endif

  contextMenu->insertSeparator();


#ifndef COLUMNS_MENU
  int id = 0;
  QPopupMenu *columnsMenu = new QPopupMenu(this);
  columnsMenu->setCaption("Manage Columns Menu Options");
  contextMenu->insertItem("&Manage Columns Menu Options", columnsMenu, CTRL+Key_C);

  for( ColumnList::Iterator pit = columnHeaderList.begin();
           pit != columnHeaderList.end();
           ++pit )
  { 
    QString s = (QString)*pit;
    columnsMenu->insertItem(s, this, SLOT(doOption(int)), CTRL+Key_1, id, -1);
    if( splv->columnWidth(id) )
    {
      columnsMenu->setItemChecked(id, TRUE);
    } else
    {
      columnsMenu->setItemChecked(id, FALSE);
    }
    id++;
  }
#endif // COLUMNS_MENU

  qaction = new QAction( this,  "exportDataAction");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Report Data..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( exportData() ) );
  qaction->setStatusTip( tr("Save the report's data to an ascii file.") );

  if( splv->selectedItem() )
  {
    qaction = new QAction( this,  "gotoSource");
    qaction->addTo( contextMenu );
    if( focusedExpID != -1 )
    {
      qaction->setText( QString("Go to source location (for Exp %1) ...").arg(focusedExpID) );
    } else
    {
      qaction->setText( "Go to source location..." );
    }
    connect( qaction, SIGNAL( activated() ), this, SLOT( gotoSource() ) );
    qaction->setStatusTip( tr("Position at source location of this item.") );

    // If we have more than one experiment... figure out focus. 
  }


  if( experimentGroupList.count() > 1 )
  {
    int id = 0;
    experimentsMenu = new QPopupMenu(this);
    columnsMenu->setCaption("Select Focused Experiment:");
    contextMenu->insertItem("&Select Focused Experiment:", experimentsMenu, CTRL+Key_M);
  
    for(ExperimentGroupList::iterator egi = experimentGroupList.begin();egi != experimentGroupList.end();egi++)
    {
      QString s = (QString)*egi;
      int index = s.find(":");
      if( index != -1 )
      {
        index++;
        int exp_id = s.mid(index,9999).stripWhiteSpace().toInt();
        s = QString("Experiment: %1").arg(exp_id);
        id = experimentsMenu->insertItem(s);
        if( exp_id == focusedExpID )
        {
          experimentsMenu->setItemChecked(id, TRUE);
        }
      }
    }

    connect(experimentsMenu, SIGNAL( activated(int) ),
      this, SLOT(focusOnExp(int)) );
  }


  contextMenu->insertSeparator();
 
  if( chartFLAG == TRUE && statsFLAG == TRUE )
  {
    qaction = new QAction( this,  "re-orientate");
    qaction->addTo( contextMenu );
    qaction->setText( "Re-orientate" );
    connect( qaction, SIGNAL( activated() ), this, SLOT( setOrientation() ) );
    qaction->setStatusTip( tr("Display chart/statistics horizontal/vertical.") );
  }

  if( chartFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are shown, hide the graphic chart.") );
  } else
  {
    qaction = new QAction( this,  "showChart");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Chart..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showChart() ) );
    qaction->setStatusTip( tr("If graphics are available, show the graphic chart.") );
  }
  if( statsFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Hide the statistics display.") );
  } else
  {
    qaction = new QAction( this,  "showStatistics");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Statistics..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showStats() ) );
    qaction->setStatusTip( tr("Show the statistics display.") );
  }

  if( infoHeaderFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideInfoHeader");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Experiment Metadata..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showInfoHeader() ) );
    qaction->setStatusTip( tr("Hide the experiment header information display.") );
  } else
  {
    qaction = new QAction( this,  "showInfoHeader");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Experiment Metadata Info..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showInfoHeader() ) );
    qaction->setStatusTip( tr("Show the experiment metadata display.") );
  }

  if( toolBarFLAG == TRUE )
  {
    qaction = new QAction( this,  "hideToolBar");
    qaction->addTo( contextMenu );
    qaction->setText( "Hide Experiment Display Option ToolBar..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showToolBar() ) );
    qaction->setStatusTip( tr("Hide the experiment display option toolbar.") );
  } else
  {
    qaction = new QAction( this,  "showToolBar");
    qaction->addTo( contextMenu );
    qaction->setText( "Show Experiment Display Option ToolBar..." );
    connect( qaction, SIGNAL( activated() ), this, SLOT( showToolBar() ) );
    qaction->setStatusTip( tr("Show the experiment display option toolbar.") );
  }

// printf("menu: canWeDiff()?\n");
  if( canWeDiff() )
  {
// printf("menu: canWeDiff() says we can!\n");
    if( insertDiffColumnFLAG == TRUE )
    {
      qaction = new QAction( this,  "hideDifference");
      qaction->addTo( contextMenu );
      qaction->setText( "Hide Difference..." );
      connect( qaction, SIGNAL( activated() ), this, SLOT( showDiff() ) );
      qaction->setStatusTip( tr("Hide the difference column.") );
    } else
    {
      qaction = new QAction( this,  "showDifference");
      qaction->addTo( contextMenu );
      qaction->setText( "Show Difference..." );
      connect( qaction, SIGNAL( activated() ), this, SLOT( showDiff() ) );
      qaction->setStatusTip( tr("Show the difference column.") );
    }
  }

#ifdef CLUSTERANALYSIS
// if( focusedExpID == -1 && currentCollectorStr == "usertime" )
if( focusedExpID == -1 )
{
  contextMenu->insertSeparator();
  qaction = new QAction( this,  "clusterAnalysisSelected");
  qaction->addTo( contextMenu );
  qaction->setText( "Compare and Analyze using cluster analysis" );
  connect( qaction, SIGNAL( activated() ), this, SLOT( clusterAnalysisSelected() ) );
  qaction->setStatusTip( tr("Perform analysis on the processes, threads, or ranks of this experiment to group similar processes, threads, or ranks.") );
}

#endif// CLUSTERANALYSIS


  contextMenu->insertSeparator();

  qaction = new QAction( this,  "customizeExperimentsSelected");
  qaction->addTo( contextMenu );
  qaction->setText( "Custom Comparison ..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( customizeExperimentsSelected() ) );
  qaction->setStatusTip( tr("Customize column data in the StatsPanel.") );

//#ifdef MIN_MAX_ENABLED
  contextMenu->insertSeparator();

  qaction = new QAction( this,  "minMaxAverageSelected");
  qaction->addTo( contextMenu );
  qaction->setText( "Show Load Balance Overview..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( minMaxAverageSelected() ) );
  qaction->setStatusTip( tr("Generate a min, max, average report in the StatsPanel.") );
//#endif

  return( TRUE );
}

QString
StatsPanel::getMostImportantClusterMetric(QString collector_name)
{
  QString metric = QString::null;

/*
  if( collector_name == "pcsamp" )
  {
    metric = "-m pcsamp::exclusive_time";
  } else if( collector_name == "usertime" )
  {
    metric = "-m usertime::exclusive_time";
  } else if( collector_name == "hwc" )
  {
    metric = "-m hwc::ThreadAverage";
  } else if( collector_name == "hwctime" )
  {
    metric = "-m hwc::ThreadAverage";
  } else if( collector_name == "mpi" )
  {
    metric = "-m mpi::ThreadAverage";
  } else if( collector_name == "mpit" )
  {
    metric = "-m mpit::ThreadAverage";
  } else if( collector_name == "io" )
  {
    metric = "-m io::ThreadAverage";
  } else if( collector_name == "iot" )
  {
    metric = "-m io::ThreadAverage";
  }
*/

  return(metric);

}

void
StatsPanel::clusterAnalysisSelected()
{
  QString command = QString::null;
  QString mim = getMostImportantClusterMetric(currentCollectorStr);
  if( focusedExpID == -1 )
  {
    command = QString("cviewCluster -x %1 %2 %3").arg(expID).arg(timeIntervalString).arg(mim);
  } else
  {
    command = QString("cviewCluster -x %1 %2 %3").arg(focusedExpID).arg(timeIntervalString).arg(mim);
  }
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  std::list<int64_t> list_of_cids;
  list_of_cids.clear();
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
         &list_of_cids, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
// printf("ran %s\n", command.ascii() );

  if( clip )
  {
    clip->Set_Results_Used();
  }

  QString pidlist = QString::null;
  if( list_of_cids.size() >= 1 )
  {
    for( std::list<int64_t>::const_iterator it = list_of_cids.begin();
         it != list_of_cids.end(); it++ )
    {
      int pid = (int64_t)*it;
      if( pidlist.isEmpty() )
      { 
        pidlist = QString("%1").arg(pid);
      } else 
      {
        pidlist += QString(", %1").arg(pid);
      }
    }
  } else
  {
// printf("No outliers...\n");
    return;
  }

//  command = QString("cview -c %1 -m usertime::exclusive_time %2").arg(pidlist).arg(timeIntervalString);
//  command = QString("cview -c %1 %2").arg(pidlist).arg(timeIntervalString);
  command = QString("cview -c %1 %2 %3").arg(pidlist).arg("-m ThreadAverage").arg(timeIntervalString);
// printf("run %s\n", command.ascii() );

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clusterAnalysisSelected() about to call updateStatsPanelData, command=%s\n", 
         command.ascii() );
#endif
  toolbar_status_label->setText("Generating Comparative Analysis Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Showing Comparative Analysis Report:");
}


//#ifdef MIN_MAX_ENABLED
void
StatsPanel::minMaxAverageSelected()
{
  QString command = QString::null;
  if( focusedExpID == -1 ) {
    command = QString("expview -x %1 %2 -m %3::ThreadMin, %4::ThreadMax, %5::ThreadAverage").arg(expID).arg(timeIntervalString).arg(currentCollectorStr).arg(currentCollectorStr).arg(currentCollectorStr);
  } else {
    command = QString("expview -x %1 %2 -m %3::ThreadMin, %4::ThreadMax, %5::ThreadAverage").arg(focusedExpID).arg(timeIntervalString).arg(currentCollectorStr).arg(currentCollectorStr).arg(currentCollectorStr);
  }


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::minMaxAverageSelected() about to call updateStatsPanelData, command=%s\n", 
         command.ascii() );
#endif

  toolbar_status_label->setText("Generating Load Balance (min,max,ave) Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Showing Load Balance (min,max,ave) Report:");

}
//#endif

void
StatsPanel::showEventListSelected()
{
  QString command = QString::null;
  traceAddition = " -v trace";
#if 0
  if( focusedExpID == -1 ) {
    command = QString("expview -x %1 %2 %3 %4").arg(expID).arg(currentCollectorStr).arg(timeIntervalString).arg(traceAddition);
  } else {
    command = QString("expview -x %1 %2 %3 %4").arg(focusedExpID).arg(currentCollectorStr).arg(timeIntervalString).arg(traceAddition);
  }
#endif


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showEventListSelected() about to call updateStatsPanelData, command=%s\n", 
         command.ascii() );
#endif

  toolbar_status_label->setText("Generating Per Event Report:");
  updateStatsPanelData(DO_FORCE_UPDATE, NULL);
  toolbar_status_label->setText("Showing Per Event Report:");

}

void
StatsPanel::clearAuxiliarySelected()
{


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::clearAuxiliarySelected() entered\n" );
#endif

  toolbar_status_label->setText("Clearing Auxiliary Setttings:");

  selectedFunctionStr = QString::null;

  timeIntervalString = QString::null;

  traceAddition = QString::null;

//  updateStatsPanelData(DONT_FORCE_UPDATE, command);
  toolbar_status_label->setText("Cleared Auxiliary Setttings, future reports are aggregated over all processes,threads, or ranks:");

}

void
StatsPanel::customizeExperimentsSelected()
{
  nprintf( DEBUG_PANELS ) ("StatsPanel::customizeExperimentsSelected()\n");

  QString name = QString("CustomizeStatsPanel [%1]").arg(expID);

  Panel *customizePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( customizePanel )
  { 
    nprintf( DEBUG_PANELS ) ("customizePanel() found customizePanel found.. raise it.\n");
    getPanelContainer()->raisePanel(customizePanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("customizePanel() no customizePanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

//    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
   ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);
   if( focusedExpID != -1 )
   {
// printf("assing qstring_data\n");
     ao->qstring_data = QString("%1").arg(focusedExpID);
   } else
   {
     ao->qstring_data = QString("%1").arg(groupID);
   }
    customizePanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("CustomizeStatsPanel", bestFitPC, ao, (const char *)NULL);
    delete ao;
  }

}   

void
StatsPanel::generateModifierMenu(QPopupMenu *menu, 
                                 std::list<std::string> modifier_list, 
                                 std::list<std::string> current_list)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::generateModifierMenu() entered\n");
#endif
  menu->setCheckable(TRUE);
  for( std::list<std::string>::const_iterator it = modifier_list.begin();
          it != modifier_list.end(); it++ )
  {
    std::string modifier = (std::string)*it;

#ifdef DEBUG_StatsPanel
    printf("modifier = (%s)\n", modifier.c_str() );
#endif

    QString s = QString(modifier.c_str() );
     int mid = menu->insertItem(s);
    for( std::list<std::string>::const_iterator it = current_list.begin();
         it != current_list.end(); it++ )
    {
      std::string current_modifier = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("building menu : current_list here's one (%s)\n", current_modifier.c_str() );
#endif
      if( modifier == current_modifier )
      {
#ifdef DEBUG_StatsPanel
        printf("WE have a match to check\n");
#endif
        menu->setItemChecked(mid, TRUE);
      }
    }
  }
}

/*! Create the context senstive menu for the report. */
bool
StatsPanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
// printf("StatsPanel::createPopupMenu(contextMenu=0x%x) entered\n", contextMenu);
  menu(contextMenu);
  return( TRUE );
}



void
StatsPanel::showChart()
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showChart() entered, CHART, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showChart() entered, CHART, chartFLAG=%d\n", chartFLAG);
#endif

  if( chartFLAG == TRUE ) {
    chartFLAG = FALSE;
    cf->hide();
  } else {
    chartFLAG = TRUE;

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::showChart(), CHART, lastCommand=(%s)\n", lastCommand.ascii() );
#endif

    cf->show();
    if( !lastCommand.startsWith("cview") )
    {
      lastCommand = QString::null;  // This will force a redraw of the data.
      // I'm not sure why, but the text won't draw unless the 
      // piechart is visible.
      updatePanel();
    } else
    {
      cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);
      updatePanel();
    }
    cf->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( chartFLAG == FALSE && statsFLAG == FALSE )
  {
    statsFLAG = TRUE;
    splv->show();
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showChart() exitted, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showChart() exitted, chartFLAG=%d\n", chartFLAG);
#endif
}


void
StatsPanel::showStats()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showStats() entered, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showStats() entered, chartFLAG=%d\n", chartFLAG);
#endif
// printf("StatsPanel::showStats() entered\n");
  if( statsFLAG == TRUE )
  {
    statsFLAG = FALSE;
    splv->hide();
  } else
  {
    statsFLAG = TRUE;
    splv->show();
  }

  // Make sure there's not a blank panel.   If the user selected to 
  // hide the only display, show the other by default.
  if( statsFLAG == FALSE && chartFLAG == FALSE )
  {
    chartFLAG = TRUE;
    cf->show();
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showStats() exitted, statsFLAG=%d\n", statsFLAG);
  printf("StatsPanel::showStats() exitted, chartFLAG=%d\n", chartFLAG);
#endif
}


void
StatsPanel::showToolBar()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showToolBar() entered, toolBarFLAG=%d\n", toolBarFLAG);
#endif
  if( toolBarFLAG == TRUE )
  {
    toolBarFLAG = FALSE;
    fileTools->hide();
  } else
  {
    toolBarFLAG = TRUE;
    fileTools->show();
  }
}

void
StatsPanel::showInfoHeader()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::showInfoHeader() entered, infoHeaderFLAG=%d\n", infoHeaderFLAG);
#endif
  if( infoHeaderFLAG == TRUE )
  {
    infoHeaderFLAG = FALSE;
    metadataAllSpaceFrame->hide();
  } else
  {
    infoHeaderFLAG = TRUE;
    metadataAllSpaceFrame->show();
  }
}


void
StatsPanel::showDiff()
{
// printf("StatsPanel::showDiff() entered\n");
  if( insertDiffColumnFLAG == TRUE )
  {
// printf("Remove the diff column.\n");
    removeDiffColumn(0); // zero is always the "|Difference|" column.
    // Force a resort in this case...
    splv->setSorting ( 0, FALSE );
    splv->sort();
    insertDiffColumnFLAG = FALSE;
  } else
  {
// printf("insert the diff column.\n");
    int insertColumn = 0;
    insertDiffColumn(insertColumn);
    insertDiffColumnFLAG = TRUE;

    // Force a resort in this case...
    splv->setSorting ( insertColumn, FALSE );
    splv->sort();
  }
}


/*! Reset the orientation of the graph/text relationship with setOrientation */
void
StatsPanel::setOrientation()
{
// printf("StatsPanel::setOrientation() entered\n");
  Orientation o = splitterB->orientation();
  if( o == QSplitter::Vertical )
  {
    splitterB->setOrientation(QSplitter::Horizontal);
  } else
  {
    splitterB->setOrientation(QSplitter::Vertical);
  }
}


/*! Go to source menu item was selected. */
void
StatsPanel::details()
{
#ifdef DEBUG_StatsPanel
  printf("details() menu selected.\n");
#endif
}

void
StatsPanel::exportData()
{
// printf("exportData() menu selected.\n");
  Orientation o = splitterB->orientation();
  QListViewItemIterator it( splv );
  int cols =  splv->columns();
  int i=0;
  QString fileName = "StatsPanel.txt";
  QString dirName = QString::null;

  if( f == NULL)
  {
    QFileDialog *fd = new QFileDialog(this, "save_StatsPanelData:", TRUE );
    fd->setCaption( QFileDialog::tr("Save StatsPanel data:") );
    fd->setMode( QFileDialog::AnyFile );
    fd->setSelection(fileName);
    QString types( 
                      "Data files (*.dat);;"
                      "Text files (*.txt);;"
                      "Any File (*.*);;"
                      );
    fd->setFilters( types );
    // Pick the initial default types to put out.
    const QString mask = QString("*.txt");
    fd->setSelectedFilter( mask );
    fd->setDir(dirName);
  
    if( fd->exec() == QDialog::Accepted )
    {
      fileName = fd->selectedFile();
    }
    
    if( !fileName.isEmpty() )
    {
        f = new QFile(fileName);
        f->open(IO_WriteOnly );
    }
  }

  bool datFLAG = FALSE;
  if( fileName.endsWith(".dat") )
  {
    datFLAG = TRUE;
  }

  if( f != NULL )
  {
    // Write out the header info
    QString line = QString("  ");
    for(i=0;i<cols;i++)
    {
      for(i=0;i<cols;i++)
      {
        if( datFLAG == TRUE )
        {
          if( i < cols-1 )
          {
            line += QString(splv->columnText(i))+"; ";
          } else
          {
            line += QString(splv->columnText(i))+" ";
          }
        } else
        {
          line += QString(splv->columnText(i))+" ";
        }
      }
      line += QString("\n");
    }
    f->writeBlock( line, qstrlen(line) );

   // Write out the body info
    while( it.current() )
    {
      QListViewItem *item = *it;
      line = QString("  ");
      for(i=0;i<cols;i++)
      {
        if( datFLAG == TRUE )
        {
          if( i < cols-1 )
          {
            line += QString(item->text(i))+"; ";
          } else
          {
            line += QString(item->text(i))+" ";
          }
        } else
        {
          line += QString(item->text(i))+" ";
        }
      }
      line += QString("\n");
      f->writeBlock( line, qstrlen(line) );
      ++it;
    }
    f->close();
  }

  f = NULL;

}

void
StatsPanel::updatePanel()
{
#ifdef DEBUG_StatsPanel
  printf("updatePanel() about to call updateStatsPanelData, lastCommand=%s\n", lastCommand.ascii());
#endif

//  updateStatsPanelData(DONT_FORCE_UPDATE, lastCommand);
  updateStatsPanelData(DONT_FORCE_UPDATE, NULL);
}

void
StatsPanel::originalQuery()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::originalQuery() about to call updateStatsPanelData()\n");
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE, originalCommand);
}

void
StatsPanel::cviewQueryStatements()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::cviewQueryStatements about to call updateStatsPanelData, originalCommand,with vstatements=(%s)\n", 
          QString(originalCommand + " -v statements").ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE, originalCommand + " -v statements");
}

#include "CustomExperimentPanel.hxx"
void
StatsPanel::timeSliceSelected()
{
// printf("WE have a SELECT_TIME_SEGMENT\n");
  if( timeSegmentDialog == NULL )
  {
    timeSegmentDialog = new SelectTimeSegmentDialog(getPanelContainer()->getMainWindow(), "Select Time Interval:");
  }

  int skylineFLAG = getPreferenceShowSkyline();
  if( skylineFLAG )
  {
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, look up the skyline....\n");
   printf("StatsPanel::timeSliceSelected, currentCollectorStr=%s\n", currentCollectorStr.ascii() );
#endif

    InputLineObject *clip = NULL;
    QString mim = CustomExperimentPanel::getMostImportantMetric(currentCollectorStr);

// int segmentSize = 10;
    int segmentSize = getPreferenceShowSkyLineLineEdit().toInt();
    int startSegment = 0;
    int endSegment=segmentSize;
    QString segmentString = QString::null;
    while( startSegment < 100 )
    {
      segmentString += QString(" -I % %1:%2").arg(startSegment).arg(endSegment);
      startSegment = endSegment+1;
      endSegment += segmentSize;
      if( endSegment > 100 ) {
        endSegment = 100;
      }
    }

//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:25 -I % 26:50 -I % 51:75 -I % 76:100";
//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:20 -I % 21:40 -I % 41:60 -I % 61:80 -I % 81:100";
//    QString command = "expCompare -m exclusive_time usertime1 -I % 0:10 -I % 11:20 -I % 21:30 -I % 31:40 -I % 41:50 -I % 51:60 -I % 61:70 -I % 71:80 -I % 81:90 -I % 91:100";

//    QString command = QString("expCompare -x %1 %2 %3 -v Summary -I % 0:20 -I % 21:40 -I % 41:60 -I % 61:80 -I % 81:100").arg(expID).arg(mim).arg(currentCollectorStr);
//    QString command = QString("expCompare -x %1 %2 %3 -v Summary -I % 0:10 -I % 11:20 -I % 21:30 -I % 31:40 -I % 41:50 -I % 51:60 -I % 61:70 -I % 71:80 -I % 81:90 -I % 91:100").arg(expID).arg(mim).arg(currentCollectorStr);


    QString command = QString("expCompare -x %1 %2 %3 -v Summary %4").arg(expID).arg(mim).arg(currentCollectorStr).arg(segmentString);
    command += QString(" %1").arg(currentThreadsStr);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::timeSliceSelected, do expCompare command=(%s)\n", command.ascii() );
#endif

    skylineValues.clear();
    skylineText.clear();

    QApplication::setOverrideCursor(QCursor::WaitCursor);
 
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());
    if( clip == NULL )
    {
      cerr << "No skyline available for this experiment.\n";
      QApplication::restoreOverrideCursor( );
      return;
    }
    Input_Line_Status status = ILO_UNKNOWN;

    while( !clip->Semantics_Complete() )
    {
      qApp->processEvents(1000);
      sleep(1);
    }

    std::list<CommandObject *>::iterator coi;

    coi = clip->CmdObj_List().begin();
    CommandObject *co = (CommandObject *)(*coi);

    std::list<CommandResult *>::iterator cri;
    std::list<CommandResult *> cmd_result = co->Result_List();
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++)
    {
// cerr << "TYPE: = " << (*cri)->Type() << "\n";
//      if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES)
      if ((*cri)->Type() == CMD_RESULT_COLUMN_ENDER)
      {
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::timeSliceSelected, Here CMD_RESULT_COLUMN_ENDER:\n");
#endif
        std::list<CommandResult *> columns;
        CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
        ccp->Value(columns);
        std::list<CommandResult *>::iterator column_it;
        for (column_it = columns.begin(); column_it != columns.end(); column_it++)
        {
          CommandResult *cr = (CommandResult *)(*column_it);
          QString vs = (*column_it)->Form().c_str();
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::timeSliceSelected, vs=(%s)\n", vs.ascii() );
#endif
          unsigned int value = 0;
          switch( cr->Type() )
          {
            case CMD_RESULT_NULL:
// cerr << "Got CMD_RESULT_NULL\n";
              value = 0;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_UINT:
// cerr << "Got CMD_RESULT_UINT\n";
              value = vs.toUInt();;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_INT:
// cerr << "Got CMD_RESULT_INT\n";
              value = vs.toInt();;
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_FLOAT:
// cerr << "Got CMD_RESULT_FLOAT\n";
              value = (int)(vs.toFloat());
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            case CMD_RESULT_STRING:
// cerr << "Got CMD_RESULT_STRING\n";
//              value = vs.toInt();;
              value = 1; // FIX
              skylineValues.push_back(value);
              skylineText.push_back(vs.stripWhiteSpace());
              break;
            default:
              continue;
              break;
          }
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::timeSliceSelected, int value = (%d)\n", value );
#endif
        }
      }
    }
    QApplication::restoreOverrideCursor( );
    clip->Set_Results_Used();
  
    // For now don't show text.
    skylineText.clear();
    timeSegmentDialog->cf->show();
    timeSegmentDialog->cf->setValues(skylineValues, skylineText, blue_color_names, 1);
//     timeSegmentDialog->cf->setValues(skylineValues, skylineText, color_names, MAX_COLOR_CNT);
  } else {
    timeSegmentDialog->cf->hide();
  }

  if( timeSegmentDialog->exec() == QDialog::Accepted )
  { 
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, The user hit accept.\n");
   printf("StatsPanel::timeSliceSelected, start=%s end=%s\n", timeSegmentDialog->startValue->text().ascii(), timeSegmentDialog->endValue->text().ascii() );
#endif

   if (!timeIntervalString.isEmpty()) {
     prevTimeIntervalString = timeIntervalString;
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::timeSliceSelected, BEFORE UPDATING timeIntervalString=(%s)\n", timeIntervalString.ascii() );
     printf("StatsPanel::timeSliceSelected, BEFORE UPDATING timeIntervalString=(%s)\n", timeIntervalString.ascii() );
#endif
   }

   // Don't have an interval string if the values are reset back to 0:100
   int start_index = timeSegmentDialog->startValue->text().toInt();
   int end_index = timeSegmentDialog->endValue->text().toInt();
   if ( start_index == 0 && end_index == 100) {
     timeIntervalString = QString("");
   } else {
     timeIntervalString = QString(" -I % %1:%2").arg(timeSegmentDialog->startValue->text()).arg(timeSegmentDialog->endValue->text());
   }

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::timeSliceSelected, start_index=%d, end_index=%d, timeIntervalString=(%s)\n", 
         start_index, end_index, timeIntervalString.ascii() );
#endif

  }
  return;
}

void
StatsPanel::focusOnExp(int val)
{
// printf("Just set the focus to the first for now... val=%d\n", val );
// printf("Menu item %s selected \n", experimentsMenu->text(val).ascii() );

  QString valStr = experimentsMenu->text(val).ascii();
  int index = valStr.find(":");
  if( index != -1 )
  {
    index++;
    int id = valStr.mid(index,9999).stripWhiteSpace().toInt();
    focusedExpID = id;
    if( experimentGroupList.count() > 0 )
    {
      updateCollectorList();
    }

// printf("You just assigned the focusedExpID=%d\n", focusedExpID);
  }
}

/*! Go to source menu item was selected. */
void
StatsPanel::gotoSource(bool use_current_item)
{
  QListViewItem *lvi = NULL;

  if( use_current_item )
  {
    nprintf(DEBUG_PANELS) ("gotoSource() menu selected, USE_CURRENT_ITEM.\n");
    lvi = currentItem;
  } else 
  {
    nprintf(DEBUG_PANELS) ("gotoSource() calls splv->selectedItem.\n");
    lvi =  splv->selectedItem();
  }

  itemSelected(lvi);
}

#include "AboutDialog.hxx"
void
StatsPanel::aboutSelected()
{
  aboutOutputString = QString("%1\n\n").arg(aboutString);
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::aboutSelected, aboutOutputString.ascii()=%s\n", aboutOutputString.ascii());
#endif

  QString command = QString::null;

  AboutOutputClass *aboutOutputClass = NULL;

  int cviewinfo_index = lastCommand.find("cview ");
  if( cviewinfo_index != -1 )
  {
    aboutOutputString = QString("%1\n\n").arg(aboutString);
    aboutOutputString += QString("Where:\n");
    for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
    {
      CInfoClass *cic = (CInfoClass *)*it;
// cic->print();
      aboutOutputString += QString("-c %1:\n").arg(cic->cid);
      aboutOutputString += QString("  Experiment: %1\n").arg(cic->expID);
      aboutOutputString += QString("  Collector: %1\n").arg(cic->collector_name);
      if( cic->host_pid_names.isEmpty() )
      {
        aboutOutputString += QString("  Host/pids: All\n");
      } else
      {
        aboutOutputString += QString("  Host/pids: %1\n").arg(cic->host_pid_names);
      }
      aboutOutputString += QString("  Metric: %1\n").arg(cic->metricStr);
    }
  }

  AboutDialog *aboutDialog = new AboutDialog(this, "StatsPanel Context:", FALSE, 0, aboutOutputString);
  aboutDialog->show();
}

void
StatsPanel::MPItraceSelected()
{
  if( MPItraceFLAG == TRUE )
  {
    MPItraceFLAG = FALSE;
  } else
  {
    MPItraceFLAG = TRUE;
  }
}

void
StatsPanel::IOtraceSelected()
{
  if( IOtraceFLAG == TRUE )
  {
    IOtraceFLAG = FALSE;
  } else
  {
    IOtraceFLAG = TRUE;
  }
}


void
StatsPanel::manageProcessesSelected()
{
// printf("manageProcessesSelected() menu selected.\n");
  QString name = QString("ManageProcessesPanel [%1]").arg(expID);


  Panel *manageProcessesPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessesPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() found ManageProcessesPanel found.. raise it.\n");
    getPanelContainer()->raisePanel(manageProcessesPanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() no ManageProcessesPanel found.. create one.\n");

    PanelContainer *startPC = getPanelContainer();
    PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    manageProcessesPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", startPC, ao);
    delete ao;
  }

  if( manageProcessesPanel )
  {
//    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", manageProcessesPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, "pcsamp", 1);
      manageProcessesPanel->listener( (void *)msg );
    }
  }

}

#ifdef OLDWAY
void
StatsPanel::headerSelected(int index)
{
  QString headerStr = splv->columnText(index);
// printf("StatsPanel::%d headerSelected(%s)\n", index, headerStr.ascii() );

  if(  headerStr == "|Difference|" )
  {
    absDiffFLAG = TRUE; // Major hack to do sort based on absolute values.
  }
}
#endif // OLDWAY

void
StatsPanel::itemSelected(int index)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::itemSelected entered, index=(%d)\n", index);
#endif
  QListViewItemIterator it( splv );
  int i = 0;
  while( it.current() )
  {
    QListViewItem *item = *it;
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::itemSelected,i=%d, index=%d\n", i, index);
#endif
    if( i == index ) {

      currentItem = (SPListViewItem *)item;
      currentItemIndex = index;

#ifdef DEBUG_StatsPanel
      printf("A: currentItemIndex set to %d\n", currentItemIndex);
#endif

     // highlight the list item
     // Now call the action routine.
      splv->setSelected((QListViewItem *)item, TRUE);
      itemSelected(item);
      break;

    } // end i==index

    i++;
    it++;
  } // end while
    
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::itemSelected exit, index=(%d)\n", index);
#endif
}

void
StatsPanel::returnPressed(QListViewItem *item)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::StatsPanel::returnPressed, lastCommand=(%s)\n", lastCommand.ascii());
#endif
  if( lastCommand.contains("Butterfly") )
  {
    updateStatsPanelData(DONT_FORCE_UPDATE );
  } else
  {
    itemSelected( item );
  }
}

void
StatsPanel::itemSelected(QListViewItem *item)
{
  nprintf(DEBUG_PANELS) ("StatsPanel::itemSelected(QListViewItem *) item=%s\n", item->text(0).ascii() );

  if( item )
  {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::itemSelected(QListViewItem *) item=%s\n", item->text(fieldCount-1).ascii() );
#endif
    matchSelectedItem( item, std::string(item->text(fieldCount-1).ascii()) );
  }
}


static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanel::doOption(int id)
{
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::doOption() id=%d\n", id);
#endif

  if( splv->columnWidth(id) )
  {
    cwidth = splv->columnWidth(id);
    splv->hideColumn(id);
#ifdef DEBUG_StatsPanel
 printf("doOption() if cwidth=%d\n", cwidth);
#endif
  } else
  {
    splv->setColumnWidth(id, cwidth);
#ifdef DEBUG_StatsPanel
 printf("doOption() else cwidth=%d\n", cwidth);
#endif
  } 
}

bool
StatsPanel::matchSelectedItem(QListViewItem *item, std::string sf )
{
  nprintf( DEBUG_PANELS) ("matchSelectedItem() entered. sf=%s\n", sf.c_str() );

  SPListViewItem *spitem = (SPListViewItem *)item;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, matchSelectedItem() entered. sf=%s\n", sf.c_str() );
  printf("StatsPanel::matchSelectedItem, A: currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
  printf("StatsPanel::matchSelectedItem, A: currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
#endif

  QString lineNumberStr = "-1";
  QString filename = QString::null;
  SourceObject *spo = NULL;
  QString ssf = QString(sf).stripWhiteSpace();

  filename = spitem->fileName.ascii();
  lineNumberStr = QString("%1").arg(spitem->lineNumber);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, spitem->funcName=(%s)\n", spitem->funcName.ascii() ); 
  printf("StatsPanel::matchSelectedItem, spitem->fileName=(%s)\n", spitem->fileName.ascii() ); 
  printf("StatsPanel::matchSelectedItem, spitem->lineNumber=(%d)\n", spitem->lineNumber ); 
#endif
  nprintf( DEBUG_PANELS) ("spitem->funcName=(%s)\n", spitem->funcName.ascii() ); 
  nprintf( DEBUG_PANELS) ("spitem->fileName=(%s)\n", spitem->fileName.ascii() ); 
  nprintf( DEBUG_PANELS) ("spitem->lineNumber=(%d)\n", spitem->lineNumber ); 


  // Explicitly make sure the highlightList is clear.
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();


  QApplication::setOverrideCursor(QCursor::WaitCursor);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::matchSelectedItem, LOOK UP FILE HIGHLIGHTS THE NEW WAY!\n");
#endif

  spo = lookUpFileHighlights(filename, lineNumberStr, highlightList);

  if( !spo )
  {
      spo = new SourceObject(NULL, NULL, -1, expID, TRUE, NULL);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::matchSelectedItem, created spo new SourceObject, expID=%d\n", expID);
#endif
  }
  if( spo )
  {
      QString name = QString::null;
      if( expID == -1  )
      {
        name = QString("Source Panel [%1]").arg(groupID);
      } else
      {
        name = QString("Source Panel [%1]").arg(expID);
      }
      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel )
      {
        char *panel_type = "Source Panel";
        PanelContainer *startPC = NULL;
        if( getPanelContainer()->parentPanelContainer != NULL )
        {
          startPC = getPanelContainer()->parentPanelContainer;
        } else
        {
          startPC = getPanelContainer();
        }
        PanelContainer *bestFitPC = topPC->findBestFitPanelContainer(startPC);
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", groupID);
        sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
      }
      if( sourcePanel )
      {
       sourcePanel->listener((void *)spo);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::matchSelectedItem, Returned from sending of spo, via sourcePanel->listener!\n");
#endif
      }
  }
  QApplication::restoreOverrideCursor( );

}

// Start looking for tokens at start_index and return  the value you find
// Be careful with this routine.  It isn't context sensitive.  The searchStr
// is a hint on what the caller is expecting to find, but if not found it
// searches for other tokens in no particular order.
static int findNextMajorToken(QString str, int start_index, QString searchStr)
{

 int eol_index = str.length() - 1;
 int end_index = -1;
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, enter start_index=%d, str.ascii()=(%s), searchStr=(%s)\n", start_index, str.ascii(), searchStr.ascii() );
#endif


 end_index = str.find(searchStr, start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, searchStr=(%s), end_index=%d\n", searchStr.ascii(), end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }
 

 end_index = str.find("-h", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -h, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-m", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -m, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }
 end_index = str.find("-p", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -p, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-r", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -r, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-t", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -t, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("Average", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, Average, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find(";", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, semi-colon-> ;, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("-I", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, -I, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("%", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, %, end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

 end_index = str.find("\n", start_index);
#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, \n end_index=%d\n", end_index);
#endif
 if (end_index != -1) {
   return (end_index);
 }

#ifdef DEBUG_StatsPanel
 printf(" findNextMajorToken, FallThrough CASE, assume we hit end of line, eol_index=%d\n", eol_index);
#endif
 return(eol_index);

}

void StatsPanel::getPidList(int exp_id)
{
// Now get the threads.
 QString command = QString::null;
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::getPidList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 currentThreadStrENUM = UNKNOWN;
 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 )
  {
    command = QString("list -v ranks -x %1").arg(exp_id);
  } else
  {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }
  currentThreadStrENUM = RANK;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPidList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_pids.clear();
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
         &list_of_pids, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  if( list_of_pids.size() == 0 )
  {
    currentThreadStrENUM = THREAD;
    if( focusedExpID == -1 )
    {
      command = QString("list -v threads -x %1").arg(exp_id);
    } else
    {
      command = QString("list -v threads -x %1").arg(focusedExpID);
    }
// printf("attempt to run (%s)\n", command.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &list_of_pids, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  } 
  if( list_of_pids.size() == 0 )
  {
    currentThreadStrENUM = PID;
    if( focusedExpID == -1 )
    {
      command = QString("list -v pids -x %1").arg(exp_id);
    } else
    {
      command = QString("list -v pids -x %1").arg(focusedExpID);
    }
// printf("attempt to run (%s)\n", command.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &list_of_pids, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPidList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  } 

  if( list_of_pids.size() > 1 )
  {
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int pid = (int64_t)*it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getPidList, pid=(%d)\n", pid );
#endif
    }
  }
 } else {
    list_of_pids.clear();
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::getPidList, not valid exp_id=%d, no pids/ranks/threads\n", exp_id);
#endif
 }
}

void StatsPanel::getHostList(int exp_id)
{
// Now get the hosts
 QString command = QString::null;
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::getHostList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadStrENUM = UNKNOWN;
 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 )
  {
    command = QString("list -v hosts -x %1").arg(exp_id);
  } else
  {
    command = QString("list -v hosts -x %1").arg(focusedExpID);
  }
//  currentThreadStrENUM = RANK;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getHostList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_hosts.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_hosts, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getHostList, ran %s, list_of_hosts.size()=%d\n", command.ascii(), list_of_hosts.size() );
#endif

  if( list_of_hosts.size() > 1 )
  {
    for( std::list<std::string>::const_iterator it = list_of_hosts.begin();
         it != list_of_hosts.end(); it++ )
    {
      std::string host = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getHostList, host=(%s)\n", host.c_str() );
#endif
    }
  }
 } else {
  list_of_hosts.clear();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getHostList, not valid exp_id=%d, no hosts\n", exp_id);
#endif
 }
}

void StatsPanel::getExecutableList(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExecutableList exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadStrENUM = UNKNOWN;
 if( exp_id > 0 || focusedExpID > 0 ) {
  if( focusedExpID == -1 )
  {
    command = QString("list -v executable -x %1").arg(exp_id);
  } else
  {
    command = QString("list -v executable -x %1").arg(focusedExpID);
  }
//  currentThreadStrENUM = RANK;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExecutableList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_executables.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_executables, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExecutableList, ran %s, list_of_executables.size()=%d\n", command.ascii(), list_of_executables.size() );
#endif

  if( list_of_executables.size() > 1 )
  {
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      std::string executable = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getExecutableList, executable=(%s)\n", executable.c_str() );
#endif
    }
  }
 } else {
  list_of_executables.clear();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExecutableList, not valid exp_id=%d, no executables\n", exp_id);
#endif

 }
}


void StatsPanel::getApplicationCommand(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getApplicationCommand exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 if( exp_id > 0 || focusedExpID > 0 ) {
//  currentThreadStrENUM = UNKNOWN;
  if( focusedExpID == -1 ) {
    command = QString("list -v appcommand -x %1").arg(exp_id);
  } else {
    command = QString("list -v appcommand -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getApplicationCommand-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_appcommands.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_appcommands, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getApplicationCommand, ran %s, list_of_appcommands.size()=%d\n", command.ascii(), list_of_appcommands.size() );
#endif

  if( list_of_appcommands.size() > 1 )
  {
    for( std::list<std::string>::const_iterator it = list_of_appcommands.begin();
         it != list_of_appcommands.end(); it++ )
    {
      std::string appcommands = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getApplicationCommand, appcommands=(%s)\n", appcommands.c_str() );
#endif
    }
  }
 } else {
  list_of_appcommands.clear();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getApplicationCommand, not valid exp_id=%d, no appcommand\n", exp_id);
#endif
 }
}


void StatsPanel::getExperimentType(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

 if( exp_id > 0 || focusedExpID > 0 ) {
//  currentThreadStrENUM = UNKNOWN;
  if( focusedExpID == -1 ) {
    command = QString("list -v types -x %1").arg(exp_id);
  } else {
    command = QString("list -v types -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_types.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_types, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType, ran %s, list_of_types.size()=%d\n", command.ascii(), list_of_types.size() );
#endif

  if( list_of_types.size() > 1 )
  {
    for( std::list<std::string>::const_iterator it = list_of_types.begin();
         it != list_of_types.end(); it++ )
    {
      std::string types = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getExperimentType, types=(%s)\n", types.c_str() );
#endif
    }
  }
 } else {
  list_of_types.clear();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getExperimentType, not valid exp_id=%d, no types\n", exp_id);
#endif
 }
}

#ifdef DBNAMES
void StatsPanel::getDatabaseName(int exp_id)
{
// Now get the executables
  QString command = QString::null;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName exp_id=%d, focusedExpID=%d\n", exp_id, focusedExpID);
#endif

//  currentThreadStrENUM = UNKNOWN;
  if( focusedExpID == -1 )
  {
    command = QString("list -v database -x %1").arg(exp_id);
  } else
  {
    command = QString("list -v database -x %1").arg(focusedExpID);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_dbnames.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_dbnames, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getDatabaseName, ran %s, list_of_dbnames.size()=%d\n", command.ascii(), list_of_dbnames.size() );
#endif

  if( list_of_dbnames.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      std::string databaseName = *it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::getDatabaseName, databaseName=(%s)\n", databaseName.c_str() );
#endif
    }
  }
}
#endif

void StatsPanel::updateMetadataForCompareIndication( QString compareStr )
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForCompareIndication, ENTER compareStr=(%s)\n", 
         compareStr.ascii() );
  printf("StatsPanel::updateMetadataForCompareIndication, ENTER infoSummaryStr.isEmpty=(%d)\n", 
         infoSummaryStr.isEmpty() );

  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateMetadataForCompareIndication, ENTER infoSummaryStr=(%s)\n", 
            infoSummaryStr.ascii() );
  }
#endif

  if (!infoSummaryStr.isEmpty()) {
      QString tempStr = QString("\nView consists of comparison columns %1").arg(compareStr);
      infoSummaryStr.append(tempStr);
  } else {
      infoSummaryStr = QString("View consists of comparison columns %1\n").arg(compareStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForCompareIndication, EXIT infoSummaryStr=(%s)\n", 
         infoSummaryStr.ascii() );
#endif
}


void StatsPanel::updateMetadataForTimeLineView( QString intervalStr )
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForTimeLineView, ENTER intervalStr=(%s)\n", 
         intervalStr.ascii() );
  printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoSummaryStr.isEmpty=(%d)\n", 
         infoSummaryStr.isEmpty() );

  printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoString.isEmpty=(%d)\n", 
         infoString.isEmpty() );

  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoSummaryStr=(%s)\n", 
            infoSummaryStr.ascii() );
  }
  if (!infoString.isEmpty()) {
    printf("StatsPanel::updateMetadataForTimeLineView, ENTER infoString=(%s)\n", 
            infoString.ascii() );
  }
#endif

  if (!infoSummaryStr.isEmpty()) {
      QString tempStr = QString("\nView consists of a time segment covering this percentage range %1").arg(intervalStr);
      infoSummaryStr.append(tempStr);
  } else {
      infoSummaryStr = QString("View consists of a time segment covering this percentage range: %1 for\n").arg(intervalStr);
  }

  if (!infoString.isEmpty()) {
      QString tempStr = QString("\nView consists of a time segment covering this percentage range %1").arg(intervalStr);
      infoString.append(tempStr);
  } else {
      infoString = QString("View consists of a time segment covering this percentage range: %1 for\n").arg(intervalStr);
  }
    
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateMetadataForTimeLineView, EXIT infoSummaryStr=(%s)\n", 
         infoSummaryStr.ascii() );
    
  printf("StatsPanel::updateMetadataForTimeLineView, EXIT infoString=(%s)\n", 
         infoString.ascii() );
#endif
}

void StatsPanel::checkForDashI()
{

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::checkForDashI, entered, lastCommand=(%s)\n", lastCommand.ascii() );
  printf("StatsPanel::checkForDashI, entered, infoAboutString=(%s)\n", infoAboutString.ascii() );
  printf("StatsPanel::checkForDashI, entered, infoSummaryStr=(%s)\n", infoSummaryStr.ascii() );
#endif

 int dashI_index = lastCommand.find("-I %");
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::checkForDashI dashI_index=%d\n", dashI_index);
#endif

 if( dashI_index != -1 ) {

   int new_index = findNextMajorToken(lastCommand, dashI_index+4, QString("\n"));

   QString intervalStr = lastCommand.mid(dashI_index+5, (new_index-(dashI_index+3)));
   
#ifdef DEBUG_StatsPanel
   printf("SP::checkForDashI -I case, dashI_index=%d, new_index=%d, (new_index-(dashI_index+4))=%d, intervalStr=%s\n",
           dashI_index, new_index, (new_index-(dashI_index+4)), intervalStr.ascii());
#endif

   updateMetadataForTimeLineView(intervalStr);
 }

}

// ----------------------------------------------
// ----------------------------------------------
// StatsPanel::getPartialExperimentInfo()
// ----------------------------------------------
// ----------------------------------------------

QString StatsPanel::getPartialExperimentInfo()
{
  QString returnString = QString::null;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPartialExperimentInfo, entered, aboutOutputString=(%s)\n", aboutOutputString.ascii() );
  printf("StatsPanel::getPartialExperimentInfo, entered, lastCommand=(%s)\n", lastCommand.ascii() );
  printf("StatsPanel::getPartialExperimentInfo, entered, infoAboutString=(%s)\n", infoAboutString.ascii() );
#endif

  int cviewinfo_index = lastCommand.find("cview ");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPartialExperimentInfo, cview-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif

  if( cviewinfo_index == -1 ) {
     cviewinfo_index = lastCommand.find("expView ");

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::getPartialExperimentInfo, expview-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif

     // Need more than just the expview to continue, must be specifications that are only present
     // if a subset of the entire experiment is being displayed.  -h host is a check to make?
     if( cviewinfo_index != -1 ) {
        cviewinfo_index = lastCommand.find("-h");
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::getPartialExperimentInfo, host-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif
     }
     if( cviewinfo_index == -1 ) {
        cviewinfo_index = lastCommand.find("-r");
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::getPartialExperimentInfo, rank-check, cviewinfo_index=(%d)\n", cviewinfo_index );
#endif
     }
  }
  if( cviewinfo_index != -1 ) {

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::getPartialExperimentInfo, infoAboutComparingString.isEmpty()=(%d)\n", infoAboutComparingString.isEmpty() );
       if (!infoAboutComparingString.isEmpty()) {
          printf("StatsPanel::getPartialExperimentInfo, infoAboutComparingString.ascii()=(%s)\n", infoAboutComparingString.ascii() );
       }
        printf("StatsPanel::getPartialExperimentInfo, infoSummaryStr.isEmpty()=(%d)\n", infoSummaryStr.isEmpty() );
       if (!infoAboutComparingString.isEmpty()) {
          printf("StatsPanel::getPartialExperimentInfo, infoSummaryStr.ascii()=(%s)\n", infoSummaryStr.ascii() );
       }
#endif

    if (!infoAboutString.isEmpty()) {
      returnString = QString("\n%1\n").arg(infoAboutString);
    }

  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getPartialExperimentInfo, return(%s)\n", returnString.ascii() );
#endif
  return (returnString);
}

void StatsPanel::updateStatsPanelInfoHeader(int exp_id)
{
  QString partialExperimentViewInfo;


//  if ( isHeaderInfoAlreadyProcessed(exp_id)) {
//
//#ifdef DEBUG_StatsPanel
//     printf("StatsPanel::updateStatsPanelInfoHeader, EXIT EARLY, exp_id=(%d) already processed\n", exp_id );
//#endif
//     
//     return; 
//  } else {
//     setHeaderInfoAlreadyProcessed(exp_id);
//  }

  int previous_pid = -1;
  int max_range_pid = -1;
  int min_range_pid = -1;

  // Initialize the summary string
  infoSummaryStr = QString("");


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader, entered, ++++++++++++++++++++ lastCommand=(%s)\n", lastCommand.ascii() );
#endif

  list_of_hosts.clear();
  list_of_pids.clear();
  list_of_executables.clear();
  list_of_appcommands.clear();
  list_of_types.clear();

  getExperimentType(exp_id);
  getApplicationCommand(exp_id);
  getExecutableList(exp_id);
#ifdef DBNAMES
  getDatabaseName(exp_id);
#endif

  int cviewinfo_aux_index = -1;
  int cviewinfo_index = lastCommand.find("cview ");

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader, cview-check, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
#endif

  if( cviewinfo_index == -1 ) {
     cviewinfo_aux_index = lastCommand.find("expView ");

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelInfoHeader, expview-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     // Need more than just the expview to continue, must be specifications that are only present
     // if a subset of the entire experiment is being displayed.  -h host is a check to make?
     if( cviewinfo_aux_index != -1 ) {
        cviewinfo_aux_index = lastCommand.find("-h");

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::updateStatsPanelInfoHeader, host-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     }
     if( cviewinfo_aux_index == -1 ) {
        cviewinfo_aux_index = lastCommand.find("-r");

#ifdef DEBUG_StatsPanel
        printf("StatsPanel::updateStatsPanelInfoHeader, rank-check, cviewinfo_aux_index=(%d)\n", cviewinfo_aux_index );
#endif

     }
  }
  
  if( cviewinfo_index != -1 || cviewinfo_aux_index != -1 ) {

    partialExperimentViewInfo = getPartialExperimentInfo();
    if (!partialExperimentViewInfo.isEmpty()) {
      partialExperimentViewInfo.insert(0,"\n  Partial Experiment View Information:");
    }
    partialExperimentViewInfo += QString("\n  Full Experiment Information:");

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, host-check, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
    printf("StatsPanel::updateStatsPanelInfoHeader, partialExperimentViewInfo.ascii()=(%s)\n", partialExperimentViewInfo.ascii() );
#endif
  }

  getHostList(exp_id);
  getPidList(exp_id);

  infoString += QString("Metadata for Experiment %1:").arg(exp_id);

  // Check for partial time segment
  checkForDashI();

  // Prepend partial results followed by the metadata for the whole experiment

  if( cviewinfo_index != -1 || cviewinfo_aux_index != -1  ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, BEFORE ADDING partialExperimentViewInfo.ascii()=(%s)\n", partialExperimentViewInfo.ascii() );
      printf("StatsPanel::updateStatsPanelInfoHeader, infoString.ascii()=(%s)\n", infoString.ascii() );
#endif
      infoString += partialExperimentViewInfo;
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_appcommands.size()=%d\n", list_of_appcommands.size());
#endif
  if( list_of_appcommands.size() > 0 )
  {
    infoString += QString("\n  Application command: ");
    int appcommands_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_appcommands.begin();
         it != list_of_appcommands.end(); it++ )
    {
      appcommands_count = appcommands_count + 1;
      std::string appcommands = *it;
      QString appcommandsStr = QString("%1").arg(appcommands.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, appcommands=(%s)\n", appcommands.c_str() );
#endif
      infoString += QString(" %1 ").arg(appcommandsStr);
//      if (appcommands_count > 6) break;
    }

  }

#ifdef DBNAMES
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_dbnames.size()=%d\n", list_of_dbnames.size());
#endif
  if( list_of_dbnames.size() > 0 )
  {
    infoString += QString("\nDatabase Name: ");
    int dbnames_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      dbnames_count = dbnames_count + 1;
      std::string dbnames = *it;
      QString dbnamesStr = QString("%1").arg(dbnames.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, dbnames=(%s)\n", dbnames.c_str() );
#endif
      infoString += QString(" %1 ").arg(dbnamesStr);
    }

  }
#endif

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_executables.size()=%d\n", list_of_executables.size());
#endif
  if( list_of_executables.size() > 0 )
  {
    infoString += QString("\n  Executables: ");
    int executable_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      executable_count = executable_count + 1;
      std::string executable = *it;
      QString executableStr = QString("%1").arg(executable.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, executable=(%s)\n", executable.c_str() );
#endif
      infoString += QString(" %1 ").arg(executableStr);
//      if (executable_count > 6) break;
    }

  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_types.size()=%d\n", list_of_types.size());
#endif
  if( list_of_types.size() > 0 )
  {
    infoString += QString("\n  Experiment type: ");
    for( std::list<std::string>::const_iterator it = list_of_types.begin();
         it != list_of_types.end(); it++ )
    {
      std::string types = *it;
      QString typesStr = QString("%1").arg(types.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, types=(%s)\n", types.c_str() );
#endif
      infoString += QString(" %1 ").arg(typesStr);
    }

  }


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_hosts.size()=%d\n", list_of_hosts.size());
#endif
  if( list_of_hosts.size() > 0 )
  {
    infoString += QString("\n  Host(s): ");
    int host_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_hosts.begin();
         it != list_of_hosts.end(); it++ )
    {
      host_count = host_count + 1;
      std::string host = *it;
      QString infoHostStr = QString("%1").arg(host.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, host=(%s)\n", host.c_str() );
#endif
      infoString += QString(" %1 ").arg(infoHostStr);
//      if (host_count > 6) break;
    }

  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , list_of_pids.size()=%d\n", list_of_pids.size());
#endif
  if( list_of_pids.size() > 0 )
  {
    infoString += QString("\n  Processes, Ranks or Threads: ");
    bool first_time = true;
    int pid_count = 0;
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      pid_count = pid_count + 1;
      int pid = (int64_t)*it;
      QString pidStr = QString("%1").arg(pid);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, pid=%d, pid_count=%d, list_of_pids.size()=%d\n", pid, pid_count, list_of_pids.size() );
#endif

      // Handle first time or only one pid cases
      if( list_of_pids.size() == 1 ) {
          infoString += QString(" %1 ").arg(pidStr);
          break;
      } else if (first_time) {
          max_range_pid = pid;
          min_range_pid = pid;
          previous_pid = pid;
          first_time = false;
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::updateStatsPanelInfoHeader, FIRST TIME, min_range_pid=%d, max_range_pid=%d\n", 
                 min_range_pid, max_range_pid );
#endif
          continue;
      }

      if (pid > previous_pid ) {

#ifdef DEBUG_StatsPanel
         printf("StatsPanel::updateStatsPanelInfoHeader, pid>prev, previous_pid=%d, pid=%d\n", previous_pid, pid );
#endif

        if (pid == previous_pid + 1  && (pid_count != list_of_pids.size()) ) {

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::updateStatsPanelInfoHeader, pid==prev+1, before(max_range_pid=%d), pid=%d\n", max_range_pid, pid );
#endif
          max_range_pid = pid;

        } else {

#ifdef DEBUG_StatsPanel
          printf("StatsPanel::updateStatsPanelInfoHeader, NOT pid==prev+1, max_range_pid=%d, min_range_pid=%d\n", max_range_pid, min_range_pid );
#endif
          if (max_range_pid != min_range_pid && (pid_count != list_of_pids.size()) ) {

            QString maxPidStr = QString("%1").arg(max_range_pid);
            infoString += QString(" %1 ").arg(maxPidStr);

          } else {

            // if in a range creation and you get to the end of the for
            // need to update the last item and output the range
            if  (pid_count == list_of_pids.size()) {
               max_range_pid = pid;
            }

            // You've arrived here because you encountered a value that is
            // not consequitive or we have gotten to the end of the pids 
            // and need to output the range creation prior to leaving the loop.

            QString maxPidStr = QString("%1").arg(max_range_pid);
            QString minPidStr = QString("%1").arg(min_range_pid);
            infoString += QString(" %1-%2 ").arg(minPidStr).arg(maxPidStr);
            min_range_pid = pid;
            max_range_pid = pid;
          } 

        } // end else -> not (pid == previous_pid + 1)



      } else {
#ifdef DEBUG_StatsPanel
       printf("ERROR - pids not ascending\n");
#endif
      } 
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelInfoHeader, SET prev at end of for, previous_pid=%d, pid=%d\n", previous_pid, pid );
#endif
     previous_pid = pid;
    } // end for

  }

// All the key data items have been processed into the infoString now...
// We now attempt to create a summary string that will always be shown
// in the stats panel.  This should contain the number of processes if > 1
// or the process if == 1.  Same with hosts, etc.


  // Process PID/RANK/THREAD information first

 if( list_of_executables.size() > 0 )
  {
//    infoSummaryStr += QString("<b>Executables:</b> ");
    infoSummaryStr += QString("Executables: ");
    for( std::list<std::string>::const_iterator it = list_of_executables.begin();
         it != list_of_executables.end(); it++ )
    {
      std::string executable = *it;
      QString executableStr = QString("%1").arg(executable.c_str());
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, executable=(%s)\n", executable.c_str() );
#endif
      infoSummaryStr += QString(" %1 ").arg(executableStr);
    }

   }

  int index_host_start = -1;
  int index_host_end = -1;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader, cviewinfo_index=%d, cviewinfo_aux_index=%d\n", 
          cviewinfo_index, cviewinfo_aux_index );
#endif

  if( cviewinfo_index != -1  || cviewinfo_aux_index != -1 ) {
  // if this is the case we have partial results or a comparison

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, infoString=(%s)\n", infoString.ascii() );
#endif

//    infoSummaryStr += QString("\nNot implemented yet ");
 
   // Look for number of hosts and a specific one to output for the summary string, 
   // find start/end character positions
   QString infoHostStr = QString::null;
   int host_count = infoString.contains("-h", TRUE);
   if (host_count > 0) {  
    index_host_start = infoString.find("-h ");
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, index_host_start=(%d)\n", index_host_start );
#endif
    if (index_host_start != -1) {
     index_host_end = infoString.find("-r");
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,index_host_end=(%d)\n", index_host_end );
#endif

     if (index_host_end == -1) {
       index_host_end = infoString.find("-p");
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::updateStatsPanelInfoHeader, process,index_host_end=(%d)\n", index_host_end );
#endif
     } 

     if (index_host_end == -1) {
       index_host_end = infoString.find("-t");
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::updateStatsPanelInfoHeader, thread,index_host_end=(%d)\n", index_host_end );
#endif
     }

      infoHostStr = infoString.mid((index_host_start+3), (index_host_end-index_host_start-4) );

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, index_host_start+3=(%d)\n", (index_host_start+3));
      printf("StatsPanel::updateStatsPanelInfoHeader, index_host_end-index_host_start-4=(%d)\n", (index_host_end-index_host_start-4));
      printf("StatsPanel::updateStatsPanelInfoHeader, infoHostStr=(%s)\n", infoHostStr.ascii() );
#endif
    }

    if (host_count == 1) {
//      infoSummaryStr += QString("<b>Host:</b> ");
      infoSummaryStr += QString("Host: ");
    } else {
//      infoSummaryStr += QString("<b>Hosts:</b>(%1) ").arg(host_count);
      infoSummaryStr += QString("Hosts:(%1) ").arg(host_count);
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, BFOR adding infoHostStr=(%s) to infoSummaryStr=(%s)\n", 
           infoHostStr.ascii(), infoSummaryStr.ascii()  );
#endif

    infoSummaryStr += QString(" %1 ").arg(infoHostStr);

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, AFTER adding infoHostStr=(%s) to infoSummaryStr=(%s)\n", 
           infoHostStr.ascii(), infoSummaryStr.ascii()  );
#endif

    if (host_count > 1) {
      infoSummaryStr += QString("... ");
    }
  }
 
   // Look for number of pids/ranks/threads and a specific one to output for the summary string, 
   // find start/end character positions
   int index_pid_start = -1;
   int index_pid_end = -1;
   QString pidStr;
   QString searchStr =("-r ");
   int pid_count = infoString.contains("-r", TRUE);
   if (pid_count == 0) {  
      pid_count = infoString.contains("-t", TRUE);
      searchStr =("-t ");
      if (pid_count == 0) {  
        pid_count = infoString.contains("-p", TRUE);
        searchStr =("-p ");
      }
   }
   
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelInfoHeader, pid_count=(%d)\n", pid_count );
   printf("StatsPanel::updateStatsPanelInfoHeader, searchStr.ascii()=(%s)\n", searchStr.ascii() );
#endif

   if (pid_count > 0) {  
    index_pid_start = infoString.find(searchStr.ascii(), 0, TRUE);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_start=(%d)\n", index_pid_start );
#endif
    if (index_pid_start != -1) {
     index_pid_end = infoString.find('\n', index_pid_start+3, TRUE);
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,EOL search index_pid_end=(%d)\n", index_pid_end );
     printf("StatsPanel::updateStatsPanelInfoHeader, rank,infoString.length()=(%d)\n", infoString.length() );
#endif
     if (index_pid_end == -1) {
       index_pid_end = infoString.find(" ", index_pid_start+3, TRUE);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::updateStatsPanelInfoHeader, rank, BLANKS search, index_pid_end=(%d)\n", index_pid_end );
#endif
     }

     if (index_pid_end != -1) {
      pidStr = infoString.mid((index_pid_start+3), (index_pid_end-index_pid_start-3) );
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_start+3=(%d)\n", (index_pid_start+3));
      printf("StatsPanel::updateStatsPanelInfoHeader, index_pid_end-index_pid_start-3=(%d)\n", (index_pid_end-index_pid_start-3));
      printf("StatsPanel::updateStatsPanelInfoHeader, pidStr=(%s)\n", pidStr.ascii() );
#endif

      if (pid_count == 1) {
        infoSummaryStr += QString("Process/Rank/Thread: ");
//        infoSummaryStr += QString("<b>Pid/Rank/Thread:</b> ");
      } else {
        infoSummaryStr += QString("Processes/Ranks/Threads:(%1) ").arg(pid_count);
//        infoSummaryStr += QString("<b>Processes/Ranks/Threads:</b>(%1) ").arg(pid_count);
      }
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, BFOR adding pidStr=(%s) to infoSummaryStr=(%s)\n", 
             pidStr.ascii(), infoSummaryStr.ascii() );
#endif

      infoSummaryStr += QString(" %1 ").arg(pidStr);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelInfoHeader, AFTER adding pidStr=(%s) to infoSummaryStr=(%s)\n", 
             pidStr.ascii(), infoSummaryStr.ascii() );
#endif

      if (pid_count > 1) {
        infoSummaryStr += QString("... ");
      }
     } // have a valid end
    } // have a valid start
  } // have some pids/ranks/threads

  } else {
   // if this is the case we have full results and should be able to use
   // the list_.... items to create the summary.
   // Process HOST information first

   if (list_of_hosts.size() == 1) {
//    infoSummaryStr += QString("<b>Host:</b> ");
     infoSummaryStr += QString("Host: ");
   } else {
     infoSummaryStr += QString("Hosts:(%1) ").arg(list_of_hosts.size());
//    infoSummaryStr += QString("<b>Hosts:</b>(%1) ").arg(list_of_hosts.size());
   }
   std::list<std::string>::const_iterator host_it = list_of_hosts.begin();
   std::string host = *host_it;
   QString infoHostStr = QString("%1").arg(host.c_str());
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelInfoHeader, host=(%s)\n", host.c_str() );
#endif
   infoSummaryStr += QString(" %1 ").arg(infoHostStr);

   if (list_of_hosts.size() > 1) {
     infoSummaryStr += QString("... ");
   }

  // Process PID/RANK/THREAD information first

   if (list_of_pids.size() == 1) {
     infoSummaryStr += QString("Pid/Rank/Thread: ");
//    infoSummaryStr += QString("\n<b>Pid/Rank/Thread:</b> ");
   } else {
     infoSummaryStr += QString("Processes/Ranks/Threads:(%1) ").arg(list_of_hosts.size());
//    infoSummaryStr += QString("\n<b>Processes/Ranks/Threads:</b>(%1) ").arg(list_of_hosts.size());
   }

   std::list<int64_t>::const_iterator pid_it = list_of_pids.begin();
   int summary_pid = (int64_t)*pid_it;
   QString pidStr = QString("%1").arg(summary_pid);
   infoSummaryStr += QString(" %1 ").arg(pidStr);

   if (list_of_pids.size() > 1) {
     infoSummaryStr += QString("... ");
   }

 
 } // cviewinfo_index if/else

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoAboutComparingString.isEmpty()=%d\n", infoAboutComparingString.isEmpty());
  if (!infoAboutComparingString.isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , infoAboutComparingString.ascii()=%s\n", infoAboutComparingString.ascii());
  }
  if (!infoSummaryStr.isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , infoSummaryStr.ascii()=%s\n", infoSummaryStr.ascii());
  }
#endif

  if (!infoAboutComparingString.isEmpty() || cviewinfo_index != -1) {
    // put out compare indication in the summary statement
    QString cStr = QString("click on the metadata icon for details.");
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelInfoHeader() , calling updateMetadataForCompareIndication\n");
#endif
    updateMetadataForCompareIndication( cStr );
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoString.ascii()=%s\n", infoString.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , infoSummaryStr.ascii()=%s\n", infoSummaryStr.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , ADDING TO metaDataTextEdit->text().isEmpty()=%d\n", metaDataTextEdit->text().isEmpty());
  if (!metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , ADDING TO metaDataTextEdit->text().ascii()=(%s)\n", metaDataTextEdit->text().ascii());
  }
#endif

  infoEditHeaderLabel->setText(infoSummaryStr);
  infoEditHeaderLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, infoEditHeaderLabel->sizePolicy().hasHeightForWidth() ) );


//#ifdef TEXT
  if (!infoAboutComparingString.isEmpty()) {
    metaDataTextEdit->setText(infoAboutComparingString);
    metaDataTextEdit->append(QString("\n"));
    infoAboutComparingString = QString::null;
  } 
  if (metaDataTextEdit->text().isEmpty()) {
    metaDataTextEdit->setText(infoString);
  } else {
    metaDataTextEdit->append(infoString);
  }
  infoString = QString("");
  metaDataTextEdit->setCursorPosition(0, 0);
  metaDataTextEdit->hide();
#if MORE_BUTTON
  infoEditHeaderMoreButton->setText( tr( "More Metadata" ) );
#else
  metadataToolButton->setIconSet( QIconSet(*MoreMetadata_icon));
  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));
#endif
  metaDataTextEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, metaDataTextEdit->sizePolicy().hasHeightForWidth() ) );
  metaDataTextEditFLAG = FALSE;
//#endif

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelInfoHeader() , exitting +++++++++++++++++++++++ with infoString.ascii()=%s\n", infoString.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , exitting metaDataTextEdit->text().isEmpty()=%d\n", metaDataTextEdit->text().isEmpty());
  if (!metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() , exitting metaDataTextEdit->text().ascii()=(%s)\n", metaDataTextEdit->text().ascii());
  }
#endif

}

// processing_preference indicates whether the data should
// be forced to be updated or not.  If not, let the other
// parameters/situations determine whether the data should be updated.

void
StatsPanel::updateStatsPanelData(bool processing_preference, QString command)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, ENTERING -------------------------\n");
#endif

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::updateStatsPanelData() entered., currentCollectorStr=%s, command=%s\n", 
        currentCollectorStr.ascii(), command.ascii() );
#endif

  levelsToOpen = getPreferenceLevelsToOpen().toInt();
  infoAboutString = QString("");
  infoAboutComparingString = QString("");

  SPListViewItem *splvi;
  columnHeaderList.clear();

  // Reinitialize these flags because of the "hiding of the no data message"
  // The no data message caused the splv (stats panel data) and cf (chart form) to be hidden
  splv->show();
  if (chartFLAG) {
    cf->show();
  }
  sml->hide();
//  metadataAllSpaceFrame->resize( metadataAllSpaceFrame->sizeHint() );


  // Percent value list (for the chart)
  cpvl.clear();
  // Text value list (for the chart)
  ctvl.clear();
  color_names = NULL;
#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, CHART, calling cf->init()\n" );
#endif
  cf->init();
  total_percent = 0.0;
  numberItemsToDisplayInStats = -1;
  if( !getPreferenceTopNLineEdit().isEmpty() )
  {
    bool ok;
    numberItemsToDisplayInStats = getPreferenceTopNLineEdit().toInt(&ok);
  }
  numberItemsToDisplayInChart = 5;
  if( !getPreferenceTopNChartLineEdit().isEmpty() )
  {
    bool ok;
    numberItemsToDisplayInChart = getPreferenceTopNChartLineEdit().toInt(&ok);
  }
#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, CHART, numberItemsToDisplayInChart = %d\n", numberItemsToDisplayInChart );
#endif

  textENUM = getPreferenceShowTextInChart();
// printf("updateStatsPanelData,textENUM=%d\n", textENUM );

  lastlvi = NULL;
  gotHeader = FALSE;
  gotColumns = FALSE;
//  fieldCount = 0;
  percentIndex = -1;


#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData, command.isEmpty()= %d\n", command.isEmpty() );
#endif
  if( command.isEmpty() )
  {
    command = generateCommand();
  } else
  {
    aboutString = "Compare/Customize report for:\n  ";
#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData, NOT command.isEmpty() aboutString.ascii()=%s\n", aboutString.ascii() );
#endif
  }

  if( command.isEmpty() )
  {
    return;
  }

#ifdef DEBUG_StatsPanel
  printf("  updateStatsPanelData, calling generateToolBar(), currentCollectorStr = %s\n", currentCollectorStr.ascii() );
  printf("  updateStatsPanelData, lastCommand = %s  command = %s\n", lastCommand.ascii(), command.ascii() );
#endif
  generateToolBar();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, getPreferenceShowToolbarCheckBox() == TRUE=%d\n",( getPreferenceShowToolbarCheckBox() == TRUE ));
#endif

  if( toolBarFLAG == TRUE ) {
    fileTools->show();
  } else {
    fileTools->hide();
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, about to append timeIntervalString = %s  to command = %s\n", 
          timeIntervalString.ascii(), command.ascii() );
#endif
  // ---------------------------------------------------------
  // ---------------------------------------------------------
  // There is some ordering issues with when the command 
  // was created and when the time segment is (can be) changed.
  // So, to rectify that issue, we look for the previous time
  // segment and replace it with the new time segment range.
  // ---------------------------------------------------------
  // ---------------------------------------------------------
  int timeIndex = -1;
  if (!prevTimeIntervalString.isEmpty() ) {
    timeIndex = command.find(prevTimeIntervalString);
  }
  if (timeIndex == -1) {
    // didn't find previous time interval (-I % nn:mm)
    command += timeIntervalString;
  } else {
    // did find previous time interval (-I % nn:mm)
    // so fix up by replacing with the new interval
    int prevLength = prevTimeIntervalString.length();
    command = command.replace( timeIndex, prevLength, timeIntervalString);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData, replaced prevTimeIntervalString =%s with timeIntervalString = %s  in command = %s\n", 
            prevTimeIntervalString.ascii(), timeIntervalString.ascii(), command.ascii() );
#endif
  }

  if( recycleFLAG == FALSE ) {

    // fire up a new stats panel and send "command" to it.
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData,fire up a new stats panel and send (%s) to it.\n", command.ascii() );
#endif
    int exp_id = expID;
    if( expID == -1  )
    {
      exp_id = groupID;
    }
     
    // JEG - might be able to do something here for rerun stats panels
    // maybe tag this string with the rerun count?
    // 8/7/2007

    QString name = QString("Stats Panel [%1]").arg(exp_id);
    Panel *sp = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    if( !sp )
    {
      char *panel_type = "Stats Panel";
      ArgumentObject *ao = new ArgumentObject("ArgumentObject", exp_id);
      sp = getPanelContainer()->dl_create_and_add_panel(panel_type, getPanelContainer(), ao);
      delete ao;
    } 

    if( sp ) {

      UpdateObject *msg = new UpdateObject((void *)NULL, -1, command.ascii(), 1);
      sp->listener( (void *)msg );
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, calling listener with UPDATEOBJECT,command.ascii()=%s\n", command.ascii() );
#endif

    } // end sp

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData,setting aboutString.ascii()=%s to lastAbout.ascii()=%s\n", 
           aboutString.ascii(), lastAbout.ascii() );
#endif

    aboutString = lastAbout;
    return;
  } // end recycleFLAG


  // processing_preference either forces and update of the data or not, depending
  // on it's value. false == no update, true == force update

  if( staticDataFLAG == TRUE && command == lastCommand && !processing_preference )
  {  // Then we really don't need to update.
#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,We really have static data and its the same command... Don't update.\n");
#endif
    return;
  }

  if( pd )
  {
    delete pd;
  }
  if( progressTimer )
  {
    delete progressTimer;
  }

#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,create the progressTimer\n");
#endif
  steps = 0;
  pd = new GenericProgressDialog(this, "Executing Command:", TRUE );
  pd->infoLabel->setText( QString("Running command") );
  progressTimer = new QTimer( this, "progressTimer" );
  connect( progressTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
  pd->show();
//progressUpdate();
#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,sort command?, command.ascii()=%s\n", command.ascii());
  fflush(stdout);
  fflush(stderr);
#endif
  progressTimer->start(0);
  pd->infoLabel->setText( QString("Running command - %1").arg(command) );
  qApp->flushX();
  qApp->processEvents(1000);

#ifdef DEBUG_StatsPanel
  printf("updateStatsPanelData,sort command?, command.ascii()=%s\n", command.ascii());
#endif

  if( command.contains("-v Butterfly") || 
      command.contains("-v CallTrees") || 
      command.contains("-v TraceBacks") )
  {
    // Don't sort these report types..  If you get a request to sort then only
    // sort on the last column.

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,butterfly, calltree, or tracebacks, Don't sort this display.\n");
#endif

    splv->setSorting ( -1 );

  } else if( command.startsWith("cview -c") && command.contains("-m ") )
  { // CLUSTER.. Don't sort this one...

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,cview type - Don't sort this display.\n");
#endif

    splv->setSorting ( -1 );

  } else
  {
    // Set the resort to be the first column when a new report is requested.

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,Sort this display on column 0.\n");
#endif

    splv->setSorting ( 0, FALSE );

  }

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  insertDiffColumnFLAG = FALSE;

  splv->clear();
  for(int i=splv->columns();i>=0;i--)
  {
    splv->removeColumn(i-1);
  }

  QApplication::setOverrideCursor(QCursor::WaitCursor);

  aboutString += "Command issued: " + command + "\n";

  if( lastCommand.isEmpty() )
  {
#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,lastCommand is empty, The original command = (%s)\n", command.ascii() );
    printf("updateStatsPanelData,lastCommand is empty, The aboutString string = (%s)\n", aboutString.ascii() );
#endif
    originalCommand = command;
  }
  lastCommand = command;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before if,&statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip,statspanel_clip);
#endif

  if( statspanel_clip )
  {

#ifdef DEBUG_StatsPanel
    printf("updateStatsPanelData,C: statspanel_clip->Set_Results_Used(), clearing statspanel_clip\n");
#endif

    statspanel_clip->Set_Results_Used();
    statspanel_clip = NULL;
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData after set NULL,&statspanel_clip=0x%x,statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel:updateStatsPanelData, about to issue: command: (%s)\n", command.ascii() );
#endif

  statspanel_clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData after set with cli->run_App..,command.ascii()=%s,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", command.ascii(), &statspanel_clip, statspanel_clip);
#endif

#ifdef DEBUG_StatsPanel
  if( statspanel_clip == NULL )
     printf("StatsPanel:updateStatsPanelData, (statspanel_clip == NULL)=%d\n", (statspanel_clip == NULL) );
#endif

#if 0
  if( statspanel_clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
    progressTimer->stop();
    delete progressTimer;
    progressTimer = NULL;
    pd->hide();
    delete pd;
    pd = NULL;
    
//    return;
  }
#endif // 0

  Input_Line_Status status = ILO_UNKNOWN;

  if( !command.startsWith("cview -c")  ) {
#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData calling updateStatsPanelInfoHeader, expID=%d, lastCommand=%s, infoAboutString=%s\n",
           expID, lastCommand.ascii(), infoAboutString.ascii());
  printf("StatsPanel::updateStatsPanelInfoHeader() , calling updateStatsPanelInfoHeader, CLEARING metaDataTextEdit->text().isEmpty()=%d\n", metaDataTextEdit->text().isEmpty());
  if (metaDataTextEdit->text().isEmpty()) {
    printf("StatsPanel::updateStatsPanelInfoHeader() calling updateStatsPanelInfoHeader, CLEARING metaDataTextEdit->text().ascii()=(%s)\n", metaDataTextEdit->text().ascii());
  }
#endif

   metaDataTextEdit->setText("");

#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData CALLING UPDATESTATSPANELINFOHEADER, expID=%d\n", expID);
#endif

   updateStatsPanelInfoHeader(expID);
 }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before ref with cli->Seman..,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  while( !statspanel_clip->Semantics_Complete() )
  {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateStatsPanelData, pinging... while( !statspanel_clip->Semantics_Complete() )\n");
#endif

    qApp->flushX();
    qApp->processEvents(1000);
    sleep(1);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, done pinging... while( !statspanel_clip->Semantics_Complete() )\n");
#endif

  pd->infoLabel->setText( tr("Processing information for display") );
  pd->show();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData before call to process_clip..,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  process_clip(statspanel_clip, NULL, FALSE);

//  process_clip(statspanel_clip, NULL, TRUE);

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData after call to process_clip..,&statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  statspanel_clip->Set_Results_Used();
  statspanel_clip = NULL;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, Done processing the clip\n");
#endif

  pd->infoLabel->setText( tr("Analyze and sort the view.") );
  pd->show();

  analyzeTheCView();

  if( command.startsWith("cview -c")  ) {
#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData call updateStatsPanelInfoHeader, cview case, expID=%d, lastCommand=%s, infoAboutString=%s\n",
           expID, lastCommand.ascii(), infoAboutString.ascii());
#endif
   int start_expID_index = 0;
   int expIDindex = 0;
#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData BEFORE WHILE cview case, expIDindex=%d, start_expID_index=%d, infoAboutStringCompareExpIDs.length()=%d\n",
           expIDindex, start_expID_index, infoAboutStringCompareExpIDs.length());
   printf("SP::updateStatsPanelData BEFORE WHILE cview case, infoAboutStringCompareExpIDs.ascii()=%s\n",
           infoAboutStringCompareExpIDs.ascii());
   printf("StatsPanel::SP::updateStatsPanelData() BEFORE WHILE cview case,  CLEARING metaDataTextEdit->text().isEmpty()=%d\n", metaDataTextEdit->text().isEmpty());
   if (!metaDataTextEdit->text().isEmpty()) {
     printf("StatsPanel::SP::updateStatsPanelData() BEFORE WHILE cview case,  CLEARING metaDataTextEdit->text().ascii()=(%s)\n", metaDataTextEdit->text().ascii());
   }
#endif

   metaDataTextEdit->setText("");

   // Let the metadata for the compares be processed once only
   // If this isn't re-initialized here, then the updateStatsPanelInfoHeader would
   // return early, because the data was processed once already upon first presentation.
   setHeaderInfoAlreadyProcessed(-1);

   while (expIDindex != (infoAboutStringCompareExpIDs.length()-1) ) {
      expIDindex = infoAboutStringCompareExpIDs.find(",", start_expID_index);
      QString expIdStr = infoAboutStringCompareExpIDs.mid(start_expID_index, expIDindex-start_expID_index);
#ifdef DEBUG_StatsPanel
      printf("SP::updateStatsPanelData TOP OF WHILD cview case, start_expID_index=%d, (expIDindex-start_expID_index)=%d, expIDindex=%d, expIdStr=%s\n",
             start_expID_index, (expIDindex-start_expID_index), expIDindex, expIdStr.ascii());
#endif
     int compareExpID = expIdStr.toInt();
      
     if (compareExpID > 0)  {

#ifdef DEBUG_StatsPanel
         printf("SP::updateStatsPanelData CALLING UPDATESTATSPANELINFOHEADER, compareExpID=%d\n", compareExpID);
#endif

         if ( !isHeaderInfoAlreadyProcessed(compareExpID)) {
           updateStatsPanelInfoHeader(compareExpID);
         }
         // keep from repeating Metadata for the same experiment
         setHeaderInfoAlreadyProcessed(compareExpID);
     }
     start_expID_index = expIDindex + 1;
#ifdef DEBUG_StatsPanel
   printf("SP::updateStatsPanelData BOTTOM OF WHILE cview case, compareExpID=%d, start_expID_index=%d, expIDindex=%d\n",
           compareExpID, start_expID_index, expIDindex);
#endif
   }
 }

 // Let new view displays get their metadata
 setHeaderInfoAlreadyProcessed(-1);
 

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelData command.startsWith(cview -c)=%d, canWeDiff()=%d\n", 
          command.startsWith("cview -c"), canWeDiff());
#endif

  if( command.startsWith("cview -c") && canWeDiff() )
  {
    int insertColumn = 0;
    insertDiffColumn(insertColumn);
    insertDiffColumnFLAG = TRUE;
  
    // Force a resort in this case...
    splv->setSorting ( insertColumn, FALSE );
    splv->sort();
  }

// Put out the chart if there is one...
   color_names = hotToCold_color_names;
   if( descending_sort != true )
   {
    color_names = coldToHot_color_names;
   }

   if( textENUM == TEXT_NONE )
   {
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::updateStatsPanelData, CHART textENUM=%d (TEXT_NONE)\n", textENUM );
#endif

     ctvl.clear();
   }

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::updateStatsPanelData, CHART Put out the chart!!!!\n");
   printf("StatsPanel::updateStatsPanelData, CHART numberItemsToDisplayInStats=(%d) cpvl.count()=(%d)\n", numberItemsToDisplayInStats, cpvl.count() );
#endif

  // Do we need an other?
#ifdef DEBUG_StatsPanel
printf("StatsPanel::updateStatsPanelData, CHART: total_percent=%f splv->childCount()=%d cpvl.count()=%d numberItemsToDisplayInStats=%d\n", total_percent, splv->childCount(), cpvl.count(), numberItemsToDisplayInStats );

printf("StatsPanel::updateStatsPanelData, CHART: cpvl.count()=%d numberItemsToDisplayInChart = %d\n", cpvl.count(), numberItemsToDisplayInChart );
#endif

/*
  if( ( total_percent > 0.0 &&
      cpvl.count() < numberItemsToDisplayInStats) ||
      ( total_percent > 0.0 && 
        cpvl.count() < numberItemsToDisplayInStats &&
        numberItemsToDisplayInChart < numberItemsToDisplayInStats) )
*/

  {
    if( total_percent < 100.00 )
    {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, CHART, add other of %f\n", 100.00-total_percent );
#endif
      cpvl.push_back( (int)(100.00-total_percent) );
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateStatsPanelData, CHART, total_percent: textENUM=%d\n", textENUM );
#endif
      if( textENUM != TEXT_NONE )
      {
        ctvl.push_back( "other" );
      }
    }
  }

  // Or were there no percents in the initial query.
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, total_percent = %f\n", total_percent );
#endif

  if( total_percent == 0.0 )
  {
    cpvl.clear();
    ctvl.clear();
    cpvl.push_back(100);
    ctvl.push_back("N/A");
    // jeg added 10-08-07
    if( thisPC->getMainWindow()->preferencesDialog->showGraphicsCheckBox->isChecked() && chartFLAG) {
       showChart();
//       cf->hide();
//       chartFLAG = FALSE;
    }
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, cpvl.count()=%d ctvl.count()=%d\n", cpvl.count(), ctvl.count() );
#endif

  cf->setValues(cpvl, ctvl, color_names, MAX_COLOR_CNT);


#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, CHART, now clean up the timer...\n");
#endif

  progressTimer->stop();
  delete progressTimer;
  progressTimer = NULL;
  pd->hide();
  delete pd;
  pd = NULL;

  QApplication::restoreOverrideCursor();

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateStatsPanelData, EXITING -------------------------\n");
#endif

}

void
StatsPanel::updateToolBarStatus(QString optionChosen)
{ 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateToolBarStatus, optionChosen.ascii()=%s\n", optionChosen.ascii());
#endif

 if (optionChosen.contains("Functions") )
 {
  toolbar_status_label->setText("Showing Functions Report:");
 } 
 else if (optionChosen.contains("LinkedObjects") )
 {
  toolbar_status_label->setText("Showing Linked Objects Report:");
 } 
 else if (optionChosen.contains("Statements by Function") )
 {
  toolbar_status_label->setText("Showing Statements by Function Report:");
 } 
 else if (optionChosen.contains("Statements") )
 {
  toolbar_status_label->setText("Showing Statements Report:");
 } 
 else if (optionChosen.contains("CallTrees by Function") )
 {
  toolbar_status_label->setText("Showing CallTrees by Function Report:");
 } 
 else if (optionChosen.contains("CallTrees,FullStack Report") )
 {
   toolbar_status_label->setText("Showing CallTrees,FullStack Report:");
 } 
 else if (optionChosen.contains("CallTrees,FullStack by Function") )
 {
   toolbar_status_label->setText("Showing CallTrees,FullStack by Function Report:");
 }
 else if (optionChosen.contains("CallTrees") )
 {
  toolbar_status_label->setText("Showing CallTrees Report:");
 } 
 else if (optionChosen.contains("TraceBacks by Function") )
 {
   toolbar_status_label->setText("Showing TraceBacks by Function Report:");
 }
 else if (optionChosen.contains("TraceBacks,FullStack") )
 {
   toolbar_status_label->setText("Showing TraceBacks,FullStack Report:");
 }
 else if (optionChosen.contains("TraceBacks,FullStack by Function") )
 {
   toolbar_status_label->setText("Showing TraceBacks,FullStack by Function Report:");
 }
 else if (optionChosen.contains("TraceBacks") )
 {
   toolbar_status_label->setText("Showing TraceBacks Report:");
 }
 else if (optionChosen.contains("Butterfly") )
 {
   toolbar_status_label->setText("Showing Butterfly Report:");
 }
 else if (optionChosen.contains("Load Balance") )
 {
   toolbar_status_label->setText("Showing Load Balance (min,max,ave) Report:");
 }
 else if (optionChosen.contains("Compare and Analyze") )
 {
   toolbar_status_label->setText("Showing Comparative Analysis Report:");
 }

}

void
StatsPanel::threadSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected(%d)\n", val);
#endif


  currentThreadStr = threadMenu->text(val).ascii();

  currentThreadsStr = QString::null;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, threadMenu: selected text=(%s)\n", threadMenu->text(val).ascii() );
#endif


  bool FOUND_FLAG = FALSE;
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); 
                                      it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;

    if( ts == currentThreadStr )
    {   // Then it's already in the list... now remove it.
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::threadSelected, add the selected thread (%s).\n", ts.ascii() );
#endif
      currentThreadGroupStrList.remove(ts);
      FOUND_FLAG = TRUE;
      break;
    }
  }

  // We didn't find it to remove it, so this is a different thread... add it.
  if( FOUND_FLAG == FALSE )
  {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::threadSelected, We must need to add it (%s) then!\n", currentThreadStr.ascii() );
#endif
    currentThreadGroupStrList.push_back(currentThreadStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, Here's the string list...\n");
#endif
  for( ThreadGroupStringList::Iterator it = currentThreadGroupStrList.begin(); it != currentThreadGroupStrList.end(); ++it)
  {
    QString ts = (QString)*it;
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::threadSelected, A: ts=(%s)\n", ts.ascii() );
#endif
  
    if( ts.isEmpty() )
    {
      continue;
    }


if( currentThreadStrENUM == RANK )
{
    if( currentThreadsStr.isEmpty() )
    {
      currentThreadsStr = "-r "+ts;
    } else
    {
      currentThreadsStr += ","+ts;
    }
} else if( currentThreadStrENUM == THREAD )
{
    if( currentThreadsStr.isEmpty() )
    {
      currentThreadsStr = "-t "+ts;
    } else
    {
      currentThreadsStr += ","+ts;
    }
} else if( currentThreadStrENUM == PID )
{
    if( currentThreadsStr.isEmpty() )
    {
      currentThreadsStr = "-p "+ts;
    } else
    {
      currentThreadsStr += ","+ts;
    }
}
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::threadSelected, currentThreadsStr = %s call updateStatsPanelData.\n", currentThreadsStr.ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  // Now, try to focus the source panel on the first entry...
  QListViewItemIterator it( splv );
  if( it.current() )
  {
    int i = 0;
    QListViewItem *item = *it;
    StatsPanel::itemSelected(item);
  }
}

void
StatsPanel::collectorMetricSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::collectorMetricSelected val=%d\n", val);
#endif
// printf("collectorMetricSelected: currentCollectorStr=(%s)\n", popupMenu->text(val).ascii() );

  currentUserSelectedReportStr = QString::null;

  QString s = popupMenu->text(val).ascii();

// printf("A: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
//      collectorStrFromMenu = s.mid(13, index-13 );
      currentCollectorStr = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::collectorMetricSelected, BB1: s=(%s) currentCollectorStr=(%s) currentMetricStr=(%s)\n", s.ascii(), currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorMetricSelected, Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}

void
StatsPanel::collectorMPIReportSelected(int val)
{
  currentCollectorStr = "mpi";
  MPIReportSelected(val);
}

void
StatsPanel::collectorMPITReportSelected(int val)
{
  currentCollectorStr = "mpit";
  MPIReportSelected(val);
}

void
StatsPanel::MPIReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::MPIReportSelected val=%d\n", val);
  printf("StatsPanel::MPIReportSelected: mpi_menu=(%s)\n", mpi_menu->text(val).ascii() );
  printf("StatsPanel::MPIReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

  QString s = QString::null;
  s = mpi_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::MPIReportSelected:B: s=%s\n", s.ascii() );
#endif

  int index = s.find(":");
  if( index != -1 )
  {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::MPIReportSelected:D: NOW FIND ::\n");
#endif
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::MPIReportSelected, MPI1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorMPIReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorMPIReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::MPIReportSelected, currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
 printf("StatsPanel::MPIReportSelected, Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);
}

void
StatsPanel::collectorIOReportSelected(int val)
{ 
  currentCollectorStr = "io";
  IOReportSelected(val);
}

void
StatsPanel::collectorIOTReportSelected(int val)
{ 
  currentCollectorStr = "iot";
  IOReportSelected(val);
}


void
StatsPanel::IOReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("IOReportSelected val=%d\n", val);
  printf("IOReportSelected: io_menu=(%s)\n", io_menu->text(val).ascii() );
  printf("IOReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = io_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("C: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("IO1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorIOReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorIOReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}

void
StatsPanel::collectorHWCReportSelected(int val)
{ 
  currentCollectorStr = "hwc";
  HWCReportSelected(val);
}

void
StatsPanel::collectorHWCTimeReportSelected(int val)
{ 
  currentCollectorStr = "hwctime";
  HWCTimeReportSelected(val);
}


void
StatsPanel::HWCReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("HWCReportSelected: collectorMetricSelected val=%d\n", val);
  printf("HWCReportSelected: hwc_menu=(%s)\n", hwc_menu->text(val).ascii() );
  printf("HWCReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = hwc_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("D: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("DD1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
// printf("A: NULLING OUT selectedFunctionStr\n");
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorHWCReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorHWCReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::HWCTimeReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("HWCTimeReportSelected: collectorMetricSelected val=%d\n", val);
  printf("HWCTimeReportSelected: hwctime_menu=(%s)\n", hwctime_menu->text(val).ascii() );
  printf("HWCTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = popupMenu->text(val).ascii();
//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = hwctime_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("D: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("DD1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
// printf("B: NULLING OUT selectedFunctionStr\n");
      }
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorHWCTimeReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorHWCTimeReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
    printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
    printf("Collector changed call updateStatsPanelData() \n");
#endif
    updateStatsPanelData(DONT_FORCE_UPDATE);
  }
}


void
StatsPanel::collectorUserTimeReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("collectorUserTimeReportSelected: val=%d\n", val);
   printf("collectorUserTimeReportSelected: usertime_menu=(%s)\n", usertime_menu->text(val).ascii() );
   printf("collectorUserTimeReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "usertime";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = usertime_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("UserTimeReport: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorUsertimeReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorUsertimeReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);

}


void
StatsPanel::collectorPCSampReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: val=%d\n", val);
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: pcsamp_menu=(%s)\n", pcsamp_menu->text(val).ascii() );
  printf("StatsPanel::collectorPCSampReportSelected, collectorPCSampReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "pcsamp";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = pcsamp_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

#ifdef DEBUG_StatsPanel
 printf("PCSampReport: (%s)\n", s.ascii() );
#endif

  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics

      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
#ifdef DEBUG_StatsPanel
      printf("PCSampReport: currentCollectorStr=(%s) currentMetricStr=(%s)\n", 
             currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      // This one resets to all...

    } else { // The user wants to do all the metrics on the selected threads...

      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
#ifdef DEBUG_StatsPanel
      printf("PCSampReport: selectedFunctionStr=(%s),currentUserSelectedReportStr.ascii()=(%s), currentCollectorStr=(%s) currentMetricStr=(%s)\n", 
             selectedFunctionStr.ascii(), currentUserSelectedReportStr.ascii(), 
             currentCollectorStr.ascii(), currentMetricStr.ascii() );
#endif
      if( !currentUserSelectedReportStr.contains("Statements by Function") )
      {
        selectedFunctionStr = QString::null;
      }
    }
  }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorPCSampReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorPCSampReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::collectorPCSampReportSelected, currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("StatsPanel::collectorPCSampReportSelected, Collector changed call updateStatsPanelData() \n");
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

}


void
StatsPanel::collectorFPEReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorFPEReportSelected: val=%d\n", val);
  printf("collectorFPEReportSelected: fpe_menu=(%s)\n", fpe_menu->text(val).ascii() );
  printf("collectorFPEReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;
  currentCollectorStr = "fpe";
  collectorStrFromMenu = QString::null;
  currentMetricStr = QString::null;

//  QString s = contextMenu->text(val).ascii();
  QString s = QString::null;
  s = fpe_menu->text(val).ascii();
  if( s.isEmpty() )
  {
    s = contextMenu->text(val).ascii();
  }

// printf("FPE Report: (%s)\n", s.ascii() );

// printf("E: s=%s\n", s.ascii() );
  int index = s.find(":");
  if( index != -1 )
  {
// printf("DD: NOW FIND :\n");
    index = s.find(":");
    if( index > 0 )
    { // The user selected one of the metrics
      collectorStrFromMenu = s.mid(13, index-13 );
      currentUserSelectedReportStr = s.mid(index+2);
// printf("UT1: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
      // This one resets to all...
    } else 
    { // The user wants to do all the metrics on the selected threads...
      currentMetricStr = QString::null;
      index = s.find(":");
      currentUserSelectedReportStr = s.mid(13, index-13);
      if( !currentUserSelectedReportStr.contains("CallTrees by Selected Function") )
      {
        selectedFunctionStr = QString::null;
      }
// printf("UT2: currentCollectorStr=(%s) currentMetricStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii() );
    }

  // The status for the tool bar needs to reflect what is 
  // going on when the same features are selected via the menu
  if( getPreferenceShowToolbarCheckBox() == TRUE ) {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::collectorFPEReportSelected, currentUserSelectedReportStr = (%s)\n", currentUserSelectedReportStr.ascii() );
    printf("StatsPanel::collectorFPEReportSelected, calling updateToolBarStatus() \n");
#endif
    updateToolBarStatus( currentUserSelectedReportStr );
  } 

#ifdef DEBUG_StatsPanel
  printf("currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  }
  updateStatsPanelData(DONT_FORCE_UPDATE);


}


void
StatsPanel::collectorGenericReportSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("collectorGenericReportSelected: val=%d\n", val);
  printf("collectorGenericReportSelected: generic_menu=(%s)\n", generic_menu->text(val).ascii() );
  printf("collectorGenericReportSelected: contextMenu=(%s)\n", contextMenu->text(val).ascii() );
#endif

  currentUserSelectedReportStr = QString::null;

  currentMetricStr = QString::null;
  currentCollectorStr = QString::null;
  selectedFunctionStr = QString::null;
#ifdef DEBUG_StatsPanel
  printf("C: NULLING OUT selectedFunctionStr\n");
  printf("Collector changed call updateStatsPanelData() \n");
#endif
  updateStatsPanelData(DONT_FORCE_UPDATE);
}


void
StatsPanel::modifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::modifierSelected val=%d\n", val);
  printf("StatsPanel::modifierSelected: (%s)\n", modifierMenu->text(val).ascii() );
#endif

  if( modifierMenu->text(val).isEmpty() )
  {
    return;
  }


  std::string s = modifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::modifierSelected: B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_modifiers.begin();
       it != current_list_of_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_modifiers.remove(modifier);
      modifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_modifiers.push_back(s);
    }
    modifierMenu->setItemChecked(val, TRUE);
  }

// Uncomment this line if the modifier selection to take place immediately.
// I used to do this, but it seemed wrong to make the use wait as they 
// selected each modifier.   Now, they select the modifier, then go out and
// reselect the Query...
//  updateStatsPanelData();
#ifdef DEBUG_StatsPanel
  printf("exit modifierSelected \n");
#endif

}


void
StatsPanel::mpiModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
 printf("mpiModifierSelected val=%d\n", val);
 printf("mpiModifierSelected: (%s)\n", mpiModifierMenu->text(val).ascii() );
 printf("mpiModifierSelected: (%d)\n", mpiModifierMenu->text(val).toInt() );
#endif


  if( mpiModifierMenu->text(val).isEmpty() )
  {
// printf("Do you want to add the \"duplicate\" submenus?\n");
    mpiModifierMenu->insertSeparator();
    if( mpi_menu )
    {
      delete mpi_menu;
    }
    mpi_menu = new QPopupMenu(this);
    mpiModifierMenu->insertItem(QString("Select mpi Reports:"), mpi_menu);
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMPIReportSelected(int)) );
    return;
  }


  std::string s = mpiModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mpi_modifiers.begin();
       it != current_list_of_mpi_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The %s modifier was in the list ... take it out!\n", s.c_str() );
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
// printf("The %s modifier was in the list ... remove it!\n", s.c_str() );
      current_list_of_mpi_modifiers.remove(modifier);
      mpiModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The %s modifier was not in the list ... add it!\n", s.c_str() );
    if( s != PTI )
    {
      current_list_of_mpi_modifiers.push_back(s);
    }
    mpiModifierMenu->setItemChecked(val, TRUE);
  }
#ifdef DEBUG_StatsPanel
  printf("exit mpiModifierSelected \n");
#endif
}


void
StatsPanel::mpitModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
   printf("mpitModifierSelected val=%d\n", val);
   printf("mpitModifierSelected: (%s)\n", mpitModifierMenu->text(val).ascii() );
#endif


  if( mpitModifierMenu->text(val).isEmpty() )
  {
    mpitModifierMenu->insertSeparator();
    if( mpi_menu )
    {
      delete mpi_menu;
    }
    mpi_menu = new QPopupMenu(this);
    mpitModifierMenu->insertItem(QString("Select mpit Reports:"), mpi_menu);
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorMPITReportSelected(int)) );
    return;
  }


  std::string s = mpitModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_mpit_modifiers.begin();
       it != current_list_of_mpit_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_mpit_modifiers.remove(modifier);
      mpitModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_mpit_modifiers.push_back(s);
    }
    mpitModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::ioModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("ioModifierSelected val=%d\n", val);
  printf("ioModifierSelected: (%s)\n", ioModifierMenu->text(val).ascii() );
#endif

  if( ioModifierMenu->text(val).isEmpty() )
  {
    ioModifierMenu->insertSeparator();
    if( io_menu )
    {
      delete io_menu;
    }
    io_menu = new QPopupMenu(this);
    ioModifierMenu->insertItem(QString("Select io Reports:"), io_menu);
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorIOReportSelected(int)) );
    return;
  }


  std::string s = ioModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_io_modifiers.begin();
       it != current_list_of_io_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_io_modifiers.remove(modifier);
      ioModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_io_modifiers.push_back(s);
    }
    ioModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::iotModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("iotModifierSelected val=%d\n", val);
  printf("iotModifierSelected: (%s)\n", iotModifierMenu->text(val).ascii() );
#endif


  if( iotModifierMenu->text(val).isEmpty() )
  {
    iotModifierMenu->insertSeparator();
    if( io_menu )
    {
      delete io_menu;
    }
    io_menu = new QPopupMenu(this);
    iotModifierMenu->insertItem(QString("Select iot Reports:"), io_menu);
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorIOTReportSelected(int)) );
    return;
  }

  std::string s = iotModifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
   printf("iot,B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_iot_modifiers.begin();
       it != current_list_of_iot_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

#ifdef DEBUG_StatsPanel
      printf("Looping through the modifier list , modifier.c_str()=%s\n", modifier.c_str() );
#endif

    if( modifier ==  s )
    {   // It's in the list, so take it out...
#ifdef DEBUG_StatsPanel
      printf("The modifier was in the list ... take it out! (%s)\n", s.c_str());
#endif
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE ) {
      current_list_of_iot_modifiers.remove(modifier);
      iotModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE ) {
#ifdef DEBUG_StatsPanel
    printf("The modifier was not in the list ... add it!, (%s)\n", s.c_str());
    printf("iot,B1: modifierStr=(%s)\n", s.c_str() );
#endif
    if( s != PTI )
    {
      current_list_of_iot_modifiers.push_back(s);
    }
    iotModifierMenu->setItemChecked(val, TRUE);
  }

#ifdef DEBUG_StatsPanel
  printf("EXIT iot, (%s)\n", s.c_str());
  for( std::list<std::string>::const_iterator iit = current_list_of_iot_modifiers.begin();
       iit != current_list_of_iot_modifiers.end();  )
  {
    std::string x_modifier = (std::string)*iit;
    printf("EXITing iot, one of the modifiers that is in the list is:(%s)\n", x_modifier.c_str());
    iit++;
  }
#endif
}


void
StatsPanel::hwcModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("hwcModifierSelected val=%d\n", val);
  printf("hwcModifierSelected: (%s)\n", hwcModifierMenu->text(val).ascii() );
#endif

  if( hwcModifierMenu->text(val).isEmpty() )
  {
    hwcModifierMenu->insertSeparator();
    if( hwc_menu )
    {
      delete hwc_menu;
    }
    hwc_menu = new QPopupMenu(this);
    hwcModifierMenu->insertItem(QString("Select hwc Reports:"), hwc_menu);
    addHWCReports(hwc_menu);
    connect(hwc_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorHWCReportSelected(int)) );
    return;
  }


  std::string s = hwcModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_hwc_modifiers.begin();
       it != current_list_of_hwc_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_hwc_modifiers.remove(modifier);
      hwcModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_hwc_modifiers.push_back(s);
    }
    hwcModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::hwctimeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("hwctimeModifierSelected val=%d\n", val);
  printf("hwctimeModifierSelected: (%s)\n", hwctimeModifierMenu->text(val).ascii() );
#endif

  if( hwctimeModifierMenu->text(val).isEmpty() )
  {
    hwctimeModifierMenu->insertSeparator();
    if( hwctime_menu )
    {
      delete hwctime_menu;
    }
    hwctime_menu = new QPopupMenu(this);
    hwctimeModifierMenu->insertItem(QString("Select hwctime Reports:"), hwctime_menu);
    addHWCTimeReports(hwctime_menu);
    connect(hwctime_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorHWCTimeReportSelected(int)) );
    return;
  }


  std::string s = hwctimeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_hwctime_modifiers.begin();
       it != current_list_of_hwctime_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_hwctime_modifiers.remove(modifier);
      hwctimeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_hwctime_modifiers.push_back(s);
    }
    hwctimeModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::usertimeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("usertimeModifierSelected val=%d\n", val);
  printf("usertimeModifierSelected: (%s)\n", usertimeModifierMenu->text(val).ascii() );
#endif

  if( usertimeModifierMenu->text(val).isEmpty() )
  {
    usertimeModifierMenu->insertSeparator();
    if( usertime_menu )
    {
      delete usertime_menu;
    }
    usertime_menu = new QPopupMenu(this);
    usertimeModifierMenu->insertItem(QString("Select usertime Reports:"), usertime_menu);
    addUserTimeReports(usertime_menu);
    connect(usertime_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorUserTimeReportSelected(int)) );
    return;
  }


  std::string s = usertimeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_usertime_modifiers.begin();
       it != current_list_of_usertime_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_usertime_modifiers.remove(modifier);
      usertimeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_usertime_modifiers.push_back(s);
    }
    usertimeModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::pcsampModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::pcsampModifierSelected val=%d\n", val);
  printf("StatsPanel::pcsampModifierSelected: (%s)\n", pcsampModifierMenu->text(val).ascii() );
#endif

  if( pcsampModifierMenu->text(val).isEmpty() )
  {
    if( pcsampModifierMenu->text(val).isEmpty() )
    {
      pcsampModifierMenu->insertSeparator();
      if( pcsamp_menu )
      {
        delete pcsamp_menu;
      }
      pcsamp_menu = new QPopupMenu(this);
      pcsampModifierMenu->insertItem(QString("Select pcsamp Reports:"), pcsamp_menu);
      addPCSampReports(pcsamp_menu);
      connect(pcsamp_menu, SIGNAL( activated(int) ),
        this, SLOT(collectorPCSampReportSelected(int)) );
      return;
    }

    return;
  }


  std::string s = pcsampModifierMenu->text(val).ascii();
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::B1: modifierStr=(%s)\n", s.c_str() );
#endif

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_pcsamp_modifiers.begin();
       it != current_list_of_pcsamp_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::The modifier was in the list ... take it out!\n");
#endif
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_pcsamp_modifiers.remove(modifier);
      pcsampModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::The modifier was not in the list ... add it!\n");
#endif
    if( s != PTI )
    {
      current_list_of_pcsamp_modifiers.push_back(s);
    }
    pcsampModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::fpeModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::fpeModifierSelected val=%d\n", val);
  printf("StatsPanel::fpeModifierSelected: (%s)\n", fpeModifierMenu->text(val).ascii() );
#endif

  if( fpeModifierMenu->text(val).isEmpty() )
  {
    fpeModifierMenu->insertSeparator();
    if( fpe_menu )
    {
      delete fpe_menu;
    }
    fpe_menu = new QPopupMenu(this);
    fpeModifierMenu->insertItem(QString("Select fpe Reports:"), fpe_menu);
    addFPEReports(fpe_menu);
    connect(fpe_menu, SIGNAL( activated(int) ),
      this, SLOT(collectorFPEReportSelected(int)) );
    return;
  }


  std::string s = fpeModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_fpe_modifiers.begin();
       it != current_list_of_fpe_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_fpe_modifiers.remove(modifier);
      fpeModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_fpe_modifiers.push_back(s);
    }
    fpeModifierMenu->setItemChecked(val, TRUE);
  }
}

void
StatsPanel::genericModifierSelected(int val)
{ 
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::genericModifierSelected val=%d\n", val);
  printf("StatsPanel::genericModifierSelected: (%s)\n", genericModifierMenu->text(val).ascii() );
#endif

  if( genericModifierMenu->text(val).isEmpty() )
  {
    return;
  }


  std::string s = genericModifierMenu->text(val).ascii();
// printf("B1: modifierStr=(%s)\n", s.c_str() );

  bool FOUND = FALSE;
  for( std::list<std::string>::const_iterator it = current_list_of_generic_modifiers.begin();
       it != current_list_of_generic_modifiers.end();  )
  {
    std::string modifier = (std::string)*it;

    if( modifier ==  s )
    {   // It's in the list, so take it out...
// printf("The modifier was in the list ... take it out!\n");
      FOUND = TRUE;
    }

    it++;

    if( FOUND == TRUE )
    {
      current_list_of_generic_modifiers.remove(modifier);
      genericModifierMenu->setItemChecked(val, FALSE);
      break;
    }
  }

  if( FOUND == FALSE )
  {
// printf("The modifier was not in the list ... add it!\n");
    if( s != PTI )
    {
      current_list_of_generic_modifiers.push_back(s);
    }
    genericModifierMenu->setItemChecked(val, TRUE);
  }
}


void
StatsPanel::raisePreferencePanel()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::raisePreferencePanel() \n");
#endif
  getPanelContainer()->getMainWindow()->filePreferences( statsPanelStackPage, QString(pluginInfo->panel_type) );
}

int
StatsPanel::getLineColor(double value)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor(%f) descending_sort= %d TotalTime=%f\n", value, descending_sort, TotalTime);
#endif
  if( (int) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }
  return(10);
}

int
StatsPanel::getLineColor(unsigned int value)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor(%u)\n", value);
#endif


  if( (int) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }
  return(10);

}


int
StatsPanel::getLineColor(uint64_t value)
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::getLineColor(%lld)\n", value);
#endif

  if( (uint64_t) value >  0.0 )
  {
    if( TotalTime*.90 >= value )
    {
      return(0);
    } else if( TotalTime*.80 >= value )
    {
      return(1);
    } else if( TotalTime*.70 >= value )
    {
      return(2);
    } else if( TotalTime*.60 >= value )
    {
      return(3);
    } else if( TotalTime*.50 >= value )
    {
      return(4);
    } else if( TotalTime*.40 >= value )
    {
      return(5);
    } else if( TotalTime*.30 >= value )
    {
      return(6);
    } else if( TotalTime*.20 >= value )
    {
      return(7);
    } else if( TotalTime*.10 >= value )
    {
      return(8);
    } else if( TotalTime*0 >= value )
    {
      return(9);
    } else
    {
      return(10);
    }
  }

  return(10);
}


void
StatsPanel::updateCollectorList()
{
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorList() entered\n");
#endif

  if( experimentGroupList.count() > 0 )
  {
    list_of_collectors.clear();
    QString command = QString("list -v expTypes -x %1").arg(focusedExpID);
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-A: attempt to run (%s)\n", command.ascii() );
#endif
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
               &list_of_collectors, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
  } else
  {
    // Now get the collectors... and their metrics...
    QString command = QString::null;
    if( focusedExpID == -1 )
    {
      command = QString("list -v expTypes -x %1").arg(expID);
    } else
    {
      command = QString("list -v expTypes -x %1").arg(focusedExpID);
    }
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-B: attempt to run (%s)\n", command.ascii() );
#endif
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_collectors.clear();
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::updateCollectorList-ran %s\n", command.ascii() );
    for( std::list<std::string>::const_iterator it = list_of_collectors.begin();
      it != list_of_collectors.end(); it++ )
    {
    std::string collector_name = (std::string)*it;
    printf("StatsPanel::updateCollectorList-DEBUG:A: collector_name = (%s)\n", collector_name.c_str() );
    }
#endif // DEBUG_StatsPanel

  }
}


void
StatsPanel::updateCollectorMetricList()
{
  // Now get the collectors... and their metrics...
  QString command = QString::null;

  if( focusedExpID == -1 ) 
  {
    command = QString("list -v metrics -x %1").arg(expID);
  } else
  {
    command = QString("list -v metrics -x %1").arg(focusedExpID);
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorMetricList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_collectors_metrics.clear();
  list_of_generic_modifiers.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_collectors_metrics, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateCollectorMetricList-ran %s\n", command.ascii() );
#endif

  if( list_of_collectors_metrics.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors_metrics.begin();
         it != list_of_collectors_metrics.end(); it++ )
    {
      std::string collector_name = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateCollectorMetricList-collector_name/metric name=(%s)\n", collector_name.c_str() );
#endif
      if( currentCollectorStr.isEmpty() )
      {
        QString s = QString(collector_name.c_str());
        int index = s.find("::");
        currentCollectorStr = s.mid(0, index );
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::updateCollectorMetricList-Default the current collector to (%s)\n", collector_name.c_str());
#endif
      }
list_of_generic_modifiers.push_back(collector_name);
    }
  }
}


void
StatsPanel::updateThreadsList()
{
// Now get the threads.
  QString command = QString::null;

  currentThreadStrENUM = UNKNOWN;
  if( focusedExpID == -1 )
  {
    command = QString("list -v ranks -x %1").arg(expID);
  } else
  {
    command = QString("list -v ranks -x %1").arg(focusedExpID);
  }
  currentThreadStrENUM = RANK;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList-attempt to run (%s)\n", command.ascii() );
#endif
  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  list_of_pids.clear();
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
         &list_of_pids, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  if( list_of_pids.size() == 0 )
  {
    currentThreadStrENUM = THREAD;
    if( focusedExpID == -1 )
    {
      command = QString("list -v threads -x %1").arg(expID);
    } else
    {
      command = QString("list -v threads -x %1").arg(focusedExpID);
    }
// printf("attempt to run (%s)\n", command.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &list_of_pids, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  } 
  if( list_of_pids.size() == 0 )
  {
    currentThreadStrENUM = PID;
    if( focusedExpID == -1 )
    {
      command = QString("list -v pids -x %1").arg(expID);
    } else
    {
      command = QString("list -v pids -x %1").arg(focusedExpID);
    }
#ifdef DEBUG_StatsPanel
    printf("attempt to run (%s)\n", command.ascii() );
#endif
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &list_of_pids, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::updateThreadsList, ran %s, list_of_pids.size()=%d\n", command.ascii(), list_of_pids.size() );
#endif
  } 

  if( list_of_pids.size() > 1 )
  {
    for( std::list<int64_t>::const_iterator it = list_of_pids.begin();
         it != list_of_pids.end(); it++ )
    {
      int pid = (int64_t)*it;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::updateThreadsList, pid=(%d)\n", pid );
#endif
    }
  }

  if( clip )
  {
    clip->Set_Results_Used();
  }
}

void
StatsPanel::setCurrentCollector()
{
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *fw_experiment = eo->FW();
      CollectorGroup cgrp = fw_experiment->getCollectors();
// printf("Is says you have %d collectors.\n", cgrp.size() );
      if( cgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known collectors for this experiment.\n");
        return;
      }
      for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
      {
        Collector collector = *ci;
        Metadata cm = collector.getMetadata();
        QString name = QString(cm.getUniqueId().c_str());

// printf("Try to match: name.ascii()=%s currentCollectorStr.ascii()=%s\n", name.ascii(), currentCollectorStr.ascii() );
        if( currentCollectorStr.isEmpty() )
        {
          currentCollectorStr = name;
// printf("Assigning currentCollectorStr=%s\n", currentCollectorStr.ascii() );
        }
        if( name == currentCollectorStr )
        {
          if( currentCollector )
          {
// printf("delete the currentCollector\n");
            delete currentCollector;
          }
          currentCollector = new Collector(*ci);
// printf("Set a currentCollector!\n");
        }
      }
    }
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return;
  }
// printf("The currentCollector has been set. currentCollectorStr=(%s)\n", currentCollectorStr.ascii() );
}

void
StatsPanel::setCurrentThread()
{
// printf("setCurrentThread() entered\n");
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      Experiment *fw_experiment = eo->FW();
      // Evaluate the collector's time metric for all functions in the thread
      ThreadGroup tgrp = fw_experiment->getThreads();
// printf("tgrp.size() = (%d)\n", tgrp.size() );
      if( tgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known threads for this experiment.\n");
        return;
      }
      ThreadGroup::iterator ti = tgrp.begin();
      Thread t1 = *ti;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
      {
        Thread t = *ti;
        std::string host = t.getHost();
        pid_t pid = t.getProcessId();
        QString pidstr = QString("%1").arg(pid);
        if( currentThreadStr.isEmpty() )
        {
// printf("Seting a currentThreadStr!\n");
          currentThreadStr = pidstr;
          // set a default thread as well...
          t1 = *ti;
          if( currentThread )
          {
            delete currentThread;
          }
          currentThread = new Thread(*ti);
// printf("A: Set a currentThread\n");
        }
        if( pidstr == currentThreadStr )
        {
// printf("Using %s\n", currentThreadStr.ascii() );
          t1 = *ti;
          if( currentThread )
          {
            delete currentThread;
          }
          currentThread = new Thread(*ti);
// printf("B: Set a currentThread\n");
          break;
        }
      }
    }
  }
  catch(const std::exception& error)
  { 
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return;
  }


}

void
StatsPanel::setCurrentMetricStr()
{
// printf("StatsPanel::setCurrentMetricStr() entered\n");

  if( !currentMetricStr.isEmpty() )
  {
// printf("StatsPanel::setCurrentMetricStr() current metric = %s\n", currentMetricStr.ascii() );
    return;
  }


  // The cli (by default) focuses on the last metric.   We should to 
  // otherwise, when trying to focus on the related source panel, we 
  // don't get the correct statistics showing up.
  for( std::list<std::string>::const_iterator it = list_of_collectors_metrics.begin();
       it != list_of_collectors_metrics.end(); it++ )
  {
       std::string collector_name = (std::string)*it;
       QString s = QString(collector_name.c_str() );
// printf("collector_name=(%s)\n", collector_name.c_str() );

        
//      if( currentMetricStr.isEmpty() ) // See comment regarding which metric
                                         // to focus on by default. (above)
      {
// printf("Can you toggle this (currentCollector) menu?\n");
        int index = s.find("::");
// printf("index=%d\n", index );
        currentMetricStr = s.mid(index+2);
// printf("A: currentCollectorStr=(%s) currentMetricStr=(%s) currentThreadStr=(%s)\n", currentCollectorStr.ascii(), currentMetricStr.ascii(), currentThreadStr.ascii() );
      }
  }
// printf("metric=currentMetricStr=(%s)\n", currentMetricStr.ascii() );
}

void
StatsPanel::outputCLIData(QString xxxfuncName, QString xxxfileName, int xxxlineNumber)
{


#ifdef DEBUG_StatsPanel
   printf("StatsPanel::outputCLIData, xxxfuncName.ascii()=%s, xxxfileName.ascii()=%s\n", xxxfuncName.ascii(), xxxfileName.ascii());
#endif

  int i = 0;

  SPListViewItem *highlight_item = NULL;
  bool highlight_line = FALSE;
  QColor highlight_color = QColor("blue");

  QString strippedString1 = QString::null; // MPI only.

  QString *strings = NULL;

  if( gotHeader == FALSE ) {

#ifdef DEBUG_StatsPanel
    printf("outputCLIData, ATTEMPT TO PUT OUT HEADER gotHeader=%d, staticDataFLAG... = (%d)\n", gotHeader, staticDataFLAG );
    printf("outputCLIData, (sml != NULL)... = (%d)\n", (sml != NULL) );
#endif

    int columnWidth = 200;
    for( FieldList::Iterator it = columnFieldList.begin();
       it != columnFieldList.end();
       ++it)
    {
      QString s = ((QString)*it).stripWhiteSpace();
      columnHeaderList.push_back(s);
      columnWidth = 200;

#ifdef DEBUG_StatsPanel
      printf("outputCLIData, columnHeaderList.push_back(s), outputCLIData, s=(%s), column i=%d\n", s.ascii(), i );
      printf("push_back(s), columnHeaderList.size()=%d\n", columnHeaderList.size() );
#endif

      // IF the string contains the no data samples message then
      // check to see if there is a StatsPanel Message Label available for use
      //      IF so, hide the stats panel list widget and chart form
      //             and view the message label 
      //      
      if( s.find("There were no data") >= 0 ) {
       if (sml != NULL) {
           splv->hide();
           cf->hide();
           sml->show();
           break;
#ifdef DEBUG_StatsPanel
           printf("outputCLIData, show sml hide splv (sml != NULL)... = (%d)\n", (sml != NULL) );
#endif
       }
         
      } else if( s.find("%") != -1 ) {
        if( percentIndex == -1 ) {
          percentIndex = i;
#ifdef DEBUG_StatsPanel
          printf("outputCLIData, Found a column with % in the header, percentIndex at %d\n", percentIndex);
#endif
        }
      } else if( (s.find("Function (defining location)") != -1) ||
                 (s.find("LinkedObject") != -1) ||
                 (s.find("Statement Location (Line Number)") != -1)  ) {
#ifdef DEBUG_StatsPanel
         printf("outputCLIData, Found a column with Function (defining..) in the header, percentIndex at %d\n", percentIndex);
#endif
        QSettings *settings = new QSettings();
        bool fullPathBool = settings->readBoolEntry( "/openspeedshop/general/viewFullPath");
#ifdef DEBUG_StatsPanel
        printf("/openspeedshop/general/viewFullPath == fullPathBool=(%d)\n", fullPathBool );
#endif
         if (fullPathBool) {
           columnWidth = 1200;
         } else {
           columnWidth = 600;
         }
        delete settings;
      }
#ifdef DEBUG_StatsPanel
      printf("outputCLIData, adding column with columnWidth=%d\n", columnWidth);
#endif
      splv->addColumn( s, columnWidth );
      i++;
    }
  
    gotHeader = TRUE;
#ifdef DEBUG_StatsPanel
   printf("outputCLIData, return early from outputCLIData gotHeader=(%d)\n", gotHeader);
#endif
    return;

  } else {

#ifdef DEBUG_StatsPanel
    printf("outputCLIData, gootHeader=%d, staticDataFLAG... = (%d)\n", gotHeader, staticDataFLAG );
#endif

    strings = new QString[columnFieldList.count()];
    int i = 0;
    for( FieldList::Iterator it = columnFieldList.begin();
       it != columnFieldList.end();
       ++it)
    {
      QString s = ((QString)*it).stripWhiteSpace();
      strings[i] = s;
      i++;
    }
    fieldCount = i;
  }

#ifdef DEBUG_StatsPanel
   printf("outputCLIData, PUT OUT DATA, fieldCount=%d\n", fieldCount);
#endif

  if ( fieldCount == 0 )
  {
    QMessageBox::information( (QWidget *)this, tr("Info:"), tr("There were no data samples for this experiment execution.\nPossible reasons for this could be:\n   The executable being run didn't run long enough to record performance data.\n   The type of performance data being gathered may not be present in the executable being executed.\n"), QMessageBox::Ok );
  
#ifdef DEBUG_StatsPanel
   printf("outputCLIData, return early from outputCLIData fieldCount=(%d)\n", fieldCount);
#endif
    return;
  }

  SPListViewItem *splvi;
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::outputCLIData, ATTEMPT TO PUT OUT DATA, More Function currentCollectorStr=%s, MPItraceFLAG=(%d)\n", currentCollectorStr.ascii(), MPItraceFLAG);
#endif

  if( (( currentCollectorStr == "mpi" || 
         currentCollectorStr == "mpit" || 
         currentCollectorStr == "io" || 
         currentCollectorStr == "iot" ) && 
        (MPItraceFLAG == FALSE &&
         !currentUserSelectedReportStr.startsWith("Functions")) && 
         ( currentUserSelectedReportStr.startsWith("CallTrees") ||
           currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || 
           currentUserSelectedReportStr.startsWith("Functions") || 
           currentUserSelectedReportStr.startsWith("TraceBacks") || 
           currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
           currentUserSelectedReportStr.startsWith("Butterfly") ) ) ||
        (currentCollectorStr == "usertime" && 
        ( currentUserSelectedReportStr == "Butterfly" || 
          currentUserSelectedReportStr.startsWith("TraceBacks") || 
          currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
          currentUserSelectedReportStr.startsWith("CallTrees") || 
          currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
        (currentCollectorStr == "fpe" &&
        (currentUserSelectedReportStr == "Butterfly" || 
         currentUserSelectedReportStr.startsWith("TraceBacks") || 
         currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
         currentUserSelectedReportStr.startsWith("CallTrees") || 
         currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) )  ||
        (currentCollectorStr.startsWith("hwc") && 
        (currentUserSelectedReportStr == "Butterfly" || 
         currentUserSelectedReportStr.startsWith("TraceBacks") || 
         currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
         currentUserSelectedReportStr.startsWith("CallTrees") || 
         currentUserSelectedReportStr.startsWith("CallTrees,FullStack") ) ) )
  {
    QString indentChar = ">";

    if( currentUserSelectedReportStr.startsWith("TraceBacks") || currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") )
    {
      indentChar = "<";
    } 
    bool indented = strings[fieldCount-1].startsWith(indentChar);
    int indent_level = 0;

// Pretty the output up a bit.
    if( currentUserSelectedReportStr.startsWith("Butterfly") )
    {
      if( indented )
      {
        // Right side
        strings[fieldCount-1].insert(1,"    ");
// printf("RS: Field (%s)\n", strings[fieldCount-1].ascii() );
      } else if( strings[fieldCount-1].startsWith("<") )
      {
        // Left side
        strings[fieldCount-1].remove("<");
// printf("LS: Field (%s)\n", strings[fieldCount-1].ascii() );
      } else
      {
// printf("Color this one: Field (%s)\n", strings[fieldCount-1].ascii() );
        // Focused Function
        strings[fieldCount-1].insert(0,"  ");
        highlight_line = TRUE;
      }
// printf("here (%s)\n", strings[fieldCount-1].ascii() );
    }


// printf("indented = (%d)\n", indented );
// printf("%d (%s) (%s)\n", indented, strings[0].ascii(), strings[fieldCount-1].ascii() );
  
    if( !indented )
    {
// printf("indent_level=zero lastIndentLevel=%d\n", indent_level, lastIndentLevel );
      // If it's not indented, make sure if is put after the last
      // root node.
      if( lastlvi )
      {
        SPListViewItem *topParent = (SPListViewItem *)lastlvi->parent();
        while( topParent )
        {
          lastlvi = topParent;
          topParent = (SPListViewItem *)topParent->parent();
        }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::outputCLIData, Put after (%s) \n", lastlvi->text(fieldCount-1).ascii() );
  printf("StatsPanel::outputCLIData, xxxfileName.ascii()=%s, xxxfuncName.ascii()=%s\n", xxxfileName.ascii(), xxxfuncName.ascii());
#endif
      }
      lastlvi = splvi =  MYListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings);
      if( highlight_line )
      {
        highlight_item = splvi;
      }
      lastIndentLevel = 0;
    } else
    {
      if( indented && lastlvi != NULL )
      {
        QRegExp rxp = QRegExp( "[_,' ',@,A-Z,a-z,0-9,%]");
        indent_level = strings[fieldCount-1].find(rxp);
        strippedString1 = strings[fieldCount-1].mid(indent_level,9999);
        strings[fieldCount-1] = strippedString1;

        // Pretty up the format a bit.
        if( currentUserSelectedReportStr.startsWith("Butterfly") )
        {
          strings[fieldCount-1].replace(0,0,QString("  ")); // This is the bad boy
        }

        if( indent_level == -1 )
        {
          fprintf(stderr, "Error in determining depth for (%s).\n", strippedString1.ascii() );

        }
// printf("indent_level = %d lastIndentLevel = %d\n", indent_level, lastIndentLevel);
        if( indent_level > lastIndentLevel )
        {
// printf("A: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), lastlvi->text(fieldCount-1).ascii() );
          lastlvi = splvi =  MYListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, lastlvi, lastlvi, strings);
          if( highlight_line )
          {
            highlight_item = splvi;
          }
        } else
        {
// printf("Go figure out the right leaf to put this in...(%s) \n", strings[1].ascii() );

          SPListViewItem *mynextlvi = lastlvi;
          SPListViewItem *after = NULL;
          while( mynextlvi->parent() )
          {
             mynextlvi = (SPListViewItem *)mynextlvi->parent();
          }
          for(int i=0;i<indent_level-1;i++)
          {
            SPListViewItem *lastChild = (SPListViewItem *)mynextlvi->firstChild();
            while( lastChild->nextSibling() )
            {
              lastChild = (SPListViewItem *)lastChild->nextSibling();
            }
            mynextlvi = lastChild;
          }
  
          lastlvi = mynextlvi;
  
          // go to this head, count down the children of the indent level... Then add
          // this item.
          // after = (SPListViewItem *)lastlvi->parent()->firstChild();
          after = (SPListViewItem *)lastlvi->firstChild();
          while( after->nextSibling() )
          {
            after = (SPListViewItem *)after->nextSibling();
          }
// printf("C: adding (%s) to (%s) after (%s)\n", strings[1].ascii(), lastlvi->text(fieldCount-1).ascii(), after->text(fieldCount-1).ascii() );
          lastlvi = splvi = MYListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, lastlvi, after, strings );
          if( highlight_line )
          {
            highlight_item = splvi;
          }
        }
      } else
      {
        fprintf(stderr, "Error in chaining child (%s) to tree.\n", strippedString1.ascii() );
      }
    }

    // Now try to open all the items.\n");
// printf("  indent_level=%d\n", indent_level );
    if( indent_level < levelsToOpen || levelsToOpen == -1 )
    {
      if( lastlvi )
      {
        lastlvi->setOpen(TRUE);
      }
    }
// printf("open lastlvi=(%s)\n", lastlvi->text(fieldCount-1).ascii() );

    lastIndentLevel = indent_level;
  } else
  {
    if( fieldCount == 2 )
    {
      lastlvi = splvi =  new SPListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings[0], strings[1] );
    } else if( fieldCount == 3 )
    { // i.e. like pcsamp
      lastlvi = splvi =  new SPListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings[0], strings[1], strings[2] );
    } else
    { // i.e. like usertime
//      lastlvi = splvi =  new SPListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings[0], strings[1], strings[2], strings[3] );
      lastlvi = splvi =  MYListViewItem( this, xxxfuncName, xxxfileName, xxxlineNumber, splv, lastlvi, strings);
      if( highlight_line )
      {
        highlight_item = splvi;
      }
    }
  }

#ifdef DEBUG_StatsPanel
printf("Are there and cpvl values?\n");
for( ChartPercentValueList::Iterator it = cpvl.begin();
       it != cpvl.end();
       ++it)
{
  int v = (int)*it;
  printf("v=(%d)\n", v);
}
#endif 

#ifdef DEBUG_StatsPanel
   printf("StatsPanel::outputCLIData, CHART, BEFORE if cpvl.count() <= numberItemsToDisplayInChart, total_percent=(%f) cpvl.count()=%d numberItemsToDisplayInStats=%d ctvl.count()=%d numberItemsToDisplayInChart=%d\n", 
          total_percent, cpvl.count(), numberItemsToDisplayInStats, ctvl.count(), numberItemsToDisplayInChart );
#endif



  if( cpvl.count() <= numberItemsToDisplayInChart &&
      ctvl.count() < numberItemsToDisplayInChart )
  {

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, fieldCount=%d\n", fieldCount);
#endif

    int percent = 0;
    for( i = 0; i<fieldCount; i++)
    {
      QString value = strings[i];
      if( i == 0 ) // Grab the (some) default metric FIX
      {
        float f = value.toFloat();
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, add f=%f to TotalTime=%f, i=%d\n", f, TotalTime, i);
#endif

        TotalTime += f;
      }
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d, i=%d\n", percentIndex, i);
#endif
      if( percentIndex == i )
      {
#ifdef DEBUG_StatsPanel
        printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d,EQUALS i=%d, value.isEmpty()=%d\n", 
                 percentIndex, i, value.isEmpty());
#endif
        if( !value.isEmpty() )
        {
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, percentIndex=%d,EQUALS i=%d, value=%s\n", 
                 percentIndex, i, value.ascii());
#endif
          float f = value.toFloat();
          percent = (int)f;
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, ADDING percent=(%d) to total_percent=%d, percentIndex=%d, i=%d\n", percent, total_percent, percentIndex, i);
#endif
          total_percent += f;
        }
      }
    }
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, after for fieldCount CHART, A: total_percent=%f\n", total_percent );
#endif

    if( textENUM == TEXT_BYVALUE )
    { 
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, TEXT_BYVALUE: textENUM=%d (%s)\n", textENUM, strings[0].stripWhiteSpace().ascii()  );
#endif
      cf->setHeader( (QString)*columnHeaderList.begin() );
      ctvl.push_back( strings[0].stripWhiteSpace() );
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, TEXT_BYVALUE: ctvl.push_back -> (%s) \n", strings[0].stripWhiteSpace().ascii() );
#endif
    } else if( textENUM == TEXT_BYPERCENT ) {
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYPERCENT: textENUM=%d\n", textENUM );
#endif

      if( percentIndex > 0 ) {
        ctvl.push_back( strings[percentIndex].stripWhiteSpace() );
        ColumnList::Iterator hit = columnHeaderList.at(percentIndex);
        cf->setHeader( (QString)*hit );
      }
    } else if( textENUM == TEXT_BYLOCATION ) {
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYLOCATION: textENUM=%d\n", textENUM );
      printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, A: TEXT_BYLOCATION: ctvl.push_back -> (%s) \n", strings[columnHeaderList.count()-1].stripWhiteSpace().ascii() );
#endif
      ctvl.push_back( strings[columnHeaderList.count()-1].stripWhiteSpace() );

      if( columnHeaderList.count() > 0 ) {
        ColumnList::Iterator hit = columnHeaderList.at(columnHeaderList.count()-1);
        cf->setHeader( (QString)*hit );
      }
    }
    // We always push back the percent for the value list.
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::outputCLIData, CHART, inside if cpvl.count() <= numberItemsToDisplayInChart, cpvl.push_back percent=%f\n", percent );
#endif
    cpvl.push_back( percent );
  }

  if( highlight_line )
  {
    //    highlight_item->setSelected(TRUE);
    for( int i=0;i<fieldCount;i++)
    {
      highlight_item->setBackground( i, QColor("red") );
    }
  }

  delete []strings;

#ifdef DEBUG_StatsPanel
   printf("Exit StatsPanel::outputCLIData\n");
#endif
}

void
StatsPanel::outputAboutData(QString *incoming_data)
{
  aboutOutputString += *incoming_data;
}


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, QListView *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 1 item (%s)\n", strings[0].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0] );
      break;
    case 2:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 2 item (%s) (%s)\n", strings[0].ascii(), strings[1].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 3 item (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 4 item (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 5 item (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 6 item (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 7 item (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
#ifdef DEBUG_StatsPanel
      printf("Put out SPListViewItem with 8 item, (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s)\n", strings[0].ascii(), strings[1].ascii(), strings[2].ascii(), strings[3].ascii(), strings[4].ascii(), strings[5].ascii(), strings[6].ascii(), strings[7].ascii() );
#endif
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
#ifdef DEBUG_StatsPanel
      printf("Warning: over 9 columns... Notify developer...\n");
#endif
      for( int i=8; i<fieldCount; i++ )
      {
        item->setText(i, strings[i]);
      }
      break;
  }

  return item;
} 


SPListViewItem *
StatsPanel::MYListViewItem( StatsPanel *arg1, QString xxxfuncName, QString xxxfileName, int xxxlineNumber, SPListViewItem *arg2, SPListViewItem *arg3, QString *strings)
{
  SPListViewItem *item = NULL;
  switch( fieldCount )
  {
    case 0:
      break;
    case 1:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0] );
      break;
    case 2:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1] );
      break;
    case 3:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2] );
      break;
    case 4:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3] );
      break;
    case 5:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4] );
      break;
    case 6:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5] );
      break;
    case 7:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6] );
      break;
    case 8:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      break;
    default:
      item = new SPListViewItem( arg1, xxxfuncName, xxxfileName, xxxlineNumber, arg2, arg3, strings[0], strings[1], strings[2], strings[3], strings[4], strings[5], strings[6], strings[7] );
      for( int i=8; i<fieldCount; i++ )
      {
        item->setText(i, strings[i]);
      }
      break;
  }

  return item;
}

QString
StatsPanel::findSelectedFunction()
{
#ifdef DEBUG_StatsPanel
  printf("findSelectedFunction() entered\n");
#endif
  QString functionStr = QString::null;
  QListViewItem *selected_function_item = NULL;
  QListViewItemIterator it( splv, QListViewItemIterator::Selected );
  while( it.current() )
  {
    int i = 0;
    selected_function_item = it.current();
    break;  // only select one for now...
    ++it;
  }

  if( selected_function_item )
  {
    SPListViewItem *spitem = (SPListViewItem *)selected_function_item;
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::findSelectedFunction, spitem->funcName=(%s)\n", spitem->funcName.ascii() ); 
   printf("StatsPanel::findSelectedFunction, spitem->fileName=(%s)\n", spitem->fileName.ascii() ); 
   printf("StatsPanel::findSelectedFunction, spitem->lineNumber=(%d)\n", spitem->lineNumber ); 
#endif

    // Clean up the function name if it needs to be...
    int index = spitem->funcName.find("(");
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::findSelectedFunction, index from spitem->funcName.find()=%d\n", index);
#endif
    if( index != -1 )
    {
//      QString clean_funcName = spitem->funcName.mid(0, index-1);
      QString clean_funcName = spitem->funcName.mid(0, index);
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::findSelectedFunction, Return the cleaned funcName (%s)\n", clean_funcName.ascii() );
#endif
      return( clean_funcName );
    } else
    {
        return( spitem->funcName );
    }
  } else
  {
    return( QString::null );
  }
  return( QString::null );
}

void
StatsPanel::resetRedirect()
{
// Just make sure any pending output goes "somewhere".
  Panel *cmdPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), "&Command Panel");
  if( cmdPanel )
  {
    MessageObject *msg = new MessageObject("Redirect_Window_Output()");
    cmdPanel->listener((void *)msg);
    delete msg;
  } else
  {
    fprintf(stderr, "Unable to redirect output to the cmdpanel.\n");
  }
}

QString
StatsPanel::getFilenameFromString( QString selected_qstring )
{

#ifdef DEBUG_StatsPanel
  printf("Get filename from (%s)\n", selected_qstring.ascii() );
#endif
  QString filename = QString::null;

  int sfi = 0;

  sfi = selected_qstring.find(" in ");
// printf("sfi=%d (Was there an \" in \"\n", sfi );

  if( sfi == -1 )
  {
    sfi = selected_qstring.find(" ");
  } else
  {
    sfi = selected_qstring.find(": ");
    sfi += 1;
  }
  sfi++;

  int efi = selected_qstring.find(",");

// printf("sfi=(%d) efi=(%d) (Was there a \",\" in (%s)\n", sfi, efi, selected_qstring.ascii()  );

  filename = selected_qstring.mid(sfi, efi-sfi );

// printf("   returning filename=(%s)\n", filename.ascii() );

  return(filename);
}

QString
StatsPanel::getFunctionNameFromString( QString selected_qstring, QString &lineNumberStr )
{
#ifdef DEBUG_StatsPanel
  printf("Get funcString from %s\n", selected_qstring.ascii() );
#endif
  QString funcString = QString::null;
  QString workString = selected_qstring;

  int sfi = 0;

  sfi = selected_qstring.find(" in ");
// printf("sfi=%d (Was there an \" in \"\n", sfi );
  if( sfi != -1 )
  {
    workString = selected_qstring.mid(sfi+4);
  } else
  {
    workString = selected_qstring;
  }

// printf("Start you function lookup from (%s)\n", workString.ascii() );

  funcString = workString.section(' ', 0, 0, QString::SectionSkipEmpty);
  std::string selected_function = funcString.ascii();

// printf("funcString=(%s)\n", funcString.ascii() );

  int efi = workString.find("(");
  QString function_name = workString.mid(0,efi);

// printf("function_name=(%s)\n", function_name.ascii() );

  if( ( currentCollectorStr == "mpi" || currentCollectorStr == "mpit" || currentCollectorStr == "io" || currentCollectorStr == "iot" ) && ( collectorStrFromMenu.startsWith("CallTrees") || collectorStrFromMenu.startsWith("Functions") || collectorStrFromMenu.startsWith("TraceBacks") || collectorStrFromMenu.startsWith("TraceBacks,FullStack") ) )
  {
    int bof = -1;
    int eof = workString.find('(');
// printf("eof=%d\n", eof);
    if( eof == -1 )
    {
// printf("main:  you should never be here..\n");
      function_name = "main";
    } else
    {

      QString tempString = workString.mid(0,eof);
// printf("tempString=%s\n", tempString.ascii() );

      QRegExp rxp = QRegExp( "[ >]");
      bof = tempString.findRev(rxp, eof);
// printf("bof=%d\n", bof);
      if( bof == -1 )
      {
        bof = 0;
      } else
      {
        bof++;
      }
    }
    function_name = workString.mid(bof,eof-bof);

    int boln = workString.find('@');
    boln++;
    int eoln = workString.find(" in ");
    lineNumberStr = workString.mid(boln,eoln-boln).stripWhiteSpace();
// printf("lineNumberStr=(%s)\n", lineNumberStr.ascii() );


// printf("mpi: function_name=(%s)\n", function_name.ascii() );
  }

#ifdef DEBUG_StatsPanel
  printf("returning function_name=(%s) lineNumberStr=(%s)\n", function_name.ascii(), lineNumberStr.ascii() );
#endif

  return(function_name);
}

QString
StatsPanel::generateCommand()
{


#ifdef DEBUG_StatsPanel
  if (!currentCollectorStr.isEmpty()) {
     printf("StatsPanel::generateCommand, currentCollectorStr=(%s), MPItraceFLAG=(%d), IOtraceFLAG=%d\n", 
         currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii(), MPItraceFLAG, IOtraceFLAG );
  }
  if (!currentUserSelectedReportStr.isEmpty()) {
     printf("StatsPanel::generateCommand, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii());
  }
  printf("StatsPanel::generateCommand, expID=(%d), focusedExpID=%d\n",  expID, focusedExpID);
#endif

  int exp_id = -1;
  if( focusedExpID == -1 ) {
    exp_id = expID;
  } else {
    exp_id = focusedExpID;
  }

  if( currentCollectorStr == "io" || 
      currentCollectorStr == "iot" ) {
    if( IOtraceFLAG == TRUE ) {
      traceAddition = " -v trace";
    }
  } else if( currentCollectorStr == "mpi" || 
             currentCollectorStr == "mpit" ) {
    if( MPItraceFLAG == TRUE ) {
      traceAddition = " -v trace";
    }
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::generateCommand, traceAddition=(%s)\n", traceAddition.ascii() );
#endif

  QString modifierStr = QString::null;

  updateCollectorList();

  updateCollectorMetricList();

  updateThreadsList();

  lastAbout = aboutString;

  nprintf( DEBUG_PANELS) ("Find_Experiment_Object() for %d\n", exp_id);

  QString command = QString("expView -x %1").arg(exp_id);
  aboutString = QString("Experiment: %1\n").arg(exp_id);

  if( currentCollectorStr.isEmpty() ) {
    if( numberItemsToDisplayInStats > 0 ) {

      command += QString(" %1%2").arg("stats").arg(numberItemsToDisplayInStats);
      aboutString += QString("Requested data for all collectors for top %1 items\n").arg(numberItemsToDisplayInStats);
      infoAboutString += QString("Experiment type: 'all' for top %1 items\n").arg(numberItemsToDisplayInStats);

    } else {

      command += QString(" %1").arg("stats");
      aboutString += QString("Requested data for all collectors\n");
      infoAboutString += QString("Data displayed is from all collectors\n");

    }
  } else {

   // Current collector string is not empty

    if( numberItemsToDisplayInStats > 0 ) {

      command += QString(" %1%2").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
      aboutString += QString("Requested data for collector %1 for top %2 items\n").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::generateCommand() before setting, infoAboutString.ascii()=%s\n", infoAboutString.ascii());
#endif

      infoAboutString += QString("Experiment type: %1 for top %2 items\n").arg(currentCollectorStr).arg(numberItemsToDisplayInStats);

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::generateCommand() after setting, infoAboutString.ascii()=%s\n", infoAboutString.ascii());
#endif
    } else {

      command += QString(" %1").arg(currentCollectorStr);
      aboutString += QString("Requested data for collector %1\n");
      infoAboutString += QString("Data displayed is from collector %1\n");

    }

  }

  if( !currentUserSelectedReportStr.isEmpty() && !currentCollectorStr.isEmpty() ) {
    if( currentCollectorStr != currentUserSelectedReportStr ) {  
       // If these 2 are equal, we want the default display... not a 
       // specific metric.
#ifdef DEBUG_StatsPanel
       printf("generateCommand, (If these 2 are equal case block): command=(%s) currentCollectorStr=(%s) currentUserSelectedReportStr(%s) currentMetricStr=(%s)\n", 
        command.ascii(), currentCollectorStr.ascii(), 
        currentUserSelectedReportStr.ascii(), currentMetricStr.ascii() );
#endif
       if (!currentMetricStr.isEmpty()) {
         command += QString(" -m %1").arg(currentMetricStr);
         aboutString += QString("for metrics %1\n").arg(currentMetricStr);
        }
       infoAboutString += QString("Display options: %1\n").arg(currentUserSelectedReportStr);
    }
  }


#ifdef DEBUG_StatsPanel
 printf("generateCommand, so far: command=(%s) currentCollectorStr=(%s) currentUserSelectedReportStr(%s) currentMetricStr=(%s)\n", 
        command.ascii(), currentCollectorStr.ascii(), 
        currentUserSelectedReportStr.ascii(), currentMetricStr.ascii() );
#endif


  if( currentCollectorStr == "pcsamp" && 
      (currentUserSelectedReportStr == "Functions") || 
      (currentUserSelectedReportStr == "LinkedObjects") || 
      (currentUserSelectedReportStr == "Statements by Function") ||
      (currentUserSelectedReportStr == "Statements") ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, inside pcsamp specific processing, currentUserSelectedReportStr=(%s)\n",
              currentUserSelectedReportStr.ascii());
#endif

    if( currentUserSelectedReportStr.isEmpty() ) { 
      currentUserSelectedReportStr = "Functions";
    }

    if( currentUserSelectedReportStr == "Statements by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, pcsamp, Statements by Function\n");
#endif

      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, pcsamp, StatementsByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                 QString("Which function?:"), 
                                 QLineEdit::Normal, QString::null, &ok, this);

      }

      if( selectedFunctionStr.isEmpty() ) {
        return( QString::null );
      }

      command = QString("expView -x %1 %4%2 -v statements -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
#ifdef DEBUG_StatsPanel
     printf("generateCommand, StatementsByFunction, command=(%s)\n", command.ascii() );
#endif

  } else {

    if( numberItemsToDisplayInStats > 0 ) {
      command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats).arg(currentUserSelectedReportStr);
    } else {
      command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
    }
  }

#ifdef DEBUG_StatsPanel
  printf("start of pcsamp generated command (%s)\n", command.ascii() );
#endif

  } else if( currentCollectorStr == "usertime" || 
             currentCollectorStr == "fpe" || 
             currentCollectorStr == "io" || 
             currentCollectorStr == "iot" || 
             currentCollectorStr == "hwctime" || 
             currentCollectorStr == "mpi" || 
             currentCollectorStr == "mpit" ) {

#ifdef DEBUG_StatsPanel
    printf("generateCommand, usertime to mpit, currentCollectorStr=(%s) currentUserSelectedReportStr=(%s)\n", 
           currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

    selectedFunctionStr = findSelectedFunction();

    if( currentUserSelectedReportStr.isEmpty() ) { 
      currentUserSelectedReportStr = "Functions";
    }

    if( currentUserSelectedReportStr == "Butterfly" ) {
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
  printf("generateCommand, Butterfly, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                    QString("Which function?:"), 
                                    QLineEdit::Normal, QString::null, &ok, this);
      }

      if( selectedFunctionStr.isEmpty() ) {
        return( QString::null );
      }

#ifdef DEBUG_StatsPanel
      printf("generateCommand, selectedFunctionStr=(%s)\n", selectedFunctionStr.ascii() );
#endif

      command = QString("expView -x %1 %4%2 -v Butterfly -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "Statements by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, Statements by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, StatementsByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                 QString("Which function?:"), 
                                 QLineEdit::Normal, QString::null, &ok, this);

      }

      if( selectedFunctionStr.isEmpty() ) {
        return( QString::null );
      }

      command = QString("expView -x %1 %4%2 -v statements -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);
#ifdef DEBUG_StatsPanel
     printf("generateCommand, StatementsByFunction, command=(%s)\n", command.ascii() );
#endif


    } else if( currentUserSelectedReportStr == "CallTrees by Function" ) {

#ifdef DEBUG_StatsPanel
       printf("generateCommand, CallTrees by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, CallTreesByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      command = QString("expView -x %1 %4%2 -v CallTrees -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "CallTrees,FullStack by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("CallTrees,FullStack by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, CallTreesFSByFunction,A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      command = QString("expView -x %1 %4%2 -v CallTrees,FullStack -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "TraceBacks by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("generateCommand, TraceBacks by Function\n");
#endif
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("A: NO FUNCTION SELECTED Prompt for one!\n");
#endif

        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      command = QString("expView -x %1 %4%2 -v Tracebacks -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);

    } else if( currentUserSelectedReportStr == "TraceBacks,FullStack by Function" ) {

#ifdef DEBUG_StatsPanel
      printf("TraceBacks,FullStack by Function\n");
#endif

      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;

#ifdef DEBUG_StatsPanel
        printf("generateCommand, TracebacksFSByFunction, A: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      command = QString("expView -x %1 %4%2 -v Tracebacks,FullStack -f \"%3\"").arg(exp_id).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr).arg(currentCollectorStr);

    } else {

#ifdef DEBUG_StatsPanel
      printf("generateCommand, Here's the usertime menu work stuff.\n");
#endif

      if( numberItemsToDisplayInStats > 0 ) {
        command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats).arg(currentUserSelectedReportStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, A: USERTIME! command=(%s)\n", command.ascii() );
#endif
      } else {
        command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);

#ifdef DEBUG_StatsPanel
        printf("generateCommand, B: USERTIME! command=(%s)\n", command.ascii() );
#endif
      }
    }

#ifdef DEBUG_StatsPanel
   printf("USERTIME! command=(%s)\n", command.ascii() );
#endif

  } else if( ( ( currentCollectorStr == "hwc" || 
                 currentCollectorStr == "hwctime" || 
                 currentCollectorStr == "mpi" || 
                 currentCollectorStr == "mpit" ) && 
               ( currentUserSelectedReportStr.startsWith("CallTrees") || 
                 currentUserSelectedReportStr.startsWith("CallTrees,FullStack") || 
                 currentUserSelectedReportStr.startsWith("Functions") || 
                 currentUserSelectedReportStr.startsWith("mpi") || 
                 currentUserSelectedReportStr.startsWith("io") || 
                 currentUserSelectedReportStr.startsWith("TraceBacks") || 
                 currentUserSelectedReportStr.startsWith("TraceBacks,FullStack") || 
                 currentUserSelectedReportStr.startsWith("Butterfly") ) )) { 

#ifdef DEBUG_StatsPanel
    printf("generateCommand, It thinks we're mpi | io!\n");
#endif
    if( currentUserSelectedReportStr.isEmpty() || currentUserSelectedReportStr == "CallTrees" ) {
      if( numberItemsToDisplayInStats > 0 ) {

        command = QString("expView -x %1 %2%3 -v CallTrees").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats);

      } else {

        command = QString("expView -x %1 %2 -v CallTrees").arg(exp_id).arg(currentCollectorStr);

      }
    } else if ( currentUserSelectedReportStr == "CallTrees by Selected Function" ) {

      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      if( selectedFunctionStr.isEmpty() ) {
        return( QString::null );
      }

      if( numberItemsToDisplayInStats > 0 ) {
        command = QString("expView -x %1 %2%3 -v CallTrees -f %4").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr);
      } else {
        command = QString("expView -x %1 %2 -v CallTrees -f %4").arg(exp_id).arg(currentCollectorStr).arg(selectedFunctionStr);
      }
    } else if ( currentUserSelectedReportStr == "TraceBacks" ) {

      if( numberItemsToDisplayInStats > 0 ) {
        command = QString("expView -x %1 %2%3 -v TraceBacks").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
      } else {
        command = QString("expView -x %1 %2%3 -v TraceBacks").arg(exp_id).arg(currentCollectorStr);
      }

    } else if ( currentUserSelectedReportStr == "TraceBacks,FullStack" ) {

      if( numberItemsToDisplayInStats > 0 ) {
        command = QString("expView -x %1 %2%3 -v TraceBacks,FullStack").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
      } else {
        command = QString("expView -x %1 %2 -v TraceBacks,FullStack").arg(exp_id).arg(currentCollectorStr);
      }

    } else if( currentUserSelectedReportStr == "Butterfly" ) {

      selectedFunctionStr = findSelectedFunction();
      if( selectedFunctionStr.isEmpty() ) {
        bool ok = FALSE;
#ifdef DEBUG_StatsPanel
        printf("generateCommand, Butterfly B: NO FUNCTION SELECTED Prompt for one!\n");
#endif
        selectedFunctionStr = QInputDialog::getText("Enter Function Name Dialog:", 
                                  QString("Which function?:"), 
                                  QLineEdit::Normal, QString::null, &ok, this);
      }

      if( selectedFunctionStr.isEmpty() ) {
        return( QString::null );
      }

      if( numberItemsToDisplayInStats > 0 ) {

        command = QString("expView -x %1 %2%3 -v Butterfly -f \"%4\"").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats).arg(selectedFunctionStr);

      } else {

        command = QString("expView -x %1 %2 -v Butterfly -f \"%4\"").arg(exp_id).arg(currentCollectorStr).arg(selectedFunctionStr);

      }

    } else {

      if( numberItemsToDisplayInStats > 0 ) {
        command = QString("expView -x %1 %2%3 -v Functions").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
      } else {
        command = QString("expView -x %1 %2 -v Functions").arg(exp_id).arg(currentCollectorStr);
      }
    }

  } else if( (currentCollectorStr == "hwc" || 
              currentCollectorStr == "hwctime") &&
            (currentUserSelectedReportStr == "Butterfly") ||
            (currentUserSelectedReportStr == "Functions") ||
            (currentUserSelectedReportStr == "LinkedObjects") ||
            (currentUserSelectedReportStr == "Statements") ||
            (currentUserSelectedReportStr == "CallTrees") ||
            (currentUserSelectedReportStr == "CallTrees,FullStack") ||
            (currentUserSelectedReportStr == "TraceBacks") ||
            (currentUserSelectedReportStr == "TraceBacks,FullStack") ) {

    if( currentUserSelectedReportStr.isEmpty() ) { 
      currentUserSelectedReportStr = "Functions";
    }

   if( currentUserSelectedReportStr.startsWith("Statements") ) { 

    if( numberItemsToDisplayInStats > 0 ) {
      command = QString("expView -x %1 %2%3 -v statements").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats);
    } else {
      command = QString("expView -x %1 %2 -v statements").arg(exp_id).arg(currentCollectorStr);
    }

   } else {

    // report string does not start with Statements
    if( numberItemsToDisplayInStats > 0 ) {
      command = QString("expView -x %1 %2%3 -v %4").arg(exp_id).arg(currentCollectorStr).arg(numberItemsToDisplayInStats).arg(currentUserSelectedReportStr);
    } else {
      command = QString("expView -x %1 %2 -v %4").arg(exp_id).arg(currentCollectorStr).arg(currentUserSelectedReportStr);
    }

  }

#ifdef DEBUG_StatsPanel
  printf("generateCommand, hwc command=(%s)\n", command.ascii() );
#endif
  aboutString = command + "\n";
 } 

  // Add any focus.
 if( !currentThreadsStr.isEmpty() ) {

#ifdef DEBUG_StatsPanel
    printf("generateCommand, add any focus, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
#endif

    command += QString(" %1").arg(currentThreadsStr);
    aboutString += QString("for threads %1\n").arg(currentThreadsStr);
    infoAboutString += QString("Hosts/Threads %1\n").arg(currentThreadsStr);

  }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, command sofar... =(%s)\n", command.ascii() );
   printf("generateCommand, add any modifiers...\n");
#endif

    std::list<std::string> *modifier_list = NULL;;

#ifdef DEBUG_StatsPanel
   printf("generateCommand: currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
   printf("generateCommand: currentCollectorStr = (%s)\n", currentCollectorStr.ascii() );
#endif
    if( currentCollectorStr == "hwc" ) {
      modifier_list = &current_list_of_hwc_modifiers;
    } else if( currentCollectorStr == "hwctime" ) {
      modifier_list = &current_list_of_hwctime_modifiers;
    } else if( currentCollectorStr == "io" ) {
      modifier_list = &current_list_of_io_modifiers;
    } else if( currentCollectorStr == "iot" ) {

#ifdef DEBUG_StatsPanel
      printf("generateCommand, &current_list_of_iot_modifiers=(%x)\n", &current_list_of_iot_modifiers);
#endif
       for( std::list<std::string>::const_iterator iot_it = current_list_of_iot_modifiers.begin();
          iot_it != current_list_of_iot_modifiers.end(); iot_it++ )
       {

         std::string iot_modifier = (std::string)*iot_it;
#ifdef DEBUG_StatsPanel
         printf("generateCommand, iot_modifier = (%s)\n", iot_modifier.c_str() );
#endif
       }
      modifier_list = &current_list_of_iot_modifiers;
    } else if( currentCollectorStr == "mpi" ) {
      modifier_list = &current_list_of_mpi_modifiers;
    } else if( currentCollectorStr == "mpit" ) {
      modifier_list = &current_list_of_mpit_modifiers;
    } else if( currentCollectorStr == "pcsamp" ) {
      modifier_list = &current_list_of_pcsamp_modifiers;
    } else if( currentCollectorStr == "usertime" ) {
      modifier_list = &current_list_of_usertime_modifiers;
    } else if( currentCollectorStr == "fpe" ) {
      modifier_list = &current_list_of_fpe_modifiers;
    } else {
//      modifier_list = &current_list_of_modifiers;
      modifier_list = &current_list_of_generic_modifiers;
    }

    for( std::list<std::string>::const_iterator it = modifier_list->begin();
       it != modifier_list->end(); it++ )
    {

      std::string modifier = (std::string)*it;
#ifdef DEBUG_StatsPanel
      printf("generateCommand, modifier = (%s)\n", modifier.c_str() );
#endif
      if( modifierStr.isEmpty() ) {

#ifdef DEBUG_StatsPanel
        printf("generateCommand, modifierStr is empty, A: modifer = (%s)\n", modifier.c_str() );
#endif

        modifierStr = QString(" -m %1").arg(modifier.c_str());
        currentMetricStr = modifier.c_str();
      } else {

#ifdef DEBUG_StatsPanel
        printf("generateCommand, before update modifierStr.ascii()=(%s) B: modifer = (%s)\n", 
               modifierStr.ascii(), modifier.c_str() );
#endif
        modifierStr += QString(",%1").arg(modifier.c_str());

#ifdef DEBUG_StatsPanel
        printf("generateCommand, after update modifierStr.ascii()=(%s) B: modifer = (%s)\n", 
               modifierStr.ascii(), modifier.c_str() );
#endif

      }
    }


#ifdef DEBUG_StatsPanel
     printf("generateCommand, before updating command=(%s) with modifierStr.ascii()=(%s)\n", 
             command.ascii(), modifierStr.ascii() );
#endif

   if( !modifierStr.isEmpty() ) {
     command += QString(" %1").arg(modifierStr);
   }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, after updating command=(%s) with modifierStr.ascii()=(%s)\n", 
           command.ascii(), modifierStr.ascii() );
   printf("generateCommand, before updating command=(%s) with traceAddition.ascii()=(%s)\n", 
           command.ascii(), traceAddition.ascii() );
#endif

   if( !traceAddition.isEmpty() ) {
      command += traceAddition;
   }

#ifdef DEBUG_StatsPanel
   printf("generateCommand, before updating command=(%s) with traceAddition.ascii()=(%s)\n", 
          command.ascii(), traceAddition.ascii() );
   printf("generateCommand() EXIT returning command=(%s), currentCollectorStr=(%s)\n", 
          command.ascii(), currentCollectorStr.ascii() );
#endif

  return( command );
} // End generateCommand

void
StatsPanel::generateMPIMenu(QString collectorName)
{
#ifdef DEBUG_StatsPanel
  printf("generateMPIMenu(%s)\n", collectorName.ascii() );
#endif
  mpi_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  mpi_menu->setCheckable(TRUE);

  mpi_menu->insertSeparator();

  list_of_mpi_modifiers.clear();
  list_of_mpit_modifiers.clear();
  if( collectorName == "mpi" )
  {
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPIReportSelected(int)) );
    if( focusedExpID != -1 )
    {
      contextMenu->insertItem(QString("Show Metrics (Exp: %1) : MPI").arg(focusedExpID), mpi_menu);
    } else
    {
      contextMenu->insertItem(QString("Display Options: MPI"), mpi_menu);
    }
    list_of_mpi_modifiers.push_back("mpi::exclusive_times");
    list_of_mpi_modifiers.push_back("mpi::inclusive_times");
//  list_of_mpi_modifiers.push_back("mpi::exclusive_details");
//  list_of_mpi_modifiers.push_back("mpi::inclusive_details");
    list_of_mpi_modifiers.push_back("min");
    list_of_mpi_modifiers.push_back("max");
    list_of_mpi_modifiers.push_back("average");
    list_of_mpi_modifiers.push_back("count");
    list_of_mpi_modifiers.push_back("percent");
    list_of_mpi_modifiers.push_back("stddev");

//  list_of_mpi_modifiers.push_back("start_time");
//  list_of_mpi_modifiers.push_back("stop_time");
//  list_of_mpi_modifiers.push_back("source");
//  list_of_mpi_modifiers.push_back("dest");
//  list_of_mpi_modifiers.push_back("size");
//  list_of_mpi_modifiers.push_back("tag");
//  list_of_mpi_modifiers.push_back("commuinicator");
//  list_of_mpi_modifiers.push_back("datatype");
//  list_of_mpi_modifiers.push_back("retval");

    if( mpiModifierMenu )
    {
      delete mpiModifierMenu;
    }
    mpiModifierMenu = new QPopupMenu(this);
    mpiModifierMenu->insertTearOffHandle();
    connect(mpiModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(mpiModifierSelected(int)) );

    generateModifierMenu(mpiModifierMenu, list_of_mpi_modifiers, current_list_of_mpi_modifiers);
    mpi_menu->insertItem(QString("Select mpi details:"), mpiModifierMenu);

    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( mpiModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(MPItraceFLAG);
    qaction->setOn(MPItraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(MPItraceSelected()) );
  } else if( collectorName == "mpit" ) {
    addMPIReports(mpi_menu);
    connect(mpi_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorMPITReportSelected(int)) );
    if( focusedExpID != -1 )
    {
      contextMenu->insertItem(QString("Show Metrics (Exp: %1) : MPIT").arg(focusedExpID), mpi_menu);
    } else
    {
      contextMenu->insertItem(QString("Display Options: MPIT"), mpi_menu);
    }
    list_of_mpit_modifiers.push_back("mpit::exclusive_times");
    list_of_mpit_modifiers.push_back("mpit::inclusive_times");
//  list_of_mpit_modifiers.push_back("mpit::exclusive_details");
//  list_of_mpit_modifiers.push_back("mpit::inclusive_details");
    list_of_mpit_modifiers.push_back("min");
    list_of_mpit_modifiers.push_back("max");
    list_of_mpit_modifiers.push_back("average");
    list_of_mpit_modifiers.push_back("count");
    list_of_mpit_modifiers.push_back("percent");
    list_of_mpit_modifiers.push_back("stddev");

    list_of_mpit_modifiers.push_back("start_time");
    list_of_mpit_modifiers.push_back("stop_time");
    list_of_mpit_modifiers.push_back("source");
    list_of_mpit_modifiers.push_back("dest");
    list_of_mpit_modifiers.push_back("size");
    list_of_mpit_modifiers.push_back("tag");
    list_of_mpit_modifiers.push_back("commuinicator");
    list_of_mpit_modifiers.push_back("datatype");
    list_of_mpit_modifiers.push_back("retval");

    if( mpitModifierMenu )
    {
      delete mpitModifierMenu;
    }
    mpitModifierMenu = new QPopupMenu(this);
    mpitModifierMenu->insertTearOffHandle();
    connect(mpitModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(mpitModifierSelected(int)) );

    generateModifierMenu(mpitModifierMenu, list_of_mpit_modifiers, current_list_of_mpit_modifiers);
    mpi_menu->insertItem(QString("Select mpit details:"), mpitModifierMenu);
  
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( mpitModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(MPItraceFLAG);
    qaction->setOn(MPItraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(MPItraceSelected()) );
  } else {
#ifdef DEBUG_StatsPanel
       printf("generateMPIMenu(%s) for mpiotf\n", collectorName.ascii() );
#endif
    // generate mpiotf menu
  }

}

void
StatsPanel::generateIOMenu(QString collectorName)
{
#ifdef DEBUG_StatsPanel
  printf("generateIOMenu(%s)\n", collectorName.ascii() );
#endif
  io_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;

  io_menu->insertSeparator();

  if( collectorName == "io" )
  {
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOReportSelected(int)) );
    if( focusedExpID != -1 )
    {
      contextMenu->insertItem(QString("Display Options: (Exp: %1) IO").arg(focusedExpID), io_menu);
    } else
    {
      contextMenu->insertItem(QString("Display Options: IO"), io_menu);
    }
    // Build the static list of io modifiers.
    list_of_io_modifiers.clear();
    list_of_io_modifiers.push_back("io::exclusive_times");
//    list_of_io_modifiers.push_back("io::inclusive_times");
//    list_of_io_modifiers.push_back("io::exclusive_details");
//    list_of_io_modifiers.push_back("io::inclusive_details");
    list_of_io_modifiers.push_back("min");
    list_of_io_modifiers.push_back("max");
    list_of_io_modifiers.push_back("average");
    list_of_io_modifiers.push_back("count");
    list_of_io_modifiers.push_back("percent");
    list_of_io_modifiers.push_back("stddev");
    list_of_io_modifiers.push_back("ThreadAverage");
    list_of_io_modifiers.push_back("ThreadMin");
    list_of_io_modifiers.push_back("ThreadMax");

//    list_of_io_modifiers.push_back("start_time");
//    list_of_io_modifiers.push_back("stop_time");
//    list_of_io_modifiers.push_back("source");
//    list_of_io_modifiers.push_back("dest");
//    list_of_io_modifiers.push_back("size");
//    list_of_io_modifiers.push_back("tag");
//    list_of_io_modifiers.push_back("commuinicator");
//    list_of_io_modifiers.push_back("datatype");
//    list_of_io_modifiers.push_back("retval");
  
    if( ioModifierMenu )
    {
      delete ioModifierMenu;
    }
    ioModifierMenu = new QPopupMenu(this);
    ioModifierMenu->insertTearOffHandle();
    connect(ioModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(ioModifierSelected(int)) );
    generateModifierMenu(ioModifierMenu, list_of_io_modifiers, current_list_of_io_modifiers);
    io_menu->insertItem(QString("Select io details:"), ioModifierMenu);

    io_menu->setCheckable(TRUE);
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( ioModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(IOtraceFLAG);
    qaction->setOn(IOtraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(IOtraceSelected()) );
  } else 
  {
#ifdef DEBUG_StatsPanel
  printf("generateIOTMenu(%s)\n", collectorName.ascii() );
#endif
    addIOReports(io_menu);
    connect(io_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorIOTReportSelected(int)) );
    if( focusedExpID != -1 )
    {
      contextMenu->insertItem(QString("Display Options: (Exp: %1) IOT").arg(focusedExpID), io_menu);
    } else
    {
      contextMenu->insertItem(QString("Display Options: IOT"), io_menu);
    }
    // Build the static list of iot modifiers.
    list_of_iot_modifiers.clear();
    list_of_iot_modifiers.push_back("iot::exclusive_times");
    list_of_iot_modifiers.push_back("iot::inclusive_times");
//    list_of_iot_modifiers.push_back("iot::exclusive_details");
//    list_of_iot_modifiers.push_back("iot::inclusive_details");
    list_of_iot_modifiers.push_back("min");
    list_of_iot_modifiers.push_back("max");
    list_of_iot_modifiers.push_back("average");
    list_of_iot_modifiers.push_back("count");
    list_of_iot_modifiers.push_back("percent");
    list_of_iot_modifiers.push_back("stddev");

    list_of_iot_modifiers.push_back("start_time");
    list_of_iot_modifiers.push_back("stop_time");
#if 0
    list_of_iot_modifiers.push_back("source");
    list_of_iot_modifiers.push_back("dest");
    list_of_iot_modifiers.push_back("size");
    list_of_iot_modifiers.push_back("tag");
    list_of_iot_modifiers.push_back("commuinicator");
    list_of_iot_modifiers.push_back("datatype");
    list_of_iot_modifiers.push_back("retval");
#else
    list_of_iot_modifiers.push_back("syscallno");
    list_of_iot_modifiers.push_back("nsysargs");
    list_of_iot_modifiers.push_back("retval");
#endif

    if( iotModifierMenu )
    {
      delete iotModifierMenu;
    }
    iotModifierMenu = new QPopupMenu(this);
    iotModifierMenu->insertTearOffHandle();
    connect(iotModifierMenu, SIGNAL( activated(int) ),
      this, SLOT(iotModifierSelected(int)) );
    generateModifierMenu(iotModifierMenu, list_of_iot_modifiers, current_list_of_iot_modifiers);
    io_menu->insertItem(QString("Select iot details:"), iotModifierMenu);

    io_menu->setCheckable(TRUE);
    qaction = new QAction(this, "showTraceInfo");
    qaction->addTo( iotModifierMenu );
    qaction->setText( tr(PTI) );
    qaction->setToggleAction(IOtraceFLAG);
    qaction->setOn(IOtraceFLAG);
    qaction->setToolTip(tr("When available, show traced timings."));
    connect( qaction, SIGNAL( activated() ), this, SLOT(IOtraceSelected()) );
  }

// printf("We made an io_menu!!\n");


}


void
StatsPanel::generateHWCMenu(QString collectorName)
{
// printf("Collector hwc_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwc_menu = new QPopupMenu(this);

  if( focusedExpID != -1 )
  {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) hwc").arg(focusedExpID), hwc_menu);
  } else
  {
    contextMenu->insertItem(QString("Display Options: hwc"), hwc_menu);
  }

  addHWCReports(hwc_menu);
  connect(hwc_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCReportSelected(int)) );

  list_of_hwc_modifiers.clear();
  list_of_hwc_modifiers.push_back("hwc::overflows");
  list_of_hwc_modifiers.push_back("hwc::counts");
  list_of_hwc_modifiers.push_back("hwc::percent");
  list_of_hwc_modifiers.push_back("hwc::ThreadAverage");
  list_of_hwc_modifiers.push_back("hwc::ThreadMin");
  list_of_hwc_modifiers.push_back("hwc::ThreadMax");
  
  if( hwcModifierMenu )
  {
    delete hwcModifierMenu;
  }
  hwcModifierMenu = new QPopupMenu(this);
  hwcModifierMenu->insertTearOffHandle();
  connect(hwcModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(hwcModifierSelected(int)) );
  generateModifierMenu(hwcModifierMenu, list_of_hwc_modifiers, current_list_of_hwc_modifiers);
  hwc_menu->insertItem(QString("Select hwc details:"), hwcModifierMenu);
}


void
StatsPanel::generateHWCTimeMenu(QString collectorName)
{
// printf("Collector hwctime_menu is being created collectorName=(%s)\n", collectorName.ascii() );

  QString s = QString::null;

  QAction *qaction = NULL;


  hwctime_menu = new QPopupMenu(this);

  if( focusedExpID != -1 )
  {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) hwctime").arg(focusedExpID), hwctime_menu);
  } else
  {
    contextMenu->insertItem(QString("Display Options: hwctime"), hwctime_menu);
  }

  addHWCTimeReports(hwctime_menu);
  connect(hwctime_menu, SIGNAL( activated(int) ),
         this, SLOT(collectorHWCTimeReportSelected(int)) );
  list_of_hwctime_modifiers.clear();
  list_of_hwctime_modifiers.push_back("hwctime::exclusive_counts");
  list_of_hwctime_modifiers.push_back("hwctime::exclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::inclusive_overflows");
  list_of_hwctime_modifiers.push_back("hwctime::inclusive_counts");
  list_of_hwctime_modifiers.push_back("hwctime::percent");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadAverage");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadMin");
  list_of_hwctime_modifiers.push_back("hwctime::ThreadMax");

  if( hwctimeModifierMenu )
  {
    delete hwctimeModifierMenu;
  }
  hwctimeModifierMenu = new QPopupMenu(this);
  hwctimeModifierMenu->insertTearOffHandle();
  connect(hwctimeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(hwctimeModifierSelected(int)) );
  generateModifierMenu(hwctimeModifierMenu, list_of_hwctime_modifiers, current_list_of_hwctime_modifiers);
  hwctime_menu->insertItem(QString("Select hwctime details:"), hwctimeModifierMenu);
}


void
StatsPanel::generateUserTimeMenu()
{
// printf("Collector usertime_menu is being created\n");

  usertime_menu = new QPopupMenu(this);
  connect(usertime_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorUserTimeReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 )
  {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) UserTime").arg(focusedExpID), usertime_menu);
  } else
  {
    contextMenu->insertItem(QString("Display Options: UserTime"), usertime_menu);
  }

  list_of_usertime_modifiers.clear();
  list_of_usertime_modifiers.push_back("usertime::exclusive_times");
  list_of_usertime_modifiers.push_back("usertime::inclusive_times");
  list_of_usertime_modifiers.push_back("usertime::percent");
  list_of_usertime_modifiers.push_back("usertime::count");
//  list_of_usertime_modifiers.push_back("usertime::exclusive_details");
//  list_of_usertime_modifiers.push_back("usertime::inclusive_details");
  list_of_usertime_modifiers.push_back("usertime::ThreadAverage");
  list_of_usertime_modifiers.push_back("usertime::ThreadMin");
  list_of_usertime_modifiers.push_back("usertime::ThreadMax");

  if( usertimeModifierMenu )
  {
    delete usertimeModifierMenu;
  }
  usertimeModifierMenu = new QPopupMenu(this);
  addUserTimeReports(usertime_menu);
  usertimeModifierMenu->insertTearOffHandle();
  connect(usertimeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(usertimeModifierSelected(int)) );
  generateModifierMenu(usertimeModifierMenu, list_of_usertime_modifiers, current_list_of_usertime_modifiers);
  usertime_menu->insertItem(QString("Select usertime Metrics:"), usertimeModifierMenu);
}

void
StatsPanel::generatePCSampMenu()
{
// printf("Collector pcsamp_menu is being created\n");

  pcsamp_menu = new QPopupMenu(this);

  QString s = QString::null;

  QAction *qaction = NULL;


  addPCSampReports(pcsamp_menu);
  connect(pcsamp_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorPCSampReportSelected(int)) );
  
  if( focusedExpID != -1 )
  {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) pcsamp").arg(focusedExpID), pcsamp_menu);
  } else
  {
    contextMenu->insertItem(QString("Display Options: pcsamp"), pcsamp_menu);
  }

  list_of_pcsamp_modifiers.clear();
  list_of_pcsamp_modifiers.push_back("pcsamp::time");
  list_of_pcsamp_modifiers.push_back("pcsamp::percent");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadAverage");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadMin");
  list_of_pcsamp_modifiers.push_back("pcsamp::ThreadMax");

  if( pcsampModifierMenu )
  {
    delete pcsampModifierMenu;
  }

  pcsampModifierMenu = new QPopupMenu(this);
  pcsampModifierMenu->insertTearOffHandle();
  connect(pcsampModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(pcsampModifierSelected(int)) );
  generateModifierMenu(pcsampModifierMenu, list_of_pcsamp_modifiers, current_list_of_pcsamp_modifiers);
  pcsamp_menu->insertItem(QString("Select pcsamp Metrics:"), pcsampModifierMenu);
}


void
StatsPanel::generateFPEMenu()
{
// printf("Collector fpe_menu is being created\n");

  fpe_menu = new QPopupMenu(this);
  connect(fpe_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorFPEReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;


  if( focusedExpID != -1 )
  {
    contextMenu->insertItem(QString("Display Options: (Exp: %1) FPE").arg(focusedExpID), fpe_menu);
  } else
  {
    contextMenu->insertItem(QString("Display Options: FPE"), fpe_menu);
  }

  list_of_fpe_modifiers.clear();
  list_of_fpe_modifiers.push_back("fpe::time");
  list_of_fpe_modifiers.push_back("fpe::counts");
  list_of_fpe_modifiers.push_back("fpe::percent");
  list_of_fpe_modifiers.push_back("fpe::ThreadAverage");
  list_of_fpe_modifiers.push_back("fpe::ThreadMin");
  list_of_fpe_modifiers.push_back("fpe::ThreadMax");
  list_of_fpe_modifiers.push_back("fpe::inexact_result_count");
  list_of_fpe_modifiers.push_back("fpe::underflow_count");
  list_of_fpe_modifiers.push_back("fpe::overflow_count");
  list_of_fpe_modifiers.push_back("fpe::division_by_zero_count");
  list_of_fpe_modifiers.push_back("fpe::unnormal_count");
  list_of_fpe_modifiers.push_back("fpe::invalid_count");
  list_of_fpe_modifiers.push_back("fpe::unknown_count");

  if( fpeModifierMenu )
  {
    delete fpeModifierMenu;
  }
  fpeModifierMenu = new QPopupMenu(this);
  addFPEReports(fpe_menu);
  fpeModifierMenu->insertTearOffHandle();
  connect(fpeModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(fpeModifierSelected(int)) );
  generateModifierMenu(fpeModifierMenu, list_of_fpe_modifiers, current_list_of_fpe_modifiers);
  fpe_menu->insertItem(QString("Select fpe Metrics:"), fpeModifierMenu);
}


void
StatsPanel::generateGenericMenu()
{
// printf("generateGenericMenu is being created\n");

  generic_menu = new QPopupMenu(this);
  connect(generic_menu, SIGNAL( activated(int) ),
           this, SLOT(collectorGenericReportSelected(int)) );

  QString s = QString::null;

  QAction *qaction = NULL;

  generic_menu->insertItem(QString("View Stats:"));

  contextMenu->insertItem(QString("Select %1 Metrics:").arg(currentCollectorStr), generic_menu);

//  list_of_generic_modifiers.clear();
  
  if( genericModifierMenu )
  {
    delete genericModifierMenu;
  }
  genericModifierMenu = new QPopupMenu(this);
  genericModifierMenu->insertTearOffHandle();
  connect(genericModifierMenu, SIGNAL( activated(int) ),
    this, SLOT(genericModifierSelected(int)) );
  generateModifierMenu(genericModifierMenu, list_of_generic_modifiers, current_list_of_generic_modifiers);

// printf("Try to generate the genericModifierMenu()\n");

  generic_menu->insertItem(QString("Select %1 Metrics:").arg(currentCollectorStr)
, genericModifierMenu);
}


#if 0
static void
debugList(QListView *splv)
{
// Debug print
SPListViewItem *top = (SPListViewItem *)splv->firstChild();
  printf("Debug:\n");
while( top )
{
  printf("  %s, %s", top->text(0).ascii(), top->text(fieldCount-1).ascii() );
  SPListViewItem *level1 = (SPListViewItem *)top->firstChild();
  while( level1 )
  {
    printf("  --%s, %s", level1->text(0).ascii(), level1->text(fieldCount-1).ascii() );

    SPListViewItem *level2 = (SPListViewItem *)level1->firstChild();
    while( level2 )
    {
      printf("  ----%s, %s", level2->text(0).ascii(), level2->text(fieldCount-1).ascii() );
  
      SPListViewItem *level3 = (SPListViewItem *)level2->firstChild();
      while( level3 )
      {
        printf("  ------%s, %s", level3->text(0).ascii(), level3->text(fieldCount-1).ascii() );
        SPListViewItem *level4 = (SPListViewItem *)level3->firstChild();
        while( level4 )
        {
          printf("  --------%s, %s", level4->text(0).ascii(), level4->text(fieldCount-1).ascii() );
          SPListViewItem *level5 = (SPListViewItem *)level4->firstChild();
          while( level5 )
          {
            printf("  ----------%s, %s", level5->text(0).ascii(), level5->text(fieldCount-1).ascii() );
        
            level5 = (SPListViewItem *)level5->nextSibling();
          }
      
          level4 = (SPListViewItem *)level4->nextSibling();
        }
    
        level3 = (SPListViewItem *)level3->nextSibling();
      }

      level2 = (SPListViewItem *)level2->nextSibling();
    }

    level1 = (SPListViewItem *)level1->nextSibling();
  }
  
  top = (SPListViewItem *)top->nextSibling();
}
  printf("End Debug\n");
}
// endif Debug
#endif // 0

void
StatsPanel::addMPIReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for MPI Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to MPI Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to MPI Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each MPI Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show Call Trees, with full stacks, to each MPI Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
#endif // PULL
}

void
StatsPanel::addIOReports(QPopupMenu *menu)
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
// printf("collector_name=(%s)\n", collector_name.c_str() );
  qaction->setToolTip(tr("Show timings for IO Functions."));

  qaction = new QAction(this, "showTracebacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show tracebacks to IO Functions."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show Call Trees to each IO Functions."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly view (caller/callees) for selected function."));

#ifdef PULL
  qaction = new QAction(this, "showCallTreesBySelectedFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees by Selected Function") );
  qaction->setToolTip(tr("Show Call Tree to MPI routine for selected function."));
#endif // PULL
}

void
StatsPanel::addUserTimeReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}


void
StatsPanel::addPCSampReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show LinkedObjects.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show Statements.") );

  qaction = new QAction(this, "showStatementsByFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements by Function") );
  qaction->setToolTip(tr("Show timings for statements by function. Select line containing function first."));
}


void
StatsPanel::addFPEReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show timings for Functions."));

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show timings for statements."));

  qaction = new QAction(this, "showStatementsByFunction");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements by Function") );
  qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show call trees for each function."));

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show trace backs for each function."));

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));


  qaction = new QAction(this, "showTracebacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}

void
StatsPanel::addHWCReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show by Functions.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show by Statements.") );

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );
}

void
StatsPanel::addHWCTimeReports(QPopupMenu *menu )
{
  QAction *qaction = new QAction(this, "showFunctions");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Functions") );
  qaction->setToolTip(tr("Show by Functions.") );

  qaction = new QAction(this, "showStatements");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Statements") );
  qaction->setToolTip(tr("Show by Statements.") );

qaction = new QAction(this, "showStatementsByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: Statements by Function") );
qaction->setToolTip(tr("Show timings for statements by function"));

  qaction = new QAction(this, "showLinkedObjects");
  qaction->addTo( menu );
  qaction->setText( tr("Show: LinkedObjects") );
  qaction->setToolTip(tr("Show by LinkedObjects.") );

  qaction = new QAction(this, "showCallTrees");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees") );
  qaction->setToolTip(tr("Show by CallTrees.") );

qaction = new QAction(this, "showCallTreesByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees by Function") );
qaction->setToolTip(tr("Show call trees for each function by function"));

  qaction = new QAction(this, "showCallTrees,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: CallTrees,FullStack") );
  qaction->setToolTip(tr("Show call trees, with full stacks, to Functions."));

qaction = new QAction(this, "showCallTrees,FullStackbyFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: CallTrees,FullStack by Function") );
qaction->setToolTip(tr("Show call trees, with full stacks, to functions by function"));

  qaction = new QAction(this, "showTraceBacks");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks") );
  qaction->setToolTip(tr("Show by TraceBacks.") );

qaction = new QAction(this, "showTraceBacksByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks by Function") );
qaction->setToolTip(tr("Show trace backs for each function by function"));

  qaction = new QAction(this, "showTraceBacks,FullStack");
  qaction->addTo( menu );
  qaction->setText( tr("Show: TraceBacks,FullStack") );
  qaction->setToolTip(tr("Show trace backs, with full stacks, to Functions."));

qaction = new QAction(this, "showTracebacks,FullStackByFunction");
qaction->addTo( menu );
qaction->setText( tr("Show: TraceBacks,FullStack by Function") );
qaction->setToolTip(tr("Show tracebacks, with full stacks, to IO functions by function."));

  qaction = new QAction(this, "showButterfly");
  qaction->addTo( menu );
  qaction->setText( tr("Show: Butterfly") );
  qaction->setToolTip(tr("Show Butterfly by function.") );

}


SourceObject *
StatsPanel::lookUpFileHighlights(QString filename, QString lineNumberStr, HighlightList *highlightList)
{
  SourceObject *spo = NULL;
  HighlightObject *hlo = NULL;

nprintf(DEBUG_PANELS) ("lookUpFileHighlights: filename=(%s) lineNumberStr=(%s)\n", filename.ascii(), lineNumberStr.ascii() );
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::lookUpFileHighlights, lfhA: expID=%d focusedExpID=%d\n", expID, focusedExpID );
 printf("StatsPanel::lookUpFileHighlights, currentMetricStr=%s\n", currentMetricStr.ascii() );
 printf("StatsPanel::lookUpFileHighlights, currentThreadsStr=(%s)\n", currentThreadsStr.ascii() );
 printf("StatsPanel::lookUpFileHighlights, currentUserSelectedReportStr=(%s)\n", currentUserSelectedReportStr.ascii() );
 printf("StatsPanel::lookUpFileHighlights, timeIntervalString=(%s)\n", timeIntervalString.ascii() );
#endif

  QString command = QString::null;

  QFileInfo qfi(filename);
  QString _fileName  = qfi.fileName();


  QString fn  = filename;
  int basename_index = filename.findRev("/");
  if( basename_index != -1 )
  {
    fn =  filename.right((filename.length()-basename_index)-1);
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, file BaseName=(%s)\n", fn.ascii() );
#endif

  if( currentMetricStr.isEmpty() ) {
    if( _fileName.isEmpty() ) {
      return(spo);
    }

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::lookUpFileHighlights, lfhB: expID=%d focusedExpID=%d\n", expID, focusedExpID );
#endif

    if( expID > 0 ) {

      // Trace experiments do not have Statements metrics! why?
      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {
        command = QString("expView -x %1 -f %2 %3").arg(expID).arg(fn).arg(timeIntervalString);
      } else {
        command = QString("expView -x %1 -v statements -f %2 %3").arg(expID).arg(fn).arg(timeIntervalString);
      }
    } else {

      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {
          command = QString("expView -x %1 -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
      } else {
          command = QString("expView -x %1 -v statements -f %2 %3").arg(focusedExpID).arg(fn).arg(timeIntervalString);
      }
    }
  } else {

    if( expID > 0 ) {
      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {
          command = QString("expView -x %1 -f %2 -m %3 %4").arg(expID).arg(fn).arg(currentMetricStr).arg(timeIntervalString);
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights, 33, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif
      } else {
          command = QString("expView -x %1 -v statements -f %2 -m %3 %4").arg(expID).arg(fn).arg(currentMetricStr).arg(timeIntervalString);
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights2, 33, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif
      }
    } else {

      if (currentCollectorStr == "io" || 
          currentCollectorStr == "iot" ||
	  currentCollectorStr == "mpi" || 
          currentCollectorStr == "mpit" ) {
          command = QString("expView -x %1 -f %2 -m %3 %4").arg(focusedExpID).arg(fn).arg(currentMetricStr).arg(timeIntervalString);
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights, 44, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif
      } else {
          command = QString("expView -x %1 -v statements -f %2 -m %3 %4").arg(focusedExpID).arg(fn).arg(currentMetricStr).arg(timeIntervalString);
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::lookUpFileHighlights2, 44, fn=%s, command=(%s)\n", fn.ascii(), command.ascii() );
#endif
      }
    }
  }

  if( !currentThreadStr.isEmpty() ) {
    command += QString(" %1").arg(currentThreadsStr);
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, command=(%s)\n", command.ascii() );
#endif

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  InputLineObject *clip = cli->run_Append_Input_String( cli->wid, (char *)command.ascii());


  if( clip == NULL ) {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    QApplication::restoreOverrideCursor();
  }

  Input_Line_Status status = ILO_UNKNOWN;

  while( !clip->Semantics_Complete() )
  {
    status = cli->checkStatus(clip, command);
    if( !status || status == ILO_ERROR )
    { // An error occurred.... A message should have been posted.. return;
      QApplication::restoreOverrideCursor();
      if( clip )
      {
        clip->Set_Results_Used();
        clip = NULL;
      }
      break;
    }

    qApp->processEvents(1000);

    if( !cli->shouldWeContinue() )
    {
      QApplication::restoreOverrideCursor();
      if( clip )
      {
        clip->Set_Results_Used();
        clip = NULL;
      }
      break;
    }

    sleep(1);
  }

  process_clip(clip, highlightList, FALSE);
//  process_clip(clip, highlightList, TRUE);

  clip->Set_Results_Used();
  clip = NULL;

  int lineNumber = lineNumberStr.toInt();
  QString value = QString::null;
  QString description = QString::null;
  QString value_description = QString::null;
  QString color = QString::null;
  HighlightObject *focusedHLO = NULL;
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    if( hlo && focusedHLO == NULL )
    {
      focusedHLO = hlo;
//      hlo->print();
    }
    if( value_description.isEmpty() )
    {
      value_description = hlo->value_description;
    }
    if( hlo->line == lineNumber )
    {
      value = hlo->value;
      description = hlo->description;
      break;
    }
  }

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::lookUpFileHighlights, if true use hotToCold_color_names[2], value.isEmpty()=(%d)\n", value.isEmpty() );
#endif

  if( value.isEmpty() ) {

    hlo = new HighlightObject(QString::null, filename, 
                              lineNumberStr.toInt(), 
                              hotToCold_color_names[2], 
                              ">>", 
                              "Callsite for this function", 
                              value_description);

  } else {

    highlightList->remove(hlo);
    hlo = new HighlightObject(QString::null, focusedHLO->fileName, 
                              lineNumberStr.toInt(), 
                              color, 
                              QString(">> %1").arg(value), 
                              QString("Callsite for this function.\n%1").arg(description), value_description);
  }

  highlightList->push_back(hlo);


#if 0
  for( HighlightList::Iterator it = highlightList->begin();
       it != highlightList->end();
       ++it)
  {
    hlo = (HighlightObject *)*it;
    hlo->print();
  }
#endif // 0

  if( focusedHLO )
  {
//  focusedHLO->print();
    if( focusedHLO->fileName != filename )
    {
      nprintf(DEBUG_PANELS) ("THE FILE NAMES %s != %s\n", focusedHLO->fileName.ascii(), filename.ascii() );
      if( !focusedHLO->fileName.isEmpty() )
      {
	nprintf(DEBUG_PANELS) ("lhfa: focusedHLO->fileName.isEmpty, CHANGE THE FILENAME!!!\n");
        filename = focusedHLO->fileName;
      }
    }
  }


  spo = new SourceObject("functionName", filename.ascii(), lineNumberStr.toInt()-1, expID, TRUE, highlightList);
  nprintf(DEBUG_PANELS) ("spo->fileName=(%s)\n", spo->fileName.ascii() );

#if 0
// Begin debug
  spo->print();
// End debug
#endif // 0



  return spo;
}


void
StatsPanel::process_clip(InputLineObject *statspanel_clip, 
                         HighlightList *highlightList=NULL, 
                         bool dumpClipFLAG=FALSE)
{

#ifdef DEBUG_StatsPanel
    dumpClipFLAG = TRUE;
    printf("StatsPanel::process_clip entered\n");
    printf("StatsPanel::process_clip entered &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  if( __internal_debug_setting & DEBUG_CLIPS )
  {
    dumpClipFLAG = TRUE;
  }
#ifdef DEBUG_StatsPanel
  printf("StatsPanel::process_clip before NULL check &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip, statspanel_clip);
#endif

  if( statspanel_clip == NULL )
  {
    cerr << "No clip to process.\n";
  }

  QString valueStr = QString::null;
  QString xxxfileName = QString::null;
  QString xxxfuncName = QString::null;
  int xxxlineNumber = -1;
  HighlightObject *hlo = NULL;

  std::list<CommandObject *>::iterator coi;

#ifdef DEBUG_StatsPanel
  printf("StatsPanel::process_clip before NULL check &statspanel_clip=0x%x, statspanel_clip=0x%x\n", &statspanel_clip,statspanel_clip);
#endif

  coi = statspanel_clip->CmdObj_List().begin();

  CommandObject *co = (CommandObject *)(*coi);
  if( co == NULL )
  {
    cerr << "No command object in clip to process.\n";
  }

  std::list<CommandResult *>::iterator cri;
  std::list<CommandResult *> cmd_result = co->Result_List();
  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++)
  {
    int skipFLAG = FALSE;
    if( dumpClipFLAG) cerr<< "DCLIP: " <<  "TYPE: " << (*cri)->Type() << "\n";
    if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES)
    {
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::process_clip, Here CMD_RESULT_COLUMN_VALUES:\n");
#endif
      std::list<CommandResult *> columns;
      CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
      ccp->Value(columns);

      std::list<CommandResult *>::iterator column_it;
      int i = 0;
      columnFieldList.clear();
      for (column_it = columns.begin(); column_it != columns.end(); column_it++)
      {
      
        if( dumpClipFLAG) cerr << "DCLIP: " << (*column_it)->Form().c_str() << "\n";
#ifdef DEBUG_StatsPanel
  cerr << "  " << (*column_it)->Form().c_str() << "\n";
#endif
        QString vs = (*column_it)->Form().c_str();
        columnFieldList.push_back(vs);

        CommandResult *cr = (CommandResult *)(*column_it);
        if( dumpClipFLAG) cerr << "DCLIP: " <<  "cr->Type=" << cr->Type() << "\n";
        if( i == 0 && highlightList )
        {
          valueStr = QString::null;
        }
        switch( cr->Type() )
        {
          case CMD_RESULT_NULL:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_NULL\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_UINT:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_UINT\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_INT:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_INT\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_FLOAT:
            {
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_FLOAT\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            }
            break;
          case CMD_RESULT_STRING:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_STRING\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_RAWSTRING:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_RAWSTRING\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
            break;
          case CMD_RESULT_FUNCTION:
          {
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_FUNCTION\n";
              CommandResult_Function *crf = (CommandResult_Function *)cr;
              std::string S = crf->getName();
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "    S=" << S << "\n";
//            LinkedObject L = crf->getLinkedObject();
//            if( dumpClipFLAG) cerr << "    L.getPath()=" << L.getPath() << "\n";

              std::set<Statement> T = crf->getDefinitions();
              if( T.size() > 0 )
              {
                std::set<Statement>::const_iterator ti = T.begin();
                Statement s = *ti;
                if( dumpClipFLAG) cerr << "DCLIP: " <<  "    s.getPath()=" << s.getPath() << "\n";
                if( dumpClipFLAG) cerr << "DCLIP: " <<  "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

                xxxfuncName = S.c_str();
                xxxfileName = QString( s.getPath().c_str() );
//                xxxfileName = QString( s.getPath().getBaseName().c_str() );
                xxxlineNumber = s.getLine();
                if( dumpClipFLAG )
                {
                  cerr << "DCLIP: " <<  "xxxfuncName=" << xxxfuncName << "\n";
                  cerr << "DCLIP: " <<  "xxxfileName=" << xxxfileName << "\n";
                  cerr << "DCLIP: " <<  "xxxlineNumber=" << xxxlineNumber << "\n";
                }
#ifdef DEBUG_StatsPanel
                cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxfuncName=" << xxxfuncName << "\n";
                cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxfileName=" << xxxfileName << "\n";
                cerr << "DLCIP: CMD_RESULT_FUNCTION: xxxlineNumber=" << xxxlineNumber << "\n";
#endif
              }

              
          }
            break;
          case CMD_RESULT_STATEMENT:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_STATEMENT\n";
            {


              CommandResult_Statement *T = (CommandResult_Statement *)cr;
              Statement s = (Statement)*T;
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "    s.getPath()=" << s.getPath() << "\n";
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

              xxxfuncName = QString::null;
              xxxfileName = QString( s.getPath().c_str() );
//              xxxfileName = QString( s.getPath().getBaseName().c_str() );
              xxxlineNumber = s.getLine();
              if( dumpClipFLAG )
              {
                cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxfuncName=" << xxxfuncName << "\n";
                cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxfileName=" << xxxfileName << "\n";
                cerr << "DCLIP: CMD_RESULT_STATEMENT:" <<  "xxxlineNumber=" << xxxlineNumber << "\n";
              }
              if( highlightList ) {

                QString colheader = (QString)*columnHeaderList.begin();
                int color_index = getLineColor((unsigned int)valueStr.toUInt());
                hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                          hotToCold_color_names[currentItemIndex], 
                                          valueStr.stripWhiteSpace(), 
                                          QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), colheader);

                if( dumpClipFLAG ) hlo->print();
                highlightList->push_back(hlo);

              }

            }
            break;
          case CMD_RESULT_LINKEDOBJECT:
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_LINKEDOBJECT\n";
            break;
          case CMD_RESULT_CALLTRACE:
          {
            if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_CALLTRACE\n";
            CommandResult_CallStackEntry *CSE = (CommandResult_CallStackEntry *)cr;


            std::vector<CommandResult *> *CSV = CSE->Value();
            int64_t sz = CSV->size();
            std::vector<CommandResult *> *C1 = CSV;
	    if( dumpClipFLAG) cerr << "DCLIP: " <<  "  CMD_RESULT_CALLTRACE: Form ="
				   << CSE->Form().c_str() << "sz= " <<
				   sz << "\n";

            CommandResult *CE = (*C1)[sz - 1];
            if( CE->Type() == CMD_RESULT_FUNCTION )
            {
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "  CMD_RESULT_CALLTRACE: sz=" << sz
				     << " and function ="
				     << CE->Form().c_str() << "\n";

              std::string S = ((CommandResult_Function *)CE)->getName();
              xxxfuncName = S.c_str();
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "((CommandResult_Function *)CE)->getName() == S=" << S << "\n";

//            LinkedObject L = ((CommandResult_Function *)CE)->getLinkedObject(); 
//            if( dumpClipFLAG) cerr << "    L.getPath()=" << L.getPath() << "\n";

#ifdef DEBUG_StatsPanel
              LinkedObject L = ((CommandResult_Function *)CE)->getLinkedObject(); 
              if( dumpClipFLAG) cerr << "DCLIP: " <<  "    L.getPath()=" << L.getPath() << "\n";
#endif

#if 0
              std::set<Statement> T = ((CommandResult_Function *)CE)->getDefinitions();
              if( T.size() > 0 )
              {
                std::set<Statement>::const_iterator ti = T.begin();
                Statement s = *ti;
                if( dumpClipFLAG) cerr << "    s.getPath()=" << s.getPath() << "\n";
                if( dumpClipFLAG) cerr << "    (int64_t)s.getLine()=" << (int64_t)s.getLine() << "\n";

                xxxfileName = QString( s.getPath().c_str() );
//                xxxfileName = QString( s.getPath().getBaseName().c_str() );
                xxxlineNumber = s.getLine();
	        if( dumpClipFLAG) cerr <<
			"  CMD_RESULT_CALLTRACE lineNumber via getDefinitions "
			<< xxxlineNumber << "\n";
              }
#endif

	      // IMPORTANT: This will focus on the actual linenumber that
	      // corresponds to the address within the function!
	      // Overrides xxxlineNumber computed above.
              std::set<Statement> TT; 
              ((CommandResult_Function *)CE)->Value(TT);

#ifdef DEBUG_StatsPanel
              printf("StatsPanel::process_clip, FUNCTION: TT.begin() != TT.end()=%d, (TT.size() > 0)=%d\n",
                     (TT.begin() != TT.end()), (TT.size() > 0));
#endif


	      if (TT.begin() != TT.end() || TT.size() > 0) {
          	std::set<Statement>::const_iterator sti = TT.begin();;
          	Statement S = *sti;
          	char l[50];
          	sprintf( &l[0], "%lld", (int64_t)(S.getLine()));
                xxxfileName = QString( S.getPath().c_str() );
                xxxlineNumber = S.getLine();
#ifdef DEBUG_StatsPanel
		printf("StatsPanel::process_clip, FUNCTION: lineNumber via getValue is %d, fileName is %s\n",xxxlineNumber, xxxfileName.ascii());
#endif
		nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: lineNumber via getValue is %d, fileName is %s\n",xxxlineNumber, xxxfileName.ascii());
              } else {
#ifdef DEBUG_StatsPanel
		printf("StatsPanel::process_clip, FUNCTION: TT.begin == TT.end!, no statement info. reset xxxfileName and xxxlineNumber\n");
#endif
		nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: TT.begin == TT.end!, no statement info. reset xxxfileName and xxxlineNumber\n");
		xxxfileName = QString::null;
		xxxlineNumber = -1;
	      }

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, FUNCTION: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                      xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);
#endif

	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, FUNCTION: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                                    xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);

            } else if( CE->Type() == CMD_RESULT_STATEMENT )
            {
#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, CMD_RESULT_STATEMENT: sz=%d, function=%s\n",
                     sz,CE->Form().c_str());
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip,   CMD_RESULT_STATEMENT: sz=%d, function=%s\n",
                                    sz,CE->Form().c_str());

              CommandResult_Statement *T = (CommandResult_Statement *)CE;
              Statement s = (Statement)*T;

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, STATEMENT: s.getPath()=%s, s.getLine()=%d\n",
                     s.getPath().c_str(),(int64_t)s.getLine());
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, STATEMENT: s.getPath()=%s, s.getLine()=%d\n",
                                    s.getPath().c_str(),(int64_t)s.getLine());

              xxxfuncName = CE->Form().c_str();
              xxxfileName = QString( s.getPath().c_str() );
//              xxxfileName = QString( s.getPath().getBaseName().c_str() );
              xxxlineNumber = s.getLine();

#ifdef DEBUG_StatsPanel
	      printf("StatsPanel::process_clip, STATEMENT: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                     xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);
#endif
	      nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, STATEMENT: xxxfuncName=%s, xxxfileName=%s, xxxlineNumber=%d\n",
                                     xxxfuncName.ascii(),xxxfileName.ascii(),xxxlineNumber);

              if( highlightList )
              {
                QString colheader = (QString)*columnHeaderList.begin();
                int color_index = getLineColor((unsigned int)valueStr.toUInt());
                hlo = new HighlightObject(xxxfuncName, xxxfileName, xxxlineNumber, 
                                          hotToCold_color_names[currentItemIndex], 
                                          valueStr.stripWhiteSpace(), 
                                          QString("%1 = %2").arg(colheader).arg(valueStr.ascii()), 
                                          colheader );

                if( dumpClipFLAG ) hlo->print();
                highlightList->push_back(hlo);
              }

            } else
            {
              if( dumpClipFLAG ) cerr << "DCLIP: " <<  "How do I handle this type? CE->Type() " << CE->Type() << "\n";

#ifdef DEBUG_StatsPanel
	       printf("StatsPanel::process_clip, CALLTRACE: How do I handle this type? %d\n", CE->Type());
#endif
	       nprintf(DEBUG_CLIPS) ("StatsPanel::process_clip, CALLTRACE: How do I handle this type? %d\n", CE->Type());
            }
          }
          break;
        case CMD_RESULT_TIME:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_TIME\n";
            if( i == 0 && highlightList )
            {
              valueStr = (*column_it)->Form().c_str();
            }
          break;
        case CMD_RESULT_TITLE:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_TITLE\n";
          break;
        case CMD_RESULT_COLUMN_HEADER:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_HEADER\n";
          break;
        case CMD_RESULT_COLUMN_VALUES:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_VALUES\n";
          break;
        case CMD_RESULT_COLUMN_ENDER:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_COLUMN_ENDER\n";
          break;
        case CMD_RESULT_EXTENSION:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_EXTENSION\n";
          break;
        default:
          if( dumpClipFLAG) cerr << "DCLIP: " <<  "Got CMD_RESULT_EXTENSION\n";
          break;
        }

        i++;
      }

    } else if ((*cri)->Type() == CMD_RESULT_STRING) {  
      // This looks to be a message we should display

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, Here CMD_RESULT_STRING:\n");
#endif
      QString s = QString((*cri)->Form().c_str());
        
      QMessageBox::information( (QWidget *)this, tr("Info:"), s, QMessageBox::Ok );
      skipFLAG = TRUE;
//      break;
    } else if( (*cri)->Type() == CMD_RESULT_COLUMN_HEADER ) {
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::process_clip, Here CMD_RESULT_COLUMN_HEADER:\n");
#endif
      std::list<CommandResult *> columns;
      CommandResult_Columns *ccp = (CommandResult_Columns *)*cri;
      ccp->Value(columns);

      std::list<CommandResult *>::iterator column_it;
      int i = 0;
      columnFieldList.clear();
      for (column_it = columns.begin(); column_it != columns.end(); column_it++)
      {

#ifdef DEBUG_StatsPanel
        cerr << "CMD_RESULT_COLUMN_HEADER:  " << (*column_it)->Form().c_str() << "\n";
#endif

        QString vs = (*column_it)->Form().c_str();
        columnFieldList.push_back(vs);

      }
    } else {

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, Here OTHER:\n");
#endif
    }

    /* You have found the next row!! */
    // Here's a formatted row
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::process_clip, Here you have found the next row - here is the formatted row:\n");
#endif

    if( dumpClipFLAG) cerr << "DCLIP: " <<  (*cri)->Form().c_str() << "\n";

    // DUMP THIS TO OUR "OLD" FORMAT ROUTINE.
//    if( highlightList == NULL )
// printf("skipFLAG == FALSE\n");
    if( highlightList == NULL && skipFLAG == FALSE ) {

      QString s = QString((*cri)->Form().c_str());

#ifdef DEBUG_StatsPanel
      printf("StatsPanel::process_clip, printing the QString s as output %s\n", s.ascii() );
      printf("StatsPanel::process_clip, calling outputCLIData, xxxfileName.ascii()=%s,printing the QString s as output %s\n", 
              xxxfileName.ascii(), s.ascii() );
#endif

      outputCLIData( xxxfuncName, xxxfileName, xxxlineNumber );
    }

  } // end for through results

}


static bool step_forward = TRUE;
void
StatsPanel::progressUpdate()
{
#ifdef DEBUG_StatsPanel
//  Lots of these occur so commenting this out unless really needed.
//  printf("StatsPanel::progressUpdate, progressUpdate() entered\n");
#endif
  pd->qs->setValue( steps );
  if( step_forward )
  {
    steps++;
  } else
  {
    steps--;
  }
  if( steps == 100 )
  {
//    step_forward = FALSE;
    step_forward = TRUE;
  } else if( steps == 0 )
  {
    step_forward = TRUE;
  }
}

void 
StatsPanel::insertDiffColumn(int insertAtIndex)
{
// The output out has been added to the StatsPanel.   Do you want to add
// the "Difference" column.
  QPtrList<QListViewItem> lst;
  QListViewItemIterator it( splv );
  int index = splv->addColumn("|Difference|", 200);
  int columnCount = splv->columns();

  int i=index;
  // First move the header columns.
  for(;i>insertAtIndex;i--)
  {
    splv->setColumnText( i, splv->columnText(i-1)  );
    splv->setColumnWidth( i, splv->columnWidth(i-1) );
  }
  splv->setColumnText( i, "|Difference|" );

  while ( it.current() )
  {
    QListViewItem *item = it.current();
#ifdef DEBUG_StatsPanel
   printf("StatsPanel::insertDiffColumn,ls=%s rs=%s\n", item->text(0).ascii(), item->text(1).ascii() );
#endif
    QString ls = item->text(0);
    QString rs = item->text(1);
    double lsd = ls.toDouble();
    double rsd = rs.toDouble();
    double dd = lsd-rsd;
    double add = fabs(dd);

    for(i=index;i>insertAtIndex;i--)
    {
      item->setText(i,     item->text(i-1)  );
    }
    item->setText(i, QString("%1").arg(add));

    ++it;
  }
}


void 
StatsPanel::removeDiffColumn(int removeIndex)
{
  splv->removeColumn(removeIndex);
}

void
StatsPanel::analyzeTheCView()
{
#ifdef DEBUG_StatsPanel
   printf("analyzeTheCView(%s) entered: focusedExpID was=%d\n", lastCommand.ascii(), focusedExpID );

   printf("analyzeTheCView, RESETTING focusedExpID to -1\n");
#endif
//  focusedExpID = -1;

  if( !lastCommand.startsWith("cview -c") )
  {
    return;
  }

  QValueList<QString> cidList;

#ifdef DEBUG_StatsPanel
  printf("analyzeTheCView, lastCommand =(%s)\n", lastCommand.ascii() );
#endif
  int vindex = lastCommand.find("-v");
#ifdef DEBUG_StatsPanel
   printf("analyzeTheCView, vindex = %d\n", vindex);
#endif
  int mindex = lastCommand.find("-m");
#ifdef DEBUG_StatsPanel
   printf("analyzeTheCView, mindex = %d\n", mindex);
#endif
  int end_index = vindex;
  if( vindex == -1 )
  {
    end_index = mindex;
  }
  if( mindex != -1 && mindex < vindex )
  {
    end_index = mindex;
  }
#ifdef DEBUG_StatsPanel
  printf("analyzeTheCView, end_index= %d\n", end_index);
#endif
  QString ws = QString::null;
  if( end_index == -1 )
  {
    ws = lastCommand.mid(9,999999);
  } else
  {
    ws = lastCommand.mid(9,end_index-10);
  }
#ifdef DEBUG_StatsPanel
  printf("analyzeTheCView, ws=(%s)\n", ws.ascii() );
#endif
  int cnt = ws.contains(",");
  if( cnt > 0 )
  {
    for(int i=0;i<=cnt;i++)
    {
      cidList.push_back( ws.section(",", i, i).stripWhiteSpace() );
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, cid list count=%d, compare id: cidList push back (%s)\n", cnt, ws.section(",", i, i).stripWhiteSpace().ascii() );
#endif
    }
  }

  if( cnt == 0 )
  {
#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, We only have one cview... option (%s).\n", ws.ascii() );
#endif
    cidList.push_back( ws );
  }

  for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
  {
    CInfoClass *cic = (CInfoClass *)*it;
    delete(cic);
  }
  cInfoClassList.clear();

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

#ifdef DEBUG_StatsPanel
  printf("analyzeTheCView, coming: currentMetricStr was %s\n", currentMetricStr.ascii() );
#endif
  currentMetricStr = QString::null;
  InputLineObject *clip = NULL;
  std::string cstring;
  infoAboutComparingString += QString("Comparing:");
  int max_cid_value = -1;
  int columnNum = 1;

  for( QValueList<QString>::Iterator it = cidList.begin(); it != cidList.end(); ++it)
  {
    QString cid_str = (QString)*it;

#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, TOP of QVALUELIST, cid_str=(%s)\n", cid_str.ascii() );
#endif

    QString command = QString("cviewinfo -c %1 %2").arg(cid_str).arg(timeIntervalString);
    if( !cli->getStringValueFromCLI( (char *)command.ascii(),
           &cstring, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
    QString str = QString( cstring.c_str() );
#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, START-STRING ANALYSIS compare id string: str=(%s)\n", str.ascii() );
#endif
    int cid = -1;
    int expID = -1;
    QString collectorStr = QString::null;
    QString metricStr = QString::null;
    QString infoRankStr = QString::null;
    int start_host = -1;
    int start_expid = -1;
    int end_expid_index = -1;
    int end_host_index = -1;
    int start_process = -1;
    int end_process_index = -1;
    int start_rank = -1;
    int end_rank_index = -1;
    int start_metric = -1;
    int end_metric_index = -1;
 
    int str_length = str.length();

    int start_index = 3;
    int end_index = str.find(":");
    int colon_index = end_index;


    // Look up cid (-c)
    QString cidStr = str.mid(start_index, end_index-start_index);
    cid = cidStr.toInt();

    if (cid > max_cid_value) {
       max_cid_value = cid;
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, str_length=%d, cid=%d, max_cid_value=%d, start_index=%d, end_index=%d, colon_index=%d\n", 
             str_length, cid, max_cid_value, start_index, end_index, colon_index);
#endif

    // Look up collector name.
    start_index = end_index+1;
    end_index = str.find("-x");
    // Pretty sure this will always find -x
    if (end_index != -1) {
      collectorStr = str.mid(start_index, end_index-start_index).stripWhiteSpace();
    }

    // Look up expID (-x)
    start_index = end_index+3;
    start_expid = end_index+3;
    
    int new_index = findNextMajorToken(str, start_index, QString("-h") );

    if (new_index != -1) {
      end_expid_index = new_index;
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,return from findNextMajorToken, for -x end, new_index=%d, end_expid_index=%d\n", new_index, end_expid_index);
#endif

    end_index = str.find("-h", start_index);
    if (end_index != -1) {
       start_host = end_index;
       // look for the end of -h host string, add 2 for -h
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, about to call findNextMajorToken, for -h end, start_host+2=%d\n", start_host+2 );
#endif
       new_index = findNextMajorToken(str, start_host+2, QString("-r") );
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,return from findNextMajorToken, for -h end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_host_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, HOST INFO results start_host=%d, start_index=%d, end_host_index=%d\n", start_host, start_index, end_host_index);
#endif

    // Look up metric
    end_index = str.find("-m", start_index);
    if (end_index != -1) {
       start_metric = end_index;
       // look for the end of -m metric string, add 2 for -m
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,about to call findNextMajorToken, for -m end, start_metric+2=%d\n", start_metric+2 );
#endif
       new_index = findNextMajorToken(str, start_metric+2, QString("-r") );
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,return from findNextMajorToken, for -m end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_metric_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, METRIC INFO results start_metric=%d, start_index=%d, end_metric_index=%d\n", start_metric, start_index, end_metric_index);
#endif

    // Look up process
    end_index = str.find("-p", start_index);
    if( end_index != -1 ) {
        start_process = end_index;
       // look for the end of -p process string, add 2 for -p
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,about to call findNextMajorToken, for -p end, start_process+2=%d\n", start_process+2 );
#endif
       new_index = findNextMajorToken(str, start_process+2, QString(";") );
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,return from findNextMajorToken, for -p end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_process_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, process info results start_process=%d, start_index=%d, end_process_index=%d\n", start_process, start_index, end_process_index);
#endif

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, LOOKING FOR RANK, start_index=%d\n", start_index);
#endif

    // Look up rank info
    end_index = str.find("-r", start_index);
    if( end_index != -1 ) {
       start_rank = end_index;
       // look for the end of -r rank string, add 2 for -r
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, about to call findNextMajorToken, for -r end, start_rank+2=%d\n", start_rank+2 );
#endif
       new_index = findNextMajorToken(str, start_rank+2, QString(";") );
#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView,return from findNextMajorToken, for -r end, new_index=%d\n", new_index );
#endif
       if (new_index != -1) {
         end_rank_index = new_index;
       }
    }

#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, RANK INFO RESULTS start_rank=%d, start_index=%d, end_rank_index=%d\n", start_rank, start_index, end_rank_index);
#endif

    if (start_rank != -1 && end_rank_index != -1) {
        infoRankStr = str.mid(start_rank+3, end_rank_index-start_rank+2);
#ifdef DEBUG_StatsPanel
        printf("analyzeTheCView, RANK INFO VALUES: start_rank = %d, end_rank_index=%d, infoRankStr.ascii()=(%s)\n", start_rank, end_rank_index, infoRankStr.ascii());
#endif
    }


#ifdef DEBUG_StatsPanel
      printf("analyzeTheCView, END-STRING ANALYSIS compare id string: cid=%d, str=(%s)\n", cid, str.ascii() );
      printf("analyzeTheCView, start_host=%d, start_index=%d, end_index=%d\n", start_host, start_index, end_index);
      printf("analyzeTheCView, start_metric=%d, start_process=%d, start_rank=%d\n", start_metric, start_process, start_rank);
      printf("analyzeTheCView, start_expid=%d, end_expid_index=%d\n", start_expid, end_expid_index);
#endif

    if(!cidStr.isEmpty() ) {
      infoAboutComparingString += QString("\nColumn %1: ").arg( columnNum );
    }

    QString expIDStr = str.mid(start_expid, end_expid_index-start_expid);
    expID = expIDStr.toInt();

    infoAboutComparingString += QString("Experiment %1 ").arg(expID);
#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, A: expID = %d, expIDStr.ascii()=(%s)\n", expID, expIDStr.ascii());
#endif

    // Make a list of comma separated experiment ids for the stats panel info header
    infoAboutStringCompareExpIDs += QString("%1,").arg(expID);

    QString host_pid_names = QString::null;

#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, start_host = (%d), infoAboutStringCompareExpIDs.ascii()=(%s)\n", start_host, infoAboutStringCompareExpIDs.ascii());
#endif

    if( start_host != -1 )
    {
      start_index = start_host;
      end_index = str.find("-m");
      if( end_index == -1 )
      {
        end_index = 999999;
      }
      host_pid_names = str.mid(start_index, end_index-start_index);
    }

    // Look up metricStr
#ifdef DEBUG_StatsPanel
    printf("look up the metricStr in str? str=(%s)\n", str.ascii() );
#endif

    start_index = str.find("-m");
    if( start_index == -1 )
    {  // see if there's one on the original command?
      // HACK! HACK!   HACK!!

      str = lastCommand;
      start_index = str.find("-m");
    }

    if( start_index != -1 )
    {
      start_index += 3;  // Skip the -m
      // metricStr = str.right(start_index);
      metricStr = str.mid(start_index, 9999999);
    } else
    {
      metricStr = QString::null;
    }

    if( currentMetricStr.isEmpty() )
    {
      currentMetricStr = metricStr;
#ifdef DEBUG_StatsPanel
      printf("StatsPanel::analyzeTheCView, currentMetricStr set to %s\n", currentMetricStr.ascii() );
      printf("StatsPanel::analyzeTheCView, new CInfoClass:cid=%d collectorStr=(%s) expID=(%d) host_pid_names=(%s) metricStr=(%s)\n",
             cid, collectorStr.ascii(), expID, host_pid_names.ascii(), metricStr.ascii() );
#endif
    }
    
    // There are at least two ways to come into this section of code
    // for ranks and hosts.
    // One way is with only ranks specified.  This is the group similar processes option.
    //    - for this option we separate out the ranks and report which ranks are
    //    - being compared for each respective column displayed
    // Another way is with hosts and ranks specified.  That is the else clause below.
    //    - we try to pretty this option up by changing the -h to "Host:" and the
    //    - "-r" to "Rank:" and remove line feeds and extra spaces to form a more
    //    - user friendly display.
#ifdef DEBUG_StatsPanel
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, start_host=%d\n", start_host );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, start_index=%d\n", start_index );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, infoRankStr.isEmpty()=%d\n", infoRankStr.isEmpty() );
     printf("StatsPanel::analyzeTheCView, before infoRankStr and is there a -h check, str.find(-h, start_index)=%d\n", str.find("-h", start_index));
#endif
    
    if(!infoRankStr.isEmpty() && start_host == -1) {
      infoAboutComparingString += QString(": Rank(s) %1 ").arg( infoRankStr );
    } else {

      QString my_host_pid_names = "";

#ifdef DEBUG_StatsPanel
     printf("StatsPanel::analyzeTheCView, before -h checks, host_pid_names=(%s)\n", host_pid_names.ascii() );
#endif

     if(!host_pid_names.isEmpty() ) {
       int h_search_start_index = 0;
       int minus_h_dx = -1;
       while (h_search_start_index != -1) {
         minus_h_dx = host_pid_names.find("-h", h_search_start_index);
         if (minus_h_dx != -1) {
           my_host_pid_names = host_pid_names.replace( minus_h_dx, 2, "Host:");
         }
         h_search_start_index = minus_h_dx;
       }
 
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::analyzeTheCView, after -h checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
       int r_search_start_index = 0;
       int minus_r_dx = -1;
       while (r_search_start_index != -1) {
         minus_r_dx = host_pid_names.find("-r", r_search_start_index);
         if (minus_r_dx != -1) {
           my_host_pid_names = host_pid_names.replace( minus_r_dx, 2, "Rank:");
         }
         r_search_start_index = minus_r_dx;
       }
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::analyzeTheCView, after -r checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       // Strip out the end of lines (line feeds), so the output come on one line
       int bslash_n_search_start_index = 0;
       int bslash_n_dx = -1;
       while (bslash_n_search_start_index != -1) {
         bslash_n_dx = host_pid_names.find("\n", bslash_n_search_start_index);
         if (bslash_n_dx != -1) {
           my_host_pid_names = host_pid_names.remove( bslash_n_dx, 2);
         }
         bslash_n_search_start_index = bslash_n_dx;
       }
 
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::analyzeTheCView, after bslash checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       // Strip out the spaces characters (5 spaces), so the output come on one line
       int spaces_search_start_index = 0;
       int spaces_dx = -1;
 
       while (spaces_search_start_index != -1) {
 
         spaces_dx = host_pid_names.find("     ", spaces_search_start_index);
 
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::analyzeTheCView, in spaces checks, spaces_search_start_index=%d, spaces_dx=%d, my_host_pid_names=(%s)\n", spaces_search_start_index, spaces_dx, my_host_pid_names.ascii() );
#endif
 
         if (spaces_dx != -1) {
           my_host_pid_names = host_pid_names.remove( spaces_dx, 5);
         }
 
         spaces_search_start_index = spaces_dx;
       }
#ifdef DEBUG_StatsPanel
       printf("StatsPanel::analyzeTheCView, after 5 spaces checks, my_host_pid_names=(%s)\n", my_host_pid_names.ascii() );
#endif
 
       infoAboutComparingString += QString(": Showing %1 ").arg( my_host_pid_names );
     }
    }

    if(!currentMetricStr.isEmpty() ) {
      infoAboutComparingString += QString("\nfor performance data type: %1 ").arg( collectorStr );
    }
    if (!metricStr.isEmpty()) {
      infoAboutComparingString += QString("using display option: %1 ").arg( metricStr );
    }

    CInfoClass *cic = new CInfoClass( cid, collectorStr, expID, host_pid_names, metricStr );
    cInfoClassList.push_back( cic );
#ifdef DEBUG_StatsPanel
    printf("StatsPanel::analyzeTheCView, push this out..,  infoAboutComparingString.ascii()=%s\n", infoAboutComparingString.ascii());
    cic->print();
#endif

    columnNum += 1;
#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, BOTTOM of QVALUELIST, infoAboutComparingString.ascii=(%s)\n", infoAboutComparingString.ascii() );
#endif
  } // end major loop through QValueList 
 
  if (max_cid_value != -1) {
    // do some adjustment for compare groups
  }

  experimentGroupList.clear();
// I eventually want a info class per column of cview data...
  for(int i=0;i < splv->columns(); i++ )
  {
    QString header = splv->columnText(i);
    int end_index = header.find(":");
    int commaIndex = header.find(",");
#ifdef DEBUG_StatsPanel
    printf("analyzeTheCView, B: commaIndex=%d\n", commaIndex);
#endif
    if(commaIndex != -1 )
    {
      end_index = commaIndex;
    }
    if( header.startsWith("-c ") && end_index > 0 )
    {
      int start_index = 3;
      QString cviewIDStr = header.mid(start_index, end_index-start_index);
      int cid = cviewIDStr.toInt();
      for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
      {
        CInfoClass *cic = (CInfoClass *)*it;
        if( cic->cid == cid )
        {
          experimentGroupList.push_back( QString("Experiment: %1").arg(cic->expID) );
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::analyzeTheCView, cic->expId=%d, focusedExpID=%d\n", cic->expID, focusedExpID );
#endif
          if( focusedExpID == -1 )
          {
            focusedExpID = cic->expID;
          }
        }
      }
      // Log this into the Column
      for( CInfoClassList::Iterator it = cInfoClassList.begin(); it != cInfoClassList.end(); ++it)
      {
        CInfoClass *cic = (CInfoClass *)*it;
        if( cic->cid == cid )
        {
          columnValueClass[i].cic = cic;
#ifdef DEBUG_StatsPanel
          printf("StatsPanel::analyzeTheCView, printing columnValueClass[i]\n" );
          columnValueClass[i].print();
#endif
          break;
        }
      }

    }
  }

  if( experimentGroupList.count() > 0 )
  {
    updateCollectorList();
  }

#ifdef DEBUG_StatsPanel
 printf("Exit StatsPanel::analyzeTheCView, focusedExpID=%d\n", focusedExpID );
#endif
}

bool
StatsPanel::canWeDiff()
{
// This broke with the changes to the headers for the report.
// A new approach needs to be done.

return (FALSE);
#if 0
// printf("canWeDiff() entered\n");
  int diffIndex = -1;
  if( splv->columns() < 2 )
  {
// printf("canWeDiff() return FALSE(A)\n");
    return( FALSE );
  }

#ifdef CLUSTERANALYSIS
// printf("canWeDiff:  lastCommand (%s)\n", lastCommand.ascii() );
if( lastCommand.startsWith("cview -c") && lastCommand.contains("-m ") )
{
// printf("lastCommand was (%s) and we're not going to sort!\n", lastCommand.ascii() );
// printf("canWeDiff() return FALSE(B)\n");
  return( FALSE );
}
#endif // CLUSTERANALYSIS

  QString c1header = splv->columnText(0);
  QString c2header = splv->columnText(1);

  if( c1header == "|Difference|" )
  {
// printf("A: return TRUE\n");
    return(TRUE);
  }

// printf("c1header=(%s) c2header=(%s)\n", c1header.ascii(), c2header.ascii() );

  if( c1header == c2header )
  {
// printf("c1header==c2header\n");
// printf("B: return TRUE\n");
    return(TRUE);
  }

  diffIndex = c1header.find(":");
printf("A: diffInde=%d\n", diffIndex);
int commaIndex = c1header.find(",");
printf("B: commaIndex=%d\n", commaIndex);
if(commaIndex != -1 )
{
  diffIndex = commaIndex;
}
  if( diffIndex > 0 )
  {
    c1header = c1header.mid(diffIndex+1);
  }
  diffIndex = c2header.find(":");
printf("B: diffInde=%d\n", diffIndex);
commaIndex = c2header.find(",");
printf("B: commaIndex=%d\n", commaIndex);
if(commaIndex != -1 )
{
  diffIndex = commaIndex;
}
  if( diffIndex > 0 )
  {
    c2header = c2header.mid(diffIndex+1);
  }
    
printf("B: c1header=(%s) c2header=(%s)\n", c1header.ascii(), c2header.ascii() );
  if( c1header == c2header )
  {
printf("new c1header==c2header\n");
printf("C: return TRUE\n");
    return(TRUE);
  }


// printf("canWeDiff() return FALSE(C)\n");
  return(FALSE);
#endif // 0
}

void
StatsPanel::generateToolBar()
{

if (currentCollectorStr != lastCollectorStr) {

  // Start of the Information Icons
  MoreMetadata_icon = new QPixmap(meta_information_plus_xpm);
  metadataToolButton = new QToolButton(*MoreMetadata_icon, "Show More Experiment Metadata", 
                                        QString::null, this, SLOT( infoEditHeaderMoreButtonSelected()), 
                                        fileTools, "show more experiment metadata");
  QToolTip::add( metadataToolButton, tr( "Push for additional experiment metadata.  This is information relating to\nthe generation of the experiment performance data being shown in the display below." ) );
// should not need this--  metadataToolButton->setIconText(QString("Show More Experiment Metadata"));

  LessMetadata_icon = new QPixmap(meta_information_minus_xpm);
  // End of the Information Icons

  // Start of the Panel Administration Icons
  QPixmap *update_icon = new QPixmap( update_icon_xpm );
  new QToolButton(*update_icon, "Update the statistics panel.  Make viewing option changes\nand then click on this icon to display the new view.", QString::null, this, SLOT( updatePanel()), fileTools, "Update the statistics panel to show updated view");

  QPixmap *clear_auxiliary = new QPixmap( clear_auxiliary_xpm );
  new QToolButton(*clear_auxiliary, "Clear all auxiliary view settings, such as, specific function setting,\ntime segment settings, per event display settings, etc..", QString::null, this, SLOT( clearAuxiliarySelected()), fileTools, "clear auxiliary settings");
  // End of the Panel Administration Icons


  // Start of the View Generation Icons
  QPixmap *functions_icon = new QPixmap( functions_xpm );
  new QToolButton(*functions_icon, "Show Functions: generate a performance statistics report\nshowing the performance data delineated by functions.", QString::null, this, SLOT( functionsSelected()), fileTools, "show functions");

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::generateToolBar, currentCollectorStr.ascii()=%s\n", currentCollectorStr.ascii() );
#endif

  if(  currentCollectorStr != "mpi" && 
       currentCollectorStr != "io" && 
       currentCollectorStr != "iot" && 
       currentCollectorStr != "mpit" ) {

    QPixmap *linkedObjects_icon = new QPixmap( linkedObjects_xpm );
    new QToolButton(*linkedObjects_icon, "Show LinkedObjects: generate a performance statistics report\nshowing the performance data delineated by the linked objects\nthat are involved in the execution of the executable(s).", QString::null, this, SLOT( linkedObjectsSelected()), fileTools, "show linked objects");

    QPixmap *statements_icon = new QPixmap( statements_xpm );
    new QToolButton(*statements_icon, "Show Statements: generate a performance statistics report\nshowing the performance data delineated by the source line\nstatements in your program.", QString::null, this, SLOT( statementsSelected()), fileTools, "show statements");

#ifdef DEBUG_StatsPanel
    printf("StatsPanel::generateToolBar, statements_icon=%d\n", statements_icon );
#endif

    QPixmap *statementsByFunction_icon = new QPixmap( statementsByFunction_xpm );
    new QToolButton(*statementsByFunction_icon, "Show Statements by Function: generate a performance statistics\nreport showing the performance data delineated by the\nsource line statements from a selected function in your program.\nSelect a function by selecting output line in the display below and\nthen clicking this icon.  This report will only show the\nperformance information for the statements in the selected function.", QString::null, this, SLOT( statementsByFunctionSelected()), fileTools, "show statements by function");

  }




  if(  currentCollectorStr != "pcsamp" && currentCollectorStr != "hwc" )
  {
    QPixmap *calltrees_icon = new QPixmap( calltrees_xpm );
    new QToolButton(*calltrees_icon, "Show CallTrees", QString::null, this, SLOT( calltreesSelected()), fileTools, "show calltrees");

    QPixmap *calltreesByFunction_icon = new QPixmap( calltreesByFunction_xpm );
    new QToolButton(*calltreesByFunction_icon, "Show CallTrees by Function", QString::null, this, SLOT( calltreesByFunctionSelected()), fileTools, "show calltrees by function");

    QPixmap *calltreesfull_icon = new QPixmap( calltreesfull_xpm );
    new QToolButton(*calltreesfull_icon, "Show CallTrees,FullStack", QString::null, this, SLOT( calltreesFullStackSelected()), fileTools, "calltrees,fullstack");

    QPixmap *calltreesfullByFunction_icon = new QPixmap( calltreesfullByFunction_xpm );
    new QToolButton(*calltreesfullByFunction_icon, "Show CallTrees,FullStack by Function", QString::null, this, SLOT( calltreesFullStackByFunctionSelected()), fileTools, "calltrees,fullstack by function");

    QPixmap *tracebacks_icon = new QPixmap( tracebacks_xpm );
    new QToolButton(*tracebacks_icon, "Show TraceBacks", QString::null, this, SLOT( tracebacksSelected()), fileTools, "show tracebacks");

    QPixmap *tracebacksByFunction_icon = new QPixmap( tracebacksByFunction_xpm );
    new QToolButton(*tracebacksByFunction_icon, "Show TraceBacks by Function", QString::null, this, SLOT( tracebacksByFunctionSelected()), fileTools, "show tracebacks by function");

    QPixmap *tracebacksfull_icon = new QPixmap( tracebacksfull_xpm );
    new QToolButton(*tracebacksfull_icon, "Show TraceBacks,FullStack", QString::null, this, SLOT( tracebacksFullStackSelected()), fileTools, "show tracebacks,fullstack");

    QPixmap *tracebacksfullByFunction_icon = new QPixmap( tracebacksfullByFunction_xpm );
    new QToolButton(*tracebacksfullByFunction_icon, "Show TraceBacks,FullStack by Function", QString::null, this, SLOT( tracebacksFullStackByFunctionSelected()), fileTools, "show tracebacks,fullstack by function");

    QPixmap *butterfly_icon = new QPixmap( butterfly_xpm );
    new QToolButton(*butterfly_icon, "Show Butterfly", QString::null, this, SLOT( butterflySelected()), fileTools, "show butterfly");
  }
  // End of the View Generatin Icons

  // Start of the Analysis Icons
  if( currentCollectorStr == "iot" || currentCollectorStr == "mpit" ) {
    QPixmap *event_list_icon = new QPixmap( event_list_icon_xpm );
    new QToolButton(*event_list_icon, "Show a per event list display.  There will be one event (call a function that was specified to be traced) per line.", QString::null, this, SLOT( showEventListSelected()), fileTools, "Show per event display");
  }

//#ifdef MIN_MAX_ENABLED
  QPixmap *load_balance_icon = new QPixmap( load_balance_icon_xpm );
  new QToolButton(*load_balance_icon, "Show minimum, maximum, and average statistics across ranks, threads,\nprocesses: generate a performance statistics report for these metric values, \ncreating comparison columns for each value.", QString::null, this, SLOT( minMaxAverageSelected()), fileTools, "Show min, max, average statistics across ranks, threads, processes.");
//#endif

  QPixmap *compare_and_analyze_icon = new QPixmap( compare_and_analyze_xpm );
  new QToolButton(*compare_and_analyze_icon, "Show Comparison and Analysis across ranks, threads,\nprocesses: generate a performance statistics report as the result\nof a cluster analysis algorithm to group ranks, threads or processes\nthat have similar performance statistics.", QString::null, this, SLOT( clusterAnalysisSelected()), fileTools, "show comparison analysis");

  QPixmap *custom_comparison_icon = new QPixmap( custom_comparison_xpm );
  new QToolButton(*custom_comparison_icon, "Show Custom Comparison report as built by user input: generate\na performance statistics report as the result of the user\ncreating comparison columns and then selecting which\nexperiments, ranks, threads, or processes\nwill comprise each column.", QString::null, this, SLOT( customizeExperimentsSelected()), fileTools, "show comparison analysis");

  toolbar_status_label = new QLabel(fileTools,"toolbar_status_label");
  // default setting to match default views
  toolbar_status_label->setText("Showing Functions Report:");
  fileTools->setStretchableWidget(toolbar_status_label);

//   fileTools->show();

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::generateToolBar, toolbar_status_label=%d, also call fileTools->hide()\n", toolbar_status_label );
#endif

    fileTools->hide();
} 

lastCollectorStr = currentCollectorStr;

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::generateToolBar, EXIT, lastCollectorStr.ascii()=%s\n", lastCollectorStr.ascii() );
#endif
}

void
StatsPanel::functionsSelected()
{
#ifdef DEBUG_StatsPanel
 printf("functionsSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "Functions";

  toolbar_status_label->setText("Generating Functions Report...");

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::functionsSelected(), calling updateStatsPanelData\n" );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Functions Report:");
}

void
StatsPanel::linkedObjectsSelected()
{
#ifdef DEBUG_StatsPanel
 printf("linkedObjectsSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "LinkedObjects";

  toolbar_status_label->setText("Generating Linked Objects Report...");

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::linkedObjectsSelected(), calling updateStatsPanelData\n" );
#endif
  toolbar_status_label->setText("Showing Linked Objects Report:");
  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Linked Objects Report:");
}


void
StatsPanel::statementsSelected()
{
#ifdef DEBUG_StatsPanel
 printf("StatsPanel::statementsSelected()\n");
 printf("StatsPanel::currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "Statements";

  toolbar_status_label->setText("Generating Statements Report...");

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::statementsSelected(), calling updateStatsPanelData\n" );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Statements Report:");
}

void
StatsPanel::statementsByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
 printf("Enter StatsPanel::statementsByFunctionSelected(), currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "Statements by Function";

  toolbar_status_label->setText("Generating Statements by Function Report...");

#ifdef DEBUG_StatsPanel
 printf("StatsPanel::statementsByFunctionSelected, calling updateStatsPanelData, currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", 
         currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Statements by Function Report:");
}

void
StatsPanel::calltreesSelected()
{
#ifdef DEBUG_StatsPanel
 printf("calltreesSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "CallTrees";

  toolbar_status_label->setText("Generating CallTrees Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees Report:");
}

void
StatsPanel::calltreesByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "CallTrees by Function";

  toolbar_status_label->setText("Generating CallTrees by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees by Function Report:");
}

void
StatsPanel::calltreesFullStackSelected()
{
#ifdef DEBUG_StatsPanel
 printf("calltreesFullStackSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "CallTrees,FullStack";

  toolbar_status_label->setText("Generating CallTrees,FullStack Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees,FullStack Report:");
}

void
StatsPanel::calltreesFullStackByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
  printf("calltreesFullStackByFunctionSelected()\n");
  printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "CallTrees,FullStack by Function";

  toolbar_status_label->setText("Generating CallTrees,FullStack by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing CallTrees,FullStack by Function Report:");
}

void
StatsPanel::tracebacksSelected()
{
#ifdef DEBUG_StatsPanel
 printf("tracebacksSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "TraceBacks";

  toolbar_status_label->setText("Generating TraceBacks Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks Report:");
}

void
StatsPanel::tracebacksByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
 printf("tracebacksByFunctionSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif

  currentUserSelectedReportStr = "TraceBacks by Function";

  toolbar_status_label->setText("Generating TraceBacks by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks by Function Report:");
}

void
StatsPanel::tracebacksFullStackSelected()
{
#ifdef DEBUG_StatsPanel
 printf("tracebacksFullStackSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  currentUserSelectedReportStr = "TraceBacks,FullStack";

  toolbar_status_label->setText("Generating TraceBacks,FullStack Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks,FullStack Report:");
}

void
StatsPanel::tracebacksFullStackByFunctionSelected()
{
#ifdef DEBUG_StatsPanel
 printf("tracebacksFullStackByFunctionSelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  currentUserSelectedReportStr = "TraceBacks,FullStack by Function";

  toolbar_status_label->setText("Generating TraceBacks,FullStack by Function Report...");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing TraceBacks,FullStack by Function Report:");
}

void
StatsPanel::butterflySelected()
{
#ifdef DEBUG_StatsPanel
 printf("butterflySelected()\n");
 printf("  currentCollectorStr=(%s) currentUserSelectedReportStr(%s)\n", currentCollectorStr.ascii(), currentUserSelectedReportStr.ascii() );
#endif
  currentUserSelectedReportStr = "Butterfly";

  toolbar_status_label->setText("Generating Butterfly Report:");

  updateStatsPanelData(DONT_FORCE_UPDATE);

  toolbar_status_label->setText("Showing Butterfly Report:");
}
