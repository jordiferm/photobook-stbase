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

#ifndef PRINTKEEPERLOGINMANAGER_H
#define PRINTKEEPERLOGINMANAGER_H
#include <QString>
#include <QUrl>

class PrintKeeperLoginManager
{
	QString UserName, Password, AccessKey;
	QUrl BaseUrl;

	bool loginSuccess(const QString& _UserName, const QString& _Password, const QString& _AccessKey);

public:
	PrintKeeperLoginManager(const QUrl& _BaseUrl);
	bool login(const QString& _UserName, const QString& _Password, const QString& _AccessKey);
	QString userName() { return UserName; }
	QString password() { return Password; }
	QString accessKey() { return AccessKey; }
};

#endif // PRINTKEEPERLOGINMANAGER_H
