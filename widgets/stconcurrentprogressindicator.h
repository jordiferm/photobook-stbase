/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STCONCURRENTPROGRESSINDICATOR_H
#define STCONCURRENTPROGRESSINDICATOR_H
#include "stprogressindicator.h"
#include <QObject>
#include <QEvent>
#include "stwidgetsexport.h"


class ST_WIDGETS_EXPORT STConcurrentProgressIndicatorEvent : public QEvent
{
public:
	enum EnEventType
	{
		TypeStart,
		TypeStop,
		TypeProgressChanged
	};

	static const QEvent::Type STConcurrentProgressIndicatorEventType;
private:
	int RangeFrom;
	int RangeTo;
	QString StartMessage;
	EnEventType ProgressType;
	int ProgressValue;

public:
	STConcurrentProgressIndicatorEvent(EnEventType _ProgressType) :
			QEvent(STConcurrentProgressIndicatorEventType), ProgressType(_ProgressType) {}
	void setRange(int _From, int _To) { RangeFrom = _From; RangeTo = _To; }
	int rangeFrom() const {  return RangeFrom; }
	int rangeTo() const { return RangeTo; }
	void setStartMessage(const QString _Value) { StartMessage = _Value; }
	QString startMessage() const { return StartMessage; }
	EnEventType progressType() const { return ProgressType; }
	void setProgressValue(int _Value) { ProgressValue = _Value; }
	int progressValue() const { return ProgressValue; }
};

class ST_WIDGETS_EXPORT STConcurrentProgressIndicator : public STProgressIndicator
{
	QObject* EventHandler;
	int RangeFrom, RangeTo;
	int LastValue;

public:
	STConcurrentProgressIndicator(QObject* _EventHandler) :
			EventHandler(_EventHandler), RangeFrom(0), RangeTo(100), LastValue(0) {}
	virtual void setRange(int _From, int _To) { RangeFrom = _From; RangeTo = _To; }
	virtual void start(const QString& _Message);
	virtual void setValue(int _Value);
	virtual void incValue();
	virtual void stop();
};

#endif // STCONCURRENTPROGRESSINDICATOR_H
