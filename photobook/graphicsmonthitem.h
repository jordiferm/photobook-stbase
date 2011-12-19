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

namespace SPhotoBook
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

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* _Event);

public:
	GraphicsMonthItem(QGraphicsItem* parent = 0);
	int type() const { return Type; }
	void scaleToSize(const QSizeF& _Size);
	QString longMonthName(int _Month) const;
	QString shortDayName(int _DayOfWeek) const;
	void setHolidays(const THolidayList& _List) { HolidayList = _List; }
	void setMonth(int _Month, int _Year);
	int month() const { return Month; }
	int year() const { return Year; }
	void setShowDayLabels(bool _Value) { ShowDayLabels = _Value; }
	bool showDayLabels() const { return ShowDayLabels; }
	void setShowMonthLabel(bool _Value) { ShowMonthLabel = _Value; }
	bool showMonthLabel() const { return ShowMonthLabel; }
	bool isHoliday(int _Day, int _Month) const;
	QString getMonthHtml() const;
	void setTextColor(const QColor& _Color) { TextColor = _Color; } 
	QColor textColor() const { return TextColor; }
	void loadElement(const QDomElement& _Element, const QString& _LoadDir = "");
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const;
	static QString tagName() { return "monthitem"; }
	void addHolyDay(const QString& _DayAndMonth);
	void addHolyDay(const QDate& _Date);
	void clearHolyDays();
	void addDefaultHolyDays(int _Year);

};
}
#endif
