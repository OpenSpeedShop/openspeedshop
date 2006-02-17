////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#ifndef COMPAREWIZARDPANEL_H
#define COMPAREWIZARDPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#include <qvariant.h>
#include <qwidget.h>

class QLayout;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QWidgetStack;
class QLabel;
class QTextEdit;
class QCheckBox;
class QGridLayout;
class QFrame;
class QRadioButton;
class QPushButton;
class QLineEdit;
class QListView;
class QListViewItem;
class QListBox;
class QListBoxItem;
class QButtonGroup;
class QTextEdit;
class QScrollView;
class QComboBox;

#include "ArgumentObject.hxx"


#define PANEL_CLASS_NAME CompareWizardPanel   // Change the value of the define
                                         // to the name of your new class.
//! CompareWizardPanel Class
class CompareWizardPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! CompareWizardPanel() - A default constructor the the Panel Class.
  CompareWizardPanel();  // Default construct

  //! CompareWizardPanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  CompareWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);

  //! ~CompareWizardPanel() - The default destructor.
  ~CompareWizardPanel();  // Active destructor

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


  QVBoxLayout * mpiFormLayout;
    QFrame* mainFrame;
    QWidgetStack* mainWidgetStack;
    QWidget* vDescriptionPageWidget;
    QLabel* vDescriptionPageTitleLabel;
    QTextEdit* vDescriptionPageText;
    QPushButton* vDescriptionPageIntroButton;
    QPushButton* vDescriptionPageNextButton;
    QPushButton* vDescriptionPageFinishButton;
    QWidget* vModePageWidget;
    QTextEdit* vModePageDescriptionText;
    QFrame* vModePageLine;
    QLabel* vModeHeaderLabel;
    QPushButton* vModePageBackButton;
    QPushButton* vModePageNextButton;
    QPushButton* vParameterPageFinishButton;
    QWidget* vAttachOrLoadPageWidget;
    QTextEdit* vLoad2ExecutablesPageDescriptionLabel;
    QFrame* vAttachOrLoadPageLine;
    QCheckBox* vAttachOrLoadPageAttachToProcessCheckBox;
    QPushButton* vAttachOrLoadPageBackButton;
    QPushButton* vAttachOrLoadPageNextButton;
    QPushButton* vAttachOrLoadPageFinishButton;
    QPushButton *vAttachOrLoadPageClearButton;
    QWidget* vSummaryPageWidget;
    QTextEdit* vSummaryPageFinishLabel;
    QPushButton* vSummaryPageBackButton;
    QPushButton* vSummaryPageFinishButton;
    QCheckBox* vwizardMode;
    QCheckBox* ewizardMode;
    QComboBox *leftSideExperimentComboBox;
    QComboBox *rightSideExperimentComboBox;

    QCheckBox *vpage1LoadExperimentCheckBox;
    QCheckBox *vpage1Load2ExperimentsCheckBox;

    QString fn;

    QString leftSideDirName;
    QString leftSideBaseName;
    QString rightSideDirName;
    QString rightSideBaseName;

public slots:
    virtual void vDescriptionPageNextButtonSelected();
    virtual void vDescriptionPageIntroButtonSelected();
    virtual void vModePageBackButtonSelected();
    virtual void vModePageNextButtonSelected();
    virtual void vAttachOrLoadPageBackButtonSelected();
    virtual void vAttachOrLoadPageNextButtonSelected();
    virtual void vSummaryPageBackButtonSelected();
    virtual void vSummaryPageFinishButtonSelected();
    virtual void ewizardModeSelected();
    virtual void vwizardModeSelected();
    virtual void wizardModeSelected();
    virtual void vpage1LoadExperimentCheckBoxSelected();
    virtual void vpage1Load2ExperimentsCheckBoxSelected();
    virtual void finishButtonSelected();

private slots:
    void leftSideExperimentDirButtonSelected();
    void rightSideExperimentDirButtonSelected();

protected:
    QVBoxLayout* mainFrameLayout;
    QVBoxLayout* vDescriptionPageLayout;
    QHBoxLayout* vDescriptionPageButtonLayout;
    QSpacerItem* vDescriptionPageButtonSpacer;
    QVBoxLayout* vModePageLayout;
    QSpacerItem* vParameterPageButtonSpacer;
    QVBoxLayout* vAttachOrLoadPageLayout;
    QSpacerItem* vAttachOrLoadPageButtonSpacer;
    QSpacerItem* vParameterPageSpacer;
    QSpacerItem* vAttachOrLoadPageSpacer;
    QVBoxLayout* vParameterPageParameterLayout;
    QVBoxLayout* vParameterPageFunctionListLayout;
    QHBoxLayout* vParameterPageButtonLayout;
    QLabel *vAttachOrLoadPageProcessListLabel;
    QLabel *vAttachOrLoadPageExecutableLabel;
    QHBoxLayout* vAttachOrLoadPageAttachOrLoadLayout;
    QHBoxLayout* vAttachOrLoadPageButtonLayout;
    QVBoxLayout* vSummaryPageLayout;
    QVBoxLayout* vSummaryPageLabelLayout;
    QSpacerItem* vSummaryPageButtonSpacer;
    QHBoxLayout* vSummaryPageButtonLayout;
    QHBoxLayout* bottomLayout;
    QSpacerItem* bottomSpacer;

    void vUpdateAttachOrLoadPageWidget();


protected slots:
    virtual void languageChange();

private:
    void warnOfnoSavedData();

};
#endif // COMPAREWIZARDPANEL_H
