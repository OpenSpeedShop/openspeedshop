////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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


#ifndef PTHREADSPanel_H_H
#define PTHREADSPanel_H_H
#include "CustomExperimentPanel.hxx"           // Do not remove
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

class ArgumentObject;


#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME PTHREADSPanel   // Change the value of the define
                                         // to the name of your new class.

class PTHREADSPanel  : public CustomExperimentPanel
{
  public: 
    PTHREADSPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);

    ~PTHREADSPanel();
};
#endif // PTHREADSPanel_H_H
