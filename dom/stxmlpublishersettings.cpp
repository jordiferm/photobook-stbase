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
#include "stxmlpublishersettings.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile> 
#include <QTextStream> 
#include <QTextCodec>
#include "stcollectionpublishermodel.h"

using namespace STDom;

STXmlPublisherSettings::STXmlPublisherSettings()
{
	clear(); 
}

void STXmlPublisherSettings::clear()
{
	DBUser = DBPassword = DBHost = DBDir = ""; 
	CodPayment = CCPayment = PayPalPayment = false;
	TransferBackPrintAsFileName = false;
	HasDealer = false; 
	DBPort = Port = DealerPort = 21;
	UpdatesDir = "/updates";
}

bool STXmlPublisherSettings::loadXml(const QString& _FileName)
{
	bool Res = false;
	clear(); 
	QDomDocument Doc("settings");
	QFile File(_FileName);
	if (File.open(QIODevice::ReadOnly))
	{
		QTextStream StrIn(&File);
		StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
		QString Error; 
		int Line, Column; 
		Res = Doc.setContent(StrIn.readAll(), &Error, &Line, &Column);
		if (!Res)
			qWarning(QString("XML Parser Error: %1, at line: %2, Col: %3").arg(
				Error).arg(Line).arg(Column).toLatin1());
		
		File.close();
	}
	QDomElement RootEl = Doc.documentElement();
	Id = RootEl.attribute("id", "");
	Name = RootEl.attribute("name", "");
	Email = RootEl.attribute("email", "");
	Web = RootEl.attribute("web", "");

	QDomNode CNode = RootEl.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "database" )
			{
				DBUser = CEl.attribute("dbuser", ""); 
				DBPort = CEl.attribute("port", "21").toInt();
				DBPassword = CEl.attribute("dbpassword", "");
				DBHost = CEl.attribute("dbhost", ""); 
				DBDir = CEl.attribute("dbdir", ""); 
				DBTransferMode = CEl.attribute("dbtransfermode", "0").toInt(); 
			}
			else
			if (CEl.tagName().toLower() == "payment" )
			{
				CodPayment = CEl.attribute("cod", "").toLower() == "true";
				CCPayment = CEl.attribute("cc", "").toLower() == "true";
				PayPalPayment = CEl.attribute("paypal", "").toLower() == "true";
			}
			else 
			if (CEl.tagName().toLower() == "ftp")
			{
				Url = CEl.attribute("url", "");
				Port = CEl.attribute("port", "21").toInt();
				UserName = CEl.attribute("username", "");
				Password = CEl.attribute("password", "");
				InitDir = CEl.attribute("initdir", "/");
				UpdatesDir = CEl.attribute("updatesdir", "/updates");
				TransferMode = CEl.attribute("transfermode", "0").toInt(); 
				TransferBackPrintAsFileName = CEl.attribute("bptextasfilename", "false").toLower() == "true";
			}
			if (CEl.tagName().toLower() == "dealerftp")
			{
				HasDealer = true; 
				DealerUrl = CEl.attribute("url", "");
				DealerPort = CEl.attribute("port", "21").toInt();
				DealerUserName = CEl.attribute("username", "");
				DealerPassword = CEl.attribute("password", "");
				DealerInitDir = CEl.attribute("initdir", "/");
				DealerTransferMode = CEl.attribute("transfermode", "0").toInt(); 
			}
		}
		CNode = CNode.nextSibling();
	}
	return Res;	
}


QString STXmlPublisherSettings::dbUser() const
{
	QString Res;
#ifdef PUBLISHER_LOGIN
	Res = PUBLISHER_LOGIN;
#endif
	if (Res.isEmpty())
		Res = DBUser;
	return Res;
}

QString STXmlPublisherSettings::dbPassword() const
{
	QString Res;
#ifdef PUBLISHER_PASSWD
	Res = PUBLISHER_PASSWD;
#endif
	if (Res.isEmpty())
		Res = DBPassword;
	return Res;
}

QString STXmlPublisherSettings::dbHost() const
{
	QString Res;
#ifdef PUBLISHER_URL
	Res = PUBLISHER_URL;
#endif
	if (Res.isEmpty())
		Res = DBHost;
	return Res;
}

QString STXmlPublisherSettings::url() const
{
	QString Res;
#ifdef PUBLISHER_URL
	Res = PUBLISHER_URL;
#endif
	if (Res.isEmpty())
		Res = Url;
	return Res;
}

QString STXmlPublisherSettings::userName() const
{
	QString Res;
#ifdef PUBLISHER_LOGIN
	Res = PUBLISHER_LOGIN;
#endif
	if (Res.isEmpty())
		Res = UserName;
	return Res;
}

QString STXmlPublisherSettings::password() const
{
	QString Res;
#ifdef PUBLISHER_PASSWD
	Res = PUBLISHER_PASSWD;
#endif
	if (Res.isEmpty())
		Res = Password;
	return Res;
}


STXmlPublisherSettings STXmlPublisherSettings::getDefaultSettings()
{
	STXmlPublisherSettings PXmlS;
	STCollectionPublisherModelDep* Model = new STCollectionPublisherModelDep;
	Model->loadPublishers();
	if (Model->rowCount() > 0)
	{
		STCollectionPublisherInfoDep PublisherInfo = Model->publisherInfo(Model->index(0,0));
		delete Model;

		QFileInfo PublXml = PublisherInfo.publisherXmlFile();

		Assert(PXmlS.loadXml(PublXml.absoluteFilePath()), Error(QString(QObject::tr("Could not load settings file: %1")).arg(PublXml.absoluteFilePath())));
	}
	return PXmlS;
}

bool STXmlPublisherSettings::isEmpty() const
{
	return url().isEmpty();
}
