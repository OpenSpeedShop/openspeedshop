#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
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
    DebugPanel(PanelContainer *pc, const char *n); // Active constructor

    //! ~DebugPanel() - The default destructor.
    ~DebugPanel();  // Active destructor

    PanelContainer *panelContainer;

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
          QCheckBox *debugSaveAsCheckBox;
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
#endif // TEMPLATE_PANEL_H
