/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "graphicsmonthitem.h"
#include <QTextStream>
#include <QFont>
#include <QDomElement>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include "stutils.h"

using namespace SPhotoBook;



void GraphicsMonthItem::init()
{
	QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable);
	setTextInteractionFlags(Qt::NoTextInteraction);
	QDate CDate = QDate::currentDate(); 
	Month = CDate.month(); 
	Year = CDate.year(); 
	ShowDayLabels = true; 
	ShowMonthLabel = true; 
	TextColor = QColor("#000000");
	QFont CalFont("Arial", 8);
	setFont(CalFont);
}

void GraphicsMonthItem::mousePressEvent(QGraphicsSceneMouseEvent* _Event)
{
	QGraphicsTextItem::mousePressEvent(_Event);
	return;
}

GraphicsMonthItem::GraphicsMonthItem(QGraphicsItem* parent): GraphicsTextItem(parent)
{
	init();
}

void GraphicsMonthItem::scaleToSize(const QSizeF& _Size)
{
	scale(_Size.width() / boundingRect().width() , _Size.height() / boundingRect().height() );	
}

QString GraphicsMonthItem::longMonthName(int _Month) const
{
	QString Res; 
	switch (_Month)
	{
		case 1 : 
			Res = tr("January");
		break; 
		case 2 : 
			Res = tr("February");
		break; 
		case 3 : 
			Res = tr("March");
		break; 
		case 4 : 
			Res = tr("April");
		break; 
		case 5 : 
			Res = tr("May");
		break; 
		case 6 : 
			Res = tr("June");
		break; 
		case 7 : 
			Res = tr("July");
		break; 
		case 8 : 
			Res = tr("August");
		break; 
		case 9 : 
			Res = tr("September");
		break; 
		case 10 : 
			Res = tr("October");
		break; 
		case 11 : 
			Res = tr("November");
		break; 
		case 12 : 
			Res = tr("December");
		break; 
	}
	return Res; 
}

QString GraphicsMonthItem::shortDayName(int _DayOfWeek) const
{
	QString Res; 
	switch (_DayOfWeek)
	{
		case 1 : 
			Res = tr("Mon");
		break; 
		case 2 : 
			Res = tr("Tue");
		break; 
		case 3 : 
			Res = tr("Wed");
		break; 
		case 4 : 
			Res = tr("Thu");
		break; 
		case 5 : 
			Res = tr("Fri");
		break; 
		case 6 : 
			Res = tr("Sat");
		break; 
		case 7 : 
			Res = tr("Sun");
		break; 
	}
	return Res; 
}


void GraphicsMonthItem::setMonth(int _Month, int _Year)
{
	Month = _Month;
	Year = _Year;
	addDefaultHolyDays(_Year);
	setHtml(getMonthHtml());

}

bool GraphicsMonthItem::isHoliday(int _Day, int _Month) const
{
	THolidayList::const_iterator it;
	bool Found = false; 
	it = HolidayList.begin();
	while (!Found && it != HolidayList.end())
	{	
		Found = it->month() == _Month && it->day() == _Day; 
		++it; 
	}
	return Found; 

}

QString GraphicsMonthItem::getMonthHtml() const
{
	QDate CDate; 
	CDate.setDate(Year, Month, 1); 
	QString Res; 
	QTextStream Strm(&Res);
	//Strm << "<table bgcolor=#00FF00 border=0 >";
	Strm << "<table width=150 border=0 >";
	if (showMonthLabel())
	{
		Strm << "<tr><th colspan='7'><span style=\" font-size:" << font().pointSize() + 5 << "pt; font-family:'" <<  font().family() << "'\">" << "<font color=" << TextColor.name() << ">"  << longMonthName(Month) << "</font></span></th></tr>";
	}
	int Vfor;
	if (ShowDayLabels)
	{
		Strm << "<tr>";
		for (Vfor=1; Vfor < 8; Vfor++)
		{
			Strm << "<th><span style=\" font-size:" << font().pointSize() - 3 << "pt; font-family:'" <<  font().family() << "'\">" << "<font color=" << TextColor.name() << ">"  << shortDayName(Vfor) << "&nbsp;</font></span></th>";
		}
		Strm << "</tr>";
	}
	Strm << "<tr>";
	for (Vfor=1; Vfor < CDate.dayOfWeek(); Vfor++)
	{
    	Strm << "<td></td>";
	}
	for (Vfor=0; Vfor < CDate.daysInMonth(); Vfor++)
	{
		QDate CDate(Year, Month, Vfor + 1); 
		QString DayColor; 
		if (CDate.dayOfWeek() == 7 || isHoliday(CDate.day(), CDate.month()))
			DayColor = "#FF0000";
		else 
			DayColor = TextColor.name();
		QString FontFamily = font().family();
		Strm << "<td align=right><span style=\" font-size:" << font().pointSize() << "pt; font-family:'" <<  FontFamily << "'\"><font color=" << DayColor << ">" << Vfor +1 << "&nbsp;</font></span></td>";
		if (CDate.dayOfWeek() == 7)
			Strm << "</tr><tr>";
	}
	Strm << "</tr></table>";

	return Res; 
}


void GraphicsMonthItem::loadElement(const QDomElement& _Element, const QString& _LoadDir)
{
	//Font (We have to set it before generate Html)
	setFont(QFont(_Element.attribute("font", "Arial"), _Element.attribute("fontsize", "9").toInt()));

	setShowMonthLabel(_Element.attribute("showmonth", "false").toLower() == "true");
	setShowDayLabels(_Element.attribute("showdays", "false").toLower() == "true");
	setTextColor(QColor(_Element.attribute("textcolor", "#000000")));

	//Geometry
	setPos(_Element.attribute("x", "0").toDouble(),
			_Element.attribute("y", "0").toDouble());


	setMonth(_Element.attribute("month", "1").toInt(),_Element.attribute("year", "2000").toInt());
	setTextWidth(_Element.attribute("textwidth", "-1").toDouble());

	//--- Char Format
	QString OutlineCol = _Element.attribute("outlinecolor", "");
	if (!OutlineCol.isEmpty())
	{
		QTextCharFormat Format;
		Format.setTextOutline(QColor(OutlineCol));
		QTextCursor Cursor = textCursor();
		Cursor.select(QTextCursor::Document);
		Cursor.mergeCharFormat(Format);
	}

	setTransform(AbstractGraphicsItem::loadTransformElement(_Element));
	AbstractGraphicsItem::loadEffectElements(this,  _Element);
	AbstractGraphicsItem::updateToolTip();
}

QDomElement GraphicsMonthItem::createElement(QDomDocument& _Doc, const QString& _StoreDir) const
{
	QDomElement MElement = _Doc.createElement(tagName());

	//Font
	MElement.setAttribute("font", font().family());
	MElement.setAttribute("fontsize", font().pointSize());

	MElement.setAttribute("showmonth", showMonthLabel() ? "true" : "false");
	MElement.setAttribute("showdays", showDayLabels() ? "true" : "false");
	MElement.setAttribute("textcolor", textColor().name());

	//Geometry
	MElement.setAttribute("x", pos().x());
	MElement.setAttribute("y", pos().y());
	MElement.setAttribute("textwidth", textWidth());
	MElement.setAttribute("month", month());
	MElement.setAttribute("year", year());

	//--- Char Format
	if (textCursor().charFormat().textOutline() != Qt::NoPen)
		MElement.setAttribute("outlinecolor", textCursor().charFormat().textOutline().color().name());


	//QDomText CText = _Doc.createTextNode(toHtml());
	//MElement.appendChild(CText);
	MElement.appendChild(AbstractGraphicsItem::createTransformElement(this, _Doc));


	//Effects
	AbstractGraphicsItem::appendEffectElements(MElement, this, _Doc);

	return MElement;
}


void GraphicsMonthItem::addHolyDay(const QString& _DayAndMonth)
{
	HolidayList.push_back(QDate::fromString(_DayAndMonth, "dd/MM"));
}

void GraphicsMonthItem::addHolyDay(const QDate& _Date)
{
	HolidayList.push_back(_Date);
}

void GraphicsMonthItem::clearHolyDays()
{
	HolidayList.clear();
}

void GraphicsMonthItem::addDefaultHolyDays(int _Year)
{
	addHolyDay("01/01");
	addHolyDay("06/01");
	addHolyDay("01/05");
	addHolyDay("24/06");
	addHolyDay("15/08");
	addHolyDay("11/09");
	addHolyDay("12/10");
	addHolyDay("06/12");
	addHolyDay("08/12");
	addHolyDay("25/12");
	addHolyDay("26/12");
	addHolyDay(STUtils::pascuaDate(_Year));
}

