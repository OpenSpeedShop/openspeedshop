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
  

#include "ManageCollectorsClass.hxx"

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
#include <qinputdialog.h>
#include <qmenubar.h>
#include <qvaluelist.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qvaluelist.h>

struct StatusStruct
{
  QString status;
  QString host;
  QString pid;
};

// #include "SS_Input_Manager.hxx"
#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusObject.hxx"
#include "UpdateObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"

ManageCollectorsClass::ManageCollectorsClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QWidget( parent, name )
{
//  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");
  dprintf("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");
  
  loadTimer = NULL;
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
  list_of_collectors.clear();

  if ( !name ) setName( "ManageCollectorsClass" );

  ManageCollectorsClassLayout = new QVBoxLayout( this, 1, 1, "ManageCollectorsClassLayout"); 

  splitter = new QSplitter(this, "splitter");
  splitter->setOrientation(QSplitter::Horizontal);

  attachCollectorsListView = new QListView( splitter, "attachCollectorsListView" );
  attachCollectorsListView->addColumn( 
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->addColumn( tr( QString("Name") ) );
  attachCollectorsListView->setColumnWidthMode(0, QListView::Manual);
  attachCollectorsListView->setColumnWidthMode(1, QListView::Maximum);
  attachCollectorsListView->setColumnWidth(0, 100);
attachCollectorsListView->setSelectionMode( QListView::Multi );
  attachCollectorsListView->setAllColumnsShowFocus( TRUE );
  attachCollectorsListView->setShowSortIndicator( TRUE );
  attachCollectorsListView->setRootIsDecorated(TRUE);

  psetListView = new QListView( splitter, "*psetlist" );
  psetListView->addColumn(tr("Process Sets"));
  psetListView->addColumn("          ");
  psetListView->addColumn("          ");
  psetListView->setColumnWidthMode(0, QListView::Manual);
  psetListView->setColumnWidth(0, 100);
  psetListView->setColumnWidthMode(1, QListView::Manual);
  psetListView->setColumnWidth(1, 100);
  psetListView->setColumnWidthMode(2, QListView::Maximum);

  psetListView->setAllColumnsShowFocus( TRUE );
  psetListView->setShowSortIndicator( TRUE );
  psetListView->setRootIsDecorated(TRUE);
psetListView->setSelectionMode( QListView::Multi );
  psetListView->show();

  int width = p->getPanelContainer()->width();
  int height = p->getPanelContainer()->height();
  QValueList<int> sizeList;
  sizeList.clear();
  if( splitter->orientation() == QSplitter::Vertical )
  {
    sizeList.push_back((int)(height/4));
    sizeList.push_back(height-(int)(height/6));
  } else
  {
    sizeList.push_back((int)(width/4));
    sizeList.push_back(width-(int)(width/6));
  }
  splitter->setSizes(sizeList);

  ManageCollectorsClassLayout->addWidget( splitter );

  languageChange();
  connect( attachCollectorsListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( focusOnProcessSelected( QListViewItem* )) );
  connect( psetListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( focusOnProcessSelected( QListViewItem* )) );

//  updateAttachedList();

}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageCollectorsClass::~ManageCollectorsClass()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsClass::ManageCollectorsClass() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageCollectorsClass::languageChange()
{
  setCaption( tr( "Manage Processes Panel" ) );
  QString command;

  command = QString("listTypes -v all");
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
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  if( selectedItem )
  {
    printf("Got an ITEM!\n");
    QString ret_value = selectedItem->text(0);
    return( ret_value );
  } else
  {
    printf("NO ITEMS SELECTED\n");
    return( NULL );
  }
}



void
ManageCollectorsClass::updateAttachedList()
{
// printf("updateAttachedList() dialogSortType=%d\n", dialogSortType);
// printf("updateAttachedList(%d) \n", expID );

  attachCollectorsListView->clear();
  psetListView->clear();

  switch( dialogSortType )
  {
    case COLLECTOR_T:
      {
        CollectorEntry *ce = NULL;
        if( clo )
        {
          delete(clo);
        }
  
        clo = new CollectorListObject(expID);
        CollectorEntryList::Iterator it;
        for( it = clo->collectorEntryList.begin();
           it != clo->collectorEntryList.end();
           ++it )
        {
          ce = (CollectorEntry *)*it;
          QListViewItem *item = new QListViewItem( attachCollectorsListView, ce->name, ce->short_name );
          try
          {
            ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      
            if( eo->FW() != NULL )
            {
  // The following bit of code was snag and modified from SS_View_exp.cxx
              ThreadGroup tgrp = eo->FW()->getThreads();
              ThreadGroup::iterator ti;
              bool atleastone = false;
              for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
              {
                Thread t = *ti;
#if 0
// Do you want executable name?   Here it is.
//printf("t.getExecutable().getPath().getBaseName()=%s\n", t.getExecutable().getPath().getBaseName().c_str() );
// printf("t.getExecutable().getPath()=0x%x", t.getExecutable().getPath() );
std::pair<bool, LinkedObject> lo = t.getExecutable();
if( lo.first == TRUE )
{
  printf("Got an executable name.\n");
  printf("(%s)\n", lo.second.getPath().getBaseName().c_str() );
} else
{
  printf("no executable name.\n");
}
#endif // 0

                std::string host = t.getHost();
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
                  if( m.getUniqueId() == ce->name.ascii() )
                  {
                    if( !tidstr.isEmpty() )
                    {
                      QListViewItem *item2 =
                        new QListViewItem( item, host, pidstr, tidstr );
                    } else if( !ridstr.isEmpty() )
                    {
                      QListViewItem *item2 =
                        new QListViewItem( item, host, pidstr, ridstr );
                    } else
                    {
                      QListViewItem *item2 =
                        new QListViewItem( item, host, pidstr );
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
      }
      break;
    case PID_T:
    {
// printf("expID=%d\n", expID );
      try
      {
        ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
        if( eo->FW() != NULL )
        {
// printf("got an experiment.\n");
  // The following bit of code was snag and modified from SS_View_exp.cxx
          ThreadGroup tgrp = eo->FW()->getThreads();
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
  
            if (!atleastone)
            {
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
            int collector_count = 0;
            QListViewItem *item =
              new QListViewItem( attachCollectorsListView, pidstr, threadStatusStr );
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
              QListViewItem *item2 = new QListViewItem( item, host, m.getUniqueId());
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
      attachCollectorsListView->setColumnText( 0, tr( QString("Processes:")) );
      attachCollectorsListView->setColumnText( 1, tr( QString("Status") ) );
    }
    break;
  case  MPIRANK_T:
// Does this one make sense?
    attachCollectorsListView->setColumnText( 0, tr( QString("Ranks")) );
    attachCollectorsListView->setColumnText( 1, tr( QString("Process ID") ) );
    break;
  case  HOST_T:
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);

      if( eo->FW() != NULL )
      {
// The following bit of code was snag and modified from SS_View_exp.cxx
        ThreadGroup tgrp = eo->FW()->getThreads();
        ThreadGroup::iterator ti;
        std::vector<std::string> v;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
        {
          Thread t = *ti;
          std::string s = t.getHost();
        
          v.push_back(s);
        
        }
        std::sort(v.begin(), v.end());
        
        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        for( std::vector<string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          QListViewItem *item = new QListViewItem( attachCollectorsListView, *hi );
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
                QListViewItem *item2 =
                  new QListViewItem(item, pidstr, tidstr, collectorliststring );
              } else if( !ridstr.isEmpty() )
              {
                QListViewItem *item2 =
                  new QListViewItem(item, pidstr, ridstr, collectorliststring );
              } else
              {
                QListViewItem *item2 = 
                  new QListViewItem( item, pidstr, collectorliststring  );
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
    attachCollectorsListView->setColumnText( 1, tr( QString("N/A") ) );
    break;
  }

}


void
ManageCollectorsClass::updatePSetList()
{
  int pset_count = 0;
// printf("updatePSetList(%d) \n", expID );

  psetListView->clearSelection();

  QListViewItem *dynamic_items = new QListViewItem( psetListView, "Dynamic Process Set", "N/A");
  QListViewItem *user_items = new QListViewItem( psetListView, "User Defined Process Set", "N/A");


  QString pset_name = QString::null;

{ // For each host, create a dynamic collector 
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);

      if( eo->FW() != NULL )
      {
// The following bit of code was snag and modified from SS_View_exp.cxx
        ThreadGroup tgrp = eo->FW()->getThreads();
        ThreadGroup::iterator ti;
        std::vector<std::string> v;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
        {
          Thread t = *ti;
          std::string s = t.getHost();
        
          v.push_back(s);
        
        }
        std::sort(v.begin(), v.end());
        
        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        for( std::vector<string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          pset_name = QString("pset%1").arg(pset_count++);
          QListViewItem *item = new QListViewItem( dynamic_items, pset_name, *hi );
// printf("hi=(%s)\n", hi->c_str() );
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
                QListViewItem *item2 =
                  new QListViewItem(item, pidstr, tidstr, collectorliststring );
              } else if( !ridstr.isEmpty() )
              {
                QListViewItem *item2 =
                  new QListViewItem(item, pidstr, ridstr, collectorliststring );
              } else
              {
                QListViewItem *item2 = 
                  new QListViewItem( item, pidstr, collectorliststring  );
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

StatusStruct statusStruct;


      try
      {
        ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  
        if( eo->FW() != NULL )
        {
// printf("got an experiment.\n");
  // The following bit of code was snag and modified from SS_View_exp.cxx
          ThreadGroup tgrp = eo->FW()->getThreads();
          ThreadGroup::iterator ti;
          bool atleastone = false;
          for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
          {
            Thread t = *ti;
            std::string host = t.getHost();
            pid_t pid = t.getProcessId();
statusStruct.host = QString(host.c_str());
statusStruct.pid = QString("%1").arg(pid);

            // Add some status to each thread.
            QString threadStatusStr;
            switch( t.getState() )
            {
              case Thread::Disconnected:
                threadStatusStr = "Disconnected";
statusStruct.status = threadStatusStr;
statusDisconnectedList.push_back(statusStruct);
                break;
              case Thread::Connecting:
                threadStatusStr = "Connecting";
statusStruct.status = threadStatusStr;
statusConnectingList.push_back(statusStruct);
                break;
                break;
              case Thread::Nonexistent:
                threadStatusStr = "Nonexistent";
statusStruct.status = threadStatusStr;
statusNonexistentList.push_back(statusStruct);
                break;
              case Thread::Running:
                threadStatusStr = "Running";
statusStruct.status = threadStatusStr;
statusRunningList.push_back(statusStruct);
                break;
              case Thread::Suspended:
                threadStatusStr = "Suspended";
statusStruct.status = threadStatusStr;
statusSuspendedList.push_back(statusStruct);
                break;
              case Thread::Terminated:
                threadStatusStr = "Terminate";
statusStruct.status = threadStatusStr;
statusTerminatedList.push_back(statusStruct);
                break;
              default:
                threadStatusStr = "Unknown";
statusStruct.status = threadStatusStr;
statusUnknownList.push_back(statusStruct);
                break;
            }
  
            if (!atleastone)
            {
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
            int collector_count = 0;
/*
            QListViewItem *item =
              new QListViewItem( attachCollectorsListView, pidstr, threadStatusStr );
*/
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
/*
              QListViewItem *item2 = new QListViewItem( item, host, m.getUniqueId());
*/
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
    QValueList<StatusStruct>::iterator vi = statusDisconnectedList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *disconnected_items = new QListViewItem( dynamic_items, pset_name, "Disconnected" );
    for( ;vi != statusDisconnectedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      printf("ss.status=(%s)\n", ss.status.ascii() );
      printf("ss.host=(%s)\n", ss.host.ascii() );
      printf("ss.pid=(%s)\n", ss.pid.ascii() );
      new QListViewItem( disconnected_items, ss.host, ss.pid);
    }
  }
  // Put out the Connecting Dynamic pset (if there is one.)
  if( statusConnectingList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusConnectingList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Connecting" );
    for( ;vi != statusConnectingList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }
  // Put out the Nonexistent Dynamic pset (if there is one.)
  if( statusNonexistentList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusNonexistentList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Nonexistent" );
    for( ;vi != statusNonexistentList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }
  // Put out the Running Dynamic pset (if there is one.)
  if( statusRunningList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusRunningList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Running" );
    for( ;vi != statusRunningList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }
  // Put out the Suspended Dynamic pset (if there is one.)
  if( statusSuspendedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusSuspendedList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Suspended" );
    for( ;vi != statusSuspendedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }
  // Put out the status Terminated Dynamic pset (if there is one.)
  if( statusTerminatedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusTerminatedList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Terminated" );
    for( ;vi != statusTerminatedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }
  // Put out the Unknown Dynamic pset (if there is one.)
  if( statusUnknownList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusUnknownList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    QListViewItem *items = new QListViewItem( dynamic_items, pset_name, "Unknown" );
    for( ;vi != statusUnknownList.end(); vi++)
    {
      StatusStruct ss = *vi;
      new QListViewItem( items, ss.host, ss.pid);
    }
  }

}

}

void
ManageCollectorsClass::detachSelected()
{
// printf("detachSelected\n");

QListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  if( selectedItem )
  {
    QString ret_value = selectedItem->text(0);
// printf("detach = (%s)\n", ret_value.ascii() );

    QString collector_name = selectedItem->text(0);
    QString command;
    command = QString("expDetach -x %1 %2").arg(expID).arg(collector_name);
// printf("command=(%s)\n", command.ascii() );
QMessageBox::information( this, tr("Under Construction:"), tr("This feature currently under construction.\nIt will eventuall do a:\n%1").arg(command), QMessageBox::Ok );
return;

    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
    }
  } else
  {
    QMessageBox::information( this, tr("Detach Collector Info:"), tr("You need to select a collector first."), QMessageBox::Ok );
  }
  updateAttachedList();
  updatePSetList();
}

void
ManageCollectorsClass::disableSelected()
{
// printf("disableSelected\n");
QListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  if( selectedItem )
  {
    QString ret_value = selectedItem->text(0);
// printf("disable = (%s)\n", ret_value.ascii() );


    QString collector_name = selectedItem->text(0);
    QString command;
    command = QString("expDisable -x %1 %2").arg(expID).arg(collector_name);
// printf("command=(%s)\n", command.ascii() );
QMessageBox::information( this, tr("Under Construction:"), tr("This feature currently under construction.\nIt will eventuall do a:\n%1").arg(command), QMessageBox::Ok );
return;
    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
    }

  } else
  {
    QMessageBox::information( this, tr("Detach Collector Info:"), tr("You need to select a collector first."), QMessageBox::Ok );
  }
  updateAttachedList();
  updatePSetList();
}


void
ManageCollectorsClass::enableSelected()
{
// printf("enableSelected\n");
QListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  if( selectedItem )
  {
    QString ret_value = selectedItem->text(0);
// printf("enableSelected = (%s)\n", ret_value.ascii() );


    QString collector_name = selectedItem->text(0);
    QString command;
    command = QString("expEnable -x %1 %2").arg(expID).arg(collector_name);
// printf("command=(%s)\n", command.ascii() );
QMessageBox::information( this, tr("Under Construction:"), tr("This feature currently under construction.\nIt will eventuall do a:\n%1").arg(command), QMessageBox::Ok );
return;
    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
    }

  } else
  {
    QMessageBox::information( this, tr("Detach Collector Info:"), tr("You need to select a collector first."), QMessageBox::Ok );
  }
  updateAttachedList();
  updatePSetList();
}

void
ManageCollectorsClass::attachProcessSelected()
{
// printf("addProcessSelected\n");
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
// printf("A: command=(%s)\n", command.ascii() );

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
    
  if( eo->FW() != NULL )
  {
    UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
    p->broadcast((char *)msg, GROUP_T);
  }
  
  updateAttachedList();
  updatePSetList();
}

void
ManageCollectorsClass::focusOnProcessSelected(QListViewItem *item)
{
printf("focusOnProcessSelected() listView=0x%x attachCollectorsListView=0x%x psetListView=0x%x\n", item->listView(), attachCollectorsListView, psetListView );
 
  if( item->listView() == attachCollectorsListView )
  {
    focusOnProcessSelected();
    psetListView->clearSelection();
  } else
  {
    focusOnPSetSelected();
    attachCollectorsListView->clearSelection();
  }
}



void
ManageCollectorsClass::focusOnProcessSelected()
{
printf("ManageCollectorsClass::focusOnProcessSelected() entered.\n");
  QString host_name = QString::null;
  FocusObject *msg = NULL;

QListViewItem *selectedItem = NULL;
{
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
}

  // If nothing is selected in the left side, look to the right...
  if( selectedItem == NULL )
  {
    return;
  }

printf("selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
printf("selectedItem->text(1) =(%s)\n", selectedItem->text(1).ascii() );

  QString pid_name = QString::null;
  QString pidString = QString::null;
 
  QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
  while( it.current() )
  {
    QListViewItem *lvi = (QListViewItem *)it.current();
    if( dialogSortType == PID_T )
    {
// printf("PID_T: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
      host_name = QString::null;
      pid_name = QString::null;
      if( lvi->firstChild() )
      {
        // Host name
        host_name = lvi->firstChild()->text(0);
        pid_name = lvi->text(0);
      } else if( lvi->parent() )
      {
        host_name = lvi->text(0);
        pid_name = lvi->parent()->text(0);
      } else
      {
        pid_name = lvi->text(0);
      }
      if( msg == NULL )
      {
        msg = new FocusObject(expID,  host_name, pidString, TRUE);
      }
      std::pair<std::string, std::string> p(host_name,pid_name);
      msg->host_pid_vector.push_back( p );
    } else if( dialogSortType == HOST_T )
    {
//printf("HOST_T: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
      pid_name = QString::null;
      host_name = QString::null;
      if( lvi->parent() == NULL )
      {
        ++it;
        continue;
      }
      if( msg == NULL )
      {
        msg = new FocusObject(expID,  NULL, NULL, TRUE);
      }
      pid_name = lvi->text(0);
      if( lvi->parent() )
      {
        host_name = lvi->parent()->text(0);
      }
//printf("host_name=%s pid_name=%s\n", host_name.ascii(), pid_name.ascii() );

      std::pair<std::string, std::string> p(host_name,pid_name);
      msg->host_pid_vector.push_back( p );
    }
    ++it;
  }


  if( !msg || msg->host_pid_vector.size() == 0 )
  {
    QMessageBox::information( this, tr("Error process selection:"), tr("Unable to focus: No processes selected."), QMessageBox::Ok );
    if( msg )
    {
      delete msg;
    }
    return;
  }



// printf("host_name=(%s) pidString=(%s)\n", host_name.ascii(), pidString.ascii() );

// printf("focus the StatsPanel...\n");
  if( p->broadcast((char *)msg, NEAREST_T) == 0 )
  {
// printf("No StatsPanel.   Make one...\n");
    char *panel_type = "Stats Panel";
    PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    Panel *sp = p->getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
    delete ao;
    if( sp != NULL )
    {
      UpdateObject *msg =
        new UpdateObject((void *)Find_Experiment_Object((EXPID)expID), expID, "pcsamp", 1);
      sp->listener( (void *)msg );
    }
// msg->print();
      sp->listener((void *)msg);
  }

}

void
ManageCollectorsClass::focusOnPSetSelected()
{
printf("ManageCollectorsClass::focusOnPSetSelected() entered.\n");

  QString pid_name = QString::null;
  QString pidString = QString::null;
 
  QListViewItemIterator it(psetListView, QListViewItemIterator::Selected);
  while( it.current() )
  {
    QListViewItem *lvi = (QListViewItem *)it.current();
printf("PSetSelection: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
printf("lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
printf("lvi->text(1) =(%s)\n", lvi->text(1).ascii() );

    ++it;
  }
}

void
ManageCollectorsClass::updatePanel()
{
// printf("ManageCollectorsClass::updatePanel()\n");

  updateAttachedList();

  updatePSetList();
}


void
ManageCollectorsClass::loadProgramSelected()
{
// printf("ManageCollectorsClass::loadProgramSelected()\n");
  mw->executableName = QString::null;
  mw->argsStr = QString::null;
  mw->loadNewProgram();
  QString executableNameStr = mw->executableName;
  if( !mw->executableName.isEmpty() )
  {
// printf("ManageCollectorsClass::loadProgramSelected() executableName=%s\n", mw->executableName.ascii() );
    executableNameStr = mw->executableName;
    QString command =
      QString("expAttach -x %1 -f \"%2 %3\"").arg(expID).arg(executableNameStr).arg(mw->argsStr);

// printf("command=(%s)\n", command.ascii() );
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
    
// printf("Send out update?\n");
    if( eo->FW() != NULL )
    {
// printf("Yes!  Send out update?\n");
      UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
      p->broadcast((char *)msg, GROUP_T);
    }
  
  }

  updateAttachedList();
  updatePSetList();
}

void
ManageCollectorsClass::paramSelected(int val)
{
//  printf("paramSelected val=%d\n", val);
//  printf("paramSelected val=%s\n", QString("%1").arg(val).ascii() );
//  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
QListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  QString param_text = QString::null;
  if( selectedItem )
  {
//printf("selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
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
// printf("paramSelected collector_name=(%s)\n", collector_name.ascii());
// printf("paramSelected param_name=(%s)\n", param_name.ascii());
// printf("paramSelected param_value=(%s)\n", param_value.ascii());
// printf("paramSelected param_value=(%u)\n", param_value.toUInt() );
// Modify the parameter....
    bool ok;
//    int res = QInputDialog::getInteger(QString("Set %1 : %2").arg(collector_name).arg(param_name), QString("New Value:"), param_value.toUInt(), 0, 9999999, 10, &ok, this);
    QString res = QInputDialog::getText(QString("Set %1 : %2").arg(collector_name).arg(param_name), QString("New Value:"), QLineEdit::Normal, param_value, &ok, this);
    if( ok )
    {
      QString command;
// printf("colletor_name=(%s)\n", collector_name.ascii() );
      command = QString("expSetParam -x %1 %2::%3=%4").arg(expID).arg(collector_name).arg(param_name).arg(res);
// printf("command=(%s)\n", command.ascii() );
      if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
      {
        QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
      }
    } else
    {
      printf("user pressed cancel.\n");
    }
    
  }
  updateAttachedList();
}



void
ManageCollectorsClass::attachCollectorSelected(int val)
{
// printf("attachCollectorSelected(val=%d)\n", val);
//  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
QListViewItem *selectedItem = NULL;
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
  QString param_text = QString::null;
  QString collector_name = QString::null;
  QString target_name = QString::null;
  if( selectedItem )
  {
// printf("selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
    if( dialogSortType == COLLECTOR_T )
    {
      if( selectedItem->parent() )
      {
        target_name = selectedItem->text(1);
      } else
      {
// printf("Can't add a collector from a selected collector.\n");
        QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to add a collector from a selected collector.\nUnselect the highlighted selector first."), QMessageBox::Ok );
        return; }
    } else if( dialogSortType == PID_T )
    {
      if( selectedItem->parent() )
      {
        target_name = selectedItem->parent()->text(0);
      } else
      {
        target_name = selectedItem->text(0);
      }
    }
  }
// printf("target_name =(%s)\n", target_name.isEmpty() ? "" : target_name.ascii() );

  if( collectorPopupMenu != NULL )
  {
// printf("Get the collector name from the popup menu.\n");
    collector_name = collectorPopupMenu->text(val);
  } else
  {
// printf("Get the collector name from the file menu.\n");
    collector_name = collectorMenu->text(val);
  }
// printf("collector_name =(%s)\n", collector_name.isEmpty() ? "" : collector_name.ascii() );

  QString command;
  command = QString("expAttach -x %1 %2 %3").arg(expID).arg(target_name).arg(collector_name);

// printf("command=(%s)\n", command.ascii() );

  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }

  updateAttachedList();
}

void
ManageCollectorsClass::fileCollectorAboutToShowSelected()
{
// printf("fileCollectorAboutToShowSelected()\n");
  if( collectorPopupMenu != NULL )
  {
    delete collectorPopupMenu;
  }
  collectorPopupMenu = NULL;
}

void
ManageCollectorsClass::sortByProcess()
{
// printf("sortByProcess\n");
  dialogSortType = PID_T;


// printf("attachCollectorsListView->columnText(0) = (%s)\n", attachCollectorsListView->columnText(1).ascii() );
  updateAttachedList();
}

void
ManageCollectorsClass::sortByCollector()
{
// printf("sortByCollector\n");
  dialogSortType = COLLECTOR_T;

  updateAttachedList();
}

void
ManageCollectorsClass::sortByHost()
{
// printf("sortByHost\n");
  dialogSortType = HOST_T;

  updateAttachedList();
}

void
ManageCollectorsClass::sortByMPIRank()
{
// printf("sortByMPIRank\n");
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

bool
ManageCollectorsClass::menu(QPopupMenu* contextMenu)
{
printf("ManageCollectorsClass::menu(0x%x) entered.\n", contextMenu);

  bool selectable = TRUE;
  bool leftSide = TRUE;

QListViewItem *selectedItem = NULL;
  if( attachCollectorsListView->hasMouse() )
  {
QListViewItemIterator it(attachCollectorsListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
if( selectedItem )
{
  printf("LEFTSIDE has selectedITem\n");
}
    psetListView->clearSelection();
  } else
  {
    attachCollectorsListView->clearSelection();
    leftSide = FALSE;

    // If this is a root node, there's no selection available.  Ignore..
QListViewItemIterator it(psetListView, QListViewItemIterator::Selected);
while( it.current() )
{
  selectedItem = (QListViewItem *)it.current();
  break;
}
    if( selectedItem->parent() == NULL )
    {
      psetListView->clearSelection();
      selectable = FALSE;
    } else
    {
printf("Right side has selected item\n");
    }
  }


  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "_updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Update Panel...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Attempt to update this panel's display with fresh data.") );


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

  qaction = new QAction( this,  "focusOnProcess");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Focus on Process(es)...") );
if( leftSide == TRUE ) 
{
printf("LEFT SIDE MENU\n");
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnProcessSelected() ) );
} else
{
printf("RIGHT SIDE MENU\n");
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnPSetSelected() ) );
}
  qaction->setStatusTip( tr("Opens dialog box to attach to running process.") );

  // If we can't select this item, leave it here, but disable it.
  if( !selectable ) 
  {
    qaction->setEnabled(FALSE);
  }


  collectorMenu = new QPopupMenu(contextMenu);
  connect( collectorMenu, SIGNAL( activated( int ) ),
                     this, SLOT( attachCollectorSelected( int ) ) );
  connect( collectorMenu, SIGNAL( aboutToShow() ),
                     this, SLOT( fileCollectorAboutToShowSelected( ) ) );

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
// printf("Add item (%s)\n", collector_name.c_str() );
    }
  }
// printf("A: size =(%d) \n", list_of_collectors.size() );
  
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

  qaction = new QAction( this,  "detachCollector");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Detach Collector") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( detachSelected() ) );
  qaction->setStatusTip( tr("Detach the selected (highlighted) collector from the experiment.") );

  qaction = new QAction( this,  "enableCollector");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Enable Collector") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( enableSelected() ) );
  qaction->setStatusTip( tr("Enable the selected (highlighted) collector from the experiment.") );

  qaction = new QAction( this,  "disableCollector");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Disable Collector") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( disableSelected() ) );
  qaction->setStatusTip( tr("Disable the selected (highlighted) collector from the experiment.") );


  return( TRUE );
}
