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

#include "csvimport.h"
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QDebug>
#include "fsqlquery.h"

CSVImport::CSVImport(QSqlDatabase _Db) : Database(_Db), FirstLineFields(true), Separator("|"),  TextDelimiter("\"")
{
}


void CSVImport::importFile(const QString& _FileName, const QString& _TableName)
{
	QFile InputFile(_FileName);
	Assert(InputFile.open(QFile::ReadOnly), Error(QObject::tr("Could not open file %1").arg(_FileName)));
	QTextStream Strm(&InputFile);
	Strm.setCodec("UTF-8");
	QString CurrLine = Strm.readLine();
	bool FirstLine = true;
	QStringList FieldList;
	QStringList ValueList;
	while (!CurrLine.isNull())
	{
		CurrLine = CurrLine.remove(TextDelimiter);
		ValueList = CurrLine.split(Separator);
		QSqlRecord CurrentRecord = Database.record(_TableName);
		parseLine(_TableName, ValueList, CurrentRecord, FirstLine);
		if (!FirstLine || !FirstLineFields)
			insertRecord(_TableName, CurrentRecord);
		if (FirstLine)
			FirstLine = false;
		CurrLine = Strm.readLine();
	}
	InputFile.close();

}

void CSVImport::parseField(const QString& _TableName, const QString& _FieldName, const QString& _Value, QSqlRecord& _CurrentRecord)
{
	if (_CurrentRecord.contains(_FieldName))
		_CurrentRecord.setValue(_FieldName, _Value);
}

void CSVImport::parseLine(const QString& _TableName, const QStringList& _Values, QSqlRecord& _CurrentRecord, bool _FirstLine)
{
	if (_FirstLine && FirstLineFields)
	{
		Fields = _Values;
	}
	else
	{
		for (int Vfor = 0; Vfor < Fields.size(); Vfor++)
		{
			QString CurrentField = Fields[Vfor];
			if (_Values.size() > Vfor)
			{
				parseField(_TableName, CurrentField, _Values[Vfor], _CurrentRecord);
			}
		}
	}
}

void CSVImport::insertRecord(const QString& _TableName, const QSqlRecord& _Record)
{
	FSqlQuery Query(Database);
	Query.prepareInsert(_Record, _TableName);
	Query.exec();
}
