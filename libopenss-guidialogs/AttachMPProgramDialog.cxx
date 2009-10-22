////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 The Krell Institute All Rights Reserved.
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
  
// Debug Flag
//#define DEBUG_AMPPD 1
//

#include "AttachMPProgramDialog.hxx"

#include "debug.hxx"

#include <qcheckbox.h>
#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qsettings.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qdialog.h>


void
AttachMPProgramDialog::parallelCBSelected()
{

  bool ok = false;
  guiSettings = new QSettings();

  if (parallelCB->isChecked()) {
#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::parallelCBSelected() entered., parallelCB->isChecked()=%d\n", parallelCB->isChecked());
#endif
  // See if we already saved the previous parallel execution command prefix
  bool localReuseParallelCommand = guiSettings->readBoolEntry(QString("/openspeedshop/ManageProcessesPanel/saveMPCommandCheckBox"), false, &ok);

#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::parallelCBSelected() localReuseParallelCommand=%d, ok=%d\n", localReuseParallelCommand, ok);
#endif

  // If this is set then this isn't the first time through this code or it was set in through the preference panel
  if (localReuseParallelCommand ) {
    
    // Set the save parallel execution command prefix checkbox to set, since it was set in the preferences.
    // When we show the parallel execution prefix entry fields they should reflect the state as it currently is
    // based on preferences.
    parallelSaveCB->setChecked(true);

    QString usersParallelPrefixCommand = guiSettings->readEntry(QString("/openspeedshop/ManageProcessesPanel/updateDisplayMultiProcessCommandLineEdit"), QString::null, &ok);

#ifdef DEBUG_AMPPD
    printf("AttachMPProgramDialog::parallelCBSelected, ok=%d\n", ok);
    printf("AttachMPProgramDialog::parallelCBSelected, usersParallelPrefixCommand.ascii()=%s\n", usersParallelPrefixCommand.ascii() );
#endif

    // If successful reading the preference entry then set the preference text into the command
    // When we show the parallel execution prefix entry fields they should reflect the state as it currently is
    // based on preferences.

    if (ok) {
      usersParallelPrefixCommand.append(" ");  // add spacing to the input prefix, need to separate it from command
      parallelPrefixLineedit->setText(usersParallelPrefixCommand);
    }

  } else {
    // Don't set the save parallel execution command prefix checkbox it was not set in the preferences
    usersParallelPrefixCommand = QString("");
    parallelSaveCB->setChecked(false);
    parallelPrefixLineedit->setText(usersParallelPrefixCommand);
  }

    parallelPrefixLabel->show();
    parallelPrefixLineedit->show();
    parallelSaveCB->show();
    psaveCBlabel->show();
  } // end isChecked()
  else {
#ifdef DEBUG_AMPPD
    printf("AttachMPProgramDialog::parallelCBSelected(), isChecked()==FALSE, parallelPrefixLineedit->text().ascii()=%s\n", parallelPrefixLineedit->text().ascii() );
#endif
    // Don't use the text in the formation of the expattach or expcreate command.
    parallelPrefixLineedit->setText("");
#ifdef DEBUG_AMPPD
    printf("AttachMPProgramDialog::parallelCBSelected(), isChecked()==FALSE, AFTER CLEAR,parallelPrefixLineedit->text().ascii()=%s\n", 
            parallelPrefixLineedit->text().ascii() );
#endif

//    parallelPrefixLabel->hide();
//    parallelPrefixLineedit->hide();
//    parallelSaveCB->hide();
//    psaveCBlabel->hide();
  }
//   delete guiSettings;
#ifdef DEBUG_AMPPD
     printf("AttachMPProgramDialog::parallelCBSelected() exited.\n");
#endif
}

void
AttachMPProgramDialog::parallelSaveCBSelected()
{
#ifdef DEBUG_AMPPD
   printf("AttachMPProgramDialog::parallelSaveCBSelected() entered.\n");
#endif

   guiSettings = new QSettings();

   QString prefNameString("updateDisplayMultiProcessCommandLineEdit");
   QString parallelPrefixStringNull("");
   bool ok = false;
   bool btrue = true;
   bool bfalse = false;

#ifdef DEBUG_AMPPD
   printf("AttachMPProgramDialog::parallelSaveCBSelected(),enter, parallelSaveCB->isChecked()=%d\n", parallelSaveCB->isChecked());
#endif
   if (parallelSaveCB->isChecked()) {

     // save the entry to the preferences file
     // at this point
     shouldWeReuseParallelCommand(true);

#ifdef DEBUG_AMPPD
     printf("AttachMPProgramDialog::parallelSaveCBSelected(),true, calling writeBoolEntry \n");
#endif
     guiSettings->writeEntry(QString("/openspeedshop/ManageProcessesPanel/saveMPCommandCheckBox"), true);

#ifdef DEBUG_AMPPD
     printf("AttachMPProgramDialog::parallelSaveCBSelected(),true,calling writeEntry, usersParallelPrefixCommand=%s\n",
             usersParallelPrefixCommand.ascii() );
#endif

     guiSettings->writeEntry(QString("/openspeedshop/ManageProcessesPanel/updateDisplayMultiProcessCommandLineEdit"), usersParallelPrefixCommand);
     
   } else {

     shouldWeReuseParallelCommand(false);

#ifdef DEBUG_AMPPD
     printf("AttachMPProgramDialog::parallelSaveCBSelected(),false, calling writeBoolEntry \n");
#endif
     guiSettings->writeEntry(QString("/openspeedshop/ManageProcessesPanel/saveMPCommandCheckBox"), false);

#ifdef DEBUG_AMPPD
     printf("AttachMPProgramDialog::parallelSaveCBSelected(),false,calling writeEntry, usersParallelPrefixCommand=%s\n",
             usersParallelPrefixCommand.ascii() );
#endif
     guiSettings->writeEntry(QString("/openspeedshop/ManageProcessesPanel/updateDisplayMultiProcessCommandLineEdit"), parallelPrefixStringNull);
   }

#ifdef DEBUG_AMPPD
   printf("AttachMPProgramDialog::parallelSaveCBSelected() exited.\n");
#endif
   delete guiSettings;

}




AttachMPProgramDialog::AttachMPProgramDialog( QWidget* parent, const char* name, bool modal)
    : QFileDialog( parent, name, modal )
{

    QObjectList *l = topLevelWidget()->queryList( "QPushButton" );
    QObjectListIt it( *l ); // iterate over the buttons
    QObject *obj;

    while ( (obj = it.current()) != 0 ) {
        // for each found object...
        ++it;
#ifdef DEBUG_AMPPD
      printf("button->text=%s\n",((QPushButton*)obj)->text().ascii());
#endif
        char* result = (char *) strstr( "OK", ((QPushButton*)obj)->text().ascii() );
        if( result != NULL ) {
          connect(((QPushButton*)obj), SIGNAL( clicked() ), this, SLOT( accepted() ) );
        }
    }
    delete l; // delete the list, not the objects

  nprintf(DEBUG_CONST_DESTRUCT) ("AttachMPProgramDialog::AttachMPProgramDialog() constructor called.\n");
#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::AttachMPProgramDialog() constructor called.\n");
#endif

  bool ok = false;
  bool bfalse = false;
  
  // Set the name of this dialog box
  if ( !name ) setName( "AttachMPProgramDialog" );

  FormLabel = new QLabel( tr("Create a MPI command of this form: parallel_prefix executable executable_args<br>"), this );
  ExampleLabel = new QLabel( tr("Example: mpirun -np 64 mpqc -f mpqc.input<br>"), this );
  addWidgets( FormLabel, NULL, (QPushButton *)NULL );
  addWidgets( ExampleLabel, NULL, (QPushButton *)NULL );

  // Create the parallel execution extra widget entities - parallel execution prefix label and text entry form
  parallelPrefixLabel = new QLabel( tr("Enter Parallel Execution Prefix:"), this );
  parallelPrefixLineedit = new QLineEdit( this );

  // Add the parallel execution text entry and label to the dialog form.
  addWidgets( parallelPrefixLabel, parallelPrefixLineedit, (QPushButton *)NULL );

  // Create the save or not - parallel execution extra widget entities - save parallel execution prefix checkbox and label
  psaveCBlabel = new QLabel( tr("Save entered parallel prefix command for reuse during this session?:"), this );
  parallelSaveCB = new QCheckBox( this, "parallelSaveCB");
  addWidgets( psaveCBlabel, parallelSaveCB, NULL );

  // Create the first extra widget entity - command line agrument label and text entry form
  label = new QLabel( tr("Program command line arguments:"), this );
  lineedit = new QLineEdit( this );

  // Add the entities to the dialog form
  addWidgets( label, lineedit, (QPushButton *)NULL );

  // Create a callback for when the save for reuse parallel execution text checkbox is clicked.
  connect( parallelSaveCB, SIGNAL( clicked() ), this, SLOT( parallelSaveCBSelected() ) );

  // Hide the parallel specific entries until the parallel job checkbox is clicked
//  parallelPrefixLabel->hide();
//  parallelPrefixLineedit->hide();
//  parallelSaveCB->hide();
//  psaveCBlabel->hide();

#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::AttachMPProgramDialog() constructor exits.\n");
#endif
}


/*
 *  Destroys the object and frees any allocated resources
 */
AttachMPProgramDialog::~AttachMPProgramDialog()
{
#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::AttachMPProgramDialog() destructor called.\n");
#endif

// no need to delete child widgets, Qt does it all for us
  
#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::AttachMPProgramDialog() destructor exits.\n");
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AttachMPProgramDialog::languageChange()
{
  setCaption( tr( "AttachMPProgramDialog" ) );
#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::languageChange() called.\n");
#endif
}

void AttachMPProgramDialog::accept()
{

#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::accept() called.\n");
  printf("AttachMPProgramDialog::accept(), parallelPrefixLineedit->text().ascii()=%s\n", 
         parallelPrefixLineedit->text().ascii() );
#endif

  usersParallelPrefixCommand = QString(parallelPrefixLineedit->text().ascii());
  usersParallelPrefixCommand.append(" "); // add spacing to the input prefix, need to separate it from command
  // Make sure the parallel prefix command is saved if it is needed to be saved.
  parallelSaveCBSelected();
  // We need to call the main dialog accept or else we hang....
  QDialog::accept();

#ifdef DEBUG_AMPPD
  printf("AttachMPProgramDialog::accept() exits.\n");
#endif
}
