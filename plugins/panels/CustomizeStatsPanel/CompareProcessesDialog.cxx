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

// debug flag
//#define DEBUG_COMPARE 1
// end debug flag
  

#include "CompareProcessesDialog.hxx"

#include "PanelListViewItem.hxx"

#include "debug.hxx"
#include "plus.xpm"
#include "minus.xpm"

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
#include <qtooltip.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "SS_Input_Manager.hxx"




CompareProcessesDialog::CompareProcessesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() constructor called.\n");

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor called.\n");
#endif

  expID = -1;

  QPixmap *plus_pm = new QPixmap( plus_xpm );
  plus_pm->setMask(plus_pm->createHeuristicMask());


  QPixmap *minus_pm = new QPixmap( minus_xpm );
  minus_pm->setMask(plus_pm->createHeuristicMask());

  updateFocus(-1, NULL);

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;


//  QToolTip::add(this, tr("This dialog helps define which processes will be display in each focused Compare\nSet/Column of the Compare Panel.\nSelecting on a column in the Compare Panel will change the focus in this display.") );
  
  if ( !name ) setName( "CompareProcessesDialog" );

  setSizeGripEnabled( TRUE );
  CompareProcessesDialogLayout = new QVBoxLayout( this, 11, 6, "CompareProcessesDialogLayout"); 

  headerLabel = new QLabel(this, "addProcessesLabel");
  headerLabel->setText( "Current list:");
  QToolTip::add(headerLabel, tr("This label details the focused compare set and column within that compare set.\nAny actions in this dialog will perform actions to the focused set.\n") );
// I'm not sure this header label makes sense.  Hide it for now.
headerLabel->hide();


  CompareProcessesDialogLayout->addWidget( headerLabel );

  QHBoxLayout *addProcessesHostLayout = new QHBoxLayout( 6, "addProcessesHostLayout");
  CompareProcessesDialogLayout->addLayout( addProcessesHostLayout );
  
  QHBoxLayout *addProcessesLayout = new QHBoxLayout( 6, "addProcessesLayout");
  CompareProcessesDialogLayout->addLayout( addProcessesLayout );
  
  addProcessesLabel = new QLabel(this, "addProcessesLabel");
  addProcessesLayout->addWidget( addProcessesLabel );

  addProcessesRegExpLineEdit = new QLineEdit(this, "addProcessesRegExpLineEdit");
  addProcessesLayout->addWidget( addProcessesRegExpLineEdit );
  QToolTip::add(addProcessesRegExpLineEdit, tr("Enter which process(es) to be added/removed.  This can be a single process name\nor a comma separated list of processes.\n\nRegular expressions will be honored. For example:\n  \"8455, 6545, 4500-5000\"\n  \"8455, hostname1:8554, hostname2:0-10000, 1303[12]\"\n  \"5433, hostname1:*, Terminated, 4323\"") );

//  QToolTip::add(addProcessesRegExpLineEdit, tr("Enter the pid (or regular expression defining the pids) that you want entered into\nthe current Column in the current Compare Set of the Compare Panel.\n\nDrag and drop, process sets or individual processes from here to the Compare Panel.") );

  QPushButton *addButton = new QPushButton( this, "addButton" );
  addButton->setText("Add");
  addButton->setPixmap(plus_xpm);
  QToolTip::add(addButton, tr("Add the items"));
  addProcessesLayout->addWidget( addButton );

  QPushButton *removeButton = new QPushButton( this, "removeButton" );
  removeButton->setText("Remove");
  removeButton->setPixmap(minus_xpm);
  QToolTip::add(removeButton, tr("Remove the items"));
  addProcessesLayout->addWidget( removeButton );


  availableProcessesListView = new MPListView( this, "availableProcessesListView", 0 );
  availableProcessesListView->addColumn( tr( "Available Processes:" ) );
  availableProcessesListView->addColumn( tr( "PID:" ) );
  availableProcessesListView->addColumn( tr( "Rank:" ) );
  availableProcessesListView->addColumn( tr( "Thread:" ) );
  availableProcessesListView->setSelectionMode( QListView::Single );
  availableProcessesListView->setShowSortIndicator( TRUE );
  availableProcessesListView->setSorting( 0, FALSE );
  availableProcessesListView->setAllColumnsShowFocus(TRUE);
  availableProcessesListView->setSortOrder( Qt::Ascending );
  availableProcessesListView->setRootIsDecorated(TRUE);
  availableProcessesListView->setResizeMode(QListView::LastColumn);
  QToolTip::add(availableProcessesListView->viewport(), tr("Listed here are all the available processes that can be added to current Column in\nthe current Compare Set of the Compare Panel.\n\nDrag and drop, process sets or individual processes from here to the Compare Panel.") );

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor added availableProcessesListView.\n");
#endif

  CompareProcessesDialogLayout->addWidget( availableProcessesListView );

 QToolTip::add( availableProcessesListView, tr( "Drag-n-drop entries from this list onto the Column containing your process group." ) );

  Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

#ifndef LATER
  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  Layout1->addWidget( buttonHelp );
#endif // LATER
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor after Layout1->addItem( Horizontal_Spacing2 ).\n");
#endif

  buttonFinished = new QPushButton( this, "buttonFinished" );
  buttonFinished->setAutoDefault( TRUE );
  Layout1->addWidget( buttonFinished );
  CompareProcessesDialogLayout->addLayout( Layout1 );
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor before    CompareProcessesDialogLayout->addLayout( Layout1 ); \n");
#endif

// jeg removing this after changes for adding the toolbar into the customize stats panel code caused the following error:
// QLayout::addChildLayout: layout already has a parent
// Commenting out this addLayout line seems to fix the problem w/o side effects.
//
#if 0
  CompareProcessesDialogLayout->addLayout( Layout1 );
#endif

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor before languageChange and after CompareProcessesDialogLayout->addLayout( Layout1 );.\n");
#endif

  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
#ifndef LATER
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( help() ) );
#endif // LATER

  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( buttonFinished, SIGNAL( clicked() ), this, SLOT( acceptProcesses() ) );

  connect( addButton, SIGNAL( clicked() ), this, SLOT( addProcesses() ) );
  connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeProcesses() ) );

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() constructor exits.\n");
#endif
  
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareProcessesDialog::~CompareProcessesDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() destructor called.\n");
#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::CompareProcessesDialog() destructor called.\n");
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CompareProcessesDialog::languageChange()
{

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::languageChange called.\n");
#endif

  setCaption( tr( name() ) );
  addProcessesLabel->setText( tr( "Add processes:" ) );
#ifndef LATER
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
#endif // LATER
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );

  buttonFinished->setText( tr( "&Finished" ) );
  buttonFinished->setAccel( QKeySequence( QString::null ) );
}

PanelListViewItem *
CompareProcessesDialog::selectedExperiment(int *expID)
{
  PanelListViewItem *selectedItem = (PanelListViewItem *)availableProcessesListView->selectedItem();

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog:::selectedExperiment() expID=%d, selectedItem=%d\n", expID, selectedItem);
#endif

  if( selectedItem ) {

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog:::selectedExperiment() HAVE selectedItem=%d\n", selectedItem);
#endif

  } else {

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog:::selectedExperiment() NO ITEMS SELECTED\n");
#endif

    nprintf( DEBUG_PANELS ) ("NO ITEMS SELECTED\n");
    return( NULL );
  }
}


void
CompareProcessesDialog::updateInfo()
{

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog:::updateInfo(), call restoreOverridCursor\n");
#endif

  QApplication::restoreOverrideCursor();
}

#include "AboutDialog.hxx"
void
CompareProcessesDialog::help()
{
  QString infoStr="Help text for dialog.";

  AboutDialog *helpDialog = new AboutDialog(this, "Help", FALSE, 0, infoStr);

  helpDialog->show();
}

#include "MPListViewItem.hxx"
void
CompareProcessesDialog::acceptProcesses()
{

#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::acceptProcesses()\n");
#endif

 accept();
}

void
CompareProcessesDialog::addProcesses()
{

#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::addProcesses(), addProcessesRegExpLineEdit->text().ascii()=%s\n", addProcessesRegExpLineEdit->text().ascii() );
#endif

  QString inputText = addProcessesRegExpLineEdit->text();

  QString pset_name = QString::null;
  QString host = QString::null;
  QString host_pidstr = addProcessesRegExpLineEdit->text().stripWhiteSpace();
  QString tidstr = QString::null;
  QString collector_name = QString::null;


  host = "Unknown";

  QStringList fields = QStringList::split( ",", inputText );

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::addProcesses(), inputText.ascii()=%s\n", inputText.ascii() );
#endif

  for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
  {
    host_pidstr = ((QString)*it).stripWhiteSpace();

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::addProcesses(), host_pidstr = (%s)\n", host_pidstr.ascii() );
#endif

    DescriptionClassObjectList *validatedHostPidList = validateHostPid(host_pidstr);

    if( validatedHostPidList ) {

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::addProcesses(), There seems to be a list... is there really something to add?\n");
#endif

      // First look for a selected item in the drop zone.

      MPListViewItem *selectedItem = NULL;
      QListViewItemIterator it(lv, QListViewItemIterator::Selected);

      while( it.current() ) {
        selectedItem = (MPListViewItem *)it.current();
        break;
      }

      // Make sure it the right target
      if( selectedItem && selectedItem->parent() && 
          selectedItem->parent()->text(0) == UDPS ) {

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::addProcesses(), Well I think we have a real selected item to add to ..\n");
#endif

      } else {

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::addProcesses(), Well I thought we had a real selected item to add to ..BUT NO\n");
#endif

        selectedItem = NULL;
      }
      
#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::addProcesses(), selectedItem=%d\n", selectedItem);
#endif

      if( !selectedItem ) {
        selectedItem = (MPListViewItem *)lv->firstChild();
      }

      for ( DescriptionClassObjectList::Iterator it = validatedHostPidList->begin();
                                          it != validatedHostPidList->end(); ++it )
      {
        DescriptionClassObject *dco = (DescriptionClassObject *)*it;
        MPListViewItem *item = new MPListViewItem( selectedItem, 
                                                   dco->host_name, 
                                                   dco->pid_name, 
                                                   dco->rid_name, 
                                                   dco->tid_name );

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::addProcesses(), A: host_name=(%s) pid_name=(%s) rid_name=(%s) tid_name=(%s)\n", 
               dco->host_name.ascii(), dco->pid_name.ascii(), dco->rid_name.ascii(), dco->tid_name.ascii() );
#endif
        item->descriptionClassObject = dco;
      }
  
      delete validatedHostPidList;
    }
  }
}

void
CompareProcessesDialog::removeProcesses()
{

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::removeProcesses(), removeProcesses(%s)\n", addProcessesRegExpLineEdit->text().ascii() );
#endif

  QString lower_rangestr = QString::null;
  QString upper_rangestr = QString::null;
  int lower_range = -1;
  int upper_range = -1;

  QString target_hostpid_str = QString::null;
  QString target_pidstr = QString::null;
  QString target_hoststr = QString::null;
  QString inputText = addProcessesRegExpLineEdit->text().stripWhiteSpace();



  QStringList fields = QStringList::split( ",", inputText );
  for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
  {
    // Do we have a host?
    target_hostpid_str = (QString)*it;
    target_hoststr = QString::null;
    int colon_index = target_hostpid_str.find(":");
    if( colon_index > -1 ) {
      target_hoststr =  target_hostpid_str.left(colon_index).stripWhiteSpace();
    
      int length = target_hostpid_str.length();
      length--; // We want to skip the ":"
      target_pidstr = target_hostpid_str.right(length-colon_index).stripWhiteSpace();
    } else {
      target_pidstr = target_hostpid_str.stripWhiteSpace();
    }

// Begin PSET delete
  // First check to see if we have a dynamic pset name.

    if( isPSetName(target_pidstr) == TRUE ) {
      bool deleted = FALSE;
      QListViewItemIterator it( lv );
      it++;
      while ( it.current() ) {

        QListViewItem *item = it.current();
        if( item->text(0) == target_pidstr || item->text(0) == target_pidstr+"*" ) {
          delete item;
          deleted = TRUE;
          break;
        }
        it++;

      } // end while

      if( deleted == TRUE ) {
        continue;
      }

    }
// End PSET delete
  
    QRegExp hostRegExp = QRegExp(target_hoststr, TRUE, TRUE);
    QRegExp pidRegExp = QRegExp(target_pidstr, TRUE, TRUE);
  
#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::removeProcesses(), target_pidstr = (%s)\n", target_pidstr.ascii() );
#endif

    // Do we have a range of pids?
    lower_range = -1;
    upper_range = -1;
    int dash_index = target_pidstr.find("-");
    if( dash_index > -1 )
    {
#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::removeProcesses(), Found a range!\n");
#endif
      int lb_index = target_pidstr.findRev("[", dash_index);
      int rb_index = target_pidstr.findRev("]", dash_index);
      if( rb_index > lb_index || lb_index == -1 ) {

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::removeProcesses(), There's a RANGE of processes to remove!\n");
#endif

        int length = target_pidstr.length();
        lower_rangestr = target_pidstr.left(dash_index);

        if( !lower_rangestr.isEmpty() ) {
          lower_range = lower_rangestr.toInt();
        }

        length--; // We want to skip the "-"
        upper_rangestr = target_pidstr.right(length-dash_index);

        if( !upper_rangestr.isEmpty() ) {
          upper_range = upper_rangestr.toInt();
        }

      }
    }

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::removeProcesses(), target_hoststr = (%s)\n", target_hoststr.ascii() );
    printf("CompareProcessesDialog::removeProcesses(), target_pidstr = (%s)\n", target_pidstr.ascii() );
    printf("CompareProcessesDialog::removeProcesses(), lower_rangestr=(%s)\n", lower_rangestr.ascii() );
    printf("CompareProcessesDialog::removeProcesses(), upper_rangestr=(%s)\n", upper_rangestr.ascii() );
#endif
   
    if( target_pidstr.find("*") > -1 ) {

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), Found a wildcard!\n");
#endif

    }

    // Loop through and attempt to find and delete this item.
    // First look for a selected item in the drop zone.
    MPListViewItem *selectedItem = NULL;
    QListViewItemIterator sel_it(lv, QListViewItemIterator::Selected);
    while( sel_it.current() )
    {
      selectedItem = (MPListViewItem *)sel_it.current();
      break;
    }
    // Make sure it the right target
    if( selectedItem && selectedItem->parent() && selectedItem->parent()->text(0) == UDPS ) {

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), Well I think we have a real seletected item to remove to ..\n");
#endif
    } else {
      selectedItem = NULL;
    }

    if( !selectedItem ) {
      selectedItem = (MPListViewItem *)lv->firstChild();
    }

#ifdef DEBUG_COMPARE
     printf("CompareProcessesDialog::removeProcesses(), lv->firstChild()->text(0)=(%s)\n", lv->firstChild()->text(0).ascii() );
#endif
    if( !selectedItem || lv->firstChild()->text(0) == CPS ) {
#ifdef DEBUG_COMPARE
     printf("CompareProcessesDialog::removeProcesses(), Do it the old way and loop through everyone..\n");
#endif
     QListViewItemIterator it( lv );
     it++;
     while ( it.current() ) {
      QListViewItem *item = it.current();
      ++it;

      if( !item ) {
        continue;
      }

      if( item->text(0).isEmpty() ) {
        continue;
      }
  
      QString pidstr = item->text(0).stripWhiteSpace();

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), pidstr=(%s)\n", pidstr.ascii() );
#endif

      // Skip any pset names...\n");
      if( isPSetName(QString(pidstr)) == TRUE ) { 
        // skip pset names...
        continue;
      }

      QString host_name = item->text(1).stripWhiteSpace();

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), host_name=(%s)\n", host_name.ascii() );
#endif

      if( !host_name.isEmpty() && host_name.find(hostRegExp) == -1 ) {

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::removeProcesses(), NO MATCH FOR HOSTNAME!\n");
#endif
        continue;
      }

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), lower_range=%d, upper_range=%d\n", lower_range, upper_range);
#endif
      if( lower_range >= 0 && upper_range >= 0 ) {
        int pid = pidstr.toInt();
#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::removeProcesses(), pid=%d, lower_range=%d, upper_range=%d\n", pid, lower_range, upper_range);
#endif
        if( pid < lower_range || pid > upper_range ) {
          continue;
        }
      } else {
        if( pidstr.find(pidRegExp) == -1 ) {

#ifdef DEBUG_COMPARE
          printf("CompareProcessesDialog::removeProcesses(), NO MATCH FOR PID!\n");
#endif
          continue;
        }
      }

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), HERE: delete item=(%s)\n", item->text(0).ascii() );
#endif
      delete item;
    }

   } else {

#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::removeProcesses(), Loop through just the children of (%s)\n", selectedItem->text(0).ascii() );
#endif

    QListViewItemIterator it( lv );
    it++;

    MPListViewItem *endItem = NULL;
    // find the start item.
    while( it.current() ) {
      if( it.current() == selectedItem ) {
        break;
      }
      ++it;
    }

    endItem = (MPListViewItem *)selectedItem->nextSibling();
    ++it;

    while ( it.current() ) {
      QListViewItem *item = it.current();
      if( endItem != NULL && endItem == item ) { 
        // That's the end of the range to remove items from...
        break;
      }
      ++it;

      if( !item ) {
        continue;
      }

      if( item->text(0).isEmpty() ) {
        continue;
      }
  
      QString pidstr = item->text(0).stripWhiteSpace();

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), pidstr=(%s)\n", pidstr.ascii() );
#endif

      // Skip any pset names...\n");
      if( isPSetName(QString(pidstr)) == TRUE ) { 
        // skip pset names...
        continue;
      }

      QString host_name = item->text(1).stripWhiteSpace();

      if( !host_name.isEmpty() && host_name.find(hostRegExp) == -1 ) {
        continue;
      }

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(), host_name=(%s)\n", host_name.ascii() );
#endif

      if( lower_range >= 0 && upper_range >= 0 ) {
        int pid = pidstr.toInt();
        if( pid < lower_range || pid > upper_range ) {
          continue;
        }
      } else {
        if( pidstr.find(pidRegExp) == -1 ) {
          continue;
        }
      }

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::removeProcesses(),  Can you delete?\n");
#endif

      if( !item->firstChild() ) {

#ifdef DEBUG_COMPARE
        printf("CompareProcessesDialog::removeProcesses(), here: delete item=(%s)\n", item->text(0).ascii() );
#endif
        delete item;
      }
    }
   }
  } // end for

}

bool
CompareProcessesDialog::isPSetName(QString name)
{

#ifdef DEBUG_COMPARE
  printf("CompareProcessesDialog::isPSetName(%s) entered\n", name.ascii() );
#endif

  for ( QStringList::Iterator it = psetNameList.begin(); it != psetNameList.end(); ++it )
  {
    QString pset_namestr = (QString)*it;

#ifdef DEBUG_COMPARE
// printf("Is it pset named %s\n", pset_namestr.ascii() );
#endif

    if( pset_namestr == name || pset_namestr+"*" == name ) {

#ifdef DEBUG_COMPARE
// printf("Found pset named %s\n", pset_namestr.ascii() );
#endif

      return TRUE;
    }
  }
  return FALSE;
}

bool
CompareProcessesDialog::updateFocus(int _expID, MPListView *_lv )
{
#ifdef DEBUG_COMPARE
  printf("updateFocus _expID = (%d) (%d)\n", expID, _expID );
#endif

  lv = _lv;
  if( _expID != -1 && _expID == expID ) {
    return FALSE;
  }
  psetNameList.clear();
  expID = _expID;

/*
  if( expID == -1 )
  {
    return FALSE;
  }
*/

#ifdef DEBUG_COMPARE
  printf("Exit updateFocus _expID = (%d) (%d)\n", expID, _expID );
#endif

  return TRUE;
}

DescriptionClassObjectList *
CompareProcessesDialog::validateHostPid(QString target_host_pidstr)
{
  DescriptionClassObjectList *dcolist = new DescriptionClassObjectList();

#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), validateHostPid (%s) \n", target_host_pidstr.ascii() );
#endif
  bool rangeFLAG = FALSE;
  if( target_host_pidstr.find("-") > -1 )
  {
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), Found a range!\n");
#endif
    rangeFLAG = TRUE;
  }
  bool colonFLAG = FALSE;
  if( target_host_pidstr.find(":") > -1 )
  {
#ifdef DEBUG_COMPARE
    printf("CompareProcessesDialog::validateHostPid(), Found a host!\n");
#endif
    colonFLAG = TRUE;
  }
  bool wildcardFLAG = FALSE;
  if( target_host_pidstr.find("*") > -1 )
  {
#ifdef DEBUG_COMPARE
// printf("CompareProcessesDialog::validateHostPid(), Found a wildcard!\n");
#endif
    wildcardFLAG = TRUE;
  }

  QString target_hoststr = QString::null;
  QString target_pidstr = QString::null;
  QString lower_rangestr = QString::null;
  QString upper_rangestr = QString::null;
  int lower_range = -1;
  int upper_range = -1;
  // Do we have a host?
  int colon_index = target_host_pidstr.find(":");
  if( colon_index > -1 )
  {
    target_hoststr =  target_host_pidstr.left(colon_index).stripWhiteSpace();
  
    int length = target_host_pidstr.length();
    length--; // We want to skip the ":"
    target_pidstr = target_host_pidstr.right(length-colon_index).stripWhiteSpace();
  } else
  {
    target_pidstr = target_host_pidstr.stripWhiteSpace();
  }
  
  if( target_pidstr.isEmpty() )
  {
    return( NULL );
  }

  // Do we have a range of pids?
  int dash_index = target_pidstr.find("-");
  if( dash_index > -1 )
  {
    int lb_index = target_pidstr.findRev("[", dash_index);
    int rb_index = target_pidstr.findRev("]", dash_index);
    if( rb_index > lb_index || lb_index == -1 )
    {
#ifdef DEBUG_COMPARE
// printf("CompareProcessesDialog::validateHostPid(), There's a RANGE!\n");
#endif
      int length = target_pidstr.length();
      lower_rangestr = target_pidstr.left(dash_index);
      if( !lower_rangestr.isEmpty() )
      {
        lower_range = lower_rangestr.toInt();
      }
      length--; // We want to skip the "-"
      upper_rangestr = target_pidstr.right(length-dash_index);
      if( !upper_rangestr.isEmpty() )
      {
        upper_range = upper_rangestr.toInt();
      }
//      return( NULL );
    }
  }

#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), target_hoststr = (%s)\n", target_hoststr.ascii() );
 printf("CompareProcessesDialog::validateHostPid(), target_pidstr = (%s)\n", target_pidstr.ascii() );
 printf("CompareProcessesDialog::validateHostPid(), lower_rangestr=(%s)\n", lower_rangestr.ascii() );
 printf("CompareProcessesDialog::validateHostPid(), upper_rangestr=(%s)\n", upper_rangestr.ascii() );
#endif

  QRegExp hostRegExp = QRegExp(target_hoststr, TRUE, TRUE);
  QRegExp pidRegExp = QRegExp(target_pidstr+" ", TRUE, TRUE);
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), pidRegExpr was based on target_pidstr (%s)\n", target_pidstr.ascii() );
 printf("CompareProcessesDialog::validateHostPid(), pidRegExp was (%s)\n", pidRegExp.pattern().ascii() );
#endif

  // First check to see if we have an exact match on the dynamic pset names.
  if( isPSetName(target_pidstr) == TRUE )
  {
    QString pset_name = target_pidstr+"*";
    MPListViewItem *pitem = new MPListViewItem( lv->firstChild(), pset_name );
    DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
    if( target_pidstr == "All" )
    {
      dco->all = TRUE;
    }

    QListViewItemIterator it( availableProcessesListView );
    while ( it.current() )
    {
      QListViewItem *item = it.current();
  
      if( item->text(0) == target_pidstr )
      {
        QListViewItem *child = item->firstChild();
        while( child )
        {
          // child->text(0) is the pidstr;
          // child->text(1) is the hoststr;
          if( target_pidstr == "All" )
          {
            MPListViewItem *item2 = new MPListViewItem( pitem, "All pids" );
          } else
          {
            MPListViewItem *item2 = new MPListViewItem( pitem, child->text(0), child->text(1)  );
          }
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), This could be in error!\n");
#endif
          DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, child->text(1), child->text(0) );
          child = child->nextSibling();
        }

        break;
      }
      it++;
    }
    return( NULL );
  }

  // Now try for a match with the frameworks host/pid entries.
  try
  {
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo->FW() != NULL )
    {
      ThreadGroup tgrp = eo->FW()->getThreads();
      ThreadGroup::iterator ti;
      std::vector<std::string> v;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++)
      {
        Thread t = *ti;
        std::string s = t.getHost();
        
        v.push_back(s);
      }

#ifdef DEBUG_COMPARE
      printf("CompareProcessesDialog::validateHostPid(), calling sort()\n");
#endif

      std::sort(v.begin(), v.end());

      std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

      for( std::vector<std::string>::iterator hi = v.begin(); hi != e; hi++ ) 
      {
        QString pset_name = QString(*hi);
        QString host_name = QString(*hi);
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), hi=(%s)\n", hi->c_str() );
#endif
        if( !host_name.isEmpty()  && host_name.find(hostRegExp) == -1 ) 
        {
          continue;
        }
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
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), pidstr=(%s)\n", pidstr.ascii() );
            QString tmp_tidstr = QString::null;
            if (pthread.first) {
              tmp_tidstr = QString("%1").arg(pthread.second);
              printf("CompareProcessesDialog::validateHostPid(), tmp_tidstr=(%s)\n", tmp_tidstr.ascii() );
            }
#endif
            if( lower_range > 0 && upper_range > 0 )
            {
              int pid = pidstr.toInt();
              if( pid < lower_range || pid > upper_range )
              {
                continue;
              }
            } else
            {
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), pidstr=(%s) target_pidstr=(%s)\n", pidstr.ascii(), target_pidstr.ascii() );
#endif
              // Add blanks before and after to help qt delineate the items.
              QString tpidstr = " "+pidstr+" ";
              if( pidstr == target_pidstr )
              { 
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), pidstr == target_pidstr: Found!\n");
#endif
              } else if( wildcardFLAG == TRUE && tpidstr.find(pidRegExp) != -1 )
              {
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), regexp: Found! (%s)\n", pidRegExp.pattern().ascii() );
#endif
              } else
              {
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), NOPE: NOT ONE OF THESE!\n");
#endif
                continue;
              }
            }
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), Found!\n");
#endif
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
            if( !pidstr.isEmpty() )
            {
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, pidstr, ridstr  );
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), A: host_name=(%s) pidstr=(%s) ridstr=(%s) tidstr=(%s)\n", host_name.ascii(), pidstr.ascii(), ridstr.ascii(), tidstr.ascii() );
#endif

              dcolist->append(dco);
            } else if( !tidstr.isEmpty() )
            {
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), B: host_name=(%s) pidstr=(%s) ridstr=(%s) tidstr=(%s)\n", host_name.ascii(), pidstr.ascii(), ridstr.ascii(), tidstr.ascii() );
#endif
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, tidstr, ridstr  );
              dcolist->append(dco);
            } else if( !ridstr.isEmpty() )
            {
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, pidstr, ridstr  );
              dcolist->append(dco);
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), C: host_name=(%s) pidstr=(%s) ridstr=(%s) tidstr=(%s)\n", host_name.ascii(), pidstr.ascii(), ridstr.ascii(), tidstr.ascii() );
#endif
            } else
            {
              DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, pidstr, ridstr  );
              dcolist->append(dco);
#ifdef DEBUG_COMPARE
 printf("CompareProcessesDialog::validateHostPid(), D: host_name=(%s) pidstr=(%s) ridstr=(%s) tidstr=(%s)\n", host_name.ascii(), pidstr.ascii(), ridstr.ascii(), tidstr.ascii() );
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
    return NULL;;
  }
 return( dcolist );
}
