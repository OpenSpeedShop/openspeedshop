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
      fprintf(stderr, "Wrote %s\n", filename.ascii() );
    } else
    {
      fprintf(stderr, "Failed to write %s\n", filename.ascii() );
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

