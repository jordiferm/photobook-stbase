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

#ifndef STMONTHSELECTOR_H
#define STMONTHSELECTOR_H
#include "stembeddedwidgetsexport.h"

#include <QWidget>
#include <QDate>

class QtScrollWheel;
class QLabel;
class ST_EMBEDDEDWIDGETS_EXPORT STMonthSelector : public QWidget
{
Q_OBJECT
	QDate FromDate;
	QDate ToDate;
	QDate CurrentDate;
	QLabel* YearLabel;
	QLabel* MonthLabel;
	QtScrollWheel* ScrWheel;
	void setCurrentDate(const QDate& _Date);
	int monthsTo(const QDate& _FromDate, const QDate& _ToDate);

public:
    explicit STMonthSelector(QWidget *parent = 0);
	void setDateRange(const QDate& _From, const QDate& _To);
	void setDate(const QDate& _Date);
	QDate currentDate() const { return CurrentDate; }

signals:
	void dateChanged(const QDate& _Date);

public slots:
	void slotWheelValueChanged(int _Value);

};

#endif // STMONTHSELECTOR_H
