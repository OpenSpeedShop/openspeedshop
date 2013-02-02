////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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
 

#ifndef SLOTINFO_H
#define SLOTINFO_H


#include <stddef.h>
#include <unistd.h>
#include <qobject.h>

class PluginInfo;

//! This creates the slot for the dynamic menus.
class SlotInfo : public QObject
{
  Q_OBJECT
public:
    //! SlotInfo() -  A default constructor.
    SlotInfo();

    //! SlotInfo(QObject *parent, const char *name, PluginInfo *pi)
    SlotInfo(QObject *parent, const char *name, PluginInfo *pi);

    //! The destructor for this object.
    ~SlotInfo();

    //! A pointer to the PluginInfo. Which is all the information about the plugin loaded when the plugin was initially opened.
    PluginInfo *pluginInfo;

public slots:
    //! The call to do the menu work.
    void dynamicMenuCallback();
signals:
};

#endif // SLOTINFO_H
