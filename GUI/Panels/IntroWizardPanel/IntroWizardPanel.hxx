#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#define PANEL_CLASS_NAME IntroWizardPanel   // Change the value of the define
                                         // to the name of your new class.

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QWidgetStack;
class QLabel;
class QRadioButton;
class QPushButton;
class QCheckBox;


//! This defines the highest level Wizard class.
class IntroWizardPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! IntroWizardPanel() - A default constructor the the Panel Class.
  IntroWizardPanel();  // Default construct

  //! IntroWizardPanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  IntroWizardPanel(PanelContainer *pc, const char *n); // Active constructor

  //! ~IntroWizardPanel() - The default destructor.
  ~IntroWizardPanel();  // Active destructor

  //! Adds use panel menus (if any).
  /*! This calls the user 'menu()' function
      if the user provides one.   The user can attach any specific panel
      menus to the passed argument and they will be displayed on a right
      mouse down in the panel.
      /param  contextMenu is the QPopupMenu * that use menus can be attached.
  */
  bool menu(QPopupMenu* contextMenu);

  //! Calls the user panel function save() request.
  void save();

  //! Calls the user panel function saveas() request.
  void saveAs();

  //! Calls the user panel function listener() request.
  /*! When a message
      has been sent (from anyone) and the message broker is notifying
      panels that they may want to know about the message, this is the
      function the broker notifies.   The listener then needs to determine
      if it wants to handle the message.
      \param msg is the incoming message.
      \return 0 means you didn't do anything with the message.
      \return 1 means you handled the message.
   */
  int listener(char *msg);

  //! Calls the panel function broadcast() message request.
  int broadcast(char *msg);


    QFrame* frame5;
    QWidgetStack* widgetStack5;
    QWidget* WStackPage;
    QLabel* epage1Header_2;
    QLabel* vpage1Label;
    QCheckBox* vpage1LoadExperimentCheckBox;
    QFrame* line3;
    QRadioButton* vpage1pcSampleRB;
    QRadioButton* vpage1UserTimeRB;
    QRadioButton* vpage1HardwareCounterRB;
    QRadioButton* vpage1FloatingPointRB;
    QRadioButton* vpage1InputOutputRB;
    QRadioButton* vpage1MpiRB;
    QPushButton* vpage1NextButton;
    QWidget* WStackPage_2;
    QLabel* epage1Header;
    QLabel* epage1Label;
    QCheckBox* epage1LoadExperimentCheckBox;
    QFrame* line2;
    QRadioButton* epage1pcSampleRB;
    QRadioButton* epage1UserTimeRB;
    QRadioButton* epage1HardwareCounterRB;
    QRadioButton* epage1FloatingPointRB;
    QRadioButton* epage1InputOutputRB;
    QRadioButton* epage1MpiRB;
    QPushButton* epage1NextButton;
    QCheckBox* wizardMode;
    QLabel* broughtToYouByLabel;

public slots:
    virtual void wizardModeSelected();
    virtual void epage1NextButtonSelected();
    virtual void vpage1NextButtonSelected();

protected:
    QVBoxLayout* IntroWizardFormLayout;
    QVBoxLayout* frame5Layout;
    QVBoxLayout* WStackPageLayout;
    QVBoxLayout* layout21;
    QHBoxLayout* layout20;
    QSpacerItem* spacer5_3;
    QHBoxLayout* layout9;
    QSpacerItem* spacer5;
    QHBoxLayout* layout10;
    QSpacerItem* spacer6;
    QHBoxLayout* layout21_2;
    QSpacerItem* spacer7;
    QHBoxLayout* layout20_2;
    QSpacerItem* spacer7_3;
    QHBoxLayout* layout19;
    QSpacerItem* spacer7_4;
    QHBoxLayout* layout18;
    QSpacerItem* spacer7_5;
    QHBoxLayout* layout52;
    QSpacerItem* spacer44;
    QVBoxLayout* WStackPageLayout_2;
    QVBoxLayout* layout20_3;
    QHBoxLayout* layout19_2;
    QSpacerItem* spacer5_2_2;
    QHBoxLayout* layout9_2;
    QSpacerItem* spacer5_2;
    QHBoxLayout* layout10_2;
    QSpacerItem* spacer6_2;
    QHBoxLayout* layout21_2_2;
    QSpacerItem* spacer7_2;
    QHBoxLayout* layout20_2_2;
    QSpacerItem* spacer7_3_2;
    QHBoxLayout* layout19_2_2;
    QSpacerItem* spacer7_4_2;
    QHBoxLayout* layout18_2;
    QSpacerItem* spacer7_5_2;
    QHBoxLayout* layout44;
    QSpacerItem* spacer37;
    QHBoxLayout* layout5;
    QSpacerItem* spacer1;

protected slots:
    virtual void languageChange();

};
#endif // TEMPLATE_PANEL_H
