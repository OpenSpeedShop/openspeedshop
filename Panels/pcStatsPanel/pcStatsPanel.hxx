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


#ifndef PCSTATSPANEL_H
#define PCSTATSPANEL_H
#include "StatsPanelBase.hxx"           // Do not remove
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QVBoxLayout;

#include <qlistview.h>
#include <qinputdialog.h>

#include "CollectorInfo.hxx"   // For dummied up data...

#include "SPListViewItem.hxx"

#include "SS_Input_Manager.hxx"

#include <qvaluelist.h>
typedef QValueList<QString> ColumnList;

typedef std::pair<std::string, double> item_type;

#include <qsettings.h>

#define PANEL_CLASS_NAME pcStatsPanel   // Change the value of the define
//! pcStatsPanel Class
class pcStatsPanel  : public StatsPanelBase
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! pcStatsPanel(PanelContainer *pc, const char *name)
    pcStatsPanel(PanelContainer *pc, const char *n, void *argument);

    //! ~pcStatsPanel() - The default destructor.
    ~pcStatsPanel();  // Active destructor

    //! Calls the user panel function listener() request.
    int listener(void *msg);

    //! Routine to popup dynamic menu.
    bool createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos );

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

  public slots:
    void itemSelected( QListViewItem * );
    void gotoSource();

  private:
    void matchSelectedItem( std::string function_name );

    void updateStatsPanelBaseData(void *expr = 0, int expID = 0, QString experiment_name = NULL);

    SmartPtr<std::map<Function, double> > orig_data;
};
#endif // PCSTATSPANEL_H
