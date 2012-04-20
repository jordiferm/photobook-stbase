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
#include <QDesktopServices>
#include <QDir>


int StarlabManager::DefaultTimeOutSecs = 10000;

QString StarlabManager::tableName(EnPublisherTable _Table)
{
	QString Res;
	switch (_Table)
	{
		case  StarlabAbstractManager::TableShippingMethods:
			Res = "ShippingMethod";
		break;
		case  StarlabAbstractManager::TablePaymentTypes:
			Res = "PaymentType";
		break;
		case  StarlabAbstractManager::TableCollectionPoints:
			Res = "CollectionPoints";
		break;
	}
	return Res;
}

QString StarlabManager::getResponse(const QString& _Path, const QList<StarlabManager::TResponsePair>& _QueryItems )
{
	QNetworkRequest Request;
	QUrl RequestUrl = StarlabUrl;
	RequestUrl.setPath(RequestUrl.path() + "/" + _Path);
	RequestUrl.setQueryItems(_QueryItems);

	Request.setUrl(RequestUrl);
	Request.setRawHeader("User-Agent", "Aili Imagelab Client 1.0");
	QNetworkReply* Reply = Manager->get(Request);

	Assert(waitForReply(Reply), Error("Time out waiting for login reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("Error connecting to Aili Image Service: %1").arg(Reply->errorString())));
	return Response;
}


QString StarlabManager::post(const QString& _Path, const QString& _FileName, const QList<TResponsePair >& _QueryItems )
{
	QNetworkRequest Request;
	QUrl RequestUrl = StarlabUrl;
	RequestUrl.setPath(RequestUrl.path() + "/" + _Path);
	RequestUrl.setQueryItems(_QueryItems);
	QFileInfo  FNameInfo(_FileName);

	Request.setUrl(RequestUrl);
	//Code from:http://developer.qt.nokia.com/forums/viewthread/11361

	QString bound="margin"; //name of the boundary
	//according to rfc 1867 we need to put this string here:
	QByteArray data(QString("--" + bound + "\r\n").toAscii());
	data.append(QString("Content-Disposition: form-data; name=\"uploaded\"; filename=\"%1\"\r\n").arg(FNameInfo.fileName()));  //name of the input is "uploaded" in my form, next one is a file name.
	data.append("Content-Type: text/plain\r\n\r\n"); //data type
	QFile File(FNameInfo.absoluteFilePath());
	Assert(File.open(QIODevice::ReadOnly), Error(tr("Error opening file to post %1").arg(_FileName)));
	data.append(File.readAll());   //let's read the file
	data.append("\r\n");
	data.append("--" + bound + "--\r\n");  //closing boundary according to rfc 1867
	Request.setRawHeader(QString("Content-Type").toAscii(),QString("multipart/form-data; boundary=" + bound).toAscii());
	Request.setRawHeader(QString("Content-Length").toAscii(), QString::number(data.length()).toAscii());
	QNetworkReply* Reply = Manager->post(Request,data);

	Assert(waitForReply(Reply), Error("Time out waiting for login reply"));

	QString Response = Reply->readAll();
	Assert(!Reply->error(), Error(tr("Error connecting to StarLab Service: %1").arg(Reply->errorString())));

	return Response;
}


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
	QString Response = getResponse("get_publisher_info");
	QStringList ResponseStrList = Response.split("|");
	Assert(ResponseStrList.size() >= 9, Error(tr("Invalid number of parameters returned from server expected >=8, returned %1").arg(ResponseStrList.size())));
	Res.setName(ResponseStrList[0]);
	Res.setEmail(ResponseStrList[1]);
	Res.setWeb(ResponseStrList[2]);
	Res.setLogoHtml(ResponseStrList[3]);
	Res.setFtpUrl(ResponseStrList[4]);
	Res.setInitDir(ResponseStrList[5]);
	Res.setTransferMode(ResponseStrList[6].toInt());
	Res.setFtpPort(ResponseStrList[7].toInt());
	Res.setPaymentUrl(ResponseStrList[8]);
	Res.setId(PublisherPath);

	return Res;
}

bool StarlabManager::productsUpToDate(const QString& _Hash)
{
	bool Res = false;
	try
	{
		QString ProductsHash = getPublisherHash("products");
		//qDebug() << "Comparing hashes: " << _Hash << "==" << ProductsHash;
		Res = ProductsHash == _Hash;
	}
	catch(...)
	{

	}
	return Res;
}

void StarlabManager::updateProductHash(const QString& _Hash)
{
	setPublisherHash("products", _Hash);
}

bool StarlabManager::templatesUpToDate(const QString& _Hash)
{
	bool Res = false;
	try
	{
		QString ProductsHash = getPublisherHash("templates");
		Res = ProductsHash == _Hash;
	}
	catch(...)
	{
	}
	return Res;

}

void StarlabManager::updateTemplateHash(const QString& _Hash)
{
	setPublisherHash("templates", _Hash);
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

StarlabTable StarlabManager::getPublisherTable(EnPublisherTable _Table)
{
	QList<QPair<QString, QString> > QueryItems;
	QString TableName = tableName(_Table);
	QueryItems.append(QPair<QString, QString>("tablename", TableName));
	QString Response = getResponse("get_publisher_table", QueryItems);
	StarlabTable Res(TableName);
	Res.setContent(Response);
	return Res;
}

QString StarlabManager::getPublisherHash(const QString& _FileName)
{
	QString Res = "";
	QList<QPair<QString, QString> > QueryItems;
	QueryItems.append(QPair<QString, QString>("filename", _FileName));
	Res = getResponse("get_publisher_hash", QueryItems);
	return Res;
}

void StarlabManager::setPublisherHash(const QString& _FileName, const QString& _Hash)
{
	QList<QPair<QString, QString> > QueryItems;
	QueryItems.append(QPair<QString, QString>("filename", _FileName));
	QueryItems.append(QPair<QString, QString>("hash", _Hash));
	QString Response = getResponse("set_publisher_hash", QueryItems);
	Assert(Response.toLower() == "ok", Error(tr("Error setting hash(%1) = %2").arg(_FileName).arg(_Hash)));
}

void StarlabManager::syncTemplateProducts(STDom::PublisherDatabase& _Database, const SPhotoBook::TemplateInfo&  _TemplateInfo)
{
	//Create a CSV file with all template products.
	QString CSVFileName = QDir::temp().absoluteFilePath("slmtmpproduct.csv");
	_Database.exportCSV(CSVFileName, _TemplateInfo.pubDatabaseProductType(), _TemplateInfo.name());
	QList<QPair<QString, QString> > QueryItems;
	QueryItems.append(QPair<QString, QString>("template", _TemplateInfo.name()));
	QString Response = post("update_products", CSVFileName, QueryItems);
	Assert(Response.toLower() == "ok", Error(tr("Error in sync Products.")));
}

//Creates if not exist only
void StarlabManager::syncTemplate(const SPhotoBook::TemplateInfo& _Template)
{
	QList<QPair<QString, QString> > QueryItems;
	QueryItems.append(QPair<QString, QString>("name", _Template.name()));
	QString Enable;
	if (_Template.hasPublicDesigns())
		Enable = "true";
	else
		Enable = "false";
	QueryItems.append(QPair<QString, QString>("enable", Enable));
	QString Response = getResponse("update_template", QueryItems);
	Assert(Response.toLower() == "ok", Error(tr("Error updating template %1").arg(_Template.name())));
}


void StarlabManager::slotReplyFinished(QNetworkReply* _Reply)
{
	CurrReplyFinished = _Reply;
}
