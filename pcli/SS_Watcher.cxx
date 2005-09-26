/******************************************************************************e
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
static std::list<Watch_Item *> Watch_Item_list;

class Watch_Item {
  friend void SS_Watch_Status ();

 public:
  enum itemCode { expTermination
                };

  itemCode itemtype;
  union {
    struct
      { EXPID expid;
        CMDWID window; };
  } item;

  Watch_Item () {};

 public:
  Watch_Item (CMDWID Watch_Window, ExperimentObject *exp) {
    item.window = Watch_Window;
    item.expid = exp->ExperimentObject_ID();
    itemtype = expTermination;
  }
  ~Watch_Item () { }
  
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
      if (id == wi->item.expid) {
        Watch_Item_list.remove(wi);
      }
    }

    Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
  }
}

void Request_Async_Notice_Of_Termination (CommandObject *cmd, ExperimentObject *exp) {
  if (!Watcher_Active) return;
  InputLineObject *clip = cmd->Clip();
  CMDWID forwindow = (clip != NULL) ? clip->Who() : 0;
  if ((exp == NULL) ||
      (forwindow == 0)) return;

 // Get rid of any extra notices.
  Cancle_Async_Notice (exp);

 // Post a new notice.
  Watch_Item *item =  new Watch_Item (forwindow, exp);

  Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);
  Watch_Item_list.push_back(item);

  if (Waiting_to_Watch) {
    Waiting_to_Watch = false;
    Assert(pthread_cond_signal(&Waiting_For_Items) == 0);
  }
  Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
}

// Catch SIGUSR1 signal and exit, when sent from main control.
static void
catch_TLI_signal (int sig, int error_num)
{
 // This isn't graceful, but it gets the job done.
  pthread_exit(0);
}

// This routine continuously checks the status of experiments
// and will notify the user if an application terminates.
void SS_Watcher () {
  Watcher_Active = true;

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
    Assert(pthread_mutex_lock(&Watch_Item_Lock) == 0);

    for ( wli = Watch_Item_list.begin(); wli != Watch_Item_list.end(); ) {
      bool purge_item = false;
      Watch_Item *wi = *wli;
      wli++;
      switch (wi->itemtype) {
        case Watch_Item::expTermination:
          ExperimentObject *exp = (wi->item.expid != 0)
                 ? Find_Experiment_Object (wi->item.expid)
                 : NULL;
          if (exp == NULL) {
            purge_item = true;
          } else {
           // Get the current status of this experiment.
            exp->Q_Lock (&cmd, false);
            int expStatus = exp->Determine_Status();
            exp->Q_UnLock ();

            if ((expStatus == ExpStatus_Terminated) ||
                (expStatus == ExpStatus_InError)) {
             // Print a message to the window.
              Send_Message_To_Window ( wi->item.window, 
                                         std::string("Experiment ")
                                            +  int2str(wi->item.expid)
                                            +  " has terminated."
                                       );
              purge_item = true;
            }

          }
          break;
      }
      if (purge_item) {
        Watch_Item_list.remove(wi);
        delete wi;
      }
    }

    if (Watch_Item_list.begin() == Watch_Item_list.end()) {
     // It might be a long time before we have something to look at.
     // Go into a long sleep.
      Waiting_to_Watch = true;
      Assert(pthread_cond_wait(&Waiting_For_Items,&Watch_Item_Lock) == 0);
      Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
    } else {
     // Assume it will be a short time before we have something to do.
     // Sleep briefly and retry.
      Assert(pthread_mutex_unlock(&Watch_Item_Lock) == 0);
      usleep (500000); // wait a while and see if conditions change.
    }
  }

}
