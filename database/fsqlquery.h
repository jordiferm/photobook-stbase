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

#ifndef FSQLQUERY_H
#define FSQLQUERY_H

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QStringList>
#include <QVariant>
#include "sterror.h"
#include "stdatabaseexport.h"


/**
Classe de query amb control d'errors per exepcions.

@author Shadow
*/
class QSqlRecord;
class ST_DATABASE_EXPORT FSqlQuery : public QSqlQuery
{

	static bool LogErrors;
	bool LogExecs;
	
public:
class Error : public STError
	{

	public:
		Error(const QSqlQuery& _Query, QSqlError::ErrorType _Type, const QString& _AppendedString = "");
		static QString typeToStr(QSqlError::ErrorType);
	};
	QSqlDatabase Database;
private:
	void checkErrors();

public:
	FSqlQuery(const QString& _Query = QString(), QSqlDatabase _Db = QSqlDatabase::database() );
	FSqlQuery ( QSqlDatabase _Db );
	QSqlDatabase database() const { return Database; }
	//! Indica si ha de fer log dels EXEC.(Per defecte a false)
	void setLogExecs(bool _Value);
	bool exec();
	bool exec(const QString& _Value);
	bool execLines(const QString& _Value);
	static void setLogErrors(bool ); 
	static bool logErrors();
	QString sqlStatement(QSqlDriver::StatementType _Type, const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly, bool _IgnoreNullFields, const QStringList& _IgnoreFieldNames = QStringList());
	void prepareInsert(const QSqlRecord& _Rec, const QString& _TableName);
	void prepareUpdate(const QSqlRecord& _Rec, const QString& _TableName);
	void prepareSelect(const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly = true, bool _IgnoreNullFields = true, const QStringList& _IgnoreFieldNames = QStringList());
	static bool exists(const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly = true, bool _IgnoreNullFields = true, const QStringList& _IgnoreFieldNames = QStringList(), QSqlDatabase _Db = QSqlDatabase::database());
	static void insert(const QSqlRecord& _Rec, const QString& _TableName, QSqlDatabase _Db = QSqlDatabase::database());
	static void update(const QSqlRecord& _Rec, const QString& _TableName, QSqlDatabase _Db = QSqlDatabase::database());
	static QVariant sequenceNextVal(const QString& _TableName, const QString& _FieldName, QSqlDatabase _Db = QSqlDatabase::database());
};

#endif
