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

#ifndef FGENMANAGER_H
#define FGENMANAGER_H

#include <QWidget>
#include "fsqldatabasemanager.h"
#include "stdbmanagexport.h"

/**
Gestió genèrica en forma de tableView, sense fitxa.
 
@author Shadow
*/
class FTableManager;
class QSqlRelationalTableModel;
class QItemSelectionModel;
class ST_DBMANAG_EXPORT FGenManager : public QWidget
{
	Q_OBJECT

	void init(const QString& _Columns, bool _SortHeader);

protected:
	FTableMetaData TMdata;
	FTableManager* TManager;
	QSqlTableModel* Model;
	QSqlRecord PrimeInsertRecord;

public:
	FGenManager(QSqlRelationalTableModel* _Model, const QString& _TableName, 
	QWidget* _Parent = 0, const QString& _Columns = "", 	bool _SortHeader = false);
	FGenManager(QSqlTableModel* _Model, const QString& _TableName, 
			QWidget* _Parent = 0, const QString& _Columns = "", 	bool _SortHeader = false);
	FGenManager(const QString& _TableName, QWidget* _Parent = 0, const QString& _Columns = "", bool _SortHeader = false,
				const QSqlDatabase& _Database = QSqlDatabase::database());
	QSqlTableModel* model() const{ return Model;}
	FTableManager* tableManager() const {return TManager; }
	void update();
	QItemSelectionModel* selectionModel() const;


protected slots:
	virtual void primeInsert(int _Row, QSqlRecord& _Record);
};

#endif
