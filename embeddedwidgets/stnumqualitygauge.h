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

#ifndef STNUMQUALITYGAUGE_H
#define STNUMQUALITYGAUGE_H
#include "stembeddedwidgetsexport.h"

#include <QWidget>

class QtSvgDialGauge;
class QLabel;
class ST_EMBEDDEDWIDGETS_EXPORT STNumQualityGauge : public QWidget
{
Q_OBJECT
	int RangeFrom, RangeTo, CurrentValue;
	QtSvgDialGauge* Gauge;
	QLabel* InfoLabel;

	void calcGaugeValue();
	void updateInfoLabel();

public:
    explicit STNumQualityGauge(QWidget *parent = 0);
	void setOptimalQualityRange(int _From, int _To);
signals:

public slots:
	void setValue(int _Value);

};

#endif // STNUMQUALITYGAUGE_H
