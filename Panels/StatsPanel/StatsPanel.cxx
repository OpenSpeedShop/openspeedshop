#include "StatsPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


#include "SPListView.hxx"   // Change this to your new class header file name
#include "SourceObject.hxx"


#include "FuncInfo.hxx" // dummy data only...
// This is only hear for the debugging tables....
static char *color_name_table[10] =
  { "red", "orange", "yellow", "skyblue", "green" };



StatsPanel::StatsPanel(PanelContainer *pc, const char *n, char *argument) : Panel(pc, n)
{
  setCaption("StatsPanel");

  numberItemsToRead = 5;

  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  lv = NULL;
  
  getBaseWidgetFrame()->setCaption("StatsPanelBaseWidget");
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
StatsPanel::~StatsPanel()
{
  // Delete anything you new'd from the constructor.
}

void
StatsPanel::itemSelected(QListViewItem *item)
{
  printf("StatsPanel::clicked() entered\n");

if( item )
{
  printf("  item->depth()=%d\n", item->depth() );

  QListViewItem *nitem = item;
  while( nitem->parent() )
  {
printf("looking for 0x%x\n", nitem->parent() );
    nitem = nitem->parent();
  }
  

  if( nitem )
  {
    printf("here's the parent! 0x%x\n", nitem);
    printf("  here's the rank of that parent: rank = %s\n", nitem->text(1).ascii() );
    matchSelectedItem( atoi( nitem->text(1).ascii() ) );
  }
}
}


void
StatsPanel::matchSelectedItem(int element)
{
  printf ("TopPanel::matchSelectedItem() = %d\n", element );

  int i = 0;
  HighlightList *highlightList = new HighlightList();
  highlightList->clear();
  HighlightObject *hlo = NULL;

  FuncInfo *fi = NULL;
  FuncInfoList::Iterator it = NULL;

  i = 0;
  for( it = experimentData->funcInfoList.begin();
       it != experimentData->funcInfoList.end();
       it++ )
  {
    fi = (FuncInfo *)*it;
    for( int line=fi->start; line <= fi->end; line++)
    {
      if( i >= 5 )
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[4], "exclusive time");
      } else
      {
        hlo = new HighlightObject(fi->fileName, line, color_name_table[i], "exclusive time");
      }
// fprintf(stderr, "  pushback hlo: line=%d in color (%s)\n", line, hlo->color);
      highlightList->push_back(hlo);
    }
    i++;
  }


  i = 0;
  for( it = experimentData->funcInfoList.begin();
       it != experimentData->funcInfoList.end();
       it++ )
  {
     fi = (FuncInfo *)*it;
     if( i == element )
     {
       break;
     }
     i++;
  }

  printf ("%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  
  char msg[1024];
  sprintf(msg, "%d (%s) (%s) (%d)\n", element, fi->functionName, fi->fileName, fi->function_line_number );
  

  SourceObject *spo = new SourceObject(fi->functionName, fi->fileName, fi->function_line_number, TRUE, highlightList);



  if( broadcast((char *)spo, NEAREST_T) == 0 )
  { // No source view up...
    char *panel_type = "Source Panel";
    Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type);
//    if( i > 0 ) 
    if( p != NULL ) 
    {
//      broadcast((char *)spo);
      p->listener((void *)spo);
    }
  }
}


void
StatsPanel::updateStatsPanelData()
{
// Read the new data, destroy the old data, and update the StatsPanel with
// the new data.


printf("updateStatsPanelData() enterd.\n");
int number_entries = getUpdatedData(numberItemsToRead);
  if( lv != NULL )
  {
    delete lv;
    lv = NULL;
  }

  if( lv == NULL )
  {
    lv = new SPListView( this, getBaseWidgetFrame(), getName(), 0 );
 
    connect( lv, SIGNAL(clicked(QListViewItem *)), this, SLOT( itemSelected( QListViewItem* )) );
  }

  lv->clear();


  QListViewItem *lvi;

// First delete the old column list.  (also used for dynamic menus)
  columnList.clear();
  for( int i=0; i<COLUMNS_OF_INFO; i++ )
  {
    lv->addColumn( header_strings[i] );
    columnList.push_back( QString(header_strings[i]) );
  }

  FuncInfo *fi;
  char buffer[1024];
  char rankstr[10];
  char filestr[21];
  char funcstr[21];
  int i = 0;
  for( FuncInfoList::Iterator it = experimentData->funcInfoList.begin();
       it != experimentData->funcInfoList.end();
       it++ )
  {
    if( i >= numberItemsToRead )
    {
      break;
    }
    fi = (FuncInfo *)*it;

    dprintf("fi->functionName=(%s)\n", fi->functionName );
    char *ptr = NULL;
    ptr = truncateCharString(fi->functionName, 17);
    strcpy(funcstr, ptr);
    free(ptr);
    ptr = truncateCharString(fi->fileName, 17);
    strcpy(filestr, ptr);
    free(ptr);
    sprintf(rankstr, "%d", fi->index);
//     sprintf(buffer, "%-9s %-15s  %3.3f   %-20s  %d\n", rankstr, funcstr, fi->percent, filestr, fi->function_line_number);

    char percentstr[10];
    char exclusivestr[20];
    char startlinenostr[20];
    char endlinenostr[20];
    sprintf(percentstr, "%f", fi->percent);
    sprintf(exclusivestr, "%f", fi->exclusive_seconds);
    sprintf(startlinenostr, "%d", fi->start);
    sprintf(endlinenostr, "%d", fi->end);
    lvi=  new QListViewItem( lv, percentstr, rankstr, exclusivestr, funcstr, filestr, startlinenostr, endlinenostr );
      lvi = new QListViewItem( lvi, "SubText", QString("Additional Text for Rank ")+QString(rankstr) );
        (void)new QListViewItem( lvi, "SubSubText", QString("Additional Text for Rank ")+QString(rankstr) );
    i++;
  }

  frameLayout->addWidget(lv);

  lv->show();
}
void
StatsPanel::languageChange()
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
StatsPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("StatsPanel::menu() requested.\n");
  contextMenu->insertSeparator();

  contextMenu->insertItem("Set number visible entries...", this, SLOT(setNumberVisibleEntries()), CTRL+Key_1, 0, -1);

  int id = 0;
  QPopupMenu *columnsMenu = new QPopupMenu(this);
  columnsMenu->setCaption("Columns Menu");
  contextMenu->insertItem("&Columns Menu", columnsMenu, CTRL+Key_C);

  for( ColumnList::Iterator pit = columnList.begin();
           pit != columnList.end();
           ++pit )
  { 
    QString s = (QString)*pit;
    columnsMenu->insertItem(s, this, SLOT(doOption(int)), CTRL+Key_1, id, -1);
    if( lv->columnWidth(id) )
    {
      columnsMenu->setItemChecked(id, TRUE);
    } else
    {
      columnsMenu->setItemChecked(id, FALSE);
    }
    id++;
  }

//  contextMenu->insertItem("Export Report Data...", this, NULL, NULL);
  contextMenu->insertItem("Export Report Data...");

  return( TRUE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
StatsPanel::save()
{
  dprintf("StatsPanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
StatsPanel::saveAs()
{
  dprintf("StatsPanel::saveAs() requested.\n");
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
StatsPanel::listener(void *msg)
{
  printf("StatsPanel::listener() requested.\n");

// BUG - BIG TIME KLUDGE.   This should have a message type.
  MessageObject *mo = (MessageObject *)msg;
  if(  mo->msgType  == "UpdateAllObject" )
  {
    updateStatsPanelData();
  }

  return 0;  // 0 means, did not want this message and did not act on anything.
}

/*! Create the context senstive menu for the report. */
bool
StatsPanel::createPopupMenu( QPopupMenu* contextMenu, const QPoint &pos )
{
  dprintf ("StatsPanel: Popup the context sensitive menu here.... can you augment it with the default popupmenu?\n");
  
  dprintf("selected item = %d\n", lv->selectedItem() );

  QPopupMenu *panelMenu = new QPopupMenu(this);
  panelMenu->setCaption("Panel Menu");
  contextMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_C);
  panelMenu->insertSeparator();
  menu(panelMenu);

  if( lv->selectedItem() )
  {
  //  contextMenu->insertItem("Tell Me MORE about %d!!!", this, SLOT(details()), CTRL+Key_1 );
    contextMenu->insertItem("Go to source location...", this, SLOT(gotoSource()), CTRL+Key_1 );
    return( TRUE );
  }
  

  return( FALSE );
}


void
StatsPanel::gotoSource()
{
  dprintf("gotoSource() menu selected.\n");
}

void
StatsPanel::setNumberVisibleEntries()
{
  dprintf("setNumberVisibleEntries()\n");
{
  bool ok;
  QString text = QInputDialog::getText(
          "Visible Lines", "Enter number visible lines:", QLineEdit::Normal,
          QString::null, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    numberItemsToRead = atoi(text.ascii());
    printf ("numberItemsToRead=%d\n", numberItemsToRead);
    updateStatsPanelData();
  } else
  {
    // user entered nothing or pressed Cancel
  }
}
}

static int cwidth = 0;  // This isn't what I want to do long term.. 
void
StatsPanel::doOption(int id)
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
StatsPanel::truncateCharString(char *str, int length)
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

// This routine needs to be rewritten when we really get the framework 
// round trip written.
int
StatsPanel::getUpdatedData(int num_entries_to_read)
{
  int values[1024];
  char *color_names[1024];
  char *strings[1024];

  experimentData = new ExprInfo();

  int number_returned = getValues(values, color_names, strings, num_entries_to_read);
  if( number_returned == 0 )
  {
    return 0;
  }

  return( number_returned );
}

// This routine will either go away or be rewritten.

enum LABEL_TYPE  { PERCENT_T, FUNCTION_NAME_T, RANK_T, NONE_T };
/*! Get the values from the experimentData. */
int
StatsPanel::getValues(int values[], char *color_names[], char *strings[], int n)
{
// LABEL_TYPE lt = PERCENT_T;
LABEL_TYPE lt = NONE_T;

  int i=0;
  float sum = 0.0;

  FuncInfo *fi = NULL;

  if( !experimentData )
  {
    return 0;
  }

  for( FuncInfoList::Iterator it = experimentData->funcInfoList.begin();
       it != experimentData->funcInfoList.end();
       it++ )
  {
values[i] = 0;
color_names[i] = "";
strings[i] = "";
    if( i == n-1 )
//    if( i == n )
    {
      break;
    }
    if( i >= 5 )
    {
      color_names[i] = color_name_table[4];
    } else
    {
      color_names[i] = color_name_table[i];
    }
    fi = (FuncInfo *)*it;
    values[i] = (int)fi->percent;

    if( lt == FUNCTION_NAME_T )
    {
      char *ptr = strchr(fi->functionName, ':');
     if( ptr )
      {
        strings[i] = ptr+=2;
      } else
      {
        strings[i] = fi->functionName;
      }
    } else if( lt == RANK_T )
    {
      char rank_buffer[10];
      sprintf(rank_buffer, "%d", i+1);
      strings[i] = strdup(rank_buffer);   // leak   FIX
    } else if( lt == PERCENT_T )
    {
      char percent_buffer[10];
      sprintf(percent_buffer, "%.2f", fi->percent);
      strings[i] = strdup(percent_buffer);   // leak   FIX
    } else   // NONE_T
    {
      strings[i] = strdup(""); // leak   FIX
    }
    sum+= values[i];
    i++;
  }
  values[i] = (int)(100-sum);
  if( lt != NONE_T )
  {
    strings[i] = strdup("other...");   // leak   FIX
  }
  if( i > 5 )
  {
    color_names[i] = color_name_table[4];
  } else
  {
    color_names[i] = color_name_table[i];
  }

  return(i+1);
}
