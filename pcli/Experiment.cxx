#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>               /* for fstat() */
#include <sys/mman.h>               /* for mmap() */
#include <time.h>
#include <stdio.h>
#include <list>
#include <inttypes.h>
#include <stdexcept>
#include <string>

// for host name description
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

#ifndef PTHREAD_MUTEX_RECURSIVE_NP
#define PTHREAD_MUTEX_RECURSIVE_NP 0
#endif

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;
#include "Commander.hxx"
#include "Clip.hxx"
#include "Experiment.hxx"

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
std::list<ExperimentObject *> ExperimentObject_list;


ExperimentObject *Find_Experiment_Object (EXPID ExperimentID)
{
// Search for existing entry.
  if (ExperimentID > 0) {
    std::list<ExperimentObject *>::iterator exp;
    for (exp = ExperimentObject_list.begin(); exp != ExperimentObject_list.end(); exp++) {
      // if (ExperimentID == ((*exp)->Exp_ID)) {
      if (ExperimentID == (*exp)->ExperimentObject_ID()) {
        return *exp;
      }
    }
  }

  return NULL;
}

// Low Level Semantic Routines

ResultObject Experiment_Create (CMDWID WindowID, EXPID ExperimentID,
                                ApplicationGroupObject *App, Collector *Inst)
{
  ExperimentObject *exp = NULL;
  if (ExperimentID <= 0) {
   // Use the current focused experiment.
    ExperimentID =  Experiment_Focus (WindowID);
  }
  if (ExperimentID > 0) {
    exp = Find_Experiment_Object (ExperimentID);
  }
  if (!exp) {
   // There is no existing experiment or something went wrong.  Allocate a new Experiment.
    exp = new ExperimentObject ();
   // When we allocate a new experiment, set the focus to point to it.
    (void)Experiment_Focus (WindowID, exp->ExperimentObject_ID());
  }
  if (exp == NULL) {
    return (ResultObject ( FAILURE, "ExperimentObject", NULL, "The requested experiment could not be found" ));
  }
  if (App != NULL) {
    exp->ExperimentObject_Add_Application (App);
  }
  if (Inst != NULL) {
    exp->ExperimentObject_Add_Instrumention (Inst);
  }
  return (ResultObject ( SUCCESS, "ExperimentObject", (void *)exp, ""));
}
