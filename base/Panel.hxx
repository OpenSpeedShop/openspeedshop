#ifndef PANEL_H
#define PANEL_H

#include "Frame.hxx"

#include <qtabwidget.h>

#include <qmenubar.h>
#include <qlabel.h> 

class InfoEventFilter;

class PanelContainer;
#define MAX_TAB_NAME_LENGTH 30

enum BROADCAST_TYPE { PC_T, NEAREST_T, GROUP_T, ALL_DECENDANTS_T, ALL_T };

//! This defines the base class for all Panels.
class Panel : public QWidget
{
   //! Q_OBJECT is needed as there are slots defined for the Panel Class
   Q_OBJECT
public:
    //! Panel() - A default constructor the the Panel Class.
    Panel();

    //! Panel(PanelContainer *pc, const char *name)
    Panel(PanelContainer *pc, const char *name);

    //! ~Panel() - The default destructor.
    ~Panel();

    //! Ugh.  This Panel Designates itself it's own top PanelContainer.
    bool topLevel;
    //! Ugh.  This Panel Designates itself it's own top PanelContainer.
    PanelContainer *topPC;

    //! A convience function to return the baseWidgetFrame.
    Frame *getBaseWidgetFrame() { return baseWidgetFrame; }

    //! A convience function to return the baseWidgetFrame.
    Frame *setBaseWidgetFrame(Frame *f) { baseWidgetFrame = f; }

    //! Adds use panel menus (if any).
    virtual bool menu(QPopupMenu* contextMenu);

    //! Calls the user panel function save() request.
    virtual void save();

    //! Calls the user panel function saveas() request.
    virtual void saveAs();

    //! Calls the user panel function listener() request.
    virtual int listener(void *msg);

    //! Calls the panel function broadcast() message request.
    virtual int broadcast(char *msg, BROADCAST_TYPE bt=ALL_T, PanelContainer *startPC = NULL);

    //! Handles the resize event.
    virtual void handleSizeEvent(QResizeEvent *e=NULL);

    //! Displays a popup like help with message... (msg)
    void displayWhatsThis(QString msg);

    //! Sets the Panel name.
    void setName(const char *);

    //! Returns the Panel name.
    const char *getName() { return name; }

    //! Returns the PanelContainer 
    PanelContainer *getPanelContainer() { return _panelContainer; }

    //! Sets the PanelContainer 
    void setPanelContainer(PanelContainer *pc) { _panelContainer = pc; }

    //! Calls the panel function info() if provided.
    virtual void info( QPoint pos, QObject *target=NULL );

    virtual void addWhatsThis(QObject *o, Panel *p);

protected:
    //! Sets the Panel specific strings.
    virtual void languageChange();

private: 
    //! A simple char array for holding the Panel name.
    /*! Note: This
        could (should?) be managed with new/delete, but it currently is
        not.
     */
    char name[MAX_TAB_NAME_LENGTH];

    //! The QWidget object that will hold all children in the user Panel.
    Frame *baseWidgetFrame;

    //! The PanelContainer that holds this Panel.
    PanelContainer *_panelContainer;

public slots:
    virtual void wakeupFromSleep();
    virtual void popupInfoAtLine();
};
#endif // PANEL_H
