#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;

#define PANEL_CLASS_NAME HW_CounterPanel   // Change the value of the define
                                         // to the name of your new class.
//! HW_CounterPanel Class
class HW_CounterPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! HW_CounterPanel() - A default constructor the the HW_CounterPanel Class.
    HW_CounterPanel();

    //! HW_CounterPanel(PanelContainer *pc, const char *name)
    HW_CounterPanel(PanelContainer *pc, const char *n); // Active constructor

    //! ~HW_CounterPanel() - The default destructor.
    ~HW_CounterPanel();  // Active destructor

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
#endif // TEMPLATE_PANEL_H
