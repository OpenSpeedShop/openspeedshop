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
  
// begin Enable debugging of the Manage Process Panel
// by uncommenting the define DEBUG_MPPanel line
//
//#define DEBUG_MPPanel 1
//
// end Enable debugging of the Manage Process Panel
//

#include "ManageCollectorsClass.hxx"

#include "debug.hxx"

#include <qapplication.h>
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

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusObject.hxx"
#include "UpdateObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"

ManageCollectorsClass::ManageCollectorsClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id, bool isInstrumentorOffline )
    : QWidget( parent, name )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");

#if DEBUG_MPPanel
  printf("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");
  printf("ManageCollectorsClass::ManageCollectorsClass() isInstrumentorOffline=%d\n", isInstrumentorOffline);
#endif

  dialog = NULL;
  user_defined_psets = NULL;
  userPsetCount = 0;
  loadTimer = NULL;
  updateTimer = NULL;
  p = _p;
  dialogSortType = PID_T;
//  dialogSortType = COLLECTOR_T;
  popupMenu = NULL;
  paramMenu = NULL;
  collectorPopupMenu = NULL;
  mw = (OpenSpeedshop *)p->getPanelContainer()->getMainWindow();
  cli = p->getPanelContainer()->getMainWindow()->cli;
  clo = NULL;
  expID = exp_id;
  timerValue = 10000;
  list_of_collectors.clear();
  setInstrumentorIsOffline(isInstrumentorOffline);

  if ( !name ) setName( "ManageCollectorsClass" );

  ManageCollectorsClassLayout = new QVBoxLayout( this, 1, 1, "ManageCollectorsClassLayout"); 

  splitter = new QSplitter(this, "splitter");
  splitter->setOrientation(QSplitter::Horizontal);

  attachCollectorsListView = new MPListView( splitter, "attachCollectorsListView", 0 );
  attachCollectorsListView->addColumn( 
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->addColumn("                   ");
  attachCollectorsListView->addColumn( tr( QString("Name") ) );
  attachCollectorsListView->addColumn( tr( QString("") ) );
  attachCollectorsListView->setColumnWidthMode(0, QListView::Manual);
  attachCollectorsListView->setColumnWidthMode(1, QListView::Maximum);
  attachCollectorsListView->setColumnWidth(0, 100);
  attachCollectorsListView->setColumnWidth(2, 120);
  attachCollectorsListView->setSelectionMode( QListView::Multi );
  attachCollectorsListView->setAllColumnsShowFocus( TRUE );
  attachCollectorsListView->setShowSortIndicator( TRUE );
  attachCollectorsListView->setRootIsDecorated(TRUE);
  attachCollectorsListView->setAcceptDrops( FALSE );
  attachCollectorsListView->viewport()->setAcceptDrops(TRUE);
  QToolTip::add(attachCollectorsListView->viewport(), tr("Drag and drop onto a user defined process\nset (see right side this panel) to create\nuser defined process set.\n\nSelect one or more of these process(es) \nand then select the menu item\n \"Focus on Process(es)\" and the statistics\nfor these processes will be displayed .") );


  psetListView = new MPListView( (QWidget *)splitter, (const char *)"psetlist", 0 );
  psetListView->addColumn(tr("Process Sets"));
  psetListView->addColumn("PID       ");
  psetListView->addColumn("Rank      ");
  psetListView->addColumn("Thread    ");
  psetListView->setColumnWidthMode(0, QListView::Manual);
  psetListView->setColumnWidth(0, 100);
  psetListView->setColumnWidthMode(1, QListView::Manual);
  psetListView->setColumnWidth(1, 100);
  psetListView->setColumnWidthMode(2, QListView::Maximum);
  QToolTip::add(psetListView->viewport(), tr("Create a new user defined process set by\ndragging and dropping processes or process\nsets on the the new definition.\n\nSelect one of the process sets and then select\nthe menu item \"Focus on Process(es)\" and the\nstatistics for these processes will be displayed.") );

  psetListView->setAllColumnsShowFocus( TRUE );
  psetListView->setShowSortIndicator( TRUE );
  psetListView->setRootIsDecorated(TRUE);
//  psetListView->setSelectionMode( QListView::Multi );
  psetListView->setSelectionMode( QListView::Single );

  QHeader *header = psetListView->header();
  header->resizeSection(0, 200);

  psetListView->show();

  int width = p->getPanelContainer()->width();
  int height = p->getPanelContainer()->height();
  QValueList<int> sizeList;
  sizeList.clear();
  if( splitter->orientation() == QSplitter::Vertical ) {
    sizeList.push_back((int)(height/4));
    sizeList.push_back(height-(int)(height/6));
  } else {
    sizeList.push_back((int)(width/2));
    sizeList.push_back(width-(int)(width/2));
  }
  splitter->setSizes(sizeList);

  // Restore geometry settings
  QSettings settings;
  QValueList<int> currentSizes = splitter->sizes();
  QValueList<int> persistedSizes;
  int index = 0;
  for(QValueList<int>::Iterator currentSize = currentSizes.begin(); currentSize != currentSizes.end(); ++currentSize) {
    persistedSizes.push_back(settings.readNumEntry(QString("/openspeedshop/managecollectors/splitter/size_%1").arg(index++), *currentSize));
  }
  splitter->setSizes(persistedSizes);

  ManageCollectorsClassLayout->addWidget( splitter );

  languageChange();

  connect(attachCollectorsListView, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint& , int ) ), this, SLOT( contextMenuRequested( QListViewItem *, const QPoint &, int ) ) );

  connect( attachCollectorsListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( focusOnProcessSelected( QListViewItem* )) );
  connect( psetListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( focusOnProcessSelected( QListViewItem* )) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageCollectorsClass::~ManageCollectorsClass()
{
  // Store geometry settings
  QSettings settings;
  QValueList<int> currentSizes = splitter->sizes();
  int index = 0;
  for(QValueList<int>::Iterator currentSize = currentSizes.begin(); currentSize != currentSizes.end(); ++currentSize) {
    settings.writeEntry(QString("/openspeedshop/managecollectors/splitter/size_%1").arg(index++), *currentSize);
  }

  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsClass::ManageCollectorsClass() destructor called.\n");
#if DEBUG_MPPanel
  printf("ManageCollectorsClass::ManageCollectorsClass() destructor called.\n");
#endif
  if( updateTimer ) {
    updateTimer->stop();
  }
  if( loadTimer ) {
    loadTimer->stop();
  }
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageCollectorsClass::languageChange()
{
  setCaption( tr( "Manage Processes Panel" ) );
  QString command;

//  command = QString("listTypes -v all");
  command = QString("list -v expTypes -v all");
// printf("command=(%s)\n", command.ascii() );
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), 
         &list_of_collectors, clip, TRUE ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }
}

QString
ManageCollectorsClass::selectedCollectors()
{
  QListViewItem *selectedItem = NULL;
  QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);

  while( it.current() ) {
    selectedItem = (QListViewItem *)it.current();
    break;
  }

  if( selectedItem ) {
    printf("Got an ITEM!\n");
    QString ret_value = selectedItem->text(0);
    return( ret_value );
  } else {
    printf("NO ITEMS SELECTED\n");
    return( NULL );
  }

}

void
ManageCollectorsClass::updateAttachedList()
{

#if DEBUG_MPPanel
  printf("ManageCollectorsClass::updateAttachedList() dialogSortType=%d\n", dialogSortType);
  printf("ManageCollectorsClass::updateAttachedList(%d) \n", expID );
  printf("ManageCollectorsClass::updateAttachedList, getInstrumentorIsOffline()=(%d) \n", getInstrumentorIsOffline() );
#endif

  if( MPListView::draggingFLAG == TRUE ) {
    return;
  }

  bool openAll = FALSE;
  if( attachCollectorsListView->childCount() > 0 ) {
    if( attachCollectorsListView->isOpen( attachCollectorsListView->firstChild()) ) {
      openAll = TRUE;
    }
  }

  attachCollectorsListView->clear();

  switch( dialogSortType )
  {
    case COLLECTOR_T:
      {
#ifdef DEBUG_MPPanel
        printf("--------ManageCollectorsClass::updateAttachedList(), COLLECTOR_T: expID=%d\n", expID );
#endif
        CollectorEntry *ce = NULL;
        if( clo ) {
          delete(clo);
        }
  
        clo = new CollectorListObject(expID);
        CollectorEntryList::Iterator it;
        for( it = clo->collectorEntryList.begin();
           it != clo->collectorEntryList.end();
           ++it )
        {
          ce = (CollectorEntry *)*it;
          MPListViewItem *item = new MPListViewItem( attachCollectorsListView, ce->name, ce->short_name );
          DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, QString::null, QString::null, ce->name );
          item->descriptionClassObject = dco;
          try
          {
            ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
#if DEBUG_MPPanel
            printf("ManageCollectorsClass::updateAttachedList() dialogSortType=%d\n", dialogSortType);
            printf("ManageCollectorsClass::updateAttachedList eo=(0x%x)\n", eo );
#endif
      
            if( eo && eo->FW() != NULL ) {

              // The following bit of code was snag and modified from SS_View_exp.cxx

              ThreadGroup tgrp = eo->FW()->getThreads();
              ThreadGroup::iterator ti;

              bool atleastone = false;

              for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {

                Thread t = *ti;
                std::string host = t.getHost();
                pid_t pid = t.getProcessId();

                if (!atleastone) {
                  atleastone = true;
                }

                QString pidstr = QString("%1").arg(pid);
                std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
                QString tidstr = QString::null;

                if (pthread.first) {
                  tidstr = QString("%1").arg(pthread.second);
                }

                std::pair<bool, int> rank = t.getMPIRank();
                QString ridstr = QString::null;

                if (rank.first) {
                  ridstr = QString("%1").arg(rank.second);
                }

                CollectorGroup cgrp = t.getCollectors();
                CollectorGroup::iterator ci;
                int collector_count = 0;

                for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
                  Collector c = *ci;
                  Metadata m = c.getMetadata();
                  if (collector_count) {
                  } else {
                    collector_count = 1;
                  }

                  if( m.getUniqueId() == ce->name.ascii() ) {

                    if( !pidstr.isEmpty() ) {

                      MPListViewItem *item2 = new MPListViewItem( item, host, pidstr, ridstr, tidstr );
                      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
                      item2->descriptionClassObject = dco;
                    } else if( !tidstr.isEmpty() ) {
                      MPListViewItem *item2 = new MPListViewItem( item, host, pidstr, ridstr, tidstr );
                      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, tidstr, ridstr, tidstr  );
                      item2->descriptionClassObject = dco;
                    } else if( !ridstr.isEmpty() ) {
                      MPListViewItem *item2 = new MPListViewItem( item, host, pidstr, ridstr, tidstr );
                      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
                      item2->descriptionClassObject = dco;
                    } else {
                      MPListViewItem *item2 = new MPListViewItem( item, host, pidstr, ridstr, tidstr );
                      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
                      item2->descriptionClassObject = dco;
                    }
                  }
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
      attachCollectorsListView->setColumnText( 0, tr( QString("Collectors")) );
      attachCollectorsListView->setColumnText( 1, tr( QString("Name") ) );
      attachCollectorsListView->setColumnText( 2, "            " );
      }
      break;

    case PID_T:
    {
#ifdef DEBUG_MPPanel
      printf("--------ManageCollectorsClass::updateAttachedList(), PID_T: expID=%d\n", expID );
#endif
      QString ridstr = QString::null;
      QString tidstr = QString::null;
      try
      {
        ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
        if( eo && eo->FW() != NULL ) {

#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updateAttachedList(), got an experiment.\n");
#endif

          // The following bit of code was snag and modified from SS_View_exp.cxx
          ThreadGroup tgrp = eo->FW()->getThreads();

#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updateAttachedList(), eo->Determine_Status() = (%d)\n", eo->Determine_Status() );
#endif
          if( ( (eo->Determine_Status() == ExpStatus_NonExistent) ||
                (eo->Determine_Status() == ExpStatus_Terminated ) ||
                (eo->Determine_Status() == ExpStatus_InError ) ) && updateTimer ) {
#ifdef DEBUG_MPPanel
            printf("ManageCollectorsClass::updateAttachedList(), 1 - stop updateTimer\n" );
#endif
            updateTimer->stop();
          }

          ThreadGroup::iterator ti;
          bool atleastone = false;

          for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {

            Thread t = *ti;
            std::string host = t.getHost();
            pid_t pid = t.getProcessId();

            // Add some status to each thread.
            QString threadStatusStr;
            switch( t.getState() )
            {
              case Thread::Disconnected:
                threadStatusStr = "Disconnected";
                break;
              case Thread::Connecting:
                threadStatusStr = "Connecting";
                break;
              case Thread::Nonexistent:
                threadStatusStr = "Nonexistent";
                break;
              case Thread::Running:
                threadStatusStr = "Running";
                break;
              case Thread::Suspended:
                threadStatusStr = "Suspended";
                break;
              case Thread::Terminated:
                threadStatusStr = "Terminate";
                break;
              default:
                threadStatusStr = "Unknown";
                break;
            }

#ifdef DEBUG_MPPanel
            printf("ManageCollectorsClass::updateAttachedList(), threadStatusStr=(%s)\n", threadStatusStr.ascii() );
#endif

            if (t.getState() == Thread::Disconnected) {
              // No reason to keep the timer going.  This is a load of previously saved data
              // It was stopped above but will restart if the updateTimer variable is not NULLed
              updateTimer = NULL;

#ifdef DEBUG_MPPanel
              printf("ManageCollectorsClass::updateAttachedList(), NULLing the updateTimer, threadStatusStr=(%s)\n", threadStatusStr.ascii() );
#endif

            }

            if (!atleastone) {
              atleastone = true;
            }

            QString pidstr = QString("%1").arg(pid);
            std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
            tidstr = QString::null;
            if (pthread.first) {
              tidstr = QString("%1").arg(pthread.second);
            }
            std::pair<bool, int> rank = t.getMPIRank();

            if (rank.first) {
              ridstr = QString("%1").arg(rank.second);
#ifdef DEBUG_MPPanel
              printf("ManageCollectorsClass::updateAttachedList(), GOT A RIDSTR=(%s)\n", ridstr.ascii() );
#endif
            }

            CollectorGroup cgrp = t.getCollectors();
            CollectorGroup::iterator ci;
            int collector_count = 0;
            MPListViewItem *item = NULL;
            if( ridstr.isEmpty() ) {
              item = new MPListViewItem( attachCollectorsListView, pidstr, threadStatusStr );
            } else {
              if( tidstr.isEmpty() ) {
                item = new MPListViewItem( attachCollectorsListView, pidstr, ridstr, threadStatusStr );
              } else {
                item = new MPListViewItem( attachCollectorsListView, pidstr, ridstr, tidstr, threadStatusStr );
              } 
            }

            DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
            item->descriptionClassObject = dco;

            for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
              Collector c = *ci;
              Metadata m = c.getMetadata();
              if (collector_count) {
              } else {
                collector_count = 1;
              }
              MPListViewItem *item2 = NULL;
              if( ridstr.isEmpty() ) {
                item2 = new MPListViewItem( item, host, m.getUniqueId());
              } else {
                if( tidstr.isEmpty() ) {
                  item2 = new MPListViewItem( item, host, ridstr, m.getUniqueId());
                } else {
                  item2 = new MPListViewItem( item, host, ridstr, tidstr, m.getUniqueId());
                } 
              }

              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
              item2->descriptionClassObject = dco;
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
      if( ridstr.isEmpty() ) {
        attachCollectorsListView->setColumnText( 0, tr( QString("Processes:")) );
        attachCollectorsListView->setColumnText( 1, tr( QString("Status") ) );
        attachCollectorsListView->setColumnText( 2, "            " );
      } else {
        if( tidstr.isEmpty() ) {
          attachCollectorsListView->setColumnText( 0, tr( QString("Processes:")) );
          attachCollectorsListView->setColumnText( 1, tr( QString("Rank") ) );
          attachCollectorsListView->setColumnText( 2, tr( QString("Status") ) );
        } else {
          attachCollectorsListView->setColumnText( 0, tr( QString("Processes:")) );
          attachCollectorsListView->setColumnText( 1, tr( QString("Rank") ) );
          attachCollectorsListView->setColumnText( 2, tr( QString("Thread") ) );
          attachCollectorsListView->setColumnText( 3, tr( QString("Status") ) );
        } 
      }
    }
    break;

  case  MPIRANK_T:


// Does this one make sense?
    attachCollectorsListView->setColumnText( 0, tr( QString("Ranks")) );
    attachCollectorsListView->setColumnText( 1, tr( QString("Process ID") ) );
    attachCollectorsListView->setColumnText( 2, "            " );
    attachCollectorsListView->setSorting(0, TRUE);
    attachCollectorsListView->setShowSortIndicator(TRUE);

    {
#ifdef DEBUG_MPPanel
    printf("--ManageCollectorsClass::updateAttachedList(), MPIRANK_T expID=%d\n", expID );
#endif
      try
      {
        ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
        if( eo && eo->FW() != NULL )
        {
#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updateAttachedList(), MPIRANK_T got an experiment.\n");
#endif
  // The following bit of code was snag and modified from SS_View_exp.cxx
          ThreadGroup tgrp = eo->FW()->getThreads();
#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updateAttachedList(), MPIRANK_T eo->Determine_Status() = (%d)\n", eo->Determine_Status() );
#endif
          if( ( (eo->Determine_Status() == ExpStatus_NonExistent) ||
            (eo->Determine_Status() == ExpStatus_Terminated ) ||
            (eo->Determine_Status() == ExpStatus_InError ) ) && updateTimer )
          {
#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updateAttachedList(), 2 - stop updateTimer\n" );
#endif
            updateTimer->stop();
            updateTimer = NULL;
          }
          ThreadGroup::iterator ti;
          bool atleastone = false;
          for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
          {
            Thread t = *ti;
            std::string host = t.getHost();
            pid_t pid = t.getProcessId();

            // Add some status to each thread.
            QString threadStatusStr;
            switch( t.getState() )
            {
              case Thread::Disconnected:
                threadStatusStr = "Disconnected";
                break;
              case Thread::Connecting:
                threadStatusStr = "Connecting";
                break;
              case Thread::Nonexistent:
                threadStatusStr = "Nonexistent";
                break;
              case Thread::Running:
                threadStatusStr = "Running";
                break;
              case Thread::Suspended:
                threadStatusStr = "Suspended";
                break;
              case Thread::Terminated:
                threadStatusStr = "Terminate";
                break;
              default:
                threadStatusStr = "Unknown";
                break;
            }

#ifdef DEBUG_MPPanel
             printf("ManageCollectorsClass::updateAttachedList(), pid=%d, threadStatusStr=(%s)\n", pid, threadStatusStr.ascii() );
#endif
  
            if (!atleastone) {
              atleastone = true;
            }

            QString pidstr = QString("%1").arg(pid);
            std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
            QString tidstr = QString::null;

            if (pthread.first) {
              tidstr = QString("%1").arg(pthread.second);
            }
            std::pair<bool, int> rank = t.getMPIRank();
            QString ridstr = QString::null;
            if (rank.first)
            {
              if (rank.second < 10) {
                ridstr = QString("%1 ").arg(rank.second);
              } else {
                ridstr = QString("%1").arg(rank.second);
              }
            }
#ifdef DEBUG_MPPanel
            printf("ManageCollectorsClass::updateAttachedList(), pidstr=%s, ridstr=(%s), tidstr=%s)\n", pidstr.ascii(), ridstr.ascii(), tidstr.ascii() );
#endif
            CollectorGroup cgrp = t.getCollectors();
            CollectorGroup::iterator ci;
            int collector_count = 0;
            MPListViewItem *item = NULL;
            item = new MPListViewItem( attachCollectorsListView, ridstr, threadStatusStr );
            DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
#ifdef DEBUG_MPPanel
            printf("ManageCollectorsClass::updateAttachedList(), item, dco -- new MPIListViewItem with ridstr=(%s))\n", ridstr.ascii() );
#endif
            item->descriptionClassObject = dco;
            for (ci = cgrp.begin(); ci != cgrp.end(); ci++)
            {
              Collector c = *ci;
              Metadata m = c.getMetadata();
              if (collector_count) {
              } else {
                collector_count = 1;
              }
              MPListViewItem *item2 = NULL;
              item2 = new MPListViewItem( item, host, m.getUniqueId());
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr  );
#ifdef DEBUG_MPPanel
              printf("ManageCollectorsClass::updateAttachedList(), item2, dco-- new MPIListViewItem with ridstr=(%s), host.c_str()=%s)\n", ridstr.ascii(),host.c_str() );
#endif
              item2->descriptionClassObject = dco;
            }
          }
//        }
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
      attachCollectorsListView->setColumnText( 0, tr( QString("Ranks:")) );
      attachCollectorsListView->setColumnText( 1, tr( QString("Status") ) );
      attachCollectorsListView->setColumnText( 2, tr( QString("      ") ) );
#ifdef DEBUG_MPPanel
      printf("--end ---- ManageCollectorsClass::updateAttachedList(), RANK_T, expID=%d\n", expID );
#endif
    }
    break;
  case  HOST_T:
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
#ifdef DEBUG_MPPanel
       printf("--------ManageCollectorsClass::updateAttachedList(), HOST_T, expID=%d\n", expID );
#endif

      if( eo && eo->FW() != NULL )
      {
// The following bit of code was snag and modified from SS_View_exp.cxx
        ThreadGroup tgrp = eo->FW()->getThreads();
        ThreadGroup::iterator ti;
        std::vector<std::string> v;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          std::string s = t.getHost();
        
          v.push_back(s);
        
        }

#ifdef DEBUG_MPPanel
       printf("-- 1 HOST -----ManageCollectorsClass::updateAttachedList(), calling std::sort\n");
#endif
        std::sort(v.begin(), v.end());
        
        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        for( std::vector<std::string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          MPListViewItem *item = new MPListViewItem( attachCollectorsListView, *hi );
          DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, QString::null, QString::null  );
          item->descriptionClassObject = dco;
          bool atleastone = false;
          for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
          {
            Thread t = *ti;
            std::string host = t.getHost();
            if( host == *hi )
            {
              pid_t pid = t.getProcessId();
              if (!atleastone) {
                atleastone = true;
              }
              QString pidstr = QString("%1").arg(pid);
              std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
              QString tidstr = QString::null;
              if (pthread.first)
              {
                tidstr = QString("%1").arg(pthread.second);
              }
              std::pair<bool, int> rank = t.getMPIRank();
              QString ridstr = QString::null;
              if (rank.first)
              {
                ridstr = QString("%1").arg(rank.second);
              }
              CollectorGroup cgrp = t.getCollectors();
              CollectorGroup::iterator ci;
              std::string collectorliststring;
              int collector_count = 0;
              for (ci = cgrp.begin(); ci != cgrp.end(); ci++)
              {
                Collector c = *ci;
                Metadata m = c.getMetadata();
                if (collector_count)
                {
                  collectorliststring += "," + m.getUniqueId();
                } else
                {
                  collector_count = 1;
                  collectorliststring = m.getUniqueId();
                }
              }
              if( !tidstr.isEmpty() ) {
                MPListViewItem *item2 =
                  new MPListViewItem(item, pidstr, tidstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, tidstr, ridstr, tidstr, collectorliststring  );
                item2->descriptionClassObject = dco;
              } else if( !ridstr.isEmpty() ) {
                MPListViewItem *item2 =
                  new MPListViewItem(item, pidstr, ridstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr, collectorliststring  );
                item2->descriptionClassObject = dco;
              } else if( !pidstr.isEmpty() ) {
                MPListViewItem *item2 = 
                  new MPListViewItem( item, pidstr, collectorliststring  );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr, collectorliststring  );
                item2->descriptionClassObject = dco;
              } else {
                MPListViewItem *item2 = 
                  new MPListViewItem( item, pidstr, collectorliststring  );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host, pidstr, ridstr, tidstr, collectorliststring  );
                item2->descriptionClassObject = dco;
              }
            }
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
    attachCollectorsListView->setColumnText( 0, tr( QString("Hosts:")) );
    attachCollectorsListView->setColumnText( 1, tr( QString("        ") ) );
    attachCollectorsListView->setColumnText( 2, tr( QString("        ") ) );
    break;
  }


#ifdef DEBUG_MPPanel
  printf("ManageCollectorsClass::updateAttachedList, openAll=(%d) \n", openAll );
#endif

  if( openAll ) {
    QPtrList<QListViewItem> lst;
    QListViewItemIterator it( attachCollectorsListView );
    while ( it.current() )
    {
      attachCollectorsListView->setOpen( it.current(), TRUE);
      ++it;
    }
  }
}


void
ManageCollectorsClass::updatePSetList(MPListView *lv)
{
  if( lv == NULL )
  {
    lv = psetListView;
  }
  int pset_count = 0;

#ifdef DEBUG_MPPanel
  printf("ManageCollectorsClass::updatePSetList, expID=%d \n", expID );
#endif


  if( MPListView::draggingFLAG == TRUE )
  {
    return;
  }

  QStringList *psl = NULL;
  if( dialog )
  {
    psl = &dialog->psetNameList;
  }

  QValueList<QString> openList;

  openList.clear();

// Construct an "open list"
  if( lv->childCount() > 0 )
  {
    QListViewItemIterator it( lv );
    while ( it.current() )
    {
      QListViewItem *item = it.current();
      if( lv->isOpen( item ) ) {
#ifdef DEBUG_MPPanel
        printf("ManageCollectorsClass::updatePSetList, lv (%s) is open\n", item->text(0).ascii() );
#endif
        openList.push_back(item->text(0));
      }
      ++it;
    }
  }
  
  lv->clearSelection();


if( lv == psetListView )
{
  if( user_defined_psets == NULL ) {
    user_defined_psets = new MPListViewItem( lv, UDPS);
    user_defined_psets->setOpen(TRUE);
  }

  if( user_defined_psets ) {
    lv->takeItem(user_defined_psets);
  }

}

  lv->clear();


  QListViewItem *dynamic_items = new MPListViewItem( lv, DPS);

  if( lv == psetListView ) {
    lv->insertItem(user_defined_psets);
  }

  dynamic_items->setOpen(TRUE);

  QString pset_name = QString::null;

{ // For each host, create a dynamic collector 
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);

      if( eo && eo->FW() != NULL )
      {
// The following bit of code was snag and modified from SS_View_exp.cxx
// printf("ManageCollectorsClass::updatePSetList, For each host, create a dynamic collector.\n");
        ThreadGroup tgrp = eo->FW()->getThreads();
        ThreadGroup::iterator ti;
        std::vector<std::string> v;
//        pset_name = QString("pset%1").arg(pset_count++);
        pset_name = QString("All");
        MPListViewItem *item = new MPListViewItem( dynamic_items, pset_name, "All" );
        if( psl ) psl->append("All");
        DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
        dco->all = TRUE;
        item->descriptionClassObject = dco;

        MPListViewItem *item2 = new MPListViewItem(item, QString("All (%1) pids...").arg(tgrp.size()) );
        dco = new DescriptionClassObject(FALSE, "All");
        dco->all = TRUE;
        item2->descriptionClassObject = dco;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
        {
          Thread t = *ti;
          std::string s = t.getHost();
        
          v.push_back(s);
        }

#ifdef DEBUG_MPPanel
        printf("---hosts-----ManageCollectorsClass::updatePSetList(), calling std::sort\n");
#endif
        std::sort(v.begin(), v.end());
        
        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        pset_name = QString("Hosts");
        MPListViewItem *host_items = new MPListViewItem( dynamic_items, pset_name );
        DescriptionClassObject *host_dco = new DescriptionClassObject(TRUE, pset_name);
        host_items->descriptionClassObject = host_dco;

        for( std::vector<std::string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          pset_name = QString(*hi);
          MPListViewItem *item = new MPListViewItem( host_items, pset_name, *hi );
          DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
          item->descriptionClassObject = dco;

#ifdef DEBUG_MPPanel
          printf("ManageCollectorsClass::updatePSetList(), hosts, hi=(%s)\n", hi->c_str() );
#endif
          bool atleastone = false;
          for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
          {
            Thread t = *ti;
            std::string host = t.getHost();
            if( host == *hi )
            {
              pid_t pid = t.getProcessId();
              if (!atleastone) {
                atleastone = true;
              }
              QString pidstr = QString("%1").arg(pid);
              std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
              QString tidstr = QString::null;
              if (pthread.first)
              {
                tidstr = QString("%1").arg(pthread.second);
              }
              std::pair<bool, int> rank = t.getMPIRank();
              QString ridstr = QString::null;
              if (rank.first)
              {
                ridstr = QString("%1").arg(rank.second);
              }
              CollectorGroup cgrp = t.getCollectors();
              CollectorGroup::iterator ci;
              std::string collectorliststring;
              int collector_count = 0;
              for (ci = cgrp.begin(); ci != cgrp.end(); ci++)
              {
                Collector c = *ci;
                Metadata m = c.getMetadata();
                if (collector_count)
                {
                  collectorliststring += "," + m.getUniqueId();
                } else
                {
                  collector_count = 1;
                  collectorliststring = m.getUniqueId();
                }
              }
              if( !tidstr.isEmpty() )
              {
                MPListViewItem *item2 =
                  new MPListViewItem(item, "", pidstr, ridstr, tidstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
                item2->descriptionClassObject = dco;
              } else if( !ridstr.isEmpty() )
              {
                MPListViewItem *item2 =
                  new MPListViewItem(item, "", pidstr, ridstr, tidstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
                item2->descriptionClassObject = dco;
              } else if( !pidstr.isEmpty() )
              {
                MPListViewItem *item2 = 
                  new MPListViewItem( item, "", pidstr, ridstr, tidstr, collectorliststring  );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
                item2->descriptionClassObject = dco;
              } else
              {
                MPListViewItem *item2 = 
                  new MPListViewItem( item, "", pidstr, ridstr, tidstr, collectorliststring  );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
                item2->descriptionClassObject = dco;
              }
            }
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
{ // For each thread status , create a dynamic pset.
      QValueList<StatusStruct> statusDisconnectedList;
      QValueList<StatusStruct> statusConnectingList;
      QValueList<StatusStruct> statusNonexistentList;
      QValueList<StatusStruct> statusRunningList;
      QValueList<StatusStruct> statusSuspendedList;
      QValueList<StatusStruct> statusTerminatedList;
      QValueList<StatusStruct> statusUnknownList;
      statusDisconnectedList.clear();
      statusConnectingList.clear();
      statusNonexistentList.clear();
      statusRunningList.clear();
      statusSuspendedList.clear();
      statusTerminatedList.clear();
      statusUnknownList.clear();

      StatusStruct *statusStruct = new StatusStruct();


      try
      {
        ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
        if( eo && eo->FW() != NULL )
        {
#ifdef DEBUG_MPPanel
           printf("ManageCollectorsClass::updatePSetList, got an experiment.\n");
#endif
  // The following bit of code was snag and modified from SS_View_exp.cxx
          ThreadGroup tgrp = eo->FW()->getThreads();
          ThreadGroup::iterator ti;
          bool atleastone = false;
          for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
          {
            Thread t = *ti;
            std::string host = t.getHost();
            pid_t pid = t.getProcessId();
            std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
            QString tidstr = QString::null;
            if (pthread.first)
            {
              tidstr = QString("%1").arg(pthread.second);
            }
            statusStruct->tid = tidstr;
            std::pair<bool, int> rank = t.getMPIRank();
            QString ridstr = QString::null;
            if (rank.first)
            {
              ridstr = QString("%1").arg(rank.second);
            }
            statusStruct->rid = ridstr;
            statusStruct->host = QString(host.c_str());
            statusStruct->pid = QString("%1").arg(pid);

#ifdef DEBUG_MPPanel
            printf("ManageCollectorsClass::updatePSetList,   statusStruct->host=(%s)\n", statusStruct->host.ascii() );
            printf("ManageCollectorsClass::updatePSetList,   statusStruct->pid=(%s)\n", statusStruct->pid.ascii() );
            printf("ManageCollectorsClass::updatePSetList,   statusStruct->rid=(%s)\n", statusStruct->rid.ascii() );
            printf("ManageCollectorsClass::updatePSetList,   statusStruct->tid=(%s)\n", statusStruct->tid.ascii() );
#endif

            // Add some status to each thread.
            QString threadStatusStr = QString::null;
            switch( t.getState() )
            {
              case Thread::Disconnected:
                threadStatusStr = "Disconnected";
                statusStruct->status = threadStatusStr;
                statusDisconnectedList.push_back(*statusStruct);
#if 0
printf("DISCONNECTED\n");
{
QValueList<StatusStruct>::iterator vi = statusDisconnectedList.begin();
for( ;vi != statusDisconnectedList.end(); vi++)
{
  StatusStruct ss = (StatusStruct)*vi;
printf("    ss.status=(%s)\n", ss.status.ascii() );
printf("    ss.host=(%s)\n", ss.host.ascii() );
printf("    ss.pid=(%s)\n", ss.pid.ascii() );
printf("    ss.tid=(%s)\n", ss.tid.ascii() );
printf("    ss.rid=(%s)\n", ss.rid.ascii() );
}
}
#endif // 0 
                break;
              case Thread::Connecting:
                threadStatusStr = "Connecting";
                statusStruct->status = threadStatusStr;
                statusConnectingList.push_back(*statusStruct);
                break;
                break;
              case Thread::Nonexistent:
                threadStatusStr = "Nonexistent";
                statusStruct->status = threadStatusStr;
                statusNonexistentList.push_back(*statusStruct);
                break;
              case Thread::Running:
                threadStatusStr = "Running";
                statusStruct->status = threadStatusStr;
                statusRunningList.push_back(*statusStruct);
                break;
              case Thread::Suspended:
                threadStatusStr = "Suspended";
                statusStruct->status = threadStatusStr;
                statusSuspendedList.push_back(*statusStruct);
#ifdef DEBUG_MPPanel
                printf("statusSuspendedList.pushback() \n");
#endif
                break;
              case Thread::Terminated:
                threadStatusStr = "Terminate";
                statusStruct->status = threadStatusStr;
                statusTerminatedList.push_back(*statusStruct);
                if( updateTimer )
                {
#ifdef DEBUG_MPPanel
                  printf("ManageCollectorsClass::updatePSetList, 1 - stop updateTimer\n" );
#endif
                  updateTimer->stop();
                  updateTimer = NULL;
                }
                break;
              default:
                threadStatusStr = "Unknown";
                statusStruct->status = threadStatusStr;
                statusUnknownList.push_back(*statusStruct);
                break;
            }
  
            if (!atleastone)
            {
              atleastone = true;
            }
#ifdef MOVED_UP
            QString pidstr = QString("%1").arg(pid);
            pthread = t.getPosixThreadId();
            tidstr = QString::null;
            if (pthread.first)
            {
              tidstr = QString("%1").arg(pthread.second);
            }
            rank = t.getMPIRank();
            ridstr = QString::null;
            if (rank.first)
            {
              ridstr = QString("%1").arg(rank.second);
            }
#endif // MOVED_UP

            CollectorGroup cgrp = t.getCollectors();
            CollectorGroup::iterator ci;
            int collector_count = 0;
            for (ci = cgrp.begin(); ci != cgrp.end(); ci++)
            {
              Collector c = *ci;
              Metadata m = c.getMetadata();
              if (collector_count)
              {
              } else
              {
                collector_count = 1;
              }
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
  // Put out the Disconnected Dynamic pset (if there is one.)
  if( statusDisconnectedList.size() > 0 )
  {
#ifdef DEBUG_MPPanel
    printf("\n\tstatusDisconnectedList.size() = (%d)\n", statusDisconnectedList.size() );
#endif
    QValueList<StatusStruct>::iterator vi = statusDisconnectedList.begin();
    pset_name = QString("Disconnected");
    if( psl ) psl->append(pset_name);

    MPListViewItem *disconnected_items = new MPListViewItem( dynamic_items, pset_name, "Disconnected" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    disconnected_items->descriptionClassObject = dco;
    for( ;vi != statusDisconnectedList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct )*vi;
#if 0
printf("ss.status=(%s)\n", ss.status.ascii() );
printf("ss.host=(%s)\n", ss.host.ascii() );
printf("ss.pid=(%s)\n", ss.pid.ascii() );
printf("ss.tid=(%s)\n", ss.tid.ascii() );
printf("ss.rid=(%s)\n", ss.rid.ascii() );
#endif // 0

      MPListViewItem *item = new MPListViewItem( disconnected_items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Connecting Dynamic pset (if there is one.)
  if( statusConnectingList.size() > 0 )
  {
#ifdef DEBUG_MPPanel
    printf("statusConnectingList.size() = (%d)\n", statusConnectingList.size() );
#endif
    QValueList<StatusStruct>::iterator vi = statusConnectingList.begin();
    pset_name = QString("Connecting");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Connecting" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusConnectingList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
#ifdef DEBUG_MPPanel
      printf("ss.status=(%s)\n", ss.status.ascii() );
      printf("ss.host=(%s)\n", ss.host.ascii() );
      printf("ss.pid=(%s)\n", ss.pid.ascii() );
#endif
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Nonexistent Dynamic pset (if there is one.)
  if( statusNonexistentList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusNonexistentList.begin();
    pset_name = QString("Non Existent");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Nonexistent" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusNonexistentList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Running Dynamic pset (if there is one.)
  if( statusRunningList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusRunningList.begin();
//    pset_name = QString("pset%1").arg(pset_count++);
    pset_name = QString("Running");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Running" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusRunningList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid );
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Suspended Dynamic pset (if there is one.)
  if( statusSuspendedList.size() > 0 )
  {
#ifdef DEBUG_MPPanel
// printf("statusSuspendedList.size() = (%d)\n", statusSuspendedList.size() );
#endif
    QValueList<StatusStruct>::iterator vi = statusSuspendedList.begin();
    pset_name = QString("Suspended");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Suspended" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusSuspendedList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
#ifdef DEBUG_MPPanel
// printf("ss.status=(%s)\n", ss.status.ascii() );
// printf("ss.host=(%s)\n", ss.host.ascii() );
// printf("ss.pid=(%s)\n", ss.pid.ascii() );
#endif
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid );
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the status Terminated Dynamic pset (if there is one.)
  if( statusTerminatedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusTerminatedList.begin();
//    pset_name = QString("pset%1").arg(pset_count++);
    pset_name = QString("Terminated");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Terminated" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusTerminatedList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid );
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Unknown Dynamic pset (if there is one.)
  if( statusUnknownList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusUnknownList.begin();
//    pset_name = QString("pset%1").arg(pset_count++);
    pset_name = QString("Unknown");
    if( psl ) psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Unknown" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusUnknownList.end(); vi++)
    {
      StatusStruct ss = (StatusStruct)*vi;
//      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid );
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
    }
  }

}

  if( lv->childCount() > 0 )
  {
    QListViewItemIterator it( lv );
    while ( it.current() )
    {
      QListViewItem *item = it.current();

      for( QValueList<QString>::iterator vi = openList.begin();
           vi != openList.end(); vi++)
      {
        QString ols = (QString)*vi;
        if( ols == item->text(0) )
        {
#ifdef DEBUG_MPPanel
 printf("Open %s\n", ols.ascii() );
#endif
          bool closeParent = FALSE;
          if( item->parent() && !lv->isOpen(item->parent()) )
          {
#ifdef DEBUG_MPPanel
 printf("%s has a parent that is not open. close the parent\n", item->parent()->text(0).ascii() );
#endif
            closeParent = TRUE;
          }
          lv->setOpen(item, TRUE);
          if( closeParent )
          {
            lv->setOpen( item->parent(), FALSE );
          }
        }
      }
      ++it;
    }
  }

}

void
ManageCollectorsClass::detachSelected()
{
#ifdef DEBUG_MPPanel
  printf("detachSelected\n");
#endif

  MPListViewItem *selectedItem = NULL;
  QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
  while( it.current() )
  {
    selectedItem = (MPListViewItem *)it.current();
    break;

  }
  if( selectedItem )
  {
    QString ret_value = selectedItem->text(0);
#ifdef DEBUG_MPPanel
    printf("ManageCollectorsClass::detachSelected, detach = (%s)\n", ret_value.ascii() );
#endif

    QString collector_name = selectedItem->text(0);
    QString command;
    command = QString("expDetach -x %1 %2").arg(expID).arg(collector_name);
#ifdef DEBUG_MPPanel
    printf("ManageCollectorsClass::detachSelected, command=(%s)\n", command.ascii() );
#endif
//    QMessageBox::information( this, tr("Under Construction:"), tr("This feature currently under construction.\nIt will eventuall do a:\n%1").arg(command), QMessageBox::Ok );
//    return;

    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
    }
  } else
  {
    QMessageBox::information( this, tr("Detach Collector Info:"), tr("You need to select a collector first."), QMessageBox::Ok );
  }
  updatePanel();
}

void
ManageCollectorsClass::disableSelected()
{
#ifdef DEBUG_MPPanel
  printf("disableSelected\n");
#endif
  QString command = QString("expDisable -x %1").arg(expID);
  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }
  updatePanel();
}


void
ManageCollectorsClass::enableSelected()
{
#if DEBUG_MPPanel
 printf("enableSelected\n");
#endif // 0 
  QString command = QString("expEnable -x %1").arg(expID);
  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }
  updatePanel();
}

void
ManageCollectorsClass::attachProcessSelected()
{
#if DEBUG_MPPanel
 printf("addProcessSelected\n");
#endif // 0 
  mw->executableName = QString::null;
  mw->pidStr = QString::null;
  mw->attachNewProcess();


  if( !mw->pidStr.isEmpty() )
  {
    QString command;

    // Hack to get host and pid strings for the attach... This will be 
    // replace with something better shortly.

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);
    command = QString("expAttach -x %1 -p %2 -h %3\n").arg(expID).arg(mw->pidStr).arg(mw->hostStr); 
#if DEBUG_MPPanel
 printf("A: command=(%s)\n", command.ascii() );
#endif // 0 

    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();

    if( !cli->runSynchronousCLI(command.ascii()) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  //    return;
    }

    // Send out a message to all those that might care about this change request
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    

    loadTimer->stop();
    pd->hide();

  }
  // Send out a message to all those that might care about this change request
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    
  if( eo && eo->FW() != NULL )
  {
    UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
    p->broadcast((char *)msg, GROUP_T);
  }
  
  updatePanel();
}

void
ManageCollectorsClass::createUserPSet()
{
    bool ok;
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::createUserPSet() entered\n");
#endif // 0 
  QString res = QInputDialog::getText("Create Named PSet %1 : %2", QString("PSet Name:"), QLineEdit::Normal, QString("udpset%1").arg(userPsetCount), &ok, this);
  if( ok )
  {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::createUserPSet(), The user named his set %s\n", res.ascii() );
#endif // 0 
    MPListViewItem *item = new MPListViewItem( user_defined_psets, res );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, res);
    item->descriptionClassObject = dco;
    userPsetCount++;
  }
}


void
ManageCollectorsClass::removeUserPSet()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::removeUserPSet() entered\n");
#endif // 0 

  QListViewItem *top = psetListView->currentItem();
  QListViewItem *last = top;
  if( !top || top->parent() == NULL )
  {
    return;
  }
  while( top )
  {
    last = top;
    top = top->parent();
  }
  
  if( last->text(0) != UDPS )
  {
    return;
  }
  
  QListViewItem *item = psetListView->currentItem();
  if( item )
  {
     delete item;
  }
}

void
ManageCollectorsClass::focusOnRankSelected(QListViewItem *item)
{
#if DEBUG_MPPanel
 printf("focusOnRankSelected(QListView *) listView=0x%x attachCollectorsListView=0x%x psetListView=0x%x\n", item->listView(), attachCollectorsListView, psetListView );
#endif // 0 
 
  if( item->listView() == attachCollectorsListView ) {

#if DEBUG_MPPanel
    printf("focusOnRankSelected(QListView *) attachCollectorsListView section\n");
#endif // 0 

    focusOnPSetList(attachCollectorsListView, TRUE /* focusOnRankOnly */);
    psetListView->clearSelection();

  } else {

#if DEBUG_MPPanel
    printf("focusOnRankSelected(QListView *) ELSE for attachCollectorsListView section\n");
#endif // 0 

    focusOnPSetList(psetListView, TRUE /* focusOnRankOnly */);
    attachCollectorsListView->clearSelection();

  }
}

void
ManageCollectorsClass::focusOnProcessSelected(QListViewItem *item)
{
#if DEBUG_MPPanel
 printf("focusOnProcessSelected(QListView *) listView=0x%x attachCollectorsListView=0x%x psetListView=0x%x\n", item->listView(), attachCollectorsListView, psetListView );
#endif // 0 
 
  if( item->listView() == attachCollectorsListView ) {

#if DEBUG_MPPanel
    printf("focusOnProcessSelected(QListView *) attachCollectorsListView section\n");
#endif // 0 

    focusOnPSetList(attachCollectorsListView, FALSE /* focusOnRankOnly */);
    psetListView->clearSelection();

  } else {

#if DEBUG_MPPanel
    printf("focusOnProcessSelected(QListView *) ELSE for attachCollectorsListView section\n");
#endif // 0 

    focusOnPSetList(psetListView, FALSE /* focusOnRankOnly */);
    attachCollectorsListView->clearSelection();

  }
}

void
ManageCollectorsClass::focusOnRankSelected()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::focusOnRankSelected() entered.\n");
#endif // 0 
  focusOnPSetList(attachCollectorsListView, TRUE /* focusOnRankOnly */);
}


void
ManageCollectorsClass::focusOnProcessSelected()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::focusOnProcessSelected() entered.\n");
#endif // 0 
  focusOnPSetList(attachCollectorsListView, FALSE /* focusOnRankOnly */);
}

void
ManageCollectorsClass::selectProcessesSelected()
{
#if DEBUG_MPPanel
 printf("Bring up the dialog!\n");
#endif // 0 

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  if( dialog == NULL )
  {
    dialog = new CompareProcessesDialog(this, "Add/Delete/Describe Compare Processes Dialog", TRUE);
  }

  if( dialog->updateFocus(expID, psetListView) )
  {
    updatePSetList(dialog->availableProcessesListView);
  }
  dialog->show();

  QApplication::restoreOverrideCursor();
}

void
ManageCollectorsClass::focusOnPSetSelected()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::focusOnPSetSelected() entered\n");
#endif // 0 
  focusOnPSetList(psetListView, FALSE /* focusOnRankOnly */);
}

void
ManageCollectorsClass::focusOnRankSetSelected()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::focusOnRankSetSelected() entered\n");
#endif // 0 
  focusOnPSetList(psetListView, TRUE /* focusOnRankOnly */);
}

void
ManageCollectorsClass::focusOnPSetList(QListView *lv, bool focusOnRankOnly)
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::focusOnPSetList(QListView *) entered.\n");
#endif // 0 

  FocusObject *msg = NULL;
  QString pid_name = QString::null;
  QString pidString = QString::null;
 
  QListViewItemIterator it(lv, QListViewItemIterator::Selected);
  while( it.current() )
  {
    MPListViewItem *lvi = (MPListViewItem *)it.current();
#if DEBUG_MPPanel
    printf("PSetSelection: lvi->text(0)=(%s) (%s)\n", lvi->text(0).ascii(), lvi->text(1).ascii() );
    if( lvi->descriptionClassObject )
    {
      lvi->descriptionClassObject->Print();
    }
#endif // 0
    if( msg == NULL ) {
#if DEBUG_MPPanel
      printf("About to create the FocusObject()\n");
#endif // 0 
      msg = new FocusObject(expID,  NULL, NULL, NULL, NULL, TRUE, focusOnRankOnly);
    }

    msg->descriptionClassList.clear();

#if DEBUG_MPPanel
    printf("Created the FocusObject()\n");
#endif // 0 

    if( !lvi || !lvi->descriptionClassObject ) {
      QMessageBox::information( this, tr("Focus Error:"), tr("Unable to focus on selection: No description for process(es)."), QMessageBox::Ok );
      return;
    }

#if DEBUG_MPPanel
    printf("lvi->descriptionClassObject->all=%d\n", lvi->descriptionClassObject->all);
#endif // 0 

    if( lvi->descriptionClassObject->all ) {

#if DEBUG_MPPanel
        printf("Do ALL threads, everywhere.\n");
#endif // 0 

//        msg->host_pid_vector.clear();
//
    } else if( lvi->descriptionClassObject->root ) {

      // Loop through all the children...
      //
#if DEBUG_MPPanel
      printf("Loop through all the children of (%s).\n", lvi->text(0).ascii() );
#endif // 0 

      MPListViewItem *mpChild = (MPListViewItem *)lvi->firstChild();
      if (!mpChild) {
         QMessageBox::information( this, tr("Warning:"), tr("There are no process(es) are in the User Defined Process Set, so displaying results for the entire experiment.\nPlease add processes to the process set."), QMessageBox::Ok );
      } else {

#if DEBUG_MPPanel
      printf("mpChild= (%s).\n", mpChild->text(0).ascii() );
#endif // 0 

      MPListViewItem *child = (MPListViewItem *)mpChild->firstChild();
      if( !child )
      {
#if DEBUG_MPPanel
 printf("no nested levels.  Just do the immediate children.\n");
#endif // 0 
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

          msg->descriptionClassList.push_back(*mpChild->descriptionClassObject);
#if DEBUG_MPPanel
          printf("A: push_back a new vector list.\n");
          mpChild->descriptionClassObject->Print();
#endif // 0 
          mpChild = (MPListViewItem *)mpChild->nextSibling();
        }
      } else {

        while( mpChild )
        {
          MPListViewItem *nested_child = (MPListViewItem *)mpChild->firstChild();
          while( nested_child )
          {

#if DEBUG_MPPanel
             printf("A: nested_child...(%s)\n", nested_child->text(0).ascii() );
#endif // 0 

            QString host_name = nested_child->descriptionClassObject->host_name;
            if( host_name.isEmpty() )
            {
              host_name = "localhost";
            }
            QString pid_name = nested_child->descriptionClassObject->pid_name;
            if( pid_name.isEmpty() )
            {
              nested_child = (MPListViewItem *)nested_child->nextSibling();
              continue;
            }
            msg->descriptionClassList.push_back(*nested_child->descriptionClassObject);
            nested_child = (MPListViewItem *)nested_child->nextSibling();
          }

          mpChild = (MPListViewItem *)mpChild->nextSibling(); 
        }
      }
     } // end there was an mpChild
    } else {

      QString host_name = lvi->descriptionClassObject->host_name;

#if DEBUG_MPPanel
      printf("host_name = (%s)\n", host_name.ascii() );
#endif // 0

      if( host_name.isEmpty() ) {
        host_name = "localhost";
      }

      QString pid_name = lvi->descriptionClassObject->pid_name;
      if( pid_name.isEmpty() ) {
        continue;
      }

      msg->descriptionClassList.push_back(*lvi->descriptionClassObject);

#if DEBUG_MPPanel
      printf("B: push_back a new vector list.. (%s-%s)\n", host_name.ascii(), pid_name.ascii() );
#endif // 0

    } 
    
    ++it;
  }

// If nothing was selected, just return.
  if( !msg ) {
    QMessageBox::information( this, tr("Error process selection:"), tr("Unable to focus: No processes selected."), QMessageBox::Ok );
    if( msg ) {
      delete msg;
    }
    return;
  }


#if DEBUG_MPPanel
  printf("A: focus the StatsPanel...\n");
#endif // 0

  QString name = QString("Stats Panel [%1]").arg(expID);
  Panel *sp = p->getPanelContainer()->findNamedPanel(p->getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if( !sp ) {
    char *panel_type = (char *) "Stats Panel";
    PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    sp = p->getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
    delete ao;
    if( sp != NULL )
    {
#if DEBUG_MPPanel
      printf("Created a stats panel... First update it's data...\n");
#endif // 0
      UpdateObject *msg = new UpdateObject((void *)Find_Experiment_Object((EXPID)expID), expID, "none", 1);
      sp->listener( (void *)msg );
    }
  } else {
    // don't raise the associated panels
    msg->raiseFLAG = false;

#if DEBUG_MPPanel
    printf("There was a statspanel... send the update message.\n");
    msg->print();
#endif // 0

    sp->listener( (void *)msg );
  }
}

void
ManageCollectorsClass::updatePanel()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::updatePanel()\n");
#endif 

  updateAttachedList();

  updatePSetList();

#if DEBUG_MPPanel
 printf("ManageCollectorsClass::updatePanel(), trying to start the updateTimer, updateTimer=%d\n", updateTimer);
#endif 
  if( updateTimer )
  {
    updateTimer->stop();
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::updatePanel(), stop then updateTimer->start() timerValue=%d\n", timerValue);
#endif 
    updateTimer->start( timerValue, TRUE );
  }


}


void
ManageCollectorsClass::loadProgramSelected()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::loadProgramSelected()\n");
#endif
  mw->executableName = QString::null;
  mw->argsStr = QString::null;
//  mw->loadNewProgram();
  PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
  // JEG - figure this out
  bool tmp_is_offline = false;
  mw->loadNewProgramPanel(p->getPanelContainer(), bestFitPC, expID, (Panel *) p, tmp_is_offline);
//  mw->loadNewProgramPanel(p->getPanelContainer(), this->topPC, expID, (Panel *) this);

#ifdef DEBUG_CustomPanel
  printf("ManageCollectorsClass::loadProgramSelected(), calling mw->loadNewProgramPanel, p=%d\n", p );
#endif

  QString executableNameStr = mw->executableName;

  if( !mw->executableName.isEmpty() ) {

#if DEBUG_MPPanel
 printf("ManageCollectorsClass::loadProgramSelected() executableName=%s\n", mw->executableName.ascii() );
#endif

    executableNameStr = mw->executableName;
    QString command =
      QString("expAttach -x %1 -f \"%2 %3\"").arg(expID).arg(executableNameStr).arg(mw->argsStr);

#if DEBUG_MPPanel
 printf("command=(%s)\n", command.ascii() );
#endif
    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();

    if( !cli->runSynchronousCLI(command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  //    return;
  
    }
    loadTimer->stop();
    pd->hide();

//    delete(pd);

    // Send out a message to all those that might care about this change request
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    
#if DEBUG_MPPanel
 printf("Send out update?\n");
#endif
    if( eo && eo->FW() != NULL )
    {
#if DEBUG_MPPanel
 printf("Yes!  Send out update?\n");
#endif
      UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
      p->broadcast((char *)msg, GROUP_T);
    }
  
  }

  updatePanel();
}

void
ManageCollectorsClass::paramSelected(int val)
{
#if DEBUG_MPPanel
  printf("ManageCollectorsClass:paramSelected(), paramSelected val=%d\n", val);
  printf("ManageCollectorsClass:paramSelected(), paramSelected val=%s\n", QString("%1").arg(val).ascii() );
#endif
//  MPListViewItem *selectedItem = attachCollectorsListView->selectedItem();
MPListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (MPListViewItem *)it.current();
  break;
}
  QString param_text = QString::null;
  if( selectedItem )
  {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass:paramSelected(), selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
#endif
    QString collector_name = QString::null;
    if( dialogSortType == PID_T )
    {
      if( selectedItem->parent() )
      {
        collector_name = selectedItem->text(1);
      } else
      {
        collector_name = selectedItem->parent()->text(0);
      }
    } else if( dialogSortType == COLLECTOR_T )
    {
      if( selectedItem->parent() )
      {
        collector_name = selectedItem->parent()->text(0);
      } else
      {
        collector_name = selectedItem->text(0);
      }
    }
    QString param_name = QString::null;
    int loc = -1;
    QString param_value = QString::null;
    if( paramMenu )
    {
      param_text = paramMenu->text(val);
      loc = param_text.find(":");
      param_name = param_text.left(loc);
      loc++;
      loc++;
      param_value = param_text.right(param_text.length()-loc);
    } else
    {
       // other menu...
      QString pt = popupMenu->text(val);
      int loc_1 = pt.find("(");
      loc_1++;
      param_text = pt.right(pt.length()-loc_1);
      loc = param_text.find(":");
      param_name = param_text.left(loc);
      loc++;
      loc++;
      pt = param_text.right(param_text.length()-loc);
      param_value = pt.left(pt.find(")") );
    }
#if DEBUG_MPPanel
 printf("ManageCollectorsClass:paramSelected(), paramSelected collector_name=(%s)\n", collector_name.ascii());
 printf("ManageCollectorsClass:paramSelected(), paramSelected param_name=(%s)\n", param_name.ascii());
 printf("ManageCollectorsClass:paramSelected(), paramSelected param_value=(%s)\n", param_value.ascii());
 printf("ManageCollectorsClass:paramSelected(), paramSelected param_value=(%u)\n", param_value.toUInt() );
#endif
// Modify the parameter....
    bool ok;
//    int res = QInputDialog::getInteger(QString("Set %1 : %2").arg(collector_name).arg(param_name), QString("New Value:"), param_value.toUInt(), 0, 9999999, 10, &ok, this);
    QString res = QInputDialog::getText(QString("Set %1 : %2").arg(collector_name).arg(param_name), QString("New Value:"), QLineEdit::Normal, param_value, &ok, this);
    if( ok )
    {
      QString command;
#if DEBUG_MPPanel
 printf("ManageCollectorsClass:paramSelected(), colletor_name=(%s)\n", collector_name.ascii() );
#endif
      command = QString("expSetParam -x %1 %2::%3=%4").arg(expID).arg(collector_name).arg(param_name).arg(res);
#if DEBUG_MPPanel
 printf("command=(%s)\n", command.ascii() );
#endif
      if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
      {
        QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
      }
    } else
    {
#if DEBUG_MPPanel
  printf("user pressed cancel.\n");
#endif
    }
    
  }

  updateAttachedList();
}

void
ManageCollectorsClass::RS_attachCollectorSelected(int val)
{
#if DEBUG_MPPanel
 printf("RS_attachCollectorSelected(val=%d)\n", val);
#endif
  QString attachStr = QString::null;
  QString collector_name = QString::null;

  QListViewItemIterator it(psetListView, QListViewItemIterator::Selected);
  while( it.current() )
  {
    MPListViewItem *lvi = (MPListViewItem *)it.current();
// Begin debug
#if DEBUG_MPPanel
 printf("RS_attachCollectorSelected: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
 printf("lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
 printf("lvi->text(1) =(%s)\n", lvi->text(1).ascii() );
 if( lvi->descriptionClassObject ) {
  lvi->descriptionClassObject->Print();
 }
#endif
// End debug


// ***
  if( collectorPopupMenu != NULL )
  {
    collector_name = collectorPopupMenu->text(val);
#if DEBUG_MPPanel
 printf("Get the collector name (%s) from the popup menu.\n", collector_name.ascii() );
#endif
  } else
  {
    collector_name = collectorMenu->text(val);
#if DEBUG_MPPanel
 printf("Get the collector name (%s) from the file menu.\n", collector_name.ascii() );
#endif
  }
#if DEBUG_MPPanel
 printf("collector_name =(%s)\n", collector_name.isEmpty() ? "" : collector_name.ascii() );
#endif

    if( lvi->descriptionClassObject->all )
    {
#if DEBUG_MPPanel
 printf("Do ALL threads, everywhere.\n");
#endif
//        msg->host_pid_vector.clear();
    } else if( lvi->descriptionClassObject->root )
    {
      // Loop through all the children...
#if DEBUG_MPPanel
 printf("Loop through all the children.\n");
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
        std::pair<std::string, std::string> p(host_name.ascii(),pid_name.ascii());
//        msg->host_pid_vector.push_back( p );
        attachStr += QString(" -h %1 -p %2 ").arg(host_name).arg(pid_name);
#if DEBUG_MPPanel
 printf("A: push_back a new vector list..\n");
#endif
        mpChild = (MPListViewItem *)mpChild->nextSibling();
      }
    } else
    {
#if DEBUG_MPPanel
 printf("else host/pid\n");
#endif
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
      std::pair<std::string, std::string> p(host_name.ascii(),pid_name.ascii());
//      msg->host_pid_vector.push_back( p );
      attachStr += QString(" -h %1 -p %2 ").arg(host_name).arg(pid_name);
// printf("B: push_back a new vector list.. (%s-%s)\n", host_name.ascii(), pid_name.ascii() );
    } 
// ***
    it++;
  }

  QString command;
  command = QString("expAttach -x %1 %2 %3").arg(expID).arg(collector_name).arg(attachStr);

#if DEBUG_MPPanel
 printf("command=%s\n", command.ascii() );
#endif
  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }

//  updateAttachedList();
  updatePanel();
}

void
ManageCollectorsClass::LS_attachCollectorSelected(int val)
{
#if DEBUG_MPPanel
 printf("LS_attachCollectorSelected(val=%d)\n", val);
#endif
  MPListViewItem *lvi = NULL;
  QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
  QString pidStr = QString::null;
  while( it.current() )
  {
    lvi = (MPListViewItem *)it.current();
#if DEBUG_MPPanel
 printf("lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
 printf("lvi->text(1) =(%s)\n", lvi->text(1).ascii() );
#endif
    if( pidStr.isEmpty()  )
    {
      pidStr = " -p"+lvi->text(0);
    } else
    {
      pidStr += ","+lvi->text(0);
    }
    it++;
  }
  MPListViewItem *selectedItem = lvi;
  QString param_text = QString::null;
  QString collector_name = QString::null;
  QString target_name = QString::null;
  if( selectedItem )
  {
#if DEBUG_MPPanel
 printf("selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
#endif
    if( dialogSortType == COLLECTOR_T )
    {
      if( selectedItem->parent() )
      {
        target_name = selectedItem->text(1);
      } else
      {
#if DEBUG_MPPanel
 printf("Can't add a collector from a selected collector.\n");
#endif
        QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to add a collector from a selected collector.\nUnselect the highlighted selector first."), QMessageBox::Ok );
        return; }
    }
  }
#if DEBUG_MPPanel
 printf("target_name =(%s)\n", target_name.isEmpty() ? "" : target_name.ascii() );
#endif

  if( collectorPopupMenu != NULL )
  {
    collector_name = collectorPopupMenu->text(val);
#if DEBUG_MPPanel
 printf("Get the collector name (%s) from the popup menu.\n", collector_name.ascii() );
#endif
  } else
  {
    collector_name = collectorMenu->text(val);
#if DEBUG_MPPanel
 printf("Get the collector name (%s) from the file menu.\n", collector_name.ascii() );
#endif
  }
#if DEBUG_MPPanel
 printf("collector_name =(%s)\n", collector_name.isEmpty() ? "" : collector_name.ascii() );
#endif

  QString command;
  command = QString("expAttach -x %1 %2 %3").arg(expID).arg(target_name).arg(collector_name);

  if( dialogSortType == PID_T )
  {  // Add the PID(s)
    command += pidStr;
  }

#if DEBUG_MPPanel
 printf("command=(%s)\n", command.ascii() );
#endif

  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }

//  updateAttachedList();
  updatePanel();
}

void
ManageCollectorsClass::fileCollectorAboutToShowSelected()
{
#if DEBUG_MPPanel
 printf("fileCollectorAboutToShowSelected()\n");
#endif
  if( collectorPopupMenu != NULL )
  {
    delete collectorPopupMenu;
  }
  collectorPopupMenu = NULL;
}

void
ManageCollectorsClass::sortByProcess()
{
#if DEBUG_MPPanel
 printf("sortByProcess\n");
#endif
  dialogSortType = PID_T;


#if DEBUG_MPPanel
 printf("attachCollectorsListView->columnText(0) = (%s)\n", attachCollectorsListView->columnText(1).ascii() );
#endif
  updateAttachedList();
}

void
ManageCollectorsClass::sortByCollector()
{
#if DEBUG_MPPanel
 printf("sortByCollector\n");
#endif
  dialogSortType = COLLECTOR_T;

  updateAttachedList();
}

void
ManageCollectorsClass::sortByHost()
{
#if DEBUG_MPPanel
 printf("sortByHost\n");
#endif
  dialogSortType = HOST_T;

  updateAttachedList();
}

void
ManageCollectorsClass::sortByMPIRank()
{
#if DEBUG_MPPanel
 printf("sortByMPIRank\n");
#endif
  dialogSortType = MPIRANK_T;

  updateAttachedList();
}

static bool step_forward = TRUE;
void
ManageCollectorsClass::progressUpdate()
{
  pd->qs->setValue( steps );
  if( step_forward )
  {
    steps++;
  } else
  {
    steps--;
  }
  if( steps == 10 )
  {
    step_forward = FALSE;
  } else if( steps == 0 )
  {
    step_forward = TRUE;
  }
}

void
ManageCollectorsClass::updateTimerCallback()
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::updateTimerCallback() entered, stop updateTimer\n");
#endif

  updateTimer->stop();

  updatePanel();
}


void
ManageCollectorsClass::contextMenuRequested( QListViewItem *item, const QPoint &pos, int col)
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested() entered, attachCollectorsListView=%x\n", attachCollectorsListView);
#endif


  if( item == 0 )
  {
    return;
  }
  QListViewItem *selected_item = item;


  QString field_name = QString::null;
  if( item )
  {
#if 0
  if (attachCollectorsListView && attachCollectorsListView->selectedItem()->text(0) && attachCollectorsListView->selectedItem()->text(1)) {
  printf("%s %s\n", attachCollectorsListView->selectedItem()->text(0).ascii(), attachCollectorsListView->selectedItem()->text(1).ascii()  );
  }
#endif
    if( dialogSortType == COLLECTOR_T )
    {
      field_name = item->text(0);
#if 0
     if (item->text(0) && item->text(1)) {
      printf("A: %s \n", item->text(0).ascii(), item->text(1).ascii()  );
      printf("A: %s %s\n", item->text(0).ascii(), item->text(1).ascii()  );
     }
#endif
    } else
    {
#if 0
     if (item->text(0) && item->text(1)) {
        printf("B: %s %s\n", item->text(0).ascii(), item->text(1).ascii() );
       if (attachCollectorsListView && attachCollectorsListView->selectedItem()->text(0) && attachCollectorsListView->selectedItem()->text(1)) {
        printf("%s %s\n", attachCollectorsListView->selectedItem()->text(0).ascii(), attachCollectorsListView->selectedItem()->text(1).ascii()  );
       }
     }
#endif
      field_name = item->text(1);
    }
  } else
  {
    return;
  }

  if( popupMenu != NULL )
  {
    delete popupMenu;
  } 
  popupMenu = new QPopupMenu(this);

  if( paramMenu != NULL )
  {
    delete paramMenu;
  }
  paramMenu = NULL;
  if( collectorPopupMenu != NULL )
  {
    delete collectorPopupMenu;
  }
  collectorPopupMenu = NULL;



  // It may make sense to allow other SortTypes to add/delete collectors... 
  // At this point only this sort type is supported.
  if( dialogSortType == COLLECTOR_T || dialogSortType == PID_T || 
      dialogSortType == HOST_T || dialogSortType == MPIRANK_T )
  {

    if( selected_item &&
       ( dialogSortType == COLLECTOR_T && selected_item->parent() == NULL ) ||
       ( dialogSortType == PID_T && selected_item->parent() != NULL ) || 
       ( dialogSortType == MPIRANK_T && selected_item->parent() != NULL ) )
    {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, Here field_name=(%s)\n", !field_name.isEmpty() ? field_name.ascii() : NULL );
#endif
      CollectorEntry *ce = NULL;
      CollectorEntryList::Iterator it;
      clo = new CollectorListObject(expID);
      if( clo )
      {
        for( it = clo->collectorEntryList.begin();
             it != clo->collectorEntryList.end();
             ++it )
        {
          ce = (CollectorEntry *)*it;
          if( field_name == ce->name )
          {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, (%s): parameters are\n", ce->name.ascii() );
#endif
            CollectorParameterEntryList::Iterator pit = ce->paramList.begin();
            if( ce->paramList.size() == 1 )
            {
#if DEBUG_MPPanel
 printf("size == 1\n");
#endif
              CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
              popupMenu->insertItem( QString("Modify Parameter ... (%1::%2)").arg(cpe->name.ascii()).arg(cpe->param_value.ascii()), this, SLOT(paramSelected(int)) );
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, done size == 1\n");
#endif
            } else
            {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, size != 1\n");
#endif
              if( paramMenu == NULL )
              {
                paramMenu = new QPopupMenu(this);
              }
              connect( paramMenu, SIGNAL( activated( int ) ),
                         this, SLOT( paramSelected( int ) ) );
              popupMenu->insertItem("Modify Parameter", paramMenu);
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, done size != 1\n");
#endif
              for( ;pit != ce->paramList.end();  pit++)
              {
                CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, \t%s   %s\n", cpe->name.ascii(), cpe->param_value.ascii() );
#endif
                paramMenu->insertItem(QString("%1::%2").arg(cpe->name.ascii()).arg(cpe->param_value.ascii()) );
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, \tdone: %s   %s\n", cpe->name.ascii(), cpe->param_value.ascii() );
#endif
              }
            }
            break;
          }
        }
      }
    }
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, here...calling menu() with popupMenu=0x%x\n", popupMenu );
#endif
  menu(popupMenu);
  }


#if DEBUG_MPPanel
 printf("ManageCollectorsClass::contextMenuRequested, here...calling popup() with &pos=0x%x\n", &pos );
#endif
  popupMenu->popup( pos );
}

bool
ManageCollectorsClass::menu(QPopupMenu* contextMenu)
{
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() entered\n");
#endif

  bool runnableFLAG = TRUE;

  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
    if( eo && eo->FW() != NULL )
    {
      // The following bit of code was snag and modified from SS_View_exp.cxx
      ThreadGroup tgrp = eo->FW()->getThreads();
      if( ( (eo->Determine_Status() == ExpStatus_NonExistent) ||
            (eo->Determine_Status() == ExpStatus_Terminated ) ||
            (eo->Determine_Status() == ExpStatus_InError ) ) )
      {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() We're NOT runnable or attachable!\n");
#endif
        runnableFLAG = FALSE;
      }
    }
  }
  catch(const std::exception& error)
  {
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return FALSE;
  }

// This is going to introduce a bug, where the time needs to be 
// restarted.   At this point, the timer seems to be messing up 
// the menus.   Turn off the timer while the menu is active.
// Any further selection or update of the view will reactivate the timer.
  if( updateTimer )
  {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() Stop the timer, then start the timer WAS COMMENTED OUT \n");
#endif
    updateTimer->stop();
//    updateTimer->start( timerValue, TRUE );
  }

  psetListView->contentsDragLeaveEvent(NULL);
  psetListView->contentsMouseReleaseEvent(NULL);


  bool udpsetSelected = FALSE;
  bool selectable = TRUE;
  bool leftSide = TRUE;

  MPListViewItem *selectedItem = NULL;
  if( attachCollectorsListView->hasMouse() )
  {
    // Left Side!
    QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
    while( it.current() )
    {
      selectedItem = (MPListViewItem *)it.current();
      break;
    }
    psetListView->clearSelection();
  } else if( psetListView->hasMouse() )
  {
    // Right Side!
    attachCollectorsListView->clearSelection();
    leftSide = FALSE;

    // If this is a root node, there's no selection available.  Ignore..
    QListViewItemIterator it(psetListView, QListViewItemIterator::Selected);
    while( it.current() )
    {
      selectedItem = (MPListViewItem *)it.current();
      break;
    }
    if( !selectedItem || selectedItem->parent() == NULL )
    {
      psetListView->clearSelection();
      selectable = FALSE;
    } else if( selectedItem && selectedItem->parent() && selectedItem->parent()->text(0) == UDPS )
    {
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() Got a user defined processes set!\n");
#endif
      udpsetSelected = TRUE;
    }
  }


#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() Now add the rest of the menu entries\n");
#endif
  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "_updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Update Panel...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Attempt to update this panel's display with fresh data.") );


#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() runnableFLAG=(%d)\n", runnableFLAG );
#endif
  if( runnableFLAG == TRUE )
  {
    qaction = new QAction( this,  "loadProgram");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Load Program...") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( loadProgramSelected() ) );
    qaction->setStatusTip( tr("Opens dialog box to load application from disk.") );
  
    qaction = new QAction( this,  "attachProcess");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Attach Process...") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( attachProcessSelected() ) );
    qaction->setStatusTip( tr("Opens dialog box to attach to running process.") );
  }

  qaction = new QAction( this,  "focusOnProcess");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Focus on Selected Thread(s) and/or Process(es)...") );
  if( leftSide == TRUE ) {
#if DEBUG_MPPanel
   printf("ManageCollectorsClass::menu() LEFT SIDE MENU\n");
   printf("ManageCollectorsClass::menu()  Focus on selected thread(s) and/or process(es)\n");
#endif
    connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnProcessSelected() ) );
    qaction->setStatusTip( tr("Focus on selected thread(s) and/or process(es).") );
  } else {
#if DEBUG_MPPanel
   printf("ManageCollectorsClass::menu() RIGHT SIDE MENU\n");
#endif
    connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnPSetSelected() ) );
    qaction->setStatusTip( tr("Focus on selected thread(s) and/or process(es).") );
  }

  qaction = new QAction( this,  "focusOnRank");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Focus on Selected Rank(s) and underlying threads...") );
  if( leftSide == TRUE ) {
#if DEBUG_MPPanel
   printf("ManageCollectorsClass::menu() LEFT SIDE MENU\n");
   printf("ManageCollectorsClass::menu()  Focus on selected ranks(s)\n");
#endif
    connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnRankSelected() ) );
    qaction->setStatusTip( tr("Focus on selected rank(s). If an individual rank is selected, also focus on underlying threads.") );
  } else {
#if DEBUG_MPPanel
   printf("ManageCollectorsClass::menu() RIGHT SIDE MENU\n");
#endif
    connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnRankSetSelected() ) );
    qaction->setStatusTip( tr("Focus on selected rank(s).") );
  }

  if( udpsetSelected == TRUE )
  {
    qaction = new QAction( this,  "selectProcesses");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Select Process(es)...") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( selectProcessesSelected() ) );
    qaction->setStatusTip( tr("Bring up dialog to selected process(es).") );
  }

  // If we can't select this item, leave it here, but disable it.
  if( !selectable ) 
  {
    qaction->setEnabled(FALSE);
  }


  collectorMenu = new QPopupMenu(contextMenu);
  if( leftSide == TRUE )
  {
    connect( collectorMenu, SIGNAL( activated( int ) ),
                     this, SLOT( LS_attachCollectorSelected( int ) ) );
  } else
  {
    connect( collectorMenu, SIGNAL( activated( int ) ),
                     this, SLOT( RS_attachCollectorSelected( int ) ) );
  }
  connect( collectorMenu, SIGNAL( aboutToShow() ),
                     this, SLOT( fileCollectorAboutToShowSelected( ) ) );

  if( runnableFLAG == TRUE )
  {
    contextMenu->insertItem("Add Collector", collectorMenu);
    if( list_of_collectors.size() > 0 ) 
    {
      for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
      {
        std::string collector_name = *it;
        QAction *qaction = new QAction( this,  "addCollector");
        qaction->addTo( collectorMenu );
        qaction->setText( QString(collector_name.c_str()) );
        qaction->setStatusTip( tr(QString("Add the collector %1 to the experiment.").arg(collector_name.c_str()) ) );
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() Add item (%s)\n", collector_name.c_str() );
#endif
      }
    }
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() A: size =(%d) \n", list_of_collectors.size() );
#endif
  }
  
#if DEBUG_MPPanel
 printf("ManageCollectorsClass::menu() leftSide=(%d)\n", leftSide );
#endif
  if( leftSide == TRUE )
  {
    QPopupMenu *sortByMenu = new QPopupMenu( contextMenu );
    qaction = new QAction( this,  "sortByProcess");
    qaction->addTo( sortByMenu );
    qaction->setText( tr("Sort By Process") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( sortByProcess() ) );
    qaction->setStatusTip( tr("Sort the collectors by attached processes.") );

    qaction = new QAction( this,  "sortByCollector");
    qaction->addTo( sortByMenu );
    qaction->setText( tr("Sort By Collector") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( sortByCollector() ) );
    qaction->setStatusTip( tr("Sort the list by attached collectors.") );

    qaction = new QAction( this,  "sortByHost");
    qaction->addTo( sortByMenu );
    qaction->setText( tr("Sort By Host") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( sortByHost() ) );
    qaction->setStatusTip( tr("Sort the list by known hosts.") );

    qaction = new QAction( this,  "sortByMPIRank");
    qaction->addTo( sortByMenu );
    qaction->setText( tr("Sort By MPI Rank") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( sortByMPIRank() ) );
    qaction->setStatusTip( tr("Sort the list mpi rank. (Currently unimplemented)") );

    contextMenu->insertItem("Sort By... ", sortByMenu);

if( dialogSortType == COLLECTOR_T )
{
if( runnableFLAG == TRUE )
{
    qaction = new QAction( this,  "detachCollector");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Detach Collector") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( detachSelected() ) );
    qaction->setStatusTip( tr("Detach the selected (highlighted) collector from the experiment.") );
}

    qaction = new QAction( this,  "enableCollector");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Enable Collector(s)") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( enableSelected() ) );
    qaction->setStatusTip( tr("Enable the collectors in the experiment.") );

    qaction = new QAction( this,  "disableCollector");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Disable Collector(s)") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( disableSelected() ) );
    qaction->setStatusTip( tr("Disable the collectors in the experiment.") );
}
  } else
  {

    contextMenu->insertSeparator();

    qaction = new QAction( this,  "createUserPSet");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Create A User Define Process Set") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( createUserPSet() ) );
    qaction->setStatusTip( tr("Create a new user defined process set.") );

    qaction = new QAction( this,  "removeUserPSet");
    qaction->addTo( contextMenu );
    qaction->setText( tr("Remove Item") );
    connect( qaction, SIGNAL( activated() ), this, SLOT( removeUserPSet() ) );
    qaction->setStatusTip( tr("Remove the highlighted user defined process set.") );
  }


  return( TRUE );
}
