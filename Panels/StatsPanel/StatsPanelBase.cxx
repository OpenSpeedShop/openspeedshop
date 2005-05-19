////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#include "StatsPanelBase.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove

#include <qvaluelist.h>
#include <qheader.h>
#include <qtooltip.h>
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;


#include "SPListView.hxx"   // Change this to your new class header file name
#include "SPListViewItem.hxx"   // Change this to your new class header file name
#include "UpdateObject.hxx"
#include "SourceObject.hxx"
#include "PreferencesChangedObject.hxx"


#include "MetricInfo.hxx" // dummy data only...
// This is only hear for the debugging tables....
static char *color_name_table[10] =
  { "red", "orange", "yellow", "skyblue", "green" };


StatsPanelBase::StatsPanelBase(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  setCaption("StatsPanelBase");

  groupID = (int)argument;

  metricHeaderTypeArray = NULL;


  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  headerLabel = new QLabel(getBaseWidgetFrame(), "headerLabel");
  frameLayout->addWidget(headerLabel);

  lv = NULL;
  
  getBaseWidgetFrame()->setCaption("StatsPanelBaseBaseWidget");

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), groupID);
  setName(name_buffer);
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanelBase::~StatsPanelBase()
{
  // Delete anything you new'd from the constructor.
}

void
StatsPanelBase::updateStatsPanelBaseData()
{
   // Read the new data, destroy the old data, and update the StatsPanelBase with
   // the new data.


  dprintf("updateStatsPanelBaseData() entered.\n");

  if( lv != NULL )
  {
    delete lv;
    lv = NULL;
  }

  if( lv == NULL )
  {
    lv = new SPListView( this, getBaseWidgetFrame(), getName(), 0 );
 
//    connect( lv, SIGNAL(clicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );
    connect( lv, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );

    lv->setAllColumnsShowFocus(TRUE);

    // If there are subitems, then indicate with root decorations.
    lv->setRootIsDecorated(TRUE);

    // If there should be sort indicators in the header, show them here.
    lv->setShowSortIndicator(TRUE);

    QToolTip::add( lv, tr( "The list of statistics collected." ) );
  }

  lv->clear();

  frameLayout->addWidget(lv);

  lv->show();
}

void
StatsPanelBase::languageChange()
{
  // Set language specific information here.
}

/*! This calls the user 'menu()' function
    if the user provides one.   The user can attach any specific panel
    menus to the passed argument and they will be displayed on a right
    mouse down in the panel.
    /param  contextMenu is the QPopupMenu * that use menus can be attached.
 */
bool
StatsPanelBase::menu(QPopupMenu* contextMenu)
{
  dprintf("StatsPanelBase::menu() entered.\n");
  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
StatsPanelBase::save()
{
  dprintf("StatsPanelBase::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
StatsPanelBase::saveAs()
{
  dprintf("StatsPanelBase::saveAs() requested.\n");
}

void
StatsPanelBase::preferencesChanged()
{ 
  dprintf("StatsPanelBase::preferencesChanged\n");
} 


/*! When a message has been sent (from anyone) and the message broker is
    notifying panels that they may want to know about the message, this is the
    function the broker notifies.   The listener then needs to determine
    if it wants to handle the message.
    \param msg is the incoming message.
    \return 0 means you didn't do anything with the message.
    \return 1 means you handled the message.
 */
int 
StatsPanelBase::listener(void *msg)
{
  dprintf("StatsPanelBase::listener() requested.\n");

  return 0;  // 0 means, did not want this message and did not act on anything.
}

/*! Create the context senstive menu for the report. */
bool
StatsPanelBase::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  dprintf ("StatsPanelBase: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");
  return( FALSE );
}

void
StatsPanelBase::compareSelected()
{
  dprintf("compareSelected()\n");
}

void
StatsPanelBase::setNumberVisibleEntries()
{
  dprintf("setNumberVisibleEntries()\n");
}

static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanelBase::doOption(int id)
{
  dprintf("doOption() id=%d\n", id);

  if( lv->columnWidth(id) )
  {
    cwidth = lv->columnWidth(id);
    lv->hideColumn(id);
  } else
  {
    lv->setColumnWidth(id, cwidth);
  }
}


/*! This is just a utility routine to truncate_name long names. */
char *
StatsPanelBase::truncateCharString(char *str, int length)
{
  char *newstr = NULL;
//  newstr = new char( length );
  newstr = (char *)calloc( length, sizeof(char)+3+1 );

  if( length > strlen(str) )
  {
    strcpy(newstr, str);
  } else
  {
    strcpy(newstr, "...");
    int extra = strlen(str)-length;
    strcat(newstr, str+extra);
    strcat(newstr, "");
  }

  return newstr;
}
