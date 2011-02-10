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

#include "eprintkeeper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QDebug>
#include <QRegExp>
#include <QStringList>


int EPrintKeeper::DefaultTimeOutSecs = 10000;


void EPrintKeeper::getUserData(const QString& _UserData)
{
	QStringList Fields = _UserData.split("|");
	if (Fields.size() > 1)
	{
		UserRealName = Fields[0];
		if (Fields[1].toLower() != "none")
			UserEmail = Fields[1];
	}
	else
		UserRealName = _UserData;
}

PKPrinterInfo EPrintKeeper::getPrinterInfo(const QString& _InfoString) const
{
	PKPrinterInfo Res;
	QStringList Fields = _InfoString.split("|");
	if (Fields.size() > 1)
	{
		Res.setIp(Fields[0]);
		Res.setMAC(Fields[1]);
	}
	return Res;
}

QString EPrintKeeper::errorString(int _ErrorNum) const
{
	QString Res;
	switch (_ErrorNum)
	{
		case 100 :
			Res = "Copies limit exceeded";
		break;
		case 101 :
			Res = "Num Copies under current";
		break;
		case 102 :
			Res = "Due date expired";
		break;
		case 104:
			Res = "Invalid Key";
		break;
		case 105:
			Res = "Invalid password";
		break;
		case 106 :
			Res = "Invalid user";
		break;
	}
	return Res;
}

/*!
	\return false when there is a time out on operation.
*/
bool EPrintKeeper::waitForReply(QNetworkReply* _Reply, int _TimeOutSecs)
{
	int TimeOutSecs;
	if (_TimeOutSecs = -1)
		TimeOutSecs = DefaultTimeOutSecs;
	else
		TimeOutSecs = _TimeOutSecs;

	CurrReplyFinished = 0;
	bool TimeOut = false;
	InitTime = QTime::currentTime();
	while(CurrReplyFinished != _Reply && !TimeOut)
	{
		QApplication::processEvents();
		TimeOut = (InitTime.secsTo(QTime::currentTime()) >= TimeOutSecs) ;
	}
	return !TimeOut;
}


EPrintKeeper::EPrintKeeper(QObject *parent) :
	QObject(parent), CurrReplyFinished(0)
{
	Manager = new QNetworkAccessManager(this);
	connect(Manager, SIGNAL(finished(QNetworkReply*)),
			 this, SLOT(slotReplyFinished(QNetworkReply*)));

}

bool EPrintKeeper::login(const QString& _User, const QString& _Password)
{
	UserName = _User;
	Password = _Password;

	QNetworkRequest Request;
	QUrl RequestUrl = RemoteUrl;
	RequestUrl.setPath("userlogin");
	RequestUrl.addQueryItem("username", _User);
	RequestUrl.addQueryItem("password", _Password);

	Request.setUrl(RequestUrl);
	Request.setRawHeader("User-Agent", "EPrintKeeper Client 1.0");
	QNetworkReply* Reply = Manager->get(Request);

	Assert(waitForReply(Reply), Error("Time out waiting for login reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("EPrintKeeper error connecting for loggin: %1").arg(Reply->errorString())));
	bool LoginOk = Response.contains("OK");
	if (LoginOk)
		getUserData(Response.right(Response.length() - Response.indexOf("-") - 1));

	Reply->deleteLater();
	return LoginOk;
}

//!!!! TODO: Make an apiCall method.
bool EPrintKeeper::isValidKey(const QString& _Key)
{
	bool Res;
	QNetworkRequest Request;
	QUrl RequestUrl = RemoteUrl;
	RequestUrl.setPath("validkey");
	RequestUrl.addQueryItem("username", UserName);
	RequestUrl.addQueryItem("password", Password);
	RequestUrl.addQueryItem("key", _Key);

	Request.setUrl(RequestUrl);
	Request.setRawHeader("User-Agent", "EPrintKeeper Client 1.0");
	QNetworkReply* Reply = Manager->get(Request);

	Assert(waitForReply(Reply), Error("Time out waiting for valid key reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("EPrintKeeper error checking key: %1").arg(Reply->errorString())));
	Res = Response.contains("OK");

	Reply->deleteLater();
	return Res;
}

bool EPrintKeeper::access(const QString& _PrinterName, PKPrinterInfo& _Info, qlonglong _Copies)
{
	bool Res = false;
	QUrl RequestUrl = RemoteUrl;
	RequestUrl.setPath("printaccess");
	RequestUrl.addQueryItem("username", UserName);
	RequestUrl.addQueryItem("password", Password);
	RequestUrl.addQueryItem("key", PrintAccessKey);
	RequestUrl.addQueryItem("printername", _PrinterName);
	RequestUrl.addQueryItem("copies", QString::number(_Copies));

	QNetworkRequest Request;
	Request.setUrl(RequestUrl);
	Request.setRawHeader("User-Agent", "EPrintKeeper Client 1.0");
	QNetworkReply* Reply = Manager->get(Request);
	Assert(waitForReply(Reply), Error("Time out waiting for login reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("EPrintKeeper error retrieving access: %1").arg(Reply->errorString())));

	Reply->deleteLater();
	if (Response.contains("OK"))
	{
		Res = true;
		_Info = getPrinterInfo(Response.right(Response.length() - Response.indexOf("-") - 1));
	}
	else
	{
		int ErrorNum = Response.right(Response.length() - Response.indexOf(":") - 1).toInt();
		ErrorInfo = errorString(ErrorNum);
	}

	return Res;
}

void EPrintKeeper::slotReplyFinished(QNetworkReply* _Reply)
{
	CurrReplyFinished = _Reply;
}
