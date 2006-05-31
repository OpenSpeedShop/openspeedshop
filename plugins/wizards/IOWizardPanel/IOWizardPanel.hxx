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


#ifndef PCSAMPLEWIZARDPANEL_H
#define PCSAMPLEWIZARDPANEL_H
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

#include <qvaluelist.h>
class CheckBoxInfoClass
{
  public:
    CheckBoxInfoClass() {};
    ~CheckBoxInfoClass() {};

    QCheckBox *checkbox;
};
typedef QValueList<CheckBoxInfoClass *> CheckBoxInfoClassList;


#include "ArgumentObject.hxx"


#define PANEL_CLASS_NAME IOWizardPanel   // Change the value of the define
                                         // to the name of your new class.
//! IOWizardPanel Class
class IOWizardPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! IOWizardPanel() - A default constructor the the Panel Class.
  IOWizardPanel();  // Default construct

  //! IOWizardPanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  IOWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);

  //! ~IOWizardPanel() - The default destructor.
  ~IOWizardPanel();  // Active destructor

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


  QVBoxLayout * ioFormLayout;
    QFrame* mainFrame;
    QWidgetStack* mainWidgetStack;
    QWidget* vDescriptionPageWidget;
    QLabel* vDescriptionPageTitleLabel;
    QTextEdit* vDescriptionPageText;
    QPushButton* vDescriptionPageIntroButton;
    QPushButton* vDescriptionPageNextButton;
    QPushButton* vDescriptionPageFinishButton;
    QWidget* vParameterPageWidget;
    QTextEdit* vParameterPageDescriptionText;
    QFrame* vParameterPageLine;
    QLabel* vParameterPageFunctionListHeaderLabel;
    QPushButton* vParameterPageBackButton;
    QPushButton* vParameterPageResetButton;
    QPushButton* vParameterPageNextButton;
    QPushButton* vParameterPageFinishButton;
    QWidget* vAttachOrLoadPageWidget;
    QTextEdit* vAttachOrLoadPageDescriptionLabel;
    QFrame* vAttachOrLoadPageLine;
    QCheckBox* vAttachOrLoadPageAttachToProcessCheckBox;
    QCheckBox* vAttachOrLoadPageLoadExecutableCheckBox;
    QCheckBox* vAttachOrLoadPageLoadDifferentExecutableCheckBox;
    QPushButton* vAttachOrLoadPageBackButton;
    QPushButton* vAttachOrLoadPageNextButton;
    QPushButton* vAttachOrLoadPageFinishButton;
    QPushButton *vAttachOrLoadPageClearButton;
    QWidget* vSummaryPageWidget;
    QTextEdit* vSummaryPageFinishLabel;
    QPushButton* vSummaryPageBackButton;
    QPushButton* vSummaryPageFinishButton;
    QWidget* eDescriptionPageWidget;
    QLabel* eDescriptionPageTitleLabel;
    QLabel* eDescriptionPageText;
    QPushButton* eDescriptionPageIntroButton;
    QPushButton* eDescriptionPageNextButton;
    QPushButton* eDescriptionPageFinishButton;
    QWidget* eParameterPageWidget;
    QTextEdit* eParameterPageDescriptionLabel;
    QFrame* eParameterPageLine;
    QCheckBox *vParameterTraceCheckBox;
    QCheckBox *eParameterTraceCheckBox;
    QLabel* eParameterPageFunctionListHeaderLabel;
    CheckBoxInfoClassList vCheckBoxInfoClassList;
    CheckBoxInfoClassList eCheckBoxInfoClassList;
    QPushButton* eParameterPageBackButton;
    QPushButton* eParameterPageResetButton;
    QPushButton* eParameterPageNextButton;
    QPushButton* eParameterPageFinishButton;
    QWidget* eAttachOrLoadPageWidget;
    QLabel* eAttachOrLoadPageDescriptionLabel;
    QFrame* eAttachOrLoadPageLine;
    QCheckBox* eAttachOrLoadPageAttachToProcessCheckBox;
    QCheckBox* eAttachOrLoadPageLoadExecutableCheckBox;
    QCheckBox* eAttachOrLoadPageLoadDifferentExecutableCheckBox;
    QPushButton* eAttachOrLoadPageBackButton;
    QPushButton* eAttachOrLoadPageNextButton;
    QPushButton* eAttachOrLoadPageFinishButton;
    QPushButton *eAttachOrLoadPageClearButton;
    QWidget* eSummaryPageWidget;
    QTextEdit* eSummaryPageFinishLabel;
    QPushButton* eSummaryPageBackButton;
    QPushButton* eSummaryPageFinishButton;
    QCheckBox* vwizardMode;
    QCheckBox* ewizardMode;

    QScrollView *sv;
    QWidget *big_box_w;
    void handleSizeEvent(QResizeEvent *e);

    ParamList paramList;
    QString paramString;

public slots:
    virtual void eDescriptionPageNextButtonSelected();
    virtual void eDescriptionPageIntroButtonSelected();
    virtual void eParameterPageBackButtonSelected();
    virtual void eParameterPageNextButtonSelected();
    virtual void eParameterPageResetButtonSelected();
    virtual void eAttachOrLoadPageBackButtonSelected();
    virtual void eAttachOrLoadPageClearButtonSelected();
    virtual void eAttachOrLoadPageNextButtonSelected();
    virtual void eSummaryPageBackButtonSelected();
    virtual void eSummaryPageFinishButtonSelected();
    virtual void vDescriptionPageNextButtonSelected();
    virtual void vDescriptionPageIntroButtonSelected();
    virtual void vParameterPageSampleRateTextReturnPressed();
    virtual void vParameterPageBackButtonSelected();
    virtual void vParameterPageNextButtonSelected();
    virtual void vParameterPageResetButtonSelected();
    virtual void vAttachOrLoadPageBackButtonSelected();
    virtual void vAttachOrLoadPageClearButtonSelected();
    virtual void vAttachOrLoadPageNextButtonSelected();
    virtual void vSummaryPageBackButtonSelected();
    virtual void vSummaryPageFinishButtonSelected();
    virtual void eParameterPageSampleRateTextReturnPressed();
    virtual void ewizardModeSelected();
    virtual void vwizardModeSelected();
    virtual void wizardModeSelected();
    virtual void vAttachOrLoadPageAttachToProcessCheckBoxSelected();
    virtual void eAttachOrLoadPageAttachToProcessCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadExecutableCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected();
    virtual void eAttachOrLoadPageLoadExecutableCheckBoxSelected();
    virtual void eAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected();
    virtual void finishButtonSelected();
    virtual void vParameterPageCheckBoxSelected();
    virtual void eParameterPageCheckBoxSelected();

protected:
    QVBoxLayout* mainFrameLayout;
    QVBoxLayout* vDescriptionPageLayout;
    QHBoxLayout* vDescriptionPageButtonLayout;
    QSpacerItem* vDescriptionPageButtonSpacer;
    QVBoxLayout* vParameterPageLayout;
    QSpacerItem* vParameterPageButtonSpacer;
    QVBoxLayout* vAttachOrLoadPageLayout;
    QSpacerItem* vAttachOrLoadPageButtonSpacer;
    QSpacerItem* vParameterPageSpacer;
    QSpacerItem* vAttachOrLoadPageSpacer;
    QVBoxLayout* vParameterPageParameterLayout;
    QVBoxLayout* vParameterPageFunctionListLayout;
    QGridLayout* vParameterPageFunctionListGridLayout;
    QGridLayout* eParameterPageFunctionListGridLayout;
    QHBoxLayout* vParameterPageButtonLayout;
    QLabel *vAttachOrLoadPageProcessListLabel;
    QLabel *vAttachOrLoadPageExecutableLabel;
    QSpacerItem* eParameterPageSpacer;
    QVBoxLayout* vAttachOrLoadPageAttachOrLoadLayout;
    QHBoxLayout* vAttachOrLoadPageSampleRateLayout;
    QHBoxLayout* vAttachOrLoadPageButtonLayout;
    QVBoxLayout* vSummaryPageLayout;
    QVBoxLayout* vSummaryPageLabelLayout;
    QSpacerItem* vSummaryPageButtonSpacer;
    QHBoxLayout* vSummaryPageButtonLayout;
    QVBoxLayout* eDescriptionPageLayout;
    QHBoxLayout* eDescriptionPageButtonLayout;
    QSpacerItem* eDescriptionPageButtonSpacer;
    QSpacerItem* eDescriptionPageSpacer;
    QVBoxLayout* eParameterPageLayout;
    QVBoxLayout* eAttachOrLoadPageLayout;
    QSpacerItem* eAttachOrLoadPageSpacer;
    QVBoxLayout* eParameterPageParameterLayout;
    QHBoxLayout* eParameterPageFunctionListLayout;
    QHBoxLayout* eParameterPageButtonLayout;
    QSpacerItem* eParameterPageButtonSpacer;
    QLabel *eAttachOrLoadPageExecutableLabel;
    QLabel *eAttachOrLoadPageProcessListLabel;
    QVBoxLayout* eAttachOrLoadPageAttachOrLoadLayout;
    QHBoxLayout* eAttachOrLoadPageSampleRateLayout;
    QHBoxLayout* eAttachOrLoadPageButtonLayout;
    QSpacerItem* eAttachOrLoadPageButtonSpacer;
    QVBoxLayout* eSummaryPageLayout;
    QSpacerItem* eSummaryPageButtonSpacer;
    QHBoxLayout* eSummaryPageButtonLayout;
    QHBoxLayout* bottomLayout;
    QSpacerItem* bottomSpacer;

    void eUpdateAttachOrLoadPageWidget();
    void vUpdateAttachOrLoadPageWidget();


protected slots:
    virtual void languageChange();

private:
    QString sampleRate;

    Panel *ioPanel;

    void appendFunctionsToMonitor();
};
#endif // PCSAMPLEWIZARDPANEL_H
