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

#include "fsql1nmappersync.h"
#include "fsqlmodelviewutils.h"
#include <QDataWidgetMapper> 
#include <QSqlTableModel> 

#include <QSqlRecord>
/*!
	It takes the SourceModel from _Mapper.
*/

void FSql1NMapperSync::relValueChanged(const QVariant& _Value)
{
	LastRelValue = _Value;
	QString QuotedValue; 
	QString FilterOld = RelatedModel->filter();
	
	if (_Value.type() == QVariant::Double || _Value.type() == QVariant::Int ||
		 _Value.type() == QVariant::LongLong || _Value.type() == QVariant::UInt ||
		 _Value.type() == QVariant::ULongLong )
		QuotedValue = _Value.toString();
	else 
		QuotedValue = "'" + _Value.toString() + "'";
	QString Filter = RelatedModelFieldName + "=" + QuotedValue;
	
	if (!BaseFilter.isEmpty())
		Filter = BaseFilter + " AND " + Filter;
	
	if (Filter != FilterOld)
	{
 		RelatedModel->setFilter(Filter);
 		RelatedModel->select();
	}
}

FSql1NMapperSync::FSql1NMapperSync(QDataWidgetMapper* _Mapper, 
	int _SourceModelColumn, QSqlTableModel* _RelatedModel, QString _RelatedModelFieldName,
	QObject* _Parent)
 : QObject(_Parent), SourceModelColumn(_SourceModelColumn), RelatedModel(_RelatedModel), 
 RelatedModelFieldName(_RelatedModelFieldName), RelatedModelFieldIndex(-1)
{
	BaseFilter = _RelatedModel->filter();
	SourceModel = _Mapper->model();
	connect(_Mapper, SIGNAL(currentIndexChanged(int )), this, SLOT(sourceModelIndexChange(int)));
	connect(_RelatedModel, SIGNAL(primeInsert(int, QSqlRecord&)), this, SLOT(relModelPrimeInsert(int, QSqlRecord& )));
}


FSql1NMapperSync::FSql1NMapperSync(QDataWidgetMapper* _Mapper, 
	int _SourceModelColumn, QSqlTableModel* _RelatedModel, int _RelatedModelFieldIndex,
	QObject* _Parent)
 : QObject(_Parent), SourceModelColumn(_SourceModelColumn), RelatedModel(_RelatedModel), 
 RelatedModelFieldIndex(_RelatedModelFieldIndex)
{
	BaseFilter = _RelatedModel->filter();
	SourceModel = _Mapper->model();
	//FIXME I think that it fails with relational tablemodels
	RelatedModelFieldName = _RelatedModel->record().fieldName(_RelatedModelFieldIndex);
	connect(_Mapper, SIGNAL(currentIndexChanged(int )), this, SLOT(sourceModelIndexChange(int)));
	connect(_RelatedModel, SIGNAL(primeInsert(int, QSqlRecord&)), this, SLOT(relModelPrimeInsert(int, QSqlRecord& )));
}

void FSql1NMapperSync::sourceModelIndexChange(int _Index)
{
	relValueChanged(SourceModel->data(SourceModel->index(_Index, SourceModelColumn)));
}

void FSql1NMapperSync::relModelPrimeInsert(int, QSqlRecord& _Record)
{
	//_Record.setValue(RelatedModelFieldName, LastRelValue);
	//For Relational Models.
	_Record.setValue(FSqlModelViewUtils::indexOf(RelatedModel, RelatedModelFieldName), LastRelValue);
}
