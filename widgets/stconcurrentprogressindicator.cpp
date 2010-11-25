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

#include "stconcurrentprogressindicator.h"
#include <QApplication>

const QEvent::Type STConcurrentProgressIndicatorEvent::STConcurrentProgressIndicatorEventType = static_cast<QEvent::Type>(QEvent::User + 3);


void STConcurrentProgressIndicator::start(const QString& _Message)
{
	STConcurrentProgressIndicatorEvent* Event = new STConcurrentProgressIndicatorEvent(STConcurrentProgressIndicatorEvent::TypeStart);
	Event->setStartMessage(_Message);
	Event->setRange(RangeFrom, RangeTo);
	QApplication::postEvent(EventHandler, Event);
}

void STConcurrentProgressIndicator::setValue(int _Value)
{
	STConcurrentProgressIndicatorEvent* Event = new STConcurrentProgressIndicatorEvent(STConcurrentProgressIndicatorEvent::TypeProgressChanged);
	Event->setProgressValue(_Value);
	Event->setRange(RangeFrom, RangeTo);
	LastValue = _Value;
	QApplication::postEvent(EventHandler, Event);
}

void STConcurrentProgressIndicator::incValue()
{
	setValue(LastValue +1);
}

void STConcurrentProgressIndicator::stop()
{
	STConcurrentProgressIndicatorEvent* Event = new STConcurrentProgressIndicatorEvent(STConcurrentProgressIndicatorEvent::TypeStop);
	QApplication::postEvent(EventHandler, Event);
}

