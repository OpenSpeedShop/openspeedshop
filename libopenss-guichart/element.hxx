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


#ifndef ELEMENT_H
#define ELEMENT_H

#include <qcolor.h>
#include <qnamespace.h>
#include <qstring.h>
#include <qvaluevector.h>

class Element;

typedef QValueVector<Element> ElementVector;

/*
    Elements are valid if they have a value which is > EPSILON.
*/
const double EPSILON = 0.0000001; // Must be > INVALID.


//! Defines one element of the chart.
class Element
{
public:
    enum { INVALID = -1 };
    enum { NO_PROPORTION = -1 };
    enum { MAX_PROPOINTS = 3 }; // One proportional point per chart type

    Element( double value = INVALID, QColor valueColor = Qt::gray,
	     int valuePattern = Qt::SolidPattern,
	     const QString& label = QString::null,
	     QColor labelColor = Qt::black ) {
	init( value, valueColor, valuePattern, label, labelColor );
	for ( int i = 0; i < MAX_PROPOINTS * 2; ++i )
	    m_propoints[i] = NO_PROPORTION;
    }
    ~Element() {}

    bool isValid() const { return m_value > EPSILON; }

    double value() const { return m_value; }
    QColor valueColor() const { return m_valueColor; }
    int valuePattern() const { return m_valuePattern; }
    QString label() const { return m_label; }
    QColor labelColor() const { return m_labelColor; }
    double proX( int index ) const;
    double proY( int index ) const;

    void set( double value = INVALID, QColor valueColor = Qt::gray,
	      int valuePattern = Qt::SolidPattern,
	      const QString& label = QString::null,
	      QColor labelColor = Qt::black ) {
	init( value, valueColor, valuePattern, label, labelColor );
    }
    void setValue( double value ) { m_value = value; }
    void setValueColor( QColor valueColor ) { m_valueColor = valueColor; }
    void setValuePattern( int valuePattern );
    void setLabel( const QString& label ) { m_label = label; }
    void setLabelColor( QColor labelColor ) { m_labelColor = labelColor; }
    void setProX( int index, double value );
    void setProY( int index, double value );

#ifdef Q_FULL_TEMPLATE_INSTANTIATION
    // xlC 3.x workaround
    Q_DUMMY_COMPARISON_OPERATOR(Element)
    bool operator!=( const Element& e) const {
	return ( !(e == *this) );
    }
#endif

private:
    void init( double value, QColor valueColor, int valuePattern,
	       const QString& label, QColor labelColor );

    double m_value;
    QColor m_valueColor;
    int m_valuePattern;
    QString m_label;
    QColor m_labelColor;
    double m_propoints[2 * MAX_PROPOINTS];
};


QTextStream &operator<<( QTextStream&, const Element& );
QTextStream &operator>>( QTextStream&, Element& );

#endif
