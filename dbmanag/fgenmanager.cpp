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

#include "fgenmanager.h"
#include <QLayout>
#include <QMap>
#include <QSqlRelationalDelegate>
#include <QHideEvent>
#include <QDebug>
#include "fsqlsearchwidget.h"
#include "fsqlmetadata.h"
#include "ftablemanager.h"
#include "fsqlrelationaltablemodel.h"
#include "fsqlmdutils.h"
#include "fsqlmodelviewutils.h"
#include "fsqlactiontableview.h"
#include "fdbutils.h"

void FGenManager::init(const QString& _Columns, bool _SortHeader)
{
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	Model->select();

	//Problem with our Model type when creating TManager.
	if (QSqlRelationalTableModel* RelModel = qobject_cast<QSqlRelationalTableModel*>(Model))
		TManager = new FTableManager(RelModel, this, _Columns, _SortHeader);
	else
		TManager = new FTableManager(Model, this, _Columns, _SortHeader);
	
	MainLayout->addWidget(TManager);
	connect(Model, SIGNAL(primeInsert(int, QSqlRecord&)), this, SLOT(primeInsert(int, QSqlRecord& )));	
}

FGenManager::FGenManager(QSqlRelationalTableModel* _Model, const QString& _TableName, QWidget* _Parent, 
	const QString& _Columns, bool _SortHeader) : QWidget(_Parent)
{
	Model = _Model; 
	TMdata = FSqlDatabaseManager::manager().tableMetaData(_TableName);
	TMdata.configure(*_Model);
	setWindowTitle(QString(tr("%1 Management")).arg(TMdata.alias)  );
	init(_Columns, _SortHeader);
	
}

FGenManager::FGenManager(QSqlTableModel* _Model, const QString& _TableName, QWidget* _Parent, 
	const QString& _Columns, bool _SortHeader) : QWidget(_Parent)
{
	Model = _Model; 
	TMdata = FSqlDatabaseManager::manager().tableMetaData(_TableName);
	TMdata.configure(*_Model);
	setWindowTitle(QString(tr("%1 Management")).arg(TMdata.alias)  );
	init(_Columns, _SortHeader);	
}

FGenManager::FGenManager(const QString& _TableName, QWidget* _Parent, const QString& _Columns, 
bool _SortHeader, const QSqlDatabase& _Database) : QWidget(_Parent)
{
	FSqlRelationalTableModel* NModel = new FSqlRelationalTableModel(this, _Database);
	//QSqlRelationalTableModel* NModel = new QSqlRelationalTableModel(this);
	Model = NModel;
	TMdata = FSqlDatabaseManager::manager().tableMetaData(_TableName);
	TMdata.configure(*NModel);
	setWindowTitle(QString(tr("%1 Management")).arg(TMdata.alias)  );
	init(_Columns, _SortHeader);
	//TManager->searchWidget()->view()->setItemDelegate(new QSqlRelationalDelegate(this));

}

void FGenManager::update()
{
	Model->select();
}

QItemSelectionModel* FGenManager::selectionModel() const
{
	return TManager->actionTableView()->searchWidget()->view()->selectionModel();
}

void FGenManager::primeInsert(int /*_Row*/, QSqlRecord& _Record)
{
	FTableMetaData::TFieldList::iterator it;
	for (it = TMdata.fields.begin(); it != TMdata.fields.end(); ++it)
	{
		if (!it->relation().isNull())	
		{
			//qDebug(QString("Relation found: %1 ").arg(it->name()).toLatin1());
			FSqlQuery Query(
				QString("SELECT %1 FROM %2").arg(it->relation().indexColumn).arg(it->relation().tableName), Model->database());
			if (Query.next())
			{
				_Record.setValue(FSqlModelViewUtils::indexOf(Model, it->name()), Query.value(0));
			}
		}
	}
		
	//Serial Fields adjustments
	// -- On SQLite we do not have information of primary keys:
	if (FDBUtils::isSqlite(Model->database())) //SQLite has no booleans and itemDelegate is not able to
	{
		//For all Serial Fields.
		FTableMetaData::TFieldList::const_iterator it; 
		for (it = TMdata.fields.begin(); it != TMdata.fields.end(); ++it)
		{
			FFieldMetaData CFMdata = *it;
			QString CurrFieldName(CFMdata.name());
			if (CFMdata.isSerial)
			{
				QVariant NextVal = FSqlQuery::sequenceNextVal(Model->tableName(), CurrFieldName, Model->database());
				if (NextVal.isValid())
					_Record.setValue(CurrFieldName, NextVal.toInt());
				else
					_Record.setValue(CurrFieldName, 1);
			}
		}
	}
	else 
	{
		QSqlRecord PKeyRec = Model->database().record(Model->primaryKey().name());
		//For each primary key field. 
		for (int Vfor=0; Vfor < PKeyRec.count(); Vfor++)
		{
			//See if it's serial.
			QString CurrFieldName = PKeyRec.fieldName(Vfor);
			if (TMdata.fields.contains(CurrFieldName))
			{
				FFieldMetaData CFMdata = TMdata.fields[CurrFieldName];
				if (CFMdata.isSerial)
				{
					QVariant NextVal = FSqlQuery::sequenceNextVal(Model->tableName(), CurrFieldName);
					if (NextVal.isValid())
						_Record.setValue(CurrFieldName, NextVal.toInt());
					else
						_Record.setValue(CurrFieldName, 1);
				}
			}
		}
	}
	//_Record.setValue(PKeyRec.fieldName(Vfor), currentRowValue(PKeyRec.fieldName(Vfor)));
	FSqlMDUtils::setDefaultValues(TMdata, _Record);
	PrimeInsertRecord = _Record;
	TManager->setPrimeInsertRecord(_Record);
}

