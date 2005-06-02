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
  

#include "ManageProcessesDialog.hxx"

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

#include "SS_Input_Manager.hxx"

ManageProcessesDialog::ManageProcessesDialog( QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageProcessesDialog::ManageProcessesDialog() constructor called.\n");
  
  pd = NULL;
  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;
  clo = NULL;
  expID = exp_id;
  if ( !name ) setName( "ManageProcessesDialog" );

  setSizeGripEnabled( TRUE );
  ManageProcessesDialogLayout = new QVBoxLayout( this, 11, 6, "ManageProcessesDialogLayout"); 

  availableHostsLabel = new QLabel( this, "availableHostsLabel" );
  ManageProcessesDialogLayout->addWidget( availableHostsLabel );
  availableHostsComboBox = new QComboBox( this, "availableHostsComboBox");
  availableHostsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, availableHostsComboBox->sizePolicy().hasHeightForWidth() ) );
  availableHostsComboBox->setEditable(TRUE);
  ManageProcessesDialogLayout->addWidget( availableHostsComboBox );

  attachCollectorsListView = new QListView( this, "attachCollectorsListView" );
  attachCollectorsListView->addColumn( 
    tr( QString("Collectors belonging to experiment: '%1':").arg(expID) ) );
  attachCollectorsListView->addColumn( tr( QString("Name") ) );
  attachCollectorsListView->setSelectionMode( QListView::Single );
  attachCollectorsListView->setAllColumnsShowFocus( TRUE );
  attachCollectorsListView->setShowSortIndicator( TRUE );
  attachCollectorsListView->setRootIsDecorated(TRUE);

  ManageProcessesDialogLayout->addWidget( attachCollectorsListView );


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
  ManageProcessesDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( availableHostsComboBox, SIGNAL( activated(const QString &) ), this, SLOT( availableHostsComboBoxActivated() ) );

  connect(attachCollectorsListView, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint& , int ) ), this, SLOT( contextMenuRequested( QListViewItem *, const QPoint &, int ) ) );

  updateAttachedCollectorsList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageProcessesDialog::~ManageProcessesDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageProcessesDialog::ManageProcessesDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageProcessesDialog::languageChange()
{
  setCaption( tr( "ManageProcessesDialog" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  availableHostsLabel->setText( tr("Available Hosts:") );
  availableHostsComboBox->insertItem( "localhost" );
}

QString
ManageProcessesDialog::selectedCollectors()
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
ManageProcessesDialog::updateAttachedCollectorsList()
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
    for( CollectorParameterEntryList::Iterator pit = ce->paramList.begin();
         pit != ce->paramList.end();  pit++)
    {
      CollectorParameterEntry *cpe = (CollectorParameterEntry *)*pit;
      QListViewItem *item2 = new QListViewItem( item, cpe->name, cpe->param_value );
    }

  }
}

void ManageProcessesDialog::availableHostsComboBoxActivated()
{
    updateAttachedCollectorsList();
}

void
ManageProcessesDialog::contextMenuRequested( QListViewItem *item, const QPoint &pos, int col)
{
  printf("ManagerCollectorsDialog::createPopupMenu() entered.\n");

  QPopupMenu *popupMenu = new QPopupMenu(this);
 
  popupMenu->insertItem("Add Process...", this, SLOT(addProcessSelected()) );
  popupMenu->insertItem("Add Program...", this, SLOT(addProgramSelected()) );
  popupMenu->insertSeparator();
  if( attachCollectorsListView->selectedItem() )
  {
    popupMenu->insertItem("Remove Entry..", this, SLOT(removeEntrySelected()) );
  }


  popupMenu->popup( pos );
}


void
ManageProcessesDialog::addProcessSelected()
{
printf("addProcessSelected\n");
  mw->executableName = QString::null;
  mw->pidStr = QString::null;
  mw->attachNewProcess();


  if( !mw->pidStr.isEmpty() )
  {

//    char command[1024];
//    sprintf(command, "expAttach -p %d\n", mw->pidStr.toInt() );

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
ManageProcessesDialog::removeEntrySelected()
{
  printf("removeEntrySelected\n");
  updateAttachedCollectorsList();
}

void
ManageProcessesDialog::addProgramSelected()
{
printf("addProgramSelected\n");
  mw->executableName = QString::null;
  mw->loadNewProgram();
  QString executableNameStr = mw->executableName;
  if( !mw->executableName.isEmpty() )
  {
    executableNameStr = mw->executableName;

//    char command[1024];
//    sprintf(command, "expAttach -f %s\n", executableNameStr.ascii() );
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

//  statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
  loadTimer->stop();
  pd->hide();
  delete(pd);

//  runnableFLAG = TRUE;
//  pco->runButton->setEnabled(TRUE);
//  pco->runButton->enabledFLAG = TRUE;

  }
  updateAttachedCollectorsList();
}


static bool step_forward = TRUE;
void
ManageProcessesDialog::progressUpdate()
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
