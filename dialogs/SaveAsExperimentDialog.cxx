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
  

#include "SaveAsExperimentDialog.hxx"

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
#include <qapplication.h>

#include "SS_Input_Manager.hxx"
SaveAsExperimentDialog::SaveAsExperimentDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("SaveAsExperimentDialog::SaveAsExperimentDialog() constructor called.\n");

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;
  
  if ( !name ) setName( "SaveAsExperimentDialog" );

  setSizeGripEnabled( TRUE );
  SaveAsExperimentDialogLayout = new QVBoxLayout( this, 11, 6, "SaveAsExperimentDialogLayout"); 

#ifdef PULL
  hostLabel = new QLabel( this, "hostLabel" );
  SaveAsExperimentDialogLayout->addWidget( hostLabel );
  hostComboBox = new QComboBox( this, "hostComboBox");
  hostComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, hostComboBox->sizePolicy().hasHeightForWidth() ) );
  hostComboBox->setEditable(TRUE);
  SaveAsExperimentDialogLayout->addWidget( hostComboBox );
#endif // PULL

  availableExperimentsListView = new QListView( this, "availableExperimentsListView" );
  availableExperimentsListView->addColumn( tr( "id:" ) );
  availableExperimentsListView->addColumn( tr( "Name:" ) );
  availableExperimentsListView->addColumn( tr( "Description:" ) );
  availableExperimentsListView->setSelectionMode( QListView::Single );
  availableExperimentsListView->setShowSortIndicator( TRUE );
  availableExperimentsListView->setSorting( 0, FALSE );
  availableExperimentsListView->setAllColumnsShowFocus(TRUE);
  availableExperimentsListView->setSortOrder( Qt::Ascending );
  availableExperimentsListView->setRootIsDecorated(TRUE);
  SaveAsExperimentDialogLayout->addWidget( availableExperimentsListView );


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
  SaveAsExperimentDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
#ifdef PULL
  connect( hostComboBox, SIGNAL( activated(const QString &) ), this, SLOT( hostComboBox() ) );
#endif // PULL

  updateAvailableExperimentList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SaveAsExperimentDialog::~SaveAsExperimentDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("SaveAsExperimentDialog::SaveAsExperimentDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SaveAsExperimentDialog::languageChange()
{
  setCaption( tr( name() ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
#ifdef PULL
  hostLabel->setText( tr("Host:") );
  hostComboBox->insertItem( "localhost" );
  hostComboBox->insertItem( "clink.americas.sgi.com" );
  hostComboBox->insertItem( "hope.americas.sgi.com" );
  hostComboBox->insertItem( "hope1.americas.sgi.com" );
  hostComboBox->insertItem( "hope2.americas.sgi.com" );
#endif // PULL
}

PanelListViewItem *
SaveAsExperimentDialog::selectedExperiment(int *expID)
{
  PanelListViewItem *selectedItem = (PanelListViewItem *)availableExperimentsListView->selectedItem();


  if( selectedItem )
  {

// First get the parent node...
    QListViewItem *parent_node = availableExperimentsListView->selectedItem();
    while( parent_node->parent() )
    {
      nprintf( DEBUG_PANELS ) ("looking for 0x%x\n", parent_node->parent() );
      parent_node = parent_node->parent();
    }
    nprintf( DEBUG_PANELS ) ("parent text=%s\n", parent_node->text(0).ascii() );

    *expID = parent_node->text(0).toInt();

    nprintf( DEBUG_PANELS ) ("Got an ITEM!\n");
    // If the user selected a leaf, just return it...
    if( selectedItem->parent() )
    {
      return selectedItem;
    }
    PanelListViewItem *firstChild = (PanelListViewItem *)selectedItem->firstChild();
    if( firstChild )
    {
      return firstChild;
    } else
    {
return selectedItem;
      return NULL; // Error condition.
    }
  } else
  {
    nprintf( DEBUG_PANELS ) ("NO ITEMS SELECTED\n");
    return( NULL );
  }
}



#include "PanelContainer.hxx"
void
SaveAsExperimentDialog::updateAvailableExperimentList()
{
#ifdef PULL
  char *host = (char *)hostComboBox->currentText().ascii();
#endif // PULL

  availableExperimentsListView->clear();

  QString command("listExp");
  std::list<int64_t> int_list;

  int_list.clear();
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &int_list ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

  std::list<int64_t>::iterator it;
  nprintf( DEBUG_PANELS ) ("int_list.size() =%d\n", int_list.size() );
  for(it = int_list.begin(); it != int_list.end(); it++ )
  {
    int64_t expID = (int64_t)(*it);

    nprintf( DEBUG_PANELS ) ("Here are the experiment ids that can be saved (%d)\n", expID);
QString expName = QString::null;
ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
Experiment *fw_experiment = NULL;
if( eo && eo->FW() )
{
  fw_experiment = eo->FW();
  CollectorGroup cgrp = fw_experiment->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++)
  {
    Collector collector = *ci;
    std::string name = collector.getMetadata().getUniqueId();
    if( !expName.isEmpty() )
    {
      expName += QString(" ,");
    }
    expName += QString(name.c_str());
  }
}
    QListViewItem *item = new QListViewItem( availableExperimentsListView,
      QString("%1").arg(expID),
      expName,
      fw_experiment ? fw_experiment->getName().c_str() : "Unknown experiment name" );

#ifdef PULL
      PanelList *panelList = mw->topPC->getPanelListByID(expID);
      if( panelList )
      {
        for( PanelList::Iterator pit = panelList->begin(); pit != panelList->end(); pit++ )
        {
          Panel *p = (Panel *)*pit;
          (void) new PanelListViewItem( (QListViewItem *)item, p->getName(), p );
        }
        delete panelList;
      }
#endif // PULL
  }

  QApplication::restoreOverrideCursor();
}

#ifdef PULL
void SaveAsExperimentDialog::attachHostComboBoxActivated()
{
    updateAvailableExperimentList();
}
#endif // PULL
