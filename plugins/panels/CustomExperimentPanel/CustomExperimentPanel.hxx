////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef CUSTOMEXPERIMENTPANEL_H
#define CUSTOMEXPERIMENTPANEL_H
#include "Panel.hxx"           // Do not remove

#include "ProcessControlObject.hxx"
#include "ControlObject.hxx"

#include "AnimatedQPushButton.hxx"
#include <qpushbutton.h>

#include <qlayout.h>
#include <qhbox.h>
#include <qtimer.h>
#include "GenericProgressDialog.hxx"
#include "ManageProcessesDialog.hxx"

#include "SS_Input_Manager.hxx"

class PanelContainer;   // Do not remove
class QLabel;
class QLineEdit;
class QButtonGroup;
class QHBoxLayout;

class OpenSpeedshop;

class ArgumentObject;
class LoadAttachObject;

#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME CustomExperimentPanel   // Change the value of the define
                                         // to the name of your new class.

#include "Experiment.hxx"
//! Creates the CustomExperimentPanel that controls the pcSampling experiment.
class CustomExperimentPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  CustomExperimentPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);
  CustomExperimentPanel(PanelContainer *pc, const char *n, ArgumentObject *ao, const char *collector_names);

  void init( PanelContainer *pc, const char *n, ArgumentObject *ao, const char *collector_names);

  //! ~CustomExperimentPanel() - The default destructor.
  ~CustomExperimentPanel();  // Active destructor

  //! Adds use panel menus (if any).
  /*! This calls the user 'menu()' function
      if the user provides one.   The user can attach any specific panel
      menus to the passed argument and they will be displayed on a right
      mouse down in the panel.
      /param  contextMenu is the QPopupMenu * that use menus can be attached.
  */
  virtual bool menu(QPopupMenu* contextMenu);

  //! Calls the user panel function save() request.
  virtual void save();

  //! Calls the user panel function saveas() request.
  virtual void saveAs();

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
  virtual int listener(void *msg);

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
  int last_status;
  QString original_cview_command;

  QString collector_names;
  QString wizardName;

  QVBoxLayout * frameLayout;
  ProcessControlObject *pco;

  bool abortPanelFLAG;
  bool runnableFLAG;
  bool aboutToRunFLAG;
  bool postProcessFLAG;
  bool readyToRunFLAG;
  bool staticDataFLAG;

  QString executableNameStr;
  QString argsStr;
  QString parallelPrefixCommandStr;
  QString pidStr;
  QTimer *timer;
  QTimer *loadTimer;

  OpenSpeedShop::Framework::Experiment *fw_experiment() { return experiment; }
  
  void outputCLIData(QString *data);
  void updatePanelStatusOnRerun(EXPID expID);
  void updatePanelStatusOnTermination(EXPID expID);
  void setAlreadyRun(bool alreadyRunFlag);
  bool wasAlreadyRun();


  virtual void hideWizard();

  int getExpID() { return expID; }
  int64_t leftSideExpID;   // Used by CompareExperimentsPanel ONLY
  int64_t rightSideExpID;   // Used by CompareExperimentsPanel ONLY
  static QString getMostImportantMetric(QString collector_name);

  void setInstrumentorIsOffline(bool flag) {
    instrumentorIsOffline = flag;
  }

  bool getInstrumentorIsOffline() {
    return instrumentorIsOffline;
  }


public slots:
  virtual void saveAsSelected();
  virtual void loadSourcePanel();
  virtual void editPanelName();
  virtual void saveExperiment();
  virtual void experimentStatus();
  virtual Panel *loadStatsPanel();
  virtual void loadManageProcessesPanel();
  virtual void progressUpdate();


private slots:
  virtual void statusUpdateTimerSlot();
  virtual void attachProcessSelected();
  virtual void loadProgramSelected();

protected slots:
  virtual void languageChange();

protected:

private:
  OpenSpeedshop *mw;
  virtual void wakeUpAndCheckExperimentStatus();

 
  int expID;  // Experiment ID of the expCreate, returned from the cli
  OpenSpeedShop::Framework::Experiment *experiment;

  virtual int processLAO(LoadAttachObject *);
  virtual void updateInitialStatus();

  QTimer *statusTimer;
  virtual void updateStatus();

  virtual void loadMain();
  GenericProgressDialog *pd;
  int steps;

  bool exitingFLAG;
  bool experimentRunAlreadyFLAG;

  QString expStatsInfoStr;

  virtual void resetRedirect();
  void getDatabaseName();
  void getDatabaseName(int exp_id);
  QString getDBName(int exp_id);

  bool instrumentorIsOffline;

};
#endif // CUSTOMEXPERIMENTPANEL_H
