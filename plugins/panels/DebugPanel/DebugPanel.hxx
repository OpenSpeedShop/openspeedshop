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


#ifndef DEBUG_PANEL_H
#define DEBUG_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;
class QVBoxLayout;

class QRadioButton;
class QSpacerItem;
class QCheckBox;
class QPushButton;
class QLabel;

#define PANEL_CLASS_NAME DebugPanel   // Change the value of the define
                                         // to the name of your new class.
//! A simple interface to toggle on/off debugging hooks.
class DebugPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! DebugPanel() - A default constructor the the DebugPanel Class.
    DebugPanel();

    //! DebugPanel(PanelContainer *pc, const char *name)
    DebugPanel(PanelContainer *pc, const char *n, void *argument);

    //! ~DebugPanel() - The default destructor.
    ~DebugPanel();  // Active destructor

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
    QLabel *label;
    QSpacerItem *spacerItem;

    QVBoxLayout * frameLayout;
      QPushButton *debugAllOnButton;
      QPushButton *debugAllOffButton;

      QHBoxLayout * topLayout;
        QVBoxLayout * guiLayout;
          QPushButton *debugGUIOnButton;
          QPushButton *debugGUIOffButton;
          QCheckBox *debugPanelsCheckBox;
          QCheckBox *debugFramesCheckBox;
          QCheckBox *debugPanelContainersCheckBox;
          QCheckBox *debugDNDCheckBox;
          QCheckBox *debugMessagesCheckBox;
          QCheckBox *debugPrintingCheckBox;
          QCheckBox *debugClipCheckBox;
          QCheckBox *debugCommandsCheckBox;
          QPushButton *debugPanelContainerTreeButton;
          QPushButton *debugAllWidgetsButton;

        QVBoxLayout * cliLayout;
          QPushButton *debugCLIOnButton;
          QPushButton *debugCLIOffButton;
        QVBoxLayout * frameWorkLayout;
          QPushButton *debugFrameWorkOnButton;
          QPushButton *debugFrameWorkOffButton;
        QVBoxLayout * backendLayout;
          QPushButton *debugBackendOnButton;
          QPushButton *debugBackendOffButton;


    QCheckBox *debugCLICheckBox;

    QCheckBox *debugFrameWorkCheckBox;

    QCheckBox *debugBackendCheckBox;

    QHBoxLayout * bottomLayout;
      QPushButton *applyButton;

  public slots:
    virtual void debugAllOnButtonSelected();
    virtual void debugAllOffButtonSelected();

    virtual void debugGUIOnButtonSelected();
    virtual void debugGUIOffButtonSelected();

    virtual void debugCLIOnButtonSelected();
    virtual void debugCLIOffButtonSelected();

    virtual void debugFrameWorkOnButtonSelected();
    virtual void debugFrameWorkOffButtonSelected();

    virtual void debugBackendOnButtonSelected();
    virtual void debugBackendOffButtonSelected();

    virtual void debugPanelContainerTreeButtonSelected();
    virtual void debugAllWidgetsButtonSelected();

    virtual void applyButtonSelected();

  protected:
    virtual void languageChange();

  private:
};
#endif // DEBUG_PANEL_H
