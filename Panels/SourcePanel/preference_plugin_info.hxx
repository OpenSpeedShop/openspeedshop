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
  QWidget* sourcePanelStackPage;
  QGroupBox* sourcePanelGroupBox;
  QCheckBox* showStatisticsCheckBox;
  QCheckBox* showLineNumbersCheckBox;
  QVBoxLayout* generalStackPageLayout_2;
  QVBoxLayout* layout6;

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
//    printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    sourcePanelStackPage = new QWidget( stack, "Source Panel" );
    generalStackPageLayout_2 = new QVBoxLayout( sourcePanelStackPage, 11, 6, "generalStackPageLayout_2"); 

    sourcePanelGroupBox = new QGroupBox( sourcePanelStackPage, "sourcePanelGroupBox" );

    QWidget* sourcePanelPrivateLayout = new QWidget( sourcePanelGroupBox, "layout6" );
//    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 150, 52 ) );
    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 200, 100 ) );
    layout6 = new QVBoxLayout( sourcePanelPrivateLayout, 11, 6, "layout6"); 

    showStatisticsCheckBox =
      new QCheckBox( sourcePanelPrivateLayout, "showStatisticsCheckBox" );
    showStatisticsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showStatisticsCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showStatisticsCheckBox );

    showLineNumbersCheckBox = new QCheckBox( sourcePanelPrivateLayout, "showLineNumbersCheckBox" );
    showLineNumbersCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showLineNumbersCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showLineNumbersCheckBox );
    generalStackPageLayout_2->addWidget( sourcePanelGroupBox );
    stack->addWidget( sourcePanelStackPage, 1 );

// languageChange();
    showStatisticsCheckBox->setChecked(FALSE);
    showLineNumbersCheckBox->setChecked(FALSE);
/*
    sourcePanelGroupBox->setTitle( tr( "Source Panel" ) );
    showStatisticsCheckBox->setText( tr( "Show statistics" ) );
    showLineNumbersCheckBox->setText( tr( "Show line numbers" ) );
*/
    sourcePanelGroupBox->setTitle( "Source Panel" );
    showStatisticsCheckBox->setText( "Show statistics" );
    showLineNumbersCheckBox->setText( "Show line numbers" );

    if( settings != NULL )
    {
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showStatisticsCheckBox->name() );
      showStatisticsCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );
  
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showLineNumbersCheckBox->name() );
      showLineNumbersCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );
    }

    return( sourcePanelStackPage );
  }
  void save_preferences_entry_point(QSettings *settings, char *name)
  {
//    printf("save_preferences_entry_point(0x%x %s) entered\n", settings, name);

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showStatisticsCheckBox->name() );
    settings->writeEntry(settings_buffer, showStatisticsCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showLineNumbersCheckBox->name() );
    settings->writeEntry(settings_buffer, showLineNumbersCheckBox->isChecked());
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
