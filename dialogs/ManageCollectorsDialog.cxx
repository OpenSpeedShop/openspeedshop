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

#include "SS_Input_Manager.hxx"

ManageCollectorsDialog::ManageCollectorsDialog( QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsDialog::ManageCollectorsDialog() constructor called.\n");
  
  popupMenu = NULL;
  paramMenu = NULL;
  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;
  clo = NULL;
  expID = exp_id;
  if ( !name ) setName( "ManageCollectorsDialog" );

  setSizeGripEnabled( TRUE );
  ManageCollectorsDialogLayout = new QVBoxLayout( this, 11, 6, "ManageCollectorsDialogLayout"); 


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

  updateAttachedCollectorsList();
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
  setCaption( tr( "ManageCollectorsDialog" ) );
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
ManageCollectorsDialog::updateAttachedCollectorsList()
{
  CollectorEntry *ce = NULL;
  char entry_buffer[1024];
  if( clo )
  {
    delete(clo);
  }

  clo = new CollectorListObject(expID);

  attachCollectorsListView->clear();
  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
    QListViewItem *item = new QListViewItem( attachCollectorsListView, ce->name, ce->short_name );
#ifdef OLDWAY
    for( CollectorParameterEntryList::Iterator pit = ce->paramList.begin();
         pit != ce->paramList.end();  pit++)
    {
      CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
      QListViewItem *item2 = new QListViewItem( item, cpe->name, cpe->param_value );
    }
#else // OLDWAY
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
// #ifdef HAVE_MPI
          std::pair<bool, int> rank = t.getMPIRank();
          QString ridstr = QString::null;
          if (rank.first)
          {
            ridstr = QString("%1").arg(rank.second);
          }
// #endif
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
                QListViewItem *item2 = new QListViewItem( item, host, pidstr, tidstr );
              } else if( !ridstr.isEmpty() )
              {
                QListViewItem *item2 = new QListViewItem( item, host, pidstr, ridstr );
              } else
              {
                QListViewItem *item2 = new QListViewItem( item, host, pidstr );
              }
            }
          }
        }
      }
    }
    catch(const std::exception& error)
    {
printf("Tossed an exception looking up pids.\n");
      return;
    }
#endif // OLDWAY
  }
}

void ManageCollectorsDialog::availableCollectorsComboBoxActivated()
{
//    updateAttachedCollectorsList();
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
 
  if( attachCollectorsListView->selectedItem() )
  {
    if( attachCollectorsListView->selectedItem()->parent() == NULL ) // it's a root node.
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
    } else
    {
    }
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
  updateAttachedCollectorsList();
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
  updateAttachedCollectorsList();
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

  updateAttachedCollectorsList();
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
    QString command = QString("expAttach -x %1 -f %2").arg(expID).arg(executableNameStr);

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
  updateAttachedCollectorsList();
}

void
ManageCollectorsDialog::paramSelected(int val)
{
//  printf("paramSelected\n");
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
  QString param_text = QString::null;
  if( selectedItem )
  {
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
  updateAttachedCollectorsList();
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

  updateAttachedCollectorsList();
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
