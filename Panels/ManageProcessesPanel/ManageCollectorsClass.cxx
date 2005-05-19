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

#include "SS_Input_Manager.hxx"
#include "PanelContainer.hxx"

ManageCollectorsClass::ManageCollectorsClass( PanelContainer *pc, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QWidget( parent, name )
{
//  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");
  dprintf("ManageCollectorsClass::ManageCollectorsClass() constructor called.\n");
  
  dialogSortType = COLLECTOR_T;
  popupMenu = NULL;
  paramMenu = NULL;
  collectorPopupMenu = NULL;
  mw = (OpenSpeedshop *)pc->getMainWindow();
  cli = pc->getMainWindow()->cli;
  clo = NULL;
  expID = exp_id;
  list_of_collectors.clear();

  if ( !name ) setName( "ManageCollectorsAndProcessesDialog" );

  ManageCollectorsClassLayout = new QVBoxLayout( this, 1, 1, "ManageCollectorsClassLayout"); 

  attachCollectorsListView = new QListView( this, "attachCollectorsListView" );
  attachCollectorsListView->addColumn( 
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->addColumn( tr( QString("Name") ) );
  attachCollectorsListView->setSelectionMode( QListView::Single );
  attachCollectorsListView->setAllColumnsShowFocus( TRUE );
  attachCollectorsListView->setShowSortIndicator( TRUE );
  attachCollectorsListView->setRootIsDecorated(TRUE);

  ManageCollectorsClassLayout->addWidget( attachCollectorsListView );

  languageChange();
  connect(attachCollectorsListView, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint& , int ) ), this, SLOT( contextMenuRequested( QListViewItem *, const QPoint &, int ) ) );

  updateAttachedList();
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
  setCaption( tr( "Manage Collectors and Processes Dialog" ) );
  QString command;

  command = QString("listTypes all");
// printf("command=(%s)\n", command.ascii() );
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), 
         &list_of_collectors, FALSE ) )
  {
    QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  }

}

QString
ManageCollectorsClass::selectedCollectors()
{
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
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
    }
      break;
    case PID_T:
    {
    try {
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
          QListViewItem *item =
            new QListViewItem( attachCollectorsListView, pidstr );
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
    }
    break;
  case  MPIRANK_T:
// Does this one make sense?
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
    break;
  }

}

void
ManageCollectorsClass::contextMenuRequested( QListViewItem *item, const QPoint &pos, int col)
{
  dprintf("ManageCollectorsClass::contextMenuRequested() entered.\n");
  QString field_name = QString::null;
  if( item )
  {
// printf("%s %s\n", attachCollectorsListView->selectedItem()->text(0).ascii(), attachCollectorsListView->selectedItem()->text(1).ascii()  );
// printf("%s %s\n", item->text(0).ascii(), item->text(1).ascii()  );
    if( dialogSortType == COLLECTOR_T )
    {
      field_name = item->text(0);
    } else
    {
// printf("%s %s\n", item->text(0).ascii(), item->text(1).ascii() );
// printf("%s %s\n", attachCollectorsListView->selectedItem()->text(0).ascii(), attachCollectorsListView->selectedItem()->text(1).ascii()  );
      field_name = item->text(1);
    }
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


  QListViewItem *selected_item = NULL;

// It may make sense to allow other SortTypes to add/delete collectors... 
// At this point only this sort type is supported.
if( dialogSortType == COLLECTOR_T || dialogSortType == PID_T || 
    dialogSortType == HOST_T )
{
  if( attachCollectorsListView->selectedItem() && 
      attachCollectorsListView->selectedItem()->parent() == NULL )
  {
    selected_item = item;
  }
  if( attachCollectorsListView->selectedItem() && 
      attachCollectorsListView->selectedItem()->parent() != NULL )
  {
    selected_item = item;
  }
// printf("dialogSortType=%d selected_item=0x%x selected_item->parent()=0x%x\n", dialogSortType, selected_item, selected_item ? selected_item->parent() : NULL );
  if( selected_item &&
     ( dialogSortType == COLLECTOR_T && selected_item->parent() == NULL ) ||
     ( dialogSortType == PID_T && selected_item->parent() != NULL ) )
  {
// printf("Here field_name=(%s)\n", !field_name.isEmpty() ? field_name.ascii() : NULL );
    CollectorEntry *ce = NULL;
    CollectorEntryList::Iterator it;
    for( it = clo->collectorEntryList.begin();
         it != clo->collectorEntryList.end();
         ++it )
    {
      ce = (CollectorEntry *)*it;
      if( field_name == ce->name )
      {
// printf("(%s): parameters are\n", ce->name.ascii() );
        CollectorParameterEntryList::Iterator pit = ce->paramList.begin();
        if( ce->paramList.size() == 1 )
        {
          CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
          popupMenu->insertItem( QString("Modify Parameter ... (%1::%2)").arg(cpe->name.ascii()).arg(cpe->param_value.ascii()), this, SLOT(paramSelected(int)) );
        } else
        {
          paramMenu = new QPopupMenu(this);
          connect( paramMenu, SIGNAL( activated( int ) ),
                     this, SLOT( paramSelected( int ) ) );
          popupMenu->insertItem("Modify Parameter", paramMenu);
          for( ;pit != ce->paramList.end();  pit++)
          {
            CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
// printf("\t%s   %s\n", cpe->name.ascii(), cpe->param_value.ascii() );
            paramMenu->insertItem(QString("%1::%2").arg(cpe->name.ascii()).arg(cpe->param_value.ascii()) );
          }
        }
        break;
      }
    }
  }
  menu(popupMenu);
}


  popupMenu->popup( pos );
}

void
ManageCollectorsClass::detachSelected()
{
// printf("detachSelected\n");

  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
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
}

void
ManageCollectorsClass::disableSelected()
{
// printf("disableSelected\n");
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
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
}


void
ManageCollectorsClass::enableSelected()
{
// printf("enableSelected\n");
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
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
    QString command = QString("expAttach -x %1 -p %2").arg(expID).arg(mw->pidStr);
QMessageBox::information( this, tr("Under Construction:"), tr("This feature currently under construction.\nIt will eventuall do a:\n%1").arg(command), QMessageBox::Ok );
return;

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

    loadTimer->stop();
    pd->hide();
  }

  updateAttachedList();
}

void
ManageCollectorsClass::attachProgramSelected()
{
  dprintf("addProgramSelected\n");
  mw->executableName = QString::null;
  mw->argsStr = QString::null;
  mw->loadNewProgram();
  QString executableNameStr = mw->executableName;
  if( !mw->executableName.isEmpty() )
  {
    executableNameStr = mw->executableName;
    QString command =
      QString("expAttach -x %1 -f \"%2 %3\"").arg(expID).arg(executableNameStr).arg(mw->argsStr);

  dprintf("command=(%s)\n", command.ascii() );
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
  delete(pd);

  }
  updateAttachedList();
}

void
ManageCollectorsClass::paramSelected(int val)
{
//  printf("paramSelected val=%d\n", val);
//  printf("paramSelected val=%s\n", QString("%1").arg(val).ascii() );
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
  QString param_text = QString::null;
  if( selectedItem )
  {
//printf("selectedItem->text(0) =(%s)\n", selectedItem->text(0).ascii() );
    QString collector_name = QString::null;
    if( selectedItem->parent() )
    {
      collector_name = selectedItem->parent()->text(0);
    } else
    {
      collector_name = selectedItem->text(0);
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
    int res = QInputDialog::getInteger(QString("Set %1 : %2").arg(collector_name).arg(param_name), QString("New Value:"), param_value.toUInt(), 0, 9999999, 10, &ok, this);
    if( ok )
    {
      QString command;
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
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
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
  attachCollectorsListView->setColumnText( 0,
    tr( QString("Processes attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("Name") ) );

  updateAttachedList();
}

void
ManageCollectorsClass::sortByCollector()
{
// printf("sortByCollector\n");
  dialogSortType = COLLECTOR_T;

  attachCollectorsListView->setColumnText( 0,
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("Name") ) );

  updateAttachedList();
}

void
ManageCollectorsClass::sortByHost()
{
// printf("sortByHost\n");
  dialogSortType = HOST_T;

  attachCollectorsListView->setColumnText( 0,
    tr( QString("Hosts associated with experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("N/A") ) );

  updateAttachedList();
}

void
ManageCollectorsClass::sortByMPIRank()
{
// printf("sortByMPIRank\n");
  dialogSortType = MPIRANK_T;

  attachCollectorsListView->setColumnText( 0,
    tr( QString("MPI ranks associated with experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("Process ID") ) );

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
  dprintf("ManageCollectorsClass::menu(0x%x) entered.\n", contextMenu);

  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "attachProgram");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Attach Program...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( attachProgramSelected() ) );
  qaction->setStatusTip( tr("Opens dialog box to load application from disk.") );

  qaction = new QAction( this,  "attachProcess");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Attach Process...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( attachProcessSelected() ) );
  qaction->setStatusTip( tr("Opens dialog box to attach to running process.") );

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
