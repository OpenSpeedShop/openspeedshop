#ifndef INTROWIZARDPANEL_H
#define INTROWIZARDPANEL_H
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
class QScrollView;


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


    QFrame* mainFrame;
    QWidgetStack* mainWidgetStack;

    QWidget* vWStackPage;
    QLabel* vWelcomeHeader;
    QLabel* vHelpfulLabel;
    QCheckBox* vpage1LoadExperimentCheckBox;
    QFrame* line3;
    QRadioButton* vpage1pcSampleRB;
    QRadioButton* vpage1UserTimeRB;
    QRadioButton* vpage1HardwareCounterRB;
    QRadioButton* vpage1FloatingPointRB;
    QRadioButton* vpage1InputOutputRB;
    QPushButton* vpage1NextButton;

    QWidget* eWStackPage;
    QLabel* eWelcomeHeader;
    QLabel* eHelpfulLabel;
    QCheckBox* epage1LoadExperimentCheckBox;
    QFrame* line2;
    QRadioButton* epage1pcSampleRB;
    QRadioButton* epage1UserTimeRB;
    QRadioButton* epage1HardwareCounterRB;
    QRadioButton* epage1FloatingPointRB;
    QRadioButton* epage1InputOutputRB;
    QPushButton* epage1NextButton;

    QCheckBox* wizardMode;
    QLabel* broughtToYouByLabel;

    void vSetStateChanged(QRadioButton *rb);
    void eSetStateChanged(QRadioButton *rb);

public slots:
    virtual void wizardModeSelected();
    virtual void epage1NextButtonSelected();
    virtual void vpage1NextButtonSelected();
    void vpage1pcSampleRBChanged();
    void vpage1UserTimeRBChanged();
    void vpage1HardwareCounterRBChanged();
    void vpage1FloatingPointRBChanged();
    void vpage1InputOutputRBChanged();

    void epage1pcSampleRBChanged();
    void epage1UserTimeRBChanged();
    void epage1HardwareCounterRBChanged();
    void epage1FloatingPointRBChanged();
    void epage1InputOutputRBChanged();

    void handleSizeEvent( QResizeEvent *e );
    QScrollView *sv;

protected:
    QVBoxLayout* IntroWizardFormLayout;
    QVBoxLayout* mainFrameLayout;
    QVBoxLayout* vWStackPageLayout;
    QVBoxLayout* vRBLayout;
    QHBoxLayout* vLoadExperimentLayout;
    QSpacerItem* spacer5_3;
    QHBoxLayout* vpcSampleRBLayout;
    QSpacerItem* spacer5;
    QHBoxLayout* vUserTimeRBLayout;
    QSpacerItem* spacer6;
    QHBoxLayout* vHWCounterRBLayout;
    QSpacerItem* spacer7;
    QHBoxLayout* vFloatingPointRBLayout;
    QSpacerItem* spacer7_3;
    QHBoxLayout* vInputOutputRBLayout;
    QSpacerItem* spacer7_4;
    QHBoxLayout* layout18;
    QSpacerItem* spacer7_5;
    QHBoxLayout* vNextButtonLayout;
    QSpacerItem* spacer44;
    QVBoxLayout* WStackPageLayout_2;
    QVBoxLayout* eRBLayout;
    QHBoxLayout* eLoadExperimentRBLayout;
    QSpacerItem* spacer5_2_2;
    QHBoxLayout* epcSampleRBLayout;
    QSpacerItem* spacer5_2;
    QHBoxLayout* eUserTimeRBLayout;
    QSpacerItem* spacer6_2;
    QHBoxLayout* eHWCounterRBLayout;
    QSpacerItem* spacer7_2;
    QHBoxLayout* eFloatingPointRBLayout;
    QSpacerItem* spacer7_3_2;
    QHBoxLayout* eInputOutputRBLayout;
    QSpacerItem* spacer7_4_2;
    QHBoxLayout* layout18_2;
    QSpacerItem* spacer7_5_2;
    QHBoxLayout* eNextButtonLayout;
    QSpacerItem* spacer37;
    QHBoxLayout* wizardModeLayout;
    QSpacerItem* spacer1;

protected slots:
    virtual void languageChange();

};
#endif // INTROWIZARDPANEL_H
