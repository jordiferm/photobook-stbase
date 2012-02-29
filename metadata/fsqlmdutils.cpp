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

#include "fsqlmdutils.h"
#include <QObject> 
#include <QString>
#include <QTime>
#include <QDateTime>
#include "fsqlmetadata.h"
#include "fsqldatabasemanager.h" 



/*!
	Retorna una query preparada amb els valors filtrats per la taula relacionada _RelatedTableName de 
	la taula _TableMetaData. A partir dels valors que hi ha a _Values i la metainformaci�.
*/

FSqlQuery FSqlMDUtils::relatedQuery(const QString& _RelatedTableName, const QString& _SelectValues, 
		const QSqlRecord& _Values, const FTableMetaData& _TableMetaData )
{
	FFieldMetaData RelField = _TableMetaData.relatedField(_RelatedTableName);
	Assert(!RelField.name().isEmpty(), 
		Error(QObject::tr(QString("Table " + _TableMetaData.name +  " has no relation with table " + _RelatedTableName).toLatin1() )));
	FSqlQuery Res;
	Res.prepare("SELECT " + _SelectValues + " FROM " + RelField.relation().tableName + " WHERE " + 
		RelField.relation().indexColumn + " = ?");
	Res.addBindValue(_Values.value(RelField.name()));
	
	return Res;
}

/*!
	El mateix que la anterior per� utilitzant el FSqlDatabaseManager per defecte per a obtenir 
	les metadades de _TableName.
*/

FSqlQuery FSqlMDUtils::relatedQuery(const QString& _RelatedTableName, const QString& _SelectValues, 
		const QSqlRecord& _Values, const QString& _TableName )
{
	//Utilitzem el manager per defecte.
	return FSqlMDUtils::relatedQuery(_RelatedTableName, _SelectValues, _Values, FSqlDatabaseManager::manager().tableMetaData( _TableName));
}



void FSqlMDUtils::setDefaultValues(const FTableMetaData& _MetaData, QSqlRecord& _Record)
{
	for (int Vfor=0; Vfor < _Record.count(); Vfor++)
	{
		if (_MetaData.fields.contains(_Record.fieldName(Vfor)))
		{
			FFieldMetaData CFMdata = _MetaData.fields[_Record.fieldName(Vfor)];	
			if (!CFMdata.defaultValue().isNull())
			{
				if (CFMdata.defaultValue().toString().toLower() == "now" && 
					(CFMdata.type() == QVariant::Time || CFMdata.type() == QVariant::DateTime ||
					CFMdata.type() == QVariant::Date))
				{
					if (CFMdata.type() == QVariant::Time)
						_Record.setValue(_Record.fieldName(Vfor), QTime::currentTime());
					else 
					if (CFMdata.type() == QVariant::DateTime)
						_Record.setValue(_Record.fieldName(Vfor), QDateTime::currentDateTime());
					else 
					if (CFMdata.type() == QVariant::Date)
						_Record.setValue(_Record.fieldName(Vfor), QDate::currentDate());
				}
				else 
				{
					//qDebug(QString("Setting field %1 of Type %2").arg(_Record.fieldName(Vfor)).arg(
					//	QVariant::typeToName(CFMdata.type())).toLatin1());
					//_Record.setValue(_Record.fieldName(Vfor), CFMdata.defaultValue().convert(CFMdata.type()));
					QVariant Val = CFMdata.defaultValue();
					Val.convert(CFMdata.type());
					_Record.setValue(_Record.fieldName(Vfor), Val);
				}
			}
		}
	}
}
