#ifndef USERTIMEPANEL_H
#define USERTIMEPANEL_H
#include "Panel.hxx"           // Do not remove

#include "ProcessControlObject.hxx"
#include "ControlObject.hxx"

#include "AnimatedQPushButton.hxx"
#include <qpushbutton.h>

#include <qlayout.h>
#include <qhbox.h>

class PanelContainer;   // Do not remove
class QLabel;
class QLineEdit;
class QButtonGroup;
class QHBoxLayout;

class OpenSpeedshop;

#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME UserTimePanel   // Change the value of the define
                                         // to the name of your new class.

//! Creates the UserTimePanel that controls the pcSampling experiment.
class UserTimePanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! UserTimePanel() - A default constructor the the Panel Class.
  UserTimePanel();  // Default construct

  //! UserTimePanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  UserTimePanel(PanelContainer *pc, const char *n); // Active constructor

  //! ~UserTimePanel() - The default destructor.
  ~UserTimePanel();  // Active destructor

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
  int listener(void *msg);

  QButtonGroup *buttonGroup;
  AnimatedQPushButton *attachCollectorButton;
  AnimatedQPushButton *detachCollectorButton;
  AnimatedQPushButton *attachProcessButton;
  AnimatedQPushButton *detachProcessButton;
  AnimatedQPushButton *runButton;
  AnimatedQPushButton *pauseButton;
  AnimatedQPushButton *continueButton;
  AnimatedQPushButton *updateButton;
  AnimatedQPushButton *interruptButton;
  AnimatedQPushButton *terminateButton;

  QHBoxLayout *statusLayout;
  QLabel *statusLabel; 
  QLineEdit *statusLabelText;

  QVBoxLayout * frameLayout;
  ProcessControlObject *pco;

  bool runnableFLAG;
public slots:
  void saveAsSelected();
  void loadNewProgramSelected();
  bool detachFromProgramSelected();
  void attachToExecutableSelected();
  void manageCollectorsSelected();
  void manageProcessesSelected();
  void manageDataSetsSelected();
  void loadSourcePanel();

protected slots:
  virtual void languageChange();

protected:

private:
  OpenSpeedshop *mw;

  void updateInitialStatus();
};
#endif // USERTIMEPANEL_H
