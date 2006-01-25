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
  
  p = _p;
  mw = (OpenSpeedshop *)p->getPanelContainer()->getMainWindow();
  cli = p->getPanelContainer()->getMainWindow()->cli;
  expID = exp_id;
  clo = NULL;
  ce = NULL;

#ifdef OLDWAY
  experimentComboBox = NULL;
  collectorComboBox = NULL;
  metricComboBox = NULL;
#endif // OLDWAY

  csl = NULL;

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

  updateInfo();

  compareList.clear();

  addNewCSet();


  languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareClass::~CompareClass()
{
// printf("CompareClass() destructor called\n");
  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); )
  {
    CompareSet *cs = (CompareSet *)*it;
// printf("delete cs\n");
    delete cs;
    ++it;
  }
  csl->clear();
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

  qaction = new QAction( this,  "updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Update...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Update the information in this panel.") );


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
  CompareSet *cset = new CompareSet( csetTB, this);
  
  if( !csl )
  {
    csl = new CompareSetList();
  }
  
  csl->push_back(cset);
}

// Entry point from the menu.  You'll need to lookup the tabWidget
void
CompareClass::addNewColumn()
{
  CompareSet *compareSet = findCurrentCompareSet();

  addNewColumn(compareSet);
} 

void
CompareClass::addNewColumn(CompareSet *compareSet)
{
  ColumnSet *columnSet = new ColumnSet(compareSet);

  compareSet->columnSetList.push_back( columnSet );

  connect( columnSet->experimentComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeExperiment( const QString & ) ) );
  connect( columnSet->collectorComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeCollector( const QString & ) ) );

  columnSet->updateInfo();
}

void
CompareClass::updatePanel()
{
  updateInfo();
}

void
CompareClass::focusOnCSetSelected()
{
printf("CompareClass::focusOnCSetSelected() entered\n");
printf("Currently unimplemented\n");


#ifdef DEBUG
// For now, dump out all the information....   Eventually focus on just 
// the highlighted CompareSet
  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); ++it )
  {
    CompareSet *compareSet = (CompareSet *)*it;
printf("CompareSet: (%s)'s info\n", compareSet->name.ascii() );

    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
    }
  }
#endif // DEBUG

  CompareSet *compareSet = findCurrentCompareSet();
  if( compareSet )
  {
// printf("CompareSet: (%s)'s info\n", compareSet->name.ascii() );
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
// printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
// printf("\t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
// printf("\t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
// printf("\t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
// printf("\t\t: processes:\n");

// Begin real focus logic
// printf("CompareClass::focusOnCSetSelected() entered.\n");

  FocusObject *msg = NULL;
  QString pid_name = QString::null;
  QString pidString = QString::null;
 
  QListViewItemIterator it(columnSet->lv);
  while( it.current() )
  {
    MPListViewItem *lvi = (MPListViewItem *)it.current();
// printf("PSetSelection: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
// printf("lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
// printf("lvi->text(1) =(%s)\n", lvi->text(1).ascii() );
// if( lvi->descriptionClassObject )
// {
//  lvi->descriptionClassObject->Print();
//}
    if( msg == NULL )
    {
      msg = new FocusObject(expID,  NULL, NULL, TRUE);
    }
    if( !lvi || !lvi->descriptionClassObject )
    {
      ++it;
      continue;
//      QMessageBox::information( this, tr("Focus Error:"), tr("Unable to focus on selection: No description for process(es)."), QMessageBox::Ok );
//      return;
    }

    if( lvi->descriptionClassObject->all )
    {
//printf("Do ALL threads, everywhere.\n");
        msg->host_pid_vector.clear();
    } else if( lvi->descriptionClassObject->root )
    {
      // Loop through all the children...
//printf("Loop through all the children.\n");
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
        std::pair<std::string, std::string> p(host_name,pid_name);
        msg->host_pid_vector.push_back( p );
//printf("A: push_back a new vector list..\n");
        mpChild = (MPListViewItem *)mpChild->nextSibling();
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
      std::pair<std::string, std::string> p(host_name,pid_name);
//printf("B: push_back a new vector list... (%s:%s)\n", host_name.ascii(), pid_name.ascii() );
      msg->host_pid_vector.push_back( p );
    } 
    
    ++it;
  }




// If nothing was selected, just return.
  if( !msg )
  {
    QMessageBox::information( this, tr("Error process selection:"), tr("Unable to focus: No processes selected."), QMessageBox::Ok );
    if( msg )
    {
      delete msg;
    }
    return;
  }



//printf("A: focus the StatsPanel...\n");
  QString name = QString("Stats Panel [%1]").arg(expID);
  Panel *sp = p->getPanelContainer()->findNamedPanel(p->getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if( !sp )
  {
    char *panel_type = "Stats Panel";
    PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    sp = p->getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
    delete ao;
    if( sp != NULL )
    {
//printf("Created a stats panel... First update it's data...\n");
      UpdateObject *msg =
        new UpdateObject((void *)Find_Experiment_Object((EXPID)expID), expID, "none", 1);
      sp->listener( (void *)msg );
    }
  } else
  {
//printf("There was a statspanel... send the update message.\n");
    sp->listener( (void *)msg );
  }
// End real focus logic
    }
  }

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

  CompareSet *cs_to_delete = findCurrentCompareSet();

  // If we removed a CompareSet from the list, it should be free
  // to delete.  Delete it.
  if( cs_to_delete )
  {
// printf("Don't forget to remove all the tabs!\n");
    csl->remove( cs_to_delete );
    csetTB->removeItem( currentItem );
    delete cs_to_delete;
  }
}

void
CompareClass::removeRaisedTab()
{
// printf("CompareClass::removeRaisedTab() entered\n");
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  CompareSet *cs_to_search = findCurrentCompareSet();

  QWidget *currentTab = currentTabWidget->currentPage();

// printf("remove Tab labeled (%s)\n", currentTabWidget->tabLabel( currentTab ).ascii() );

  
// printf("NOW FIND AND REMOVE THIS TAB FROM THE columnList!\n");

  // Look for the tab in the CompareSet.   The delete it from the compareSet's
  // list.
  if( cs_to_search )
  {
// printf("Don't forget to remove all the tabs!\n");
    ColumnSetList::Iterator it;
    for( it = cs_to_search->columnSetList.begin(); it != cs_to_search->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) )
      {
// printf("Okay I'm sure you need to delete this (%s) tab\n", cs->name.ascii() );
        cs_to_search->columnSetList.remove( cs );
        currentTabWidget->removePage(currentTab);
        delete cs;
        break;
      }
    }

  }


  
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

void CompareClass::changeExperiment( const QString &path )
{
printf("CompareClass::changeExperiment(%s)\n", path.ascii()  );
}

void CompareClass::changeCollector( const QString &path )
{
printf("CompareClass::changeCollector(%s)\n", path.ascii()  );
}

void
CompareClass::updateInfo()
{
// printf("CompareClass::updateInfo() entered\n");
  gatherInfo();

  if( csl )
  {
    CompareSetList::Iterator it;
    for( it = csl->begin(); it != csl->end(); ++it )
    {
      CompareSet *cs = (CompareSet *)*it;
// printf("attempt to update (%s)'s info\n", cs->name.ascii() );
      cs->updateInfo();
    }
  }
}

CompareSet *
CompareClass::findCurrentCompareSet()
{
  QWidget *currentItem = csetTB->currentItem();

  int  currentIndex = csetTB->currentIndex();

  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); ++it )
  {
    CompareSet *cs = (CompareSet *)*it;
    if( cs->name == csetTB->itemLabel(currentIndex) )
    {
      return( cs );
    }
  }


 return( NULL );
}
