#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QWidgetStack;
class QLabel;
class QTextEdit;
class QCheckBox;
class QPushButton;
class QLineEdit;


#define PANEL_CLASS_NAME UserTimePanelWizard   // Change the value of the define
                                         // to the name of your new class.
//! UserTimePanelWizard Class
/*! UserTimePanelWizard Class is intended to be used as a starting point to create
    user defined panels.   There's a script: mknewpanel that takes this
    template panel and creates a panel for the user to work with.    (See:
    mknewpanel in this directory.  i.e. type: mknewpanel --help)

    $ mknewpanel
    usage: mknewpanel directory panelname "menu header" "menu label" "show immediate" "grouping"
    where:
      directory:  Is the path to the directory to put the new panel code.
      panelname:  Is the name of the new panel.
      menu header: Is the Menu named to be put on the menu bar.
      menu label: Is the menu label under the menu header.
      show immediate: Default is 0.  Setting this to 1 will display the panel upon initialization.
      grouping: Which named panel container should this menu item drop this panel by default.


    An exmple would be to cd to this UserTimePanelWizard directory and issue the
    following command:
    mknewpanel ../NewPanelName "NewPanelName" "New Panel Menu Heading" "New Panel Label" 0 "Performance"

    That command would create a new panel directory, with the necessary
    structure for the user to create a new panel.   The user's new panel would
    be in the NewPanelName directory.   The future panel would be called,
    "NewPanelName".   A toplevel menu heading will be created called "New 
    Panel Menu Heading".   An entry under that topleve menu would read "New
    Panel Label".    The panel would not be displayed upon initialization of
    the tool, but only upon menu selection.    The final argument hints to the 
    tool that this panel belongs to the group of other Performance related 
    panels.
*/
class UserTimePanelWizard  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! UserTimePanelWizard() - A default constructor the the Panel Class.
  UserTimePanelWizard();  // Default construct

  //! UserTimePanelWizard(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  UserTimePanelWizard(PanelContainer *pc, const char *n); // Active constructor

  //! ~UserTimePanelWizard() - The default destructor.
  ~UserTimePanelWizard();  // Active destructor

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


    QFrame* frame3;
    QWidgetStack* userTimeWizardStack;
    QWidget* WStackVPage0;
    QLabel* vWizardTitleLabel;
    QTextEdit* vWizardDescription;
    QCheckBox* vpage0HideWizardCheckBox;
    QPushButton* vpage0StartButton;
    QPushButton* vpage0NextButton;
    QWidget* WStackVPage1;
    QLabel* vWizardOptionsLabel;
    QFrame* line8;
    QLabel* rateHeaderLabel;
    QLabel* rateLabel;
    QLineEdit* vpage1SampleRateText;
    QPushButton* vpage1BackButton;
    QPushButton* vpage1ResetButton;
    QPushButton* vpage1NextButton;
    QWidget* WStackVPage2;
    QLabel* vFinishLabel;
    QPushButton* vpage2BackButton;
    QPushButton* vpage2FinishButton;
    QWidget* WStackEPage0;
    QLabel* eWizardTitleLabel;
    QTextEdit* eWizardDescription;
    QCheckBox* epage0HideWizardCheckBox;
    QPushButton* epage0StartButton;
    QPushButton* epage0NextButton;
    QWidget* WStackEPage1;
    QLabel* eWizardOptionsLabel;
    QFrame* line8_2;
    QLabel* rateHeaderLabel_2;
    QLabel* rateLabel_2;
    QLineEdit* epage1SampleRateText;
    QPushButton* epage1BackButton;
    QPushButton* epage1ResetButton;
    QPushButton* epage1NextButton;
    QWidget* WStackEPage2;
    QLabel* eFinishLabel_2;
    QPushButton* epage2BackButton;
    QPushButton* epage2FinishButton;
    QCheckBox* wizardMode;
    QLabel* broughtToYouByLabel;
 
    QHBoxLayout * frameLayout;
public slots:
    virtual void epage0HideWizardCheckBoxSelected();
    virtual void epage0StartButtonSelected();
    virtual void epage0NextButtonSelected();
    virtual void epage1BackButtonSelected();
    virtual void epage1NextButtonSelected();
    virtual void epage1ResetButtonSelected();
    virtual void epage1SampleRateTextSelected();
    virtual void epage2BackButtonSelected();
    virtual void epage2FinishButtonSelected();
    virtual void wizardModeSelected();
    virtual void vpage0HideWizardCheckBoxSelected();
    virtual void vpage0NextButtonSelected();
    virtual void vpage0StartButtonSelected();
    virtual void vpage1BackButtonSelected();
    virtual void vpage1NextButtonSelected();
    virtual void vpage1ResetButtonSelected();
    virtual void vpage2BackButtonSelected();
    virtual void vpage2FinishButtonSelected();
    virtual void vpage1SampleRateTextSelected();

protected:
    QVBoxLayout* layout16;
    QVBoxLayout* frame3Layout;
    QVBoxLayout* WStackVPage0Layout;
    QHBoxLayout* layout7;
    QSpacerItem* spacer4;
    QSpacerItem* spacer4_2;
    QHBoxLayout* layout17;
    QSpacerItem* spacer20_2;
    QVBoxLayout* WStackVPage1Layout;
    QSpacerItem* spacer7;
    QVBoxLayout* layout9;
    QHBoxLayout* layout1_2;
    QHBoxLayout* layout31;
    QVBoxLayout* WStackVPage2Layout;
    QVBoxLayout* layout25;
    QSpacerItem* spacer20;
    QHBoxLayout* layout7_2;
    QVBoxLayout* WStackEPage0Layout;
    QHBoxLayout* layout15;
    QSpacerItem* spacer9;
    QHBoxLayout* layout17_2;
    QSpacerItem* spacer20_2_2;
    QVBoxLayout* WStackEPage1Layout;
    QSpacerItem* spacer7_2;
    QVBoxLayout* layout9_2;
    QHBoxLayout* layout1_2_2;
    QHBoxLayout* layout31_2;
    QVBoxLayout* WStackEPage2Layout;
    QHBoxLayout* layout7_2_2;
    QHBoxLayout* layout15_2;
    QSpacerItem* spacer15;

protected slots:
    virtual void languageChange();

};
#endif // TEMPLATE_PANEL_H
