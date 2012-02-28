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

#include "fdbutils.h"
#include <QSqlDatabase>
#include <QSqlRecord> 
#include <QSqlRelationalTableModel>
#include "fsqlquery.h"

//! Retorna un QStringList amb els camps del registre _Record.
QStringList FDBUtils::fieldList(const QSqlRecord& _Record)
{
	QStringList Res; 
	for (int Vfor = 0; Vfor < _Record.count(); Vfor++)
		Res.push_back(_Record.fieldName(Vfor));	

	return Res;
}

//! Copia els registres de dues taules amb els mateixos noms de camps.
void FDBUtils::copyRecords(const QSqlDatabase& _Database, const QString& _FromTable, const QString& _ToTable, const QString& _Filter)
{
	FSqlQuery Query;
	QString FieldList = fieldList(_Database.record(_ToTable)).join(",");
	QString Filter; 
	if (!_Filter.isEmpty())
		Filter = " WHERE " + _Filter;
	Query.exec("INSERT INTO " + _ToTable + " (" + FieldList + ") " 
		"(SELECT " + FieldList + " FROM " + _FromTable + Filter + ")");
}

void FDBUtils::setRelationDefaults(QSqlRelationalTableModel* _Model, QSqlRecord& _Record)
{
	for (int Vfor=0; Vfor < _Model->columnCount(); Vfor++)
	{
		QSqlRelation Rel = _Model->relation(Vfor); 
		if (Rel.isValid())
		{
			FSqlQuery Query("SELECT " + Rel.indexColumn() + " FROM " +  Rel.tableName() );
			if (Query.next())
				_Record.setValue(Vfor, Query.value(0));
		}
	}
}

bool FDBUtils::isSqlite(const QSqlDatabase& _Database)
{
	return _Database.driverName().left(7).toUpper() == "QSQLITE";
}
