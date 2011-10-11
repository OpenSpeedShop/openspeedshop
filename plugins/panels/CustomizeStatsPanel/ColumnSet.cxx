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

//#define DEBUG_COMPARE 1

// This turns on the ability to see the database names in the experiment selection menu items
#define DBNAMES 1

#include "ColumnSet.hxx"
#include "CompareSet.hxx"
#include "CustomizeClass.hxx"
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

// This routine is strongly based (copy of) 
// on the Tokenizer routine found at this URL:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html

void createTokens(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);

#ifdef DEBUG_CLI
        printf("createTokens, in while, str.c_str()=%s, lastPos = %d, pos = %d\n", str.c_str(), lastPos, pos);
#endif

    }
}


// Database name request code
//
std::list<std::string> list_of_dbnames;

#ifdef DBNAMES
void ColumnSet::getDatabaseName(int exp_id)
{

// Now get the executables

  QString command = QString::null;

#ifdef DEBUG_COMPARE
  printf("getDatabaseName exp_id=%d\n", exp_id);
#endif

  command = QString("list -v database -x %1").arg(exp_id);

#ifdef DEBUG_COMPARE
  printf("getDatabaseName-attempt to run (%s)\n", command.ascii() );
#endif

  CLIInterface *cli = this->compareSet->compareClass->p->getPanelContainer()->getMainWindow()->cli;
  list_of_dbnames.clear();
  InputLineObject *clip = NULL;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_dbnames, clip, TRUE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }
#ifdef DEBUG_COMPARE
  printf("getDatabaseName, ran %s, list_of_dbnames.size()=%d\n", command.ascii(), list_of_dbnames.size() );
#endif

  if( list_of_dbnames.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      std::string databaseName = *it;
#ifdef DEBUG_COMPARE
      printf("getDatabaseName, databaseName=(%s)\n", databaseName.c_str() );
#endif
    }
  }
}
#endif


ColumnSet::ColumnSet(QWidget *w, CompareSet *cc) : QWidget(0, "columnSetObject")
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::ColumnSet() constructor called.\n");
#endif


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
  cbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(cbl);

  experimentComboBox = new QComboBox(FALSE, frame, "experimentComboBox");
  connect( experimentComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeExperiment( const QString & ) ) );

  QToolTip::add(experimentComboBox, __TR("Select which experiment that you want\nto use in the comparison for this column.") );
  experimentLayout->addWidget(experimentComboBox);

//  QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//  experimentLayout->addItem(spacer);
}

} // Here ends the section to set up the experiment combobox

{ // Here begins the section to set up the collector combobox
// if( clo->collectorEntryList.size() > 1 )
{
  QHBoxLayout *collectorLayout = new QHBoxLayout( TBlayout, 1, "collectorLayout" );

  QLabel *cbl = new QLabel(frame, "collectorComboBoxLabel");
  cbl->setText( __TR("Available Collectors:") );
  cbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(cbl);

  collectorComboBox = new QComboBox(FALSE, frame, "collectorComboBox");
//  collectorComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  connect( collectorComboBox, SIGNAL( activated(const QString &) ), this, SLOT( changeCollector( const QString & ) ) );

  QToolTip::add(collectorComboBox, __TR("Select which collector that you want\nto use in the comparison for this column.") );
  collectorLayout->addWidget(collectorComboBox);

//  QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//  collectorLayout->addItem(spacer);
}

} // Here ends the section to set up the collector combobox
  
{ // Here begins the section to set up the metric/modifier combobox

  QHBoxLayout *metricLayout = new QHBoxLayout( TBlayout, 1, "metricLayout" );

  QLabel *cbl = new QLabel(frame, "metricComboBoxLabel");
  cbl->setText( __TR("Available Metrics/Modifers:") );
  cbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QToolTip::add(cbl, __TR("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(cbl);

  metricComboBox = new QComboBox(FALSE, frame, "metricComboBox");
  QToolTip::add(metricComboBox, __TR("Select which metric/modifier that you want\nto use in the comparison for this column.") );
  metricLayout->addWidget(metricComboBox);

//  QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//  metricLayout->addItem(spacer);
} // Here ends the section to set up the metric/modifier combobox
  
  lv = new MPListView( frame, CPS, 0  );
  lv->addColumn("Processes/PSets:");
  lv->addColumn("PID:");
  lv->addColumn("Rank:");
  lv->addColumn("Thread:");
  lv->setResizeMode(QListView::LastColumn);
  QToolTip::add(lv->header(), "Process/Process Sets to be display in this column:");
  lv->setAllColumnsShowFocus( TRUE );
  lv->setShowSortIndicator( TRUE );
  lv->setRootIsDecorated(TRUE);
  lv->setSelectionMode( QListView::Single );
  initializeListView();

  QToolTip::add(lv->viewport(), __TR("Drag and drop, process sets or individual processes from the\nManageProcessesPanel.  In the StatsPanel, the statistics from\nthese grouped processes will be displayed in\ncolumns relative to this display.") );
  TBlayout->addWidget(lv);
  
  // ??  compareList.push_back(lv);
  
  compareSet->tabWidget->addTab( frame, name );
  int count = compareSet->tabWidget->count();
#ifdef DEBUG_COMPARE
  printf("count=%d\n", count);
#endif
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
#ifdef DEBUG_COMPARE
  printf("ColumnSet::ColumnSet destructor called\n");
#endif
}

void
ColumnSet::updateInfo()
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::updateInfo() entered\n");
#endif

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

#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherExperimentInfo() entered\n");
#endif

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

#ifdef DEBUG_COMPARE
  printf("int_list.size() =%d\n", int_list.size() );
#endif

  for(it = int_list.begin(); it != int_list.end(); it++ ) {

    eid = (int64_t)(*it);

#ifdef DEBUG_COMPARE
     printf("eid=(%d)\n", eid );
#endif

    if( saved_eid == 0 ) {
      saved_eid = eid;
    }
    QString expIDStr = QString("%1").arg(eid);

    command = QString("list -v exptypes -x %1").arg(expIDStr);
    std::list<std::string> list_of_collectors;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
#ifdef DEBUG_COMPARE
      printf("Unable to run %s command.\n", command.ascii() );
#endif
      
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
#ifdef DEBUG_COMPARE
        printf("  collector_name=(%s)\n", collector_name.ascii() );
#endif

        if( expTypes.isEmpty() ) {
          expTypes = QString("Exp:%1 %2").arg(eid).arg(collector_name);
        } else {
          expTypes += ", " + collector_name;
        }
      }
    }


    // get the datafile file name
#ifdef DBNAMES
#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherExperimentInfo() , list_of_dbnames.size()=%d\n", list_of_dbnames.size());
#endif
  getDatabaseName(eid);
  if( list_of_dbnames.size() > 0 )
  {
    int dbnames_count = 0;
    for( std::list<std::string>::const_iterator it = list_of_dbnames.begin();
         it != list_of_dbnames.end(); it++ )
    {
      dbnames_count = dbnames_count + 1;
      std::string dbnames = *it;
      QString dbnamesStr = QString("%1").arg(dbnames.c_str());
#ifdef DEBUG_COMPARE
      printf("ColumnSet::gatherExperimentInfo() , dbnames=(%s)\n", dbnames.c_str() );
#endif
      expTypes += " Database Name: " + dbnamesStr;
    }

  }
#endif


    if( !expTypes.isEmpty() )
    {
      const char *str = expTypes.ascii();
      experiment_list.push_back( std::make_pair(eid, str ) );
#ifdef DEBUG_COMPARE
       printf("What's going on ... push (%s) back to the experiment_list\n", str );
#endif
      if( expStr.isEmpty() )
      {
        expStr = expTypes;
#ifdef DEBUG_COMPARE
        printf("initialize expStr with default value (%s)\n", expStr.ascii() );
#endif
      }
    }
  } // end of the for loop over the compare list of experiments

  // Update the experiment fields
  // First save the existing state (if any).
  bool expStrFoundFLAG = FALSE;
  if( experimentComboBox )
  {
    if( !experimentComboBox->currentText().isEmpty() )
    {
      expStr = experimentComboBox->currentText();
      saved_eid = getExpidFromExperimentComboBoxStr(expStr);
#ifdef DEBUG_COMPARE
      printf("There was a combo box.  It was (%s)\n", expStr.ascii() );
#endif
    } else {

      // THIS IS A BUG.   This code block should be look "back"
      // for the previous columns experiment string and attempting
      // to focus this information with that information.
      //
#ifdef DEBUG_COMPARE
      printf("There was a combo box.  IT WAS EMPTY!!\n");
#endif

    }
    
#ifdef DEBUG_COMPARE
    printf("expStr = (%s) saved_eid=%d\n", expStr.ascii(), saved_eid );
#endif
  
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

#ifdef DEBUG_COMPARE
    printf("str1=(%s) str2=(%s)\n", str1.ascii(), str2.ascii() );
#endif

    if( experimentComboBox ) {

#ifdef DEBUG_COMPARE
      printf("WE HAVE AN experimentComboBox!\n");
#endif

      QString label(str2);
      experimentComboBox->insertItem( label );

#ifdef DEBUG_COMPARE
      printf("label.ascii()=(%s) expStr.ascii()=(%s)\n", label.ascii(), expStr.ascii() );
#endif

      if( compareSet->compareClass->focusedExpID > 0 )
      {
        if( getExpidFromExperimentComboBoxStr(str2) == compareSet->compareClass->focusedExpID )
        {
#ifdef DEBUG_COMPARE
          printf("FORCE THE FOCUS\n");
#endif
          expStrFoundFLAG = TRUE;
          saved_eid = getExpidFromExperimentComboBoxStr(str2);
          // Unset this...
// Not until you fix the "BUG" (See above comment) above.
//          compareSet->compareClass->focusedExpID = -1;

          expStr = str2;
        } else if( label == expStr ) {

          expStrFoundFLAG = TRUE;
          saved_eid = getExpidFromExperimentComboBoxStr(str2);
#ifdef DEBUG_COMPARE
          printf("A: We found the existing label, prepare to reset it.\n");
#endif
          expStr = str2;

        }

      } else if( label == expStr ) {

        expStrFoundFLAG = TRUE;
        saved_eid = getExpidFromExperimentComboBoxStr(str2);

#ifdef DEBUG_COMPARE
        printf("B: We found the existing label, prepare to reset it.\n");
#endif

        expStr = str2;
      }
    }
  }
  if( !expStr.isEmpty() ) {

#ifdef DEBUG_COMPARE
    printf("Try to set the ComboBox back to what it was. (%s)\n", expStr.ascii() );
#endif

    experimentComboBox->setCurrentText( expStr );

  }

#ifdef DEBUG_COMPARE
  printf("is saved_eid=(%d) better?\n", saved_eid );
#endif

  if( saved_eid > 0 ) {
    eid = saved_eid;
  }

  if( compareSet->compareClass->focusedExpID > 0 )
  {

#ifdef DEBUG_COMPARE
    printf("If saved_eid <= 0 force the new one!\n");
#endif

    saved_eid = compareSet->compareClass->focusedExpID;

  }

#ifdef DEBUG_COMPARE
  printf("Sending eid=(%d) down to gatherCollectorInfo()\n", eid) ;
#endif

  ce = gatherCollectorInfo(eid);

  return( eid );
} // End gather experiment info

CollectorEntry *
ColumnSet::gatherCollectorInfo(int id)
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherCollectorInfo(%d) entered\n", id);
#endif

  if( clo ) {
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
#ifdef DEBUG_COMPARE
   printf("A: collectorStr = (%s)\n", collectorStr.ascii() );
#endif
  
    int cb_count = 0;
    for(cb_count = collectorComboBox->count(); cb_count > 0; cb_count--)
    {
      collectorComboBox->removeItem(cb_count-1);
    }
  }
#ifdef DEBUG_COMPARE
  printf("Now look up up the collectors of the above (focused) experiment.   Now try to build the collector list from it...\n");
#endif

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
#ifdef DEBUG_COMPARE
      printf("saved_ce is null, initialize it to (%s)\n", ce->name.ascii() );
#endif
      saved_ce = ce;
    }
    if( collectorStr.isEmpty() )
    {
#ifdef DEBUG_COMPARE
       printf("collectorStr was empty, initialize it to (%s)\n", ce->name.ascii() );
#endif
       collectorStr = ce->name;
       saved_ce = ce;
     }
    if( collectorComboBox )
    {
      QString label(ce->name);
      collectorComboBox->insertItem( label );
#ifdef DEBUG_COMPARE
      printf("Put this to menu: ce->name=%s ce->short_name\n", ce->name.ascii(), ce->short_name.ascii() );
#endif
      if( label == collectorStr )
      {
        collectorStrFoundFLAG = TRUE;
        saved_ce = ce;
#ifdef DEBUG_COMPARE
        printf("found a collectorStr to try to restore, set saved_ce = (%s)\n", ce->name.ascii() );
#endif
      }
    }
  }

  if( collectorStrFoundFLAG == TRUE )
  {
    if( !collectorStr.isEmpty() )
    {
#ifdef DEBUG_COMPARE
      printf("Attempt to restore the collectorComboBox to (%s)\n", collectorStr.ascii() );
#endif
      collectorComboBox->setCurrentText( collectorStr );
    }
  } else
  {
    collectorComboBox->setCurrentText( saved_ce->name );
  }

  ce = saved_ce;
#ifdef DEBUG_COMPARE
  printf("  Right before call to gatherMetric ce=(%s)\n", ce->name.ascii() );
#endif
  gatherMetricInfo(ce);

  return(ce);
}

void
ColumnSet::gatherMetricInfo(CollectorEntry *ce)
{

  if( ce == NULL ) {
    fprintf(stderr, "WARNING: ColumnSet::gatherMetricInfo() returned\n");
    return;
  }

#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherMetricInfo(%s) entered\n", ce->name.ascii() );
#endif

  // Update the metrics (and any other modifiers) of the above collector
  // First save the existing state (if any).
  QString metricStr = QString::null;
  bool metricStrFoundFLAG = FALSE;

  if( metricComboBox ) {
    if( !metricComboBox->currentText().isEmpty() ) {
      metricStr = metricComboBox->currentText();
    }

#ifdef DEBUG_COMPARE
    printf("ColumnSet::gatherMetricInfo, metricStr = (%s)\n", metricStr.ascii() );
#endif
  
    int cb_count = 0;
    for(cb_count = metricComboBox->count(); cb_count > 0; cb_count--) {
      metricComboBox->removeItem(cb_count-1);
    }
  }

#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherMetricInfo, Now look up up the metrics of the ce->name (%s) collector\n", ce->name.ascii() );
#endif

  CollectorMetricEntryList::Iterator plit;
  for( plit = ce->metricList.begin();
       plit != ce->metricList.end(); ++plit )
  {
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
    if( metricComboBox ) {
      metricComboBox->insertItem( cpe->name );

#ifdef DEBUG_COMPARE
      printf("ColumnSet::gatherMetricInfo, Add (%s) to metricComboBox\n", cpe->name.ascii() );
#endif

      if( metricStr == cpe->name ) {
        metricStrFoundFLAG = TRUE;
        metricStr = cpe->name;
#ifdef DEBUG_COMPARE
        printf("ColumnSet::gatherMetricInfo, Found! (%s)\n", cpe->name.ascii() );
#endif
      }
      if( metricStr.isEmpty() ) {
#ifdef DEBUG_COMPARE
        printf("ColumnSet::gatherMetricInfo, Set a default for metricStr!!!\n");
#endif
        metricStr = cpe->name;
      }
    }

#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherMetricInfo, Put this to a menu: cpe->name=(%s) cpe->type=(%s) cpe->metric_val=(%s)\n", cpe->name.ascii(), cpe->type.ascii(), cpe->metric_val.ascii() );
#endif

  } // end for

  if( metricComboBox ) {

    if( ce->name == "pcsamp" || ce->name == "hwc" || ce->name == "hwcsamp" )
    { // Nothing extra here... only the collector defined metric.
      metricComboBox->insertItem("percent");
      if( metricStr == "percent" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "percent";
      }
    }

    if( ce->name == "usertime" || ce->name == "hwctime" ||
        ce->name == "mpi" || ce->name == "mpit" ||\
        ce->name == "io" || ce->name == "iot" )
    { // count, percent
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
    }

    if(  ce->name == "mpi" || ce->name == "mpit" ||
         ce->name == "io" || ce->name == "iot" )
    { // min, max, average, stddev
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
      metricComboBox->insertItem("stddev");
      if( metricStr == "stddev" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "stddev";
      }
    }

    if( ce->name == "mpit" )
    { // start_time, stop_time, source, dest, size, tag, comm, datatype, retval
      metricComboBox->insertItem("start_time");
      if( metricStr == "start_time" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "start_time";
      }
      metricComboBox->insertItem("stop_time");
      if( metricStr == "stop_time" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "stop_time";
      }
      metricComboBox->insertItem("source");
      if( metricStr == "source" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "source";
      }
      metricComboBox->insertItem("dest");
      if( metricStr == "dest" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "dest";
      }
      metricComboBox->insertItem("size");
      if( metricStr == "size" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "size";
      }
      metricComboBox->insertItem("tag");
      if( metricStr == "tag" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "tag";
      }
      metricComboBox->insertItem("comm");
      if( metricStr == "comm" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "comm";
      }
      metricComboBox->insertItem("datatype");
      if( metricStr == "datatype" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "datatype";
      }
      metricComboBox->insertItem("retval");
      if( metricStr == "retval" )
      {
        metricStrFoundFLAG = TRUE;
        metricStr = "retval";
      }
    }

    // Add the actual hwc (PAPI) events here
    if( ce->name == "hwcsamp" ) { 


#ifdef DEBUG_COMPARE
       printf("ColumnSet::gatherMetricInfo, dealing with hwcsamp metrics\n" );
#endif

       // Now get the the metrics for hwcsamp to pull off the event names to add to the metrics list
       QString expStr = QString::null;
       int expID = -1;
       QString command = QString::null;
       std::list<std::string> list_of_hwcsamp_modifiers;
       if( experimentComboBox )
       {
         if( !experimentComboBox->currentText().isEmpty() )
         {
           expStr = experimentComboBox->currentText();
           expID = getExpidFromExperimentComboBoxStr(expStr);
#ifdef DEBUG_COMPARE
           printf("There was a combo box.  It was (%s)\n", expStr.ascii() );
#endif
         } 
       } 
    
       if( expID > 0 ) {
          command = QString("list -v justparamvalues -x %1").arg(expID);
       } else {
          printf("ERROR no experiment could be found when processing hwcsamp metrics\n");
       }

#ifdef DEBUG_COMPARE
       printf("ColumnSet::gatherMetricInfo, attempt to run (%s)\n", command.ascii() );
#endif

       //CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
       CLIInterface *cli = this->compareSet->compareClass->p->getPanelContainer()->getMainWindow()->cli;
       list_of_hwcsamp_modifiers.clear();
       InputLineObject *clip = NULL;
       std::string cstring;
       std::string lastToken;

       if( !cli->getStringValueFromCLI( (char *)command.ascii(), &cstring, clip, TRUE ) )
       {
         printf("Unable to run %s command.\n", command.ascii() );
       }

#ifdef DEBUG_COMPARE
         printf("ColumnSet::gatherMetricInfo, ran %s, result=%s\n", command.ascii(), cstring.c_str() );
#endif

       std::vector<std::string> tokens;
       createTokens(cstring, tokens, ",");
       std::vector<std::string>::iterator k;
       
       // Find the hwcsamp PAPI event names
       for (k=tokens.begin();k != tokens.end(); k++) {
          lastToken = *k;
#ifdef DEBUG_COMPARE
          printf("ColumnSet::gatherMetricInfo, list_of_hwcsamp_modifiers.push_back, lastToken=%s\n", lastToken.c_str() );
#endif
          list_of_hwcsamp_modifiers.push_back(lastToken.c_str());
       }

       // Now add to the metricComboBox
       for( std::list<std::string>::const_iterator it = list_of_hwcsamp_modifiers.begin();
            it != list_of_hwcsamp_modifiers.end();  )
       {
         std::string modifier = (std::string)*it;
         metricComboBox->insertItem(modifier.c_str());
#ifdef DEBUG_COMPARE
          printf("ColumnSet::gatherMetricInfo, LOOP THROUGH, list_of_hwcsamp_modifiers, modifier=%s\n", modifier.c_str() );
#endif
          it++;
       }

    } // end hwcsamp


#ifdef DEBUG_COMPARE
  printf("ColumnSet::gatherMetricInfo, down here!  metricStr=(%s)\n", metricStr.ascii() );
#endif

  if( metricStrFoundFLAG == FALSE )
  {
    plit = ce->metricList.begin();
    CollectorMetricEntry *cpe = (CollectorMetricEntry *)*plit;
    metricStr = cpe->name;
  } 
  if( !metricStr.isEmpty() )
  {
#ifdef DEBUG_COMPARE
    printf("ColumnSet::gatherMetricInfo, Attempt to reset the metricComboBox to the saved text\n");
#endif
    metricComboBox->setCurrentText( metricStr );
  }
 }
}

int
ColumnSet::getExpidFromExperimentComboBoxStr( const QString path )
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::getExpidFromExperimentComboBoxStr(%s)\n", path.ascii()  );
#endif
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

#ifdef DEBUG_COMPARE
  printf("The expID = (%d)\n", expID);
#endif

  return( expID );
}

#include "CompareProcessesDialog.hxx"
void 
ColumnSet::changeExperiment( const QString &path )
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::changeExperiment()\n");
#endif

  int expID = getExpidFromExperimentComboBoxStr(path);

  ce = gatherCollectorInfo(expID);

  
  if( compareSet->compareClass->dialog )
  {
    compareSet->compareClass->dialog->expID = expID;
    compareSet->updatePSetList();
// Also current process set to defaults (all)...
    initializeListView();
  }
}

void 
ColumnSet::changeCollector( const QString &path )
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::changeCollector(%s)\n", path.ascii()  );
#endif

  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
    if( ce->name == path )
    {
#ifdef DEBUG_COMPARE
      printf("We found the selected collector... now get the metrics for (%s)\n", ce->name.ascii() );
#endif
      break;
    }
  }

#ifdef DEBUG_COMPARE
  printf("ce (%s) right before call to getherMetricInfo\n", ce->name.ascii() );
#endif
  gatherMetricInfo(ce);
}

void
ColumnSet::initializeListView()
{
#ifdef DEBUG_COMPARE
  printf("ColumnSet::initializeListView\n" );
#endif
  lv->clear();
  MPListViewItem *dynamic_items = new MPListViewItem( lv, CPS);

  QString pset_name = "All";
  MPListViewItem *item = new MPListViewItem( dynamic_items, pset_name );
  DescriptionClassObject *dco = new DescriptionClassObject(TRUE, pset_name);
  dco->all = TRUE;
  item->descriptionClassObject = dco;

  dynamic_items->setOpen(TRUE);
}
