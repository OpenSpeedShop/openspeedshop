#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class QHBoxLayout;

#define PANEL_CLASS_NAME TemplatePanel   // Change the value of the define
                                         // to the name of your new class.
//! TemplatePanel Class
class TemplatePanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! TemplatePanel() - A default constructor the the TemplatePanel Class.
    TemplatePanel();

    //! TemplatePanel(PanelContainer *pc, const char *name)
    TemplatePanel(PanelContainer *pc, const char *n, char *argument);

    //! ~TemplatePanel() - The default destructor.
    ~TemplatePanel();  // Active destructor

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
#endif // TEMPLATE_PANEL_H
