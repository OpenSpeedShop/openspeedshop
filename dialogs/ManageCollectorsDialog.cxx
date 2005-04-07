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
  

#include "ManageCollectorsDialog.hxx"

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

#include "SS_Input_Manager.hxx"

ManageCollectorsDialog::ManageCollectorsDialog( QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsDialog::ManageCollectorsDialog() constructor called.\n");
  
  dialogSortType = COLLECTOR_T;
  popupMenu = NULL;
  paramMenu = NULL;
  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;
  clo = NULL;
  expID = exp_id;
  if ( !name ) setName( "ManageCollectorsAndProcessesDialog" );

  setSizeGripEnabled( TRUE );
  ManageCollectorsDialogLayout = new QVBoxLayout( this, 1, 1, "ManageCollectorsDialogLayout"); 

 QHBoxLayout *menuLayout = new QHBoxLayout( ManageCollectorsDialogLayout );
  QPopupMenu *file = new QPopupMenu( this );
  file->insertItem( "&Attach Program...",  this, SLOT(attachProgramSelected()), CTRL+Key_L );
  file->insertItem( "&Attach Process", this, SLOT(attachProcessSelected()), CTRL+Key_A );
  file->insertItem( "Add &Collector", this, SLOT(addCollectorSelected()), CTRL+Key_C );
  file->insertItem( "&Detach Collector", this, SLOT(detachSelected()), CTRL+Key_D );
  
  QPopupMenu *view = new QPopupMenu( this );
  view->insertItem( "Sort By &Process...", this, SLOT(sortByProcess()), CTRL+Key_P );
  view->insertItem( "Sort By &Collector...", this, SLOT(sortByCollector()), CTRL+Key_C );
  view->insertItem( "Sort By &Host...", this, SLOT(sortByHost()), CTRL+Key_H );
  view->insertItem( "Sort By &MPI Rank...", this, SLOT(sortByMPIRank()), CTRL+Key_M );

  QMenuBar *menuBar = new QMenuBar(this, "menubar");
  menuBar->insertItem( "&File", file );
  menuBar->insertItem( "&View", view );


  QSpacerItem *menuSpacer = new QSpacerItem( 1, menuBar->height(), QSizePolicy::Fixed, QSizePolicy::Fixed );
  
  menuLayout->addItem( menuSpacer );
  menuLayout->addWidget( menuBar );
  
  attachCollectorsListView = new QListView( this, "attachCollectorsListView" );
  attachCollectorsListView->addColumn( 
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->addColumn( tr( QString("Name") ) );
  attachCollectorsListView->setSelectionMode( QListView::Single );
  attachCollectorsListView->setAllColumnsShowFocus( TRUE );
  attachCollectorsListView->setShowSortIndicator( TRUE );
  attachCollectorsListView->setRootIsDecorated(TRUE);

  ManageCollectorsDialogLayout->addWidget( attachCollectorsListView );

  AddCollectorLayout = new QHBoxLayout( 0, 0, 6, "AddCollectorLayout"); 

  availableCollectorsLabel = new QLabel( this, "availableCollectorsLabel" );
  AddCollectorLayout->addWidget( availableCollectorsLabel );
  availableCollectorsComboBox = new QComboBox( this, "availableCollectorsComboBox");
  availableCollectorsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, availableCollectorsComboBox->sizePolicy().hasHeightForWidth() ) );
  availableCollectorsComboBox->setEditable(TRUE);

  AddCollectorLayout->addWidget( availableCollectorsComboBox );

  addOk = new QPushButton( this, "addOk" );
  addOk->setAutoDefault( TRUE );
  AddCollectorLayout->addWidget( addOk );
  
  ManageCollectorsDialogLayout->addLayout( AddCollectorLayout );

  Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  Layout1->addWidget( buttonHelp );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  Layout1->addWidget( buttonOk );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );
  ManageCollectorsDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( availableCollectorsComboBox, SIGNAL( activated(const QString &) ), this, SLOT( availableCollectorsComboBoxActivated() ) );

  connect( addOk, SIGNAL( clicked() ), this, SLOT( addCollectorSelected() ) );

  connect(attachCollectorsListView, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint& , int ) ), this, SLOT( contextMenuRequested( QListViewItem *, const QPoint &, int ) ) );

  updateAttachedList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageCollectorsDialog::~ManageCollectorsDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsDialog::ManageCollectorsDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageCollectorsDialog::languageChange()
{
  setCaption( tr( "Manage Collectors and Processes Dialog" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  availableCollectorsLabel->setText( tr("Available Collectors:") );
  addOk->setText( tr( "&Add" ) );
  addOk->setAccel( QKeySequence( QString::null ) );
  QString command;
  command = QString("listTypes all");
// printf("command=(%s)\n", command.ascii() );
  std::list<std::string> list_of_collectors;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(), 
         &list_of_collectors, FALSE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

  if( list_of_collectors.size() > 0 ) 
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
    {
      std::string collector_name = *it;
      availableCollectorsComboBox->insertItem( collector_name.c_str() );
    }
  }
}

QString
ManageCollectorsDialog::selectedCollectors()
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
ManageCollectorsDialog::updateAttachedList()
{
// printf("updateAttachedList() dialogSortType=%d\n", dialogSortType);

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

void ManageCollectorsDialog::availableCollectorsComboBoxActivated()
{
//    updateAttachedList();
}

void
ManageCollectorsDialog::contextMenuRequested( QListViewItem *item, const QPoint &pos, int col)
{
//  printf("ManagerCollectorsDialog::createPopupMenu() entered.\n");

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

QListViewItem *selected_item = NULL;

// It may make sense to allow other SortTypes to add/delete collectors... 
// At this point only this sort type is supported.
if( dialogSortType == COLLECTOR_T )
{
  if( attachCollectorsListView->selectedItem() && 
      attachCollectorsListView->selectedItem()->parent() == NULL )
  {
    selected_item = item;
  }
}
 
  if( selected_item )
  {
    CollectorEntry *ce = NULL;
    CollectorEntryList::Iterator it;
    for( it = clo->collectorEntryList.begin();
         it != clo->collectorEntryList.end();
         ++it )
    {
      ce = (CollectorEntry *)*it;
      if( item->text(0) == ce->name )
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
          int i = 0;
          for( ;pit != ce->paramList.end();  pit++)
          {
            CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
// printf("\t%s   %s\n", cpe->name.ascii(), cpe->param_value.ascii() );
            int id = paramMenu->insertItem(QString("%1::%2").arg(cpe->name.ascii()).arg(cpe->param_value.ascii()) );
            i++;
          }
        }
        break;
      }
    }
    popupMenu->insertSeparator();
    popupMenu->insertItem("Detach...", this, SLOT(detachSelected()) );
    popupMenu->insertItem("Disable...", this, SLOT(disableSelected()) );
  }
  popupMenu->insertSeparator();
  popupMenu->insertItem("Attach Process...", this, SLOT(attachProcessSelected()) );
  popupMenu->insertItem("Attach Program...", this, SLOT(attachProgramSelected()) );


  popupMenu->popup( pos );
}

void
ManageCollectorsDialog::detachSelected()
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
    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
  }
  updateAttachedList();
}

void
ManageCollectorsDialog::disableSelected()
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
    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }

  }
  updateAttachedList();
}

void
ManageCollectorsDialog::attachProcessSelected()
{
// printf("addProcessSelected\n");
  mw->executableName = QString::null;
  mw->pidStr = QString::null;
  mw->attachNewProcess();


  if( !mw->pidStr.isEmpty() )
  {
    QString command = QString("expAttach -x %1 -p %2").arg(expID).arg(mw->pidStr);

    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();

    if( !cli->runSynchronousCLI(command.ascii()) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
  //    return;
    }

    loadTimer->stop();
    pd->hide();
  }

  updateAttachedList();
}

void
ManageCollectorsDialog::attachProgramSelected()
{
//  printf("addProgramSelected\n");
  mw->executableName = QString::null;
  mw->loadNewProgram();
  QString executableNameStr = mw->executableName;
  if( !mw->executableName.isEmpty() )
  {
    executableNameStr = mw->executableName;
    QString command =
      QString("expAttach -x %1 -f %2").arg(expID).arg(executableNameStr);

    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();

    if( !cli->runSynchronousCLI(command.ascii() ) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
  //    return;
  }

  loadTimer->stop();
  pd->hide();
  delete(pd);

  }
  updateAttachedList();
}

void
ManageCollectorsDialog::paramSelected(int val)
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
        printf("Unable to run %s command.\n", command.ascii() );
      }
    } else
    {
      printf("user pressed cancel.\n");
    }
    
  }
  updateAttachedList();
}



void
ManageCollectorsDialog::addCollectorSelected()
{
//  printf("addCollectorSelected()\n");
  QString collector_name = availableCollectorsComboBox->currentText();
//  printf("collector_name=(%s)\n", collector_name.ascii() );

  QString command;
  command = QString("expAttach -x %1 %2").arg(expID).arg(collector_name);
// printf("command=(%s)\n", command.ascii() );
  if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

  updateAttachedList();
}

void
ManageCollectorsDialog::sortByProcess()
{
// printf("sortByProcess\n");
  dialogSortType = PID_T;


printf("attachCollectorsListView->columnText(0) = (%s)\n", attachCollectorsListView->columnText(1).ascii() );
  attachCollectorsListView->setColumnText( 0,
    tr( QString("Processes attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("Name") ) );

  updateAttachedList();
}

void
ManageCollectorsDialog::sortByCollector()
{
// printf("sortByCollector\n");
  dialogSortType = COLLECTOR_T;

  attachCollectorsListView->setColumnText( 0,
    tr( QString("Collectors attached to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("Name") ) );

  updateAttachedList();
}

void
ManageCollectorsDialog::sortByHost()
{
// printf("sortByHost\n");
  dialogSortType = HOST_T;

  attachCollectorsListView->setColumnText( 0,
    tr( QString("Hosts associated with experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->setColumnText( 1, tr( QString("N/A") ) );

  updateAttachedList();
}

void
ManageCollectorsDialog::sortByMPIRank()
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
ManageCollectorsDialog::progressUpdate()
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
