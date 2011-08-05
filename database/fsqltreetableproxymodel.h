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


#ifndef FSQLTREETABLEPROXYMODEL_H
#define FSQLTREETABLEPROXYMODEL_H

#include <QAbstractProxyModel>

class QSqlTableModel;
class FSqlTreeTableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
	QSqlTableModel* SourceModel;
	int KeyColumn;
	int ParentColumn;

public:
	explicit FSqlTreeTableProxyModel(int _KeyColumn, int _ParentKeyColumn, QObject *parent = 0);
	void setSourceModel(QSqlTableModel* _SourceModel);
	virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex());
signals:

public slots:

};

#endif // FSQLTREETABLEPROXYMODEL_H
