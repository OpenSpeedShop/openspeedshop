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


#ifndef PANELCONTAINER_H
#define PANELCONTAINER_H

#include <debug.hxx>

#include "Panel.hxx"

#include "Frame.hxx"

#include <qvariant.h>
#include <qwidget.h>
#include <qhbox.h>
#include <TabWidget.hxx>
#include <TabBarWidget.hxx>
#include <TopWidget.hxx>
#include <qtabwidget.h>

#include <qbutton.h>

#include <qlayout.h>

#include <qmenubar.h>
#include <qlabel.h>   // needed by qmenubar.h to create the menu labels
#include <qsplitter.h>

#include <qvaluelist.h>

#include <qtimer.h>
#include <qpoint.h>

#include "WhatsThis.hxx"

class ArgumentObject;

// #include "openspeedshop.hxx"
class OpenSpeedshop;

#include <qbitmap.h>
#include <qiconset.h>

#define MENU_ACTIVE_HEIGHT 5

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QSplitter;
class QTabWidget;

class PluginInfo;
typedef QValueList<Panel *> PanelList;
typedef QValueList<PanelContainer *> PanelContainerList;
typedef QValueList<PluginInfo *> PluginRegistryList;

//! The PanelContainer manages Panels.
class PanelContainer : public QWidget
{
    //! Q_OBJECT is needed as there are slots defined for the Panel Class
    Q_OBJECT
  public:
    //! This routine returns the master PanelContainer.
    PanelContainer *getMasterPC() { return _masterPC; }

    //! This routine sets the master PanelContainer.
    void setMasterPC(PanelContainer *pc) { _masterPC = pc; }

    //! This routine returns the master PanelContainer list.
    PanelContainerList *getMasterPCList() { return _masterPanelContainerList; }

    //! This routine sets the master PanelContainer list.
    void setMasterPCList(PanelContainerList *mpcl)
                                    { _masterPanelContainerList = mpcl; }

    //! Flag to enable disable event processing.
    bool _eventsEnabled;
    bool _resizeEventsEnabled;

    QTimer *popupTimer;
    QTimer *sleepTimer;
    InfoEventFilter *whatsThisEventFilter;
    WhatsThis *whatsThis;
    bool whatsThisActive;
    QPoint last_pos;

    //! This is the registry of all known gui plugins.
    /*! At runtime this registry is created by doing dlopens on all the
        dynamic libraries. */
    PluginRegistryList *_pluginRegistryList;

    //! Used to launch a plugin's Panel entry.
    //    int dl_create_and_add_panel(char *panel_type);
    Panel *dl_create_and_add_panel(const char *panel_type, PanelContainer *targetPC=NULL, ArgumentObject *ao=NULL, const char *collector_names = NULL);

    //! The parent PanelContainer of this pa
    PanelContainer *parentPanelContainer;

    //! The parent QWidget that this PanelContainer is a child of.
    QWidget *parent;   // The parent that this constructor was called with...

    //! Default contructor for a PanelContainer.   
    PanelContainer( );

    //! The working constructor for PanelContainer.
    PanelContainer( QWidget* parent = 0, const char* name = 0,
                    PanelContainer *parentPanelContainer = 0,
                    PanelContainerList *pcl = 0, WFlags fl = 0);

    //! Destroys the object and frees any allocated resources
    ~PanelContainer();

    //! A flag to easily tell if this is a topLevel Outside PanelContainer.
    /*! This is true if the panel is a topLevel PanelContainer
        and if it's outside the MainWindow
        A outsidePC is parented to the desktop.
     */
    bool outsidePC;
    TopWidget *topWidget;
                  
    //! A flag to easily tell if this is a topLevel PanelContainer.
    /*! This is true if the panel is a topLevel PanelContainer.
        A topLevel indicates the it's inside a panel as a container
        by itself (with optional child panel containers (and panels)).
     */
    bool topLevel;

    LocalToolbar *ltb;

    QColor getTabColor(int);
                  
    //! The name of this PanelContainer given by the panel developer.
    QString external_name;

    //! A convienience routine to set the PanelContainer's internal  name.
    void setInternalName(const char *n);

    //! A convienience routine to set the PanelContainer's external name.
    void setExternalName(const char *n);

    //! A convinience routine to get the internal PanelContainer name.
    QString getInternalName() { return !internal_name.isEmpty() ? internal_name : QString("dead pc"); };
    //! A convinience routine to get the external PanelContainer name.
    QString getExternalName() { return !external_name.isEmpty() ? external_name : QString("dead pc"); };

    //! Called to save the PanelContainer tree to a file.
    void savePanelContainerTree(char *fn = NULL);

    //! marks the PanelContainer for deletion later.
    bool markedForDelete;

    //! Notify a specific PanelContainer (and all it's panels) of a message.
    int notifyPC(char *msg);

    //! Notify the nearest Panel who is listening and handles this message.
    Panel *notifyNearest(char *msg);

    //! Notify a group of panels of a message.
    int notifyGroup(char *msg, int groupID);

    //! Notify all decendants of a message.
    int notifyAllDecendants(char *msg, PanelContainer *targetPC);

  //! Internal routine that does the actual walking of the decendant's tree
  void _notifyAllDecendants(char *msg, PanelContainer *targetPC);

    //! Notify all Panels of a particular message.
    int notifyAll(char *msg);

    //! Find a PanelContainer location based on mouse x,y position.
    PanelContainer *findPanelContainerByMouseLocation();

    //! Find a PanelContainer by internal name.
    PanelContainer *findInternalNamedPanelContainer(char *panel_container_name);

    //! Find nearest Panel by name.
    Panel *findNamedPanel(PanelContainer *start_pc, char *panel_name);

    //! Return a list of all the panels matching the id.
    PanelList *getPanelListByID(int id);

    //! Find the PanelContainer by external name.
    PanelContainer *findPanelContainerWithNamedPanel(char *panel_container_name);

    //! Find the best (closest) PanelContainer that will accept a Panel drop.
    PanelContainer *findBestFitPanelContainer(PanelContainer *start_pc);

    //! The list of Panels in this PanelContainer.
    PanelList panelList;

    //! Add a Panel to a PanelContainer.
    Panel *addPanel(Panel *panel, PanelContainer *pc, char *tab_title);

    //! Raise the named panel.
    Panel *raiseNamedPanel(char *);

    //! Raise the panel.
    Panel *raisePanel(Panel *p);

    //! Percolate the panel to the top, even if it's buried.
    Panel *raiseToTop(Panel *p);

    //! This is the parent for drop site layout.
    /*! This goes into the PanelContainer's left side */
    QWidget *dropSiteLayoutParent; 


    //! This the layout for the Panel's QTabWidget (tabWidget)
    QVBoxLayout *dropSiteLayout;

    //! This is the tabWidget that holds all Panels.
    TabWidget *tabWidget;
    TabBarWidget *tabBarWidget;

    //! The routine that puts the icon to the tab.
    void augmentTab( QWidget *targetWidget,  Panel *p, QIconSet *iconset=NULL);

    //! Callback to notify to display the PanelContainer menu.
    void panelContainerContextMenuEvent( PanelContainer *targetPC,
         bool localMenu = FALSE );

    //! Are we in the middle of handling a PanelContainer menu flag.
    bool _doingMenuFLAG;

    //! Returns the current (raised) Panel.
    Panel *getRaisedPanel();

    //! Routine to tell if there are any Panels in the PaneContainer.
    bool areTherePanels();

    //! The splitter widget for this PanelContainer.
    QSplitter *splitter;

    //! The left/top side of the widget tree.
    Frame *leftFrame;

    //! The PanelContainer in the left/top side of this PanelContainer.
    /*! This is only set when the PanelContainer is split. */
    PanelContainer *leftPanelContainer;

    //! The right/bottom side of the widget tree.
    Frame *rightFrame;

    //! The PanelContainer in the right/bottom side of this PanelContainer.
    /*! This is only set when the PanelContainer is split. */
    PanelContainer *rightPanelContainer;

    //! This is called to handle resizing the PanelContainer.
    void handleSizeEvent(QResizeEvent *e=NULL);

#ifdef OLD_DRAG_AND_DROP
    void mouseReleaseEvent(QMouseEvent *e=NULL);
#endif // OLD_DRAG_AND_DROP

    //! Sets the main window (OpenSpeedshop class)
    void setMainWindow(OpenSpeedshop *oss) { _masterPC->mainWindow = oss; };

    //! Returns the main window (OpenSpeedshop class) 
    OpenSpeedshop *getMainWindow() { return _masterPC->mainWindow; };

    //! Moves give panel to targetPC.
    void movePanel( Panel *p, QWidget *currentPage, PanelContainer *targetPC );

    //! Hide the panel (remove from tabWidget)
    void hidePanel(Panel *p);

    //! This routine is called (for debugging) to dump the PanelContainer tree.
    void debugPanelContainerTree();

    //! This routine is called (for debugging) to dump the widget tree.
    void debugAllWidgets();

  public slots:
    //! This is a convienience routine that splits PanelContainers horizontally.
    void splitHorizontal(int leftSidePercent=-1);

    //! This is a convienience routine that splits PanelContainers vertically.
    void splitVertical(int leftSidePercent=-1);

    //! This routine drags the current (raised) Panel.
    void dragRaisedPanel();

    //! This routine removes a PanelContainer.
    void removeLastPanelContainer();

    //! This routine removes a PanelContainer.
    void setThenRemoveLastPanelContainer();

    //! This routine removes a PanelContainer.
    void removePanelContainer(PanelContainer *pc=NULL);

    //! This routine removes the current (raised) Panel.
    void removeRaisedPanel(PanelContainer *pc=NULL);

    //! This routine calls all the top level panel containers and deletes them.
    void closeAllExternalPanelContainers();

    //! This routine closes the empty PanelContainer and window.
    void closeWindow(PanelContainer *pc=NULL);

  AnimatedQPushButton *deleteButton;
  protected:

  private slots:
    //! This is the routine that does the actual split.
    void split(Orientation orientation, bool showRight=TRUE, int leftSidePercent=-1);

  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();
  
  private:
    //! A pointer to the top PanelContainer.
    /*! The master PanelContainer is the root of all panel containers.
        It is created first and deleted last.   All major actions such as 
        disabling events, delete or removing other PanelContainers are to 
        be done by the master.   */
    PanelContainer *_masterPC;

    //! A list of all PanelContainers
    PanelContainerList *_masterPanelContainerList;

    //! The number of panel containers that have been created.
    /*! This is used to generate the internal panel container name. */
    int _panel_container_count;

    //! A arbitrary derived PanelContainer name.
    /*! It's base on the string literal "pc: " and the panel_container_count
        which is incremented and decremented base on the current number 
        of PanelContainers.  

        NOTE: These names are not to be relied upon to stay the same
        throughout the execution of the application.  In fact, it would 
        be highly unlikely for this name to stay the same.   
     */
    char pc_name_count[128];

    //! Context Sensitive Menu's panel container.
    PanelContainer *_lastPC;

    //! A flag enabling menus for this PanelContainer.
    bool menuEnabled;

    //! Widget to be reparent on splitting.
    /*! When splitting PanelContainers this pointer is used to point to
        the Panels that need to be reparented to the new PanelContainer.
     */
    QWidget *widget_to_reparent;

    //! This is a handle to the sourcePC for reparenting.
    PanelContainer *sourcePC;

    //! A pointer back to the mainwindow and the initial launch information.
    OpenSpeedshop *mainWindow;

    //! Recovers any hidden (markedForDelete) PanelContainers.
    void recover(PanelContainer *pc);

    //! Convienece routine to reparent one PanelContainer's Panels into another.
    void reparentPCPanels(PanelContainer *tPC, PanelContainer *fPC);

    //! Moves all source PanelContainer's Panels to 'this' PanelContainer.
    void movePanelsToNewPanelContainer( PanelContainer *sourcePC);

    //! Moves current sourcePC PanelContainer's to 'this' PanelContainer.
    void moveCurrentPanelToNewPanelContainer( PanelContainer *sourcePC );

    //! Loops through all PanelContainers saving by level. (depth)
    void _saveOrderedPanelContainerTree(PanelContainer *pc, FILE *fd);

    //! Recursivey calls itself, saving away the PanelContainer tree.
    void __saveOrderedPanelContainerTree(PanelContainer *pc, FILE *fd, int level);

    //! The depth (level) of this PanelContainer in the PanelContainer tree.
    int _depth;

    //! Convienience routine to set the depth field in the PanelContainer Class.
    void setDepthPanelContainerTree(PanelContainer *pc);

    //! Loops trough the entire PanelContainer tree renaming the PanelContainers
    void renamePanelContainerTree(PanelContainer *pc);

    //! Called from renamePanelContainerTree(...) to rename all PanelConatiners
    void _renamePanelContainerTree(PanelContainer *pc, int i);

    //! This routine does the actual renaming of the PanelContainer name field.
    void renamePanelContainer();

    //! This writes pertinent information for each PanelContainer to the file.
    void savePanelContainer(int depth, FILE *fd);

    //! Used for debugging routines to print out PanelContainer tree.
    void traversePanelContainerTree();

    //! Called from traversePanelContainerTree() this calls itself to print tree
    void _traversePanelContainerTree(PanelContainer *pc);
 
    /*! Given a top level PanelContainer.   Remove it and all it's children. */
    void removeTopLevelPanelContainer(PanelContainer *topPCToRemove, bool recursive=FALSE);

    //! Removes all panels from a PanelContainer.
    void removePanels(PanelContainer *pc = NULL);   // Removes all panels

    //! Deletes the panels from existence.
    void deletePanel(Panel *p, PanelContainer *targetPC);

    //! Deletes the hidden panels from target panel container.
    void deleteHiddenPanels(PanelContainer *targetPC);

    //! This is the routine that calls the Panel->menu function.
    bool addPanelMenuItems(QPopupMenu* contextMenu);

    //! Returns a pointer to the left/top PanelContainer.
    PanelContainer *getLeftPanelContainer() { return leftPanelContainer; }

    //! Returns a pointer to the right/bottom PanelContainer.
    PanelContainer *getRightPanelContainer() { return rightPanelContainer; }

    //! Print PanelContainer specific information during debugging call.
    void printPanelContainer(int depth);

    //! Prints Panel specific information during debuggging call.
    void printPanels(char *indent_buffer);

    QPopupMenu *pcMenu;
    QPopupMenu *contextMenu;

    //! The name of this PanelContainer.
    QString internal_name;

    //! Find the nearest Panel that is interested in a message.
    Panel *findNearestInterestedPanel(PanelContainer *start_pc, char *msg, int *ret_val);

    //! Returns true if a Panel interested in the message in this PanelContainer
    Panel *wasThereAnInterestedPanel(PanelContainer *pc, char *msg, int *return_value );

    //! Find the first (nearest) PanelContainer that has no Panels
    PanelContainer *findFirstEmptyPanelContainer(PanelContainer *start_pc);

    //! Find the first (nearest) PanelContainer that can contain Panels
    PanelContainer *findFirstAvailablePanelContainer(PanelContainer *start_pc);

    //! The layout widget for this PanelContainer.
    QVBoxLayout* panelContainerFrameLayout;

};

extern PanelContainer *createPanelContainer( QWidget *, const char *, PanelContainer *, PanelContainerList *);
#endif // PANELCONTAINER_H
