#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include "AnimatedQPushButton.hxx"
#include <qpushbutton.h>

#include <qlayout.h>
#include <qhbox.h>

class PanelContainer;   // Do not remove
class QLabel;
class QLineEdit;
class QButtonGroup;
class QHBoxLayout;

#define PANEL_CLASS_NAME pcSamplePanelControl   // Change the value of the define
                                         // to the name of your new class.
//! pcSamplePanelControl Class
/*! pcSamplePanelControl Class is intended to be used as a starting point to create
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


    An exmple would be to cd to this pcSamplePanelControl directory and issue the
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
class pcSamplePanelControl  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! pcSamplePanelControl() - A default constructor the the Panel Class.
  pcSamplePanelControl();  // Default construct

  //! pcSamplePanelControl(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  pcSamplePanelControl(PanelContainer *pc, const char *n); // Active constructor

  //! ~pcSamplePanelControl() - The default destructor.
  ~pcSamplePanelControl();  // Active destructor

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


  QButtonGroup *buttonGroup;
  AnimatedQPushButton *attachButton;
  AnimatedQPushButton *detachButton;
  QPushButton *runButton;
  QPushButton *pauseButton;
  QPushButton *continueButton;
  QPushButton *updateButton;
  QPushButton *interruptButton;
  QPushButton *terminateButton;

  QHBoxLayout *statusLayout;
  QLabel *statusLabel; 
  QLineEdit *statusLabelText;

  QVBoxLayout * frameLayout;
#ifdef PULL
  void enterEvent(QEvent *);
  void leaveEvent(QEvent *);
#endif // PULL

public slots:
  virtual void attachButtonSlot();
  virtual void detachButtonSlot();
  virtual void runButtonSlot();
  virtual void pauseButtonSlot();
  virtual void continueButtonSlot();
  virtual void updateButtonSlot();
  virtual void interruptButtonSlot();
  virtual void terminateButtonSlot();
  void saveAsSelected();
  void openNewSelected();

protected slots:
  virtual void languageChange();

protected:

private:
};
#endif // TEMPLATE_PANEL_H
