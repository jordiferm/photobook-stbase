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

#include "publictemplatesdatabase.h"
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>
#include "templateinfo.h"

using namespace SPhotoBook;


void PublicTemplatesDatabase::createTemplatesTable()
{
	QSqlQuery Query(*this);

	Query.exec("CREATE TABLE IF NOT EXISTS templates (name TEXT, width NUMERIC, height NUMERIC, design TEXT, description TEXT, version NUMERIC, type NUMERIC, PRIMARY KEY (name, design, width, height))");
}

void PublicTemplatesDatabase::ensureCreated()
{
	if (!tables().contains("templates"))
	{
		createTemplatesTable();
	}

}

void PublicTemplatesDatabase::removeDInfo(const TemplateInfo& _Template, const DesignInfo& _Design)
{
	QSqlQuery Query(*this);
	//DELETE if exist
	Query.prepare("DELETE FROM templates WHERE name=:name AND design=:design AND width=:width AND height=:height");
	Query.bindValue(":name", _Template.name());
	Query.bindValue(":design", _Design.name());
	Query.bindValue(":width", _Template.size().width());
	Query.bindValue(":height", _Template.size().height());
	Query.exec();
}

void PublicTemplatesDatabase::updateDInfo(const TemplateInfo& _Template, const DesignInfo& _Design)
{
	QSqlQuery Query(*this);
	//Update if exist
	Query.prepare("SELECT * FROM templates WHERE name=:name AND design=:design AND width=:width AND height=:height");
	Query.bindValue(":name", _Template.name());
	Query.bindValue(":design", _Design.name());
	Query.bindValue(":width", _Template.size().width());
	Query.bindValue(":height", _Template.size().height());
	Query.exec();
	if (Query.next())
	{
		Query.prepare("UPDATE templates SET description=:description, version=:version, type=:type "
					  " WHERE name=:name AND design=:design AND width=:width AND height=:height");
		Query.bindValue(":name", _Template.name());
		Query.bindValue(":design", _Design.name());
		Query.bindValue(":width", _Template.size().width());
		Query.bindValue(":height", _Template.size().height());
		Query.bindValue(":description", _Design.description());
		Query.bindValue(":version", _Design.metaInfo().version());
		Query.bindValue(":type", _Template.type());
		Assert(Query.exec(), Error(QObject::tr("Error updating templates %1").arg(Query.lastError().text())));
	}
	else
	{
		Query.prepare("INSERT INTO templates (name, design, width, height, description, version, type)"
					  " VALUES (:name, :design, :width, :height, :description, :version, :type)");
		Query.bindValue(":name", _Template.name());
		Query.bindValue(":design", _Design.name());
		Query.bindValue(":width", _Template.size().width());
		Query.bindValue(":height", _Template.size().height());
		Query.bindValue(":description", _Design.description());
		Query.bindValue(":version", _Design.metaInfo().version());
		Query.bindValue(":type", _Template.type());
		Assert(Query.exec(), Error(QObject::tr("Error updating templates %1").arg(Query.lastError().text())));
	}
}

const QString PublicTemplatesDatabase::DefaultPublicTemplatesDBConnectionName = "PublicTemplatesDatabase";

PublicTemplatesDatabase::PublicTemplatesDatabase(const QString& _DatabaseName) : QSqlDatabase("QSQLITE")
{
	setDatabaseName(_DatabaseName);
}

PublicTemplatesDatabase::PublicTemplatesDatabase(const QSqlDatabase& _Other ) : QSqlDatabase(_Other)
{

}

QSqlDatabase PublicTemplatesDatabase::addDatabase(const QString& _DatabaseName, const QString& _ConnectionName)
{
	return QSqlDatabase::cloneDatabase(PublicTemplatesDatabase(_DatabaseName), _ConnectionName);
}

void PublicTemplatesDatabase::updateTemplateInfo(const TemplateInfo& _Template)
{
	ensureCreated();
	//For each design
	DesignInfoList Designs = _Template.designs();
	DesignInfoList::const_iterator it;
	for (it = Designs.begin(); it != Designs.end(); ++it)
	{
		DesignInfo DInfo = *it;
		if (DInfo.isPublic())
			updateDInfo(_Template, DInfo);
		else
			removeDInfo(_Template, DInfo);
	}
}

TemplateInfoList PublicTemplatesDatabase::publicTemplatesInfoList() const
{
	TemplateInfoList Res;
	QSqlQuery Query(*this);
	//Update if exist
	Query.prepare("SELECT * FROM templates GROUP BY name, width, height");
	Query.exec();
	//For each template
	while (Query.next())
	{
		QSqlRecord CRecord = Query.record();
		TemplateInfo CTemplate(CRecord.value("name").toString(), static_cast<MetaInfo::EnTemplateType>(CRecord.value("type").toInt()));
		CTemplate.setSize(QSizeF(CRecord.value("width").toDouble(), CRecord.value("height").toDouble()));

		//For Each design
		QSqlQuery MQuery(*this);
		MQuery.prepare("SELECT * FROM templates WHERE name=:name AND width=:width AND height=:height");
		MQuery.bindValue(":name", CTemplate.name());
		MQuery.bindValue(":width", CTemplate.size().width());
		MQuery.bindValue(":height", CTemplate.size().height());
		MQuery.exec();
		while (MQuery.next())
		{
			QSqlRecord DRecord = MQuery.record();
			DesignInfo DInfo(DRecord.value("design").toString());
			DInfo.setDescription(DRecord.value("description").toString());
			DInfo.setPublicVersion(DRecord.value("version").toInt());
			CTemplate.addDesign(DInfo);
		}
		Res.push_back(CTemplate);
	}
	return Res;
}



