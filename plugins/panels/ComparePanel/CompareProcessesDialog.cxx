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
  

#include "CompareProcessesDialog.hxx"

#include "PanelListViewItem.hxx"

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
#include <qtooltip.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "CompareClass.hxx"
#include "CompareSet.hxx"
#include "ColumnSet.hxx"

#include "SS_Input_Manager.hxx"
CompareProcessesDialog::CompareProcessesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() constructor called.\n");

  
  updateFocus(-1, NULL, NULL, NULL);

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;

  QToolTip::add(this, tr("This dialog helps define which processes will be display in each focused Compare\nSet/Column of the Compare Panel.\nSelecting on a column in the Compare Panel will change the focus in this display.") );
  
  if ( !name ) setName( "CompareProcessesDialog" );

  setSizeGripEnabled( TRUE );
  CompareProcessesDialogLayout = new QVBoxLayout( this, 11, 6, "CompareProcessesDialogLayout"); 

  headerLabel = new QLabel(this, "addProcessesLabel");
  headerLabel->setText( "Modify Compare Set %%1: Column %%1" );
  QToolTip::add(headerLabel, tr("This label details the focused compare set and column within that compare set.\nAny actions in this dialog will perform actions to the focused set.\n") );

  CompareProcessesDialogLayout->addWidget( headerLabel );

  QHBoxLayout *addProcessesHostLayout = new QHBoxLayout( 6, "addProcessesHostLayout");
  CompareProcessesDialogLayout->addLayout( addProcessesHostLayout );
  
  addProcessesHostLabel = new QLabel(this, "addProcessesHostLabel");
addProcessesHostLabel->setText("Target host:");
  addProcessesHostLayout->addWidget( addProcessesHostLabel );

  addProcessesHostRegExpLineEdit = new QLineEdit(this, "addProcessesHostRegExpLineEdit");
// addProcessesHostRegExpLineEdit->setText("*");
  addProcessesHostLayout->addWidget( addProcessesHostRegExpLineEdit );
  QToolTip::add(addProcessesHostRegExpLineEdit, tr("Select which host the process(es) should be farmed.\nThis can be a single host name or a comma separated list of host names.\nRegular expressions will be honored.") );

  QHBoxLayout *addProcessesLayout = new QHBoxLayout( 6, "addProcessesLayout");
  CompareProcessesDialogLayout->addLayout( addProcessesLayout );
  
  addProcessesLabel = new QLabel(this, "addProcessesLabel");
  addProcessesLayout->addWidget( addProcessesLabel );

  addProcessesRegExpLineEdit = new QLineEdit(this, "addProcessesRegExpLineEdit");
  addProcessesLayout->addWidget( addProcessesRegExpLineEdit );
  QToolTip::add(addProcessesRegExpLineEdit, tr("Select which process(es), on the selected host, should be added.\nThis can be a single processes name or a comma separated list of processes names.\nRegular expressions will be honored.(Eventually)") );

//  QToolTip::add(addProcessesRegExpLineEdit, tr("Enter the pid (or regular expression defining the pids) that you want entered into\nthe current Column in the current Compare Set of the Compare Panel.\n\nDrag and drop, psets or individual processes from here to the Compare Panel.") );

  QHBoxLayout *removeProcessesLayout = new QHBoxLayout( 6, "removeProcessesLayout");
  CompareProcessesDialogLayout->addLayout( removeProcessesLayout );
  
  removeProcessesLabel = new QLabel(this, "removeProcessesLabel");
  removeProcessesLayout->addWidget( removeProcessesLabel );
  removeProcessesRegExpLineEdit = new QLineEdit(this, "removeProcessesRegExpLineEdit");
  QToolTip::add(addProcessesRegExpLineEdit, tr("Select which process(es), on the selected host, should be removed.\nThis can be a single processes name or a comma separated list of processes names.\nRegular expressions will be honored.") );

//  QToolTip::add(removeProcessesRegExpLineEdit, tr("Enter the pid (or regular expression defining the pids) that you want removed from\nthe current Column in the current Compare Set of the Compare Panel.\n\nDrag and drop, psets or individual processes from here to the ComparePanel.") );
  removeProcessesLayout->addWidget( removeProcessesRegExpLineEdit );


  availableProcessesListView = new MPListView( this, "availableProcessesListView", 0 );
  availableProcessesListView->addColumn( tr( "Available Processes:" ) );
  availableProcessesListView->setSelectionMode( QListView::Single );
  availableProcessesListView->setShowSortIndicator( TRUE );
  availableProcessesListView->setSorting( 0, FALSE );
  availableProcessesListView->setAllColumnsShowFocus(TRUE);
  availableProcessesListView->setSortOrder( Qt::Ascending );
  availableProcessesListView->setRootIsDecorated(TRUE);
  availableProcessesListView->setResizeMode(QListView::LastColumn);
  QToolTip::add(availableProcessesListView->viewport(), tr("Listed here are all the available processes that can be added to current Column in\nthe current Compare Set of the Compare Panel.\n\nDrag and drop, psets or individual processes from here to the Compare Panel.") );


  CompareProcessesDialogLayout->addWidget( availableProcessesListView );

 QToolTip::add( availableProcessesListView, tr( "Drag-n-drop entries from this list onto the Column containing your process group." ) );

  Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  Layout1->addWidget( buttonHelp );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

  applyOk = new QPushButton( this, "applyOk" );
  applyOk->setAutoDefault( TRUE );
  applyOk->setDefault( TRUE );
  Layout1->addWidget( applyOk );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  Layout1->addWidget( buttonOk );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );
  CompareProcessesDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( buttonOkSelected() ) );
  connect( applyOk, SIGNAL( clicked() ), this, SLOT( applyOkSelected() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

//  connect( addProcessesHostRegExpLineEdit, SIGNAL( returnPressed() ), this, SLOT( addProcessesHostRegExpLineEditEntered() ) );
//  connect( addProcessesRegExpLineEdit, SIGNAL( returnPressed() ), this, SLOT( addProcessesRegExpLineEditEntered() ) );
//  connect( removeProcessesRegExpLineEdit, SIGNAL( returnPressed() ), this, SLOT( removeProcesses() ) );
  
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareProcessesDialog::~CompareProcessesDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CompareProcessesDialog::languageChange()
{
  setCaption( tr( name() ) );
  addProcessesLabel->setText( tr( "Add processes:" ) );
  removeProcessesLabel->setText( tr( "Remove processes:" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  applyOk->setText( tr( "&Apply" ) );
  applyOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
}

PanelListViewItem *
CompareProcessesDialog::selectedExperiment(int *expID)
{
  PanelListViewItem *selectedItem = (PanelListViewItem *)availableProcessesListView->selectedItem();


  if( selectedItem )
  {

  } else
  {
    nprintf( DEBUG_PANELS ) ("NO ITEMS SELECTED\n");
    return( NULL );
  }
}


void
CompareProcessesDialog::updateInfo()
{

  QApplication::restoreOverrideCursor();
}

#include "MPListViewItem.hxx"
void
CompareProcessesDialog::addProcessesRegExpLineEditEntered()
{
printf("addProcessesRegExpLineEditEntered(%s)\n", addProcessesRegExpLineEdit->text().ascii() );

  QString inputText = addProcessesRegExpLineEdit->text();

  QString pset_name = QString::null;
  QString host = QString::null;
  QString pidstr = addProcessesRegExpLineEdit->text().stripWhiteSpace();
  QString tidstr = QString::null;
  QString collector_name = QString::null;


  host = "Unknown";
  // First get the host scope.
  if( addProcessesHostRegExpLineEdit->text() == "*" )
  {
    host = "Any host";
  } else
  {
    host = addProcessesHostRegExpLineEdit->text();
  }

QStringList fields = QStringList::split( ",", inputText );
for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
{
  pidstr = ((QString)*it).stripWhiteSpace();

printf("pidstr = (%s)\n", pidstr.ascii() );

  if( pidstr.find(":") > -1 )
  {
printf("Found a range!\n");
   
  }
  if( pidstr.find("*") > -1 )
  {
printf("Found a wildcard!\n");
  }
  QStringList validatedPidList = validatePid(host, pidstr);

  for ( QStringList::Iterator it = validatedPidList.begin(); it != validatedPidList.end(); ++it )
  {
    QString vpidstr = ((QString)*it).stripWhiteSpace();
    MPListViewItem *item = new MPListViewItem( columnSet->lv->firstChild(), vpidstr, host, tidstr );
    DescriptionClassObject *dco = new DescriptionClassObject(FALSE, pset_name, host, vpidstr);
    item->descriptionClassObject = dco;
  }
}
}

void
CompareProcessesDialog::removeProcesses()
{
printf("removeProcesses(%s)\n", removeProcessesRegExpLineEdit->text().ascii() );


QString target_pidstr = QString::null;
QString inputText = removeProcessesRegExpLineEdit->text().stripWhiteSpace();

QStringList fields = QStringList::split( ",", inputText );
for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
{
  target_pidstr = ((QString)*it).stripWhiteSpace();

  QRegExp pidRegExp = QRegExp(target_pidstr, TRUE, TRUE);

printf("target_pidstr = (%s)\n", target_pidstr.ascii() );

  if( target_pidstr.find(":") > -1 )
  {
printf("Found a range!\n");
   
  }
  if( target_pidstr.find("*") > -1 )
  {
printf("Found a wildcard!\n");
  }
  // Loop through and attempt to find and delete this item.
  QListViewItemIterator it( columnSet->lv );
  it++;
  while ( it.current() )
  {
    QListViewItem *item = it.current();
    QString pidstr = item->text(0).stripWhiteSpace();
    printf("Item: (%s)\n", pidstr.ascii() );
    ++it;
    if( pidstr.find(pidRegExp) != -1 )
    {
printf("REMOVE: (%s)\n", pidstr.ascii() );
      delete item;
    }
  }


}


}


void
CompareProcessesDialog::buttonOkSelected()
{
printf("buttonOkSelected() entered\n");

  applyOkSelected();

  hide();
}


void
CompareProcessesDialog::applyOkSelected()
{
printf("applyOkSelected\n");

  // first add the processes selected.
  if( !addProcessesRegExpLineEdit->text().isEmpty() )
  {
    addProcessesRegExpLineEditEntered();
  } 
  // Now clean any up that were specifically unselected. 
  if( !removeProcessesRegExpLineEdit->text().isEmpty() )
  {
    removeProcesses();
  }
//  if( !addProcessesHostRegExpLineEdit->text().isEmpty() )
//  {
//    addProcessesHostRegExpLineEditEntered();
//  }
}

void
CompareProcessesDialog::updateFocus(int _expID, CompareClass *_compareClass, CompareSet *_compareSet, ColumnSet *_columnSet )
{
  compareClass = _compareClass;
  compareSet = _compareSet;
  columnSet = _columnSet;
  expID = _expID;

  if( expID == -1 || compareSet == NULL || compareSet == NULL )
  {
    return;
  }

  headerLabel->setText( QString("Modify Compare Set %1: Column %2").arg(compareSet->name).arg(columnSet->name) );

  compareSet->updatePSetList();
}

QStringList
CompareProcessesDialog::validatePid(QString target_host, QString target_pidstr)
{
  QStringList vpidlist;

  printf("validatePid (%s:%s) \n", target_host.ascii(), target_pidstr.ascii() );

  QRegExp pidRegExp = QRegExp(target_pidstr, TRUE, TRUE);
  QRegExp hostRegExp = QRegExp(target_host, TRUE, TRUE);

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
        std::sort(v.begin(), v.end());

        std::vector<std::string>::iterator e 
                        = unique(v.begin(), v.end());

        for( std::vector<string>::iterator hi = v.begin(); hi != e; hi++ ) 
        {
          QString pset_name = QString(*hi);
QString host_name = QString(*hi);
printf("hi=(%s)\n", hi->c_str() );
if( !host_name.isEmpty()  && host_name.find(hostRegExp) == -1 )
{
  continue;
}
printf("We're on the right target host.\n");
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
// if( pidstr != target_pidstr )
if( pidstr.find(pidRegExp) == -1 )
{
  continue;
}
printf("Found!\n");
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
printf("host_name=(%s) tidstr=(%s)\n", host_name.ascii(), tidstr.ascii() );
vpidlist.append(tidstr);
              } else if( !ridstr.isEmpty() )
              {
vpidlist.append(ridstr);
printf("host_name=(%s) ridstr=(%s)\n", host_name.ascii(), ridstr.ascii() );
              } else
              {
vpidlist.append(pidstr);
printf("host_name=(%s) pidstr=(%s)\n", host_name.ascii(), pidstr.ascii() );
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

  return( vpidlist );
}
