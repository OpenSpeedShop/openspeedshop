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


#include "optionsform.hxx"

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qfontdialog.h>
#include <qframe.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include "images/options_horizontalbarchart.xpm"
#include "images/options_piechart.xpm"
#include "images/options_verticalbarchart.xpm"


OptionsForm::OptionsForm( QWidget* parent, const char* name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    setCaption( "Chart -- Options" );
    resize( 320, 290 );

    optionsFormLayout = new QVBoxLayout( this, 11, 6 );

    chartTypeLayout = new QHBoxLayout( 0, 0, 6 );

    chartTypeTextLabel = new QLabel( "&Chart Type", this );
    chartTypeLayout->addWidget( chartTypeTextLabel );

    chartTypeComboBox = new QComboBox( FALSE, this );
    chartTypeComboBox->insertItem( QPixmap( options_piechart ), "Pie Chart" );
    chartTypeComboBox->insertItem( QPixmap( options_verticalbarchart ),
				   "Vertical Bar Chart" );
    chartTypeComboBox->insertItem( QPixmap( options_horizontalbarchart ),
				   "Horizontal Bar Chart" );
    chartTypeLayout->addWidget( chartTypeComboBox );
    optionsFormLayout->addLayout( chartTypeLayout );

    fontLayout = new QHBoxLayout( 0, 0, 6 );

    fontPushButton = new QPushButton( "&Font...", this );
    fontLayout->addWidget( fontPushButton );
    QSpacerItem* spacer = new QSpacerItem( 0, 0,
					   QSizePolicy::Expanding,
					   QSizePolicy::Minimum );
    fontLayout->addItem( spacer );

    fontTextLabel = new QLabel( this ); // Must be set by caller via setFont()
    fontLayout->addWidget( fontTextLabel );
    optionsFormLayout->addLayout( fontLayout );

    addValuesFrame = new QFrame( this );
    addValuesFrame->setFrameShape( QFrame::StyledPanel );
    addValuesFrame->setFrameShadow( QFrame::Sunken );
    addValuesFrameLayout = new QVBoxLayout( addValuesFrame, 11, 6 );

    addValuesButtonGroup = new QButtonGroup( "Show Values", addValuesFrame );
    addValuesButtonGroup->setColumnLayout(0, Qt::Vertical );
    addValuesButtonGroup->layout()->setSpacing( 6 );
    addValuesButtonGroup->layout()->setMargin( 11 );
    addValuesButtonGroupLayout = new QVBoxLayout(
					addValuesButtonGroup->layout() );
    addValuesButtonGroupLayout->setAlignment( Qt::AlignTop );

    noRadioButton = new QRadioButton( "&No", addValuesButtonGroup );
    noRadioButton->setChecked( TRUE );
    addValuesButtonGroupLayout->addWidget( noRadioButton );

    yesRadioButton = new QRadioButton( "&Yes", addValuesButtonGroup );
    addValuesButtonGroupLayout->addWidget( yesRadioButton );

    asPercentageRadioButton = new QRadioButton( "As &Percentage",
						addValuesButtonGroup );
    addValuesButtonGroupLayout->addWidget( asPercentageRadioButton );
    addValuesFrameLayout->addWidget( addValuesButtonGroup );

    decimalPlacesLayout = new QHBoxLayout( 0, 0, 6 );

    decimalPlacesTextLabel = new QLabel( "&Decimal Places", addValuesFrame );
    decimalPlacesLayout->addWidget( decimalPlacesTextLabel );

    decimalPlacesSpinBox = new QSpinBox( addValuesFrame );
    decimalPlacesSpinBox->setMinValue( 0 );
    decimalPlacesSpinBox->setMaxValue( 9 );
    decimalPlacesLayout->addWidget( decimalPlacesSpinBox );

    addValuesFrameLayout->addLayout( decimalPlacesLayout );

    optionsFormLayout->addWidget( addValuesFrame );

    buttonsLayout = new QHBoxLayout( 0, 0, 6 );
    spacer = new QSpacerItem( 0, 0,
			      QSizePolicy::Expanding, QSizePolicy::Minimum );
    buttonsLayout->addItem( spacer );

    okPushButton = new QPushButton( "OK", this );
    okPushButton->setDefault( TRUE );
    buttonsLayout->addWidget( okPushButton );

    cancelPushButton = new QPushButton( "Cancel", this );
    buttonsLayout->addWidget( cancelPushButton );
    optionsFormLayout->addLayout( buttonsLayout );

    connect( fontPushButton, SIGNAL( clicked() ), this, SLOT( chooseFont() ) );
    connect( okPushButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelPushButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    chartTypeTextLabel->setBuddy( chartTypeComboBox );
    decimalPlacesTextLabel->setBuddy( decimalPlacesSpinBox );
}


void OptionsForm::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont( &ok, m_font, this );
    if ( ok )
	setFont( font );
}


void OptionsForm::setFont( QFont font )
{
    QString label = font.family() + " " +
		    QString::number( font.pointSize() ) + "pt";
    if ( font.bold() )
	label += " Bold";
    if ( font.italic() )
	label += " Italic";
    fontTextLabel->setText( label );
    m_font = font;
}
