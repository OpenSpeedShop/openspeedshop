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


#include "setdataform.hxx"
#include "chartform.hxx"

#include <qcolordialog.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qtable.h>

#include "images/pattern01.xpm"
#include "images/pattern02.xpm"
#include "images/pattern03.xpm"
#include "images/pattern04.xpm"
#include "images/pattern05.xpm"
#include "images/pattern06.xpm"
#include "images/pattern07.xpm"
#include "images/pattern08.xpm"
#include "images/pattern09.xpm"
#include "images/pattern10.xpm"
#include "images/pattern11.xpm"
#include "images/pattern12.xpm"
#include "images/pattern13.xpm"
#include "images/pattern14.xpm"

const int MAX_PATTERNS = 14;


SetDataForm::SetDataForm( ElementVector *elements, int decimalPlaces,
			  QWidget* parent,  const char* name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )

{
    m_elements = elements;
    m_decimalPlaces = decimalPlaces;

    setCaption( "Chart -- Set Data" );
    resize( 540, 440 );

    tableButtonBox = new QVBoxLayout( this, 11, 6, "table button box layout" );

    table = new QTable( this, "data table" );
    table->setNumCols( 5 );
    table->setNumRows( ChartForm::MAX_ELEMENTS );
    table->setColumnReadOnly( 1, TRUE );
    table->setColumnReadOnly( 2, TRUE );
    table->setColumnReadOnly( 4, TRUE );
    table->setColumnWidth( 0, 80 );
    table->setColumnWidth( 1, 60 ); // Columns 1 and 4 must be equal
    table->setColumnWidth( 2, 60 );
    table->setColumnWidth( 3, 200 );
    table->setColumnWidth( 4, 60 );
    QHeader *th = table->horizontalHeader();
    th->setLabel( 0, "Value" );
    th->setLabel( 1, "Color" );
    th->setLabel( 2, "Pattern" );
    th->setLabel( 3, "Label" );
    th->setLabel( 4, "Color" );
    tableButtonBox->addWidget( table );

    buttonBox = new QHBoxLayout( 0, 0, 6, "button box layout" );

    colorPushButton = new QPushButton( this, "color button" );
    colorPushButton->setText( "&Color..." );
    colorPushButton->setEnabled( FALSE );
    buttonBox->addWidget( colorPushButton );

    QSpacerItem *spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding,
						 QSizePolicy::Minimum );
    buttonBox->addItem( spacer );

    okPushButton = new QPushButton( this, "ok button" );
    okPushButton->setText( "OK" );
    okPushButton->setDefault( TRUE );
    buttonBox->addWidget( okPushButton );

    cancelPushButton = new QPushButton( this, "cancel button" );
    cancelPushButton->setText( "Cancel" );
    cancelPushButton->setAccel( Key_Escape );
    buttonBox->addWidget( cancelPushButton );

    tableButtonBox->addLayout( buttonBox );

    connect( table, SIGNAL( clicked(int,int,int,const QPoint&) ),
	     this, SLOT( setColor(int,int) ) );
    connect( table, SIGNAL( currentChanged(int,int) ),
	     this, SLOT( currentChanged(int,int) ) );
    connect( table, SIGNAL( valueChanged(int,int) ),
	     this, SLOT( valueChanged(int,int) ) );
    connect( colorPushButton, SIGNAL( clicked() ), this, SLOT( setColor() ) );
    connect( okPushButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelPushButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QPixmap patterns[MAX_PATTERNS];
    patterns[0]  = QPixmap( pattern01 );
    patterns[1]  = QPixmap( pattern02 );
    patterns[2]  = QPixmap( pattern03 );
    patterns[3]  = QPixmap( pattern04 );
    patterns[4]  = QPixmap( pattern05 );
    patterns[5]  = QPixmap( pattern06 );
    patterns[6]  = QPixmap( pattern07 );
    patterns[7]  = QPixmap( pattern08 );
    patterns[8]  = QPixmap( pattern09 );
    patterns[9]  = QPixmap( pattern10 );
    patterns[10] = QPixmap( pattern11 );
    patterns[11] = QPixmap( pattern12 );
    patterns[12] = QPixmap( pattern13 );
    patterns[13] = QPixmap( pattern14 );

    QRect rect = table->cellRect( 0, 1 );
    QPixmap pix( rect.width(), rect.height() );

    for ( int i = 0; i < ChartForm::MAX_ELEMENTS; ++i ) {
	Element element = (*m_elements)[i];

	if ( element.isValid() )
	    table->setText(
		i, 0,
		QString( "%1" ).arg( element.value(), 0, 'f',
				     m_decimalPlaces ) );

	QColor color = element.valueColor();
	pix.fill( color );
	table->setPixmap( i, 1, pix );
	table->setText( i, 1, color.name() );

	QComboBox *combobox = new QComboBox;
	for ( int j = 0; j < MAX_PATTERNS; ++j )
	    combobox->insertItem( patterns[j] );
	combobox->setCurrentItem( element.valuePattern() - 1 );
	table->setCellWidget( i, 2, combobox );

	table->setText( i, 3, element.label() );

	color = element.labelColor();
	pix.fill( color );
	table->setPixmap( i, 4, pix );
	table->setText( i, 4, color.name() );
    }

}


void SetDataForm::currentChanged( int, int col )
{
    colorPushButton->setEnabled( col == 1 || col == 4 );
}


void SetDataForm::valueChanged( int row, int col )
{
    if ( col == 0 ) {
	bool ok;
	double d = table->text( row, col ).toDouble( &ok );
	if ( ok && d > EPSILON )
	    table->setText(
		row, col, QString( "%1" ).arg(
			    d, 0, 'f', m_decimalPlaces ) );
	else if ( !table->text( row, col ).isEmpty() )
	    table->setText( row, col, table->text( row, col ) + "?" );
    }
}


void SetDataForm::setColor()
{
    setColor( table->currentRow(), table->currentColumn() );
    table->setFocus();
}


void SetDataForm::setColor( int row, int col )
{
    if ( !( col == 1 || col == 4 ) )
	return;

    QColor color = QColorDialog::getColor(
			QColor( table->text( row, col ) ),
			this, "color dialog" );
    if ( color.isValid() ) {
	QPixmap pix = table->pixmap( row, col );
	pix.fill( color );
	table->setPixmap( row, col, pix );
	table->setText( row, col, color.name() );
    }
}


void SetDataForm::accept()
{
    bool ok;
    for ( int i = 0; i < ChartForm::MAX_ELEMENTS; ++i ) {
	Element &element = (*m_elements)[i];
	double d = table->text( i, 0 ).toDouble( &ok );
	if ( ok )
	    element.setValue( d );
	else
	    element.setValue( Element::INVALID );
	element.setValueColor( QColor( table->text( i, 1 ) ) );
	element.setValuePattern(
		((QComboBox*)table->cellWidget( i, 2 ))->currentItem() + 1 );
	element.setLabel( table->text( i, 3 ) );
	element.setLabelColor( QColor( table->text( i, 4 ) ) );
    }

    QDialog::accept();
}
