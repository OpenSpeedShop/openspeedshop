#ifndef CLIINTERFACE_H
#define CLIINTERFACE_H

#include "qobject.h"
#include "qevent.h"
#include "qtimer.h"

#include "SS_Input_Manager.hxx"

// This will need to be larger for a shipped product... something like 30-60
// seconds.    However, for our current testing purposes. 5 seconds seems
// like a number that doesn't irrirtate too much.
#define MAXTIME 5000

class CLIInterface : QObject
{
  //! Q_OBJECT is needed as there are slots defined
  Q_OBJECT
  public:
    CLIInterface(int _wid);

    ~CLIInterface();

    int wid;

    bool getIntListValueFromCLI(char *command, std::list<int64_t> *int_list, bool mark_value_for_delete=true, int mt=MAXTIME, bool warn_of_time=false );

    bool getIntValueFromCLI(char *command, int64_t *val, bool mark_value_for_delete = true , int maxTime = MAXTIME, bool warn_of_time = true  );

    bool runSynchronousCLI(char *command, int maxTime = MAXTIME, bool warn_of_time = true );

    Input_Line_Status checkStatus(InputLineObject *clip);

    void setInterrupt(bool val) { interrupt = val; };
    bool getInterrupt() { return interrupt; };
   
  private:
    bool interrupt;

    QTimer *timer;

    int maxTime;

    bool shouldWeContinue();

  public slots:
   void wakeupFromTimer();

};

#endif // CLIINTERFACE_H
