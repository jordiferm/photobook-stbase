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

#include "stomwatchdog.h"
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QProcess>
#include <QCoreApplication>

STOmWatchDog::STOmWatchDog(QObject* _Parent) : QObject(_Parent)
{
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(update()));
}

void STOmWatchDog::watch()
{
	update();
	Timer->start(1000);
}

bool STOmWatchDog::isOmAlive()
{
	QSettings Settings("Starblitz", "OM");
	QDateTime TimeStamp = Settings.value("watchdog/timestamp").toDateTime();
	return TimeStamp.addSecs(30) > QDateTime::currentDateTime();
}

void STOmWatchDog::runOm()
{
#ifndef Q_OS_WIN32
	QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/ordermanagerbin");
#else
	QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/ordermanagerbin.exe");
#endif
}

void STOmWatchDog::runOmIfNotAlive()
{
	if (!isOmAlive())
		runOm();
}

void STOmWatchDog::update()
{
	QSettings Settings("Starblitz", "OM");
	Settings.setValue("watchdog/timestamp",QDateTime::currentDateTime());
}


