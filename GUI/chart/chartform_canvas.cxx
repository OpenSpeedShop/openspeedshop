#include "canvasellipse.hxx"
#include "canvasshadowellipse.hxx"
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
    nprintf(DEBUG_PANELS) ("drawElements() entered\n");
    for ( QCanvasItemList::iterator it = list.begin(); it != list.end(); ++it )
    {
      nprintf(DEBUG_PANELS) ("DELETE IT!\n");
	  delete *it;
    }

	// 360 * 16 for pies; Qt works with 16ths of degrees
#ifdef OLDWAY
    int scaleFactor = m_chartType == PIEWITHSHADOW || m_chartType == PIEWITHNOSHADOW || m_chartType == PIEWITH3D ? 5760 :
			m_chartType == VERTICAL_BAR ? m_canvas->height() :
			    m_canvas->width();
#else // OLDWAY
    int scaleFactor = m_chartType == PIEWITHSHADOW || m_chartType == PIEWITHNOSHADOW || m_chartType == PIEWITH3D ? 5760 :
			m_chartType == VERTICAL_BAR || m_chartType == BARWITH3D ? (int)(m_canvas->height()*.66) : (int)(m_canvas->width()*.66);
#endif // OLDWAY
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

      nprintf(DEBUG_PANELS) ("m_chartType=%d\n", m_chartType );
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

  nprintf(DEBUG_PANELS) ("ChartForm::drawPieChart(%d)\n", m_chartType );

    float pitch = .35;
    int margin = 10;
    double width = m_canvas->width()-margin;
    double height = m_canvas->height()-margin;
    int size = int(width > height ? height : width);
    int x = int(width / 2);
    int y = int(height / 2);
    x+=(margin/2);
    y+=(margin/2);
    int angle = 0;

// Begin try to put out shadow
if(  m_chartType == PIEWITH3D )
{
  int z = 0;
  for(int depth = 12; depth > 0; depth--)
  {
    for ( int i = 0; i < count; ++i ) {
	if ( m_elements[i].isValid() ) {
	    int extent = int(scales[i]);
nprintf(DEBUG_PANELS) ("angle=%d extent=%d\n", angle,extent);
	    CanvasShadowEllipse *arc = new CanvasShadowEllipse(
					    i, size, (int)(size*pitch), angle, extent, m_canvas );
//	    QCanvasEllipse *arc = new QCanvasEllipse(
//					    size, size*pitch, angle, extent, m_canvas );
	    arc->setX( x );
	    arc->setY( y+depth );
	    arc->setZ( z++ );
	    arc->setBrush( QBrush( m_elements[i].valueColor(),
			   BrushStyle(m_elements[i].valuePattern()) ) );
	    arc->show();
	    angle += extent;
	  }
    }
  }
} else if( m_chartType == PIEWITHSHADOW )
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
	    arc->setBrush( QBrush( QColor("black"),
				   BrushStyle(1)) );
	    arc->show();
	    angle += extent;
	  }
    }
}
// End try to put out shadow...

    margin = 10;
    width = m_canvas->width()-margin;
    height = m_canvas->height()-margin;
    size = int(width > height ? height : width);
    x = int(width / 2);
    y = int(height / 2);
    x+=(margin/2);
    y+=(margin/2);
    angle = 0;

if( m_chartType == PIEWITH3D )
{
// Begin outline
// QCanvasEllipse *arc = new QCanvasEllipse(
//                         size, size*pitch, 0, 5760, m_canvas );
QCanvasEllipse *arc = new QCanvasEllipse(
                        size+1, (int)((size+1)*pitch), 4000, 5760, m_canvas );
arc->setX( x );
arc->setY( y );
arc->setZ( 100 );
arc->setBrush( QBrush( QColor("black"), BrushStyle(1)) );
arc->show();
// End outline
}

    for ( int i = 0; i < count; ++i )
    {
	if ( m_elements[i].isValid() )
    {
	    int extent = int(scales[i]);
	    CanvasEllipse *arc = new CanvasEllipse(
					    i, size, (int)(size*pitch), angle, extent, m_canvas );
	    arc->setX( x );
	    arc->setY( y );
	    arc->setZ( 200 );
	    arc->setBrush( QBrush( m_elements[i].valueColor(),
				   BrushStyle(m_elements[i].valuePattern()) ) );
	    arc->show();
	    angle += extent;
#ifdef TEXT_WANTED
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
#endif // TEXT_WANTED
	  }
    }
}


void ChartForm::drawVerticalBarChart(
	const double scales[], double total, int count )
{
    double width = m_canvas->width();
    double height = m_canvas->height();
//    int prowidth = int(width / count);
    int prowidth = int(width / (count+1));
    int x = 0;
    QPen pen;
    pen.setStyle( NoPen );

int X = 0;
int Y = 0;

    for ( int i = 0; i < MAX_ELEMENTS; ++i ) {
	if ( m_elements[i].isValid() ) {
for( int d = 0; d < prowidth; d++ ) // begin depth
{
	    int extent = int(scales[i]);
	    int y = int(height - extent);
 Y = y - d;
 X = x + 1; 
	    QCanvasRectangle *rect = new QCanvasRectangle(
					    X, Y, prowidth, extent, m_canvas );
	    rect->setBrush( QBrush( m_elements[i].valueColor(),
				    BrushStyle(m_elements[i].valuePattern()) ) );
	    rect->setPen( pen );
//	    rect->setZ( 0 );
	    rect->setZ( i );
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
//		text->setZ( i );
		text->show();
		m_elements[i].setProX( VERTICAL_BAR, proX );
		m_elements[i].setProY( VERTICAL_BAR, proY );
	    }
//	    x += prowidth;
	    x += 1;
}
	}
    }
}


void ChartForm::drawHorizontalBarChart(
	const double scales[], double total, int count )
{
    double width = m_canvas->width();
    double height = m_canvas->height();
    int proheight = int(height / count);
//    int proheight = int(height / (count+1));
    int y = 0;
    QPen pen;
    pen.setStyle( NoPen );
//int Y = 0;
//int X = 0;

    for ( int i = 0; i < MAX_ELEMENTS; ++i ) {
// X = 0;
	if ( m_elements[i].isValid() ) {
// for( int d = 0; d < proheight; d++ ) // begin depth
// {
	    int extent = int(scales[i]);
// Y = y + d;
// X++;
	    QCanvasRectangle *rect = new QCanvasRectangle(
//					    X, Y, extent, proheight, m_canvas );
					    0, y, extent, proheight, m_canvas );
	    rect->setBrush( QBrush( m_elements[i].valueColor(),
				    BrushStyle(m_elements[i].valuePattern()) ) );
	    rect->setPen( pen );
//	    rect->setZ( i );
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
//	    y += 1;
// }
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

printf("count=%d block_cnt=%d\n", count, block_cnt);

int row_priority = 1;
    for ( int i = count-1; i >= 0; i-- ) {
	if ( m_elements[i].isValid() ) {
// printf("put out element %d\n", i);
if(row > 0 )
{
  int residual = i%block_cnt;
// printf("residual = %d  %d = (%d)\n", i, block_cnt, residual);
  if( residual == 0 )
  {
row_priority++;
//    printf("                            break!\n");
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
