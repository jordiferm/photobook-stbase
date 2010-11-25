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

#ifndef FSQL1NMAPPERSYNC_H
#define FSQL1NMAPPERSYNC_H

#include <QObject>
#include <QVariant>
#include "stdbmanagexport.h"

/**
A class than sincronize QDataWidgetMapper with a filter from 1:N relation of 2 sql tables.
This class is more flexible than FSql1NSyncRelModel because you can use any QSqlTableModel. You are not forced to use FSqlRelationalTableModel.
 
@author Shadow
*/
class QSqlTableModel;
class QDataWidgetMapper;
class QAbstractItemModel;
class QSqlRecord;
class ST_DBMANAG_EXPORT FSql1NMapperSync : public QObject
{
	Q_OBJECT

	int SourceModelColumn;
	QSqlTableModel* RelatedModel;
	QString RelatedModelFieldName;
	QString RelatedModelFieldIndex;
	QString BaseFilter;
	QString FilterOld;
	QVariant LastRelValue;
	QAbstractItemModel* SourceModel;
	void relValueChanged(const QVariant& _Value);

public:
	//! Special for QSqlRelationalTableModel _RelatedModel (The other constructor have problems getting related fields because displayfield names).
	FSql1NMapperSync(QDataWidgetMapper* _Mapper, 
							int _SourceModelColumn, QSqlTableModel* _RelatedModel, QString _RelatedModelFieldName,
							QObject* _Parent = 0);
	
	FSql1NMapperSync(QDataWidgetMapper* _Mapper,
	                 int _SourceModelColumn, QSqlTableModel* _RelatedModel, int _RelatedModelFieldIndex,
	                 QObject* _Parent = 0);

protected slots:
	void sourceModelIndexChange(int _Index);
	//! Sets the last SourceModelValue to RelatedModel(RelatedModelFieldName)
	void relModelPrimeInsert(int, QSqlRecord& _Record);

};

#endif
