////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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


#ifndef COMPARESET_H
#define COMPARESET_H

#include <stddef.h>
#include <qvariant.h>
#include <qdialog.h>
#include <qpopupmenu.h>

#include <qstring.h>

class QToolBox;
class CustomizeClass;
class QString;
class QTabWidget;
class QComboBox;

class ColumnSet;

#include <qvaluelist.h>

typedef QValueList <ColumnSet *> ColumnSetList;

class CompareSet : public QObject
{
    Q_OBJECT
  public:
    CompareSet( QToolBox *csetTB, CustomizeClass *compareClass );
    ~CompareSet();

    QString name;
    QTabWidget *tabWidget;

    int tcnt;

    void updateInfo();

    CustomizeClass *compareClass;

    ColumnSetList columnSetList;

    void setNewFocus(QWidget *tab = NULL);

    void updatePSetList();
    void relabel();

  protected:

  private:

  private slots:
    void currentChanged( QWidget *);

};

#endif // COMPARESET_H
