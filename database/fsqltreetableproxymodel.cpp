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


int FSqlTreeTableProxyModel::childRow(const QVariant& _ChildId)
{

}

QModelIndex parentIndex(const QVariant& _ChildId)
{
	//Recursive ?
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
	QVariant RowId = SourceModel->data(_SourceIndex);
	return  index(childRow(RowId), _SourceIndex.row(), parentIndex(RowId));
}

virtual QModelIndex	FSqlTreeTableProxyModel::mapToSource ( const QModelIndex & proxyIndex ) const
{

}

QModelIndex FSqlTreeTableProxyModel::parent(const QModelIndex& _Child) const
{
	return _Child.parent();
}

int FSqlTreeTableProxyModel::rowCount(const QModelIndex &parent) const
{
}
