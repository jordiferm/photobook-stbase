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

#ifndef FSQLRELATIONALTABLEMODEL_H
#define FSQLRELATIONALTABLEMODEL_H

#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QSqlQuery>
#include "stdatabaseexport.h"


/**
@author Shadow
*/
class ST_DATABASE_EXPORT FSqlRelationalTableModel : public QSqlRelationalTableModel
{
 Q_OBJECT 

public: 
	enum FEditStrategy
	{
		FOnManualSubmit = 2, 
		FOnDataChange = 3
	} ;
	
	typedef QList<int> TColSortList;
	
	FEditStrategy MEditStrategy; 
	
private:
	QSqlRecord BaseRec;
	TColSortList ColSortList;
	QSqlQuery EditQuery;

protected:
	static void qAppendWhereClause(QString &query, const QString &clause1, const QString &clause2);

public:
	FSqlRelationalTableModel( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() );
	void updateColumnSortList();
	//! Donada una posici� ens diu la columna(Absoluta) que mostra aquella posicio.
	int relativeColumn(int _Pos) const;
	//! Donada una _Columna(Absoluta) retorna la posici� relativa on es mostrar�.
	int relativePos(int _Column) const;
	//! Situa els camps per ordre segons _Columns.
	void setColOrder(const QStringList& _Columns); // Deprecated
	void setColPos(const QString& _ColumnField, int _Pos); // Deprecated
	void setColPos(const QString& _ColumnField, const QString& _PosField); // Deprecated
	void setColPos(int _Column, int _Pos); // Deprecated
/*	QVariant headerData( int _Section, Qt::Orientation _Orientation, int _Role = Qt::DisplayRole ) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;*/
	void setTable(const QString& _Table);
	
	bool insertRowIntoTable(const QSqlRecord& _Values);
	bool exec(const QString &stmt, bool prepStatement,
		const QSqlRecord &rec, const QSqlRecord &whereValues);
	QString escapedRelationField(const QString &tableName, const QString &fieldName) const;
	QString selectStatement () const;
	QSqlRecord primaryValues(int row);
	bool deleteRowFromTable(int row);
 	bool updateRowInTable(int _Row, const QSqlRecord& _Values);
//	void setEditStrategy(FEditStrategy _Strategy);
	bool submit();
	QSqlTableModel* relationModel(const QString& _FieldName) const;
	QModelIndex recordMatch(const QSqlRecord& ) const; 	
/*protected:
	virtual void setEditStrategy ( EditStrategy strategy );*/
	
/*	protected slots: 
	void fdataChange(const QModelIndex & , const QModelIndex &);*/
	
signals: 
	void errorOnSubmit(const QSqlError&);	
	//! S'ha fet un submit sense errors.
	void noErrorOnSubmit();	
	void afterUpdate();
/*	void submitAllOnIndex(const QModelIndex& );*/
};


#endif
