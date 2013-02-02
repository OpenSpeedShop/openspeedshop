////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
//
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


#ifndef COLUMNSET_H
#define COLUMNSET_H



class QToolBox;
class CustomizeClass;
class QTabWidget;
class QComboBox;
class MPListView;

#include <stddef.h>
#include "CollectorListObject.hxx"  // For getting pid list off a host...
#include "CollectorEntryClass.hxx"

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Thread.hxx"
#include "LinkedObject.hxx"

#include "openspeedshop.hxx"
#include "CLIInterface.hxx"
#include <qvariant.h>

#include <qstring.h>


typedef std::pair<int64_t, std::string> pair_def;


class CompareSet;

class ColumnSet : public QWidget
{
  Q_OBJECT
  public:
    ColumnSet(QWidget *, CompareSet *);
    ~ColumnSet();

    QComboBox *experimentComboBox;
    QComboBox *collectorComboBox;
    QComboBox *metricComboBox;

    void updateInfo();

    CompareSet *compareSet;

    QString name;
    MPListView *lv;
    void initializeListView();

    int getExpidFromExperimentComboBoxStr( const QString path );
    void getDatabaseName(int expID );
  protected:

  private:
    int gatherExperimentInfo();
    CollectorEntry *gatherCollectorInfo(int);
    void gatherMetricInfo(CollectorEntry *);
   
    CollectorListObject *clo;
    CollectorEntry *ce;

    std::vector<pair_def> experiment_list;

  public slots:
    void changeExperiment( const QString &path );
    void changeCollector( const QString &path );

};

#endif // COLUMNSET_H
