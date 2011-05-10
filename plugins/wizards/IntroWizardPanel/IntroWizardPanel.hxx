////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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
class QVBox;
#include <qvaluelist.h>


//! This defines the highest level Wizard class.
class IntroWizardPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! IntroWizardPanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  IntroWizardPanel(PanelContainer *pc, const char *n, void *argument);

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
  int listener(void *msg);

  //! Calls the panel function broadcast() message request.
  int broadcast(char *msg);


    QFrame* mainFrame;
    QWidgetStack* mainWidgetStack;

    QLabel* vpage0WelcomeHeader;
    QLabel* epage0WelcomeHeader;
    QLabel* vpage1WelcomeHeader;
    QLabel* epage1WelcomeHeader;
//    QCheckBox* vpage1LoadExperimentCheckBox;
//    QCheckBox* vpage1CompareExperimentsCheckBox;
    QFrame* line3;
//  Page 0
    QLabel* vpage0HelpfulLabel;
    QLabel* vpage0SpacerItem1;
    QLabel* vpage0SpacerItem2;
    QLabel* vpage0SpacerItem3;
    QLabel* vpage0SpacerItem4;
    QLabel* vpage0SpacerItem5;
    QLabel* vHelpfulLabel;
    QLabel* epage0SpacerItem1;
    QLabel* epage0SpacerItem2;
    QLabel* epage0SpacerItem3;
    QLabel* epage0SpacerItem4;
    QLabel* epage0SpacerItem5;
    QLabel* eHelpfulLabel;
    QWidget* vWStackPage0;
    QWidget* eWStackPage0;
    QRadioButton* vpage0SavedExpDataRB;
    QRadioButton* vpage0SavedExpCompareDataRB;
    QRadioButton* vpage0CreateLoadExpDataRB;
    QRadioButton* vpage0CreateAttachExpDataRB;
    QRadioButton* epage0SavedExpDataRB;
    QRadioButton* epage0SavedExpCompareDataRB;
    QRadioButton* epage0CreateLoadExpDataRB;
    QRadioButton* epage0CreateAttachExpDataRB;
//  Page 1
    QLabel* vpage1SpacerItem1;
    QLabel* vpage1SpacerItem2;
    QLabel* vpage1SpacerItem3;
    QLabel* vpage1SpacerItem4;
    QLabel* vpage1SpacerItem4b;
    QLabel* vpage1SpacerItem5;
    QLabel* vpage1SpacerItem6;
    QLabel* vpage1SpacerItem7;
    QLabel* vpage1SpacerItem8;
    QLabel* vpage1HelpfulLabel;
    QWidget* vWStackPage1;
    QWidget* eWStackPage1;
    QRadioButton* vpage1pcSampleRB;
    QRadioButton* vpage1UserTimeRB;
    QRadioButton* vpage1HardwareCounterRB;
    QRadioButton* vpage1HardwareCounterSampRB;
    QRadioButton* vpage1FloatingPointRB;
    QRadioButton* vpage1InputOutputRB;
    QRadioButton* vpage1MPIRB;

    QPushButton* vpage1BackButton;
    QPushButton* vpage1NextButton;
    QPushButton* vpage1FinishButton;

    QPushButton* epage1BackButton;
    QPushButton* epage1NextButton;
    QPushButton* epage1FinishButton;

    QLabel* epage1SpacerItem1;
    QLabel* epage1SpacerItem2;
    QLabel* epage1SpacerItem3;
    QLabel* epage1SpacerItem4;
    QLabel* epage1SpacerItem4b;
    QLabel* epage1SpacerItem5;
    QLabel* epage1SpacerItem6;
    QLabel* epage1SpacerItem7;
    QLabel* epage1SpacerItem8;
    QLabel* epage0HelpfulLabel;
    QLabel* epage1HelpfulLabel;
    QCheckBox* epage1LoadExperimentCheckBox;
    QCheckBox* epage1CompareExperimentsCheckBox;
    QFrame* line2;
    QRadioButton* epage1pcSampleRB;
    QRadioButton* epage1UserTimeRB;
    QRadioButton* epage1HardwareCounterRB;
    QRadioButton* epage1HardwareCounterSampRB;
    QRadioButton* epage1FloatingPointRB;
    QRadioButton* epage1InputOutputRB;
    QRadioButton* epage1MPIRB;

    QPushButton* vpage0IntroButton;
    QPushButton* vpage0FinishButton;
    QPushButton* vpage0NextButton;
    QPushButton* epage0IntroButton;
    QPushButton* epage0FinishButton;
    QPushButton* epage0NextButton;


    QCheckBox* wizardMode;
    QCheckBox* vpage0wizardMode;
    QCheckBox* vpage1wizardMode;
    QCheckBox* epage0wizardMode;
    QCheckBox* epage1wizardMode;
    QLabel* broughtToYouByLabel;

    void vSetStateChanged(QRadioButton *rb);
    void eSetStateChanged(QRadioButton *rb);
    void vpage1SetStateChanged(QRadioButton *rb);
    void epage1SetStateChanged(QRadioButton *rb);

protected:
    QVBoxLayout* IntroWizardFormLayout;
    QVBoxLayout* mainFrameLayout;
    QVBoxLayout* vWStackPage0Layout;
    QVBoxLayout* vScrollViewLayout0;
    QVBoxLayout* vWStackPage1Layout;
    QVBoxLayout* vpage0InitialChoiceLayout;
    QVBoxLayout* vRBLayout1;
    QVBoxLayout* vLoadExperimentLayout;
    QHBoxLayout* vSavedExpRBLayout;
    QSpacerItem* spacer3a;
    QSpacerItem* spacer3b;
    QSpacerItem* spacer5a;
    QHBoxLayout* vSavedExpCompareRBLayout;
    QSpacerItem* spacer5b;
    QHBoxLayout* vLoadExpRBLayout;
    QSpacerItem* spacer5c;
    QHBoxLayout* vAttachExpRBLayout;
    QSpacerItem* spacer5d;
    QSpacerItem* spacer5;
    QHBoxLayout* vpcSampleRBLayout;
    QSpacerItem* spacer5_3;
    QHBoxLayout* vUserTimeRBLayout;
    QSpacerItem* spacer6;
    QHBoxLayout* vHWCounterRBLayout;
    QSpacerItem* spacer7;
    QHBoxLayout* vFloatingPointRBLayout;
    QSpacerItem* spacer7_3;
    QHBoxLayout* vInputOutputRBLayout;
    QHBoxLayout* vMPIRBLayout;
    QSpacerItem* spacer7_4;
    QHBoxLayout* layout18;
    QSpacerItem* spacer7_5;
    QHBoxLayout* vNextButtonPage0Layout;
    QHBoxLayout* eNextButtonPage0Layout;
    QHBoxLayout* vNextButtonPage1Layout;
    QHBoxLayout* eNextButtonPage1Layout;
    QSpacerItem* spacer44a;
    QSpacerItem* spacer44b;
    QSpacerItem* spacer44;
    QSpacerItem* spacer49a;
    QSpacerItem* spacer49b;
    QSpacerItem* spacer49c;
    QSpacerItem* spacer49d;
    QSpacerItem* spacer49e;
    QSpacerItem* spacer49f;
    QVBoxLayout* eWStackPage0Layout;
    QVBoxLayout* eWStackPage1Layout;
    QVBoxLayout* WStackPage1Layout_2;
    QVBoxLayout* eRBLayout0;
    QVBoxLayout* vRBLayout0;
    QVBoxLayout* eRBLayout1;
    QVBoxLayout* epage0InitialChoiceLayout;
    QSpacerItem* spacer5_2_2;
    QSpacerItem* spacer5_a;
    QSpacerItem* spacer5_b;
    QSpacerItem* spacer5_c;
    QSpacerItem* spacer5_d;
    QHBoxLayout* epcSampleRBLayout;
    QSpacerItem* spacer5_2;
    QHBoxLayout* eUserTimeRBLayout;
    QSpacerItem* spacer6_2;
    QHBoxLayout* eHWCounterRBLayout;
    QSpacerItem* spacer7_2;
    QHBoxLayout* eFloatingPointRBLayout;
    QSpacerItem* spacer7_3_2;
    QHBoxLayout* eInputOutputRBLayout;
    QHBoxLayout* eMPIRBLayout;
    QSpacerItem* spacer7_4_2;
    QHBoxLayout* layout18_2;
    QSpacerItem* spacer7_5_2;
    QHBoxLayout* eNextButtonLayout;
    QHBoxLayout* vNextButtonLayout;
    QSpacerItem* spacer37;
    QHBoxLayout* wizardModeLayout;
    QSpacerItem* spacer1;

    QSpacerItem* vNextButtonPage0ButtonSpacer;
    QSpacerItem* eNextButtonPage0ButtonSpacer;
    QSpacerItem* vNextButtonPage1ButtonSpacer;
    QSpacerItem* eNextButtonPage1ButtonSpacer;

    void languageChange();
    void languageChange2();

    QScrollView *vpage0sv;
    QVBox *vpage0big_box;
    QScrollView *epage0sv;
    QVBox *epage0big_box;
    QScrollView *vpage1sv;
    QVBox *vpage1big_box;
    QScrollView *epage1sv;
    QVBox *epage1big_box;
    void handleSizeEvent(QResizeEvent *e);


public slots:
    virtual void wizardModeSelected();
    virtual void vpage0wizardModeSelected();
    virtual void vpage1wizardModeSelected();
    virtual void epage0wizardModeSelected();
    virtual void epage1wizardModeSelected();
    virtual void vORepage0NextButtonSelected();
    virtual void vORepage1NextButtonSelected();
    virtual void vpage1BackButtonSelected();
    virtual void epage1BackButtonSelected();

    void printRaisedPanel();
    void vpage0SavedExpDataRBChanged();
    void vpage0SavedExpCompareDataRBChanged();
    void vpage0CreateLoadExpDataRBChanged();
    void vpage0CreateAttachExpDataRBChanged();
    void epage0SavedExpDataRBChanged();
    void epage0SavedExpCompareDataRBChanged();
    void epage0CreateLoadExpDataRBChanged();

    void vpage1pcSampleRBChanged();
    void vpage1UserTimeRBChanged();
    void vpage1HardwareCounterRBChanged();
    void vpage1HardwareCounterSampRBChanged();
    void vpage1FloatingPointRBChanged();
    void vpage1InputOutputRBChanged();
    void vpage1MPIRBChanged();
    void page1LoadExperimentCheckBoxChanged();
    void page1CompareExperimentsCheckBoxChanged();

    void epage1pcSampleRBChanged();
    void epage1UserTimeRBChanged();
    void epage1HardwareCounterRBChanged();
    void epage1HardwareCounterSampRBChanged();
    void epage1FloatingPointRBChanged();
    void epage1InputOutputRBChanged();
    void epage1MPIRBChanged();
};
#endif // INTROWIZARDPANEL_H
