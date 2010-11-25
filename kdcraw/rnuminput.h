/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-08-16
 * Description : Integer and double num input widget
 *               re-implemented with a reset button to switch to
 *               a default value.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RNUMINPUT_H
#define RNUMINPUT_H

#include <QWidget>

// Local includes.

#include "stkdcrawexport.h"

class QSpinBox;
class QDoubleSpinBox;
namespace KDcrawIface
{

	class RIntNumInputPriv;

	class ST_KDCRAW_EXPORT RIntNumInput : public QWidget
	{

		Q_OBJECT
		
	public:

		RIntNumInput(QWidget *parent = 0);
		~RIntNumInput();

		int  value();

		void setSliderEnabled(bool b);
		void setRange(int min, int max, int step);
		void setLabel(const QString& _Text, Qt::Alignment _Alignment); 

		void setDefaultValue(int d);
		int  defaultValue();

		QSpinBox* input() const;

	Q_SIGNALS:

		void reset();
		void valueChanged(int);

	public Q_SLOTS:

		void setValue(int d);
		void slotReset();

	private Q_SLOTS:

		void slotValueChanged(int);
		void slotSliderValueChange( int _Value );

	private:

		RIntNumInputPriv* d;
	};

// ---------------------------------------------------------

	class RDoubleNumInputPriv;

	class ST_KDCRAW_EXPORT RDoubleNumInput : public QWidget
	{

		Q_OBJECT

	public:

		RDoubleNumInput(QWidget *parent = 0);
		~RDoubleNumInput();

		double value();

		void   setDecimals(int p);
		void   setRange(double min, double max, double step, bool slider = true);

		void   setDefaultValue(double d);
		double defaultValue();

		QDoubleSpinBox* input() const;

	Q_SIGNALS:

		void reset();
		void valueChanged(double);

	public Q_SLOTS:

		void setValue(double d);
		void slotReset();

	private Q_SLOTS:

		void slotValueChanged(double);
		void slotSliderValueChange( int _Value );
	
	private:

		int intValue(double _Value); 
		double doubleValue(int _Value); 

		RDoubleNumInputPriv* d;
	};

}  // namespace KDcrawIface

#endif /* RNUMINPUT_H */
