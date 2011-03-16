/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
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
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "labeledsliderwidget.h"
#include <QLayout> 
#include <QLabel> 
#include <QSlider> 

LabeledSliderWidget::LabeledSliderWidget(const QString& _Title, int _MinValue, int _MaxValue, QWidget* _Parent)
 : QWidget(_Parent), Title(_Title)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	
	MLabel = new QLabel(this);
	MLayout->addWidget(MLabel);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
	MSlider = new QSlider(this);
	connect(MSlider, SIGNAL(sliderReleased()), this, SIGNAL(valueChanged()));
	connect(MSlider, SIGNAL(valueChanged(int )), this, SLOT(setLabelValue(int )));
	MSlider->setRange(_MinValue, _MaxValue);
	MSlider->setOrientation(Qt::Horizontal);
	MLayout->addWidget(MSlider);
}


LabeledSliderWidget::~LabeledSliderWidget()
{
}

QSlider* LabeledSliderWidget::slider()
{
	return MSlider;
}

void LabeledSliderWidget::setLabelValue(int _Value)
{
	MLabel->setText(QString("%1 : %2").arg(Title).arg(QString::number(_Value)));
}

