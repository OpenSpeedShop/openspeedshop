////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007, 2008 Krell Institute All Rights Reserved.
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


#ifndef LOADATTACHOBJECT_H
#define LOADATTACHOBJECT_H

#include <qvaluelist.h>
typedef QValueList<QString> ParamList;

#include "MessageObject.hxx"
#include "CompareInfo.hxx"

//! The message object for passing file/line/highlight changes to the SourcePanel
class LoadAttachObject : public MessageObject
{
public:
    //! Unused constructor.
    LoadAttachObject();
    //! Constructor for attaching to executable
    LoadAttachObject(QString executable_name = NULL, 
                     QString pid_string = NULL, 
                     QString parallelprefixstring = NULL, 
                     ParamList *param_list = NULL, 
                     bool loadNowHint = FALSE, 
                     bool doesThisExperimentUseOfflineInstrumentation = FALSE,
                     compareByType localCompareByType = compareByFunctionType );

    //! Destructor
    ~LoadAttachObject();

    //! Prints debug information about this object.
    void print();

    //! The executable to attach to...
    QString executableName;

    //! The pid to attach to..
    QString pidStr;

    //! The parallel prefix for a MPI or other multiprocessing job 
    QString parallelprefixstring;

    //! The list of parameters
//    QString paramList;
    ParamList *paramList;

    //! The left side experiment to compare
    QString leftSideExperiment;

    //! The right side experiment to compare
    QString rightSideExperiment;

    //! Load now!
    bool loadNowHint;

    //! Offline Instrumentation flag!
    bool doesThisExperimentUseOfflineInstrumentation;

    //! Compare by what criteria 
    compareByType compareByThisType;
};
#endif // LOADATTACHOBJECT_H
