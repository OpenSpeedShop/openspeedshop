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
  

#include "ManageCompareClass.hxx"

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

ManageCompareClass::ManageCompareClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QWidget( parent, name )
{
//  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCompareClass::ManageCompareClass() constructor called.\n");
  dprintf("ManageCompareClass::ManageCompareClass() constructor called.\n");
  ccnt = 0;
tcnt = 0;
  
  p = _p;
  mw = (OpenSpeedshop *)p->getPanelContainer()->getMainWindow();
  cli = p->getPanelContainer()->getMainWindow()->cli;
  expID = exp_id;

  if ( !name ) setName( "ManageCompareClass" );

  QVBoxLayout *mainCompareLayout = new QVBoxLayout( this, 1, 1, "mainCompareLayout");
  QLabel *header = new QLabel(this, "header");
  header->setText("Compare Processes Factory:");
  header->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  QToolTip::add(header, tr("The Compare Processes Factor is the interface that allows you to compare one set\nof process/thread to another -or- one experiment run against another.\n\nIt allows you manage columns of data that will be displayed in the StatsPanel.\nEach cset defined can have mulitiple columns defined.   Each coloumn can have an\nindividual collector/metric/modifier displayed.\n\nFirst you create the CompareSet (cset) definition, using this panel, then select\n\"Focus on this CSET\" and the StatsPanel (and eventually the SourcePanel) will be\nupdated with the selected statistics.") );
  mainCompareLayout->addWidget(header);

  ManageCompareClassLayout = new QHBoxLayout( mainCompareLayout, 1, "ManageCompareClassLayout"); 

// mainCompareLayout->addLayout(ManageCompareClassLayout);

  splitter = new QSplitter(this, "splitter");
  splitter->setOrientation(QSplitter::Horizontal);

  ManageCompareClassLayout->addWidget( splitter );

  // Vertical list of compare sets (set of psets) defined by the user.
  // this simple defaults to "All process/threads, as if this pane never
  // existed.
  csetTB = new QToolBox( splitter, "listOfCompareSets");

  compareList.clear();

  addNewCSet();


  languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageCompareClass::~ManageCompareClass()
{
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageCompareClass::languageChange()
{
  setCaption( tr( "Manage Processes Panel" ) );
}

bool
ManageCompareClass::menu(QPopupMenu* contextMenu)
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

  qaction = new QAction( this,  "addNewTab");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add new tab to current compare set...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewTab() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current cset.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Remove raised tab from current compare set...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes column (tab) from the current cset.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "removeUserPSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Remove process/process set..") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeUserPSet() ) );
  qaction->setStatusTip( tr("Removes process/process set from the current cset.") );

  return( TRUE );
}

void
ManageCompareClass::addNewCSet()
{
  ccnt++;

  QTabWidget *tabWidget = new QTabWidget(this, "tabWidget");

  addNewTab(tabWidget);

  csetTB->addItem(tabWidget, QString("cset%1").arg(ccnt) );

}

// Entry point from the menu.  You'll need to lookup the tabWidget
void
ManageCompareClass::addNewTab()
{
  QWidget *currentItem = csetTB->currentItem();

  addNewTab( (QTabWidget *)currentItem );
}

void
ManageCompareClass::addNewTab(QTabWidget *tabWidget)
{

  QFrame *frame = new QFrame(tabWidget, QString("frame%1-%2").arg(tcnt).arg(ccnt) );
  QVBoxLayout *TBlayout = new QVBoxLayout( frame, 1, 1, QString("TBlayout%1-%2").arg(tcnt).arg(ccnt) );
  QLabel *l;
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Collector for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Select which collect (from the list) that you want\nto use in the comparison for this column.") );
  TBlayout->addWidget(l);
  
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Metric for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Select which metric (from the list) that you want\nto use in the comparison for this column.") );
  TBlayout->addWidget(l);
  
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Modifiers for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Select which modifiers (from the list) that you want\nto use in the comparison for this column.") );
  TBlayout->addWidget(l);
  
  l = new QLabel( frame, QString("l%1-%2").arg(tcnt).arg(ccnt)  );
  l->setText( QString("Sort Key (i.e. functions) for cset %1-%2").arg(tcnt).arg(ccnt)  );
  QToolTip::add(l, tr("Well, this really won't be a selection ... I don't think.") );
  TBlayout->addWidget(l);
  
//  MPListView *lv = new MPListView( frame, QString("lv%1-%2").arg(tcnt).arg(ccnt), 0  );
  MPListView *lv = new MPListView( frame, CPS, 0  );
  lv->addColumn("lv header");
  lv->setAllColumnsShowFocus( TRUE );
  lv->setShowSortIndicator( TRUE );
  lv->setRootIsDecorated(TRUE);
  lv->setSelectionMode( QListView::Single );
  MPListViewItem *dynamic_items = new MPListViewItem( lv, CPS);

  QToolTip::add(l, tr("Drag and drop, psets or individual processes from the processes\nsets (psets) above.   In the StatsPanel, the statistics from\nthese grouped processes will be displayed in\ncolumns relative to this display.") );
  TBlayout->addWidget(lv);
  
  // ??  compareList.push_back(lv);
  
  tabWidget->addTab( frame, QString("Column #%1-%2").arg(tcnt).arg(ccnt)  );
  int count = tabWidget->count();
// printf("count=%d\n", count);
QWidget *cp = tabWidget->page(count-1);
cp->show();
tabWidget->showPage(cp);
  tabWidget->setTabToolTip(cp, "The information in this tab will be displayed in\nthe StatsPanel's associated column.  i.e. Column #1\nwill be in the first column of the StatsPanel\nColumn #2 the second.  Column #3 the third...");

tcnt++;
}

void
ManageCompareClass::removeCSet()
{
  QWidget *currentItem = csetTB->currentItem();

printf("Don't forget to destroy the allocated cset\n");

  csetTB->removeItem( currentItem );
}

void
ManageCompareClass::removeRaisedTab()
{
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();


  QWidget *currentTab = currentTabWidget->currentPage();


  currentTabWidget->removePage(currentTab);
  
}

void
ManageCompareClass::removeUserPSet()
{
printf("removeUserPSet() hmmmm\n");
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  QWidget *currentTab = currentTabWidget->currentPage();

printf("Hmmmm.\n");
}

void
ManageCompareClass::focusOnCSet()
{
printf("focusOnCSet() entered.   Now all you need to do is implement it.\n");
}
