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
  

#include "CompareClass.hxx"

#include "debug.hxx"

#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
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

#include <qtoolbox.h>

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusObject.hxx"
#include "UpdateObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"

CompareClass::CompareClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QWidget( parent, name )
{
//  nprintf(DEBUG_CONST_DESTRUCT) ("CompareClass::CompareClass() constructor called.\n");
  dprintf("CompareClass::CompareClass() constructor called.\n");
  ccnt = 0;
tcnt = 0;
  
  p = _p;
  mw = (OpenSpeedshop *)p->getPanelContainer()->getMainWindow();
  cli = p->getPanelContainer()->getMainWindow()->cli;
  expID = exp_id;
  clo = NULL;
  ce = NULL;

  if ( !name ) setName( "CompareClass" );

  QVBoxLayout *mainCompareLayout = new QVBoxLayout( this, 1, 1, "mainCompareLayout");
  QLabel *header = new QLabel(this, "header");
  header->setText("Compare Processes Factory:");
  header->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  QToolTip::add(header, tr("The Compare Processes Factor is the interface that allows you to compare one set\nof process/thread to another -or- one experiment run against another.\n\nIt allows you manage columns of data that will be displayed in the StatsPanel.\nEach cset defined can have mulitiple columns defined.   Each coloumn can have an\nindividual collector/metric/modifier displayed.\n\nFirst you create the CompareSet (cset) definition, using this panel, then select\n\"Focus on this CSET\" and the StatsPanel (and eventually the SourcePanel) will be\nupdated with the selected statistics.") );
  mainCompareLayout->addWidget(header);

  CompareClassLayout = new QHBoxLayout( mainCompareLayout, 1, "CompareClassLayout"); 

// mainCompareLayout->addLayout(CompareClassLayout);

  splitter = new QSplitter(this, "splitter");
  splitter->setOrientation(QSplitter::Horizontal);

  CompareClassLayout->addWidget( splitter );

  // Vertical list of compare sets (set of psets) defined by the user.
  // this simple defaults to "All process/threads, as if this pane never
  // existed.
  csetTB = new QToolBox( splitter, "listOfCompareSets");

  compareList.clear();

  gatherInfo();

  addNewCSet();


  languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareClass::~CompareClass()
{
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CompareClass::languageChange()
{
  setCaption( tr( "Compare Class" ) );
}

bool
CompareClass::menu(QPopupMenu* contextMenu)
{
  QAction *qaction = NULL;

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "focusOnCSET");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Focus on CSet...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnCSet() ) );
  qaction->setStatusTip( tr("Focus the StatsPanel on compare set (cset) in focus.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "addNewCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add new cset (Compare Set)...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewCSet() ) );
  qaction->setStatusTip( tr("Creates a new cset (Compare Set) to be modified.") );

  qaction = new QAction( this,  "removeCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Remove cset...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeCSet() ) );
  qaction->setStatusTip( tr("Removes this currently raised cset.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "addNewColumn");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add new column to current compare set...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewColumn() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current cset.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Remove raised column from current compare set...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes column (tab) from the current cset.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "removeUserPSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Remove process/process set..") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeUserPSet() ) );
  qaction->setStatusTip( tr("Removes process/process set from the current cset.") );

  qaction = new QAction( this,  "focusOnProcess");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Focus on Process(es) in compare set...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnCSetSelected() ) );
  qaction->setStatusTip( tr("Focus on processes in current cset.") );

  qaction = new QAction( this,  "addProcessesSelected");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add process(es)...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addProcessesSelected() ) );
  qaction->setStatusTip( tr("Add processes to the current column of current cset.") );

  return( TRUE );
}

void
CompareClass::addNewCSet()
{
  ccnt++;

  QTabWidget *tabWidget = new QTabWidget(this, "tabWidget");

  addNewColumn(tabWidget);

  csetTB->addItem(tabWidget, QString("cset%1").arg(ccnt) );

}

// Entry point from the menu.  You'll need to lookup the tabWidget
void
CompareClass::addNewColumn()
{
  QWidget *currentItem = csetTB->currentItem();

  addNewColumn( (QTabWidget *)currentItem );
}

void
CompareClass::addNewColumn(QTabWidget *tabWidget)
{

  QFrame *frame = new QFrame(tabWidget, QString("frame%1-%2").arg(tcnt).arg(ccnt) );
  QVBoxLayout *TBlayout = new QVBoxLayout( frame, 1, 1, QString("TBlayout%1-%2").arg(tcnt).arg(ccnt) );


{
QComboBox *cb = NULL;
// if( experiment_list.size() > 1 )
{
  QHBoxLayout *experimentLayout = new QHBoxLayout( TBlayout, 1, "experimentLayout" );

  QLabel *cbl = new QLabel(frame, "experimentComboBoxLabel");
  cbl->setText( tr("Available Experiments:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, tr("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(cbl);

  cb = new QComboBox(FALSE, frame, "experimentComboBox");
  cb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  QToolTip::add(cb, tr("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(cb);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
  experimentLayout->addItem(spacer);
}

for( std::vector<pair_def>::const_iterator it = experiment_list.begin();         it != experiment_list.end(); it++ )
{
// printf("it->first=%d it->second.c_str()=(%s)\n", it->first, it->second.c_str() );

  QString str1 = QString("%1").arg(it->first);
  QString str2 = QString("%1").arg(it->second.c_str());
  if( cb )
  {
    cb->insertItem( "Exp:"+str1+" "+str2 );
  }
// printf("str1.ascii()=(%s) str2.ascii()=(%s)\n", str1.ascii(), str2.ascii() );
}
}

{
QComboBox *cb = NULL;
// if( clo->collectorEntryList.size() > 1 )
{
  QHBoxLayout *collectorLayout = new QHBoxLayout( TBlayout, 1, "collectorLayout" );

  QLabel *cbl = new QLabel(frame, "collectorComboBoxLabel");
  cbl->setText( tr("Available Collectors:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, tr("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(cbl);

  cb = new QComboBox(FALSE, frame, "collectorComboBox");
  cb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  QToolTip::add(cb, tr("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(cb);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
  collectorLayout->addItem(spacer);
}

  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
if( cb )
{
    cb->insertItem( ce->name );
// printf("Put this to menu: ce->name=%s ce->short_name\n", ce->name.ascii(), ce->short_name.ascii() );
}
  }
}
  
{

if( ce )
{
  QHBoxLayout *metricLayout = new QHBoxLayout( TBlayout, 1, "metricLayout" );

  QLabel *cbl = new QLabel(frame, "metricComboBoxLabel");
  cbl->setText( tr("Available Metrics/Modifers:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, tr("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(cbl);

  QComboBox *cb = new QComboBox(FALSE, frame, "metricComboBox");
  QToolTip::add(cb, tr("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(cb);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
  metricLayout->addItem(spacer);

  CollectorMetricEntryList::Iterator plit;
  for( plit = ce->metricList.begin();
       plit != ce->metricList.end(); ++plit )
  {
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
// printf("Put this to a menu: cpe->name=(%s) cpe->type=(%s) cpe->metric_val=(%s)\n", cpe->name.ascii(), cpe->type.ascii(), cpe->metric_val.ascii() );
    cb->insertItem( cpe->name );
  }
  cb->insertItem("min");
  cb->insertItem("max");
  cb->insertItem("average");
  cb->insertItem("count");
  cb->insertItem("percent");
  cb->insertItem("stddev");
}
}
  
#ifdef OLDWAY
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Modifiers for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Select which modifiers (from the list) that you want\nto use in the comparison for this column.") );
  TBlayout->addWidget(l);
  
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Sort Key (i.e. functions) for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Well, this really won't be a selection ... I don't think.") );
  TBlayout->addWidget(l);
#endif // OLDWAY
  
  MPListView *lv = new MPListView( frame, CPS, 0  );
  lv->addColumn("Processes/PSets:");
  QToolTip::add(lv->header(), "Process/Process Sets (psets) to be display in this column:");
  lv->setAllColumnsShowFocus( TRUE );
  lv->setShowSortIndicator( TRUE );
  lv->setRootIsDecorated(TRUE);
  lv->setSelectionMode( QListView::Single );
  MPListViewItem *dynamic_items = new MPListViewItem( lv, CPS);

  QToolTip::add(lv->viewport(), tr("Drag and drop, psets or individual processes from the\nManageProcessesPanel.  In the StatsPanel, the statistics from\nthese grouped processes will be displayed in\ncolumns relative to this display.") );
  TBlayout->addWidget(lv);
  
  // ??  compareList.push_back(lv);
  
  tabWidget->addTab( frame, QString("Column #%1").arg(tcnt)  );
  int count = tabWidget->count();
// printf("count=%d\n", count);
  QWidget *cp = tabWidget->page(count-1);
  cp->show();
  tabWidget->showPage(cp);
  tabWidget->setTabToolTip(cp, "The information in this column will be displayed in\nthe StatsPanel's associated column.  i.e. Column #1\nwill be in the first column of the StatsPanel\nColumn #2 the second.  Column #3 the third...");

tcnt++;
}

void
CompareClass::focusOnCSetSelected()
{
printf("CompareClass::focusOnCSetSelected() entered\n");
printf("Currently unimplemented\n");
}

void
CompareClass::addProcessesSelected()
{
printf("CompareClass::addProcessesSelected() entered\n");
printf("Currently unimplemented\n");
}

void
CompareClass::removeCSet()
{
  QWidget *currentItem = csetTB->currentItem();

printf("Don't forget to destroy the allocated cset\n");

  csetTB->removeItem( currentItem );
}

void
CompareClass::removeRaisedTab()
{
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();


  QWidget *currentTab = currentTabWidget->currentPage();


  currentTabWidget->removePage(currentTab);
  
}

void
CompareClass::removeUserPSet()
{
printf("removeUserPSet() hmmmm\n");
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  QWidget *currentTab = currentTabWidget->currentPage();

printf("Hmmmm.\n");
}

void
CompareClass::focusOnCSet()
{
printf("focusOnCSet() entered.   Now all you need to do is implement it.\n");
}


#include "CollectorListObject.hxx"
#include "CollectorEntryClass.hxx"
#include "CollectorMetricEntryClass.hxx"
void
CompareClass::gatherInfo(QString collectorName)
{
// printf("gatherInfo() entered\n");
{ // Begin gather experiment info

  std::list<int64_t> int_list;
  int_list.clear();
  experiment_list.clear();

  QString command = QString("listExp");
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &int_list, clip, TRUE ) )
  {
//    printf("Unable to run %s command.\n", command.ascii() );
    QMessageBox::information(this, QString(tr("Initialization warning:")), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );
  }

  std::list<int64_t>::iterator it;
// printf("int_list.size() =%d\n", int_list.size() );
  for(it = int_list.begin(); it != int_list.end(); it++ )
  {
    int64_t expID = (int64_t)(*it);
// printf("expID=(%d)\n", expID );


    QString expStr = QString("%1").arg(expID);

    command = QString("listTypes -x %1").arg(expStr);
    std::list<std::string> list_of_collectors;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
// printf("Unable to run %s command.\n", command.ascii() );
      
      QMessageBox::information(this, QString(tr("Initialization warning:")), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );

      return;
    }
  
    int knownCollectorType = FALSE;
    QString panel_type = "other";
    if( list_of_collectors.size() > 0 )
    {
      for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
      {
//      std::string collector_name = *it;
        QString collector_name = (QString)*it;
// printf("  collector_name=(%s)\n", collector_name.ascii() );

const char *str = collector_name.ascii();
experiment_list.push_back( std::make_pair(expID, str ) );
      }
    }
  }
} // End gather experiment info

  if( clo )
  {
    delete(clo);
  }
  
  clo = new CollectorListObject(expID);
// printf("expID=%d\n", expID);

  ce = NULL;
  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
// printf("ce->name=%s ce->short_name\n", ce->name.ascii(), ce->short_name.ascii() );
    if( ce->name == collectorName )
    {
      CollectorMetricEntryList::Iterator plit;
      for( plit = ce->metricList.begin();
           plit != ce->metricList.end(); ++plit )
      {
        CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
// printf("cpe->name=(%s) cpe->type=(%s) cpe->metric_val=(%s)\n", cpe->name.ascii(), cpe->type.ascii(), cpe->metric_val.ascii() );
      }
      break;
    } else
    {
      ce = NULL;
    }
  }
}
