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
    Out.push_back(ThreadInfo(HasThreadId, prank.first, hid, pid, pthreadid, rank));
  }
}

/**
 * Utility: Remove_Unnecessary_ThreadInfo
 *
 * Look through the set of ThreadInfo entries that is passed
 * in and try to determine a field that will uniquely identify
 * the associated thread.
 *
 * All the entries are scanned and each field is placed into a
 * set of the same field types.  If a new field has already been
 * placed into the set, that field contains duplicates and can
 * not be used to uniquely identify the Thread.
 *
 * If a unique field is found, all the other fields, for every
 * ThreadInfo object is set to a null value.  If multiple unique
 * fields are discovered, one is chosen in the following order:
 *  rankId
 *  processId
 *  threadId
 *  hostName
 *
 * @param  std::vector<std::vector<ThreadInfo> >&
 *
 * @return void - the input may be changed.
 *
 * Note: This algorithm can only detect conflicts in the information
 *  that it is passed.  To be useful with commands that may follow,
 *  all the Framework::Threads that are part of an experiment must be
 *  included in the argument.  This is why a list of lists is required
 *  as an argument - any unneeded Threads can be added to a vector that
 *  will later be ignored by the calling function.
 *
 */
void Remove_Unnecessary_ThreadInfo (std::vector<std::vector<ThreadInfo> >& Info) {
 // Are any of the fields non-repeating?
  std::set<string> hostset;
  bool hosts_are_unique = true;
  std::set<pid_t> pidset;
  bool pids_are_unique = true;
  std::set<int64_t> threadset;
  bool threads_are_unique = true;
  std::set<int64_t> rankset;
  bool ranks_are_unique = true;

  int64_t len = Info.size();
  for (int64_t ix = 0; ix < len; ix++) {
    std::vector<ThreadInfo>::iterator i;
    for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
      if (hosts_are_unique) {
        std::string s = i->hostName;
        if (hostset.find(s) != hostset.end()) {
          hosts_are_unique = false;
        } else {
          hostset.insert(s);
        }
      }
      if (pids_are_unique) {
        pid_t s = i->processId;
        if (s == 0) {
          pids_are_unique = false;
          pidset.clear();
        } else {
          if (pidset.find(s) != pidset.end()) {
            pids_are_unique = false;
          } else {
            pidset.insert(s);
          }
        }
      }
      if (threads_are_unique) {
        int64_t s = i->threadId;
        if (i->thread_required) {
          if (threadset.find(s) != threadset.end()) {
            threads_are_unique = false;
          } else {
            threadset.insert(s);
          }
        } else {
          threads_are_unique = false;
          threadset.clear();
        }
      }
      if (ranks_are_unique) {
        int64_t s = i->rankId;
        if (i->rank_required) {
          if (rankset.find(s) != rankset.end()) {
            ranks_are_unique = false;
          } else {
            rankset.insert(s);
          }
        } else {
          ranks_are_unique = false;
          rankset.clear();
        }
      }
    }
  }

  if (ranks_are_unique && !rankset.empty()) {
   // Ranks are a unique identifier.
   // We only need to preserve ranks. Get rid of the rest.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->host_required = false;
        i->pid_required = false;
        i->thread_required = false;
        i->processId = 0;
        i->threadId = 0;
      }
    }
  } else if (threads_are_unique && !threadset.empty()) {
   // Mpi/Posix thread Id's are a unique identifier.
   // We only need to preserve threads. Get rid of the rest.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->host_required = false;
        i->pid_required = false;
        i->rank_required = false;
        i->processId = 0;
        i->rankId = 0;
      }
    }
  } else if (pids_are_unique && !pidset.empty()) {
   // Pids are a unique identifier.
   // We only need to preserve pids. Get rid of the rest.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->host_required = false;
        i->thread_required = false;
        i->rank_required = false;
        i->threadId = 0;
        i->rankId = 0;
      }
    }
  } else {
   // Preserve host and pid, which is enough to uniquely identify the item.
   // We could keep thread and rank Id's, but it is not necessary and will
   // prevent us from finding ranges for the pids.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->thread_required = false;
        i->rank_required = false;
        i->threadId = 0;
        i->rankId = 0;
      }
    }
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

   // Initialize base information with first information definition.
    std::string current_host = i->hostName;
    pid_t end_pid = i->processId;
    int64_t end_thread = i->threadId;
    int64_t end_rank = i->rankId;

    bool merging_ranks = (i->rank_required);
    bool merging_threads = (!(merging_ranks) && (i->thread_required));
    bool merging_pids = (!(merging_threads) && (i->pid_required));
    bool have_hosts = i->host_required;

   // Create the first result entry.
    Out.push_back(ThreadRangeInfo(have_hosts, merging_pids, merging_threads, merging_ranks,
                                  current_host, end_pid,   end_thread,   end_rank));
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

        merging_ranks = (i->rank_required);
        merging_threads = (!(merging_ranks) && (i->thread_required));
        merging_pids = (!(merging_threads));
        have_hosts = i->host_required;

       // Add new item to <target_list>.
        Out.push_back(ThreadRangeInfo(have_hosts, merging_pids, merging_threads, merging_ranks,
                                      current_host, end_pid,   end_thread,   end_rank));
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
