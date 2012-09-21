////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


#include <cstddef>
#include "canvasellipse.hxx"
#include "canvasshadowellipse.hxx"
#include "canvasrectangle.hxx"
#include "canvastext.hxx"
#include "chartform.hxx"

#include <qbrush.h>
#include <qcanvas.h>

#include <math.h> // sin, cos

#include <qapplication.h>

#ifndef M_PI
#define M_PI 3.1415
#endif

#include "debug.hxx"

void ChartForm::drawElements()
{
    QCanvasItemList list = m_canvas->allItems();
    for ( QCanvasItemList::iterator it = list.begin(); it != list.end(); ++it )
    {
	  delete *it;
    }

	// 360 * 16 for pies; Qt works with 16ths of degrees
    int scaleFactor = m_chartType == PIEWITHSHADOW || m_chartType == PIEWITHNOSHADOW || m_chartType == PIEWITH3D ? 5760 :
			m_chartType == VERTICAL_BAR || m_chartType == BARWITH3D ? (int)(m_canvas->height()*.66) : (int)(m_canvas->width()*.66);
    double biggest = 0.0;
    int count = 0;
    double total = 0.0;
    static double scales[MAX_ELEMENTS];

    for ( int i = 0; i < MAX_ELEMENTS; ++i ) {
	if ( m_elements[i].isValid() ) {
	    double value = m_elements[i].value();
	    count++;
	    total += value;
	    if ( value > biggest )
		biggest = value;
	    scales[i] = m_elements[i].value() * scaleFactor;
	}
    }

    if ( count ) {
	    // 2nd loop because of total and biggest
	for ( int i = 0; i < MAX_ELEMENTS; ++i )
    {
	    if ( m_elements[i].isValid() )
        {
		  if ( m_chartType == PIEWITHSHADOW || m_chartType == PIEWITHNOSHADOW || m_chartType == PIEWITH3D )
          {
		    scales[i] = (m_elements[i].value() * scaleFactor) / total;
          } else
          {
		    scales[i] = (m_elements[i].value() * scaleFactor) / biggest;
          }
        }
     }

	switch ( m_chartType ) {
	    case PIEWITHSHADOW:
	    case PIEWITHNOSHADOW:
	    case PIEWITH3D:
		drawPieChart( scales, total, count );
		break;
	    case BARWITH3D:
		draw3DBarChart( scales, total, count );
        break;
	    case VERTICAL_BAR:
		drawVerticalBarChart( scales, total, count );
		break;
	    case HORIZONTAL_BAR:
		drawHorizontalBarChart( scales, total, count );
		break;
	}
    }

    m_canvas->update();
}


void ChartForm::drawPieChart( const double scales[], double total, int count)
{
//    float pitch = .35;
    float pitch = .90; // Makes the chart 2D (flat)
    int margin = 10;
    double width = m_canvas->width()-margin;
    double height = m_canvas->height()-margin;
    int size = int(width > height ? height : width);
    int x = int(width / 2);
    int y = int(height / 2);
    x+=(margin/2);
    y+=(margin/2);
    int angle = 0;

#ifdef OLDWAY
if( m_chartType == PIEWITHSHADOW )
{
    int shadow_angle = 3;
    int depth = 12;

    for ( int i = 0; i < count; ++i )
    {
	if ( m_elements[i].isValid() )
    {
	    int extent = int(scales[i]);
	    CanvasShadowEllipse *arc = new CanvasShadowEllipse(
					    i, size, (int)(size*pitch), angle, extent, m_canvas );
//	    QCanvasEllipse *arc = new QCanvasEllipse(
//					    size, size*pitch, angle, extent, m_canvas );
	    arc->setX( x+shadow_angle );
	    arc->setY( y+depth );
	    arc->setZ( 0 );
	    arc->setBrush( QBrush( QColor(Qt::black),
				   BrushStyle(1)) );
	    arc->show();
	    angle += extent;
	  }
    }
}
#endif // OLDWAY

    margin = 10;
    width = m_canvas->width()-margin;
    height = m_canvas->height()-margin;
    size = int(width > height ? height : width);
    x = int(width / 2);
    y = int(height / 2);
    x+=(margin/2);
    y+=(margin/2);
    angle = 0;

    for ( int i = 0; i < count; ++i )
    {
	if ( m_elements[i].isValid() )
    {
	    int extent = int(scales[i]);
	    CanvasEllipse *arc = new CanvasEllipse(
					    i, size, (int)(size*pitch), angle, extent, m_canvas );
	    arc->setX( x );
	    arc->setY( y );
	    arc->setZ( 0 );
	    arc->setBrush( QBrush( m_elements[i].valueColor(),
				   BrushStyle(m_elements[i].valuePattern()) ) );
	    arc->show();
	    angle += extent;
// #ifndef TEXT_WANTED
// printf("Pie chart text wanted.\n");
	    QString label = m_elements[i].label();
	    if ( !label.isEmpty() || m_addValues != NO ) {
		label = valueLabel( label, m_elements[i].value(), total );
		CanvasText *text = new CanvasText( i, label, m_font, m_canvas );
		double proX = m_elements[i].proX( PIEWITHSHADOW );
		double proY = m_elements[i].proY( PIEWITHSHADOW );
		if ( proX < 0 || proY < 0 ) {
		    // Find the centre of the pie segment
		    QRect rect = arc->boundingRect();
		    proX = ( rect.width() / 2 ) + rect.x();
		    proY = ( rect.height() / 2 ) + rect.y();
		    // Centre text over the centre of the pie segment
		    rect = text->boundingRect();
		    proX -= ( rect.width() / 2 );
		    proY -= ( rect.height() / 2 );
		    // Make proportional
		    proX /= width;
		    proY /= height;
		}
		text->setColor( m_elements[i].labelColor() );
		text->setX( proX * width );
		text->setY( proY * height );
		text->setZ( 1 );
		text->show();
		m_elements[i].setProX( PIEWITHSHADOW, proX );
		m_elements[i].setProY( PIEWITHSHADOW, proY );
	    }
// #endif // TEXT_WANTED
	  }
    }
}


void ChartForm::drawVerticalBarChart(
	const double scales[], double total, int count )
{
    double width = m_canvas->width();
    double height = m_canvas->height();
    int prowidth = int(width / count);
    int x = 0;
    QPen pen;
    pen.setStyle( NoPen );

    for ( int i = 0; i < MAX_ELEMENTS; ++i ) {
	if ( m_elements[i].isValid() ) {
	    int extent = int(scales[i]);
	    int y = int(height - extent);
	    CanvasRectangle *rect = new CanvasRectangle(
					    i, x, y, prowidth, extent, m_canvas );
	    rect->setBrush( QBrush( m_elements[i].valueColor(),
				    BrushStyle(m_elements[i].valuePattern()) ) );
	    rect->setPen( pen );
	    rect->setZ( 0 );
	    rect->show();
	    QString label = m_elements[i].label();
	    if ( !label.isEmpty() || m_addValues != NO ) {
		double proX = m_elements[i].proX( VERTICAL_BAR );
		double proY = m_elements[i].proY( VERTICAL_BAR );
		if ( proX < 0 || proY < 0 ) {
		    proX = x / width;
		    proY = y / height;
		}
		label = valueLabel( label, m_elements[i].value(), total );
		CanvasText *text = new CanvasText( i, label, m_font, m_canvas );
		text->setColor( m_elements[i].labelColor() );
		text->setX( proX * width );
		text->setY( proY * height );
		text->setZ( 1 );
		text->show();
		m_elements[i].setProX( VERTICAL_BAR, proX );
		m_elements[i].setProY( VERTICAL_BAR, proY );
	    }
	    x += prowidth;
	}
    }
}


void ChartForm::drawHorizontalBarChart(
	const double scales[], double total, int count )
{
    double width = m_canvas->width();
    double height = m_canvas->height();
    int proheight = int(height / count);
    int y = 0;
    QPen pen;
    pen.setStyle( NoPen );

    for ( int i = 0; i < MAX_ELEMENTS; ++i ) {
	if ( m_elements[i].isValid() ) {
	    int extent = int(scales[i]);
	    CanvasRectangle *rect = new CanvasRectangle(
					    i, 0, y, extent, proheight, m_canvas );
	    rect->setBrush( QBrush( m_elements[i].valueColor(),
				    BrushStyle(m_elements[i].valuePattern()) ) );
	    rect->setPen( pen );
	    rect->setZ( 0 );
	    rect->show();
	    QString label = m_elements[i].label();
	    if ( !label.isEmpty() || m_addValues != NO ) {
		double proX = m_elements[i].proX( HORIZONTAL_BAR );
		double proY = m_elements[i].proY( HORIZONTAL_BAR );
		if ( proX < 0 || proY < 0 ) {
		    proX = 0;
		    proY = y / height;
		}
		label = valueLabel( label, m_elements[i].value(), total );
		CanvasText *text = new CanvasText( i, label, m_font, m_canvas );
		text->setColor( m_elements[i].labelColor() );
		text->setX( proX * width );
		text->setY( proY * height );
		text->setZ( 1 );
		text->show();
		m_elements[i].setProX( HORIZONTAL_BAR, proX );
		m_elements[i].setProY( HORIZONTAL_BAR, proY );
	    }
	    y += proheight;
	}
    }
}


#include <math.h>
void ChartForm::draw3DBarChart(
	const double scales[], double total, int count )
{
    double width = m_canvas->width();
    double height = m_canvas->height();
//    int prowidth = int(width / count);
//    int prowidth = int(width / (count+1));
int prowidth = 2;
// int prowidth = 1;
    int x = 0;
    QPen pen;
    pen.setStyle( NoPen );

int X = 0;
int Y = 0;
int block_cnt = (int)sqrt(count);
block_cnt++;
int row = block_cnt;
int shadow_angle = 3;
x = block_cnt*shadow_angle;

int row_priority = 1;
    for ( int i = count-1; i >= 0; i-- ) {
	if ( m_elements[i].isValid() ) {
if(row > 0 )
{
  int residual = i%block_cnt;
  if( residual == 0 )
  {
row_priority++;
    row--;
    x = row*shadow_angle;
  }
}
for( int d = 0; d < prowidth; d++ ) // begin depth
{
	    int extent = int(scales[i]);
	    int y = int(height - extent);
Y = y - d - (row*prowidth);
// X = x + 1 + (row*prowidth); 
X = x + 1;
	    QCanvasRectangle *rect = new QCanvasRectangle(
					    X, Y, prowidth, extent, m_canvas );
	    rect->setBrush( QBrush( m_elements[i].valueColor(),
				    BrushStyle(m_elements[i].valuePattern()) ) );
	    rect->setPen( pen );
//	    rect->setZ( 0 );
	    rect->setZ( row_priority );
	    rect->show();
	    x += prowidth;
//	    x += 1;
} // end depth
	}
    }
}


QString ChartForm::valueLabel(
	    const QString& label, double value, double total )
{
    if ( m_addValues == NO )
	return label;

    QString newLabel = label;
    if ( !label.isEmpty() )
	if ( m_chartType == VERTICAL_BAR )
	    newLabel += '\n';
	else
	    newLabel += ' ';
    if ( m_addValues == YES )
	newLabel += QString::number( value, 'f', m_decimalPlaces );
    else if ( m_addValues == AS_PERCENTAGE )
	newLabel += QString::number( (value / total) * 100, 'f', m_decimalPlaces )
		    + '%';
    return newLabel;
}



int
ChartForm::getItemFromPos( QPoint p )
{

  QPoint pos = mapFromGlobal(p);

  int selected = -1;

  QCanvasItemList canvas_list = m_canvas->collisions( pos );
  
  CanvasEllipse *item = NULL;
  QCanvasItemList::iterator it = NULL;
  QCanvasItem *m_movingItem = NULL;
  for ( it = canvas_list.begin(); it != canvas_list.end(); ++it )
  {
    if ( (*it)->rtti() == CanvasEllipse::CANVAS_ELLIPSE )
    {
      m_movingItem = *it;
      item = (CanvasEllipse *)m_movingItem;
    }
    if( item != NULL )
    {
        selected = item->index();
        break;
    }
  }

  return( selected );
}
