////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#ifndef PERFORMANCE_PANEL_H
#define PERFORMANCE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include <qwidget.h>
#include <qhbox.h>
#include <qlayout.h>

class PanelContainer;   // Do not remove

#define PANEL_CLASS_NAME PerformancePanel   // Change the value of the define
                                         // to the name of your new class.
#include "PanelContainer.hxx"

//! Simple example of a vertically split top level PanelContainer.
class PerformancePanel  : public Panel
{
public:
  //! Default construct
  PerformancePanel(); 
  //! Work construct
  PerformancePanel(PanelContainer *pc, const char *n); // Active constructor
  //! Desstructor
  ~PerformancePanel();

  //! The layout that controls resizing.
  QHBoxLayout * frameLayout; 

  //! The child widget of frameLayout... All widgets are placed here.
  QWidget *performancePanelContainerWidget;
protected:

private: 
  //! The list of child PanelContainer for this PanelContainer.
  PanelContainerList *lpcl;
};
#endif // PERFORMANCE_PANEL_H
