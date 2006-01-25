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

#define __TR QString



#include "debug.hxx"

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"
#include "PanelContainer.hxx"
#include "FocusObject.hxx"


ColumnSet::ColumnSet(CompareSet *cc)
{
// printf("ColumnSet::ColumnSet() constructor called.\n");

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

  QToolTip::add(experimentComboBox, __TR("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(experimentComboBox);

  QSpacerItem *spacer = new QSpacerItem(1000,1, QSizePolicy::Maximum, QSizePolicy::Fixed);
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
  QToolTip::add(lv->header(), "Process/Process Sets (psets) to be display in this column:");
  lv->setAllColumnsShowFocus( TRUE );
  lv->setShowSortIndicator( TRUE );
  lv->setRootIsDecorated(TRUE);
  lv->setSelectionMode( QListView::Single );
  MPListViewItem *dynamic_items = new MPListViewItem( lv, CPS);

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

  if( compareSet == NULL || compareSet->compareClass == NULL )
  {
    return;
  }

// printf("CompareSet::updateInfo() put the data out!\n");
  // Update the experiment fields
  for( std::vector<pair_def>::const_iterator it = compareSet->compareClass->experiment_list.begin();         it != compareSet->compareClass->experiment_list.end(); it++ )
  {
// printf("it->first=%d it->second.c_str()=(%s)\n", it->first, it->second.c_str() );
  
    QString str1 = QString("%1").arg(it->first);
    QString str2 = QString("%1").arg(it->second.c_str());
    if( experimentComboBox )
    {
      experimentComboBox->insertItem( "Exp:"+str1+" "+str2 );
// printf("str1.ascii()=(%s) str2.ascii()=(%s)\n", str1.ascii(), str2.ascii() );
    }
  }


  // Update the collectors (of the above experiment) fields
  CollectorEntryList::Iterator it;
  for( it = compareSet->compareClass->clo->collectorEntryList.begin();
       it != compareSet->compareClass->clo->collectorEntryList.end();
       ++it )
  {
    compareSet->compareClass->ce = (CollectorEntry *)*it;
    if( collectorComboBox )
    {
        collectorComboBox->insertItem( compareSet->compareClass->ce->name );
// printf("Put this to menu: ce->name=%s ce->short_name\n", compareSet->compareClass->ce->name.ascii(), compareSet->compareClass->ce->short_name.ascii() );
    }
  }

  // Update the metrics (and any other modifiers) of the above collector
  CollectorMetricEntryList::Iterator plit;
  for( plit = compareSet->compareClass->ce->metricList.begin();
       plit != compareSet->compareClass->ce->metricList.end(); ++plit )
  {
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
    if( metricComboBox )
    {
      metricComboBox->insertItem( cpe->name );
    }
// printf("Put this to a menu: cpe->name=(%s) cpe->type=(%s) cpe->metric_val=(%s)\n", cpe->name.ascii(), cpe->type.ascii(), cpe->metric_val.ascii() );
  }
  if( metricComboBox )
  {
    metricComboBox->insertItem("min");
    metricComboBox->insertItem("max");
    metricComboBox->insertItem("average");
    metricComboBox->insertItem("count");
    metricComboBox->insertItem("percent");
    metricComboBox->insertItem("stddev");
  }
}
