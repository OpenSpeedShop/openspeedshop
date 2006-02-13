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

#include "CompareClass.hxx"
#include "CompareSet.hxx"
#include "ColumnSet.hxx"

#include "SS_Input_Manager.hxx"
CompareProcessesDialog::CompareProcessesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() constructor called.\n");

  QPixmap *plus_pm = new QPixmap( plus_xpm );
  plus_pm->setMask(plus_pm->createHeuristicMask());


  QPixmap *minus_pm = new QPixmap( minus_xpm );
  minus_pm->setMask(plus_pm->createHeuristicMask());

  
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
  
  QHBoxLayout *addProcessesLayout = new QHBoxLayout( 6, "addProcessesLayout");
  CompareProcessesDialogLayout->addLayout( addProcessesLayout );
  
  addProcessesLabel = new QLabel(this, "addProcessesLabel");
  addProcessesLayout->addWidget( addProcessesLabel );

  addProcessesRegExpLineEdit = new QLineEdit(this, "addProcessesRegExpLineEdit");
  addProcessesLayout->addWidget( addProcessesRegExpLineEdit );
  QToolTip::add(addProcessesRegExpLineEdit, tr("Select which process(es), on the selected host, should be added.\nThis can be a single processes name or a comma separated list of processes names.\nRegular expressions will be honored.(Eventually)") );

//  QToolTip::add(addProcessesRegExpLineEdit, tr("Enter the pid (or regular expression defining the pids) that you want entered into\nthe current Column in the current Compare Set of the Compare Panel.\n\nDrag and drop, psets or individual processes from here to the Compare Panel.") );

QPushButton *addButton = new QPushButton( this, "addButton" );
addButton->setText("Add");
addButton->setPixmap(plus_xpm);
addProcessesLayout->addWidget( addButton );

QPushButton *removeButton = new QPushButton( this, "removeButton" );
removeButton->setText("Remove");
		removeButton->setPixmap(minus_xpm);
addProcessesLayout->addWidget( removeButton );


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


  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );
  CompareProcessesDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

  connect( addButton, SIGNAL( clicked() ), this, SLOT( addProcesses() ) );
  connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeProcesses() ) );
  
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
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
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
CompareProcessesDialog::addProcesses()
{
// printf("addProcesses(%s)\n", addProcessesRegExpLineEdit->text().ascii() );

  QString inputText = addProcessesRegExpLineEdit->text();

  QString pset_name = QString::null;
  QString host = QString::null;
  QString host_pidstr = addProcessesRegExpLineEdit->text().stripWhiteSpace();
  QString tidstr = QString::null;
  QString collector_name = QString::null;


  host = "Unknown";

  QStringList fields = QStringList::split( ",", inputText );
  for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
  {
    host_pidstr = ((QString)*it).stripWhiteSpace();

// printf("host_pidstr = (%s)\n", host_pidstr.ascii() );

    if( host_pidstr.find("-") > -1 )
    {
// printf("Found a range!\n");
    }
    if( host_pidstr.find(":") > -1 )
    {
// printf("Found a host!\n");
    }
    if( host_pidstr.find("*") > -1 )
    {
// printf("Found a wildcard!\n");
    }
    DescriptionClassObjectList *validatedHostPidList = validateHostPid(host_pidstr);

    if( validatedHostPidList )
    {
      for ( DescriptionClassObjectList::Iterator it = validatedHostPidList->begin(); it != validatedHostPidList->end(); ++it )
      {
        DescriptionClassObject *dco = (DescriptionClassObject *)*it;
        MPListViewItem *item = new MPListViewItem( columnSet->lv->firstChild(), dco->pid_name, dco->host_name, tidstr );
        item->descriptionClassObject = dco;
      }
  
      delete validatedHostPidList;
    }
  }
}

void
CompareProcessesDialog::removeProcesses()
{
// printf("removeProcesses(%s)\n", addProcessesRegExpLineEdit->text().ascii() );

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
    if( colon_index > -1 )
    {
      target_hoststr =  target_hostpid_str.left(colon_index).stripWhiteSpace();
    
      int length = target_hostpid_str.length();
      length--; // We want to skip the ":"
      target_pidstr = target_hostpid_str.right(length-colon_index).stripWhiteSpace();
    } else
    {
      target_pidstr = target_hostpid_str.stripWhiteSpace();
    }
  
    QRegExp hostRegExp = QRegExp(target_hoststr, TRUE, TRUE);
    QRegExp pidRegExp = QRegExp(target_pidstr, TRUE, TRUE);
  
// printf("target_pidstr = (%s)\n", target_pidstr.ascii() );

    if( target_pidstr.find(":") > -1 )
    {
// printf("Found a host!\n");
    }

    // Do we have a range of pids?
    lower_range = -1;
    upper_range = -1;
    int dash_index = target_pidstr.find("-");
    if( dash_index > -1 )
    {
// printf("Found a range!\n");
      int lb_index = target_pidstr.findRev("[", dash_index);
      int rb_index = target_pidstr.findRev("]", dash_index);
      if( rb_index > lb_index || lb_index == -1 )
      {
// printf("There's a RANGE of processes to remove!\n");
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
      }
    }

// printf("target_hoststr = (%s)\n", target_hoststr.ascii() );
// printf("target_pidstr = (%s)\n", target_pidstr.ascii() );
// printf("lower_rangestr=(%s)\n", lower_rangestr.ascii() );
// printf("upper_rangestr=(%s)\n", upper_rangestr.ascii() );

   
    if( target_pidstr.find("*") > -1 )
    {
// printf("Found a wildcard!\n");
    }
    // Loop through and attempt to find and delete this item.
    QListViewItemIterator it( columnSet->lv );
    it++;
    while ( it.current() )
    {
      QListViewItem *item = it.current();
  
      QString pidstr = item->text(0).stripWhiteSpace();
// printf("Item: (%s)\n", pidstr.ascii() );
      ++it;
      QString host_name = item->text(1).stripWhiteSpace();
      if( !host_name.isEmpty()  && host_name.find(hostRegExp) == -1 )
      {
        continue;
      }
// printf("pidstr=(%s)\n", pidstr.ascii() );
      if( lower_range >= 0 && upper_range >= 0 )
      {
        int pid = pidstr.toInt();
        if( pid < lower_range || pid > upper_range )
        {
          continue;
        }
      } else
      {
        if( pidstr.find(pidRegExp) == -1 )
        {
          continue;
        }
      }
      delete item;
    }
  }
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

DescriptionClassObjectList *
CompareProcessesDialog::validateHostPid(QString target_host_pidstr)
{
  DescriptionClassObjectList *dcolist = new DescriptionClassObjectList();

// printf("validateHostPid (%s) \n", target_host_pidstr.ascii() );

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
      printf("There's a RANGE!\n");
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

// printf("target_hoststr = (%s)\n", target_hoststr.ascii() );
// printf("target_pidstr = (%s)\n", target_pidstr.ascii() );
// printf("lower_rangestr=(%s)\n", lower_rangestr.ascii() );
// printf("upper_rangestr=(%s)\n", upper_rangestr.ascii() );

  QRegExp hostRegExp = QRegExp(target_hoststr, TRUE, TRUE);
  QRegExp pidRegExp = QRegExp(target_pidstr, TRUE, TRUE);

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
// printf("hi=(%s)\n", hi->c_str() );
if( !host_name.isEmpty()  && host_name.find(hostRegExp) == -1 )
{
  continue;
}
// printf("We're on the right target host.\n");
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
// printf("pidstr=(%s)\n", pidstr.ascii() );
              if( lower_range > 0 && upper_range > 0 )
              {
                int pid = pidstr.toInt();
                if( pid < lower_range || pid > upper_range )
                {
                  continue;
                }
              } else
              {
                if( pidstr.find(pidRegExp) == -1 )
                {
                  continue;
                }
              }
// printf("Found!\n");
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
// printf("host_name=(%s) tidstr=(%s)\n", host_name.ascii(), tidstr.ascii() );
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, tidstr  );
                dcolist->append(dco);
              } else if( !ridstr.isEmpty() )
              {
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, ridstr  );
                dcolist->append(dco);
// printf("host_name=(%s) ridstr=(%s)\n", host_name.ascii(), ridstr.ascii() );
              } else
              {
                DescriptionClassObject *dco = new DescriptionClassObject(FALSE, QString::null, host_name, pidstr  );
                dcolist->append(dco);
// printf("host_name=(%s) pidstr=(%s)\n", host_name.ascii(), pidstr.ascii() );
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
