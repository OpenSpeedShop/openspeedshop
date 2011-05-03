////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011  Krell Institute  All Rights Reserved.
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


#include "ChooseExperimentDialog.hxx"
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a ChooseExperimentDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */

ChooseExperimentDialog::ChooseExperimentDialog( QWidget* parent,  const char* name, std::vector<int> *argCompareExpIDs, std::vector<QString> *argCompareExpDBNames, std::vector<QString> *argCompareSrcFilenames, int argFocusOnExpIDsCheckBox, bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    if ( !name )
        setName( "ChooseExperimentDialog" );

//    resize( 310, 222 );
    resize( 610, 222 );

    setCaption( tr( "Choose Experiment for View"  ) );
    compareExpIDs = argCompareExpIDs;
    compareExpDBNames = argCompareExpDBNames;
    compareSrcFilenames = argCompareSrcFilenames;
    focusOnExpIDsCheckBox = argFocusOnExpIDsCheckBox;
    int howManyButtons = compareExpIDs->size();
#if DEBUG_Choose
    printf("ChooseExperimentDialog::ChooseExperimentDialog, howManyButtons=%d, focusOnExpIDsCheckBox=%d\n", howManyButtons, focusOnExpIDsCheckBox);
#endif
    focusExpID = -1;


    mainLayout = new QVBoxLayout( this, 11, 6, "mainLayout");

    checkBoxLayout = new QVBoxLayout( 0, 11, 6, "checkBoxLayout");
    QLabel *checkBoxLabel = new QLabel( this, "check box label", 0 );
    checkBoxLabel->setText(tr("Please select an experiment to use for generation of the selected view."));
    checkBoxLabel->setMinimumSize( QSize(0,0) );
    checkBoxLabel->resize( checkBoxLabel->sizeHint() );
    checkBoxLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );
    checkBoxLayout->addWidget(checkBoxLabel);


    if (howManyButtons > 0) {
      ExpChoiceCheckBox1 = new QCheckBox( this, "ExpChoiceCheckBox1" );
      ExpChoiceCheckBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ExpChoiceCheckBox1->sizePolicy().hasHeightForWidth() ) );
      ExpChoiceCheckBox1->setText( tr( "Experiment Choice 1"  ) );
      ExpChoiceCheckBox1->setChecked( TRUE );
      checkBoxLayout->addWidget(ExpChoiceCheckBox1);
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ChooseExperimentDialog, adding ExpChoiceCheckBox1\n");
#endif
    }

    if (howManyButtons > 1) {
      ExpChoiceCheckBox2 = new QCheckBox( this, "ExpChoiceCheckBox2" );
      ExpChoiceCheckBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ExpChoiceCheckBox2->sizePolicy().hasHeightForWidth() ) );
      ExpChoiceCheckBox2->setText( tr( "Experiment Choice 2"  ) );
      ExpChoiceCheckBox2->setChecked( FALSE );
      checkBoxLayout->addWidget(ExpChoiceCheckBox2);
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ChooseExperimentDialog, adding ExpChoiceCheckBox2\n");
#endif
    }

    if (howManyButtons > 2) {
      ExpChoiceCheckBox3 = new QCheckBox( this, "ExpChoiceCheckBox3" );
      ExpChoiceCheckBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ExpChoiceCheckBox3->sizePolicy().hasHeightForWidth() ) );
      ExpChoiceCheckBox3->setText( tr( "Experiment Choice 3"  ) );
      ExpChoiceCheckBox3->setChecked( FALSE );
      checkBoxLayout->addWidget(ExpChoiceCheckBox3);
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ChooseExperimentDialog, adding ExpChoiceCheckBox3\n");
#endif
    }

    if (howManyButtons > 3) {
      ExpChoiceCheckBox4 = new QCheckBox( this, "ExpChoiceCheckBox4" );
      ExpChoiceCheckBox4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ExpChoiceCheckBox4->sizePolicy().hasHeightForWidth() ) );
      ExpChoiceCheckBox4->setText( tr( "Experiment Choice 4"  ) );
      ExpChoiceCheckBox4->setChecked( FALSE );
      checkBoxLayout->addWidget(ExpChoiceCheckBox4);
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ChooseExperimentDialog, adding ExpChoiceCheckBox4\n");
#endif
    }

    if (howManyButtons > 4) {
      ExpChoiceCheckBox5 = new QCheckBox( this, "ExpChoiceCheckBox5" );
      ExpChoiceCheckBox5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ExpChoiceCheckBox5->sizePolicy().hasHeightForWidth() ) );
      ExpChoiceCheckBox5->setText( tr( "Experiment Choice 5"  ) );
      ExpChoiceCheckBox5->setChecked( FALSE );
      checkBoxLayout->addWidget(ExpChoiceCheckBox5);
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ChooseExperimentDialog, adding ExpChoiceCheckBox5\n");
#endif
    }

    buttonLayout = new QHBoxLayout( 0, 0, 6, "buttonLayout");
    buttonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
    buttonLayout->addItem( buttonSpacer );


    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, buttonOk->sizePolicy().hasHeightForWidth() ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    buttonLayout->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, buttonCancel->sizePolicy().hasHeightForWidth() ) );
    buttonCancel->setAutoDefault( TRUE );

    // There really should not be a cancel for this.  The view will fail to generate if one doesn't 
    // select an experiment for the view to use.
    buttonCancel->hide();

    buttonLayout->addWidget( buttonCancel );

    mainLayout->addLayout( checkBoxLayout );

    mainLayout->addLayout( buttonLayout );

#if DEBUG_Choose
    printf("ChooseExperimentDialog::ChooseExperimentDialog, calling languageChange\n");
#endif
    languageChange();

}

void
ChooseExperimentDialog::languageChange()
{
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  connect( buttonOk, SIGNAL( clicked() ), this,
      SLOT( buttonOkSelected() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this,
      SLOT( reject() ) );

  int count = 0;
  std::vector<QString>::const_iterator itDB = compareExpDBNames->begin();
  std::vector<QString>::const_iterator itSF = compareSrcFilenames->begin();
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      count = count + 1;
      int exp_id = *it;
      QString dbName = *itDB;
      itDB++;
      QString sfName = *itSF;
      itSF++;
      char expIdStr[2048];
//      sprintf(expIdStr, "Experiment %d", exp_id);
      if (sfName != QString::null) {
        sprintf(expIdStr, "Experiment: %d, Database Name: %s\nSource Info: %s", exp_id, dbName.ascii(), sfName.ascii());
      } else {
        sprintf(expIdStr, "Experiment: %d, Database Name: %s", exp_id, dbName.ascii());
      }

#if DEBUG_Choose
      printf("ChooseExperimentDialog::languageChange, expIdStr=%s\n", expIdStr);
#endif

      if (count == 1) {
        if (exp_id == focusOnExpIDsCheckBox) {
          ExpChoiceCheckBox1->setChecked( TRUE );
        } else {
          ExpChoiceCheckBox1->setChecked( FALSE );
        }
        ExpChoiceCheckBox1->setText( tr( expIdStr ) );
        connect( ExpChoiceCheckBox1, SIGNAL( clicked() ), this, 
                 SLOT( ExpChoiceCheckBox1Selected() ) );
      } else if (count == 2) {
        if (exp_id == focusOnExpIDsCheckBox) {
          ExpChoiceCheckBox2->setChecked( TRUE );
        } else {
          ExpChoiceCheckBox2->setChecked( FALSE );
        }
        ExpChoiceCheckBox2->setText( tr( expIdStr ) );
        connect( ExpChoiceCheckBox2, SIGNAL( clicked() ), this, 
                 SLOT( ExpChoiceCheckBox2Selected() ) );
      } else if (count == 3) {
        if (exp_id == focusOnExpIDsCheckBox) {
          ExpChoiceCheckBox3->setChecked( TRUE );
        } else {
          ExpChoiceCheckBox3->setChecked( FALSE );
        }
        ExpChoiceCheckBox3->setText( tr( expIdStr ) );
        connect( ExpChoiceCheckBox3, SIGNAL( clicked() ), this, 
                 SLOT( ExpChoiceCheckBox3Selected() ) );
      } else if (count == 4) {
        if (exp_id == focusOnExpIDsCheckBox) {
          ExpChoiceCheckBox4->setChecked( TRUE );
        } else {
          ExpChoiceCheckBox4->setChecked( FALSE );
        }
        ExpChoiceCheckBox4->setText( tr( expIdStr ) );
        connect( ExpChoiceCheckBox4, SIGNAL( clicked() ), this, 
                 SLOT( ExpChoiceCheckBox4Selected() ) );
      } else if (count == 5) {
        if (exp_id == focusOnExpIDsCheckBox) {
          ExpChoiceCheckBox5->setChecked( TRUE );
        } else {
          ExpChoiceCheckBox5->setChecked( FALSE );
        }
        ExpChoiceCheckBox5->setText( tr( expIdStr ) );
        connect( ExpChoiceCheckBox5, SIGNAL( clicked() ), this, 
                 SLOT( ExpChoiceCheckBox5Selected() ) );
      } else {
      }

#if DEBUG_Choose
      printf("ChooseExperimentDialog::languageChange, exp_id=(%d), count=%d\n", exp_id, count);
#endif
  }
}


/*
 *  Destroys the object and frees any allocated resources
 */

ChooseExperimentDialog::~ChooseExperimentDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void
ChooseExperimentDialog::buttonCancelSelected()
{
    // no need to delete child widgets, Qt does it all for us
}

void
ChooseExperimentDialog::buttonOkSelected()
{
   if ( ExpChoiceCheckBox1->isChecked() ) {

#if DEBUG_Choose
       printf("ExpChoiceCheckBox1 is Checked\n");
#endif

       int count = 1;
       for( std::vector<int>::const_iterator it = compareExpIDs->begin();
            it != compareExpIDs->end(); it++ )
       {
           int exp_id = *it;
           focusExpID = exp_id;
#if DEBUG_Choose
           printf("ChooseExperimentDialog::ExpChoiceCheckBox1Selected, exp_id=(%d)\n", exp_id);
#endif
           if (count == 1) {
             break;
           } 
       }
//       ExpChoiceCheckBox1Selected();
   } else if ( ExpChoiceCheckBox2->isChecked()) {
#if DEBUG_Choose
       printf("ExpChoiceCheckBox2 is Checked\n");
#endif
       int count = 1;
       for( std::vector<int>::const_iterator it = compareExpIDs->begin();
            it != compareExpIDs->end(); it++ )
       {
           int exp_id = *it;
           if (count == 2) {
             focusExpID = exp_id;
#if DEBUG_Choose
             printf("ChooseExperimentDialog::ExpChoiceCheckBox2Selected, exp_id=(%d)\n", exp_id);
#endif
             break;
           }
           count = count + 1;
       }
//       ExpChoiceCheckBox2Selected();
   } else if ( ExpChoiceCheckBox3->isChecked()) {
#if DEBUG_Choose
       printf("ExpChoiceCheckBox3 is Checked\n");
#endif
       int count = 1;
       for( std::vector<int>::const_iterator it = compareExpIDs->begin();
            it != compareExpIDs->end(); it++ )
       {
           int exp_id = *it;
           if (count == 3) {
             focusExpID = exp_id;
#if DEBUG_Choose
             printf("ChooseExperimentDialog::ExpChoiceCheckBox3Selected, exp_id=(%d)\n", exp_id);
#endif
             break;
           }
           count = count + 1;
       }
//       ExpChoiceCheckBox3Selected();
   } else if ( ExpChoiceCheckBox4->isChecked()) {
#if DEBUG_Choose
       printf("ExpChoiceCheckBox4 is Checked\n");
#endif
       int count = 1;
       for( std::vector<int>::const_iterator it = compareExpIDs->begin();
            it != compareExpIDs->end(); it++ )
       {
           int exp_id = *it;
           if (count == 4) {
             focusExpID = exp_id;
#if DEBUG_Choose
             printf("ChooseExperimentDialog::ExpChoiceCheckBox4Selected, exp_id=(%d)\n", exp_id);
#endif
             break;
           }
           count = count + 1;
       }
//       ExpChoiceCheckBox4Selected();
   } else if ( ExpChoiceCheckBox5->isChecked()) {
#if DEBUG_Choose
       printf("ExpChoiceCheckBox5 is Checked\n");
#endif
       int count = 1;
       for( std::vector<int>::const_iterator it = compareExpIDs->begin();
            it != compareExpIDs->end(); it++ )
       {
           int exp_id = *it;
           if (count == 5) {
             focusExpID = exp_id;
#if DEBUG_Choose
             printf("ChooseExperimentDialog::ExpChoiceCheckBox5Selected, exp_id=(%d)\n", exp_id);
#endif
             break;
           }
           count = count + 1;
       }
//       ExpChoiceCheckBox5Selected();
   }

   accept();
}


void
ChooseExperimentDialog::ExpChoiceCheckBox1Selected()
{
#if DEBUG_Choose
  printf("ChooseExperimentDialog::ExpChoiceCheckBox1Selected, entered\n");
#endif
#if 0
  int count = 1;
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      int exp_id = *it;
      focusExpID = exp_id;
#if DEBUG_Choose
      printf("ChooseExperimentDialog::ExpChoiceCheckBox1Selected, exp_id=(%d)\n", exp_id);
#endif
      if (count == 1) {
        break;
      } 
  }
#else
    ExpChoiceCheckBox1->setChecked(TRUE);
    int howManyButtons = compareExpIDs->size();
    if (howManyButtons > 1) {
       ExpChoiceCheckBox2->setChecked(FALSE);
    }
    if (howManyButtons > 2) {
       ExpChoiceCheckBox3->setChecked(FALSE);
    }
    if (howManyButtons > 3) {
       ExpChoiceCheckBox4->setChecked(FALSE);
    }
    if (howManyButtons > 4) {
       ExpChoiceCheckBox5->setChecked(FALSE);
    }
#endif

}

void
ChooseExperimentDialog::ExpChoiceCheckBox2Selected()
{
#if DEBUG_Choose
  printf("ChooseExperimentDialog::ExpChoiceCheckBox2Selected, entered\n");
#endif
#if 0
  int count = 1;
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      int exp_id = *it;
      if (count == 2) {
        focusExpID = exp_id;
#if DEBUG_Choose
        printf("ChooseExperimentDialog::ExpChoiceCheckBox2Selected, exp_id=(%d)\n", exp_id);
#endif
        break;
      }
      count = count + 1;
  }
#else
    ExpChoiceCheckBox2->setChecked(TRUE);
    int howManyButtons = compareExpIDs->size();
    if (howManyButtons > 1) {
       ExpChoiceCheckBox1->setChecked(FALSE);
    }
    if (howManyButtons > 2) {
       ExpChoiceCheckBox2->setChecked(FALSE);
    }
    if (howManyButtons > 3) {
       ExpChoiceCheckBox4->setChecked(FALSE);
    }
    if (howManyButtons > 4) {
       ExpChoiceCheckBox5->setChecked(FALSE);
    }
#endif
}

void
ChooseExperimentDialog::ExpChoiceCheckBox3Selected()
{
#if DEBUG_Choose
  printf("ChooseExperimentDialog::ExpChoiceCheckBox3Selected, entered\n");
#endif
#if 0
  int count = 1;
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      int exp_id = *it;
      if (count == 3) {
        focusExpID = exp_id;
#if DEBUG_Choose
        printf("ChooseExperimentDialog::ExpChoiceCheckBox3Selected, exp_id=(%d)\n", exp_id);
#endif
        break;
      }
      count = count + 1;
  }
#else
    ExpChoiceCheckBox3->setChecked(TRUE);
    int howManyButtons = compareExpIDs->size();
    if (howManyButtons > 1) {
       ExpChoiceCheckBox1->setChecked(FALSE);
    }
    if (howManyButtons > 2) {
       ExpChoiceCheckBox2->setChecked(FALSE);
    }
    if (howManyButtons > 3) {
       ExpChoiceCheckBox3->setChecked(FALSE);
    }
    if (howManyButtons > 4) {
       ExpChoiceCheckBox5->setChecked(FALSE);
    }
#endif
}

void
ChooseExperimentDialog::ExpChoiceCheckBox4Selected()
{
#if DEBUG_Choose
  printf("ChooseExperimentDialog::ExpChoiceCheckBox4Selected, entered\n");
#endif
#if 0
  int count = 1;
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      int exp_id = *it;
      if (count == 4) {
        focusExpID = exp_id;
#if DEBUG_Choose
        printf("ChooseExperimentDialog::ExpChoiceCheckBox4Selected, exp_id=(%d)\n", exp_id);
#endif
        break;
      }
      count = count + 1;
  }
#else
    ExpChoiceCheckBox4->setChecked(TRUE);
    int howManyButtons = compareExpIDs->size();
    if (howManyButtons > 1) {
       ExpChoiceCheckBox1->setChecked(FALSE);
    }
    if (howManyButtons > 2) {
       ExpChoiceCheckBox2->setChecked(FALSE);
    }
    if (howManyButtons > 3) {
       ExpChoiceCheckBox3->setChecked(FALSE);
    }
    if (howManyButtons > 4) {
       ExpChoiceCheckBox5->setChecked(FALSE);
    }
#endif
}

void
ChooseExperimentDialog::ExpChoiceCheckBox5Selected()
{
#if DEBUG_Choose
  printf("ChooseExperimentDialog::ExpChoiceCheckBox5Selected, entered\n");
#endif
#if 0
  int count = 1;
  for( std::vector<int>::const_iterator it = compareExpIDs->begin();
       it != compareExpIDs->end(); it++ )
  {
      int exp_id = *it;
      if (count == 5) {
        focusExpID = exp_id;
#if DEBUG_Choose
        printf("ChooseExperimentDialog::ExpChoiceCheckBox5Selected, exp_id=(%d)\n", exp_id);
#endif
        break;
      }
      count = count + 1;
  }
#else
    ExpChoiceCheckBox5->setChecked(TRUE);
    int howManyButtons = compareExpIDs->size();
    if (howManyButtons > 1) {
       ExpChoiceCheckBox1->setChecked(FALSE);
    }
    if (howManyButtons > 2) {
       ExpChoiceCheckBox2->setChecked(FALSE);
    }
    if (howManyButtons > 3) {
       ExpChoiceCheckBox3->setChecked(FALSE);
    }
    if (howManyButtons > 4) {
       ExpChoiceCheckBox4->setChecked(FALSE);
    }
#endif
}
