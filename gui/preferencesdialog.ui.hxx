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

