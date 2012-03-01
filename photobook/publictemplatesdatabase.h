/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef PUBLICTEMPLATESDATABASE_H
#define PUBLICTEMPLATESDATABASE_H

#include <QSqlDatabase>
#include "stphotobookexport.h"
#include "sterror.h"
#include "templateinfolist.h"

namespace SPhotoBook
{

class TemplateInfo;
class DesignInfo;
class ST_PHOTOBOOK_EXPORT PublicTemplatesDatabase : public QSqlDatabase
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	static const QString DefaultPublicTemplatesDBConnectionName;
	void createTemplatesTable();
	void ensureCreated();
	void removeDInfo(const TemplateInfo& _Template, const DesignInfo& _Design);
	void updateDInfo(const TemplateInfo& _Template, const DesignInfo& _Design);

public:
	explicit PublicTemplatesDatabase(const QString& _DatabaseName);
	PublicTemplatesDatabase(const QSqlDatabase& _Other = QSqlDatabase::database(DefaultPublicTemplatesDBConnectionName));
	//Adds database to QSqlDatabase active connections or replaces the old connection if a connection with _ConnectionName already exists.
	QSqlDatabase addDatabase(const QString& _DatabaseName, const QString& _ConnectionName = DefaultPublicTemplatesDBConnectionName);
	void updateTemplateInfo(const TemplateInfo& _Template);
	TemplateInfoList publicTemplatesInfoList() const;


};
}

#endif // PUBLICTEMPLATESDATABASE_H
