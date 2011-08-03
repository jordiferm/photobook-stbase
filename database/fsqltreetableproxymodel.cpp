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

#include "fsqltreetableproxymodel.h"
#include <QSqlTableModel>
#include <QSqlRecord>


int FSqlTreeTableProxyModel::childRow(const QVariant& _ChildId)
{

}


FSqlTreeTableProxyModel::FSqlTreeTableProxyModel(int _KeyColumn, int _ParentKeyColumn, QObject *parent) :
	QAbstractProxyModel(parent), KeyColumn(_KeyColumn), ParentKeyColumn(_ParentKeyColumn)
{
}

void FSqlTreeTableProxyModel::setSourceModel(QSqlTableModel* _SourceModel)
{
	SourceModel = _SourceModel;
}

virtual QModelIndex	FSqlTreeTableProxyModel::mapFromSource ( const QModelIndex & _SourceIndex) const
{
	return  index(childRow(_SourceIndex.row()), _SourceIndex.column());
}

virtual QModelIndex	FSqlTreeTableProxyModel::mapToSource ( const QModelIndex & _ProxyIndex) const
{
	if (!_ProxyIndex.isValid() || !SourceModel)
		return QModelIndex();

	QModelIndex Res;
	QModelIndexList IndexList = SourceModel->match(SourceModel->index(0, KeyColumn), Qt::EditRole, _ProxyIndex.internalId(), 1, Qt::MatchExactly);
	if (IndexList.size() > 0)
		Res = IndexList.first();

	return Res;
}

QModelIndex FSqlTreeTableProxyModel::parent(const QModelIndex& _Child) const
{
	if (!_Child.isValid() || !SourceModel)
		return QModelIndex();

	return createIndex()
}

int FSqlTreeTableProxyModel::rowCount(const QModelIndex &parent) const
{
	return
}

QModelIndex	FSqlTreeTableProxyModel::index ( int row, int column, const QModelIndex& _Parent)
{
}
