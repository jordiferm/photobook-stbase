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
#ifndef GRAPHICSMONTHITEM_H
#define GRAPHICSMONTHITEM_H

#include "graphicstextitem.h"
#include <QList> 
#include <QDate> 
#include <QColor> 
#include "stphotobookexport.h"

/**
Glass to generate calendar months graphic items.

	@author
*/

namespace STPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsMonthItem : public GraphicsTextItem
{
Q_OBJECT
public: 
	typedef QList<QDate> THolidayList; 
	enum { Type = UserType + 6 };

private:
	THolidayList HolidayList; 
	int Month, Year; 
	bool ShowDayLabels, ShowMonthLabel; 
	QColor TextColor; 

	void init(); 

public:
	GraphicsMonthItem(QGraphicsItem* parent = 0);
	//int type() const { return Type; } //Temporary the same type as TextItem.
	void scaleToSize(const QSizeF& _Size);
	QString longMonthName(int _Month);
	QString shortDayName(int _DayOfWeek);
	void setHolidays(const THolidayList& _List) { HolidayList = _List; }
	void setMonth(int _Month, int _Year) { Month = _Month; Year = _Year; }
	int month() { return Month; }
	int year() { return Year; }
	void setShowDayLabels(bool _Value) { ShowDayLabels = _Value; }
	void setShowMonthLabel(bool _Value) { ShowMonthLabel = _Value; }
	bool isHoliday(int _Day, int _Month);
	QString getMonthHtml();
	void setTextColor(const QColor& _Color) { TextColor = _Color; } 
};
}
#endif
