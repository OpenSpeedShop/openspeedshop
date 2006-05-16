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
  

#include "CustomizeClass.hxx"

#include "debug.hxx"

#include <qapplication.h>
#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <MPListViewItem.hxx>
#include <qinputdialog.h>
#include <qmenubar.h>
#include <qvaluelist.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qheader.h>
#include <qtooltip.h>

#include <qtoolbox.h>

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusCompareObject.hxx"
#include "UpdateObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"
#include "CompareProcessesDialog.hxx"


CustomizeClass::CustomizeClass( Panel *_p, QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id, QString focusedExpIDStr )
    : QWidget( parent, name )
{
//  nprintf(DEBUG_CONST_DESTRUCT) ("CustomizeClass::CustomizeClass() constructor called.\n");
  dprintf("CustomizeClass::CustomizeClass() constructor called.\n");
//  ccnt = 0;
  ccnt = 1;
  
  p = _p;
  if( focusedExpIDStr.isEmpty() )
  {
    focusedExpID = -1;
  } else
  {
    focusedExpID = focusedExpIDStr.toInt();
  }
  expID = exp_id;

  csl = NULL;
  dialog = NULL;

  if ( !name ) setName( "CustomizeClass" );

  QVBoxLayout *mainCompareLayout = new QVBoxLayout( this, 1, 1, "mainCompareLayout");
  QLabel *header = new QLabel(this, "header");
  header->setText("Customize StatsPanel Factory:");
  header->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  QToolTip::add(header, tr("The Customize StatsPanel Factory is the interface that allows you to compare one set\nof process/thread to another -or- one experiment run against another.\n\nIt allows you manage columns of data that will be displayed in the StatsPanel.\nEach cset defined can have mulitiple columns defined.   Each coloumn can have an\nindividual collector/metric/modifier displayed.\n\nFirst you create the CompareSet (cset) definition, using this panel, then select\n\"Focus on this CSET\" and the StatsPanel (and eventually the SourcePanel) will be\nupdated with the selected statistics.") );
  mainCompareLayout->addWidget(header);

// I'm not sure this header makes much sense here.   Hide for now.
header->hide();

  compareClassLayout = new QVBoxLayout( mainCompareLayout, 1, "compareClassLayout"); 

  // Vertical list of compare sets (set of psets) defined by the user.
  // this simple defaults to "All process/threads, as if this pane never
  // existed.
  csetTB = new QToolBox( this, "listOfCompareSets");

  compareClassLayout->addWidget( csetTB );

  updateInfo();

  compareList.clear();

  addNewCSet();


  languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CustomizeClass::~CustomizeClass()
{
// printf("CustomizeClass() destructor called\n");

  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); )
  {
    CompareSet *cs = (CompareSet *)*it;
// printf("delete cs\n");
    delete cs;
    ++it;
  }
  csl->clear();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CustomizeClass::languageChange()
{
  setCaption( tr( "Customize StatsPanel Class" ) );
}

bool
CustomizeClass::menu(QPopupMenu* contextMenu)
{
  CompareSet *currentCompareSet = findCurrentCompareSet();
  QString currentCompareSetString = QString::null;
  QString currentColumnString = QString::null;

  if( currentCompareSet )
  {
    currentCompareSetString = QString(" (%1)").arg(currentCompareSet->name);
    QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();
    QWidget *currentTab = currentTabWidget->currentPage();
    currentColumnString = QString(" (%1)").arg(currentTabWidget->tabLabel( currentTab ) );
  }


  QAction *qaction = NULL;

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "updatePanel");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Update...") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( updatePanel() ) );
  qaction->setStatusTip( tr("Update the information in this panel.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "focusOnProcess");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Focus Stats Panel with defined info...")+currentCompareSetString) );
  qaction->setText( tr(QString("Focus StatsPanel")) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( focusOnCSetSelected() ) );
  qaction->setStatusTip( tr("Focus the StastPanel on defined set of information.") );


  contextMenu->insertSeparator();

#ifdef NONWALKING
  qaction = new QAction( this,  "addNewColumn");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Add column to Compare Set...")+currentCompareSetString) );
  qaction->setText( tr(QString("Add column") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewColumn() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current set.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Remove%1 from Compare Set...").arg(currentColumnString)) );
  qaction->setText( tr(QString("Remove column %1").arg(currentColumnString)) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes the raised column (tab).") );
#else // NONWALKING

  QPopupMenu *addDeleteMenu = new QPopupMenu(this);
  contextMenu->setCaption("Add/Delete Columns");
  contextMenu->insertItem("Add/Delete Columns", addDeleteMenu);

  qaction = new QAction( this,  "addNewColumn");
  qaction->addTo( addDeleteMenu );
//  qaction->setText( tr(QString("Add column to Compare Set...")+currentCompareSetString) );
  qaction->setText( tr(QString("Add column") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewColumn() ) );
  qaction->setStatusTip( tr("Adds an additional column (tab) to the definition of the current set.") );

  qaction = new QAction( this,  "removeRaisedTab");
  qaction->addTo( addDeleteMenu );
//  qaction->setText( tr(QString("Remove%1 from Compare Set...").arg(currentColumnString)) );
  qaction->setText( tr(QString("Remove column %1").arg(currentColumnString)) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeRaisedTab() ) );
  qaction->setStatusTip( tr("Removes the raised column (tab).") );
#endif // NONWALKING

  contextMenu->insertSeparator();


  qaction = new QAction( this,  "addProcessesSelected");
  qaction->addTo( contextMenu );
//  qaction->setText( tr(QString("Select process(es) for: ")+currentCompareSetString+currentColumnString) );
  qaction->setText( tr(QString("Add Process(es)...") ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addProcessesSelected() ) );
//  qaction->setStatusTip( tr("Select processes to be added or removed from the current column of current cset.") );
  qaction->setStatusTip( tr("Add -and/or- remove processes from the set.") );

  qaction = new QAction( this,  "removeUserPSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Remove process(es)") ) );
//  qaction->setText( tr(QString("Remove process/process set..")+currentColumnString) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeUserPSet() ) );
  qaction->setStatusTip( tr("Removes process(es) from the set.") );

  contextMenu->insertSeparator();

#ifdef DONT_PULL
  qaction = new QAction( this,  "addNewCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr("Add Compare Set") );
  connect( qaction, SIGNAL( activated() ), this, SLOT( addNewCSet() ) );
  qaction->setStatusTip( tr("Creates a new Compare Set  to be modified.") );

  qaction = new QAction( this,  "removeCSet");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Remove Compare Set")+currentCompareSetString) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( removeCSet() ) );
  qaction->setStatusTip( tr("Removes this currently raised Compare Set.") );

  contextMenu->insertSeparator();
#endif // DONT_PULL

  qaction = new QAction( this,  "loadAdditionalExperiment");
  qaction->addTo( contextMenu );
  qaction->setText( tr(QString("Load another (additional) experiment's data.")  ) );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadAdditionalExperimentSelected() ) );
  qaction->setStatusTip( tr("Loads another experiment's data.") );


  return( TRUE );
}

void
CustomizeClass::addNewCSet()
{
  CompareSet *cset = new CompareSet( csetTB, this);
  
  if( !csl )
  {
    csl = new CompareSetList();
  }
  
  csl->push_back(cset);
}

// Entry point from the menu.  You'll need to lookup the tabWidget
void
CustomizeClass::addNewColumn()
{
  CompareSet *compareSet = findCurrentCompareSet();

  addNewColumn(compareSet);
} 

void
CustomizeClass::addNewColumn(CompareSet *compareSet)
{
  ColumnSet *columnSet = new ColumnSet(this, compareSet);

  compareSet->columnSetList.push_back( columnSet );

  compareSet->setNewFocus();
}

void
CustomizeClass::updatePanel()
{
  updateInfo();
}

void
CustomizeClass::focusOnCSetSelected()
{
// printf("CustomizeClass::focusOnCSetSelected() entered\n");

  QValueList<int64_t> cIntList;

  FocusCompareObject *focus_msg = NULL;
  QString cViewCreateCommand = "cViewCreate ";
  QString temp_expCompareCommand = "cViewCreate ";

  CompareSet *compareSet = findCurrentCompareSet();
  if( compareSet )
  {
    cIntList.clear();
// printf("CompareSet: (%s)'s info\n", compareSet->name.ascii() );
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
// printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
// printf("\t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
// printf("\t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
// printf("\t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );

      {
      int id = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());
      QString collectorName = columnSet->collectorComboBox->currentText().ascii();
      QString metricName = columnSet->metricComboBox->currentText().ascii();
      cViewCreateCommand += QString("-x %1 %2 -m %3 ").arg(id).arg(collectorName).arg(metricName);

      if( temp_expCompareCommand.isEmpty() )
      {
        temp_expCompareCommand += QString("%2 -m %3 ").arg(collectorName).arg(metricName);
      }
      }


// printf("\t\t: processes:\n");
      QString expCompareProcessList = QString::null;
      QString temp_expCompareProcessList = QString::null;




// Begin real focus logic
// printf("CustomizeClass::focusOnCSetSelected() entered.\n");

      QString pid_name = QString::null;
      QString pidString = QString::null;
 
      QListViewItemIterator it(columnSet->lv);
      expCompareProcessList = QString::null;
      temp_expCompareProcessList = QString::null;
      while( it.current() )
      {
        MPListViewItem *lvi = (MPListViewItem *)it.current();
// printf("PSetSelection: lvi->text(0)=(%s)\n", lvi->text(0).ascii() );
// printf("lvi->text(0) =(%s)\n", lvi->text(0).ascii() );
// printf("lvi->text(1) =(%s)\n", lvi->text(1).ascii() );
// if( lvi->descriptionClassObject )
// {
//  lvi->descriptionClassObject->Print();
// }
        if( focus_msg == NULL )
        {
          focus_msg = new FocusCompareObject(expID,  NULL, TRUE);
        }
        if( !lvi || !lvi->descriptionClassObject )
        {
          ++it;
          continue;
//      QMessageBox::information( this, tr("Focus Error:"), tr("Unable to focus on selection: No description for process(es)."), QMessageBox::Ok );
//      return;
        }

        if( lvi->descriptionClassObject->all )
        {
// printf("Do ALL threads, everywhere.\n");
//        focus_msg->host_pid_vector.clear();
        } else if( lvi->descriptionClassObject->root )
        {
          // Loop through all the children...
// printf("Loop through all the children.\n");
          MPListViewItem *mpChild = (MPListViewItem *)lvi->firstChild();
          while( mpChild )
          {
            QString host_name = mpChild->descriptionClassObject->host_name;
            if( host_name.isEmpty() )
            {
              host_name = "localhost";
            }
            QString pid_name = mpChild->descriptionClassObject->pid_name;
            if( pid_name.isEmpty() )
            {
              mpChild = (MPListViewItem *)mpChild->nextSibling();
              continue;
            }
            std::pair<std::string, std::string> p(host_name,pid_name);
//        focus_msg->host_pid_vector.push_back( p );
// printf("A: push_back a new host:pid entry (%s:%s)\n", host_name.ascii(), pid_name.ascii());
            expCompareProcessList += QString(" -h %1 -p %1 ").arg(host_name).arg(pid_name);
            temp_expCompareProcessList += QString(" -p %1 ").arg(pid_name);
            mpChild = (MPListViewItem *)mpChild->nextSibling();
            ++it;
          }
        } else
        {
          QString host_name = lvi->descriptionClassObject->host_name;
          if( host_name.isEmpty() )
          {
            host_name = "localhost";
          }
          QString pid_name = lvi->descriptionClassObject->pid_name;
          if( pid_name.isEmpty() )
          {
            continue;
          }
          std::pair<std::string, std::string> p(host_name,pid_name);
// printf("B: push_back a new host::pid entry... (%s:%s)\n", host_name.ascii(), pid_name.ascii() );
          expCompareProcessList += QString(" -h %1 -p %1 ").arg(host_name).arg(pid_name);
          temp_expCompareProcessList += QString(" -p %1 ").arg(pid_name);
//      focus_msg->host_pid_vector.push_back( p );
        } 
        ++it;
      }


      // If nothing was selected, just return.
      if( !focus_msg )
      {
        QMessageBox::information( this, tr("Error process selection:"), tr("Unable to focus: No processes selected."), QMessageBox::Ok );
        if( focus_msg )
        {
          delete focus_msg;
        }
        return;
      }


      cViewCreateCommand += expCompareProcessList;
      focus_msg->compare_command = cViewCreateCommand;


// printf("I think you really want this compare command:\n(%s)\n", cViewCreateCommand.ascii() );

      {  //Begin build the actual compare commands and store the id's from them
         // to send to the StatsPanel
      CLIInterface *cli = p->getPanelContainer()->getMainWindow()->cli;
      int64_t val = 0;
      bool mark_value_for_delete = true;
      QString command = QString(cViewCreateCommand);
// printf("command=(%s)\n", command.ascii() );
      if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete   ) )
      {
        printf("Unable to creat cview for %s\n", command.ascii() );
        return;
      }
// printf("pushback %d\n", val);
      cIntList.push_back(val);
      // Now start over...
      cViewCreateCommand = "cViewCreate ";
      }

      temp_expCompareCommand += temp_expCompareProcessList;
      focus_msg->compare_command = temp_expCompareCommand;

    }

    focus_msg->compare_command = QString("cview -c ");
    QValueList<int64_t>::Iterator cit;
    int count = 0;
    for( cit = cIntList.begin(); cit != cIntList.end(); ++cit )
    {
      int64_t cval = (int64_t)*cit;
  
      if( count > 0 )
      {
        focus_msg->compare_command += QString(", ");
      }
      focus_msg->compare_command += QString("%1").arg(cval);
      count++;
    }

// printf("Really send this : command: (%s)\n", focus_msg->compare_command.ascii() );

// printf("A: focus the StatsPanel...\n");
    QString name = QString("Stats Panel [%1]").arg(expID);
// printf("find a stats panel named (%s)\n", name.ascii() );
    Panel *sp = p->getPanelContainer()->findNamedPanel(p->getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    if( !sp )
    {
// printf("Didn't find a stats panel.... Create one.\n");
      char *panel_type = "Stats Panel";
      PanelContainer *bestFitPC = p->getPanelContainer()->getMasterPC()->findBestFitPanelContainer(p->getPanelContainer());
      ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
      sp = p->getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
      delete ao;
      if( sp != NULL )
      {
// printf("Created a stats panel... First update it's data... expID=%d\n", expID);
      sp->listener( (void *)focus_msg );
      }
    } else
    {
// printf("There was a statspanel... send the update message.\n");
      sp->listener( (void *)focus_msg );
    }
// End real focus logic
  }

}

void
CustomizeClass::addProcessesSelected()
{
// printf("CustomizeClass::addProcessesSelected() entered\n");


  QApplication::setOverrideCursor(QCursor::WaitCursor);
  if( dialog == NULL )
  {
    dialog = new CompareProcessesDialog(this, "Add/Delete/Describe Compare Processes Dialog");
  }
  QApplication::restoreOverrideCursor();

  CompareSet *compareSet = findCurrentCompareSet();

  ColumnSet *columnSet = NULL;
{

  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  QWidget *currentTab = currentTabWidget->currentPage();

  // Look for the tab in the CompareSet.   The delete it from the compareSet's
  // list.
  if( compareSet )
  {
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) )
      {
        columnSet = cs;
        break;
      }
    }

  }


}

  dialog->updateInfo();
  int expid = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());

// printf("Focus on expID=%d\n",  expid );
// printf("CustomizeClass=%s\n",  name() );
// printf("compareSet=%s\n",  compareSet->name.ascii() );
// printf("columnSet=(%s)\n", columnSet->name.ascii() );

  dialog->updateFocus(expid, columnSet->lv);
  compareSet->updatePSetList();

  dialog->show();

  QApplication::restoreOverrideCursor();
}

void
CustomizeClass::loadAdditionalExperimentSelected()
{
// printf("CustomizeClass::loadAdditionalExperimentSelected() entered\n");
  QString fn = QString::null;
  char *cwd = get_current_dir_name();
  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;Any Files(*.*)", this, "open experiment dialog", "Choose an experiment file to open for comparison");
  free(cwd);
  if( !fn.isEmpty() )
  {
//      printf("fn = %s\n", fn.ascii() );
//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
    p->getPanelContainer()->getMainWindow()->executableName = QString::null;
    p->getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn, FALSE);
    updateInfo();
  } else
  {
    fprintf(stderr, "No experiment file name given.\n");
  }
}

void
CustomizeClass::removeCSet()
{
  QWidget *currentItem = csetTB->currentItem();

  CompareSet *cs_to_delete = findCurrentCompareSet();

  // If we removed a CompareSet from the list, it should be free
  // to delete.  Delete it.
  if( cs_to_delete )
  {
// printf("Don't forget to remove all the tabs!\n");
    csl->remove( cs_to_delete );
    csetTB->removeItem( currentItem );
    delete cs_to_delete;
  }
}

void
CustomizeClass::removeRaisedTab()
{
// printf("CustomizeClass::removeRaisedTab() entered\n");
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();

  CompareSet *cs_to_search = findCurrentCompareSet();

  QWidget *currentTab = currentTabWidget->currentPage();

// printf("remove Tab labeled (%s)\n", currentTabWidget->tabLabel( currentTab ).ascii() );

  
// printf("NOW FIND AND REMOVE THIS TAB FROM THE columnList!\n");

  // Look for the tab in the CompareSet.   The delete it from the compareSet's
  // list.
  if( cs_to_search )
  {
// printf("Don't forget to remove all the tabs!\n");
    ColumnSetList::Iterator it;
    for( it = cs_to_search->columnSetList.begin(); it != cs_to_search->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) )
      {
// printf("Okay I'm sure you need to delete this (%s) tab\n", cs->name.ascii() );
        cs_to_search->columnSetList.remove( cs );
        currentTabWidget->removePage(currentTab);
        delete cs;
        break;
      }
    }

  }


  
}

void
CustomizeClass::removeUserPSet()
{
  // First find the current (raised) cset.\n");
  QTabWidget *currentTabWidget = (QTabWidget* )csetTB->currentItem();
// printf("removeUserPSet()\n");

  QWidget *currentTab = currentTabWidget->currentPage();

  CompareSet *compareSet = findCurrentCompareSet();

  if( compareSet )
  {
// printf("compareSet (%s)\n", compareSet->name.ascii() );
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *cs = (ColumnSet *)*it;
      if( cs->name == currentTabWidget->tabLabel( currentTab ) )
      {
        ColumnSet *columnSet = cs;
        QListViewItem *item = columnSet->lv->currentItem();
        if( item )
        {
// printf("\t: Delete: ColumnSet (%s)'s info item=(%s)\n", columnSet->name.ascii(), item->text(0).ascii() );
          delete item;
        }
// printf("Now check for first child\n");
        // If we've deleted everything, simply add back the default entry.
        if( !columnSet->lv->firstChild() )
        {
// printf("No firstChild...add the default back in.\n");
          MPListViewItem *cps_item = new MPListViewItem( columnSet->lv, CPS);
          cps_item->setOpen(TRUE);
        }
        break;
      }
    }

  }

}


void
CustomizeClass::updateInfo()
{
// printf("CustomizeClass::updateInfo() entered\n");
  if( csl )
  {
    CompareSetList::Iterator it;
    for( it = csl->begin(); it != csl->end(); ++it )
    {
      CompareSet *cs = (CompareSet *)*it;
// printf("attempt to update (%s)'s info\n", cs->name.ascii() );
      cs->updateInfo();
    }
  }
}

CompareSet *
CustomizeClass::findCurrentCompareSet()
{
  QWidget *currentItem = csetTB->currentItem();

  int  currentIndex = csetTB->currentIndex();

  CompareSetList::Iterator it;
  for( it = csl->begin(); it != csl->end(); ++it )
  {
    CompareSet *cs = (CompareSet *)*it;
    if( cs->name == csetTB->itemLabel(currentIndex) )
    {
      return( cs );
    }
  }


 return( NULL );
}

