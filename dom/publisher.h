/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QString>

namespace STDom
{

/**
	@author Shadow
*/
class Publisher
{

	QString Id, Name, Email, Web, LogoHtml;
	QString FtpUrl, UserName, Password, InitDir;
	int TransferMode, FtpPort;

public:
	Publisher();
	Publisher(const QString& _Id, const QString& _Name, const QString& _Url, const QString& _UserName, const QString& _Password);
	void setId(const QString& _Value) { Id = _Value; }
	QString id() const { return Id; }

	void setName(const QString& _Value) { Name = _Value; }
	QString name() const { return Name; }

	void setEmail(const QString& _Value) { Email = _Value; }
	QString email() const { return Email; }

	void setWeb(const QString& _Value) { Web = _Value; }
	QString web() const { return Web; }

	void setLogoHtml(const QString& _Value) { LogoHtml = _Value; }
	QString logoHtml() const { return LogoHtml; }

	void setFtpUrl(const QString& _Value) { FtpUrl = _Value; }
	QString ftpUrl() const { return FtpUrl; }

	void setUserName(const QString& _Value) { UserName = _Value; }
	QString userName() const { return UserName; }

	void setPassword(const QString& _Value) { Password = _Value; }
	QString password() const { return Password; }

	void setInitDir(const QString& _Value) { InitDir = _Value; }
	QString initDir() const { return InitDir; }

	void setTransferMode(int _Value) { TransferMode = _Value; }
	int transferMode() const { return TransferMode; }

	void setFtpPort(int _Value) { FtpPort = _Value; }
	int ftpPort() const { return FtpPort; }

};

}

#endif // PUBLISHER_H
