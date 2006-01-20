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
  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops(TRUE);
}

/*! The default destructor. */
MPListView::~MPListView( )
{
}


void MPListView::contentsDragEnterEvent( QDragEnterEvent *e )
{
// printf("MPListView::contentsDragEnterEvent() \n");
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
//printf("MPListView::contentsDragLeaveEvent()\n");
  dropItem = 0;
}

void MPListView::contentsDropEvent( QDropEvent *e )
{
// printf("           MPListView::contentsDropEvent() \n");
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

  if( !item )
  {
      e->ignore();
      return;
  }

if( item->parent() )
{
  MPListViewItem *pi = (MPListViewItem *)item->parent();
  if( pi->text(0).find("pset") == 0 || pi->text(0).find("upset") == 0 )
  { // Nope, this is the leaf.  Drop it on the parent or ignore the drop..
    // For drop it on the parent.
    item = (MPListViewItem *)pi->parent();
  }
}
if( item->text(0).find("pset") == 0 || item->text(0).find("upset") == 0 )
{ // Nope, this is the leaf.  Drop it on the parent or ignore the drop..
  // For drop it on the parent.
  item = (MPListViewItem *)item->parent();
}

  // Check to make sure we're not dropping it on the "static" dynamic 
  // pset list.   These are not user configurable.
  QListViewItem *top = item;
  QListViewItem *last = top;
  if( top->parent() == NULL && item->text(0) != CPS )
  {
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
    e->ignore();
    return;
  }

  // Check to see if we're dropping it on a leaf, but can figure out the
  // intended target.  Modify the target to enable the drop.
  if( item->parent() )
  {
    MPListViewItem *pitem = (MPListViewItem *)item->parent();
    if( pitem->parent() && pitem->descriptionClassObject->root == TRUE )
    {
      item = (MPListViewItem *)pitem;
    }
  }


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
    if( MPListView::oldCurrent->text(0).find("pset") == 0 || MPListView::oldCurrent->text(0).find("upset") == 0 )
    {
      namedSet = MPListView::oldCurrent->text(0)+"*";
// printf("We have a pset named!!   Just put out the pset name...\n");
    }

    // First check to see if it's already been added.
    if( isThisADuplicate( item ) )
    {
// printf("It thinks this is a duplicate!\n");
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
        DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, namedSet );
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

        MPListViewItem *item2 =
          new MPListViewItem( item, pid_name, host_name );
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
            DescriptionClassObject *dco = new DescriptionClassObject(TRUE, QString::null, QString::null, QString::null, namedSet );
            namedSetItem->descriptionClassObject = dco;
            item = namedSetItem;
          }
          MPListViewItem *item2 =
            new MPListViewItem( item, MPListView::oldCurrent->firstChild()->text(0).ascii() );
          item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
        } else
        {
          MPListViewItem *item2 =
            new MPListViewItem( item, MPListView::oldCurrent->text(0).ascii() );
          item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
        }
      } else
      {
        MPListViewItem *item2 =
          new MPListViewItem( item, MPListView::oldCurrent->descriptionClassObject->pid_name, MPListView::oldCurrent->descriptionClassObject->host_name );
        item2->descriptionClassObject = MPListView::oldCurrent->descriptionClassObject;
      }
    }
  } else
  {
    e->ignore();
  }
}

void MPListView::contentsMousePressEvent( QMouseEvent* e )
{
//printf("MPListView::contentsMousePressEvent() entered\n");
  if( QString(name()) == "attachCollectorsListView" )
  {
    setSelectionMode(QListView::Multi);
  }

  QListView::contentsMousePressEvent(e);
  QPoint p( contentsToViewport( e->pos() ) );
  QListViewItem *i = itemAt( p );

// Reset these on a select.
  MPListView::oldCurrent = 0;
  MPListView::draggingFLAG = FALSE;
  setCurrentItem(i);

  if( e->button() != QMouseEvent::LeftButton )
  {
// printf("Not a left mouse button... return\n");
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
// printf("MPListView::contentsMouseMoveEvent() entered\n");
  if( mousePressed &&
      ( presspos - e->pos() ).manhattanLength() >
        QApplication::startDragDistance() )
  {
    mousePressed = FALSE;
    QListViewItem *item = itemAt( contentsToViewport(presspos) );

//    if( item && item->parent() )
    if( item )
    {
// printf("We have an item! (%s)\n", item->text(0).ascii() );
      MPListView::oldCurrent = (MPListViewItem *)item;
      MPListView::draggingFLAG = TRUE;
      if( MPListView::oldCurrent->descriptionClassObject == NULL )
      {
        MPListView::oldCurrent = NULL;
        MPListView::draggingFLAG = FALSE;
        return;
      }
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
// printf("MPListView::contentsMouseReleaseEvent() entered\n");
  mousePressed = FALSE;
}

void MPListView::dragEnterEvent( QDragEnterEvent* e)
{
// printf("MPListView::dragEnterEvent() entered\n");
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
  if ( i )
  {
      dropItem = i;
  }
}

bool
MPListView::isThisADuplicate(MPListViewItem *item)
{
// printf("isThisADuplicate(%s %s) entered\n", MPListView::oldCurrent->text(0).ascii(), item->text(0).ascii() );

  // If this is a named pset, look for another named pset that's already there.
  QString namedSet = QString::null;
  if( MPListView::oldCurrent->text(0).find("pset") == 0 ||
      MPListView::oldCurrent->text(0).find("upset") == 0 )
  {
    namedSet = MPListView::oldCurrent->text(0)+"*";
// printf("this (%s) is a named pset!\n", namedSet.ascii() );
    QListViewItem *nitem = NULL;
    nitem = item->firstChild();
    while( nitem )
    {
// printf("  (%s)\n", nitem->text(0).ascii() );
      if( nitem->text(0) == namedSet )
      {
// printf("We have a duplicate namedSet\n");
        return(TRUE);
      }
      nitem = nitem->nextSibling();
    }
  }


  QListViewItem *nitem = item->firstChild();
  while( nitem )
  {
    if( MPListView::oldCurrent &&
        MPListView::oldCurrent->descriptionClassObject &&
        nitem->text(0) == MPListView::oldCurrent->descriptionClassObject->pid_name &&
        nitem->text(1) == MPListView::oldCurrent->descriptionClassObject->host_name )
    {  
// printf("Don't add duplicates... just ignore.\n");
      return( TRUE );
    }
    nitem = nitem->nextSibling();
  }


  return(FALSE);
}

