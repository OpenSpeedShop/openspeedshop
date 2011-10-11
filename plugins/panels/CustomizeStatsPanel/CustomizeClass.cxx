////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
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
// Debug Flag
//#define DEBUG_CC 1
//

#include "CustomizeClass.hxx"

#include "debug.hxx"

#include <qapplication.h>
#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <MPListViewItem.hxx>
#include <qinputdialog.h>
#include <qmenubar.h>
#include <qvaluelist.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qheader.h>
#include <qtooltip.h>
#include <qtoolbar.h>

#include <qtoolbox.h>

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusCompareObject.hxx"
#include "UpdateObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"
#include "CompareProcessesDialog.hxx"


CustomizeClass::CustomizeClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id, QString focusedExpIDStr )
    : QWidget( parent, name )
{
#ifdef DEBUG_CC 
//  nprintf(DEBUG_CONST_DESTRUCT) ("CustomizeClass::CustomizeClass() constructor called.\n");
  printf("CustomizeClass::CustomizeClass() constructor called., exp_id=%d, focusedExpIDStr.ascii()=%s\n", exp_id, focusedExpIDStr.ascii());
#endif
  dprintf("CustomizeClass::CustomizeClass() constructor called.\n");
//  ccnt = 0;
  ccnt = 1;
  
  p = _p;
  if( focusedExpIDStr.isEmpty() ) {
    focusedExpID = -1;
  } else {
    focusedExpID = focusedExpIDStr.toInt();
  }

  expID = exp_id;

  csl = NULL;
  dialog = NULL;
  currentCompareByType = compareByFunctionType; // default compare type is by function
  currentCompareTypeStr = "functions"; // default current compare by string

  if ( !name ) setName( "CustomizeClass" );

  QVBoxLayout *mainCompareLayout = new QVBoxLayout( this, 1, 1, "mainCompareLayout");

#if 0
  QLabel *header = new QLabel(this, "header");
  header->setText("Customize StatsPanel Factory:");
  header->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

  QToolTip::add(header, tr("The Customize StatsPanel Factory is the interface that allows you to compare one set\nof process/thread to another -or- one experiment run against another.\n\nIt allows you manage columns of data that will be displayed in the StatsPanel.\nEach cset defined can have mulitiple columns defined.   Each coloumn can have an\nindividual collector/metric/modifier displayed.\n\nFirst you create the CompareSet (cset) definition, using this panel, then select\n\"Focus on this CSET\" and the StatsPanel (and eventually the SourcePanel) will be\nupdated with the selected statistics.") );
  mainCompareLayout->addWidget(header);

// I'm not sure this header makes much sense here.   Hide for now.
//jeg  header->hide();

#endif


#if 1

// Add toolbar here
//  fileTools = new QToolBar(QString("label"), NULL, NULL , "file operations" );
  fileTools = new QToolBar(QString("label"), NULL, this , "file operations" );
//  fileTools = new QToolBar(QString("label"), p->getPanelContainer()->getMainWindow(), (QWidget *)p->getBaseWidgetFrame(), "file operations" );
  fileTools->setOrientation( Qt::Horizontal );
  fileTools->setLabel( "File Operations" );
  fileTools->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, fileTools->sizePolicy().hasHeightForWidth() ) );

  mainCompareLayout->addWidget(fileTools);

#ifdef DEBUG_StatsPanel
  printf("CustomClass::CustomClass:: fileTools created as QToolBar\n");
#endif

  QPixmap *customize_update_icon = new QPixmap( customize_update_icon_xpm );
  new QToolButton(*customize_update_icon, "Update the customize stats panel.", QString::null, this, SLOT( updatePanel() ), fileTools, "Update the custimized statistics panel");

  QPixmap *add_processes_icon = new QPixmap( add_processes_icon_xpm );
  new QToolButton(*add_processes_icon, "Add new processes to the compare process set.", QString::null, this, SLOT( addProcessesSelected() ), fileTools, "Add new processes to the compare process set.");

  QPixmap *remove_processes_icon = new QPixmap( remove_processes_icon_xpm );
  new QToolButton(*remove_processes_icon, "Remove new processes to the compare process set.", QString::null, this, SLOT( removeUserPSet() ), fileTools, "Remove new processes to the compare process set.");

  QPixmap *add_column_icon = new QPixmap( add_column_icon_xpm );
  new QToolButton(*add_column_icon, "Add new column to the compare set.", QString::null, this, SLOT( addNewColumn() ), fileTools, "Add a new column to the compare set.");

  QPixmap *remove_column_icon = new QPixmap( remove_column_icon_xpm );
  new QToolButton(*remove_column_icon, "Remove the raised tab column from the compare set.", QString::null, this, SLOT( removeRaisedTab() ), fileTools, "Remove the raised tab column.");

  QPixmap *load_experiment_icon = new QPixmap( load_experiment_xpm );
  new QToolButton(*load_experiment_icon, "Load another experiment into the customize stats panel.", QString::null, this, SLOT( loadAdditionalExperimentSelected() ), fileTools, "Load another experiment into the customize stats panel.");

  QPixmap *focus_stats_icon = new QPixmap( focus_stats_icon_xpm );
  new QToolButton(*focus_stats_icon, "Focus the StatsPanel on this information.  Generate/Display the view from the currently requested settings.", QString::null, this, SLOT( focusOnCSetSelected() ), fileTools, "Generate the StatsPanel view from the current settings.");



  compareClassLayout = new QVBoxLayout( mainCompareLayout, 1, "compareClassLayout"); 

  // Vertical list of compare sets (set of psets) defined by the user.
  // this simple defaults to "All process/threads, as if this pane never
  // existed.
  csetTB = new QToolBox( this, "listOfCompareSets");

  compareClassLayout->addWidget( csetTB );

#ifdef DEBUG_CC 
  printf("CustomizeClass() constructor calling updateInfo\n");
#endif

  updateInfo();

  compareList.clear();

#ifdef DEBUG_CC 
  printf("CustomizeClass() constructor calling addNewCSet\n");
#endif

  addNewCSet();

  toolbar_status_label = new QLabel(fileTools,"toolbar_status_label");

#if 1

  // Create a compare type button group
  QButtonGroup *vCompareTypeBG = new QButtonGroup( 1, QGroupBox::Vertical, "Compare/Display Choice", fileTools);
  vCompareTypeBG->setExclusive( TRUE );

  // insert 1 or 3 radiobuttons
  vCompareTypeFunctionRB = new QRadioButton( "Functions", vCompareTypeBG );
  connect( vCompareTypeFunctionRB, SIGNAL( clicked() ), this, SLOT( compareByFunction() ) );

  bool full_compare_by_menu = TRUE;
  if (csetTB) {
    QString collectorName = getCollectorName();
    if (collectorName.contains("mpi") || collectorName.contains("io")) {
      full_compare_by_menu = FALSE;
    }
  } else {
    full_compare_by_menu = FALSE;
  } 

  if (full_compare_by_menu) {
    vCompareTypeStatementRB = new QRadioButton( "Statements", vCompareTypeBG );
    connect( vCompareTypeStatementRB, SIGNAL( clicked() ), this, SLOT( compareByStatement() ) );
    vCompareTypeLinkedObjectRB = new QRadioButton( "Linked Objects", vCompareTypeBG );
    connect( vCompareTypeLinkedObjectRB, SIGNAL( clicked() ), this, SLOT( compareByLinkedObject() ) );

    if (currentCompareByType == compareByFunctionType) {
       vCompareTypeFunctionRB->setChecked(TRUE);
       vCompareTypeStatementRB->setChecked(FALSE);
       vCompareTypeLinkedObjectRB->setChecked(FALSE);
    } else if (currentCompareByType == compareByStatementType) {
       vCompareTypeFunctionRB->setChecked(FALSE);
       vCompareTypeStatementRB->setChecked(TRUE);
       vCompareTypeLinkedObjectRB->setChecked(FALSE);
    } else {
       vCompareTypeFunctionRB->setChecked(FALSE);
       vCompareTypeStatementRB->setChecked(FALSE);
       vCompareTypeLinkedObjectRB->setChecked(TRUE);
    }
  } else {
       vCompareTypeFunctionRB->setChecked(TRUE);
  }

#endif


  // default setting to match default views
  toolbar_status_label->setText("");
  fileTools->setStretchableWidget(toolbar_status_label);

#endif

#ifdef DEBUG_CC 
  printf("CustomizeClass() constructor calling languageChange\n");
#endif

  languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CustomizeClass::~CustomizeClass()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass() destructor called\n");
#endif

  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); )
  {
    CompareSet *cs = (CompareSet *)*it;
#ifdef DEBUG_CC 
    printf("delete cs\n");
#endif
    delete cs;
    ++it;
  }
  csl->clear();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CustomizeClass::languageChange()
{
  setCaption( tr( "Customize StatsPanel Class" ) );
}

void
CustomizeClass::compareByFunction()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass::compareByFunction() entered\n");
#endif
  currentCompareTypeStr = "functions";
  currentCompareByType = compareByFunctionType;
}

void
CustomizeClass::compareByStatement()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass::compareByStatement() entered\n");
#endif
  currentCompareTypeStr = "statements";
  currentCompareByType = compareByStatementType;
}

void
CustomizeClass::compareByLinkedObject()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass::compareByLinkedObject() entered\n");
#endif
  currentCompareTypeStr = "linkedobjects";
  currentCompareByType = compareByLinkedObjectType;
}

bool
CustomizeClass::menu(QPopupMenu* contextMenu)
{

  CompareSet *currentCompareSet = findCurrentCompareSet();
  QString currentCompareSetString = QString::null;
  QString currentColumnString = QString::null;

  if( currentCompareSet ) {

    currentCompareSetString = QString(" (%1)").arg(currentCompareSet->name);
    QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();
    QWidget *currentTab = currentTabWidget->currentPage();
    currentColumnString = QString(" (%1)").arg(currentTabWidget->tabLabel( currentTab ) );

  }


  QAction *qaction = NULL;

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Update...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Update the information in this panel.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "focusOnProcess");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Focus Stats Panel with defined info...")+currentCompareSetString) );
  qaction->setText( tr(QString("Focus StatsPanel (Generate Compare View)")) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnCSetSelected() ) );
  qaction->setStatusTip( tr("Focus the StatsPanel on defined set of information.") );


  contextMenu->insertSeparator();

  bool full_compare_by_menu = TRUE;
  if (csetTB) {
    QString collectorName = getCollectorName();
    if (collectorName.contains("mpi") || collectorName.contains("io")) {
      full_compare_by_menu = FALSE;
    }
  } else {
    full_compare_by_menu = FALSE;
  }

  QPopupMenu *compareByMenu = new QPopupMenu( contextMenu );

#if 1
  qaction = new QAction( this,  "compareByFunction");
  qaction->addTo( compareByMenu );
  qaction->setText( tr("Compare By Function") );
  qaction->setToggleAction(TRUE  );
  connect( qaction, SIGNAL( activated() ), this, SLOT( compareByFunction() ) );
  qaction->setStatusTip( tr("Compare and or display the columns using function level performance information.") );
  if (currentCompareByType == compareByFunctionType || !full_compare_by_menu) {
     qaction->setOn(TRUE);
  } else {
     qaction->setOn(FALSE);
  } 
#else
  compareByMenu->insertItem(tr("Compare/Display By Function"), this, SLOT(compareByFunction()),0,0,-1);
  if (currentCompareByType == compareByFunctionType || !full_compare_by_menu) {
     compareByMenu->setItemChecked(0, TRUE);
  } else {
     compareByMenu->setItemChecked(0, FALSE);
  } 
#endif

  if (full_compare_by_menu) {

#if 1
    qaction = new QAction( this,  "compareByStatement");
    qaction->addTo( compareByMenu );
    qaction->setText( tr("Compare/Display By Statement") );
    qaction->setToggleAction(TRUE  );
    connect( qaction, SIGNAL( activated() ), this, SLOT( compareByStatement() ) );
    qaction->setStatusTip( tr("Compare and or display the columns using statement level performance information.") );
   if (currentCompareByType == compareByStatementType) {
       qaction->setOn(TRUE);
    } else {
       qaction->setOn(FALSE);
    } 
#else
    compareByMenu->insertItem(tr("Compare/Display By Statement"), this, SLOT(compareByStatement()),0,1,-1);
    if (currentCompareByType == compareByStatementType) {
       compareByMenu->setItemChecked(1, TRUE);
    } else {
       compareByMenu->setItemChecked(1, FALSE);
    } 
#endif

#if 1
   qaction = new QAction( this,  "compareByLinkedObject");
   qaction->addTo( compareByMenu );
   qaction->setText( tr("Compare/Display By Linked Object") );
   qaction->setToggleAction(TRUE  );
   connect( qaction, SIGNAL( activated() ), this, SLOT( compareByLinkedObject() ) );
   qaction->setStatusTip( tr("Compare and/or display the columns using linked object level performance information.") );
   if (currentCompareByType == compareByLinkedObjectType) {
       qaction->setOn(TRUE);
    } else {
       qaction->setOn(FALSE);
    } 
#else
    compareByMenu->insertItem(tr("Compare/Display By Linked Object"), this, SLOT(compareByLinkedObject()),0,2,-1);
    if (currentCompareByType == compareByLinkedObjectType) {
       compareByMenu->setItemChecked(2, TRUE);
    } else {
       compareByMenu->setItemChecked(2, FALSE);
    } 
#endif
  } // end full_compare_by_menu

  if (full_compare_by_menu) {
     // hide button group 
//     (QWidget *)p->getBaseWidgetFrame()->vCompareTypeBG->hide();
  } else {
     // show button group 
//     parent.vCompareTypeBG->show();
  }

  contextMenu->insertItem("Compare/Display By... ", compareByMenu);
  //compareByMenu->setStatusTip( tr("Compare or Display Performance Information at the Function, Statement, or Linked Object level.") );

#ifdef NONWALKING
  qaction = new QAction( this,  "addNewColumn");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Add column to Compare Set...")+currentCompareSetString) );
  qaction->setText( tr(QString("Add column") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewColumn() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current set.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Remove%1 from Compare Set...").arg(currentColumnString)) );
  qaction->setText( tr(QString("Remove column %1").arg(currentColumnString)) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes the raised column (tab).") );
#else // NONWALKING

  QPopupMenu *addDeleteMenu = new QPopupMenu(this);
  contextMenu->setCaption("Add/Remove Columns");
  contextMenu->insertItem("Add/Remove Columns", addDeleteMenu);

  qaction = new QAction( this,  "addNewColumn");
  qaction->addTo( addDeleteMenu );
//  qaction->setText( tr(QString("Add column to Compare Set...")+currentCompareSetString) );
  qaction->setText( tr(QString("Add column") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewColumn() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current set.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( addDeleteMenu );
//  qaction->setText( tr(QString("Remove%1 from Compare Set...").arg(currentColumnString)) );
  qaction->setText( tr(QString("Remove column %1").arg(currentColumnString)) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes the raised column (tab).") );
#endif // NONWALKING

  contextMenu->insertSeparator();


  qaction = new QAction( this,  "addProcessesSelected");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Select process(es) for: ")+currentCompareSetString+currentColumnString) );
  qaction->setText( tr(QString("Add Process(es)...") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addProcessesSelected() ) );
//  qaction->setStatusTip( tr("Select processes to be added or removed from the current column of current cset.") );
  qaction->setStatusTip( tr("Add -and/or- remove processes from the set.") );

  qaction = new QAction( this,  "removeUserPSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Remove process(es)") ) );
//  qaction->setText( tr(QString("Remove process/process set..")+currentColumnString) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeUserPSet() ) );
  qaction->setStatusTip( tr("Removes process(es) from the set.") );

  contextMenu->insertSeparator();

#ifdef DONT_PULL
  qaction = new QAction( this,  "addNewCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add Compare Set") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewCSet() ) );
  qaction->setStatusTip( tr("Creates a new Compare Set  to be modified.") );

  qaction = new QAction( this,  "removeCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Remove Compare Set")+currentCompareSetString) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeCSet() ) );
  qaction->setStatusTip( tr("Removes this currently raised Compare Set.") );

  contextMenu->insertSeparator();
#endif // DONT_PULL

  qaction = new QAction( this,  "loadAdditionalExperiment");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Load another (additional) experiment's data.")  ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadAdditionalExperimentSelected() ) );
  qaction->setStatusTip( tr("Loads another experiment's data.") );


  return( TRUE );
}

void
CustomizeClass::addNewCSet()
{
  CompareSet *cset = new CompareSet( csetTB, this);
  
  if( !csl )
  {
    csl = new CompareSetList();
  }
  
  csl->push_back(cset);
}

// Entry point from the menu.  You'll need to lookup the tabWidget
void
CustomizeClass::addNewColumn()
{
  CompareSet *compareSet = findCurrentCompareSet();

  addNewColumn(compareSet);
} 

void
CustomizeClass::addNewColumn(CompareSet *compareSet)
{
  ColumnSet *columnSet = new ColumnSet(this, compareSet);

  compareSet->columnSetList.push_back( columnSet );

  compareSet->relabel();

  compareSet->setNewFocus();
}

void
CustomizeClass::updatePanel()
{
  updateInfo();
}

QString
CustomizeClass::getCollectorName()
{
  CompareSet *compareSet = findCurrentCompareSet();
  if( compareSet ) {

#ifdef DEBUG_CC
    printf("CustomizeClass::getCollectorName, CompareSet: (%s)'s info\n", compareSet->name.ascii() );
#endif

    ColumnSetList::Iterator it;
    it = compareSet->columnSetList.begin();
    if ( it != compareSet->columnSetList.end() ) {
      ColumnSet *columnSet = (ColumnSet *)*it;

#ifdef DEBUG_CC
      printf("CustomizeClass::getCollectorName, \t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
      printf("CustomizeClass::getCollectorName, \t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
      printf("CustomizeClass::getCollectorName, \t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
      printf("CustomizeClass::getCollectorName, \t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
#endif // 0

      {
       QString collectorName = columnSet->collectorComboBox->currentText().ascii();
       QString metricName = columnSet->metricComboBox->currentText().ascii();
#ifdef DEBUG_CC
       printf("CustomizeClass::getCollectorName, collectorName=(%s)\n", collectorName.ascii() );
#endif
       return collectorName;
      }
    } // end if not end()
  } // end if compareSet


}

void
CustomizeClass::focusOnCSetSelected()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass::focusOnCSetSelected() entered\n");
#endif

  QValueList<int64_t> cIntList;

  FocusCompareObject *focus_msg = NULL;
  QString cViewCreateCommand = "cViewCreate ";
  QString temp_expCompareCommand = "cViewCreate ";

  CompareSet *compareSet = findCurrentCompareSet();

  if( compareSet ) {

    cIntList.clear();

#ifdef DEBUG_CC 
    printf("CustomizeClass::focusOnCSetSelected, CompareSet: (%s)'s info\n", compareSet->name.ascii() );
#endif

    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;

#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected, \t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
      printf("CustomizeClass::focusOnCSetSelected, \t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
      printf("CustomizeClass::focusOnCSetSelected, \t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
      printf("CustomizeClass::focusOnCSetSelected, \t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
#endif // 0

      {
       int id = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());
       QString collectorName = columnSet->collectorComboBox->currentText().ascii();
       QString metricName = columnSet->metricComboBox->currentText().ascii();

       cViewCreateCommand += QString("-x %1 %2 -m %3 -v %4").arg(id).arg(collectorName).arg(metricName).arg(currentCompareTypeStr);
 
       if( temp_expCompareCommand.isEmpty() ) {
         temp_expCompareCommand += QString("%2 -m %3 ").arg(collectorName).arg(metricName);
       }

      }


#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected, \t\t: processes:\n");
#endif
      QString expCompareProcessList = QString::null;

// Begin real focus logic
#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected() entered.\n");
#endif

      QString pid_name = QString::null;
      QString pidString = QString::null;
 
      QListViewItemIterator it(columnSet->lv);
      expCompareProcessList = QString::null;
      while( it.current() )
      {
        MPListViewItem *lvi = (MPListViewItem *)it.current();
#ifdef DEBUG_CC 
       printf("CustomizeClass::focusOnCSetSelected, PSetSelection: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
       printf("CustomizeClass::focusOnCSetSelected, lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
       printf("CustomizeClass::focusOnCSetSelected, lvi->text(1) =(%s)\n", lvi->text(1).ascii() );
       if( lvi->descriptionClassObject )
       {
         lvi->descriptionClassObject->Print();
       }
#endif // 1

        if( focus_msg == NULL ) {
          focus_msg = new FocusCompareObject(expID,  NULL, TRUE);
        }

        if( !lvi || !lvi->descriptionClassObject ) {
          ++it;
          continue;
//      QMessageBox::information( this, tr("Focus Error:"), tr("Unable to focus on selection: No description for process(es)."), QMessageBox::Ok );
//      return;
        }

        if( lvi->descriptionClassObject->all ) {

#ifdef DEBUG_CC 
           printf("CustomizeClass::focusOnCSetSelected, Do ALL threads, everywhere.\n");
#endif

        } else if( lvi->descriptionClassObject->root ) {

          // Loop through all the children...

#ifdef DEBUG_CC 
          printf("CustomizeClass::focusOnCSetSelected, Loop through all the children.\n");
#endif

          MPListViewItem *mpChild = (MPListViewItem *)lvi->firstChild();
          while( mpChild )
          {
            QString host_name = mpChild->descriptionClassObject->host_name;
            if( host_name.isEmpty() )
            {
              host_name = "localhost";
            }
            QString pid_name = mpChild->descriptionClassObject->pid_name;
            if( pid_name.isEmpty() )
            {
              mpChild = (MPListViewItem *)mpChild->nextSibling();
              continue;
            }
#ifdef DEBUG_CC 
            printf("CustomizeClass::focusOnCSetSelected, A: push_back a new host:pid entry (%s:%s)\n", host_name.ascii(), pid_name.ascii());
#endif
            QString rid_name = mpChild->descriptionClassObject->rid_name;
            QString tid_name = mpChild->descriptionClassObject->tid_name;
            if( !expCompareProcessList.isEmpty() )
            {
              expCompareProcessList += " ; ";
            }
            if( !rid_name.isEmpty() )
            {
              expCompareProcessList += QString(" -h %1 -r %1 ").arg(host_name).arg(rid_name);
            } else if( !tid_name.isEmpty() )
            {
              expCompareProcessList += QString(" -h %1 -t %1 ").arg(host_name).arg(tid_name);
            } else  // Default to pid.. We should get here if its null.
            {
                        expCompareProcessList += QString(" -h %1 -p %1 ").arg(host_name).arg(pid_name);
            }
            mpChild = (MPListViewItem *)mpChild->nextSibling();
            ++it;
          }
        } else
        {
          QString host_name = lvi->descriptionClassObject->host_name;
          if( host_name.isEmpty() )
          {
            host_name = "localhost";
          }
          QString pid_name = lvi->descriptionClassObject->pid_name;
          if( pid_name.isEmpty() )
          {
            continue;
          }
#ifdef DEBUG_CC 
          printf("CustomizeClass::focusOnCSetSelected, B: push_back a new host::pid entry... (%s:%s)\n", host_name.ascii(), pid_name.ascii() );
#endif
          QString rid_name = lvi->descriptionClassObject->rid_name;
          QString tid_name = lvi->descriptionClassObject->tid_name;

          if( !expCompareProcessList.isEmpty() ) {
            expCompareProcessList += " ; ";
          }

          if( !rid_name.isEmpty() ) {
            expCompareProcessList += QString(" -h %1 -r %1 ").arg(host_name).arg(rid_name);
          } else if( !tid_name.isEmpty() ) {
            expCompareProcessList += QString(" -h %1 -t %1 ").arg(host_name).arg(tid_name);
          } else  {
            // Default to pid.. We should get here if its null.
            expCompareProcessList += QString(" -h %1 -p %1 ").arg(host_name).arg(pid_name);
          }
        } 
        ++it;
      }


      // If nothing was selected, just return.
      if( !focus_msg ) {
        QMessageBox::information( this, tr("Error process selection:"), tr("Unable to focus: No processes selected."), QMessageBox::Ok );
        if( focus_msg ) {
          delete focus_msg;
        }
        return;
      }


      cViewCreateCommand += expCompareProcessList;
      focus_msg->compare_command = cViewCreateCommand;


#ifdef DEBUG_CC 
       printf("CustomizeClass::focusOnCSetSelected, I think you really want this compare command:\n(%s)\n", cViewCreateCommand.ascii() );
#endif

      {  //Begin build the actual compare commands and store the id's from them
         // to send to the StatsPanel
      CLIInterface *cli = p->getPanelContainer()->getMainWindow()->cli;
      int64_t val = 0;
      bool mark_value_for_delete = true;
      QString command = QString(cViewCreateCommand);

#ifdef DEBUG_CC 
       printf("CustomizeClass::focusOnCSetSelected, command=(%s)\n", command.ascii() );
#endif

      if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete   ) ) {
        printf("CustomizeClass::focusOnCSetSelected, Unable to creat cview for %s\n", command.ascii() );
        return;
      }

#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected, pushback %d\n", val);
#endif

      cIntList.push_back(val);
      // Now start over...
      cViewCreateCommand = "cViewCreate ";
      }

    } // end for

    focus_msg->compare_command = QString("cview -c ");
    QValueList<int64_t>::Iterator cit;
    int count = 0;
    for( cit = cIntList.begin(); cit != cIntList.end(); ++cit )
    {
      int64_t cval = (int64_t)*cit;
  
      if( count > 0 ) {
        focus_msg->compare_command += QString(", ");
      }
      focus_msg->compare_command += QString("%1").arg(cval);
      count++;
    }

#ifdef DEBUG_CC 
    printf("CustomizeClass::focusOnCSetSelected, Really send this : command: (%s)\n", focus_msg->compare_command.ascii() );
    printf("CustomizeClass::focusOnCSetSelected, A: focus the StatsPanel...\n");
#endif

    QString name = QString("Stats Panel [%1]").arg(expID);

#ifdef DEBUG_CC 
    printf("CustomizeClass::focusOnCSetSelected, find a stats panel named (%s)\n", name.ascii() );
#endif

    Panel *sp = p->getPanelContainer()->findNamedPanel(p->getPanelContainer()->getMasterPC(), (char *)name.ascii() );

    if( !sp ) {

#ifdef DEBUG_CC 
       printf("CustomizeClass::focusOnCSetSelected, Didn't find a stats panel.... Create one.\n");
#endif

      char *panel_type = (char *) "Stats Panel";
      PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
      ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
      sp = p->getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
      delete ao;

      if( sp != NULL ) {

#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected, Created a stats panel... First update it's data... expID=%d\n", expID);
#endif

      sp->listener( (void *)focus_msg );

      }

    } else {

#ifdef DEBUG_CC 
      printf("CustomizeClass::focusOnCSetSelected, There was a statspanel... send the update message.\n");
#endif

      sp->listener( (void *)focus_msg );

    }

  // End real focus logic
  }

}

void
CustomizeClass::addProcessesSelected()
{

#ifdef DEBUG_CC 
  printf("CustomizeClass::addProcessesSelected() entered\n");
#endif


  QApplication::setOverrideCursor(QCursor::WaitCursor);
  if( dialog == NULL ) {
    dialog = new CompareProcessesDialog(this, "Add/Delete/Describe Compare Processes Dialog");
  }

#ifdef DEBUG_CC 
  printf("CustomizeClass::addProcessesSelected() dialog=%x\n", dialog);
#endif

  QApplication::restoreOverrideCursor();

  CompareSet *compareSet = findCurrentCompareSet();

  ColumnSet *columnSet = NULL;

{

  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  QWidget *currentTab = currentTabWidget->currentPage();

  // Look for the tab in the CompareSet.   The delete it from the compareSet's
  // list.

  if( compareSet ) {

    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) ) {
        columnSet = cs;
        break;
      }
    }

  }

}

  dialog->updateInfo();
  int expid = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());

#ifdef DEBUG_CC 
  printf("CustomizeClass::addProcessesSelected, Focus on expID=%d\n",  expid );
  printf("CustomizeClass::addProcessesSelected, CustomizeClass=%s\n",  name() );
  printf("CustomizeClass::addProcessesSelected, compareSet=%s\n",  compareSet->name.ascii() );
  printf("CustomizeClass::addProcessesSelected, columnSet=(%s)\n", columnSet->name.ascii() );
#endif

  dialog->updateFocus(expid, columnSet->lv);
  compareSet->updatePSetList();

  dialog->show();

  QApplication::restoreOverrideCursor();
}

void
CustomizeClass::loadAdditionalExperimentSelected()
{

#ifdef DEBUG_CC 
  printf("CustomizeClass::loadAdditionalExperimentSelected() entered\n");
#endif

  QString fn = QString::null;
  char *cwd = get_current_dir_name();
  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;Any Files(*.*)", this, "open experiment dialog", "Choose an experiment file to open for comparison");
  free(cwd);
  if( !fn.isEmpty() ) {

#ifdef DEBUG_CC 
    printf("CustomizeClass::loadAdditionalExperimentSelected, fn = %s\n", fn.ascii() );
#endif

//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
    p->getPanelContainer()->getMainWindow()->executableName = QString::null;
    p->getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn, FALSE);
    updateInfo();
  } else {
    fprintf(stderr, "No experiment file name given.\n");
  }
}

void
CustomizeClass::removeCSet()
{
  QWidget *currentItem = csetTB->currentItem();

  CompareSet *cs_to_delete = findCurrentCompareSet();

  // If we removed a CompareSet from the list, it should be free
  // to delete.  Delete it.
  if( cs_to_delete ) {

#ifdef DEBUG_CC 
    printf("CustomizeClass::removeCSet, Don't forget to remove all the tabs!\n");
#endif

    csl->remove( cs_to_delete );
    csetTB->removeItem( currentItem );
    delete cs_to_delete;
  }
}

void
CustomizeClass::removeRaisedTab()
{

#ifdef DEBUG_CC 
  printf("CustomizeClass::removeRaisedTab() entered\n");
#endif

  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  CompareSet *cs_to_search = findCurrentCompareSet();

  QWidget *currentTab = currentTabWidget->currentPage();


  // Always keep one column...
  if( currentTabWidget->count() == 1 ) {

#ifdef DEBUG_CC 
    printf("CustomizeClass::removeRaisedTab, DUDE YOU CAN'T REMOVE ANY.  YOU ONLY HAVE ONE LEFT!\n");
#endif

    return;
  }

#ifdef DEBUG_CC 
  printf("CustomizeClass::removeRaisedTab, remove Tab labeled (%s)\n", currentTabWidget->tabLabel( currentTab ).ascii() );
  printf("CustomizeClass::removeRaisedTab, NOW FIND AND REMOVE THIS TAB FROM THE columnList!\n");
#endif

  // Look for the tab in the CompareSet.   The delete it from the compareSet's
  // list.
  if( cs_to_search )
  {
#ifdef DEBUG_CC 
    printf("CustomizeClass::removeRaisedTab, Don't forget to remove all the tabs!\n");
#endif
    ColumnSetList::Iterator it;
    for( it = cs_to_search->columnSetList.begin(); it != cs_to_search->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) ) {

#ifdef DEBUG_CC 
        printf("CustomizeClass::removeRaisedTab, Okay I'm sure you need to delete this (%s) tab\n", cs->name.ascii() );
#endif
        cs_to_search->columnSetList.remove( cs );
        currentTabWidget->removePage(currentTab);
        delete cs;
        break;
      }
    }
cs_to_search->relabel();

  }


  
}

void
CustomizeClass::removeUserPSet()
{
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();
#ifdef DEBUG_CC 
  printf("CustomizeClass::removeUserPSet()\n");
#endif

  QWidget *currentTab = currentTabWidget->currentPage();

  CompareSet *compareSet = findCurrentCompareSet();

  if( compareSet )
  {
#ifdef DEBUG_CC 
   printf("CustomizeClass::removeUserPSet, compareSet (%s)\n", compareSet->name.ascii() );
#endif
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) )
      {
        ColumnSet *columnSet = cs;
        QListViewItem *item = columnSet->lv->currentItem();
        if( item )
        {
#ifdef DEBUG_CC 
          printf("CustomizeClass::removeUserPSet, \t: Delete: ColumnSet (%s)'s info item=(%s)\n", columnSet->name.ascii(), item->text(0).ascii() );
#endif
          delete item;
        }
#ifdef DEBUG_CC 
        printf("CustomizeClass::removeUserPSet, Now check for first child\n");
#endif
        // If we've deleted everything, simply add back the default entry.
        if( !columnSet->lv->firstChild() )
        {
#ifdef DEBUG_CC 
          printf("CustomizeClass::removeUserPSet, No firstChild...add the default back in.\n");
#endif
          MPListViewItem *cps_item = new MPListViewItem( columnSet->lv, CPS);
          cps_item->setOpen(TRUE);
        }
        break;
      }
    }

  }

}


void
CustomizeClass::updateInfo()
{
#ifdef DEBUG_CC 
  printf("CustomizeClass::updateInfo() entered\n");
#endif
  if( csl )
  {
    CompareSetList::Iterator it;
    for( it = csl->begin(); it != csl->end(); ++it )
    {
      CompareSet *cs = (CompareSet *)*it;
#ifdef DEBUG_CC 
      printf("CustomizeClass::updateInfo, attempt to update (%s)'s info\n", cs->name.ascii() );
#endif
      cs->updateInfo();
    }
  }
}

CompareSet *
CustomizeClass::findCurrentCompareSet()
{
  QWidget *currentItem = csetTB->currentItem();

  int  currentIndex = csetTB->currentIndex();

#ifdef DEBUG_CC 
  printf("CustomizeClass::findCurrentCompareSet, found currentIndex=%d\n", currentIndex);
#endif

  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); ++it )
  {
    CompareSet *cs = (CompareSet *)*it;
    if( cs->name == csetTB->itemLabel(currentIndex) ) {
#ifdef DEBUG_CC 
      printf("CustomizeClass::findCurrentCompareSet, return cs=(%s), currentIndex=%d\n", cs->name.ascii(), currentIndex );
#endif
      return( cs );
    }
  }

 return( NULL );
}

