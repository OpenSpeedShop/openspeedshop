#ifndef FPE_TRACING_PANEL_H
#define FPE_TRACING_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;

#define PANEL_CLASS_NAME FPE_TracingPanel   // Change the value of the define
                                         // to the name of your new class.
//! FPE_TracingPanel Class
class FPE_TracingPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! FPE_TracingPanel() - A default constructor the the FPE_TracingPanel Class.
    FPE_TracingPanel();

    //! FPE_TracingPanel(PanelContainer *pc, const char *name)
    FPE_TracingPanel(PanelContainer *pc, const char *n); // Active constructor

    //! ~FPE_TracingPanel() - The default destructor.
    ~FPE_TracingPanel();  // Active destructor

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
#endif // FPE_TRACING_PANEL_H
