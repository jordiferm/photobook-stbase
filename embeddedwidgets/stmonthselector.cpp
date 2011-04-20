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

#include "stmonthselector.h"
#include <QLayout>
#include <QLabel>
#include <QDebug>
#include "QtScrollWheel"

void STMonthSelector::setCurrentDate(const QDate& _Date)
{
	CurrentDate = _Date;
	YearLabel->setText(QString("<h2>%1</h2>").arg(CurrentDate.year()));
	MonthLabel->setText(QString("%1").arg(CurrentDate.toString("MMMM")));
}

int STMonthSelector::monthsTo(const QDate& _FromDate, const QDate& _ToDate)
{
	int RangeMonths = 12 - _FromDate.month();
	if (_ToDate.year() > _FromDate.year())
	{
		RangeMonths+= _ToDate.month();
		RangeMonths+= (_ToDate.year() - _FromDate.year() - 1) * 12;
	}
	else
		RangeMonths-= 12 - _ToDate.month();

	return RangeMonths;
}

STMonthSelector::STMonthSelector(QWidget *parent) :
	QWidget(parent)
{
	QGridLayout* MLayout = new QGridLayout(this);
	ScrWheel = new QtScrollWheel(this);
	connect(ScrWheel, SIGNAL(valueChanged(int)), this, SLOT(slotWheelValueChanged(int )));
	MLayout->addWidget(ScrWheel, 0, 0, 2, 1);

	YearLabel = new QLabel(this);
	YearLabel->setText("<h2>2001</h2>");
	MLayout->addWidget(YearLabel, 0, 1);

	MonthLabel = new QLabel(this);
	MonthLabel->setText(tr("<h2>April</h2>"));
	MLayout->addWidget(MonthLabel, 1, 1);

	setMinimumWidth(240);

	setDateRange(QDate(QDate::currentDate().year(), 1, 1), QDate(QDate::currentDate().year(), 12, 1)) ;
}

void STMonthSelector::setDateRange(const QDate& _From, const QDate& _To)
{
	FromDate = _From;
	ToDate = _To;
	int RangeMonths = monthsTo(_From, _To);
	ScrWheel->setRange(0, RangeMonths);
	setCurrentDate(FromDate);
}

void STMonthSelector::setDate(const QDate& _Date)
{
	int RangeMonths = monthsTo(_Date, ToDate);
	ScrWheel->setValue(RangeMonths);
	setCurrentDate(_Date);
}

void STMonthSelector::slotWheelValueChanged(int _Value)
{
	setCurrentDate(ToDate.addMonths(-_Value));
	emit dateChanged(CurrentDate);
}
