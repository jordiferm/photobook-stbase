/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "stnumqualitygauge.h"
#include <QLayout>
#include <QLabel>
#include "qtsvgdialgauge.h"

void STNumQualityGauge::calcGaugeValue()
{
	double Median = RangeFrom + ((RangeTo - RangeFrom) / 2);

	double Relation;
	if (CurrentValue < Median)
		Relation = static_cast<double>(CurrentValue) / Median;
	else
		Relation = Median / static_cast<double>(CurrentValue);

	int NewValue = 100 - (Relation * 100);


	Gauge->setValue(NewValue);
	updateInfoLabel();
}

void STNumQualityGauge::updateInfoLabel()
{
	InfoLabel->setText(tr("<small>N.Imag.:%1,Optimal(%2-%3)</small>").arg(CurrentValue).arg(RangeFrom).arg(RangeTo));
}

STNumQualityGauge::STNumQualityGauge(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setMargin(1);
	MLayout->setSpacing(0);

	Gauge = new QtSvgDialGauge(this);
	Gauge->setSkin("Amperemeter");
	Gauge->setNeedleOrigin(0.5, .83596590);
	Gauge->setMinimum(0);
	Gauge->setMaximum(100);
	Gauge->setStartAngle(-42);
	Gauge->setEndAngle(42);
	Gauge->setValue(50);
	Gauge->setMaximumSize(200, 200);
	MLayout->addWidget(Gauge);

	InfoLabel = new QLabel(this);
	InfoLabel->setAlignment(Qt::AlignCenter);
	MLayout->addWidget(InfoLabel);

	setOptimalQualityRange(45, 55);
}

void STNumQualityGauge::setOptimalQualityRange(int _From, int _To)
{
	RangeFrom = _From;
	RangeTo = _To;
}

void STNumQualityGauge::setValue(int _Value)
{
	CurrentValue = _Value;
	calcGaugeValue();
}
