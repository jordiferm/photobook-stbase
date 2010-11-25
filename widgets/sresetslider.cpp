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

#include "sresetslider.h"
#include <QLayout>
#include <QSlider>
#include <QToolButton>

SResetSlider::SResetSlider(Qt::Orientation _Orientation, QWidget* _Parent) : QWidget(_Parent), ResetValue(0)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setSpacing(0);
	MLayout->setMargin(0);
	Slider = new QSlider(_Orientation, this);
	connect(Slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

	MLayout->addWidget(Slider);;
	QToolButton* ResetButton = new QToolButton(this);
	ResetButton->setIcon(QIcon(":/st/editundo.png"));
	MLayout->addWidget(ResetButton);
	connect(ResetButton, SIGNAL(clicked()), this, SLOT(reset()));

}

void SResetSlider::reset()
{
	Slider->setValue(ResetValue);
}

int SResetSlider::value() const
{
	return Slider->value();
}

void SResetSlider::setValue(int _Value)
{
	Slider->setValue(_Value);
}
