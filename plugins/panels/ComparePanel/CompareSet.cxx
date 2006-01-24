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

CompareSet::CompareSet(QToolBox *csetTB, CompareClass *cc )
{
// printf("CompareSet::CompareSet() constructor called.\n");
  tcnt = 0;
  compareClass = cc;

  name = QString("cset%1").arg(compareClass->ccnt);

  tabWidget = new QTabWidget(compareClass, "tabWidget");

  compareClass->addNewColumn(this);

  csetTB->addItem(tabWidget, name );

  compareClass->ccnt++;
}

void
CompareSet::updateInfo()
{
// printf("CompareSet::updateInfo() put the data out!\n");
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
