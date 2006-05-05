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

/**
 * class ThreadInfo and class ThreadRangeInfo.
 *
 * A Framework::Thread object is described with a host, pid,
 * posix(ormpi) thread ID, and an mpi rank ID.  This is the
 * information that the user sees and wha thte user needs to
 * to pass to the Openss when it is desired that particular
 * objects be processed.  It is often the case that much of
 * this information is redundant.  These classes are provided
 * in order to detect redundancy and compress the required
 * information that needs to be communicated to the user for
 * the porpose of uniquely identifing an object.
 *
 * The usual processing steps are:
 *
 * A particular Framework::Thread object is described with
 * a ThreadInfo entry. This is usually accomplished by using
 * the utility 'Build_ThreadInfo'.
 *
 * A set of ThreadInfo entries can be processed to recognize
 * and remove redundant information.  This is usually
 * accomplished by using the * utility 'Remove_Unnecessary_ThreadInfo'.
 *
 * A set of ThreadInfo entries can be compressed further by using
 * range information.  This is usually accomplished by using the
 * utility 'Compress_ThreadInfo'.
 *
 * The desired form of output that is neded to cummunicate wiht the
 * user can then be generated from the resulting ThreadRangeInfo
 * entries.
 */

class ThreadRangeInfo
{
 public:
  std::string hostName;
  pid_t processId_b;
  pid_t processId_e;
  int64_t threadId_b;
  int64_t threadId_e;
  int64_t rankId_b;
  int64_t rankId_e;

 public:
  ThreadRangeInfo (std::string hid, pid_t pid_b, int64_t tid_b, int64_t rid_b,
                                    pid_t pid_e, int64_t tid_e, int64_t rid_e) {
    hostName = hid;
    processId_b = pid_b;
    processId_e = pid_e;
    threadId_b = tid_b;
    threadId_e = tid_e;
    rankId_b = rid_b;
    rankId_e = rid_e;
  }

};

class ThreadInfo
{
 private:
  std::string hostName;
  pid_t processId;
  int64_t threadId;
  int64_t rankId;

 public:
  ThreadInfo (std::string hid, pid_t pid, int64_t tid, int64_t rid) {
    hostName = hid;
    processId = pid;
    threadId = tid;
    rankId = rid;
  }


 friend void Remove_Unnecessary_ThreadInfo (std::vector<std::vector<ThreadInfo> >& Info);
 friend void Compress_ThreadInfo (std::vector<ThreadInfo>& In, std::vector<ThreadRangeInfo>& Out);
 friend bool operator<(const ThreadInfo& T1, const ThreadInfo& T2);
};

void Build_ThreadInfo (ThreadGroup& tgrp, std::vector<ThreadInfo>& Out);
void Remove_Unnecessary_ThreadInfo (std::vector<ThreadInfo>& Out);
void Compress_ThreadInfo (std::vector<ThreadInfo>& In, std::vector<ThreadRangeInfo>& Out);
