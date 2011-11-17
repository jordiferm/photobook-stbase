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
#ifndef STARLABMANAGER_H
#define STARLABMANAGER_H
#include <QObject>
#include <QString>
#include <QUrl>
#include <QTime>
#include <QList>
#include <QPair>
#include "publisher.h"
#include "sterror.h"
#include "storomelexport.h"
#include "starlababstractmanager.h"
#include "starlabtable.h"

class QNetworkAccessManager;
class QNetworkReply;

class ST_OROMEL_EXPORT StarlabManager : public QObject, public StarlabAbstractManager
{
	Q_OBJECT
public:
	ST_DECLARE_ERRORCLASS();

	QUrl StarlabUrl;
	QNetworkAccessManager* Manager;
	QNetworkReply* CurrReplyFinished;
	QString PublisherPath;
	QTime InitTime;
	static int DefaultTimeOutSecs;

	QString tableName(EnPublisherTable _Table);
	QString getResponse(const QString& _Path, const QList<QPair<QString, QString> >& _QueryItems = QList<QPair<QString, QString> >());
	QString post(const QString& _Path, const QString& _FileName, const QList<QPair<QString, QString> >& _QueryItems = QList<QPair<QString, QString> >());
	bool waitForReply(QNetworkReply* _Reply, int _TimeOutSecs = -1);
	QUrl templateInfoUrl(const SPhotoBook::TemplateInfo& _Template, const QString& _Path);

public:
	bool productsUpToDate(const QString& _Hash);
	void updateProductHash(const QString& _Hash);
	StarlabManager(const QString& _StarlabUri, const QString& _PublisherPath, QObject* _Parent = 0);
	STDom::Publisher getPublisher();
	QUrl infoUrl(const SPhotoBook::TemplateInfo& _Template);
	void editTemplateInfo(const SPhotoBook::TemplateInfo& _Template);
	StarlabTable getPublisherTable(EnPublisherTable _Table);
	QString getPublisherHash(const QString& _FileName);
	void setPublisherHash(const QString& _FileName, const QString& _Hash);
	void syncTemplateProducts(STDom::PublisherDatabase&, const SPhotoBook::TemplateInfo&  );
	void syncTemplate(const SPhotoBook::TemplateInfo& );

signals:

private slots:
	void slotReplyFinished(QNetworkReply* _Reply);
};

#endif // STARLABMANAGER_H
