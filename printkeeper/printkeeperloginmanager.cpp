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

#include "printkeeperloginmanager.h"
#include <QDebug>
#include "printkeeper.h"
#include "smessagebox.h"
#include "logindialog.h"

bool PrintKeeperLoginManager::loginSuccess(const QString& _UserName, const QString& _Password, const QString& _AccessKey)
{
	bool Res = false;
	if (PrintKeeper::login(_UserName, _Password))
	{
		if (!PrintKeeper::userInfoIsComplete())
		{
			QUrl NewUrl = BaseUrl;
			NewUrl.setPath("updatedata");
			SMessageBox::information(0, QObject::tr("User info"), QObject::tr("User info is incomplete, please click <a href=\"%1\">Here</a> to complete it.").arg(NewUrl.toString()));
		}

		Res = PrintKeeper::isValidKey(_AccessKey);
		if (!Res)
			SMessageBox::critical(0, QObject::tr("User login"), QObject::tr("Invalid access key, please check your account data"));
		else
			PrintKeeper::setPrintAccessKey(_AccessKey);
	}
	else
		SMessageBox::critical(0, QObject::tr("User login"), QObject::tr("Login fails, please check your account data"));
	return Res;
}

PrintKeeperLoginManager::PrintKeeperLoginManager(const QUrl& _BaseUrl) : BaseUrl(_BaseUrl)
{
	PrintKeeper::enableEPrintKeeper(true);
	PrintKeeper::setRemoteUrl(_BaseUrl);
}


bool PrintKeeperLoginManager::login(const QString& _UserName, const QString& _Password, const QString& _AccessKey)
{
	bool LoginSuccess = loginSuccess(_UserName, _Password, _AccessKey);
	bool Cancel = false;

	UserName = _UserName;
	Password = _Password;
	AccessKey = _AccessKey;
	LoginDialog LDialog(true);
	LDialog.setUserName(_UserName);
	LDialog.setPassword(_Password);
	LDialog.setKey(_AccessKey);
	QUrl NewUrl = BaseUrl;
	NewUrl.setPath("newaccount");
	LDialog.setNewAccountUrl(NewUrl);
	NewUrl.setPath("lostpassword");
	LDialog.setForgetPassword(NewUrl);
	while (!Cancel && !LoginSuccess)
	{
		Cancel = LDialog.exec() == QDialog::Rejected;
		if (!Cancel)
		{
			LoginSuccess = loginSuccess(LDialog.userName(), LDialog.password(), LDialog.key());
			if (LoginSuccess)
			{
				UserName = LDialog.userName();
				Password = LDialog.password();
				AccessKey = LDialog.key();
			}
		}
	}
	return LoginSuccess;
}
