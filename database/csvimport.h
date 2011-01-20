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

#ifndef CSVIMPORT_H
#define CSVIMPORT_H
#include <QString>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QStringList>
#include "stdatabaseexport.h"
#include "sterror.h"

class ST_DATABASE_EXPORT CSVImport
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	QString Separator, TextDelimiter;
	QSqlDatabase Database;
	QStringList Fields;
	bool FirstLineFields;

public:
	CSVImport(QSqlDatabase _Db = QSqlDatabase::database());
	//! CSV field separator (Default is ',')
	void setSeparator(const QString& _Sep) { Separator = _Sep; }
	void setTextDelimiter(const QString& _Delimiter) { TextDelimiter = _Delimiter; }
	void setFields(const QStringList& _Fields) { Fields = _Fields; }
	//! If true (Default) the first CVS file line becomes fieldnames.
	void getFieldsFromFirstLine(bool _FirstLineFields) { FirstLineFields = _FirstLineFields; }
	void importFile(const QString& _FileName, const QString& _TableName);
	virtual void parseField(const QString& _TableName, const QString& _FieldName, const QString& _Value, QSqlRecord& _CurrentRecord);
	virtual void parseLine(const QString& _TableName, const QStringList& _Values, QSqlRecord& _CurrentRecord, bool _FirstLine);
	virtual void insertRecord(const QString& _TableName, const QSqlRecord& _Record);
};

#endif // CSVIMPORT_H
