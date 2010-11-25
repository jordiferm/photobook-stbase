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

#include "fsqlquery.h"
#include <QApplication>
#include <QSqlRecord>
#include <QSqlIndex>
#include "stlog.h"
#include "fdbutils.h"


bool FSqlQuery::LogErrors = false;

//___________________________________________________________________________
//
// Class FSqlQueryError.
//___________________________________________________________________________

QString FSqlQuery::Error::typeToStr(QSqlError::ErrorType _Type)
{
	QString Res = "";
		switch (_Type)
		{
			case QSqlError::NoError:
				Res = "NoError";
			break;
			case QSqlError::ConnectionError:
				Res = "ConnectionError";
			break;
			case QSqlError::StatementError:
				Res = "StatementError";
			break;
			case QSqlError::TransactionError:
				Res = "TransactionError";
			break;
			case QSqlError::UnknownError:
				Res = "UnknownError";
			break;
		}
	return Res; 
}


/*!
   Al missatge d'error de la query, no s'ha pogut executar query Select ... ... s'hi 
   afegirà el paràmetre _AppendedString.
*/

FSqlQuery::Error::Error(const QSqlQuery& _Query, QSqlError::ErrorType _Type, const QString& _AppendedString)
		: STError(QApplication::tr("Error (") + typeToStr(_Type) +  QApplication::tr(") running query") + ":" + _Query.lastQuery() + " "
		                 + _AppendedString) 
{
	
}


//___________________________________________________________________________
//
// Class FSqlQuery.
//___________________________________________________________________________

void FSqlQuery::checkErrors()
{
	QSqlError LastErr = lastError();
	
	if (FSqlQuery::logErrors() && LastErr.type() != QSqlError::NoError)
	{
		stLog << ISTLogIO::L2 << ISTLogIO::Warning << "FSqlQuery::Error : " <<
		 FSqlQuery::Error::typeToStr(LastErr.type()) << " ->" <<  LastErr.text() << 
		 QApplication::tr(" running query") << ":'" << executedQuery() << "'" << ISTLogIO::Endl;
	}

	Assert(LastErr.type() == QSqlError::NoError, Error(*this, LastErr.type(), LastErr.text()));
}


FSqlQuery::FSqlQuery(const QString& _Query, QSqlDatabase _Db) : QSqlQuery(_Query, _Db), 
Database(_Db), LogExecs(false)
{
	if (!_Query.isEmpty())
		checkErrors();
}

FSqlQuery::FSqlQuery ( QSqlDatabase _Db ) : QSqlQuery(_Db), Database(_Db), LogExecs(false)
{
}

void FSqlQuery::setLogExecs(bool _Value)
{
	LogExecs = _Value;
}


bool FSqlQuery::exec()
{
	bool Res = QSqlQuery::exec();
	checkErrors();
	if (LogExecs) 
		stLog << ISTLogIO::L2 << ISTLogIO::Warning << "FSqlQuery:: executed : '" <<
		 lastQuery() << "'" << ISTLogIO::Endl;
		
	return Res;
}

bool FSqlQuery::exec(const QString& _Value)
{
	bool Res = QSqlQuery::exec(_Value);
	checkErrors();
	if (LogExecs) 
		stLog << ISTLogIO::L2 << ISTLogIO::Warning << "FSqlQuery:: executed : '" <<
		 lastQuery() << "'" << ISTLogIO::Endl;
	return Res;
}

/*!
	\return true if all lines exec are true.
*/
bool FSqlQuery::execLines(const QString& _Value)
{
	bool Res = true; 
	
	QStringList ExecLines = _Value.split('\n'); 
	if (ExecLines.size() > 0)
	{
		QStringList::iterator it; 
		for (it = ExecLines.begin(); it != ExecLines.end(); ++it)
		{
			QString CurrLine = it->trimmed().remove(';');
			
			if (!CurrLine.isEmpty())
				Res = Res & exec(CurrLine);
		}
	}
	
	return Res;
}

/*! Indica si volem log per els errors de les querys.
	El valor per defecte és false;
*/
void FSqlQuery::setLogErrors(bool _Value)
{
	LogErrors = _Value;
}

bool FSqlQuery::logErrors()
{
	return LogErrors;
}

QString FSqlQuery::sqlStatement(QSqlDriver::StatementType _Type, const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly, bool _IgnoreNullFields, const QStringList& _IgnoreFieldNames)
{
	QSqlRecord Record = _Rec; 
	QSqlIndex PrimaryIndex = Database.driver()->primaryIndex(_TableName);
	for (int Vfor=0; Vfor < Record.count(); Vfor++)
	{
		if (_PrimaryKeyOnly)
		{
			if (!PrimaryIndex.contains(Record.fieldName(Vfor)))
				Record.remove(Vfor);
		}
		if (_IgnoreNullFields)
		{
			if (Record.isNull(Vfor))
				Record.remove(Vfor);
		}
		if (_IgnoreFieldNames.contains(Record.fieldName(Vfor)))
			Record.remove(Vfor);
	}
	return Database.driver()->sqlStatement(_Type, _TableName, Record, false);	
}

void FSqlQuery::prepareInsert(const QSqlRecord& _Rec, const QString& _TableName)
{
	prepare(sqlStatement(QSqlDriver::InsertStatement, _Rec, _TableName, false, false ));
}
	
void FSqlQuery::prepareUpdate(const QSqlRecord& _Rec, const QString& _TableName)
{
	prepare(sqlStatement(QSqlDriver::UpdateStatement, _Rec, _TableName, false, false) + " " 
			+ sqlStatement(QSqlDriver::WhereStatement, _Rec, _TableName, true, true));
}

/*!
	Selects all fields from _Table where values like _Rec values. 
	if _PrimaryKeyOnly is true only primary key fields are used.
	if _IgnoreNullFields is true the fields that are null are not used.
*/
void FSqlQuery::prepareSelect(const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly, bool _IgnoreNullFields, const QStringList& _IgnoreFieldNames)
{
	prepare("SELECT * FROM " + _TableName + " " + sqlStatement(QSqlDriver::WhereStatement, _Rec, _TableName, _PrimaryKeyOnly, _IgnoreNullFields, _IgnoreFieldNames));
}

/*! 
	returns true if a value with _Rec fields and values exists in _TableName using _PrimaryKeyOnly and _IgnoreNullFields on sqlStatement method 
*/
bool FSqlQuery::exists(const QSqlRecord& _Rec, const QString& _TableName, bool _PrimaryKeyOnly, bool _IgnoreNullFields, const QStringList& _IgnoreFieldNames, QSqlDatabase _Db)
{
	FSqlQuery Query(_Db);
	Query.prepareSelect(_Rec, _TableName, _PrimaryKeyOnly, _IgnoreNullFields, _IgnoreFieldNames);
	Query.exec();
	return Query.size() > 0;
}

void FSqlQuery::insert(const QSqlRecord& _Rec, const QString& _TableName, QSqlDatabase _Db)
{
	FSqlQuery Query(_Db); 
	Query.prepareInsert(_Rec, _TableName);
	Query.exec();
}

void FSqlQuery::update(const QSqlRecord& _Rec, const QString& _TableName, QSqlDatabase _Db)
{
	FSqlQuery Query(_Db); 
	Query.prepareUpdate(_Rec, _TableName);
	Query.exec();
}

QVariant FSqlQuery::sequenceNextVal(const QString& _TableName, const QString& _FieldName, QSqlDatabase _Db)
{
	QVariant Res;
	FSqlQuery Query(_Db) ;
	//if (_Db.driverName().left(7).toUpper() == "QSQLITE" )
	if (FDBUtils::isSqlite(_Db))
	{
		int IdCommand = 0;
		Query.exec("select max(" + _FieldName + ") from " + _TableName + "");
		if (Query.next())
			Res = Query.value(0).toInt() + 1;	
	}
	else 
	{
		Query.exec(QString("select nextval('" + _TableName +  "_" + _FieldName + "_seq')")); 
		if (Query.next())
			Res = Query.value(0);
	}
	return Res;
}
