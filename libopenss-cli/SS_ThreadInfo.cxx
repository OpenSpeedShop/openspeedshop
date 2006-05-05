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
 * Framework::Thread in the passed-in THreadGroup.  Build a single ThreadInfo
 * object for each of these Threads and put them into a std:vector that can
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
    bool threadHasThreadId = false;
    int64_t pthreadid = -1;
    if (pthread.first) {
      threadHasThreadId = true;
      pthreadid = pthread.second;
    } else {
      std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
      if (posixthread.first) {
        threadHasThreadId = true;
        pthreadid = posixthread.second;
      }
    }
    std::pair<bool, int> prank = j->getMPIRank();
    int64_t rank = prank.first ? prank.second : -1;
    Out.push_back(ThreadInfo(hid, pid, pthreadid, rank));
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
        if (s < 0) {
          threads_are_unique = false;
          threadset.clear();
        } else {
          if (threadset.find(s) != threadset.end()) {
            threads_are_unique = false;
          } else {
            threadset.insert(s);
          }
        }
      }
      if (ranks_are_unique) {
        int64_t s = i->rankId;
        if (s < 0) {
          ranks_are_unique = false;
          rankset.clear();
        } else {
          if (rankset.find(s) != rankset.end()) {
            ranks_are_unique = false;
          } else {
            rankset.insert(s);
          }
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
        i->hostName = "";
        i->processId = 0;
        i->threadId = -1;
      }
    }
  } else if (threads_are_unique && !threadset.empty()) {
   // Mpi/Posix thread Id's are a unique identifier.
   // We only need to preserve threads. Get rid of the rest.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->hostName = "";
        i->processId = 0;
        i->rankId = -1;
      }
    }
  } else if (pids_are_unique && !pidset.empty()) {
   // Pids are a unique identifier.
   // We only need to preserve pids. Get rid of the rest.
    for (int64_t ix = 0; ix < len; ix++) {
      std::vector<ThreadInfo>::iterator i;
      for (i = Info[ix].begin(); i != Info[ix].end(); i++) {
        i->hostName = "";
        i->threadId = -1;;
        i->rankId = -1;
      }
    }
  }
 // else - Preserve everything!
}

/**
 * Utility: Compress_ThreadInfo
 *
 * A vector of ThreadInfo entries is sorted and then scanned to
 * detect ranges of values.  This allows multiple descriptions to
 * be expressed in a single description by using the range notation
 * that is available in the CLI command language.
 *
 * @param  std::vector<ThreadInfo>&.
 * @param  std::vector<ThreadRangeInfo>&.
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
    pid_t begin_pid = i->processId;
    pid_t end_pid = begin_pid;
    int64_t begin_thread = i->threadId;
    int64_t end_thread = begin_thread;
    int64_t begin_rank = i->rankId;
    int64_t end_rank = begin_rank;

    bool merging_ranks = (begin_rank >= 0);
    bool merging_threads = (!(merging_ranks) && (begin_thread >= 0));
    bool merging_pids = (!(merging_threads) && (begin_pid > 0));

   // Go through the rest of the information definitions.
    for (i++; i != In.end(); i++) {
      std::string next_host = i->hostName;
      pid_t next_pid = i->processId;
      int64_t next_thread = i->threadId;
      int64_t next_rank = i->rankId;

      if ((next_host != current_host) ||
          (!merging_pids && (next_pid != begin_pid)) ||
          (!merging_threads && (next_thread != begin_thread)) ||
          (!merging_ranks && (next_rank != begin_rank))) {
       // Add last set of compressed information to the output vector.
        Out.push_back(ThreadRangeInfo(current_host, begin_pid, begin_thread, begin_rank,
                                                      end_pid,   end_thread,   end_rank));
       // Reinitialize.
        current_host = next_host;
        begin_pid = next_pid;
        end_pid = begin_pid;
        begin_thread = next_thread;
        end_thread = begin_thread;
        begin_rank = next_rank;
        end_rank = begin_rank;

        merging_ranks = (begin_rank >= 0);
        merging_threads = (!(merging_ranks) && (begin_thread >= 0));
        merging_pids = (!(merging_threads) && (begin_pid > 0));

        continue;
      }

      if (merging_ranks) {
        if (begin_rank == -1) {
          begin_rank = next_rank;
          end_rank = next_rank;
        } else if (next_rank == (end_rank+1)) {
          end_rank = next_rank;
        } else {
         // Add to output vector.
          Out.push_back(ThreadRangeInfo(current_host, begin_pid, begin_thread, begin_rank,
                                                        end_pid,   end_thread,   end_rank));
         // Reinitialize.
          begin_rank = next_rank;
          end_rank = next_rank;
        }
      } else if (merging_threads) {
        if (begin_thread == -1) {
          begin_thread = next_thread;
          end_thread = next_thread;
        } else if (next_thread == (end_thread+1)) {
          end_thread = next_thread;
        } else {
         // Add to output vector.
          Out.push_back(ThreadRangeInfo(current_host, begin_pid, begin_thread, begin_rank,
                                                        end_pid,   end_thread,   end_rank));
         // Reinitialize.
          begin_thread = next_thread;
          end_thread = next_thread;
        }
      } else if (merging_pids) {
        if (begin_pid == -1) {
          begin_pid = next_pid;
          end_pid = next_pid;
        } else if (next_pid == (end_pid+1)) {
          end_pid = next_pid;
        } else {
         // Add to output vector.
          Out.push_back(ThreadRangeInfo(current_host, begin_pid, begin_thread, begin_rank,
                                                        end_pid,   end_thread,   end_rank));
         // Reinitialize.
          begin_pid = next_pid;
          end_pid = next_pid;
        }
      }
    }

   // Add last item to output vector.
    Out.push_back(ThreadRangeInfo(current_host, begin_pid, begin_thread, begin_rank,
                                                  end_pid,   end_thread,   end_rank));
  }

}
