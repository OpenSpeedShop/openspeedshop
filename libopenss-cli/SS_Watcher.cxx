/******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006 Krell Institute  All Rights Reserved.
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
#include "SS_Timings.hxx"

//#define DEBUG_SYNC 1
//#define DEBUG_SYNC_SIGNAL 1

inline std::string int2str (int64_t e) {
  char s[40];
  sprintf ( s, "%lld", e);
  return std::string (s);
}

class Watch_Item;  // forward def

static bool Watcher_Active = false;
static CommandObject cmd;  // Needed to use the lock in the experiment
static pthread_mutex_t Watch_Item_Lock = PTHREAD_MUTEX_INITIALIZER;
static bool            Waiting_to_Watch = false;
static pthread_cond_t  Waiting_For_Items = PTHREAD_COND_INITIALIZER;
int WaitCount = 1000;
static std::list<Watch_Item *> Watch_Item_list;

class Watch_Item {
  friend void SS_Watch_Status ();

 public:
  enum itemCode { printTermination,
                  waitTermination
                };

  itemCode itemtype;
  union {
    struct
      { EXPID expid;
        CMDWID window; } p;
    struct
      { EXPID expid;
        pthread_cond_t condition; } w;
  } item;

  Watch_Item () {};

 public:
  Watch_Item (ExperimentObject *exp) {
    pthread_cond_init(&item.w.condition, (pthread_condattr_t *)NULL);
    item.w.expid = exp->ExperimentObject_ID();
    itemtype = waitTermination;
  }
  Watch_Item (CMDWID Watch_Window, ExperimentObject *exp) {
    item.p.window = Watch_Window;
    item.p.expid = exp->ExperimentObject_ID();
    itemtype = printTermination;
  }
  ~Watch_Item () {
    if (itemtype == waitTermination) {
     // Let the Command Processor continue to read commands.
      Assert(pthread_cond_signal(&item.w.condition) == 0);
      pthread_cond_destroy (&item.w.condition);
    }
  }
  
};

// The user interface is through specific utilitiy interfaces.

void Cancle_Async_Notice (ExperimentObject *exp) {
  if (!Watcher_Active) return;
  EXPID id = (exp != NULL) ? exp->ExperimentObject_ID() : 0;
  if (id != 0) {
    std::list<Watch_Item *>::iterator wli;
    Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

    for ( wli = Watch_Item_list.begin(); wli != Watch_Item_list.end(); ) {
      bool purge_item = false;
      Watch_Item *wi = *wli;
      wli++;
      if ((wi->itemtype == Watch_Item::printTermination) &&
          (id == wi->item.p.expid)) {
        Watch_Item_list.remove(wi);
        delete wi;
      }
    }

    Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
  }
}

void Request_Async_Notice_Of_Termination (CommandObject *cmd, ExperimentObject *exp) {

#if DEBUG_SYNC
  printf("[TID=%ld], Request_Async_Notice_Of_Termination, Watcher_Active=%ld\n", pthread_self(),  Watcher_Active);
#endif

  if (!Watcher_Active) return;
  InputLineObject *clip = cmd->Clip();
  CMDWID forwindow = (clip != NULL) ? clip->Who() : 0;

#if DEBUG_SYNC
  printf("[TID=%ld], Request_Async_Notice_Of_Termination, exp=%ld, forwindow=%ld\n", pthread_self(),  exp, forwindow);
#endif

  if ((exp == NULL) ||
      (forwindow == 0)) return;

 // Get rid of any extra notices.
  Cancle_Async_Notice (exp);

 // Post a new notice.
  Watch_Item *item =  new Watch_Item (forwindow, exp);

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Request_Async_Notice_Of_Termination, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Request_Async_Notice_Of_Termination, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif

  Watch_Item_list.push_back(item);

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Request_Async_Notice_Of_Termination, before if, Waiting_to_Watch=%ld) \n", pthread_self(),  Waiting_to_Watch);
#endif

  if (Waiting_to_Watch) {
    Waiting_to_Watch = false;

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Request_Async_Notice_Of_Termination, Waiting_to_Watch, before calling pthread_cond_signal(&Waiting_For_Items=%ld)\n", pthread_self(),  Waiting_For_Items);
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Request_Async_Notice_Of_Termination, Waiting_to_Watch, before calling pthread_cond_signal WaitCount=%ld \n", pthread_self(), WaitCount);
#endif

    Assert(pthread_cond_signal(&Waiting_For_Items) == 0);

#if DEBUG_SYNC_SIGNAL
    WaitCount = WaitCount - 1;
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Request_Async_Notice_Of_Termination, Waiting_to_Watch, after calling pthread_cond_signal(&Waiting_For_Items=%ld) \n", pthread_self(),  Waiting_For_Items);
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Request_Async_Notice_Of_Termination, Waiting_to_Watch, after calling pthread_cond_signal WaitCount=%ld \n", pthread_self(), WaitCount);
#endif

  }

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Request_Async_Notice_Of_Termination, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Request_Async_Notice_Of_Termination, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

}

void Cancle_Exp_Wait (ExperimentObject *exp) {

#if DEBUG_SYNC
  printf("[TID=%ld], Cancle_Exp_Wait, entered, Watcher_Active=%ld\n", pthread_self(),  Watcher_Active);
#endif

  if (!Watcher_Active) return;

  EXPID id = (exp != NULL) ? exp->ExperimentObject_ID() : 0;

  if (id != 0) {

    std::list<Watch_Item *>::iterator wli;

#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: Cancle_Exp_Wait, entering, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: Cancle_Exp_Wait, entering, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    for ( wli = Watch_Item_list.begin(); wli != Watch_Item_list.end(); ) {
      bool purge_item = false;
      Watch_Item *wi = *wli;
      wli++;
      if ((wi->itemtype == Watch_Item::waitTermination) &&
          (id == wi->item.w.expid)) {
#if DEBUG_SYNC
        printf("[TID=%ld], Cancle_Exp_Wait, removing Watch_Item::waitTermination for expID=%ld)  \n", pthread_self(),  wi->item.w.expid );
#endif
        Watch_Item_list.remove(wi);
        delete wi;
      }
    }

#if DEBUG_SYNC
    printf("[TID=%ld], UNLOCK: Cancle_Exp_Wait, exiting, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
    printf("[TID=%ld], UNLOCK: Cancle_Exp_Wait, exiting, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  }
}

void Wait_For_Exp (CommandObject *cmd, ExperimentObject *exp) {
  if (!Watcher_Active) return;
  if (exp == NULL) return;

 // Post a new notice.
  Watch_Item *item =  new Watch_Item (exp);

#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: Wait_For_Exp, Post a new notice, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: Wait_For_Exp, Post a new notice, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Watch_Item_list.push_back(item);

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], Wait_For_Exp, before check, Waiting_to_Watch, Waiting_to_Watch=%ld) \n", pthread_self(),  Waiting_to_Watch);
#endif

  if (Waiting_to_Watch) {
    Waiting_to_Watch = false;

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Wait_For_Exp, Waiting_to_Watch, before calling pthread_cond_signal(&Waiting_For_Items=%ld), WaitCount=%ld \n", pthread_self(),  Waiting_For_Items, WaitCount);
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Wait_For_Exp, Waiting_to_Watch, before calling pthread_cond_signal(), WaitCount=%ld \n", pthread_self(), WaitCount);
#endif

    Assert(pthread_cond_signal(&Waiting_For_Items) == 0);

#if DEBUG_SYNC_SIGNAL
    WaitCount = WaitCount - 1;
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Wait_For_Exp, Waiting_to_Watch, after calling pthread_cond_signal(&Waiting_For_Items=%ld), WaitCount=%ld \n", pthread_self(),  Waiting_For_Items, WaitCount);
    printf("[TID=%ld], PTHREAD_COND_SIGNAL: Wait_For_Exp, Waiting_to_Watch, after calling pthread_cond_signal(), WaitCount=%ld \n", pthread_self(), WaitCount);
#endif

  }

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_WAIT: Wait_For_Exp, Go to sleep until SS_Watcher or ~Watch_Item () wakes me up. before calling pthread_cond_wait(&(item->item.w.condition=%ld),&Watch_Item_Lock=%ld\n", pthread_self(),  item->item.w.condition, Watch_Item_Lock);
#endif

  // Go to sleep until SS_Watcher or ~Watch_Item () wakes me up.
  Assert(pthread_cond_wait(&(item->item.w.condition),&Watch_Item_Lock) == 0);

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_WAIT: Wait_For_Exp, Go to sleep until SS_Watcher or ~Watch_Item () wakes me up. after calling pthread_cond_wait(&(item->item.w.condition=%ld),&Watch_Item_Lock=%ld\n", pthread_self(),  item->item.w.condition, Watch_Item_Lock);
#endif


#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Wait_For_Exp, Return control to caller, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

 // Return control to caller.
  Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Wait_For_Exp, Return control to caller, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif
}

void Purge_Watcher_Events () {
  if (!Watcher_Active) return;

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Purge_Watcher_Events, Watcher_Active, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Purge_Watcher_Events, Watcher_Active, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  std::list<Watch_Item *>::iterator wli;
  for ( wli = Watch_Item_list.begin(); wli != Watch_Item_list.end(); ) {
    Watch_Item *wi = *wli;
    wli++;
    Watch_Item_list.remove(wi);
    delete wi;
  }

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Purge_Watcher_Events, Watcher_Active, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Purge_Watcher_Events, Watcher_Active, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

}

void Purge_Watcher_Waits () {
  if (!Watcher_Active) return;

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Purge_Watcher_Waits, Watcher_Active, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], LOCK: Purge_Watcher_Waits, Watcher_Active, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  std::list<Watch_Item *>::iterator wli;
  for ( wli = Watch_Item_list.begin(); wli != Watch_Item_list.end(); ) {
    Watch_Item *wi = *wli;
    wli++;
    if (wi->itemtype == Watch_Item::waitTermination) {
#if DEBUG_SYNC
      printf("[TID=%ld], Purge_Watcher_Waits, Watcher_Active, after removing waitTermination\n", pthread_self() );
#endif
      Watch_Item_list.remove(wi);
      delete wi;
    }
  }

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Purge_Watcher_Waits, Watcher_Active, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

  Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
  printf("[TID=%ld], UNLOCK: Purge_Watcher_Waits, Watcher_Active, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

}

// Catch SIGUSR1 signal and exit, when sent from main control.
static void
catch_TLI_signal (int sig, int error_num)
{
 // Clean up.
  Purge_Watcher_Events ();

 // This isn't graceful, but it gets the job done.
  pthread_exit(0);
}

// This routine continuously checks the status of experiments
// and will notify the user if an application terminates.
void SS_Watcher () {

    // Gather performance information on the cli's watcher thread 
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliWatcherStart] = Time::Now();
    }

#if DEBUG_SYNC
  printf("[TID=%ld], SS_Watcher, Entered\n", pthread_self() );
#endif

  Watcher_Active = true;

#if DEBUG_SYNC
  printf("[TID=%ld], SS_Watcher, setting Watcher_Active\n", pthread_self() );
#endif

 // Set up to catch keyboard control signals
  SET_SIGNAL (SIGUSR1, catch_TLI_signal);  // request to terminate processing

 // Allow us to be terminated from the main thread.
  int previous_value;
  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, &previous_value);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &previous_value);

 // Infinite loop to watch for special conditions.
  for(;;) {
   // Look for something that changed.
    std::list<Watch_Item *>::iterator wli;
    std::list<Watch_Item *> wli_copy;


#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: SS_Watcher, Look for something that changed, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
    printf("[TID=%ld], LOCK: SS_Watcher, Look for something that changed, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    wli_copy = Watch_Item_list;

#if DEBUG_SYNC
    printf("[TID=%ld], UNLOCK: SS_Watcher, Look for something that changed, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif
    Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
#if DEBUG_SYNC
    printf("[TID=%ld], UNLOCK: SS_Watcher, Look for something that changed, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    for ( wli = wli_copy.begin(); wli != wli_copy.end(); ) {
      bool purge_item = false;
      Watch_Item *wi = *wli;
      wli++;
#if DEBUG_SYNC
      printf("[TID=%ld], SS_Watcher, In wli = wli_copy.begin(); wli != wli_copy.end(); loop, wi->itemtype=%d\n", pthread_self(),  wi->itemtype);
#endif
      switch (wi->itemtype) {
        case Watch_Item::printTermination:
        case Watch_Item::waitTermination:
          ExperimentObject *exp = (wi->item.p.expid != 0)
                 ? Find_Experiment_Object (wi->item.p.expid)
                 : NULL;
          if (exp == NULL) {
            purge_item = true;
          } else {
           // Get the current status of this experiment.
            exp->Q_Lock (&cmd, false);
            int expStatus = exp->Determine_Status();
#if DEBUG_SYNC_SIGNAL
            printf("[TID=%ld], SS_Watcher, expStatus=%ld\n", pthread_self(),  expStatus);
#endif
            exp->Q_UnLock ();

            if ((expStatus == ExpStatus_NonExistent) ||
                (expStatus == ExpStatus_Paused) ||
                (expStatus == ExpStatus_Terminated) ||
                (expStatus == ExpStatus_InError)) {
              if (wi->itemtype == Watch_Item::printTermination) {
               // Print a message to the window.
                Send_Message_To_Window ( wi->item.p.window, 
                                           std::string("Experiment ")
                                              +  int2str(wi->item.p.expid)
                                              +  " has terminated."
                                         );
              } else if (wi->itemtype == Watch_Item::waitTermination) {

#if DEBUG_SYNC_SIGNAL
               printf("[TID=%ld], PTHREAD_COND_SIGNAL: SS_Watcher, Watch_Item::waitTermination, before calling pthread_cond_signal(&(wi->item.w.condition=%ld) \n", pthread_self(),  wi->item.w.condition);
#endif

                Assert(pthread_cond_signal(&(wi->item.w.condition)) == 0);

#if DEBUG_SYNC_SIGNAL
               printf("[TID=%ld], PTHREAD_COND_SIGANL: SS_Watcher, Watch_Item::waitTermination, after calling pthread_cond_signal(&(wi->item.w.condition=%ld) \n", pthread_self(),  wi->item.w.condition);
#endif

              }
              purge_item = true;
            }

          }
          break;
      }
      if (purge_item) {
        wli_copy.remove(wi);
#if DEBUG_SYNC_SIGNAL
        printf("[TID=%ld], LOCK: SS_Watcher, purge_item, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
        Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);
#if DEBUG_SYNC_SIGNAL
        printf("[TID=%ld], LOCK: SS_Watcher, purge_item, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
        Watch_Item_list = wli_copy;
#if DEBUG_SYNC_SIGNAL
        printf("[TID=%ld], UNLOCK: SS_Watcher, purge_item, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
        Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
#if DEBUG_SYNC_SIGNAL
        printf("[TID=%ld], UNLOCK: SS_Watcher, purge_item, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
        delete wi;
      }
    } // end for

    if (wli_copy.begin() == wli_copy.end()) {

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], LOCK: SS_Watcher, Waiting_to_Watch, before calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
     // It might be a long time before we have something to look at.
     // Go into a long sleep.
      Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], LOCK: SS_Watcher, Waiting_to_Watch, after calling pthread_mutex_lock(&Watch_Item_Lock=%ld)\n", pthread_self(),  Watch_Item_Lock);
#endif
      Waiting_to_Watch = true;

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_WAIT: SS_Watcher, Waiting_to_Watch, before calling pthread_cond_wait(&Waiting_For_Items=%ld, &Watch_Item_Lock=%ld)\n", pthread_self(),  Waiting_For_Items, Watch_Item_Lock);
    printf("[TID=%ld], PTHREAD_COND_WAIT: SS_Watcher, Waiting_to_Watch, before calling pthread_cond_wait(), WaitCount=%ld \n", pthread_self(), WaitCount);
      WaitCount = WaitCount + 1;
#endif

      Assert(pthread_cond_wait(&Waiting_For_Items,&Watch_Item_Lock) == 0);

#if DEBUG_SYNC_SIGNAL
    printf("[TID=%ld], PTHREAD_COND_WAIT: SS_Watcher, Waiting_to_Watch, after calling pthread_cond_wait(&Waiting_For_Items=%ld, &Watch_Item_Lock=%ld)\n", pthread_self(),  Waiting_For_Items, Watch_Item_Lock);
    printf("[TID=%ld], PTHREAD_COND_WAIT: SS_Watcher, Waiting_to_Watch, after calling pthread_cond_wait(), WaitCount=%ld\n", pthread_self(), WaitCount);
#endif

#if DEBUG_SYNC
      printf("[TID=%ld], UNLOCK: SS_Watcher, Waiting_to_Watch, before calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

      Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if DEBUG_SYNC
      printf("[TID=%ld], UNLOCK: SS_Watcher, Waiting_to_Watch, after calling pthread_mutex_unlock(&Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

    } else {

#if DEBUG_SYNC
    printf("[TID=%ld], SS_Watcher, Sleep briefly and retry, Watch_Item_Lock=%ld)  \n", pthread_self(),  Watch_Item_Lock);
#endif

      // Assume it will be a short time before we have something to do.
      // Sleep briefly and retry.
//      Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);

#if 0
      usleep (500000); // wait a while and see if conditions change.
#else
      struct timespec wait;
      wait.tv_sec = 0;
      wait.tv_nsec = 500 * 1000 * 1000;
      while(nanosleep(&wait, &wait));
#endif

    }
  }

  // Process the performance information on the cli's watcher thread 
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::cliWatcherStart,
                                                SS_Timings::cliWatcherCount,
                                                SS_Timings::cliWatcherMax,
                                                SS_Timings::cliWatcherMin,
                                                SS_Timings::cliWatcherTotal,
                                                SS_Timings::cliWatcherEnd);
  }

}
