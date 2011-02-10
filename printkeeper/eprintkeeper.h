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

#ifndef EPRINTKEEPER_H
#define EPRINTKEEPER_H

#include <QObject>
#include <QUrl>
#include <QTime>
#include "pkprinterinfo.h"
#include "sterror.h"

class QNetworkAccessManager;
class QNetworkReply;
class EPrintKeeper : public QObject
{
Q_OBJECT

public:
	ST_DECLARE_ERRORCLASS();

private:
	QString UserRealName;
	QString UserEmail;
	QString UserName, Password;
	QString ErrorInfo;
	QUrl RemoteUrl;
	QString PrintAccessKey;
	QNetworkAccessManager* Manager;
	QNetworkReply* CurrReplyFinished;
	QTime InitTime;
	static int DefaultTimeOutSecs;
	void getUserData(const QString& _UserData);
	PKPrinterInfo getPrinterInfo(const QString& _InfoString) const;
	QString errorString(int _ErrorNum) const;
	bool waitForReply(QNetworkReply* _Reply, int _TimeOutSecs = -1);

public:
	explicit EPrintKeeper(QObject *parent = 0);
	void setRemoteUrl(const QUrl& _RemoteUrl) { RemoteUrl = _RemoteUrl; }
	bool login(const QString& _User, const QString& _Password);
	bool isValidKey(const QString& _Key);
	QString userRealName() { return UserRealName; }
	QString userEmail() { return UserEmail; }
	QString errorInfo() const { return ErrorInfo; }
	void setPrintAccessKey(const QString& _Key) { PrintAccessKey = _Key; }
	bool access(const QString& _PrinterName, PKPrinterInfo& _Info, qlonglong _Copies);

signals:

private slots:
	void slotReplyFinished(QNetworkReply* _Reply);

};

#endif // EPRINTKEEPER_H
