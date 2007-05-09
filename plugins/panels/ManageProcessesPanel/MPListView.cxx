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

//
//#define MPList_DEBUG 1
//

#include "MPListView.hxx"
#include "MPListViewItem.hxx"

#include "debug.hxx"

#include <qapplication.h>
#include <qevent.h>
#include <qpoint.h>
#include <qdragobject.h>
#include <qheader.h>
#include <qmessagebox.h>

MPListViewItem *MPListView::oldCurrent = 0;
bool MPListView::draggingFLAG = FALSE;

MPListView::MPListView( QWidget *parent, const char *name, WFlags f = 0)
     : QListView( parent, name, f),
       dropItem(0), mousePressed( FALSE )
{
#if MPList_DEBUG 
  printf("MPListView::MPListView()\n" );
#endif
  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops(TRUE);
  setShowSortIndicator(TRUE);
  setSorting(0, TRUE);
//  setSorting(1, TRUE);
}

/*! The default destructor. */
MPListView::~MPListView( )
{
#if MPList_DEBUG 
  printf("MPListView::~MPListView()\n" );
#endif
}


void MPListView::contentsDragEnterEvent( QDragEnterEvent *e )
{
#if MPList_DEBUG 
 printf("MPListView::contentsDragEnterEvent() \n");
#endif
  if( MPListView::oldCurrent == 0 )
  {
    return;
  }

  if( !QUriDrag::canDecode(e) )
  {
    e->ignore();
    return;
  }

  QListViewItem *i = itemAt( contentsToViewport(e->pos()) );
#if MPList_DEBUG 
  printf("MPListView::contentsDragEnterEvent() i->text().ascii()=%s\n", i->text(0).ascii() );
#endif
  if( i )
  {
    dropItem = i;
  }
}


void MPListView::contentsDragMoveEvent( QDragMoveEvent *e )
{
if( MPListView::oldCurrent == 0 )
{
  return;
}
  if( !QUriDrag::canDecode(e) )
  {
    e->ignore();
    return;
  }

  QPoint vp = contentsToViewport( ( (QDragMoveEvent*)e )->pos() );
  QListViewItem *i = itemAt( vp );
#if MPList_DEBUG 
  printf("MPListView::contentsDragMoveEvent() i->text().ascii()=%s\n", i->text(0).ascii() );
#endif
  if( i )
  {
    setSelected( i, TRUE );
    e->accept();
    if( i != dropItem )
    {
      dropItem = i;
    }
    switch ( e->action() )
    {
      case QDropEvent::Copy:
        break;
      case QDropEvent::Move:
        e->acceptAction();
        break;
      case QDropEvent::Link:
        e->acceptAction();
        break;
      default:
        ;
    }
  } else
  {
    e->ignore();
    dropItem = 0;
  }
}

void MPListView::contentsDragLeaveEvent( QDragLeaveEvent * )
{
#if MPList_DEBUG 
printf("MPListView::contentsDragLeaveEvent()\n");
#endif
  dropItem = 0;
}

void MPListView::contentsDropEvent( QDropEvent *e )
{
#if MPList_DEBUG 
 printf("           MPListView::contentsDropEvent() \n");
#endif
  if( MPListView::oldCurrent == 0 )
  {
    return;
  }

  if( !acceptDrops() || !viewport()->acceptDrops() )
  {
    mousePressed = FALSE;
    return;
  }

  if( !QUriDrag::canDecode(e) )
  {
    e->ignore();
    return;
  }

  MPListViewItem *item = (MPListViewItem *)itemAt( contentsToViewport(e->pos()) );
#if MPList_DEBUG 
  printf("MPListView::contentsDropEvent() item->text().ascii()=%s\n", item->text(0).ascii() );
#endif

  if( !item )
  {
      e->ignore();
      return;
  }


#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent()  accepted!\n");
 printf("MPListView::contentsDropEvent(), item->text(0)=(%s) MPListView::oldCurrent->text(0)=(%s)\n", item->text(0).ascii(),  MPListView::oldCurrent->text(0).ascii() );
#endif

  if( item->parent() )
  {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Item has parent\n");
#endif
    MPListViewItem *pi = (MPListViewItem *)item->parent();
    if( pi->parent() && pi->descriptionClassObject->root != TRUE )
    { // Nope, this is the leaf.  Drop it on the parent or ignore the drop..
      // For drop it on the parent.
      item = (MPListViewItem *)pi->parent();
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Reset item to parent!!!\n");
#endif
    }
  }
//  if( item->text(0).find("pset") == 0 || item->text(0).find("udpset") == 0 )
  if( item->parent() && item->descriptionClassObject->root == TRUE )
  { // Nope, this is the leaf.  Drop it on the parent or ignore the drop..
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Item has parent and it's a root.\n");
#endif
    // For drop it on the parent.
    QListViewItem *top = item;
    QListViewItem *last = top;
    while( top )
    {
      last = top;
      top = top->parent();
    }
    if( last->text(0) == CPS )
    {
      item = (MPListViewItem *)item->parent();
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), (CPS) Reset item to parent!!!\n");
#endif
    }
  }

  // Check to make sure we're not dropping it on the "static" dynamic 
  // pset list.   These are not user configurable.
  QListViewItem *top = item;
  QListViewItem *last = top;
  if( top->parent() == NULL && item->text(0) != CPS )
  {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Item has no parent and it's != CPS\n");
#endif
    e->ignore();
    return;
  }
  while( top )
  {
    last = top;
    top = top->parent();
  }
  if( last->text(0) == DPS )
  {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), it thinks it's a DPS\n");
#endif
    e->ignore();
    return;
  }

  // Check to see if we're dropping it on a leaf, but can figure out the
  // intended target.  Modify the target to enable the drop.
  if( item->parent() )
  {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Item has parent ... lets continue.\n");
#endif
    MPListViewItem *pitem = (MPListViewItem *)item->parent();
    if( pitem->parent() && pitem->descriptionClassObject->root == TRUE )
    {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), pitem has a parent! or it's a root.\n");
#endif
      item = (MPListViewItem *)pitem;
    } else if( pitem )
    {
#if 1 // Prevents double drop on leaf... but wasn't quite right.
      item = (MPListViewItem *)pitem;
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), item is now pitem\n");
#endif
#endif // 0
    }
    
  }


#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), We're here.  item(0)=(%s) last->text(0)=(%s)\n", item->text(0).ascii(), last->text(0).ascii() );
#endif
  if ( (item && item->parent()) || (item && last->text(0) == CPS) )
  {
    QStrList lst;

    QUriDrag::decode( e, lst );

    QString str;

    switch ( e->action() )
    {
      case QDropEvent::Copy:
        str = "Copy";
        break;
      case QDropEvent::Move:
        str = "Move";
        e->acceptAction();
        break;
      case QDropEvent::Link:
        str = "Link";
        e->acceptAction();
        break;
      default:
        str = "Unknown";
    }

    str += "\n\n";

    e->accept();


// printf("Drop (%s) on item=(%s)\n", MPListView::oldCurrent->text(0).ascii(), item->text(0).ascii() );
// for( int i=0; !MPListView::oldCurrent->text(i).isEmpty(); i++)
// {
// printf("field[%d]=(%s)\n", i, MPListView::oldCurrent->text(i).ascii() );
// }

    QString namedSet = QString::null;
    if( MPListView::oldCurrent->descriptionClassObject->root == TRUE )
    {
      namedSet = MPListView::oldCurrent->text(0)+"*";
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), We have a pset named!!   Just put out the pset name...\n");
#endif
    }

    // First check to see if it's already been added.
    if( isThisADuplicate( item ) )
    {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), It thinks this is a duplicate!\n");
#endif
      return;
    }

    if( MPListView::oldCurrent && !MPListView::oldCurrent->descriptionClassObject )
    {
      e->ignore();
    } else if( MPListView::oldCurrent && MPListView::oldCurrent->descriptionClassObject &&
        MPListView::oldCurrent->descriptionClassObject->root &&
        !MPListView::oldCurrent->descriptionClassObject->all )
    {
      // Loop through all the children...
      if( namedSet )
      {
        MPListViewItem *namedSetItem =
                new MPListViewItem( item, namedSet );
        DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, QString::null, QString::null, namedSet );
        namedSetItem->descriptionClassObject = dco;
        item = namedSetItem;
      }
      MPListViewItem *mpChild = (MPListViewItem *)MPListView::oldCurrent->firstChild();
      while( mpChild )
      {
        QString host_name = mpChild->descriptionClassObject->host_name;
        if( host_name.isEmpty() )
        {
          host_name = "localhost";
        }
        QString pid_name = mpChild->descriptionClassObject->pid_name;
        if( pid_name.isEmpty() )
        {
          mpChild = (MPListViewItem *)mpChild->nextSibling();
          continue;
        }

#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), DROP A:\n");
#endif
        MPListViewItem *item2 =
          new MPListViewItem( item, pid_name, host_name, mpChild->descriptionClassObject->rid_name );
        item2->descriptionClassObject = mpChild->descriptionClassObject;

        mpChild = (MPListViewItem *)mpChild->nextSibling();
      }
    } else
    {
      if( MPListView::oldCurrent && MPListView::oldCurrent->descriptionClassObject &&
          MPListView::oldCurrent->descriptionClassObject->all )
      {
        if( MPListView::oldCurrent->descriptionClassObject->root )
        {
          if( namedSet )
          {
            MPListViewItem *namedSetItem =
                    new MPListViewItem( item, namedSet );
            DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, QString::null, QString::null, namedSet );
            namedSetItem->descriptionClassObject = dco;
            item = namedSetItem;
          }
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), DROP B:\n");
#endif
          MPListViewItem *item2 =
            new MPListViewItem( item, MPListView::oldCurrent->firstChild()->text(0).ascii() );
          item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
        } else
        {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), DROP C:\n");
#endif
          MPListViewItem *item2 =
            new MPListViewItem( item, MPListView::oldCurrent->text(0).ascii() );
          item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
        }
      } else
      {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), DROP D:\n");
#endif
DescriptionClassObject *dco = MPListView::oldCurrent->descriptionClassObject;
#if 0
printf("Drop item:\n");
dco->Print();
#endif // 0
        MPListViewItem *item2 =
//          new MPListViewItem( item, MPListView::oldCurrent->descriptionClassObject->pid_name, MPListView::oldCurrent->descriptionClassObject->host_name, MPListView::oldCurrent->descriptionClassObject->rid_name, MPListView::oldCurrent->descriptionClassObject->tid_name );
          new MPListViewItem( item, dco->host_name, dco->pid_name, dco->rid_name, dco->tid_name);
        item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
      }
    }
  } else
  {
#if MPList_DEBUG 
 printf("MPListView::contentsDropEvent(), Awww shucks.   We have to ignore this one.\n");
#endif
    e->ignore();
  }
}

void MPListView::contentsMousePressEvent( QMouseEvent* e )
{
#if MPList_DEBUG 
  printf("MPListView::contentsMousePressEvent() entered\n");
#endif
  if( QString(name()) == "attachCollectorsListView" )
  {
    setSelectionMode(QListView::Multi);
  }

  QListView::contentsMousePressEvent(e);
  QPoint p( contentsToViewport( e->pos() ) );
  QListViewItem *i = itemAt( p );
#if MPList_DEBUG 
  printf("MPListView::contentsMousePressEvent() i->text().ascii()=%s\n", i->text(0).ascii() );
#endif
// Reset these on a select.
  MPListView::oldCurrent = 0;
  MPListView::draggingFLAG = FALSE;
  setCurrentItem(i);

  if( e->button() != QMouseEvent::LeftButton )
  {
#if MPList_DEBUG 
 printf("MPListView::contentsMousePressEvent(), Not a left mouse button... return\n");
#endif
    return;
  }

  if( i )
  {
    // if the user clicked into the root decoration of the item, don't try to start a drag!
    if( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
	     treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
	     p.x() < header()->cellPos( header()->mapToActual( 0 ) ) )
    {
      presspos = e->pos();
      mousePressed = TRUE;
    }
  }
}

void MPListView::contentsMouseMoveEvent( QMouseEvent* e )
{
#if MPList_DEBUG 
 printf("MPListView::contentsMouseMoveEvent() entered\n");
#endif
  if( mousePressed &&
      ( presspos - e->pos() ).manhattanLength() >
        QApplication::startDragDistance() )
  {
    mousePressed = FALSE;
    QListViewItem *item = itemAt( contentsToViewport(presspos) );
#if MPList_DEBUG 
    printf("MPListView::contentsMouseMoveEvent() item->text().ascii()=%s\n", item->text(0).ascii() );
#endif

//    if( item && item->parent() )
    if( item )
    {
#if MPList_DEBUG 
 printf("We have an item! (%s)\n", item->text(0).ascii() );
#endif
      MPListView::oldCurrent = (MPListViewItem *)item;
      MPListView::draggingFLAG = TRUE;
      if( MPListView::oldCurrent->descriptionClassObject == NULL )
      {
        MPListView::oldCurrent = NULL;
        MPListView::draggingFLAG = FALSE;
        return;
      }
#if 0
#if MPList_DEBUG 
printf("Drag item:\n");
#endif
MPListView::oldCurrent->descriptionClassObject->Print();
#endif // 0
      if( QString(name()) == "attachCollectorsListView" )
      {
        setSelectionMode(QListView::NoSelection);
      }
      QUriDrag* ud = new QUriDrag(viewport());
      ud->setUnicodeUris( "Barney-boy" );
      if( ud->drag() )
      {
        QMessageBox::information( this, "Drag source",
        QString("Delete "), "B: Not implemented" );
      }
    }
  }
}

void MPListView::contentsMouseReleaseEvent( QMouseEvent * )
{
#if MPList_DEBUG 
 printf("MPListView::contentsMouseReleaseEvent() entered\n");
#endif
  mousePressed = FALSE;
}

void MPListView::dragEnterEvent( QDragEnterEvent* e)
{
#if MPList_DEBUG 
 printf("MPListView::dragEnterEvent() entered\n");
#endif
  if( MPListView::oldCurrent == 0 )
  {
    return;
  }

  if ( !QUriDrag::canDecode(e) )
  {
    e->ignore();
    return;
  }


  QListViewItem *i = itemAt( contentsToViewport(e->pos()) );
#if MPList_DEBUG 
  printf("MPListView::dragEnterEvent() i->text().ascii()=%s\n", i->text(0).ascii() );
#endif
  if ( i )
  {
      dropItem = i;
  }
}

void MPListView::sort ()
{
#if MPList_DEBUG 
    printf("MPListView::sort () Entered\n" );
#endif
    QListView::sort();
}

bool
MPListView::isThisADuplicate(MPListViewItem *item)
{
#if MPList_DEBUG 
 printf("isThisADuplicate(%s %s) entered\n", MPListView::oldCurrent->text(0).ascii(), item->text(0).ascii() );
#endif

  // If this is a named pset, look for another named pset that's already there.
  QString namedSet = QString::null;
  if( MPListView::oldCurrent->descriptionClassObject->root == TRUE )
  {
    namedSet = MPListView::oldCurrent->text(0)+"*";
#if MPList_DEBUG 
 printf("MPListView::isThisADuplicate, this (%s) is a named pset!\n", namedSet.ascii() );
#endif
    QListViewItem *nitem = NULL;
    nitem = item->firstChild();
    while( nitem )
    {
#if MPList_DEBUG 
 printf("MPListView::isThisADuplicate,   (%s)\n", nitem->text(0).ascii() );
#endif
      if( nitem->text(0) == namedSet )
      {
#if MPList_DEBUG 
 printf("MPListView::isThisADuplicate, We have a duplicate namedSet\n");
#endif
        return(TRUE);
      }
      nitem = nitem->nextSibling();
    }
  }


  MPListViewItem *nitem = (MPListViewItem *)item->firstChild();
  while( nitem )
  {
    if( MPListView::oldCurrent &&
        MPListView::oldCurrent->descriptionClassObject &&
        nitem &&
        nitem->descriptionClassObject )
    {
      DescriptionClassObject *n = nitem->descriptionClassObject;
      DescriptionClassObject *o = MPListView::oldCurrent->descriptionClassObject;
      if( o->host_name == n->host_name &&
        o->pid_name == n->pid_name &&
        o->rid_name == n->rid_name &&
        o->tid_name == n->tid_name &&
        o->collector_name == n->collector_name &&
        o->all == n->all &&
        o->root == n->root )
      {  
#if MPList_DEBUG 
 printf("MPListView::isThisADuplicate, Don't add duplicates... just ignore.\n");
#endif
        return( TRUE );
      }
    }
    nitem = (MPListViewItem *)nitem->nextSibling();
  }


  return(FALSE);
}
