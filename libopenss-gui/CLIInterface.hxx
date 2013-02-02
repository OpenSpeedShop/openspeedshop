////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute  All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef CLIINTERFACE_H
#define CLIINTERFACE_H

#include "stddef.h"
#include "qobject.h"
#include "qevent.h"
#include "qtimer.h"

#include "SS_Input_Manager.hxx"

#define MAXTIME 0

class CLIInterface : QObject
{
  //! Q_OBJECT is needed as there are slots defined
  Q_OBJECT
  public:
    //! Constructor for the command line inteface (cli) interface.
    CLIInterface(int _wid);

    //! Destructor for the command line inteface (cli) interface.
    ~CLIInterface();

    //! The window id (defined in the cli) for this connection.
    int wid;

    //! Return a list of string values from a given cli command.
    bool getStringListValueFromCLI(const char *command, std::list<std::string> *string_list, InputLineObject *clip, bool mark_value_for_delete=true, int mt=MAXTIME, bool warn_of_time=false );

    //! Return string value from a given cli command.
    bool getStringValueFromCLI(const char *command, std::string *val_string, bool mark_value_for_delete = true , int maxTime = MAXTIME, bool warn_of_time = true  );

    //! Return a list of int values from a given cli command.
    bool getIntListValueFromCLI(const char *command, std::list<int64_t> *int_list, InputLineObject *clip, bool mark_value_for_delete=true, int mt=MAXTIME, bool warn_of_time=false );

    //! Return an int value from a given cli command.
    bool getIntValueFromCLI(const char *command, int64_t *val, bool mark_value_for_delete = true , int maxTime = MAXTIME, bool warn_of_time = true  );


    //! Run a command and return a success or failure result.
    bool runSynchronousCLI(const char *command, int maxTime = MAXTIME, bool warn_of_time = false );

    //! Run a simple command in the cli.  The caller must manage all cli returns
    InputLineObject *run_Append_Input_String( int wid, const char *command );

    //! The flag set when a command has been interrupted.
    static bool interrupt;

    //! A routine to check status and timer before continuing.
    bool shouldWeContinue();

    //! A routine to check status of the command.
    Input_Line_Status checkStatus(InputLineObject *clip, const char *command="");
  private:
    //! The timer set to how long the command should wait before aborting.
    QTimer *timer;

    //! The time value for the timer.
    int maxTime;

    //! The flag to tell if we need to warn before returning.
    bool warn_of_time;


  public slots:
    void wakeupFromTimer();

};

#endif // CLIINTERFACE_H
