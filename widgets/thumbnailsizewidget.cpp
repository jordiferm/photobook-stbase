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

#include "thumbnailsizewidget.h"
#include <QLayout>
#include <QLabel>
#include <QSlider>

ThumbnailSizeWidget::ThumbnailSizeWidget(QWidget* _Parent) : QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	QLabel* ZoomOutLabel = new QLabel(this);
	ZoomOutLabel->setPixmap(QPixmap(":/st/zoom-out-mini.png"));
	MLayout->addWidget(ZoomOutLabel);

	Slider = new QSlider(Qt::Horizontal, this);
	MLayout->addWidget(Slider);
	Slider->setRange(1, 5);
	connect(Slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

	QLabel* ZoomInLabel = new QLabel(this);
	ZoomInLabel->setPixmap(QPixmap(":/st/zoom-in-mini.png"));
	MLayout->addWidget(ZoomInLabel);
	setMaximumWidth(128);
}

void ThumbnailSizeWidget::setValue(int _Value)
{
	Slider->setValue(_Value);
}
