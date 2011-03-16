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

#ifndef STOMWATCHDOG_H
#define STOMWATCHDOG_H
#include "stordermanagerlexport.h"
#include <QObject>

class QTimer;
class ST_ORDERMANAGERL_EXPORT STOmWatchDog : public QObject
{
Q_OBJECT

	QTimer* Timer;

public:
	STOmWatchDog(QObject* _Parent = 0);
	void watch();
	static bool isOmAlive();
	static void runOm();
	static void runOmIfNotAlive();

public slots:
	void update();
};

#endif // STTOMWATCHDOG_H
