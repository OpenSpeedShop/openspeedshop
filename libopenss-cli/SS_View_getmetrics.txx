/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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
   #define DEBUG_CLI 1 
   #define DEBUG_CLI_DETAILS 1 
*/


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
      printf("In GetMetricInThreadGroupByThread, before Queries::GetMetricValues, objects.size()=%d\n", objects.size());
#endif

      Queries::GetMetricValues(collector, metric,
                               TimeInterval(Start_Time, End_Time),
                               tgrp, objects, individual);
#if DEBUG_CLI
    printf("In GetMetricInThreadGroupByThread, after Queries::GetMetricValues, objects.size()=%d\n", objects.size());
#endif
    }

}

template <typename TO, typename TS>
void ReduceMetricByThread ( SmartPtr<std::map<TO, std::map<Thread, TS > > >& individual,
                            TS (*reduction)(const std::map<Framework::Thread, TS >&),
                            SmartPtr<std::map<TO, TS > >& result)
{
   // Allocate (if necessary) a new map of source objects to values
    if(result.isNull()) {
      result = SmartPtr<std::map<TO, TS > >(new std::map<TO, TS >());
      Assert(!result.isNull());
    }

#if DEBUG_CLI
    std::cout << "In ReduceMetricByThread  individual->size()=" << individual->size() << std::endl;
#endif

   // Reduce the per-thread values.
    SmartPtr<std::map<TO, TS > > reduced = Queries::Reduction::Apply(individual, reduction);

#if DEBUG_CLI
    std::cout << "In ReduceMetricByThread  reduced->size()=" << reduced->size() << std::endl;
#endif

    // Merge the temporary reduction into the actual results
    for(typename std::map<TO, TS >::const_iterator i = reduced->begin(); i != reduced->end(); ++i) {


#if DEBUG_CLI_DETAILS
        fprintf(stderr, "ReduceMetricByThread, inside reduction loop\n");
        fflush(stderr);
#endif
   
        if(result->find(i->first) == result->end()) {
            result->insert(std::make_pair(i->first, i->second));
        } else {
            (*result)[i->first] += i->second;
        }
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
    printf("GetMetricInThreadGroup, metric.c_str()=%s, objects.size()=%d\n", metric.c_str(), objects.size());
#endif

    if(result.isNull()) {
#if DEBUG_CLI
      printf("GetMetricInThreadGroup, result.isNull is true\n");
#endif
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

#if DEBUG_CLI
    printf("GetMetricInThreadGroup, EXIT\n");
#endif
}

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
   case ViewReduction_mean:
    ReduceMetricByThread (individual, Queries::Reduction::ArithmeticMean, result);
    break;
   default:
    return;
  }

 // Convert to typeless CommandResult objects.
  for(typename std::map<TOBJECT, TI>::const_iterator
      item = result->begin(); item != result->end(); ++item) {
    std::pair<TOBJECT, TI> p = *item;
//    std::cout << "GetReducedSet, p.second=" << p.second << std::endl;
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


// MinMaxIndex support

int64_t inline  getMinMaxThreadId (Framework::Thread t,
                                   int64_t thread_identifier)
{
  switch (thread_identifier)
  {
    case View_ByThread_Rank:
    {
      std::pair<bool, int> prank = t.getMPIRank();
      if (prank.first) {
        return prank.second;
      }
      break;
    }
    case View_ByThread_OpenMPThread:
    {
      std::pair<bool, int> pthread = t.getOpenMPThreadId();
      if (pthread.first) {
        return pthread.second;
      }
      break;
    }
    case View_ByThread_PosixThread:
    {
      std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
      if (posixthread.first) {
        return posixthread.second;
      }
      break;
    }
    case View_ByThread_Process:
    {
      pid_t pid = t.getProcessId();
      return (int64_t)pid;
    }
  }
  return -1;
}

template <typename TOBJECT, typename TM>
void GetMaxMinIdxAvg (
    const Framework::SmartPtr<
        std::map<TOBJECT, std::map<Framework::Thread, TM > > >& individual,
    int64_t thread_identifier,
    SmartPtr<std::map<TOBJECT, CommandResult *> > min_items,
    SmartPtr<std::map<TOBJECT, CommandResult *> > imin_items,
    SmartPtr<std::map<TOBJECT, CommandResult *> > max_items,
    SmartPtr<std::map<TOBJECT, CommandResult *> > imax_items,
    SmartPtr<std::map<TOBJECT, CommandResult *> > average_items)
{
    // Check preconditions
    Assert(!individual.isNull());

#if DEBUG_CLI
  printf("Enter GetMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif

    // Iterate over each source object in the individual results
    for(typename std::map<TOBJECT, std::map<Framework::Thread, TM > >::const_iterator
            j = individual->begin(); j != individual->end(); ++j) {

	// Determine min, max and locations
        TOBJECT object = j->first;
        std::map<Framework::Thread, TM > im = j->second;
        typename std::map<Framework::Thread, TM >::const_iterator i = im.begin();
        if (i != im.end()) {

        int64_t imin = getMinMaxThreadId(i->first, thread_identifier);
        TM vmin = i->second;
        int64_t imax = getMinMaxThreadId(i->first, thread_identifier);
        TM vmax = i->second;
        TM sum = i->second;
        int64_t threadcnt = 1;

        for(++i ; i != im.end(); ++i) {

            if (vmax < i->second) {
                imax = getMinMaxThreadId(i->first, thread_identifier);
                vmax = i->second;
            } else if (i->second < vmin) {
                imin = getMinMaxThreadId(i->first, thread_identifier);
                vmin = i->second;
            }
            sum += i->second;
            threadcnt++;
        }

#if DEBUG_CLI
  printf("Insert the results of  GetMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif

       // Convert to typeless CommandResult objects.
        min_items->insert( std::make_pair(object, CRPTR (vmin)) );
        CommandResult *cr_imin = CRPTR (imin);
        cr_imin->SetValueIsID();
        imin_items->insert(std::make_pair(object, cr_imin) );
        max_items->insert(std::make_pair(object, CRPTR (vmax)) );
        CommandResult *cr_imax = CRPTR (imax);
        cr_imax->SetValueIsID();
        imax_items->insert(std::make_pair(object, cr_imax) );
        average_items->insert(std::make_pair(object, CRPTR (sum / threadcnt) ));
        }
    }

}

template <typename TI, typename TOBJECT>
void GetReducedMaxMinIdxAvgSet (
          TI *dummyType,
          Collector& collector,
          std::string& metric,
          std::vector<std::pair<Time,Time> >& intervals,
          ThreadGroup& tgrp,
          int64_t thread_identifier,
          std::set<TOBJECT>& objects,
          SmartPtr<std::map<TOBJECT, CommandResult *> > min_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > imin_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > max_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > imax_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > average_items)
{
#if DEBUG_CLI
  printf("Enter GetReducedMaxMinIdxAvgSet, in SS_View_getmetics.txx\n"); 
#endif

  SmartPtr<std::map<TOBJECT, std::map<Thread, TI> > > individual;
  GetMetricInThreadGroupByThread (collector, metric, intervals, tgrp, objects, individual);

#if DEBUG_CLI
  printf("Call GetMaxMinIdxAvg from GetReducedMaxMinIdxAvgSet, in SS_View_getmetics.txx\n"); 
#endif

 // Reduce the per-thread values.
  GetMaxMinIdxAvg (individual, thread_identifier,
                   min_items, imin_items, max_items, imax_items, average_items);

#if DEBUG_CLI
  printf("Process results in GetReducedMaxMinIdxAvgSet, in SS_View_getmetics.txx\n"); 
#endif
}



template <typename TOBJECT>
bool GetReducedMaxMinIdxAvg (
          CommandObject *cmd,
          ExperimentObject *exp,
          ThreadGroup& tgrp,
          Collector &collector,
          std::string &metric,
          int64_t thread_identifier,
          std::set<TOBJECT>& objects,
          SmartPtr<std::map<TOBJECT, CommandResult *> > min_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > imin_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > max_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > imax_items,
          SmartPtr<std::map<TOBJECT, CommandResult *> > average_items)
{
#if DEBUG_CLI
  printf("Enter GetReducedMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif
  Framework::Experiment *experiment = exp->FW();

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
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else if( m.isType(typeid(uint64_t)) ) {
    uint64_t *V;
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else if( m.isType(typeid(int)) ) {
    int *V;
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t *V;
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else if( m.isType(typeid(float)) ) {
    float *V;
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else if( m.isType(typeid(double)) ) {
    double *V;
    GetReducedMaxMinIdxAvgSet (V, collector, metric, intervals, tgrp, thread_identifier, objects,
                               min_items, imin_items, max_items, imax_items, average_items);
  } else {
    std::string S("(Cluster Analysis can not be performed on metric '");
    S = S +  metric + "' of type '" + m.getType() + "'.)";
    Mark_Cmd_With_Soft_Error(cmd, S);
#if DEBUG_CLI
    printf("Exit false1 GetMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif
    return false;
  }
  if (min_items->size() == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for metric reduction.)");
#if DEBUG_CLI
    printf("Exit false2 GetMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif
    return false;
  }

#if DEBUG_CLI
  printf("Exit true GetMaxMinIdxAvg, in SS_View_getmetics.txx\n"); 
#endif
  return true;
}
