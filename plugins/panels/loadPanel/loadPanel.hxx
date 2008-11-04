////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006, 2007, 2008 Krell Institute All Rights Reserved.
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



#ifndef TEMPLATE_PANEL_H
#define TEMPLATE_PANEL_H
#include "Panel.hxx"           // Do not remove

#include "Panel.hxx"           // Do not remove
#include <qvariant.h>
#include <qwidget.h>
#include "ArgumentObject.hxx"

class PanelContainer;   // Do not remove
class QHBoxLayout;
class QVBoxLayout;
class QVBox;
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
class QScrollView;


#define PANEL_CLASS_NAME loadPanel   // Change the value of the define
                                         // to the name of your new class.
//! loadPanel Class
class loadPanel  : public Panel
{
    //! Q_OBJECT is needed as there are slots defined for the class
    Q_OBJECT
  public:
    //! loadPanel(PanelContainer *pc, const char *name)
    loadPanel(PanelContainer *pc, const char *n, ArgumentObject *ao);
//    loadPanel(PanelContainer *pc, const char *n, void *argument);

    //! ~loadPanel() - The default destructor.
    ~loadPanel();  // Active destructor

    //! Adds use panel menus (if any).
    bool menu(QPopupMenu* contextMenu);

    //! Calls the user panel function save() request.
    void save();

    //! Calls the user panel function saveas() request.
    void saveAs();

    //! Calls the user panel function listener() request.
    int listener(void *msg);

    //! Calls the panel function broadcast() message request.
    int broadcast(char *msg);

    void vUpdateAttachOrLoadPageWidget();
    void vMPLoadPageProcessAccept();

    Panel *targetPanel;

    //! Controls the base layout of the Panel's widgets.
    QVBoxLayout * frameLayout;
    QVBoxLayout * loadPanelFormLayout;
    QVBoxLayout * mainLayout;
    QFrame * mainFrame;
    QVBoxLayout * mainFrameLayout;
    QWidgetStack* mainWidgetStack;

    QWidget* vALStackPage0;
    QTextEdit* vAttachOrLoadPageDescriptionLabel;
    QCheckBox* vAttachOrLoadPageAttachToProcessCheckBox;
    QCheckBox* vAttachOrLoadPageAttachToMultiProcessCheckBox;
    QCheckBox* vAttachOrLoadPageLoadExecutableCheckBox;
    QCheckBox* vAttachOrLoadPageLoadMultiProcessExecutableCheckBox;
    QCheckBox* vAttachOrLoadPageLoadDifferentExecutableCheckBox;
    QCheckBox* vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox;
    QPushButton* vAttachOrLoadPageBackButton;
    QPushButton* vAttachOrLoadPageNextButton;
    QPushButton* vAttachOrLoadPageFinishButton;
    QPushButton *vAttachOrLoadPageClearButton;
    QVBoxLayout* vALStackPage0Layout;
    QSpacerItem* vAttachOrLoadPageButtonSpacer;
    QSpacerItem* vAttachOrLoadPageSpacer;
    QLabel *vAttachOrLoadPageProcessListLabel;
    QLabel *vAttachOrLoadPageMultiProcessListLabel;
    QLabel *vAttachOrLoadPageMultiProcessExecutableLabel;
    QLabel *vAttachOrLoadPageExecutableLabel;
    QVBoxLayout* vAttachOrLoadPageAttachOrLoadLayout;
    QHBoxLayout* vAttachOrLoadPageSampleRateLayout;
    QHBoxLayout* vAttachOrLoadPageButtonLayout;

    QFrame* vLAPage0Line1;
    QFrame* vLAPage0Line2;
    QFrame* vLAPage0SeqMPSeparatorLine;
    QLabel* vLAPageTitleLabel1;
    QLabel* vLAPageTitleLabel2;

    QWidget* vMPStackPage1;
    QVBoxLayout* vMPStackPage1Layout;
    QTextEdit* vMPLoadPageDescriptionLabel;
    QFrame* vMPLoadLine;
    QPushButton* vMPLoadPageBackButton;
    QPushButton* vMPLoadPageNextButton;
    QPushButton* vMPLoadPageFinishButton;
    QPushButton *vMPLoadPageClearButton;
    QPushButton* vMPLoadPageLoadButton;
    QPushButton* vMPLoadPageArgBrowseButton;
    QPushButton *vMPLoadPageAcceptButton;
    QLineEdit* vMPLoadMPILoadLineedit;
    QLabel* vMPLoadTitleLabel0;
    QLabel* vMPLoadTitleLabel1;
    QLabel* vMPLoadTitleLabel2;
    QFrame* vMPLoadLine1;
    QLabel* vMPLoadParallelPrefixLabel;
    QLineEdit* vMPLoadParallelPrefixLineedit;
    QLabel* vMPLoadCommandArgumentsLabel;
    QLineEdit* vMPLoadCommandArgumentsLineedit;
    QLabel* vMPLoadPageLoadLabel;
#ifdef SHOWCOMMAND_ENABLED
    QLabel* vMPLoadMPIShowCommandLabel;
    QPushButton *vMPLoadPageShowButton;
    // put into public slots if enabled at some point
    virtual void vMPLoadPageShowButtonSelected();
#endif
    QLabel* vMPLoadMPIAcceptCommandLabel;
    QLineEdit* vMPLoadMPICommandLineedit;
    QString vMPLoadPageLoadMPHostName;
    QString vMPLoadPageLoadMPPidName;
    QString vMPLoadPageLoadMPProgName;
    QCheckBox* vMPLoadAttachToProcessCheckBox;
    QCheckBox* vMPLoadAttachToMultiProcessCheckBox;
    QCheckBox* vMPLoadLoadExecutableCheckBox;
    QCheckBox* vMPLoadLoadMultiProcessExecutableCheckBox;
    QCheckBox* vMPLoadLoadDifferentExecutableCheckBox;
    QCheckBox* vMPLoadLoadDifferentMultiProcessExecutableCheckBox;
    QFrame* vMPLoadLine2;
    QScrollView *vpage1sv;
    QVBox *vpage1big_box;
    QSpacerItem* vMPLoadButtonSpacer;
    QSpacerItem* vMPLoadSpacer;
    QHBoxLayout* vMPLoadButtonLayout;
    QVBoxLayout* vMPLoadPageLayout;
    QHBoxLayout* vMPLoadPageSelectLayout;
    QHBoxLayout* vMPLoadPageArgSelectLayout;
#ifdef SHOWCOMMAND_ENABLED
    QHBoxLayout* vMPLoadPageShowLayout;
#endif


    QLabel *vMPLoadProcessListLabel;
    QLabel *vMPLoadMultiProcessListLabel;
    QLabel *vMPLoadMultiProcessExecutableLabel;
    QLabel *vMPLoadExecutableLabel;
    QVBoxLayout* vMPLoadAttachOrLoadLayout;
    QHBoxLayout* vMPLoadSampleRateLayout;

    QScrollView *vpage0sv;
    QVBox *vpage0big_box;
  
    void setMPIWizardCalledMe(bool indicator) {
        areWeControlledByMPIWizard = indicator;
    }

    bool getMPIWizardCalledMe() {
        return areWeControlledByMPIWizard;
    }
  
    void setHaveWeLoadedAnExecutableBefore(bool indicator) {
        haveWeLoadedAnExecutableBefore = indicator;
    }

    bool getHaveWeLoadedAnExecutableBefore() {
        return haveWeLoadedAnExecutableBefore;
    }

    void setHaveWeAttachedToPidBefore(bool indicator) {
        haveWeAttachedToPidBefore = indicator;
    }

    bool getHaveWeAttachedToPidBefore() {
        return haveWeAttachedToPidBefore;
    }

    void setWasDoingParallel(bool wasIdoingParallel) {
        doingParallelLoadsOrAttaches = wasIdoingParallel;
    }

    bool getWasDoingParallel() {
        return doingParallelLoadsOrAttaches;
    }

    void setInstrumentorIsOffline(bool flag) {
        instrumentorIsOffline = flag;
    }

    bool getInstrumentorIsOffline() {
        return instrumentorIsOffline;
    }


  public slots:
    virtual void vAttachOrLoadPageBackButtonSelected();
    virtual void vAttachOrLoadPageClearButtonSelected();
    virtual void vAttachOrLoadPageNextButtonSelected();
    virtual void vAttachOrLoadPageAttachToProcessCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadExecutableCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadMultiProcessExecutableCheckBoxSelected();
    virtual void vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBoxSelected();
    virtual void vAttachOrLoadPageAttachToMultiProcessCheckBoxSelected();
    virtual void vMPLoadPageBackButtonSelected();
    virtual void vMPLoadPageClearButtonSelected();
    virtual void vMPLoadPageLoadButtonSelected();
    virtual void vMPLoadPageAcceptButtonSelected();
    virtual void vMPLoadPageArgBrowseSelected();
    virtual void vSummaryPageFinishButtonSelected(bool wasParallel);
    virtual void finishButtonSelected();
    virtual void vMPLoadPageFinishButtonSelected();

  protected:
    //! Sets the language specific strings.
    virtual void languageChange();
    bool areWeControlledByMPIWizard;
    bool haveWeLoadedAnExecutableBefore;
    bool haveWeAttachedToPidBefore;
    bool doingParallelLoadsOrAttaches;

  private:
    bool instrumentorIsOffline;
};
#endif // TEMPLATE_PANEL_H
