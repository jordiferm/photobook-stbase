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

#ifndef SINDARDATABASE_H
#define SINDARDATABASE_H
#include "stsindarlexport.h"
#include <QString>
#include <QSqlDatabase>
#include "stphotolayout.h"
#include "sterror.h"
#include "publisherdatabase.h"
#include "stsindarlexport.h"

class ST_SINDARL_EXPORT DefaultSindarDatabase : public QSqlDatabase
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	QString EmbedFileName;
	QString Name;
	QString Description;
public:
	DefaultSindarDatabase();
	DefaultSindarDatabase(const QString& _Name, const QString& _EmbedFileName, const QString& _Description = "");
	void setDescription(const QString& _Description) { Description = _Description; }
	QString description() const { return Description; }
	QString name() const { return Name; }
	bool open();
	void exportPublisherDB(const QString& _FileName);
};

class QFileInfo;
class ST_SINDARL_EXPORT SindarDatabase : public QSqlDatabase
{
public:
	ST_DECLARE_ERRORCLASS();
	typedef QList<DefaultSindarDatabase> TDefaultDatabaseList;

private:
	STDom::PublisherDatabase::EnProductType productTypeFromLayoutType(STPhotoLayout::EnLayoutType _Type);

public:
	SindarDatabase(const QSqlDatabase& _Other = QSqlDatabase::database());
	int insertProductsForAllTemplates(STPhotoLayout::EnLayoutType _Type);
	//! \return The num products inserted.
	int insertProductsForAllTemplates();
	int importTemplateRefs(STPhotoLayout::EnLayoutType _Type);
	int importTemplateRefs();
	static SindarDatabase::TDefaultDatabaseList getDefaultDatabases();
	void importDefaultPublisherProducts(const QFileInfo& _PubliserDbFile);

};

#endif // SINDARDATABASE_H
