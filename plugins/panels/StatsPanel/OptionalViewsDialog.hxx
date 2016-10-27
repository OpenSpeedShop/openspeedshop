////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007-2014 Krell Institute  All Rights Reserved.
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
  


#ifndef OPTIONALVIEWSDIALOG_H
#define OPTIONALVIEWSDIALOG_H

#include <stddef.h>
#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QSplitter;
class QFrame;
class QListView;
class QListViewItem;
class QWidgetStack;
class QWidget;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QCheckBox;

#include "qsettings.h"

class PanelContainer;

class OptionalViewsDialog : public QDialog
{
    Q_OBJECT

public:

    OptionalViewsDialog( QWidget* parent = 0, 
                         const char* name = 0, 
                         QString collectorString = "", 
                         std::list<std::string> *current_modifiers = NULL, 
                         bool modal = FALSE, WFlags fl = 0 );
    ~OptionalViewsDialog();

    PanelContainer *panelContainer;

    QSplitter* mainSplitter;
    QFrame* preferenceDialogLeftFrame;
    QListView* categoryListView;
    QFrame* preferenceDialogRightFrame;
    QWidgetStack* preferenceDialogWidgetStack;
    QWidget* generalStackPage;
    QGroupBox* GeneralGroupBox;
    QGroupBox* VTraceGroupBox;
    QPushButton* setFontButton;
    QLineEdit* fontLineEdit;
    QLabel* remoteShellLabel;
    QLineEdit* remoteShellEdit;
    QCheckBox* setShowSplashScreenCheckBox;
    QCheckBox* showGraphicsCheckBox;
    QPushButton* buttonHelp;
    QPushButton* buttonDefaults;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QString globalCollectorString;
    std::list<std::string> *globalCurrentModifiers;

    bool pcsamp_time;
    bool pcsamp_percent;
    bool pcsamp_ThreadAverage;
    bool pcsamp_ThreadMin;
    bool pcsamp_ThreadMax;

    bool usertime_exclusive_times;
    bool usertime_inclusive_times;
    bool usertime_percent;
    bool usertime_count;
    bool usertime_ThreadAverage;
    bool usertime_ThreadMin;
    bool usertime_ThreadMax;

    bool omptp_exclusive_times;
    bool omptp_inclusive_times;
    bool omptp_percent;
    bool omptp_count;
    bool omptp_ThreadAverage;
    bool omptp_ThreadMin;
    bool omptp_ThreadMax;

    bool hwc_overflows;
    bool hwc_counts;
    bool hwc_percent;
    bool hwc_ThreadAverage;
    bool hwc_ThreadMin;
    bool hwc_ThreadMax;

    bool hwcsamp_time;
    bool hwcsamp_allEvents;
    bool hwcsamp_percent;
    bool hwcsamp_ThreadAverage;
    bool hwcsamp_ThreadMin;
    bool hwcsamp_ThreadMax;

    bool hwctime_exclusive_counts;
    bool hwctime_exclusive_overflows;
    bool hwctime_inclusive_overflows;
    bool hwctime_inclusive_counts;
    bool hwctime_percent;
    bool hwctime_ThreadAverage;
    bool hwctime_ThreadMin;
    bool hwctime_ThreadMax;

    bool io_exclusive_times;
    bool io_min;
    bool io_max;
    bool io_average;
    bool io_count;
    bool io_percent;
    bool io_stddev;
    bool io_ThreadAverage;
    bool io_ThreadMin;
    bool io_ThreadMax;

    bool iot_exclusive_times;
    bool iot_inclusive_times;
    bool iot_min;
    bool iot_max;
    bool iot_average;
    bool iot_count;
    bool iot_percent;
    bool iot_stddev;
    bool iot_start_time;
    bool iot_stop_time;
    bool iot_syscallno;
    bool iot_nsysargs;
    bool iot_retval;
    bool iot_pathname;

    bool iop_exclusive_times;
    bool iop_inclusive_times;
    bool iop_percent;
    bool iop_count;
    bool iop_ThreadAverage;
    bool iop_ThreadMin;
    bool iop_ThreadMax;

    bool mem_exclusive_times;
    bool mem_min;
    bool mem_max;
    bool mem_average;
    bool mem_count;
    bool mem_percent;
    bool mem_stddev;
    bool mem_ThreadAverage;
    bool mem_ThreadMin;
    bool mem_ThreadMax;

    bool mpi_exclusive_times;
    bool mpi_inclusive_times;
    bool mpi_min;
    bool mpi_max;
    bool mpi_average;
    bool mpi_count;
    bool mpi_percent;
    bool mpi_stddev;

    bool mpip_exclusive_times;
    bool mpip_inclusive_times;
    bool mpip_percent;
    bool mpip_count;
    bool mpip_ThreadAverage;
    bool mpip_ThreadMin;
    bool mpip_ThreadMax;

    bool mpit_exclusive_times;
    bool mpit_inclusive_times;
    bool mpit_min;
    bool mpit_max;
    bool mpit_average;
    bool mpit_count;
    bool mpit_percent;
    bool mpit_stddev;
    bool mpit_start_time;
    bool mpit_stop_time;
    bool mpit_source;
    bool mpit_dest;
    bool mpit_size;
    bool mpit_tag;
    bool mpit_communicator;
    bool mpit_datatype;
    bool mpit_retval;

    bool pthreads_exclusive_times;
    bool pthreads_min;
    bool pthreads_max;
    bool pthreads_average;
    bool pthreads_count;
    bool pthreads_percent;
    bool pthreads_stddev;
    bool pthreads_ThreadAverage;
    bool pthreads_ThreadMin;
    bool pthreads_ThreadMax;

    bool fpe_time;
    bool fpe_counts;
    bool fpe_inclusive_counts;
    bool fpe_percent;
    bool fpe_ThreadAverage;
    bool fpe_ThreadMin;
    bool fpe_ThreadMax;
    bool fpe_inexact_result_count;
    bool fpe_underflow_count;
    bool fpe_overflow_count;
    bool fpe_division_by_zero_count;
    bool fpe_unnormal_count;
    bool fpe_invalid_count;
    bool fpe_unknown_count;

    QWidget *matchPreferencesToStack(QString s);
    void createExperimentDependentOptionalView(QWidgetStack* stack, const char* name );
    bool isInCurrentModifierList(std::string);

    bool preferencesAvailable;
    QString globalRemoteShell;
    QString globalFontFamily;
    int globalFontPointSize;
    int globalFontWeight;
    bool globalFontItalic;

    int viewFieldSize;
    int viewPrecision; 
    int historyLimit; 
    int historyDefault; 
    int maxAsyncCommands; 
    int helpLevelDefault; 
    bool viewFullPath; 
    bool saveExperimentDatabase; 
    bool onRerunSaveCopyOfExperimentDatabase; 
    bool askAboutChangingArgs; 
    bool askAboutSavingTheDatabase; 
    bool viewMangledName; 
    bool allowPythonCommands; 
    bool instrumentorIsOffline; 
    bool lessRestrictiveComparisons; 
    bool blanksInsteadOfZerosInComparison; 

    QSettings *settings;
    void readPreferencesOnEntry();
    void savePreferences();
    void applyPreferences();
public slots:
    virtual void resetPreferenceDefaults();
    virtual void listItemSelected(QListViewItem *);
    virtual void buttonApplySelected();
    virtual void buttonOkSelected();

protected:
    QVBoxLayout* OptionalViewsDialogLayout;
    QVBoxLayout* preferenceDialogListLayout;
    QVBoxLayout* preferenceDialogLeftFrameLayout;
    QVBoxLayout* preferenceDialogRightFrameLayout;
    QVBoxLayout* generalStackPageLayout;
    QVBoxLayout* rightSideLayout;
    QVBoxLayout* rightSideVTraceLayout;
    QHBoxLayout* fontLayout;
    QHBoxLayout* remoteShellLayout;
    QVBoxLayout* generalStackPageLayout_4;
    QHBoxLayout* OptionalViewsDialogWidgetStackLayout;
    QSpacerItem* Horizontal_Spacing2;

    QHBoxLayout *viewFieldSizeLayout;
      QLabel *viewFieldSizeLabel;
      QLineEdit *viewFieldSizeLineEdit;
    QHBoxLayout *viewPrecisionLayout;
      QLabel *viewPrecisionLabel;
      QLineEdit *viewPrecisionLineEdit;
    QHBoxLayout *historyLimitLayout;
      QLabel *historyLimitLabel;
      QLineEdit *historyLimitLineEdit;
    QHBoxLayout *historyDefaultLayout;
      QLabel *historyDefaultLabel;
      QLineEdit *historyDefaultLineEdit;
    QHBoxLayout *maxAsyncCommandsLayout;
      QLabel *maxAsyncCommandsLabel;
      QLineEdit *maxAsyncCommandsLineEdit;
    QHBoxLayout *helpLevelDefaultLayout;
      QLabel *helpLevelDefaultLabel;
      QLineEdit *helpLevelDefaultLineEdit;
    QCheckBox *viewFullPathCheckBox;

    QCheckBox *pcsamp_time_CheckBox;
    QCheckBox *pcsamp_percent_CheckBox;
    QCheckBox *pcsamp_ThreadAverage_CheckBox;
    QCheckBox *pcsamp_ThreadMin_CheckBox;
    QCheckBox *pcsamp_ThreadMax_CheckBox;

    QCheckBox *usertime_exclusive_times_CheckBox;
    QCheckBox *usertime_inclusive_times_CheckBox;
    QCheckBox *usertime_percent_CheckBox;
    QCheckBox *usertime_count_CheckBox;
    QCheckBox *usertime_ThreadAverage_CheckBox;
    QCheckBox *usertime_ThreadMin_CheckBox;
    QCheckBox *usertime_ThreadMax_CheckBox;

    QCheckBox *omptp_exclusive_times_CheckBox;
    QCheckBox *omptp_inclusive_times_CheckBox;
    QCheckBox *omptp_percent_CheckBox;
    QCheckBox *omptp_count_CheckBox;
    QCheckBox *omptp_ThreadAverage_CheckBox;
    QCheckBox *omptp_ThreadMin_CheckBox;
    QCheckBox *omptp_ThreadMax_CheckBox;

    QCheckBox *hwcsamp_time_CheckBox;
    QCheckBox *hwcsamp_allEvents_CheckBox;
    QCheckBox *hwcsamp_percent_CheckBox;
    QCheckBox *hwcsamp_ThreadAverage_CheckBox;
    QCheckBox *hwcsamp_ThreadMin_CheckBox;
    QCheckBox *hwcsamp_ThreadMax_CheckBox;

    QCheckBox *hwctime_exclusive_counts_CheckBox;
    QCheckBox *hwctime_exclusive_overflows_CheckBox;
    QCheckBox *hwctime_inclusive_overflows_CheckBox;
    QCheckBox *hwctime_inclusive_counts_CheckBox;
    QCheckBox *hwctime_percent_CheckBox;
    QCheckBox *hwctime_ThreadAverage_CheckBox;
    QCheckBox *hwctime_ThreadMin_CheckBox;
    QCheckBox *hwctime_ThreadMax_CheckBox;

    QCheckBox *hwc_overflows_CheckBox;
    QCheckBox *hwc_counts_CheckBox;
    QCheckBox *hwc_percent_CheckBox;
    QCheckBox *hwc_ThreadAverage_CheckBox;
    QCheckBox *hwc_ThreadMin_CheckBox;
    QCheckBox *hwc_ThreadMax_CheckBox;

    QCheckBox *iot_exclusive_times_CheckBox;
    QCheckBox *iot_inclusive_times_CheckBox;
    QCheckBox *iot_min_CheckBox;
    QCheckBox *iot_max_CheckBox;
    QCheckBox *iot_average_CheckBox;
    QCheckBox *iot_count_CheckBox;
    QCheckBox *iot_percent_CheckBox;
    QCheckBox *iot_stddev_CheckBox;
    QCheckBox *iot_start_time_CheckBox;
    QCheckBox *iot_stop_time_CheckBox;
    QCheckBox *iot_syscallno_CheckBox;
    QCheckBox *iot_nsysargs_CheckBox;
    QCheckBox *iot_retval_CheckBox;
    QCheckBox *iot_pathname_CheckBox;

    QCheckBox *io_exclusive_times_CheckBox;
    QCheckBox *io_min_CheckBox;
    QCheckBox *io_max_CheckBox;
    QCheckBox *io_average_CheckBox;
    QCheckBox *io_count_CheckBox;
    QCheckBox *io_percent_CheckBox;
    QCheckBox *io_stddev_CheckBox;
    QCheckBox *io_ThreadAverage_CheckBox;
    QCheckBox *io_ThreadMin_CheckBox;
    QCheckBox *io_ThreadMax_CheckBox;

    QCheckBox *iop_exclusive_times_CheckBox;
    QCheckBox *iop_inclusive_times_CheckBox;
    QCheckBox *iop_percent_CheckBox;
    QCheckBox *iop_count_CheckBox;
    QCheckBox *iop_ThreadAverage_CheckBox;
    QCheckBox *iop_ThreadMin_CheckBox;
    QCheckBox *iop_ThreadMax_CheckBox;

    QCheckBox *mem_exclusive_times_CheckBox;
    QCheckBox *mem_min_CheckBox;
    QCheckBox *mem_max_CheckBox;
    QCheckBox *mem_average_CheckBox;
    QCheckBox *mem_count_CheckBox;
    QCheckBox *mem_percent_CheckBox;
    QCheckBox *mem_stddev_CheckBox;
    QCheckBox *mem_ThreadAverage_CheckBox;
    QCheckBox *mem_ThreadMin_CheckBox;
    QCheckBox *mem_ThreadMax_CheckBox;

    QCheckBox *mpi_exclusive_times_CheckBox;
    QCheckBox *mpi_inclusive_times_CheckBox;
    QCheckBox *mpi_min_CheckBox;
    QCheckBox *mpi_max_CheckBox;
    QCheckBox *mpi_average_CheckBox;
    QCheckBox *mpi_count_CheckBox;
    QCheckBox *mpi_percent_CheckBox;
    QCheckBox *mpi_stddev_CheckBox;

    QCheckBox *mpip_exclusive_times_CheckBox;
    QCheckBox *mpip_inclusive_times_CheckBox;
    QCheckBox *mpip_percent_CheckBox;
    QCheckBox *mpip_count_CheckBox;
    QCheckBox *mpip_ThreadAverage_CheckBox;
    QCheckBox *mpip_ThreadMin_CheckBox;
    QCheckBox *mpip_ThreadMax_CheckBox;

    QCheckBox *mpit_exclusive_times_CheckBox;
    QCheckBox *mpit_inclusive_times_CheckBox;
    QCheckBox *mpit_min_CheckBox;
    QCheckBox *mpit_max_CheckBox;
    QCheckBox *mpit_average_CheckBox;
    QCheckBox *mpit_count_CheckBox;
    QCheckBox *mpit_percent_CheckBox;
    QCheckBox *mpit_stddev_CheckBox;
    QCheckBox *mpit_start_time_CheckBox;
    QCheckBox *mpit_stop_time_CheckBox;
    QCheckBox *mpit_source_CheckBox;
    QCheckBox *mpit_dest_CheckBox;
    QCheckBox *mpit_size_CheckBox;
    QCheckBox *mpit_tag_CheckBox;
    QCheckBox *mpit_communicator_CheckBox;
    QCheckBox *mpit_datatype_CheckBox;
    QCheckBox *mpit_retval_CheckBox;

    QCheckBox *pthreads_exclusive_times_CheckBox;
    QCheckBox *pthreads_min_CheckBox;
    QCheckBox *pthreads_max_CheckBox;
    QCheckBox *pthreads_average_CheckBox;
    QCheckBox *pthreads_count_CheckBox;
    QCheckBox *pthreads_percent_CheckBox;
    QCheckBox *pthreads_stddev_CheckBox;
    QCheckBox *pthreads_ThreadAverage_CheckBox;
    QCheckBox *pthreads_ThreadMin_CheckBox;
    QCheckBox *pthreads_ThreadMax_CheckBox;

    QCheckBox *fpe_time_CheckBox;
    QCheckBox *fpe_counts_CheckBox;
    QCheckBox *fpe_inclusive_counts_CheckBox;
    QCheckBox *fpe_percent_CheckBox;
    QCheckBox *fpe_ThreadAverage_CheckBox;
    QCheckBox *fpe_ThreadMin_CheckBox;
    QCheckBox *fpe_ThreadMax_CheckBox;
    QCheckBox *fpe_inexact_result_count_CheckBox;
    QCheckBox *fpe_underflow_count_CheckBox;
    QCheckBox *fpe_overflow_count_CheckBox;
    QCheckBox *fpe_division_by_zero_count_CheckBox;
    QCheckBox *fpe_unnormal_count_CheckBox;
    QCheckBox *fpe_invalid_count_CheckBox;
    QCheckBox *fpe_unknown_count_CheckBox;

    QCheckBox *saveExperimentDatabaseCheckBox;
    QCheckBox *onRerunSaveCopyOfExperimentDatabaseCheckBox;
    QCheckBox *askAboutChangingArgsCheckBox;
    QCheckBox *askAboutSavingTheDatabaseCheckBox;
    QCheckBox *viewMangledNameCheckBox;
    QCheckBox *allowPythonCommandsCheckBox;
    QCheckBox *instrumentorIsOfflineCheckBox;
    QCheckBox *lessRestrictiveComparisonsCheckBox;
    QCheckBox *blanksInsteadOfZerosInComparisonCheckBox;
    QCheckBox *logByDefaultCheckBox;
    QCheckBox *limitSignalCatchingCheckBox;


protected slots:
    virtual void languageChange();

};

#endif // OPTIONALVIEWSDIALOG_H

