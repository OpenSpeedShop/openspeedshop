////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007, 2008 Krell Institute. All Rights Reserved.
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


#ifndef ARGUMENTOBJECT_H
#define ARGUMENTOBJECT_H

#include <qstring.h>
#include "LoadAttachObject.hxx"

class Panel;

//! The base message class that contains a simple QString type of the message.
class ArgumentObject
{
public:
    ArgumentObject();
    ArgumentObject(QString  msg_type, int);
    ArgumentObject(QString  msg_type, Panel *);
    ArgumentObject(QString  msg_type, QString);
    void init();
    ~ArgumentObject();

    void print();

    QString msgType;
    int int_data;
    Panel *panel_data;
    QString qstring_data;
    bool loadedFromSavedFile;

    LoadAttachObject *lao;
    bool isInstrumentorOffline;
};
#endif // ARGUMENTOBJECT_H
