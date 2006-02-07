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

#include "debug.hxx"

#include "CompareSet.hxx"
#include "ColumnSet.hxx"
#include "CompareClass.hxx"

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

CompareSet::CompareSet(QToolBox *csetTB, CompareClass *cc ) : QObject()
{
// printf("CompareSet::CompareSet() constructor called.\n");
  tcnt = 0;
  compareClass = cc;

  name = QString("cset%1").arg(compareClass->ccnt);

  tabWidget = new QTabWidget(compareClass, "tabWidget");

  compareClass->addNewColumn(this);

  csetTB->addItem(tabWidget, name );

  compareClass->ccnt++;

  connect(tabWidget, SIGNAL( currentChanged(QWidget *)), this, SLOT( currentChanged(QWidget *) ) );
}

void
CompareSet::updateInfo()
{
// printf("CompareSet::updateInfo() entered\n");
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
// printf("attempt to delete (%s)'s info\n", cs->name.ascii() );

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
// printf("Destroy the list of column information.\n");
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
// printf("attempt to delete (%s)'s info\n", cs->name.ascii() );

    delete(cs);
    ++it;
  }

// printf("Finished cleaning up the CompareSet\n");
  columnSetList.clear();
}

void
CompareSet::currentChanged( QWidget *tab )
{
printf("currentChanged() set the focus on a new tab...\n");


  setNewFocus(tab);

}

void
CompareSet::setNewFocus(QWidget *tab)
{
  if( !tab )
  {
// find the current tab... 
    tab = tabWidget->currentPage();
  }

  CompareSet *compareSet = this;
  if( compareSet )
  {
printf("CompareSet: (%s)'s info\n", compareSet->name.ascii() );
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
printf("Is it? columnSet->name=(%s) tabWidget->tabLabel()=(%s)\n", columnSet->name.ascii(), tabWidget->tabLabel(tab).ascii()  );
      if( columnSet->name == tabWidget->tabLabel(tab) )
      {
        int expID = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());
printf("\t: CompareSet (%s)'s info\n", compareSet->name.ascii() );
printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
printf("\t\t: expID=(%d)\n", expID );
printf("\t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
printf("\t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
printf("\t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
         if( compareSet->compareClass->dialog )
         {
           compareSet->compareClass->dialog->updateFocus(expID, compareSet->compareClass, compareSet, columnSet);
         }
         break;
       }
    }
  }

}



void
CompareSet::updatePSetList()
{
  if( !compareClass->dialog )
  {
    return;
  }

  int expID = compareClass->dialog->expID;

  MPListView *psetListView = compareClass->dialog->availableProcessesListView;
  int pset_count = 0;
printf("updatePSetList(%d) \n", expID );

  psetListView->clearSelection();

  psetListView->clear();


  QListViewItem *dynamic_items = new MPListViewItem( psetListView, DPS);
dynamic_items->setOpen(TRUE);

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
        pset_name = QString("pset%1").arg(pset_count++);
        MPListViewItem *item = new MPListViewItem( dynamic_items, pset_name, "All" );
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
        std::sort(v.begin(), v.end());
        
        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        for( std::vector<string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          pset_name = QString("pset%1").arg(pset_count++);
          MPListViewItem *item = new MPListViewItem( dynamic_items, pset_name, *hi );
          DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
          item->descriptionClassObject = dco;
printf("hi=(%s)\n", hi->c_str() );
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
printf("pidstr=(%s)\n", pidstr.ascii() );
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
                  new MPListViewItem(item, pidstr, tidstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), tidstr, collectorliststring);
                item2->descriptionClassObject = dco;
              } else if( !ridstr.isEmpty() )
              {
                MPListViewItem *item2 =
                  new MPListViewItem(item, pidstr, ridstr, collectorliststring );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), ridstr, collectorliststring);
                item2->descriptionClassObject = dco;
              } else
              {
                MPListViewItem *item2 = 
                  new MPListViewItem( item, pidstr, collectorliststring  );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, QString(host.c_str()), pidstr, collectorliststring);
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
    pset_name = QString("pset%1").arg(pset_count++);

    MPListViewItem *disconnected_items = new MPListViewItem( dynamic_items, pset_name, "Disconnected" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    disconnected_items->descriptionClassObject = dco;
    for( ;vi != statusDisconnectedList.end(); vi++)
    {
      StatusStruct ss = *vi;
//      printf("ss.status=(%s)\n", ss.status.ascii() );
//      printf("ss.host=(%s)\n", ss.host.ascii() );
//      printf("ss.pid=(%s)\n", ss.pid.ascii() );

      MPListViewItem *item = new MPListViewItem( disconnected_items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Connecting Dynamic pset (if there is one.)
  if( statusConnectingList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusConnectingList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Connecting" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusConnectingList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Nonexistent Dynamic pset (if there is one.)
  if( statusNonexistentList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusNonexistentList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Nonexistent" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusNonexistentList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Running Dynamic pset (if there is one.)
  if( statusRunningList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusRunningList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Running" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusRunningList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Suspended Dynamic pset (if there is one.)
  if( statusSuspendedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusSuspendedList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Suspended" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusSuspendedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the status Terminated Dynamic pset (if there is one.)
  if( statusTerminatedList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusTerminatedList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Terminated" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusTerminatedList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }
  // Put out the Unknown Dynamic pset (if there is one.)
  if( statusUnknownList.size() > 0 )
  {
    QValueList<StatusStruct>::iterator vi = statusUnknownList.begin();
    pset_name = QString("pset%1").arg(pset_count++);
    MPListViewItem *items = new MPListViewItem( dynamic_items, pset_name, "Unknown" );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    items->descriptionClassObject = dco;
    for( ;vi != statusUnknownList.end(); vi++)
    {
      StatusStruct ss = *vi;
      MPListViewItem *item = new MPListViewItem( items, ss.pid, ss.host);
      DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, ss.host, ss.pid );
      item->descriptionClassObject = dco;
    }
  }

}

}
