/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Starblitz reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SRESETSLIDER_H
#define SRESETSLIDER_H

#include <QWidget>
#include "stwidgetsexport.h"

//! A Slider with reset button.

class QSlider;
class ST_WIDGETS_EXPORT SResetSlider : public QWidget
{
	Q_OBJECT

	QSlider* Slider;
	int ResetValue;

public:
	SResetSlider(Qt::Orientation _Orientation, QWidget* _Parent = 0);
	QSlider* slider() const { return Slider; }
	int resetValue() const { return ResetValue; }
	void setResetValue(int _Value) { ResetValue = _Value; }
	int value() const;
	void setValue(int _Value);

public slots:
	void reset();

signals:
	void valueChanged(int);
};

#endif // SRESETSLIDER_H
