/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


static QWidget *lastPageWidget=NULL;
void WizardForm::wizardPageSlot()
{
 printf("Someone called wizardPageSlot()\n");
 
#ifndef DOESNT_WORK_LIKE_I_WANT
 QWidget *currentPageWidget = currentPage();
 if( currentPageWidget == page1 )
 {
   printf("Page 1 was initialized...\n");
   setBackEnabled(page2, TRUE);
   setNextEnabled(page2, TRUE);
 } else if( currentPageWidget == page2 )
 {
   printf("Page 2 was selected...\n");
   setBackEnabled(page3, TRUE);
   setNextEnabled(page3, TRUE);
 } else if( currentPageWidget == page3 )
 {
   printf("Page 3 was selected...\n");
   setBackEnabled(page4, TRUE);
   setNextEnabled(page4, TRUE);
 } else if( currentPageWidget == page4 )
 {
   printf("Page 4 was selected!...\n");
   setBackEnabled(page4, TRUE);
   setNextEnabled(page4, FALSE);
   setFinishEnabled(page4, TRUE);

#ifdef OLDWAY
   if( lastPageWidget == currentPageWidget )
   {
     extern void finishedProfileScenario();
     finishedProfileScenario();
     this->hide();
     delete this;
   }
#endif // OLDWAY
 } else
 {
   printf("Some other page was selected\n");
   setBackEnabled(page1, FALSE);
   setNextEnabled(page1, TRUE);
   setFinishEnabled(page1, FALSE);
 }

 lastPageWidget = currentPageWidget;
#endif // DOESNT_WORK_LIKE_I_WANT
 
}

void finishedProfileScenario()
{

  printf("finishedProfileScenario() entered.\n");
}



void WizardForm::selected( const QString & )
{
    printf("WizardForm::selected() entered\n");
}
