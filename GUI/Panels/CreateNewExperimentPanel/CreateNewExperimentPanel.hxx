#ifndef CREATENEWEXPERIMENTPANEL_H
#define CREATENEWEXPERIMENTPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;

#define PANEL_CLASS_NAME CreateNewExperimentPanel   // Change the value of the define
                                         // to the name of your new class.
//! CreateNewExperimentPanel Class
class CreateNewExperimentPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! CreateNewExperimentPanel() - A default constructor the the CreateNewExperimentPanel Class.
    CreateNewExperimentPanel();

    //! CreateNewExperimentPanel(PanelContainer *pc, const char *name)
    CreateNewExperimentPanel(PanelContainer *pc, const char *n); // Active constructor

    //! ~CreateNewExperimentPanel() - The default destructor.
    ~CreateNewExperimentPanel();  // Active destructor

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

  public slots:

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();

  private:
};
#endif // CREATENEWEXPERIMENTPANEL_H
