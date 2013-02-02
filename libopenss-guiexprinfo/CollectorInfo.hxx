////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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


#ifndef COLLECTORINFO_H
#define COLLECTORINFO_H

#include "MetricInfo.hxx"

#include <stddef.h>  // For Qt iterators.... but make this non-qt iterator.
#include <qvaluelist.h>  // For Qt iterators.... but make this non-qt iterator.

//! Experiment Information Class
/*! Experiment Information Class is the class contain information about
    performance experiments.   Time, Data, Expreriment Type, and any sampling
    data, or pointers to sampling data.
 */

enum MetricHeaderType { UNKNOWN_T, INT_T, UNSIGNED_INT_T, UINT64_T, FLOAT_T, CHAR_T };

class MetricHeaderInfo
{
  public:

    MetricHeaderInfo() {};
    MetricHeaderInfo(QString l, MetricHeaderType t)
    {
      label = l;
      type = t;
    };

    QString label;
    MetricHeaderType type;
};


typedef QValueList<MetricInfo *> MetricInfoList;  // This should be some structure or class type...
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;

class CollectorInfo
{
public:
    /*! ExperInfo() - A default constructor the the Panel Class.
     */
    CollectorInfo();

    CollectorInfo(char *experiment_name);

    //! ~CollectorInfo() - The default destructor.
    /*! Destroys the object and frees any allocated resources.
     */
    ~CollectorInfo();

    MetricHeaderInfoList metricHeaderInfoList;
    int metricHeaderCount() { return metricHeaderInfoList.count(); };

    void init();
    void fudgeValues();

    char *targetProgram;
    char *experimentName;
    char *marchingOrders;
    char *experimentNotes;
    MetricInfoList metricInfoList;
};
#endif // COLLECTORINFO_H
