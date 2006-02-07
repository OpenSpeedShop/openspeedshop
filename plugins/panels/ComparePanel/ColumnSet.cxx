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


#include "ColumnSet.hxx"
#include "CompareSet.hxx"
#include "CompareClass.hxx"
#include "MPListView.hxx"

#include <qapplication.h>
#include <qtranslator.h>

#include <qtoolbox.h>
#include <qtooltip.h>
#include <qtabwidget.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qheader.h>
#include <qstring.h>
#include <qmessagebox.h>

#define __TR QString



#include "debug.hxx"

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusObject.hxx"


ColumnSet::ColumnSet(QWidget *w, CompareSet *cc) : QWidget(0, "columnSetObject")
{
// printf("ColumnSet::ColumnSet() constructor called.\n");


  clo = NULL;
  ce = NULL;

  compareSet = cc;

  name = QString("Column #%1").arg(compareSet->tcnt);

  QFrame *frame = new QFrame(compareSet->tabWidget, QString("frame%1").arg(compareSet->tcnt) );
  QVBoxLayout *TBlayout = new QVBoxLayout( frame, 1, 1, QString("TBlayout%1").arg(compareSet->tcnt) );


{ // Here begins the section to set up the experiment combobox
// if( experiment_list.size() > 1 )
{
  QHBoxLayout *experimentLayout = new QHBoxLayout( TBlayout, 1, "experimentLayout" );

  QLabel *cbl = new QLabel(frame, "experimentComboBoxLabel");
  cbl->setText( __TR("Available Experiments:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(cbl);

  experimentComboBox = new QComboBox(FALSE, frame, "experimentComboBox");
  experimentComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  connect( experimentComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeExperiment( const QString & ) ) );

  QToolTip::add(experimentComboBox, __TR("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(experimentComboBox);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Preferred, QSizePolicy::Fixed);
  experimentLayout->addItem(spacer);
}

} // Here ends the section to set up the experiment combobox

{ // Here begins the section to set up the collector combobox
// if( clo->collectorEntryList.size() > 1 )
{
  QHBoxLayout *collectorLayout = new QHBoxLayout( TBlayout, 1, "collectorLayout" );

  QLabel *cbl = new QLabel(frame, "collectorComboBoxLabel");
  cbl->setText( __TR("Available Collectors:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(cbl);

  collectorComboBox = new QComboBox(FALSE, frame, "collectorComboBox");
  collectorComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  connect( collectorComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeCollector( const QString & ) ) );

  QToolTip::add(collectorComboBox, __TR("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(collectorComboBox);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
  collectorLayout->addItem(spacer);
}

} // Here ends the section to set up the collector combobox
  
{ // Here begins the section to set up the metric/modifier combobox

  QHBoxLayout *metricLayout = new QHBoxLayout( TBlayout, 1, "metricLayout" );

  QLabel *cbl = new QLabel(frame, "metricComboBoxLabel");
  cbl->setText( __TR("Available Metrics/Modifers:") );
  cbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(cbl);

  metricComboBox = new QComboBox(FALSE, frame, "metricComboBox");
  QToolTip::add(metricComboBox, __TR("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(metricComboBox);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
  metricLayout->addItem(spacer);

} // Here ends the section to set up the metric/modifier combobox
  
  lv = new MPListView( frame, CPS, 0  );
  lv->addColumn("Processes/PSets:");
lv->addColumn("Host:");
  lv->setResizeMode(QListView::LastColumn);
  QToolTip::add(lv->header(), "Process/Process Sets (psets) to be display in this column:");
  lv->setAllColumnsShowFocus( TRUE );
  lv->setShowSortIndicator( TRUE );
  lv->setRootIsDecorated(TRUE);
  lv->setSelectionMode( QListView::Single );
  MPListViewItem *dynamic_items = new MPListViewItem( lv, CPS);
dynamic_items->setOpen(TRUE);

  QToolTip::add(lv->viewport(), __TR("Drag and drop, psets or individual processes from the\nManageProcessesPanel.  In the StatsPanel, the statistics from\nthese grouped processes will be displayed in\ncolumns relative to this display.") );
  TBlayout->addWidget(lv);
  
  // ??  compareList.push_back(lv);
  
  compareSet->tabWidget->addTab( frame, name );
  int count = compareSet->tabWidget->count();
// printf("count=%d\n", count);
  QWidget *cp = compareSet->tabWidget->page(count-1);
  cp->show();
  compareSet->tabWidget->showPage(cp);
  compareSet->tabWidget->setTabToolTip(cp, "The information in this column will be displayed in\nthe StatsPanel's associated column.  i.e. Column #1\nwill be in the first column of the StatsPanel\nColumn #2 the second.  Column #3 the third...");

  compareSet->tcnt++;


  gatherExperimentInfo();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ColumnSet::~ColumnSet()
{
// printf("ColumnSet destructor called\n");
}

void
ColumnSet::updateInfo()
{
// printf("ColumnSet::updateInfo() entered\n");
  gatherExperimentInfo();

  if( compareSet == NULL || compareSet->compareClass == NULL )
  {
    return;
  }
}


#include "CollectorListObject.hxx"
#include "CollectorEntryClass.hxx"
#include "CollectorMetricEntryClass.hxx"

int
ColumnSet::gatherExperimentInfo()
{ // Begin gather experiment info

// printf("ColumnSet::gatherExperimentInfo() entered\n");

  CLIInterface *cli = compareSet->compareClass->p->getPanelContainer()->getMainWindow()->cli;

  std::list<int64_t> int_list;
  int_list.clear();
  experiment_list.clear();

  QString command = QString("list -v exp");
  InputLineObject *clip = NULL;
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &int_list, clip, TRUE ) )
  {
    QMessageBox::information(compareSet->compareClass, QString("Initialization warning:"), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );

  }

  QString expStr = QString::null;
  int eid = 1;
  int saved_eid = 0;
  std::list<int64_t>::iterator it;
// printf("int_list.size() =%d\n", int_list.size() );
  for(it = int_list.begin(); it != int_list.end(); it++ )
  {
    eid = (int64_t)(*it);
// printf("eid=(%d)\n", eid );

    if( saved_eid == 0 )
    {
      saved_eid = eid;
    }


    QString expIDStr = QString("%1").arg(eid);

//    command = QString("listTypes -x %1").arg(expIDStr);
    command = QString("list -v exptypes -x %1").arg(expIDStr);
    std::list<std::string> list_of_collectors;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
// printf("Unable to run %s command.\n", command.ascii() );
      
      QMessageBox::information(compareSet->compareClass, QString("Initialization warning:"), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );

      return 0;
    }
  
    int knownCollectorType = FALSE;
    QString panel_type = "other";
    QString expTypes = QString::null;
    if( list_of_collectors.size() > 0 )
    {
      for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
      {
//      std::string collector_name = *it;
        QString collector_name = (QString)*it;
// printf("  collector_name=(%s)\n", collector_name.ascii() );

        if( expTypes.isEmpty() )
        {
          expTypes = QString("Exp:%1 %2").arg(eid).arg(collector_name);
        } else
        {
          expTypes += ", " + collector_name;
        }
      }
    }
    if( !expTypes.isEmpty() )
    {
      const char *str = expTypes.ascii();
      experiment_list.push_back( std::make_pair(eid, str ) );
// printf("What's going on ... push (%s) back to the experiment_list\n", str );
      if( expStr.isEmpty() )
      {
        expStr = expTypes;
// printf("initialize expStr with default value (%s)\n", expStr.ascii() );
      }
    }
  }

  // Update the experiment fields
  // First save the existing state (if any).
  bool expStrFoundFLAG = FALSE;
  if( experimentComboBox )
  {
    if( !experimentComboBox->currentText().isEmpty() )
    {
      expStr = experimentComboBox->currentText();
      saved_eid = getExpidFromExperimentComboBoxStr(expStr);
    }
// printf("expStr = (%s)\n", expStr.ascii() );
  
    int cb_count = 0;
    for(cb_count = experimentComboBox->count(); cb_count > 0; cb_count--)
    {
      experimentComboBox->removeItem(cb_count-1);
    }
  }
  for( std::vector<pair_def>::const_iterator it = experiment_list.begin();         it != experiment_list.end(); it++ )
  {
  
    QString str1 = QString("%1").arg(it->first);
    QString str2 = QString("%1").arg(it->second.c_str());
// printf("str1=(%s) str2=(%s)\n", str1.ascii(), str2.ascii() );
    if( experimentComboBox )
    {
      QString label(str2);
      experimentComboBox->insertItem( label );
// printf("str1.ascii()=(%s) str2.ascii()=(%s)\n", str1.ascii(), str2.ascii() );
      if( label == expStr )
      {
        expStrFoundFLAG = TRUE;
        saved_eid = getExpidFromExperimentComboBoxStr(str2);
// printf("We found the existing label, prepare to reset it.\n");
      }
    }
  }
  if( !expStr.isEmpty() )
  {
// printf("Try to set the ComboBox back to what it was. (%s)\n", expStr.ascii() );
    experimentComboBox->setCurrentText( expStr );
  }

// printf("is saved_eid=(%d) better?\n", saved_eid );
  if( saved_eid > 0 )
  {
    eid = saved_eid;
  }
// printf("Sending eid=(%d) down to gatherCollectorInfo()\n", eid) ;
  ce = gatherCollectorInfo(eid);

  return( eid );
} // End gather experiment info

CollectorEntry *
ColumnSet::gatherCollectorInfo(int id)
{
// printf("ColumnSet::gatherCollectorInfo(%d) entered\n", id);
  if( clo )
  {
    delete(clo);
  }
  
  clo = new CollectorListObject(id);

  // Update the collectors (of the above experiment) fields
  // First save the existing state (if any).
  QString collectorStr = QString::null;
  bool collectorStrFoundFLAG = FALSE;
  if( collectorComboBox )
  {
    if( !collectorComboBox->currentText().isEmpty() )
    {
      collectorStr = collectorComboBox->currentText();
    }
// printf("A: collectorStr = (%s)\n", collectorStr.ascii() );
  
    int cb_count = 0;
    for(cb_count = collectorComboBox->count(); cb_count > 0; cb_count--)
    {
      collectorComboBox->removeItem(cb_count-1);
    }
  }
// printf("Now look up up the collectors of the above (focused) experiment.   Now try to build the collector list from it...\n");

  CollectorEntry *saved_ce = NULL;
  ce = NULL;
  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
    if( saved_ce == NULL )
    {
// printf("saved_ce is null, initialize it to (%s)\n", ce->name.ascii() );
      saved_ce = ce;
    }
    if( collectorStr.isEmpty() )
    {
// printf("collectorStr was empty, initialize it to (%s)\n", ce->name.ascii() );
       collectorStr = ce->name;
       saved_ce = ce;
     }
    if( collectorComboBox )
    {
      QString label(ce->name);
      collectorComboBox->insertItem( label );
// printf("Put this to menu: ce->name=%s ce->short_name\n", ce->name.ascii(), ce->short_name.ascii() );
      if( label == collectorStr )
      {
        collectorStrFoundFLAG = TRUE;
        saved_ce = ce;
// printf("found a collectorStr to try to restore, set saved_ce = (%s)\n", ce->name.ascii() );
      }
    }
  }

  if( collectorStrFoundFLAG == TRUE )
  {
    if( !collectorStr.isEmpty() )
    {
// printf("Attempt to restore the collectorComboBox to (%s)\n", collectorStr.ascii() );
      collectorComboBox->setCurrentText( collectorStr );
    }
  } else
  {
    collectorComboBox->setCurrentText( saved_ce->name );
  }

  ce = saved_ce;
// printf("  Right before call to gatherMetric ce=(%s)\n", ce->name.ascii() );
  gatherMetricInfo(ce);

  return(ce);
}

void
ColumnSet::gatherMetricInfo(CollectorEntry *ce)
{

  if( ce == NULL )
  {
fprintf(stderr, "WARNING: ColumnSet::gatherMetricInfo() returned\n");
    return;
  }

// printf("ColumnSet::gatherMetricInfo(%s) entered\n", ce->name.ascii() );
  // Update the metrics (and any other modifiers) of the above collector
  // First save the existing state (if any).
  QString metricStr = QString::null;
  bool metricStrFoundFLAG = FALSE;
  if( metricComboBox )
  {
    if( !metricComboBox->currentText().isEmpty() )
    {
      metricStr = metricComboBox->currentText();
    }
// printf("metricStr = (%s)\n", metricStr.ascii() );
  
    int cb_count = 0;
    for(cb_count = metricComboBox->count(); cb_count > 0; cb_count--)
    {
      metricComboBox->removeItem(cb_count-1);
    }
  }
// printf("Now look up up the metrics of the (%s) collector\n", ce->name.ascii() );

  CollectorMetricEntryList::Iterator plit;
  for( plit = ce->metricList.begin();
       plit != ce->metricList.end(); ++plit )
  {
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
    if( metricComboBox )
    {
      metricComboBox->insertItem( cpe->name );
// printf("Add (%s) to metricComboBox\n", cpe->name.ascii() );
      if( metricStr == cpe->name )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = cpe->name;
// printf("Found! (%s)\n", cpe->name.ascii() );
      }
      if( metricStr.isEmpty() )
      {
// printf("Set a default for metricStr!!!\n");
        metricStr = cpe->name;
      }
    }
// printf("Put this to a menu: cpe->name=(%s) cpe->type=(%s) cpe->metric_val=(%s)\n", cpe->name.ascii(), cpe->type.ascii(), cpe->metric_val.ascii() );
  }
  if( metricComboBox )
  {
    metricComboBox->insertItem("min");
if( metricStr == "min" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "min";
}
    metricComboBox->insertItem("max");
if( metricStr == "max" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "max";
}
    metricComboBox->insertItem("average");
if( metricStr == "average" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "average";
}
    metricComboBox->insertItem("count");
if( metricStr == "count" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "count";
}
    metricComboBox->insertItem("percent");
if( metricStr == "percent" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "percent";
}
    metricComboBox->insertItem("stddev");
if( metricStr == "stddev" )
{
  metricStrFoundFLAG = TRUE;
  metricStr = "stddev";
}
  }
// printf("down here!  metricStr=(%s)\n", metricStr.ascii() );
  if( metricStrFoundFLAG == FALSE )
  {
    plit = ce->metricList.begin();
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
    metricStr = cpe->name;
  } 
  if( !metricStr.isEmpty() )
  {
// printf("Attempt to reset the metricComboBox to the saved text\n");
    metricComboBox->setCurrentText( metricStr );
  }
}

int
ColumnSet::getExpidFromExperimentComboBoxStr( const QString path )
{
// printf("ColumnSet::getExpidFromExperimentComboBoxStr(%s)\n", path.ascii()  );
  int si = 0;
  int ei = 0;
  QString expIDStr = QString::null;
  int expID = 0;

  si = path.find(":");

  if( si >= 0 )
  {
    ei = path.find(" ");
  }

  if( ei >= 0 )
  {
    expIDStr = path.mid(si+1, ei-si);
    expID = expIDStr.toInt();
  }

// printf("The expID = (%d)\n", expID);

  return( expID );
}

#include "CompareProcessesDialog.hxx"
void 
ColumnSet::changeExperiment( const QString &path )
{
// printf("ColumnSet::changeExperiment()\n");

  int expID = getExpidFromExperimentComboBoxStr(path);

  ce = gatherCollectorInfo(expID);

  
  if( compareSet->compareClass->dialog )
  {
    compareSet->compareClass->dialog->expID = expID;
    compareSet->updatePSetList();
  }
}

void 
ColumnSet::changeCollector( const QString &path )
{
// printf("ColumnSet::changeCollector(%s)\n", path.ascii()  );

  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
    if( ce->name == path )
    {
// printf("We found the selected collector... now get the metrics for (%s)\n", ce->name.ascii() );
      break;
    }
  }

// printf("ce (%s) right before call to getherMetricInfo\n", ce->name.ascii() );
  gatherMetricInfo(ce);
}

