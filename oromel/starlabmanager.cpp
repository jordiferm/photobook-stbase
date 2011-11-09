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

#include "starlabmanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QApplication>
#include <QList>
#include <QDesktopServices>

int StarlabManager::DefaultTimeOutSecs = 10000;

/*!
	\return false when there is a time out on operation.
*/
bool StarlabManager::waitForReply(QNetworkReply* _Reply, int _TimeOutSecs)
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

QUrl StarlabManager::templateInfoUrl(const SPhotoBook::TemplateInfo& _Template, const QString& _Path)
{
	QUrl Res(StarlabUrl);
	Res.setPath(Res.path() + QString("/%1").arg(_Path));
	typedef QPair<QString, QString> TQItem;
	QList< TQItem> QueryItems;
	QueryItems << QPair<QString, QString>("templateref", _Template.name());
	Res.setQueryItems(QueryItems);
	qDebug() << Res.toString();
	return Res;
}

StarlabManager::StarlabManager(const QString& _StarlabUri, const QString& _PublisherPath, QObject* _Parent) :
	QObject(_Parent), CurrReplyFinished(0), PublisherPath(_PublisherPath)
{
	StarlabUrl.setUrl(_StarlabUri);
	QString PubPath = _PublisherPath;
	PubPath.remove("/");
	StarlabUrl.setPath(PubPath);
	Manager = new QNetworkAccessManager(this);
	connect(Manager, SIGNAL(finished(QNetworkReply*)),
			 this, SLOT(slotReplyFinished(QNetworkReply*)));
}

STDom::Publisher StarlabManager::getPublisher()
{
	STDom::Publisher Res;
	QNetworkRequest Request;
	QUrl RequestUrl = StarlabUrl;
	RequestUrl.setPath(RequestUrl.path() + "/get_publisher_info");

	Request.setUrl(RequestUrl);
	Request.setRawHeader("User-Agent", "Starblitzlab Client 1.0");
	QNetworkReply* Reply = Manager->get(Request);

	Assert(waitForReply(Reply), Error("Time out waiting for login reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("EPrintKeeper error connecting for loggin: %1").arg(Reply->errorString())));
	QStringList ResponseStrList = Response.split("|");
	Assert(ResponseStrList.size() >= 8, Error(tr("Invalid number of parameters returned from server expected >=9, returned %s").arg(ResponseStrList.size())));
	Res.setName(ResponseStrList[0]);
	Res.setEmail(ResponseStrList[1]);
	Res.setWeb(ResponseStrList[2]);
	Res.setLogoHtml(ResponseStrList[3]);
	Res.setFtpUrl(ResponseStrList[4]);
	Res.setInitDir(ResponseStrList[5]);
	Res.setTransferMode(ResponseStrList[6].toInt());
	Res.setFtpPort(ResponseStrList[7].toInt());
	Res.setId(PublisherPath);

	//qDebug() << "My response ->" << Response.split("|");
	return Res;
}

QUrl StarlabManager::infoUrl(const SPhotoBook::TemplateInfo& _Template)
{
	return templateInfoUrl(_Template, "view_templateinfo");
}

void StarlabManager::editTemplateInfo(const SPhotoBook::TemplateInfo& _Template)
{
	QUrl EditTemplateUrl = templateInfoUrl(_Template, "edit_templatename");
	QDesktopServices::openUrl(EditTemplateUrl);

}



void StarlabManager::slotReplyFinished(QNetworkReply* _Reply)
{
	CurrReplyFinished = _Reply;
}
