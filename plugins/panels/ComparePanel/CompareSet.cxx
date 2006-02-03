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

#include "debug.hxx"

#include "CompareSet.hxx"
#include "ColumnSet.hxx"
#include "CompareClass.hxx"

#include "CompareProcessesDialog.hxx"

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

CompareSet::CompareSet(QToolBox *csetTB, CompareClass *cc ) : QObject()
{
// printf("CompareSet::CompareSet() constructor called.\n");
  tcnt = 0;
  compareClass = cc;

  name = QString("cset%1").arg(compareClass->ccnt);

  tabWidget = new QTabWidget(compareClass, "tabWidget");

  compareClass->addNewColumn(this);

  csetTB->addItem(tabWidget, name );

  compareClass->ccnt++;

  connect(tabWidget, SIGNAL( currentChanged(QWidget *)), this, SLOT( currentChanged(QWidget *) ) );
}

void
CompareSet::updateInfo()
{
// printf("CompareSet::updateInfo() entered\n");
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
// printf("attempt to delete (%s)'s info\n", cs->name.ascii() );

    cs->updateInfo();
    ++it;
  }
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareSet::~CompareSet()
{
   // Destroy the list of column information.
// printf("Destroy the list of column information.\n");
  ColumnSetList::Iterator it;
  for( it = columnSetList.begin(); it != columnSetList.end(); )
  {
    ColumnSet *cs = (ColumnSet *)*it;
// printf("attempt to delete (%s)'s info\n", cs->name.ascii() );

    delete(cs);
    ++it;
  }

// printf("Finished cleaning up the CompareSet\n");
  columnSetList.clear();
}

void
CompareSet::currentChanged( QWidget *tab )
{
printf("currentChanged() set the focus on a new tab...\n");


  setNewFocus(tab);

}

void
CompareSet::setNewFocus(QWidget *tab)
{
  if( !tab )
  {
// find the current tab... 
    tab = tabWidget->currentPage();
  }

  CompareSet *compareSet = this;
  if( compareSet )
  {
printf("CompareSet: (%s)'s info\n", compareSet->name.ascii() );
    ColumnSetList::Iterator it;
    for( it = compareSet->columnSetList.begin(); it != compareSet->columnSetList.end(); ++it )
    {
      ColumnSet *columnSet = (ColumnSet *)*it;
printf("Is it? columnSet->name=(%s) tabWidget->tabLabel()=(%s)\n", columnSet->name.ascii(), tabWidget->tabLabel(tab).ascii()  );
      if( columnSet->name == tabWidget->tabLabel(tab) )
      {
        int id = columnSet->getExpidFromExperimentComboBoxStr(columnSet->experimentComboBox->currentText());
printf("\t: CompareSet (%s)'s info\n", compareSet->name.ascii() );
printf("\t: ColumnSet (%s)'s info\n", columnSet->name.ascii() );
printf("\t\t: expID=(%d)\n", id );
printf("\t\t: experimentComboBox=(%s)\n", columnSet->experimentComboBox->currentText().ascii() );
printf("\t\t: collectorComboBox=(%s)\n", columnSet->collectorComboBox->currentText().ascii() );
printf("\t\t: metricComboBox=(%s)\n", columnSet->metricComboBox->currentText().ascii() );
if( compareSet->compareClass->dialog )
{
  compareSet->compareClass->dialog->updateFocus(compareSet->compareClass, compareSet, columnSet);
}
         break;
       }
    }
  }

}
