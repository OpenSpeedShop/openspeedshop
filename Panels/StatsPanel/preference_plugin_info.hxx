#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>


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

  QWidget *preference_info_init(QWidgetStack *stack, char *name)
  {
    printf("preference_info_init(0x%x %s) entered\n", stack, name);
//    QWidget *statsPanelStackPage = new QWidget( stack, "statsPanelStackPage" );
//    QWidget *statsPanelStackPage = new QWidget( stack, "Stats Panel" );
    QWidget *statsPanelStackPage = new QWidget( stack, name );

    generalStackPageLayout_3 = new QVBoxLayout( statsPanelStackPage, 11, 6, "generalStackPageLayout_3");

    statsPanelGroupBox = new QGroupBox( statsPanelStackPage, "statsPanelGroupBox" );

    QWidget* statsPanelPrivateLayout = new QWidget( statsPanelGroupBox, "layout8" );
//    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 271, 57 ) );
    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 300, 100 ) );
    layout8 = new QVBoxLayout( statsPanelPrivateLayout, 11, 6, "layout8");

    sortDecendingCheckBox = new QCheckBox( statsPanelPrivateLayout, "sortDecendingCheckBox" );
    layout8->addWidget( sortDecendingCheckBox );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7");

    showTopNTextLabel = new QLabel( statsPanelPrivateLayout, "showTopNTextLabel" );
    layout7->addWidget( showTopNTextLabel );

    showTopNLineEdit = new QLineEdit( statsPanelPrivateLayout, "showTopNLineEdit" );
    layout7->addWidget( showTopNLineEdit );
    layout8->addLayout( layout7 );
    generalStackPageLayout_3->addWidget( statsPanelGroupBox );
    stack->addWidget( statsPanelStackPage, 2 );

//    languageChange();

    sortDecendingCheckBox->setChecked( TRUE );
/*
    statsPanelGroupBox->setTitle( tr( "Stats Panel" ) );
    sortDecendingCheckBox->setText( tr( "Sort Descending" ) );
    showTopNTextLabel->setText( tr( "Show top N items:" ) );
    showTopNLineEdit->setText( tr( "5" ) );
*/
    statsPanelGroupBox->setTitle( "Stats Panel" );
    sortDecendingCheckBox->setText( "Sort Descending" );
    showTopNTextLabel->setText( "Show top N items:" );
    showTopNLineEdit->setText( "5" );

    return statsPanelStackPage;
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H

