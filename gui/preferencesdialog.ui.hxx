#include <qsettings.h>
#include <qapplication.h>

void PreferencesDialog::readPreferencesOnEntry()
{
//  printf("readPreferencesOnEntry() entered\n");

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

  preferencesAvailable = TRUE;

  globalFontFamily =
    settings.readEntry( "/openspeedshop/general/globalFontFamily" );
  fontLineEdit->setText( globalFontFamily );

  globalFontPointSize =
    settings.readNumEntry("/openspeedshop/general/globalFontPointSize");
  globalFontWeight =
    settings.readNumEntry("/openspeedshop/general/globalFontWeight");
  globalFontItalic =
     settings.readBoolEntry("/openspeedshop/general/globalFontItalic");



  precisionLineEdit->setText(
    settings.readEntry( "/openspeedshop/general/globalPrecision") );

  setShowSplashScreenCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/showSplashScreen") );
  setShowColoredTabsCheckBox->setChecked(settings
    .readBoolEntry( "/openspeedshop/general/showColoredTabs") );
  deleteEmptyPCCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/deleteEmptyPC") );
  showGraphicsCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/showGraphics") );
  showStatisticsCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/source panel/showStatistics") );
  showLineNumbersCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/source panel/showLineNumber") );
  sortDecendingCheckBox->setChecked(
    settings.readEntry( "/openspeedshop/stats panel/sortDecending") );

  showTopNLineEdit->setText( 
    settings.readEntry( "/openspeedshop/stats panel/showTopN" ));
}

void PreferencesDialog::resetPreferenceDefaults()
{
    qWarning( "PreferencesDialog::resetPreferenceDefaults(): Not implemented yet" );
  languageChange();
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
  if( lvi == NULL )
  {
    return;
  }

  QString s = lvi->text(0);

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

#include <qfontdialog.h>
void PreferencesDialog::selectGlobalFont()
{
  bool ok;
  QFont font = QFontDialog::getFont( &ok, this );
  if ( ok )
  { // font is set to the font the user selected
    globalFontFamily = font.family();
    fontLineEdit->setText( globalFontFamily );
    globalFontPointSize = font.pointSize();
//  fontLineEdit->setText( globalFontFamily );
    globalFontWeight = font.weight();
//  fontLineEdit->setText( globalFontFamily );
    globalFontItalic = font.italic();
//  fontLineEdit->setText( globalFontFamily );
  }
}

void PreferencesDialog::applyPreferences()
{

  QFont *m_font = new QFont( globalFontFamily,
                      globalFontPointSize,
                      globalFontWeight,
                      globalFontItalic );
  qApp->setFont(*m_font, TRUE);

  delete( m_font );


  // NOTIFY EVERYONE THAT PREFERENCES HAVE CHANGED!
  printf("NOTIFY EVERYONE THAT PREFERENCES HAVE CHANGED!\n");
}

void PreferencesDialog::buttonApplySelected()
{
    applyPreferences();
}

void PreferencesDialog::buttonOkSelected()
{
    applyPreferences();

    hide();
}

void PreferencesDialog::savePreferencesOnExit()
{
  QSettings settings;
//  settings.insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.

  settings.writeEntry( "/openspeedshop/general/globalFontFamily", globalFontFamily );

  settings.writeEntry("/openspeedshop/general/globalFontPointSize", globalFontPointSize );
  settings.writeEntry("/openspeedshop/general/globalFontWeight", globalFontWeight );
  settings.writeEntry("/openspeedshop/general/globalFontItalic", globalFontItalic );


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
