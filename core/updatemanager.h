/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H
#include <QString>
#include <QDir>
#include "sterror.h"
#include "stcoreexport.h"

class ST_CORE_EXPORT UpdateManager
{
	QString Host;
	int Port;
	QString User;
	QString Password;
	QString RemoteDir;
	int Minor, Major, Release;
	int TMode;

public:
	ST_DECLARE_ERRORCLASS();

public:
	UpdateManager(const QString& _Host = "", int _Port = 21, const QString& _User = "anonymous", const QString& _Password = "guest",
				  const QString& _RemoteDir = "/", int _TMode = 1);
	void setAppVersion(int _Major, int _Minor, int _Release);
	int major() const { return Major; }
	int minor() const { return Minor; }
	int release() const { return Release; }
	int transferMode() const { return TMode; }
	void getArguments();
	void checkForUpdates();
	QString host() const { return Host; }
	int port() const { return Port; }
	QString user() const { return User; }
	QString password() const { return Password; }
	QString remoteDir() const { return RemoteDir; }
	QString remoteDirBySystem() const;
	static QDir updatesDir();
	static void registerUpdateFile(const QString& _FileName);
	static QString updateFile();
	static bool updateAvailable();
	static void unregisterUpdateFile();
	static void installUpdates();

};

#endif // UPDATEMANAGER_H
