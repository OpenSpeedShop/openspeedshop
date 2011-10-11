////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
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



#ifndef CUSTOMIZESTATSPANEL_H
#define CUSTOMIZESTATSPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;
class CustomizeClass;
class QHBoxLayout;
class ArgumentObject;
class QAction;

#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME CustomizeStatsPanel   // Change the value of the define
                                         // to the name of your new class.
//! CustomizeStatsPanel Class
class CustomizeStatsPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! CustomizeStatsPanel(PanelContainer *pc, const char *name)
    CustomizeStatsPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);

    //! ~CustomizeStatsPanel() - The default destructor.
    ~CustomizeStatsPanel();  // Active destructor

    //! Adds use panel menus (if any).
    bool menu(QPopupMenu* contextMenu);

    //! Calls the user panel function save() request.
    void save();

    //! Calls the user panel function saveas() request.
    void saveAs();

    //! Calls the user panel function listener() request.
    int listener(void *msg);

    //! Calls the panel function broadcast() message request.
    int broadcast(char *msg);

    //! Controls the base layout of the Panel's widgets.
    QHBoxLayout * frameLayout;

    CustomizeClass *mcc;
    CustomizeClass *mcc1;

    void preferencesChanged();

    int expID;

  public slots:

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

  private:
};
#endif // CUSTOMIZESTATSPANEL_H
