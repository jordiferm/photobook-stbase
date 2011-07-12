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
#include "graphicsmonthitem.h"
#include <QTextStream>
#include <QFont>

using namespace SPhotoBook;

void GraphicsMonthItem::init()
{
	QDate CDate = QDate::currentDate(); 
	Month = CDate.month(); 
	Year = CDate.year(); 
	ShowDayLabels = true; 
	ShowMonthLabel = true; 
	TextColor = QColor("#FF0000"); 
}

GraphicsMonthItem::GraphicsMonthItem(QGraphicsItem* parent): GraphicsTextItem(parent)
{
	init();
}

void GraphicsMonthItem::scaleToSize(const QSizeF& _Size)
{
	scale(_Size.width() / boundingRect().width() , _Size.height() / boundingRect().height() );	
}

QString GraphicsMonthItem::longMonthName(int _Month)
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

QString GraphicsMonthItem::shortDayName(int _DayOfWeek)
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

bool GraphicsMonthItem::isHoliday(int _Day, int _Month)
{
	THolidayList::iterator it; 
	bool Found = false; 
	it = HolidayList.begin();
	while (!Found && it != HolidayList.end())
	{	
		Found = it->month() == _Month && it->day() == _Day; 
		++it; 
	}
	return Found; 

}

QString GraphicsMonthItem::getMonthHtml()
{
	QDate CDate; 
	CDate.setDate(Year, Month, 1); 
	QString Res; 
	QTextStream Strm(&Res);
	//Strm << "<table bgcolor=#00FF00 border=0 >";
	Strm << "<table border=0 >";
	Strm << "<tr><th colspan='7'><span style=\" font-size:40pt; font-family:'" <<  font().family() << "'\">" << "<font color=" << TextColor.name() << ">"  << longMonthName(Month) << "</font></span></th></tr>";
  	Strm << "<tr>";
	int Vfor; 
	for (Vfor=1; Vfor < 8; Vfor++)
	{
		Strm << "<th><span style=\" font-size:22pt; font-family:'" <<  font().family() << "'\">" << "<font color=" << TextColor.name() << ">"  << shortDayName(Vfor) << "&nbsp;</font></span></th>";
	}
  	Strm << "</tr><tr>";
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
    	Strm << "<td align=right><span style=\" font-size:30pt; font-family:'" <<  FontFamily << "'\"><font color=" << DayColor << ">" << Vfor +1 << "&nbsp;</font></span></td>";
		if (CDate.dayOfWeek() == 7)
			Strm << "</tr><tr>";
	}
	Strm << "</tr></table>";

	return Res; 
}




