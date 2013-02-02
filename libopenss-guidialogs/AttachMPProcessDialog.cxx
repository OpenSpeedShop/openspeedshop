////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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
//#define DEBUG_AMPPD 1
//


#include <stddef.h>
#include "AttachMPProcessDialog.hxx"

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
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qsplitter.h>
#include <qsize.h>
#include <qsettings.h>

AttachMPProcessDialog::AttachMPProcessDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("AttachMPProcessDialog::AttachMPProcessDialog() constructor called.\n");
#ifdef DEBUG_AMPPD
  printf("AttachMPProcessDialog::AttachMPProcessDialog() constructor called.\n");
#endif
  
  // Workaround for Qt bug that sends errant signal to buttonFilterSelected() 
  addSelectedFLAG = FALSE;

  width = 0;
  height = 0;
  filterFLAG = FALSE;
  plo = NULL;
  incExcList.clear();
  if ( !name ) setName( "AttachMPProcessDialog" );

  setSizeGripEnabled( TRUE );

  layout = new QVBoxLayout( this, 1, 2, "layout" );

  attachMPProcessSplitter = new QSplitter( this, "attachMPProcessSplitter" );
  attachMPProcessSplitter->setOrientation( QSplitter::Vertical );
  layout->addWidget(attachMPProcessSplitter);

  topFrame = new QFrame(attachMPProcessSplitter, "topFrame");
  topFrame->setFrameShape( QFrame::Box );
  topFrame->setFrameShadow( QFrame::Plain );

  AttachMPProcessDialogLayout = new QVBoxLayout( topFrame, 11, 6, "AttachMPProcessDialogLayout"); 

  QHBoxLayout *headerLayout = new QHBoxLayout( AttachMPProcessDialogLayout, 6, "headerLayout" );

  attachHostLabel = new QLabel( topFrame, "attachHostLabel" );
  headerLayout->addWidget( attachHostLabel );
  attachHostComboBox = new QComboBox( topFrame, "attachHostComboBox");
  attachHostComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, attachHostComboBox->sizePolicy().hasHeightForWidth() ) );
  attachHostComboBox->setEditable(TRUE);
  headerLayout->addWidget( attachHostComboBox );

  availableProcessListView = new QListView( topFrame, "availableProcessListView" );
  availableProcessListView->addColumn( tr( "Attachable Process List:" ) );
  availableProcessListView->setSelectionMode( QListView::Extended );
  availableProcessListView->setAllColumnsShowFocus( FALSE );
  availableProcessListView->setShowSortIndicator( FALSE );
  AttachMPProcessDialogLayout->addWidget( availableProcessListView );


  mpiCB = new QCheckBox( topFrame, "mpiCB");
  AttachMPProcessDialogLayout->addWidget( mpiCB );
  mpiCB->setText(tr("Attach to all mpi related process."));
  mpiCB->setChecked(TRUE);




  bottomFrame = new QFrame(attachMPProcessSplitter, "bottomFrame");
  bottomFrame->setFrameShape( QFrame::Box );
  bottomFrame->setFrameShadow( QFrame::Plain );

  bottomLayout = new QVBoxLayout( bottomFrame, 11, 6, "bottomLayout" );

  exclusionInclusionList = new QListView( bottomFrame, "exclusionInclusionList" );
  exclusionInclusionList->addColumn( tr( "List of processes to include or exclude from the above list:" ) );
  exclusionInclusionList->setSelectionMode( QListView::Extended );
  exclusionInclusionList->setAllColumnsShowFocus( FALSE );
  exclusionInclusionList->setShowSortIndicator( FALSE );
  exclusionInclusionList->setMinimumHeight( 50 );

  bottomLayout->addWidget(exclusionInclusionList);


  QHBoxLayout *layout1 = new QHBoxLayout( bottomLayout, 0, "layout1" );

  exclusionRB = new QRadioButton( bottomFrame, "exclusionRB");
  layout1->addWidget(exclusionRB);
  exclusionRB->setText(tr("Exclude this list of executables from above top list."));
  exclusionRB->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, attachHostComboBox->sizePolicy().hasHeightForWidth() ) );
  exclusionRB->setChecked(TRUE);

  QPushButton *removeButton = new QPushButton(bottomFrame, "removeButton");
  removeButton->setText( tr("Remove") );
  connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeSelected() ) );
  layout1->addWidget(removeButton);

  QHBoxLayout *layout2 = new QHBoxLayout( bottomLayout, 0, "layout2" );

  inclusionRB = new QRadioButton( bottomFrame, "inclusionRB");
  layout2->addWidget(inclusionRB);
  inclusionRB->setText(tr("Include only this list of executables in the top list."));
  inclusionRB->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, attachHostComboBox->sizePolicy().hasHeightForWidth() ) );

  QPushButton *saveButton = new QPushButton(bottomFrame, "saveButton");
  saveButton->setText( tr("Save") );
  connect( saveButton, SIGNAL( clicked() ), this, SLOT( saveSelected() ) );
  layout2->addWidget(saveButton);



// Begin Add process to filter section
  addLayout = new QHBoxLayout( bottomLayout, 0, "addLayout"); 
  addButtonLabel = new QLabel(bottomFrame, "addButtonLabel");

  addButtonLabel->setText( tr("Process to add:") );
  addLayout->addWidget(addButtonLabel);

  addButtonText = new QLineEdit(bottomFrame, "addButtonText");
  connect( addButtonText, SIGNAL( returnPressed() ), this,
           SLOT( addSelected() ) );
  addLayout->addWidget(addButtonText);

  addButton = new QPushButton(bottomFrame, "addButton");
  addButton->setText( tr("Add") );
  connect( addButton, SIGNAL( clicked() ), this, SLOT( addSelected() ) );

  addLayout->addWidget(addButton);
// End Add process to filter section

  bottomFrame->hide();

  buttonLayout = new QHBoxLayout( 0, 0, 6, "buttonLayout"); 

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  buttonLayout->addWidget( buttonHelp );
  buttonSpacing = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  buttonLayout->addItem( buttonSpacing );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  buttonLayout->addWidget( buttonOk );

  updateOk = new QPushButton( this, "updateOk" );
  updateOk->setAutoDefault( TRUE );
  updateOk->setDefault( TRUE );
  buttonLayout->addWidget( updateOk );

  buttonFilter = new QPushButton( this, "buttonFilter" );
  buttonFilter->setAutoDefault( TRUE );
  buttonFilter->setDefault( TRUE );
  buttonLayout->addWidget( buttonFilter );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  buttonLayout->addWidget( buttonCancel );
  layout->addLayout( buttonLayout );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  attachHostComboBox->setAutoCompletion(TRUE);

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ok_accept() ) );
  connect( updateOk, SIGNAL( clicked() ), this, SLOT( attachHostComboBoxActivated() ) );
  connect( buttonFilter, SIGNAL( clicked() ), this, SLOT( buttonFilterSelected() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( attachHostComboBox, SIGNAL( activated(const QString &) ), this, SLOT( attachHostComboBoxActivated() ) );

  connect( exclusionRB, SIGNAL(clicked()), this, SLOT(exclusionRBSelected()) );
  connect( inclusionRB, SIGNAL(clicked()), this, SLOT(inclusionRBSelected()) );

  readFilterList();

  updateAttachableProcessList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
AttachMPProcessDialog::~AttachMPProcessDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("AttachMPProcessDialog::AttachMPProcessDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AttachMPProcessDialog::languageChange()
{
  setCaption( tr( "AttachMPProcessDialog" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&Attach" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  updateOk->setText( tr( "&Update" ) );
  updateOk->setAccel( QKeySequence( QString::null ) );
  buttonFilter->setText( tr( "&Filter" ) );
  buttonFilter->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  attachHostLabel->setText( tr("Host:") );
  attachHostComboBox->insertItem( "localhost" );
}

// QString
QStringList *
AttachMPProcessDialog::selectedProcesses(bool *mpiFLAG)
{
//  QString ret_value = attachHostComboBox->currentText();

#ifdef DEBUG_AMPPD
  printf("AttachMPProcessDialog::selectedProcesses() entered\n");
#endif

  QStringList *qsl = new QStringList();
  qsl->clear();
  QListViewItem *selectedItem = availableProcessListView->selectedItem();
  *mpiFLAG = mpiCB->isChecked();
  QListViewItemIterator it( availableProcessListView, QListViewItemIterator::Selected );
  while( it.current() )
  {
    QListViewItem *item = it.current();
    QString ret_val = item->text(0);
    qsl->push_back(ret_val);
#ifdef DEBUG_AMPPD
// printf("push_back (%s)\n", ret_val.ascii() );
#endif
    ++it;
  }
  return( qsl );
}



void
AttachMPProcessDialog::updateAttachableProcessList()
{
  char *host = (char *)attachHostComboBox->currentText().ascii();
#ifdef DEBUG_AMPPD
// printf("host=(%s)\n", host );
#endif
  ProcessEntry *pe = NULL;
  char entry_buffer[1024];

  if( plo )
  {
    delete(plo);
  }
#ifdef DEBUG_AMPPD
// printf("look up processes on host=(%s)\n", host);
#endif
  plo = new ProcessListObject(host);

  availableProcessListView->clear();

  ProcessEntryList::Iterator it;
  for( it = plo->processEntryList.begin();
       it != plo->processEntryList.end();
       ++it )
  {
    pe = (ProcessEntry *)*it;
    if( includeOrExcludeThisItem((const char *)pe->process_name) )
    {
#ifdef DEBUG_AMPPD
// printf("%-20s %-10d %-20s\n", pe->host_name, pe->pid, pe->process_name);
#endif
      sprintf(entry_buffer, "%-20s %-10d %-20s\n", pe->host_name, pe->pid, pe->process_name);
      QListViewItem *item = new QListViewItem( availableProcessListView, 0 );
      item->setText( 0, tr(entry_buffer) );
    }
  }
}

void AttachMPProcessDialog::attachHostComboBoxActivated()
{
#ifdef DEBUG_AMPPD
// printf("attachHostComboBoxActivated\n");
#endif
// attachHostComboBox->insertItem( attachHostComboBox->currentText() );
    updateAttachableProcessList();
}

void AttachMPProcessDialog::buttonFilterSelected()
{
#ifdef DEBUG_AMPPD
//  printf("buttonFilterSelected() entered\n");
#endif

  if( addSelectedFLAG == TRUE )
  { // Workaround for Qt bug that sends errant signal to buttonFilterSelected() 
    addSelectedFLAG = FALSE;
    return;
  }

  if( filterFLAG == TRUE )
  {
    bottomFrame->hide();
    filterFLAG = FALSE;
  } else
  {
    if( width == 0 || height == 0 )
    {
      QSize size = this->size();
      width = size.width();
      height = size.height();
    
      QValueList<int> sizeList;
      sizeList.clear();
      sizeList.push_back(height);
      sizeList.push_back((int)(height*.50));
      attachMPProcessSplitter->setSizes(sizeList);
    

      resize( QSize(width, (int)(height*1.50)) );
    }
    bottomFrame->show();
    filterFLAG = TRUE;
  }
}

void AttachMPProcessDialog::accept()
{
#ifdef DEBUG_AMPPD
// printf("AttachMPProcessDialog::accept() called.\n");
#endif
}

void AttachMPProcessDialog::ok_accept()
{
#ifdef DEBUG_AMPPD
// printf("AttachMPProcessDialog::ok_accept() called.\n");
#endif
  QDialog::accept();
}


bool
AttachMPProcessDialog::includeOrExcludeThisItem(const char *process_name)
{
#ifdef DEBUG_AMPPD
// printf("Is process_name (%s) in the incExcList?\n", process_name);
#endif
  bool foundFLAG = FALSE;
  IncExcList::Iterator it;
  for( it = incExcList.begin();
         it != incExcList.end();
         ++it )
  {
    QString s = (QString)*it;
    if( s == QString(process_name) )
    {
#ifdef DEBUG_AMPPD
// printf("It's in my list! (%s-%s)\n", s.ascii(), process_name );
#endif
      foundFLAG = TRUE;
      break;
    }
  }

  if( exclusionRB->isChecked() )
  {
    if( foundFLAG )
    {
      return( FALSE );
    } else
    { 
      return( TRUE );
    }
  } else if( inclusionRB->isChecked() )
  {
    if( foundFLAG )
    {
      return( TRUE );
    } else
    { 
      return( FALSE );
    }
  }

  return(TRUE);
}

void
AttachMPProcessDialog::exclusionRBSelected()
{
  exclusionRB->setChecked(TRUE);
  inclusionRB->setChecked(FALSE);

  updateAttachableProcessList();
}

void
AttachMPProcessDialog::inclusionRBSelected()
{
  exclusionRB->setChecked(FALSE);
  inclusionRB->setChecked(TRUE);

  updateAttachableProcessList();
}

void
AttachMPProcessDialog::addSelected()
{
#ifdef DEBUG_AMPPD
// printf("addSelected() \n");
#endif

  // Workaround for Qt bug that sends errant signal to buttonFilterSelected() 
  addSelectedFLAG = TRUE;

  QString processToAdd = addButtonText->text().stripWhiteSpace();

  if( processToAdd.isEmpty() )
  {
    return;
  }

  
  // First check to see if we already have it in the list.
  IncExcList::Iterator it;
  for( it = incExcList.begin();
         it != incExcList.end();
         ++it )
  {
    QString s = (QString)*it;
    if( s == processToAdd )
    {
      return;
    }
  }
#ifdef DEBUG_AMPPD
// printf("process to add: (%s)\n", processToAdd.ascii() );
#endif
  incExcList.push_back(processToAdd);

  updateAttachableProcessList();
  updateFilterList();
}

void
AttachMPProcessDialog::removeSelected()
{
#ifdef DEBUG_AMPPD
// printf("removeSelected() \n");
#endif

  QStringList *qsl = new QStringList();
  QListViewItem *selectedItem = exclusionInclusionList->selectedItem();
  QListViewItemIterator it( exclusionInclusionList, QListViewItemIterator::Selected );
  while( it.current() )
  {
    QListViewItem *item = it.current();
    QString ret_val = item->text(0);

    incExcList.remove(ret_val);
    ++it;
  }

  updateFilterList();
  updateAttachableProcessList();
}

void
AttachMPProcessDialog::readFilterList()
{
#ifdef DEBUG_AMPPD
// printf("readFilterlist() entered\n");
#endif

  settings = new QSettings();

  QString ds = QString::null;

  ds = settings->readEntry( "/openspeedshop/general/incExcProcessList");

#ifdef DEBUG_AMPPD
// printf("ds=(%s)\n", ds.ascii() );
#endif


  if( !ds.isEmpty() )
  {
    const char FIELD_SEP = ',';
    QStringList fields = QStringList::split( FIELD_SEP, ds );
    for( int i =0; i< fields.count(); i++ )
  {
      incExcList.push_back(fields[i]);
    }
  }

  char settings_buffer[1024];
  sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", "general", exclusionRB->name() );
  exclusionRB->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );
  sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", "general", inclusionRB->name() );
  inclusionRB->setChecked(
        settings->readBoolEntry(settings_buffer, FALSE) );

  updateFilterList();
  delete settings;
}

void
AttachMPProcessDialog::saveSelected()
{
#ifdef DEBUG_AMPPD
//printf("saveSelected() entered\n");
#endif
  QString s = QString::null;
  IncExcList::Iterator it;
  for( it = incExcList.begin();
         it != incExcList.end();
         ++it )
  {
    if( !s.isEmpty() )
    {
      s += ",";
    }
    s += (QString)*it;
  }

  if( s.isEmpty() )
  {
    return;
  }

#ifdef DEBUG_AMPPD
//printf("You'll want to save (%s)\n", s.ascii() );
#endif
  if( !s.isEmpty() )
  {
    settings = new QSettings();
#ifdef DEBUG_AMPPD
//printf("are you really writting this?\n");
#endif
    if( !settings->writeEntry( "/openspeedshop/general/incExcProcessList", s) )
    {
      fprintf(stderr, "Write of process list failed.\n");
    }
  }

  char settings_buffer[1024];

  sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", "general", exclusionRB->name() );
  settings->writeEntry(settings_buffer, exclusionRB->isChecked() );

  sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", "general", inclusionRB->name() );
  settings->writeEntry(settings_buffer, inclusionRB->isChecked() );


  delete settings;
}

void
AttachMPProcessDialog::updateFilterList()
{
  exclusionInclusionList->clear();

  IncExcList::Iterator it;
  for( it = incExcList.begin();
         it != incExcList.end();
         ++it )
  {
    QString s = (QString)*it;
    QListViewItem *item = new QListViewItem( exclusionInclusionList, 0 );
    item->setText( 0, tr(s) );
  }
}
