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


#include "fsqlrelationaltablemodel.h"
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRelation>
#include <QSqlIndex>
#include <QStringList>
#include <QSqlDriver>
#include "fsqlmodelviewutils.h"


FSqlRelationalTableModel::FSqlRelationalTableModel(QObject* _Parent, QSqlDatabase _Db)
		: QSqlRelationalTableModel(_Parent, _Db), MEditStrategy(FOnManualSubmit)
{
	QSqlRelationalTableModel::setEditStrategy(QSqlTableModel::OnManualSubmit);
	//QSqlRelationalTableModel::setEditStrategy(QSqlTableModel::OnFieldChange);
	/*	connect(this, SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex & )),
		        this, SLOT(fdataChange ( const QModelIndex & , const QModelIndex & )));*/
}

void FSqlRelationalTableModel::updateColumnSortList()
{
	ColSortList.clear();
	for (int Vfor=0; Vfor < columnCount(); Vfor++)
		ColSortList.push_back(Vfor);
}

int FSqlRelationalTableModel::relativeColumn(int _Pos) const
{
	int Res = _Pos;
	if (columnCount() == ColSortList.size())
		Res = ColSortList[_Pos];
	return Res;
}

int FSqlRelationalTableModel::relativePos(int _Column) const
{
	int Res = _Column;
	if (columnCount() == ColSortList.size())
		Res = ColSortList.indexOf(_Column);
	return Res;
}

void FSqlRelationalTableModel::setColOrder(const QStringList& /*_Columns*/)
{
}

void FSqlRelationalTableModel::setColPos(const QString& _ColumnField, int _Pos)
{
	if (columnCount() != ColSortList.size())
		updateColumnSortList();
	setColPos(ColSortList[FSqlModelViewUtils::indexOf(this, _ColumnField)], _Pos);
}

void FSqlRelationalTableModel::setColPos(const QString& _ColumnField, const QString& _PosField)
{
	if (columnCount() != ColSortList.size())
		updateColumnSortList();
	setColPos(ColSortList[FSqlModelViewUtils::indexOf(this, _ColumnField)],
	          FSqlModelViewUtils::indexOf(this, _PosField));
}

void FSqlRelationalTableModel::setColPos(int _Column, int _Pos)
{
	if (columnCount() != ColSortList.size())
		updateColumnSortList();
	if (_Column < ColSortList.size() && _Column >= 0 && _Pos < ColSortList.size() && _Pos >= 0)
	{
		//qDebug("ColSortList[%d] = %d", _Pos, ColSortList[_Column]);
		int PosOld = ColSortList[_Pos];
		//ColSortList[_Pos] = ColSortList[_Column];
		ColSortList[_Pos] = ColSortList[_Column];
		//qDebug("ColSortList[%d] = %d", _Column, PosOld);
		ColSortList[_Column] = PosOld;
	}
	else
		qWarning("_Column out of bounds: %d", _Column);
}

// QVariant FSqlRelationalTableModel::headerData(int _Section, Qt::Orientation _Orientation, int _Role) const
// {
// 	if (columnCount() != ColSortList.size())
// 		const_cast<FSqlRelationalTableModel*>(this)->updateColumnSortList();
// 	return QSqlRelationalTableModel::headerData(ColSortList[_Section], _Orientation, _Role);
// }
//
//
// QVariant FSqlRelationalTableModel::data(const QModelIndex& _Index, int _Role) const
// {
// 	if (columnCount() != ColSortList.size())
// 		const_cast<FSqlRelationalTableModel*>(this)->updateColumnSortList();
// 	QVariant Res;
// 	if (_Index.model())
// 		Res = QSqlRelationalTableModel::data(_Index.model()->index(_Index.row(), ColSortList[_Index.column()]), _Role);
// 	return Res;
// }

/*!
    \reimp
*/
void FSqlRelationalTableModel::setTable(const QString& _Table)
{
	// memorize the table before applying the Relations
	BaseRec = database().record(_Table);
	QSqlRelationalTableModel::setTable(_Table);
}

/*!
    \reimp
*/

void FSqlRelationalTableModel::qAppendWhereClause(QString &query, const QString &clause1, const QString &clause2)
{
	if (clause1.isEmpty() && clause2.isEmpty())
		return;
	if (clause1.isEmpty() || clause2.isEmpty())
		query.append(QLatin1String(" WHERE (")).append(clause1).append(clause2);
	else
		query.append(QLatin1String(" WHERE (")).append(clause1).append(
		    QLatin1String(") AND (")).append(clause2);
	query.append(QLatin1String(") "));
}



/*!
    \reimp
*/
bool FSqlRelationalTableModel::insertRowIntoTable(const QSqlRecord& _Values)
{
	QSqlRecord Rec = _Values;

	// translate the field names
	for (int i = 0; i < _Values.count(); ++i)
	{
		if (relation(i).isValid())
		{
			QVariant OldVal = Rec.field(i).value();
			Rec.replace(i, BaseRec.field(i));
			Rec.setValue(i, OldVal);
			/*			QSqlField NField = Rec.field(i);
						NField.setName(BaseRec.field(i).name());
						Rec.replace(i, NField);*/
		}
	}
	return QSqlRelationalTableModel::insertRowIntoTable(Rec);

	//     emit beforeInsert(Rec);
	//
	//     bool prepStatement = database().driver()->hasFeature(QSqlDriver::PreparedQueries);
	//     QString stmt = database().driver()->sqlStatement(QSqlDriver::InsertStatement, tableName(),
	//                                                 Rec, prepStatement);
	//     //return exec(stmt, prepStatement, rec);
	// 	 qDebug(stmt.toLatin1());
	// 	 return false;
}

bool FSqlRelationalTableModel::exec(const QString &stmt, bool prepStatement,
                                    const QSqlRecord &rec, const QSqlRecord &whereValues)
{
	if (stmt.isEmpty())
		return false;

	// lazy initialization of editQuery

	if (EditQuery.driver() != database().driver())
		EditQuery = QSqlQuery(database());

	if (prepStatement)
	{
		if (EditQuery.lastQuery() != stmt)
		{
			if (!EditQuery.prepare(stmt))
			{
				setLastError(EditQuery.lastError());
				return false;
			}
		}
		int i;
		for (i = 0; i < rec.count(); ++i)
		{
			if (rec.isGenerated(i) && rec.value(i).type() != QVariant::Invalid)
				EditQuery.addBindValue(rec.value(i));
		}
		for (i = 0; i < whereValues.count(); ++i)
		{
			if (whereValues.isGenerated(i))
				EditQuery.addBindValue(whereValues.value(i));
		}

		if (!EditQuery.exec())
		{
			setLastError(EditQuery.lastError());
			return false;
		}
	}
	else
	{
		if (!EditQuery.exec(stmt))
		{
			setLastError(EditQuery.lastError());
			return false;
		}
	}
	//qDebug("executed: %s (%d)", stmt.ascii(), editQuery.numRowsAffected());
	return true;
}


QString FSqlRelationalTableModel::escapedRelationField(const QString &tableName, const QString &fieldName) const
{
    QString esc;
    esc.reserve(tableName.size() + fieldName.size() + 1);
    esc.append(tableName).append(QLatin1Char('.')).append(fieldName);

    return database().driver()->escapeIdentifier(esc, QSqlDriver::FieldName);
}


/*! Soluciona un problema que te el model relacional amb claus primaries febles. !*/

QString FSqlRelationalTableModel::selectStatement () const
{
	//return QSqlRelationalTableModel::selectStatement();
	QString query;

	if (tableName().isEmpty())
		return query;
// 	if (relations().isEmpty())
// 		return QSqlTableModel::selectStatement();

	QString tList;
	QString fList;
	QString where;

	QSqlRecord rec = database().record(tableName());
	QStringList tables;
	const QSqlRelation nullRelation;
    for (int i = 0; i < rec.count(); ++i) {
        QSqlRelation CRelation = relation(i);
        if (CRelation.isValid()) {
            QString relTableAlias = QString::fromLatin1("relTblAl_%1").arg(i);
            fList.append(escapedRelationField(relTableAlias, CRelation.displayColumn()));
            fList.append(QLatin1Char(','));
            if (!tables.contains(CRelation.tableName()))
                tables.append(database().driver()->escapeIdentifier(CRelation.tableName(),
                       QSqlDriver::TableName).append(QLatin1String(" AS ")).append(
                       database().driver()->escapeIdentifier(relTableAlias, QSqlDriver::TableName)));
            
            
            where.append(escapedRelationField(tableName(), rec.fieldName(i)));
            where.append(QLatin1Char('='));
            where.append(escapedRelationField(relTableAlias, CRelation.indexColumn()));
            where.append(QLatin1String(" AND "));
        } else {
            fList.append(escapedRelationField(tableName(), rec.fieldName(i)));
            fList.append(QLatin1Char(','));
        }
    }
	// A la select també hi posem el camp clau per a tenir el valor alhora de fer deletes.
	
	for (int i = 0; i < rec.count(); ++i)
	{
		QSqlRelation CRelation = relation(i);
		if (CRelation.isValid())
		{
            QString relTableAlias = QString::fromLatin1("relTblAl_%1").arg(i);
            //--- Begin -- Added by flam 
				if (CRelation.displayColumn() != CRelation.indexColumn() && 
					database().primaryIndex(tableName()).contains(rec.fieldName(i)))
				{
					QString KeyCol = escapedRelationField(relTableAlias,  CRelation.indexColumn());
					//QString KeyCol =  relTableAlias + "." + CRelation.indexColumn();
					fList.append(KeyCol).append(" AS ").append(relTableAlias + "_" + CRelation.indexColumn()).append(QLatin1Char(','));
				}
            //--- End -- Added by flam 
		}
	}
	
// 	for (int i = 0; i < rec.count(); ++i)
// 	{
// 		QSqlRelation CRelation = relation(i);
// 		if (CRelation.isValid())
// 		{
// 			QString relTableAlias = QString::fromLatin1("relTblAl_%1").arg(i);
// 			fList.append(relTableAlias).append(QLatin1Char('.'));
// 			fList.append(CRelation.displayColumn()).append(QLatin1Char(','));
// 			if (CRelation.displayColumn() != CRelation.indexColumn() && 
// 				database().primaryIndex(tableName()).contains(rec.fieldName(i)))
// 			{
// 				QString KeyCol = relTableAlias + "." + CRelation.indexColumn();
// 				fList.append(KeyCol).append(" AS ").append(relTableAlias + "_" + CRelation.indexColumn()).append(QLatin1Char(','));
// 			}
// 			if (!tables.contains(CRelation.tableName()))
// 				tables.append(CRelation.tableName().append(QLatin1String(" AS ")).append(relTableAlias));
// 			where.append(tableName()).append(QLatin1Char('.')).append(rec.fieldName(i));
// 			where.append(QLatin1Char('=')).append(relTableAlias).append(QLatin1Char('.'));
// 			where.append(CRelation.indexColumn()).append(QLatin1String(" AND "));
// 		}
// 		else
// 		{
// 			fList.append(tableName()).append(QLatin1Char('.')).append(rec.fieldName(i)).append(
// 			    QLatin1Char(','));
// 		}
// 	}
	if (!tables.isEmpty())
		tList.append(tables.join(QLatin1String(","))).append(QLatin1String(","));
	if (fList.isEmpty())
		return query;
	tList.prepend(QLatin1Char(',')).prepend(tableName());
	// truncate tailing comma
	tList.chop(1);
	fList.chop(1);
	query.append(QLatin1String("SELECT "));
	query.append(fList).append(QLatin1String(" FROM ")).append(tList);
	if (!where.isEmpty())
		where.chop(5);
	qAppendWhereClause(query, where, filter());

	QString orderBy = orderByClause();
	if (!orderBy.isEmpty())
		query.append(QLatin1Char(' ')).append(orderBy);
	return query;
}


/*! Soluciona un problema que te el model relacional amb claus primaries febles. !*/

QSqlRecord FSqlRelationalTableModel::primaryValues(int row)
{
	QSqlRecord Res;
	
	bool UsingPrimaryKey = true;
	QSqlRecord Rec = database().record(tableName());
	//const QSqlRelation nullRelation;

	if (!query().seek(row)) {
		setLastError(query().lastError());
		return Res;
	}

	Res = primaryKey();

	if (Res.isEmpty())
	{
		Res = record();
		UsingPrimaryKey = false;
	}

	for (int i = 0; i < Res.count(); ++i)
	{
		if (UsingPrimaryKey)
		{
			QSqlRelation CRelation = relation(Rec.indexOf(Res.fieldName(i)));
			if (CRelation.isValid() && (CRelation.displayColumn() != CRelation.indexColumn()))
			{
				Res.setValue(i,
					query().value(query().record().indexOf(QString::fromLatin1("relTblAl_%1_%2").arg(Rec.indexOf(Res.fieldName(i))).arg( CRelation.indexColumn()))));
/*				int Index = query().record().indexOf(QString::fromLatin1("relTblAl_%1_%2").arg(i).arg( CRelation.indexColumn()));*/
			}
			else 
				Res.setValue(i, query().value(Rec.indexOf(Res.fieldName(i))));
		}
		else			
			Res.setValue(i, query().value(i));
	}
	return Res;
}

/*!
    \reimp <= exec is not virtual. 
*/

bool FSqlRelationalTableModel::deleteRowFromTable(int row)
{
    emit beforeDelete(row);

    QSqlRecord rec = primaryValues(row);
    bool prepStatement = database().driver()->hasFeature(QSqlDriver::PreparedQueries);
    QString stmt = database().driver()->sqlStatement(QSqlDriver::DeleteStatement, tableName(),
                                                QSqlRecord(), prepStatement);
    QString where = database().driver()->sqlStatement(QSqlDriver::WhereStatement, tableName(),
                                                rec, prepStatement);

    if (stmt.isEmpty() || where.isEmpty()) {
        setLastError(QSqlError(QLatin1String("Unable to delete row"), QString(),
                             QSqlError::StatementError));
        return false;
    }
    stmt.append(QLatin1Char(' ')).append(where);
    return exec(stmt, prepStatement, rec, QSqlRecord());
}


/*!
    \reimp <= exec no és virtual. 
*/

bool FSqlRelationalTableModel::updateRowInTable(int _Row, const QSqlRecord& _Values)
{
	QSqlRecord Rec = _Values;

	// translate the field names
	for (int i = 0; i < _Values.count(); ++i)
	{
		if (relation(i).isValid())
		{
			QVariant OldVal = Rec.field(i).value();
			Rec.replace(i, BaseRec.field(i));
			Rec.setValue(i, OldVal);
		}
	}

   emit beforeUpdate(_Row, Rec);

	bool prepStatement = database().driver()->hasFeature(QSqlDriver::PreparedQueries);
	QString stmt = database().driver()->sqlStatement(QSqlDriver::UpdateStatement, tableName(),
	               Rec, prepStatement);
	//return exec(stmt, prepStatement, rec);
// 	qDebug(stmt.toLatin1());

	
	const QSqlRecord whereValues = primaryValues(_Row);
	QString where = database().driver()->sqlStatement(QSqlDriver::WhereStatement, tableName(),
	                whereValues, prepStatement);
// 	qDebug(where.toLatin1());
	if (stmt.isEmpty() || where.isEmpty() || _Row < 0 || _Row >= rowCount())
	{
		setLastError( QSqlError(QLatin1String("No Fields to update"), QString(),
		                        QSqlError::StatementError));
		return false;
	}
	stmt.append(QLatin1Char(' ')).append(where);
	bool Res = exec(stmt, prepStatement, Rec, whereValues);
	emit afterUpdate();
	return Res;
	//return QSqlRelationalTableModel::updateRowInTable(_Row, Rec);
}


bool FSqlRelationalTableModel::submit()
{
	bool Res =	QSqlRelationalTableModel::submit();
	if (lastError().type() != QSqlError::NoError)
	{
		/*		qDebug(QString("Ara si que hi ha error !!!!!!!!!!!!!!!!!!!->" + lastError().text()).toLatin1() );
				qDebug(QString("Driver->" + lastError().driverText()).toLatin1() );
				qDebug(QString("Database->" + lastError().databaseText()).toLatin1() );*/
		//QSqlError Error = lastError();
		//setLastError(QSqlError());
		//emit errorOnSubmit(Error);
		emit errorOnSubmit(lastError());
	}
	else
		emit noErrorOnSubmit();

	return Res;
}




QSqlTableModel* FSqlRelationalTableModel::relationModel(const QString& _FieldName) const
{
	return QSqlRelationalTableModel::relationModel(record().indexOf(_FieldName));
}

QModelIndex FSqlRelationalTableModel::recordMatch(const QSqlRecord& _Rec) const
{
	QModelIndex Res;
	QSqlRecord PKeyRec = database().record(primaryKey().name());
	//! Mirem de fet match amb el primer camp.
	QModelIndex CurrIndx = index(0, record().indexOf(PKeyRec.fieldName(0)));
	bool Found = false;
	bool Exit = false;
	while (!Found && !Exit)
	{
		QModelIndexList IndxList = match(CurrIndx, Qt::EditRole, _Rec.value(PKeyRec.fieldName(0)),
		                                 1, Qt::MatchFlags(Qt::MatchExactly));
		if (!IndxList.isEmpty())
		{
			CurrIndx = IndxList.first();
			if (PKeyRec.count() > 1)
			{
				Found = true;
				QSqlRecord CurrRec = record(CurrIndx.row());
				//Comprovo amb els altres camps de la clau primaria.
				for (int Vfor=1; Vfor<PKeyRec.count() && Found; Vfor++)
					Found = Found && CurrRec.value(PKeyRec.fieldName(Vfor)) == _Rec.value(PKeyRec.fieldName(Vfor));
			}
			else
				Found = true;
		}
		else
			Exit = true;
	}
	if (Found)
		Res = CurrIndx;

	return Res;
}


// void FSqlRelationalTableModel::setEditStrategy ( FEditStrategy _Strategy)
// {
// 	MEditStrategy = _Strategy;
// }

// void FSqlRelationalTableModel::setEditStrategy ( EditStrategy _Strategy )
// {
// 	QSqlRelationalTableModel::setEditStrategy(_Strategy);
// }

//!!!!!! Dona problemes de refresc !!!!! No se pq ho teniem amb les Qt 4.0 ...
// void FSqlRelationalTableModel::fdataChange(const QModelIndex & _TopLeft , const QModelIndex& /*_BottomRight*/)
// {
//   	if (MEditStrategy == FSqlRelationalTableModel::FOnDataChange )
//   	{
//   		if (data(_TopLeft) != QSqlQueryModel::data(_TopLeft))
//   		{
//   			//qDebug("FSqlRelationalTableModel::fdataChange");
//   			submitAll();
//   			if (_TopLeft.isValid())
//   				emit submitAllOnIndex(_TopLeft);
//   		}
//   	}
// }
