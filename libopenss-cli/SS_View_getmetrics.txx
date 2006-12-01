/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/* Take the define below out of comments for debug
   output in several CLI routines that
   include this include file.........
*/
/* #define DEBUG_CLI 1 */

/* TEST */
template <typename TO, typename TS>
void GetMetricInThreadGroupByThread (
    const Collector& collector,
    const std::string& metric,
          std::vector<std::pair<Time,Time> >& intervals,
    const ThreadGroup& tgrp,
    const std::set<TO >& objects,
    SmartPtr<std::map<TO, std::map<Thread, TS > > >& individual)
{
   // Get the metric values for all the threads over all specified time intervals.
    for (std::vector<std::pair<Time,Time> >::iterator
                 iv = intervals.begin(); iv != intervals.end(); iv++) {
      Time Start_Time = iv->first;
      Time End_Time = iv->second;
#if DEBUG_CLI
      printf("In GetMetricInThreadGroupByThread, Start_Time.getValue()=%u\n", Start_Time.getValue());
      printf("In GetMetricInThreadGroupByThread, End_Time.getValue()=%u\n", End_Time.getValue());
#endif
      Queries::GetMetricValues(collector, metric,
                               TimeInterval(Start_Time, End_Time),
                               tgrp, objects, individual);
    }

}

template <typename TO, typename TS>
void ReduceMetricByThread (
    SmartPtr<std::map<TO, std::map<Thread, TS > > >& individual,
    TS (*reduction)(const std::map<Framework::Thread, TS >&),
    SmartPtr<std::map<TO, TS > >& result)
{
   // Allocate (if necessary) a new map of source objects to values
    if(result.isNull()) {
      result = SmartPtr<std::map<TO, TS > >(new std::map<TO, TS >());
      Assert(!result.isNull());
    }

   // Reduce the per-thread values.
    SmartPtr<std::map<TO, TS > > reduced =
        Queries::Reduction::Apply(individual, reduction);

   // Merge the temporary reduction into the actual results
    for(typename std::map<TO, TS >::const_iterator
            i = reduced->begin(); i != reduced->end(); ++i) {
        if(result->find(i->first) == result->end())
            result->insert(std::make_pair(i->first, i->second));
        else
            (*result)[i->first] += i->second;
    }

}

template <typename TO, typename TS>
void GetMetricInThreadGroup(
    const Collector& collector,
    const std::string& metric,
          std::vector<std::pair<Time,Time> >& intervals,
    const ThreadGroup& tgrp,
    const std::set<TO >& objects,
    SmartPtr<std::map<TO, TS > >& result)
{
   // Allocate (if necessary) a new map of source objects to values

#if DEBUG_CLI
    printf("GetMetricInThreadGroup, metric.c_str()=%s\n", metric.c_str());
#endif

    if(result.isNull()) {
      result = SmartPtr<std::map<TO, TS > >(new std::map<TO, TS >());
      Assert(!result.isNull());
    }

   // Get the metric values for all the threads over all specified time intervals.
    SmartPtr<std::map<TO, std::map<Thread, TS > > > individual;
#if DEBUG_CLI
    printf("GetMetricInThreadGroup, calling GetMetricInThreadGroupByThread, metric.c_str()=%s\n", metric.c_str());
#endif
    GetMetricInThreadGroupByThread (collector, metric, intervals, tgrp, objects, individual);

   // Reduce the per-thread values.
#if DEBUG_CLI
    printf("GetMetricInThreadGroup, calling ReduceMetricByThread, metric.c_str()=%s\n", metric.c_str());
#endif
    ReduceMetricByThread (individual, Queries::Reduction::Summation, result);

   // Reclaim space.
    individual = SmartPtr<std::map<TO, std::map<Thread, TS > > >();
}

/* TEST */

template <typename TI, typename TOBJECT>
void GetReducedSet (
          TI *dummyType,
          Collector& collector,
          std::string& metric,
          std::vector<std::pair<Time,Time> >& intervals,
          ThreadGroup& tgrp,
          std::set<TOBJECT>& objects,
          int64_t reduction_index,
          SmartPtr<std::map<TOBJECT, CommandResult *> >& items) {

  SmartPtr<std::map<TOBJECT, std::map<Thread, TI> > > individual;
  GetMetricInThreadGroupByThread (collector, metric, intervals, tgrp, objects, individual);

 // Reduce the per-thread values.
  SmartPtr<std::map<TOBJECT, TI > > result;
  switch (reduction_index) {
   case ViewReduction_min:
    ReduceMetricByThread (individual, Queries::Reduction::Minimum, result);
    break;
   case ViewReduction_max:
    ReduceMetricByThread (individual, Queries::Reduction::Maximum, result);
    break;
   default:
    ReduceMetricByThread (individual, Queries::Reduction::ArithmeticMean, result);
    break;
  }

 // Convert to typeless CommandResult objects.
  for(typename std::map<TOBJECT, TI>::const_iterator
      item = result->begin(); item != result->end(); ++item) {
    std::pair<TOBJECT, TI> p = *item;
    items->insert( std::make_pair(p.first, CRPTR (p.second)) );
  }
}

template <typename TOBJECT>
bool GetReducedType (
          CommandObject *cmd,
          ExperimentObject *exp,
          ThreadGroup& tgrp,
          Collector &collector,
          std::string &metric,
          std::set<TOBJECT>& objects,
          int64_t reduction_index,
          SmartPtr<std::map<TOBJECT, CommandResult *> >& items) {

  Framework::Experiment *experiment = exp->FW();
#if DEBUG_CLI
  printf("Enter GetReducedType, in SS_View_getmetics.txx\n"); 
#endif

 // Get the list of desired objects.
  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no objects specified for metric reduction.)");
    return false;
  }

  std::vector<std::pair<Time,Time> > intervals;
  Parse_Interval_Specification (cmd, exp, intervals);

  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

  if( m.isType(typeid(unsigned int)) ) {
    uint *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else if( m.isType(typeid(uint64_t)) ) {
    uint64_t *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else if( m.isType(typeid(int)) ) {
    int *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else if( m.isType(typeid(float)) ) {
    float *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else if( m.isType(typeid(double)) ) {
    double *V;
    GetReducedSet (V, collector, metric, intervals, tgrp, objects, reduction_index, items);
  } else {
    std::string S("(Cluster Analysis can not be performed on metric '");
    S = S +  metric + "' of type '" + m.getType() + "'.)";
    Mark_Cmd_With_Soft_Error(cmd, S);
#if DEBUG_CLI
    printf("Exit false1 GetReducedType, in SS_View_getmetics.txx\n"); 
#endif
    return false;
  }
  if (items->size() == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for metric reduction.)");
#if DEBUG_CLI
    printf("Exit false2 GetReducedType, in SS_View_getmetics.txx\n"); 
#endif
    return false;
  }

#if DEBUG_CLI
  printf("Exit true GetReducedType, in SS_View_getmetics.txx\n"); 
#endif
  return true;
}
