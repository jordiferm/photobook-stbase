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

#ifndef FSQLTABLETREEMODEL_H
#define FSQLTABLETREEMODEL_H

#include <QAbstractItemModel>
#include <QSqlRecord>
#include "stdatabaseexport.h"

/**
FRecordWidget for articles.

@author Shadow
*/

class TreeItem
 {
 private:
	 QList<TreeItem*> ChildItems;
	 QSqlRecord ItemData;
	 TreeItem* ParentItem;

 public:
	 TreeItem(const QSqlRecord& _Data, TreeItem* _Parent = 0);
	 ~TreeItem();

	 TreeItem *child(int row);
	 int childCount() const;
	 int columnCount() const;
	 QVariant data(int column) const;
	 int row() const;
	 TreeItem *parent();
 };


/**
Tree model filled with TableModel.

@author Shadow
*/
class QSqlTableModel;
class ST_DATABASE_EXPORT FSqlTableTreeModel : public QAbstractItemModel
{
Q_OBJECT
	TreeItem* RootItem;
	int KeyCol;
	const QSqlTableModel* SourceModel;
	void addChildItems(TreeItem* _Parent, const QVariant& _ParentId, const QSqlTableModel* _Model,
					   int _KeyCol, int _ParentKeyCol);


public:
    explicit FSqlTableTreeModel(QObject *parent = 0);
	QModelIndex mapToSource(const QModelIndex& _Index);
	void setSourceTableModel(const QSqlTableModel* _Model, int _KeyCol, int _ParentKeyCol);

	~FSqlTableTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:

};

#endif // FSQLTABLETREEMODEL_H
