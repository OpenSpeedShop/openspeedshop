#ifndef IO_PANEL_H
#define IO_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;

#define PANEL_CLASS_NAME IOPanel   // Change the value of the define
                                         // to the name of your new class.
//! IOPanel Class
class IOPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! IOPanel() - A default constructor the the IOPanel Class.
    IOPanel();

    //! IOPanel(PanelContainer *pc, const char *name)
    IOPanel(PanelContainer *pc, const char *n); // Active constructor

    //! ~IOPanel() - The default destructor.
    ~IOPanel();  // Active destructor

    //! Adds use panel menus (if any).
    bool menu(QPopupMenu* contextMenu);

    //! Calls the user panel function save() request.
    void save();

    //! Calls the user panel function saveas() request.
    void saveAs();

    //! Calls the user panel function listener() request.
    int listener(char *msg);

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
#endif // IO_PANEL_H
