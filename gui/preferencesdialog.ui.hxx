#include <qsettings.h>
#include <qapplication.h>

void PreferencesDialog::readPreferencesOnEntry()
{

printf("readPreferencesOnEntry() entered\n");

  QSettings settings;
//  settings.insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.
  QString ds = QString::null;
  bool ok;


  settings.readEntry( "/openspeedshop/general/globalFontFamily", ds, &ok);
  if( ok == FALSE )
  {
    printf("Didn't find any defaults file.  Just take the defaults.\n");
    return;
  }

  fontLineEdit->setText( settings.readEntry( "/openspeedshop/general/globalFontFamily" ));
  precisionLineEdit->setText( settings.readEntry( "/openspeedshop/general/globalPrecision" ));

  setShowSplashScreenCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/general/showSplashScreen") );
  setShowColoredTabsCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/general/showColoredTabs") );
  deleteEmptyPCCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/general/deleteEmptyPC") );
  showGraphicsCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/general/showGraphics") );
  showStatisticsCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/source panel/showStatistics") );
  showLineNumbersCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/source panel/showLineNumber") );
  sortDecendingCheckBox->setChecked(settings.readEntry( "/openspeedshop/stats panel/sortDecending") );

  showTopNLineEdit->setText( settings.readEntry( "/openspeedshop/stats panel/showTopN" ));
}

void PreferencesDialog::resetPreferenceDefaults()
{
    qWarning( "PreferencesDialog::resetPreferenceDefaults(): Not implemented yet" );
}

void PreferencesDialog::setGlobalPrecision()
{
    qWarning( "PreferencesDialog::setGlobalPrecision(): Not implemented yet" );
}

void PreferencesDialog::setShowSplashScreen()
{
    qWarning( "PreferencesDialog::setShowSplashScreen(): Not implemented yet" );
}

void PreferencesDialog::setShowColoredPanelTabs()
{
    qWarning( "PreferencesDialog::setShowColoredPanelTabs(): Not implemented yet" );
}

void PreferencesDialog::setRemoveEmptyPC()
{
    qWarning( "PreferencesDialog::setRemoveEmptyPC(): Not implemented yet" );
}

void PreferencesDialog::setShowAvailableGraphics()
{
    qWarning( "PreferencesDialog::setShowAvailableGraphics(): Not implemented yet" );
}

void PreferencesDialog::setShoeLineNumbers()
{
    qWarning( "PreferencesDialog::setShoeLineNumbers(): Not implemented yet" );
}

void PreferencesDialog::setShowLineNumbers()
{
    qWarning( "PreferencesDialog::setShowLineNumbers(): Not implemented yet" );
}

void PreferencesDialog::setSortDescending()
{
    qWarning( "PreferencesDialog::setSortDescending(): Not implemented yet" );
}

void PreferencesDialog::setShowTopN()
{
    qWarning( "PreferencesDialog::setShowTopN(): Not implemented yet" );
}

void PreferencesDialog::setShowStats()
{
    qWarning( "PreferencesDialog::setShowStats(): Not implemented yet" );
}

void PreferencesDialog::listItemSelected(QListViewItem*lvi)
{
    qWarning( "PreferencesDialog::listItemSelected(QListViewItem*): Not implemented yet" );

  QString s = lvi->text(0);

  printf("s.ascii()=%s\n", s.ascii() );

  if( s == "General" )
  { 
    preferenceDialogWidgetStack->raiseWidget(generalStackPage);
  } else if( s == "Source Panel" )
  {
    preferenceDialogWidgetStack->raiseWidget(sourcePanelStackPage);
  } else if( s == "Stats Panel" )
  {
    preferenceDialogWidgetStack->raiseWidget(statsPanelStackPage);
  } else if( s == "Cmd Panel" )
  {
    preferenceDialogWidgetStack->raiseWidget(cmdPanelStackPage);
  } else
  {
    fprintf(stderr, "Warning: Unable to find category.\n");
  }
}

void PreferencesDialog::selectGlobalFont()
{
    qWarning( "PreferencesDialog::selectGlobalFont(): Not implemented yet" );
}

void PreferencesDialog::setGlobalFont()
{
    qWarning( "PreferencesDialog::setGlobalFont(): Not implemented yet" );
}

void PreferencesDialog::buttonOkSelected()
{
    qWarning( "PreferencesDialog::buttonOkSelected(): Not implemented yet" );

    hide();
}

void PreferencesDialog::savePreferencesOnExit()
{
    qWarning( "PreferencesDialog::savePreferencesOnExit(): Not implemented yet" );

  QSettings settings;
//  settings.insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.
  settings.writeEntry( "/openspeedshop/general/globalFontFamily", qApp->font().family() );
  settings.writeEntry( "/openspeedshop/general/globalPrecision", precisionLineEdit->text() );
  settings.writeEntry( "/openspeedshop/general/showSplashScreen", setShowSplashScreenCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/showColoredTabs", setShowColoredTabsCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/deleteEmptyPC", deleteEmptyPCCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/showGraphics", showGraphicsCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/source panel/showStatistics", showStatisticsCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/source panel/showLineNumber", showLineNumbersCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/stats panel/sortDecending", sortDecendingCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/stats panel/showTopN", showTopNLineEdit->text() );
}
