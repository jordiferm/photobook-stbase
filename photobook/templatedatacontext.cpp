/****************************************************************************
**
** Copyright (C) 2006-2011 SoftTopia. All rights reserved.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "templatedatacontext.h"
#include <QDate>


using namespace SPhotoBook;
void TemplateDataContext::setDefaultValues()
{
	setDate(QDate::currentDate());
	setTitle(QObject::tr("No Title"));
	setSubTitle("");
	setAuthor("");
}

TemplateDataContext::TemplateDataContext()
{
	setDefaultValues();
}


QString TemplateDataContext::value(const QString& _VarName) const
{
	QString Res = "";
	if (VarMap.contains(_VarName))
	{
		Res = VarMap.value(_VarName);
	}
	return Res;
}

void TemplateDataContext::setValue(const QString& _VarName, const QVariant& _Value)
{
	VarMap.insert(_VarName.toLower(), _Value.toString());
}

QStringList TemplateDataContext::vars() const
{
	return VarMap.keys();
}

void TemplateDataContext::setDay(int _Value)
{
	setValue("day", _Value);
}

void TemplateDataContext::setMonth(int _Value)
{
	setValue("month", _Value);
	setValue("short_month_name", QDate::shortMonthName(_Value));
	setValue("long_month_name", QDate::longMonthName(_Value));
}

void TemplateDataContext::setYear(int _Value)
{
	setValue("year", _Value);
}

void TemplateDataContext::setDate(const QDate& _Date)
{
	setYear(_Date.year());
	setMonth(_Date.month());
	setDay(_Date.day());
	setValue("day_of_week", _Date.dayOfWeek());
	setValue("days_in_month", _Date.daysInMonth());
	setValue("date", _Date.toString(Qt::SystemLocaleShortDate));
	setValue("longdate", _Date.toString(Qt::SystemLocaleLongDate));
	setValue("days_in_year", _Date.daysInYear());

	QString MonthDays;
	for (int Vfor=0; Vfor < _Date.daysInMonth(); Vfor++)
	{
		if (!MonthDays.isEmpty())
			MonthDays+= " ";
		MonthDays += QString::number(Vfor +1);
	}
	setValue("monthdays", MonthDays);
}

void TemplateDataContext::setTitle(const QString& _Title)
{
	setValue("title", _Title);
}

void TemplateDataContext::setSubTitle(const QString& _SubTitle)
{
	setValue("subtitle", _SubTitle);
}

void TemplateDataContext::setAuthor(const QString& _Author)
{
	setValue("author", _Author);
}


QString TemplateDataContext::varPattern(const QString& _VarName)
{
	return QString("\%%1\%").arg(_VarName);
}

