/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2011 Krell Institute  All Rights Reserved.
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


#include "SS_Input_Manager.hxx"

/**
 * Utility: operator<
 *
 * One ThreadInfo object is less than another if a field of one
 * object is less than the corresponding field in the other object.
 * The fields are searched in the following order:
 *  rankId
 *  processId
 *  threadId
 *  hostName
 *
 * @param ThreadInfo objects.
 *
 * @return boolean true if the first object is less than the second.
 *
 */
bool operator<(const ThreadInfo& T1, const ThreadInfo& T2) {
  if (T1.rankId != T2.rankId) return T1.rankId < T2.rankId;
  if (T1.processId != T2.processId) return T1.processId < T2.processId;
  if (T1.threadId != T2.threadId) return T1.threadId < T2.threadId;
  if (T1.hostName != T2.hostName) return T1.hostName < T2.hostName;
  return false;
}

template <class T>
struct sort_ascending_ThreadInfo : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x < y;
    }
};

/**
 * Utility: Build_ThreadInfo
 *
 * Access the information from the database that is associated with each
 * Framework::Thread in the passed-in ThreadGroup.  Build a single ThreadInfo
 * object for each of these Threads and put them into a std::vector that can
 * be used by other utilities for the purpose of determining what fields
 * are neded to uniquely identify the Thread.
 *
 * @param  the Framework::ThreadGroup used for input.
 * @param  the result vector.
 *
 * @return  void - results are placed in the second argument.
 *
 */
void Build_ThreadInfo (ThreadGroup& tgrp, std::vector<ThreadInfo>& Out) {
  for(ThreadGroup::const_iterator
           j = tgrp.begin(); j != tgrp.end(); ++j) {

    Thread t = *j;
    std::string hid = j->getHost();
    pid_t pid = j->getProcessId();
    std::pair<bool, int> pthread = t.getOpenMPThreadId();
    bool HasThreadId = false;
    int64_t pthreadid = -1;
    if (pthread.first) {
      HasThreadId = true;
      pthreadid = pthread.second;
    } else {
      std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
      if (posixthread.first) {
        HasThreadId = true;
        pthreadid = posixthread.second;
      }
    }
    std::pair<bool, int> prank = j->getMPIRank();
    int64_t rank = prank.first ? prank.second : -1;
    Out.push_back(ThreadInfo(hid, pid, pthreadid, rank));
  }
}

/**
 * Utility: Compress_ThreadInfo
 *
 * A vector of ThreadInfo entries is sorted and then scanned to
 * detect ranges of values.  This allows multiple descriptions to
 * be expressed in a single description by using the range notation
 * that is available in the CLI command language and described in
 * documentation as <target_list>.
 *
 * @param  std::vector<ThreadInfo>& In.
 * @param  std::vector<ThreadRangeInfo>& Out.
 *
 * @return  void - data is returned through the second argument.
 *
 */

void Compress_ThreadInfo (std::vector<ThreadInfo>& In,
			  std::vector<ThreadRangeInfo>& Out) {

  if (!In.empty()) {
   // Order them for the next step.
    std::sort (In.begin(), In.end(),
	       sort_ascending_ThreadInfo<ThreadInfo>());

   // Determine the ranges that are covered.
    std::vector<ThreadInfo>::iterator i = In.begin();

   // Identify a field with multiple data samples.
   // This is the field we will use to compress ranges.
    int64_t host_count = 0;
    int64_t pid_count = 0;
    int64_t thread_count = 0;
    int64_t rank_count = 0;
    for (i = In.begin(); i != In.end(); i++) {
      host_count += ((i->hostName != "") ? 1 : 0);
      pid_count += ((i->processId) ? 1 : 0);
      thread_count += ((i->threadId) ? 1 : 0);
      rank_count += ((i->rankId >= 0) ? 1 : 0);
    }

   // We are only attempting to merge one field.
    bool merging_ranks = (rank_count > 1);
    bool merging_threads = (!(merging_ranks) && (thread_count > 1));
    bool merging_pids = (!(merging_threads) && !(merging_ranks) && (pid_count > 1));
    bool have_hosts = (host_count > 1);

 
   // Determine the ranges that are covered for the selected field.
   // Initialize base information with first information definition.
    i = In.begin();
    std::string current_host = i->hostName;
    pid_t end_pid = i->processId;
    int64_t end_thread = i->threadId;
    int64_t end_rank = i->rankId;

   // Create the first result entry.
    Out.push_back(ThreadRangeInfo(current_host, end_pid,   end_thread,   end_rank));
    ThreadRangeInfo *currentTRI = &Out[0];

   // Go through the rest of the information definitions.
    for (i++; i != In.end(); i++) {
      std::string next_host = i->hostName;
      pid_t next_pid = i->processId;
      int64_t next_thread = i->threadId;
      int64_t next_rank = i->rankId;

      if (have_hosts && (next_host != current_host) ||
	  (!merging_pids && (next_pid != end_pid)) ||
	  (!merging_threads && (next_thread != end_thread)) ||
	  (!merging_ranks && (next_rank != end_rank))) {

       // Reinitialize.
	current_host = next_host;
	end_pid = next_pid;
	end_thread = next_thread;
	end_rank = next_rank;

       // Add new item to <target_list>.
        Out.push_back(ThreadRangeInfo(current_host, end_pid, end_thread, end_rank));
        currentTRI = &Out[Out.size()-1];
	continue;
      }

      if (merging_ranks) {
	if (next_rank == (end_rank+1)) {
         // Extend current range.
          currentTRI->rankId[currentTRI->rankId.size()-1].second = next_rank;
	} else {
	 // Add new range.
          currentTRI->rankId.push_back(std::make_pair(next_rank,next_rank));
        }
        end_rank = next_rank;
      } else if (merging_threads) {
	if (next_thread == (end_thread+1)) {
         // Extend current range.
          currentTRI->threadId[currentTRI->threadId.size()-1].second = next_thread;
	} else {
	 // Add new range.
          currentTRI->threadId.push_back(std::make_pair(next_thread,next_thread));
	}
        end_thread = next_thread;
      } else if (merging_pids) {
	if (next_pid == (end_pid+1)) {
         // Extend current range.
          currentTRI->processId[currentTRI->processId.size()-1].second = next_pid;
	} else {
	 // Add new range.
          currentTRI->processId.push_back(std::make_pair(next_pid,next_pid));
	}
        end_pid = next_pid;
      }
    }

  }

}
