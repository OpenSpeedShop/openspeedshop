/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "../GettingStartedPanel.hxx"

void GettingStartedClass::experimentListSlot( QListBoxItem * )
{
  printf("experimentListSlot() entered\n");

  gettingStartedPanel->experimentListSlot( );
}
