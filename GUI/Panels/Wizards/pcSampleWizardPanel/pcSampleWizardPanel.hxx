#ifndef PCSAMPLEWIZARDPANEL_H
#define PCSAMPLEWIZARDPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QWidgetStack;
class QLabel;
class QTextEdit;
class QCheckBox;
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


// enum pcSamplingActionType { PCSAT_load, PCSAT_attach, PCSAT_view };

#define PANEL_CLASS_NAME pcSampleWizardPanel   // Change the value of the define
                                         // to the name of your new class.
//! pcSampleWizardPanel Class
class pcSampleWizardPanel  : public Panel
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! pcSampleWizardPanel() - A default constructor the the Panel Class.
  pcSampleWizardPanel();  // Default construct

  //! pcSampleWizardPanel(PanelContainer *pc, const char *name)
    /*! This constructor is the work constructor.   It is called to
        create the new Panel and attach it to a PanelContainer.
        \param pc is a pointer to PanelContainer
               the Panel will be initially attached.
        \param name is the name give to the Panel.

        This is where the user would create the panel specific Qt code
        to do whatever functionality the user wanted the panel to perform.
     */
  pcSampleWizardPanel(PanelContainer *pc, const char *n); // Active constructor

  //! ~pcSampleWizardPanel() - The default destructor.
  ~pcSampleWizardPanel();  // Active destructor

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


    QFrame* topFrame;
    QWidgetStack* pcSampleWizardPanelStack;
    QWidget* vDescriptionPageWidget;
    QLabel* vDescriptionPageTitleLabel;
    QTextEdit* vDescriptionPageText;
    QCheckBox* vHideWizardCheckBox;
    QPushButton* vDescriptionPageStartButton;
    QPushButton* vDescriptionPageNextButton;
    QWidget* vParameterPageWidget;
    QLabel* vParameterPageDescriptionLabel;
    QFrame* vParameterPageLine;
    QLabel* vParameterPageSampleRateHeaderLabel;
    QLabel* vParameterPageSampleRateLabel;
    QLineEdit* vParameterPageSempleRateText;
    QPushButton* vParameterPageBackButton;
    QPushButton* vParameterPageResetButton;
    QPushButton* vParameterPageNextButton;
    QWidget* vSummaryPageWidget;
    QLabel* vSummaryPageFinishLabel;
    QPushButton* vSummaryPageBackButton;
    QPushButton* vSummaryPageFinishButton;
    QWidget* eDescriptionPageWidget;
    QLabel* eDescriptionPageTitleLabel;
    QLabel* eDescriptionPageText;
    QCheckBox* eHideWizardCheckBox;
    QPushButton* eDescriptionPageStartButton;
    QPushButton* eDescriptionPageNextButton;
    QWidget* eParameterPageWidget;
    QLabel* eParameterPageDescriptionLabel;
    QFrame* eParameterPageLine;
    QLabel* eParameterPageSampleRateHeaderLabel;
    QLabel* eParameterPageSampleRateLabel;
    QLineEdit* epage1pcSampleRateText;
    QPushButton* eParameterPageBackButton;
    QPushButton* eParameterPageResetButton;
    QPushButton* eParameterPageNextButton;
    QWidget* eSummaryPageWidget;
    QLabel* eSummaryPageFinishLabel;
    QPushButton* eSummaryPageBackButton;
    QPushButton* eSummaryPageFinishButton;
    QCheckBox* wizardMode;
    QLabel* broughtToYouByLabel;

public slots:
    virtual void eHideWizardCheckBoxSelected();
    virtual void eDescriptionPageNextButtonSelected();
    virtual void eDescriptionPageStartButtonSelected();
    virtual void eParameterPageBackButtonSelected();
    virtual void eParameterPageNextButtonSelected();
    virtual void eParameterPageResetButtonSelected();
    virtual void eSummaryPageBackButtonSelected();
    virtual void eSummaryPageFinishButtonSelected();
    virtual void vHideWizardCheckBoxSelected();
    virtual void vDescriptionPageNextButtonSelected();
    virtual void vDescriptionPageStartButtonSelected();
    virtual void vParameterPageSempleRateTextReturnPressed();
    virtual void vParameterPageBackButtonSelected();
    virtual void vParameterPageNextButtonSelected();
    virtual void vParameterPageResetButtonSelected();
    virtual void vSummaryPageBackButtonSelected();
    virtual void vSummaryPageFinishButtonSelected();
    virtual void epage1pcSampleRateTextReturnPressed();
    virtual void wizardModeSelected();

protected:
    QVBoxLayout* topLayout;
    QVBoxLayout* topFrameLayout;
    QVBoxLayout* vDescriptionPageLayout;
    QHBoxLayout* vHideWizardLayout;
    QSpacerItem* vHideWizardSpacer;
    QSpacerItem* spacer4_2;
    QHBoxLayout* vDescriptionPageButtonLayout;
    QSpacerItem* vDescriptionPageButtonSpacer;
    QVBoxLayout* vParameterPageLayout;
    QSpacerItem* spacer7;
    QVBoxLayout* vParameterPageParameterLayout;
    QHBoxLayout* vParameterPageSampleRateLayout;
    QHBoxLayout* vParameterPageButtonLayout;
    QVBoxLayout* vSummaryPageLayout;
    QVBoxLayout* vSummaryPageLabelLayout;
    QSpacerItem* spacer20;
    QHBoxLayout* vSummaryPageButtonLayout;
    QVBoxLayout* eDescriptionPageLayout;
    QHBoxLayout* eHideWizardLayout;
    QSpacerItem* eHideWizardSpacer;
    QHBoxLayout* eDescriptionPageButtonLayout;
    QSpacerItem* spacer20_2_2;
    QVBoxLayout* eParameterPageLayout;
    QSpacerItem* spacer7_2;
    QVBoxLayout* eParameterPageParameterLayout;
    QHBoxLayout* eParameterPageSampleRateLayout;
    QHBoxLayout* eParameterPageButtonLayout;
    QVBoxLayout* eSummaryPageLayout;
    QHBoxLayout* eSummaryPageButtonLayout;
    QHBoxLayout* bottomLayout;
    QSpacerItem* bottomSpacer;

protected slots:
    virtual void languageChange();

};
#endif // PCSAMPLEWIZARDPANEL_H
