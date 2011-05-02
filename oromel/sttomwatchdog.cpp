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

#include "sttomwatchdog.h"
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QProcess>
#include <QCoreApplication>

STTomWatchDog::STTomWatchDog(QObject* _Parent) : QObject(_Parent)
{
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(update()));
}

void STTomWatchDog::watch()
{
	update();
	Timer->start(1000);
}

bool STTomWatchDog::isTomAlive()
{
	QSettings Settings("Starblitz", "TOM");
	QDateTime TimeStamp = Settings.value("watchdog/timestamp").toDateTime();
	return TimeStamp.addSecs(30) > QDateTime::currentDateTime();
}

void STTomWatchDog::runTom()
{
#ifndef Q_OS_WIN32
	QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/tombin");
#else
	QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/tombin.exe");
#endif
}

void STTomWatchDog::update()
{
	QSettings Settings("Starblitz", "TOM");
	Settings.setValue("watchdog/timestamp",QDateTime::currentDateTime());
}

