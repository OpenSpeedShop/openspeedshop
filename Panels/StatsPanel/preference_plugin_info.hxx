#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsettings.h>


extern "C"
{
  QWidget* statsPanelStackPage;
  QVBoxLayout* generalStackPageLayout_3;
  QGroupBox* statsPanelGroupBox;
  QVBoxLayout* layout8;
  QCheckBox* sortDecendingCheckBox;
  QHBoxLayout* layout7;
  QLabel* showTopNTextLabel;
  QLineEdit* showTopNLineEdit;

  static char *pname = NULL;

  bool getPreferenceSortDecending()
  {
// printf("getPreferenceSortDecending(%s)\n", pname);
    return( sortDecendingCheckBox->isChecked() );
  }

  QString getPreferenceTopNLineEdit()
  {
// printf("getPreferenceTopNLineEdit(%s)\n", pname);
    return( showTopNLineEdit->text() );
  }

  void initPreferenceSettings()
  {
// printf("initPreferenceSettings(%s)\n", pname);
    sortDecendingCheckBox->setChecked(TRUE);
    showTopNLineEdit->setText( "5" );
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
//    printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    QWidget *statsPanelStackPage = new QWidget( stack, name );
    pname = strdup(name);

    generalStackPageLayout_3 =
      new QVBoxLayout( statsPanelStackPage, 11, 6, "generalStackPageLayout_3");

    statsPanelGroupBox = new QGroupBox( statsPanelStackPage, "statsPanelGroupBox" );

    QWidget* statsPanelPrivateLayout = new QWidget( statsPanelGroupBox, "layout8" );
//    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 271, 57 ) );
    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 300, 100 ) );
    layout8 = new QVBoxLayout( statsPanelPrivateLayout, 11, 6, "layout8");

    sortDecendingCheckBox =
      new QCheckBox( statsPanelPrivateLayout, "sortDecendingCheckBox" );
    layout8->addWidget( sortDecendingCheckBox );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7");

    showTopNTextLabel =
      new QLabel( statsPanelPrivateLayout, "showTopNTextLabel" );
    layout7->addWidget( showTopNTextLabel );

    showTopNLineEdit =
      new QLineEdit( statsPanelPrivateLayout, "showTopNLineEdit" );

    layout7->addWidget( showTopNLineEdit );
    layout8->addLayout( layout7 );
    generalStackPageLayout_3->addWidget( statsPanelGroupBox );
    stack->addWidget( statsPanelStackPage, 2 );

    statsPanelGroupBox->setTitle( "Stats Panel" );
    sortDecendingCheckBox->setText( "Sort Descending" );
    showTopNTextLabel->setText( "Show top N items:" );

    initPreferenceSettings();

    if( settings != NULL )
    {
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, sortDecendingCheckBox->name() );
      sortDecendingCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTopNLineEdit->name() );
      showTopNLineEdit->setText(
        settings->readEntry(settings_buffer, "5") );
    }

    return statsPanelStackPage;
  }
  int save_preferences_entry_point(QSettings *settings, char *name)
  {
//    printf("save_preferences_entry_point(0x%x %s) entered\n", settings, name);

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, sortDecendingCheckBox->name() );
    settings->writeEntry(settings_buffer, sortDecendingCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTopNLineEdit->name() );
    settings->writeEntry(settings_buffer, showTopNLineEdit->text() );
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H

