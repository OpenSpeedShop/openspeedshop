#include "canvasview.hxx"
#include "chartform.hxx"

#include <qfile.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qstatusbar.h>


void ChartForm::load( const QString& filename )
{
}


void ChartForm::fileSave()
{
}


void ChartForm::fileSaveAsPixmap()
{
    QString filename = QFileDialog::getSaveFileName(
			    QString::null, "Images (*.png *.xpm *.jpg)",
			    this, "file save as bitmap",
			    "Chart -- File Save As Bitmap" );
    if ( QPixmap::grabWidget( m_canvasView ).
	    save( filename,
		  filename.mid( filename.findRev( '.' ) + 1 ).upper() ) )
    {
      fprintf(stderr, "Wrote %s\n", filename );
    } else
    {
      fprintf(stderr, "Failed to write %s\n", filename );
    }
}


bool ChartForm::fileSaveAsPixmap(QString filename)
{
    return( QPixmap::grabWidget( m_canvasView ).save( filename,
		  filename.mid( filename.findRev( '.' ) + 1 ).upper() ) );
}

void ChartForm::filePrint()
{
#ifdef MAINWINDOW
    if ( !m_printer )
	m_printer = new QPrinter;
    if ( m_printer->setup() ) {
	QPainter painter( m_printer );
	m_canvas->drawArea( QRect( 0, 0, m_canvas->width(), m_canvas->height() ),
			    &painter, FALSE );
	if ( !m_printer->outputFileName().isEmpty() )
	    statusBar()->message( QString( "Printed \'%1\'" ).
				  arg( m_printer->outputFileName() ), 2000 );
    }
#endif // MAINWINDOW
}

