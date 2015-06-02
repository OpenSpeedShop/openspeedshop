////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "AboutClass.hxx"
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtabwidget.h>

#ifdef HAVE_CONFIG_H
#include "config.h" // For version (VERSION) number
#endif

#include "splash_noborder.xpm"
AboutClass::AboutClass(QWidget *w) : QDialog(w, "", 0, 0)
{


  QVBoxLayout *aboutLayout = new QVBoxLayout( this, 1, 2, "aboutLayout" );

  QLabel *version = new QLabel(this, "version");
  QString version_str = VERSION;
  version->setText(QString("Open|SpeedShop(tm) Version %1").arg(version_str));

  
  QTabWidget *tabWidget = new QTabWidget( this, "aboutLayout" );


  QWidget *tab1Widget = new QWidget( this, "tab1Widget" );
  QVBoxLayout *tab1Layout = new QVBoxLayout( tab1Widget, 1, 2, "tab1Layout" );
  QLabel *label1 = new QLabel(tab1Widget, "label1");
  QPixmap *splashPixmap = new QPixmap( splash_noborder_xpm );
  label1->setPixmap( *splashPixmap );
  label1->setBackgroundColor( QColor(Qt::white) );
  label1->setFrameStyle( QFrame::NoFrame );
  tabWidget->addTab(tab1Widget, "About");
  tab1Layout->addWidget(label1);


  
  QWidget *tab2Widget = new QWidget( this, "tab2Widget" );
  QVBoxLayout *tab2Layout = new QVBoxLayout( tab2Widget, 1, 2, "tab2Layout" );
  QLabel *label2 = new QLabel(tab2Widget, "label2");
  label2->setText("Jack Carter\nSteve Collins\nJim Galarowicz\nWilliam Hachfeld\nDonald Maghrak\nTodd Smith\nHelen Stepanov\nAllen Stipek\nDavid Whitney\n");
  tabWidget->addTab(tab2Widget, "Authors");
  tab2Layout->addWidget(label2);



  QWidget *tab3Widget = new QWidget( this, "tab3Widget" );
  QVBoxLayout *tab3Layout = new QVBoxLayout( tab3Widget, 1, 2, "tab3Layout" );
  QLabel *label3 = new QLabel(tab3Widget, "label3");
  label3->setText("Thanks to the Universities of Wisconsin and Maryland, in particular:\nBarton Miller\nJeff Hollingsworth\nDrew Bernat\nMatt Legendre\nTodd L Miller\nJames Waskiewicz\nGregory Quinn\n\nThanks to the Department of Energy and the Tri-laboratories of LLNL, LANL, and Sandia.\nSpecial Thanks to the Tri-laboratory Technical Leadership Team:\nMartin Schulz, LLNL\nChip Kent, formerly of LANL\nScott Cranford, Sandia\nNathan DeBardeleben, LANL\nDavid Montoya, LANL\nMary Zosel, LLNL\n\nThanks to SGI and these current and former SGI employees, in particular:\nGeorge Pirocanac\nSteve Reinhardt\nKent Randolph\nSteve Siegfried\nDave Alexander\nRick Chapek\nand Amaury Le Leyzour, Doug Crisman, Kevin G. Snow, Scott Soloway, Leslie Tung\nBeverly Bernard, John Janicek, Leland Bigger, Kate Cassell, Glen Aalund and Carol Gibson\n");
  tabWidget->addTab(tab3Widget, "Thanks To");
  tab3Layout->addWidget(label3);





  QWidget *tab4Widget = new QWidget( this, "tab4Widget" );
  QVBoxLayout *tab4Layout = new QVBoxLayout( tab4Widget, 1, 2, "tab4Layout" );
  QLabel *label4 = new QLabel(tab4Widget, "label4");
  label4->setText("                                   Open|SpeedShop License\n\n\nOpen|SpeedShop code has been released.  The source code has been released almost entirely\nunder the LGPL license.\n\n");
  tabWidget->addTab(tab4Widget, "License Agreement");
  tab4Layout->addWidget(label4);




  QPushButton *button = new QPushButton( this, "button");
  button->setAutoDefault(TRUE);
  button->setDefault(TRUE);
  button->setText( "Close" );

  button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  connect( button, SIGNAL( clicked() ), this, SLOT( accept() ) );








  aboutLayout->addWidget(version);
  aboutLayout->addWidget(tabWidget);
  aboutLayout->addWidget(button);


  resize( QSize(500, 300).expandedTo(minimumSizeHint()) );
}

AboutClass::~AboutClass()
{
}


void
AboutClass::accept()
{
  QDialog::accept();
}
