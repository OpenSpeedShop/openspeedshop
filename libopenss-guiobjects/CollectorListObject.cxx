////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


#include <cstddef>
#include "CollectorListObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

class CollectorEntry;
#include "CollectorEntryClass.hxx"

#include <qvaluelist.h>

CollectorListObject::CollectorListObject(int expID)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("CollectorListObject::CollectorListObject(host=%d)\n", expID);

  createCollectorList(expID);
}

CollectorListObject::~CollectorListObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("delete all the collectorEntries!\n");

  CollectorEntry *ce = NULL;
  CollectorEntryList::Iterator it;
  for( it = collectorEntryList.begin();
       it != collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)(*it);
    delete ce;
  }

  collectorEntryList.clear();
}

using namespace OpenSpeedShop::Framework;
void
CollectorListObject::createCollectorList(int expID)
{
// do the lookup based on expID here...
  try
  {
    OpenSpeedShop::Framework::Experiment *experiment = NULL;
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
    }
    if( experiment == NULL )
    {
      return;
    }
//    ThreadGroup tgrp = experiment->getThreads();
//    if( tgrp.size() == 0 )
//    {
//      fprintf(stderr, "There are no known threads for this experiment.\n");
//      return;
//    }
//    ThreadGroup::iterator ti = tgrp.begin();
//    Thread t1 = *ti; 
    CollectorGroup cgrp = experiment->getCollectors();
    if( cgrp.size() > 0 )
    {
      for(CollectorGroup::iterator ci = cgrp.begin();ci != cgrp.end();ci++)
      {
        Collector collector = *ci;
            
        Metadata cm = collector.getMetadata();

        QString name = QString(cm.getUniqueId().c_str());
        QString short_name = QString(cm.getShortName().c_str());
        QString description = QString(cm.getDescription().c_str());
        CollectorEntry *ce = new CollectorEntry(name, short_name, description);

        Metadata m;
        std::set<Metadata> md =collector.getParameters();
        std::set<Metadata>::const_iterator mi;
        for (mi = md.begin(); mi != md.end(); mi++)
        {
          m = *mi;
// printf("%s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
// printf("%s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
// printf("%s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );

// While sub-marvelous this works for now...
            QString param = QString(m.getUniqueId().c_str());
            QString param_val = QString::null;
            double double_param;
            std::string string_param;
            unsigned int uint_param = 0;
            int int_param = 0;
            if( m.isType(typeid(int)) )
            {
// printf("int\n");
              collector.getParameterValue(param.ascii(), int_param);
              param_val = QString("%1").arg(int_param);
            } else if( m.isType(typeid(unsigned int)) )
            {
// printf("unsigned int\n");
              collector.getParameterValue(param.ascii(), uint_param);
              param_val = QString("%1").arg(uint_param);
            } else if( m.isType(typeid(double)) )
            {
// printf("double\n");
              collector.getParameterValue(param.ascii(), double_param);
              param_val = QString("%1").arg(double_param);
            } else if( m.isType(typeid(std::string)) )
            {
// printf("std::string\n");
              collector.getParameterValue(param.ascii(), string_param);
              param_val = QString("%1").arg(string_param.c_str());
            } else
            {
              param_val = QString("Unknown type.");
            }
            CollectorParameterEntry *cpe = new CollectorParameterEntry(param, param_val);
            ce->paramList.push_back(cpe);
        }

//Begin metrics
{
        Metadata m;
        std::set<Metadata> md =collector.getMetrics();
        std::set<Metadata>::const_iterator mi;
        for (mi = md.begin(); mi != md.end(); mi++)
        {
          m = *mi;
//printf("%s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
//printf("%s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
//printf("%s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );

// While sub-marvelous this works for now...
            QString metric = QString(m.getUniqueId().c_str());
            QString metric_val = QString::null;
            double double_metric;
            unsigned int uint_metric = 0;
            int int_metric = 0;
QString metric_type = QString::null;
            if( m.isType(typeid(int)) )
            {
// printf("int\n");
metric_type = "int";
            } else if( m.isType(typeid(unsigned int)) )
            {
// printf("unsigned int\n");
metric_type = "unsigned int";
            } else if( m.isType(typeid(double)) )
            {
// printf("double\n");
metric_type = "double";
            } else
            {
              metric_val = QString("Unknown type.");
metric_type = "unknown";
            }
            CollectorMetricEntry *cpe = new CollectorMetricEntry(metric, metric_type, metric_val);
            ce->metricList.push_back(cpe);
        }
}
//End metrics


        collectorEntryList.push_back(ce);
//      printf("sampling_rate=%u\n", sampling_rate);
      }
    }
  }
  catch(const std::exception& error)
  {
    std::cerr
      << std::endl 
      << "Error: " 
      << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
      "Unknown runtime error." : error.what()) << std::endl
      << std::endl;
    return;
  }
  return;
}



void
CollectorListObject::print()
{
}
