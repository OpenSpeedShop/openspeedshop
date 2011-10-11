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

// Debug Flag
//#define DEBUG_COMPARE 1
//

#include "debug.hxx"

#include "CompareSet.hxx"
#include "ColumnSet.hxx"
#include "CustomizeClass.hxx"

#include "CompareProcessesDialog.hxx"

#include <qtoolbox.h>
#include <qtooltip.h>
#include <qtabwidget.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qheader.h>
#include <qstring.h>

CompareSet::CompareSet(QToolBox *csetTB, CustomizeClass *cc ) : QObject()
{
#ifdef DEBUG_COMPARE
 printf("CompareSet::CompareSet() constructor called.\n");
#endif
//  tcnt = 0;
  tcnt = 1;
  compareClass = cc;

//  name = QString("cset%1").arg(compareClass->ccnt);
  name = QString("Generate Customized Stats Panel.");

  tabWidget = new QTabWidget(compareClass, "tabWidget");

  compareClass->addNewColumn(this);

  csetTB->addItem(tabWidget, name );

  compareClass->ccnt++;

  connect(tabWidget, SIGNAL( currentChanged(QWidget *)), this, SLOT( currentChanged(QWidget *) ) );
}

void
CompareSet::updateInfo()
{
#ifdef DEBUG_COMPARE
 printf("CompareSet::updateInfo() entered\n");
#endif
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updateInfo(), attempt to delete (%s)'s info\n", cs->name.ascii() );
#endif

    cs->updateInfo();
    ++it;
  }
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareSet::~CompareSet()
{
   // Destroy the list of column information.
#ifdef DEBUG_COMPARE
 printf("Destroy the list of column information.\n");
#endif
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
#ifdef DEBUG_COMPARE
 printf("attempt to delete (%s)'s info\n", cs->name.ascii() );
#endif

    delete(cs);
    ++it;
  }

#ifdef DEBUG_COMPARE
 printf("Finished cleaning up the CompareSet\n");
#endif
  columnSetList.clear();
}

void
CompareSet::currentChanged( QWidget *tab )
{
#ifdef DEBUG_COMPARE
  printf("CompareSet::currentChanged() called.\n");
#endif
  setNewFocus(tab);
}

void
CompareSet::setNewFocus(QWidget *tab)
{
#ifdef DEBUG_COMPARE
  printf("CompareSet::setNewFocus() called.\n");
#endif
  if( !tab )
  {
// find the current tab... 
    tab = tabWidget->currentPage();
  }

  CompareSet *compareSet = this;
  ColumnSetList::Iterator it;
  if( compareSet )
  {
#ifdef DEBUG_COMPARE
 printf("CompareSet::setNewFocus(), (%s)'s info\n", compareSet->name.ascii() );
#endif
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
#ifdef DEBUG_COMPARE
 printf("Is it? columnSet->name=(%s) tabWidget->tabLabel()=(%s)\n", columnSet->name.ascii(), tabWidget->tabLabel(tab).ascii()  );
#endif
      if( columnSet->name == tabWidget->tabLabel(tab) )
      {
        int expID = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());
#ifdef DEBUG_COMPARE
 printf("\t: CompareSet (%s)'s info\n", compareSet->name.ascii() );
 printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
 printf("\t\t: expID=(%d)\n", expID );
 printf("\t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
 printf("\t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
 printf("\t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
#endif


         if( compareSet->compareClass->dialog )
         {
#ifdef DEBUG_COMPARE
 printf("call dialogs updateFocus\n");
#endif
           if( compareSet->compareClass->dialog->updateFocus(expID, columnSet->lv) == TRUE )
           {
             compareSet->updatePSetList();
           }
           QString headerStr = QString("Modify Compare Set %1: Column %2").arg(compareSet->name).arg(columnSet->name);
           compareSet->compareClass->dialog->headerLabel->setText(headerStr);
         }
         break;
       }
    }
  }

}



// This routine is the same/similar to the one in ManageProcessesPanel.
// At some point this routine and the one in ManageProcesssesPanel should
// be shared.
void
CompareSet::updatePSetList()
{

#ifdef DEBUG_COMPARE
  printf("CompareSet::updatePSetList() called.\n");
#endif

  if( !compareClass->dialog ) {
    return;
  }

  int expID = compareClass->dialog->expID;

  MPListView *psetListView = compareClass->dialog->availableProcessesListView;

  QStringList *psl = &compareClass->dialog->psetNameList;
  int pset_count = 0;

#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList(%d) \n", expID );
#endif

  psetListView->clearSelection();

  psetListView->clear();


  QListViewItem *dynamic_items = new MPListViewItem( psetListView, DPS);
dynamic_items->setOpen(TRUE);

  QString pset_name = QString::null;

#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, for each host, create a dynamic collector\n");
#endif
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo->FW() != NULL )
    {
// The following bit of code was snag and modified from SS_View_exp.cxx
      ThreadGroup tgrp = eo->FW()->getThreads();
      ThreadGroup::iterator ti;
      std::vector<std::string> v;
      pset_name = QString("All");
      psl->append("All");
      MPListViewItem *item = new MPListViewItem( dynamic_items, pset_name );
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
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, calling std::sort\n");
#endif
      std::sort(v.begin(), v.end());

      pset_name = QString("Hosts");
      MPListViewItem *host_items = new MPListViewItem( dynamic_items, pset_name );
      DescriptionClassObject *host_dco = new DescriptionClassObject(TRUE, pset_name);
      host_items->descriptionClassObject = host_dco;
      
      std::vector<std::string>::iterator e 
                      = unique(v.begin(), v.end());
      for( std::vector<std::string>::iterator hi = v.begin(); hi != e; hi++ ) 
      {
        pset_name = QString(*hi);
        MPListViewItem *item = new MPListViewItem( host_items, pset_name );
        DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
        item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
// printf("CompareSet::updatePSetList, hi=(%s)\n", hi->c_str() );
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
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, A: pidstr=(%s)\n", pidstr.ascii() );
 printf("CompareSet::updatePSetList, A: tidstr=(%s)\n", tidstr.ascii() );
 printf("CompareSet::updatePSetList, A: ridstr=(%s)\n", ridstr.ascii() );
#endif
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
            if( !ridstr.isEmpty() )
            {
              MPListViewItem *item2 =
                new MPListViewItem(item, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring );
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
              item2->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, A: Put ridstr out: \n");
 dco->Print();
#endif
            } else if( !tidstr.isEmpty() )
            {
              MPListViewItem *item2 =
                new MPListViewItem(item, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring );
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
              item2->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, A: Put tidstr out: \n");
 dco->Print();
#endif
            } else if( !pidstr.isEmpty() )
            {
              MPListViewItem *item2 = 
                new MPListViewItem( item, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring );
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
              item2->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, A: Put pidstr out: \n");
 dco->Print();
#endif
            } else
            {
              MPListViewItem *item2 = 
                new MPListViewItem( item, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring  );
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, ridstr, tidstr, collectorliststring);
              item2->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, A: Put \"other\" out: \n");
 dco->Print();
#endif
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
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, got an experiment.\n");
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
        statusStruct.host = QString(host.c_str());
        statusStruct.pid = QString("%1").arg(pid);
        std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
        QString tidstr = QString::null;
        if (pthread.first)
        {
          tidstr = QString("%1").arg(pthread.second);
          statusStruct.tid = tidstr;
        }
        std::pair<bool, int> rank = t.getMPIRank();
        QString ridstr = QString::null;
        if (rank.first)
        {
          ridstr = QString("%1").arg(rank.second);
          statusStruct.rid = ridstr;
        }
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: pid=(%s)\n", statusStruct.pid.ascii() );
 printf("CompareSet::updatePSetList, B: tid=(%s)\n", statusStruct.tid.ascii() );
 printf("CompareSet::updatePSetList, B: rid=(%s)\n", statusStruct.rid.ascii() );
#endif

        // Add some status to each thread.
        QString threadStatusStr;
        switch( t.getState() )
        {
          case Thread::Disconnected:
            threadStatusStr = "Disconnected";
            statusStruct.status = threadStatusStr;
            statusDisconnectedList.push_back(statusStruct);
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, push_back: pid=(%s)\n", statusStruct.pid.ascii() );
 printf("CompareSet::updatePSetList, push_back: tid=(%s)\n", statusStruct.tid.ascii() );
 printf("CompareSet::updatePSetList, push_back: rid=(%s)\n", statusStruct.rid.ascii() );
#endif
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
    QValueList<StatusStruct>::iterator vi = statusDisconnectedList.begin();
    pset_name = QString("Disconnected");
    psl->append(pset_name);

    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, statusDisconnectedList: \n");
#endif
    items->descriptionClassObject = dco;
    for( ;vi != statusDisconnectedList.end(); vi++)
    {
      StatusStruct ss = *vi;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, pset_name=(%s)\n", pset_name.ascii() );
 printf("CompareSet::updatePSetList, ss.status=(%s)\n", ss.status.ascii() );
 printf("CompareSet::updatePSetList, ss.host=(%s)\n", ss.host.ascii() );
 printf("CompareSet::updatePSetList, ss.pid=(%s)\n", ss.pid.ascii() );
 printf("CompareSet::updatePSetList, ss.rid=(%s)\n", ss.rid.ascii() );
 printf("CompareSet::updatePSetList, ss.tid=(%s)\n", ss.tid.ascii() );
#endif

      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#if 0
printf("B: Put \"disconnected\" out: \n");
item->descriptionClassObject->Print();
printf("...");
printf("dco->pid_name=(%s)\n", dco->pid_name.ascii() );
printf("...");
#endif // 0
    }
  }
  // Put out the Connecting Dynamic pset (if there is one.)
  if( statusConnectingList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusConnectingList.begin();
    pset_name = QString("Connecting");
    psl->append(pset_name);
//    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Connecting" );
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusConnectingList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"connecting\" out: \n");
 dco->Print();
#endif
    }
  }
  // Put out the Nonexistent Dynamic pset (if there is one.)
  if( statusNonexistentList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusNonexistentList.begin();
    pset_name = QString("Non Existent");
    psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusNonexistentList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"nonexistent\" out: \n");
 dco->Print();
#endif
    }
  }
  // Put out the Running Dynamic pset (if there is one.)
  if( statusRunningList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusRunningList.begin();
    pset_name = QString("Running");
    psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusRunningList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"running\" out: \n");
 dco->Print();
#endif
    }
  }
  // Put out the Suspended Dynamic pset (if there is one.)
  if( statusSuspendedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusSuspendedList.begin();
    pset_name = QString("Suspended");
    psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusSuspendedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"suspended\" out: \n");
 dco->Print();
#endif
    }
  }
  // Put out the status Terminated Dynamic pset (if there is one.)
  if( statusTerminatedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusTerminatedList.begin();
    pset_name = QString("Terminated");
    psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusTerminatedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"terminated\" out: \n");
 dco->Print();
#endif
    }
  }
  // Put out the Unknown Dynamic pset (if there is one.)
  if( statusUnknownList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusUnknownList.begin();
    pset_name = QString("Unknown");
    psl->append(pset_name);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusUnknownList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.host, ss.pid, ss.rid, ss.tid);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid, ss.rid, ss.tid );
      item->descriptionClassObject = dco;
#ifdef DEBUG_COMPARE
 printf("CompareSet::updatePSetList, B: Put \"unknown\" out: \n");
 dco->Print();
#endif
    }
  }
}


void
CompareSet::relabel()
{
  int i = 0;
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); ++it )
  {
    ColumnSet *columnSet = (ColumnSet *)*it;
    QWidget *thisTab = tabWidget->page(i);
#ifdef DEBUG_COMPARE
// printf("OLD: columnSet->name=(%s) tabWidget->tabLabel()=(%s)\n", columnSet->name.ascii(), tabWidget->tabLabel(thisTab).ascii()  );
#endif

    QString header = QString("Column #%1").arg(i+1);
    columnSet->name = header;
    tabWidget->setTabLabel(thisTab, header);
#ifdef DEBUG_COMPARE
 printf("CompareSet::relabel, NEW: columnSet->name=(%s) tabWidget->tabLabel()=(%s)\n", columnSet->name.ascii(), tabWidget->tabLabel(thisTab).ascii()  );
#endif
    i++;
  }

  tcnt = i+1;
}

