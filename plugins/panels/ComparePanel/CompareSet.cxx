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
#include <qstring.h>

CompareSet::CompareSet(QToolBox *csetTB, CompareClass *compareClass )
{
printf("CompareSet::CompareSet() constructor called.\n");

  name = QString("cset%1").arg(ccnt);

  QTabWidget *tabWidget = new QTabWidget(compareClass, "tabWidget");

  compareClass->addNewColumn(tabWidget);

  csetTB->addItem(tabWidget, name );

  compareClass->updateInfo();

  ccnt++;
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareSet::~CompareSet()
{
   // Destroy the list of column information.
printf("Destroy the list of column information.\n");
}
