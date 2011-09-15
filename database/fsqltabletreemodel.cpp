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

#include "fsqltabletreemodel.h"
#include <QSqlTableModel>
#include <QSqlField>
#include <QDebug>

//_____________________________________________________________________________
//
// Class FSqlTableFSqlTableTreeModel
//_____________________________________________________________________________


TreeItem::TreeItem(const QSqlRecord& _Data, TreeItem* _Parent)
 {
	 ParentItem = _Parent;
	 ItemData = _Data;
	 if (_Parent)
		_Parent->ChildItems.append(this);
 }

 TreeItem::~TreeItem()
 {
	 qDeleteAll(ChildItems);
 }


 TreeItem* TreeItem::child(int row)
 {
	 return ChildItems.value(row);
 }

 int TreeItem::childCount() const
 {
	 return ChildItems.count();
 }

 int TreeItem::columnCount() const
 {
	 return ItemData.count();
 }

 QVariant TreeItem::data(int column) const
 {
	 return ItemData.value(column);
 }

 TreeItem* TreeItem::parent()
 {
	 return ParentItem;
 }

 int TreeItem::row() const
 {
	 if (ParentItem)
		 return ParentItem->ChildItems.indexOf(const_cast<TreeItem*>(this));

	 return 0;
 }

//_____________________________________________________________________________
//
// Class FSqlTableTreeModel
//_____________________________________________________________________________

void FSqlTableTreeModel::addChildItems(TreeItem* _Parent, const QVariant& _ParentId, const QSqlTableModel* _Model,
									   int _KeyCol, int _ParentKeyCol)
{
	QModelIndexList Childs = _Model->match(_Model->index(0, _ParentKeyCol), Qt::EditRole, _ParentId, -1, Qt::MatchExactly);
	QModelIndexList::iterator it;
	for (it = Childs.begin(); it != Childs.end(); ++it)
	{
		//qDebug() << "Row find" << it->row();
		QSqlRecord Rec = _Model->record(it->row());
		TreeItem* NewItem = new TreeItem(Rec,_Parent);
		addChildItems(NewItem, Rec.value(_KeyCol), _Model, _KeyCol, _ParentKeyCol);
	}
}

FSqlTableTreeModel::FSqlTableTreeModel(QObject *parent) :
	QAbstractItemModel(parent), RootItem(0)
{
}

QModelIndex FSqlTableTreeModel::mapToSource(const QModelIndex& _Index)
{
	QModelIndex Res;
	QVariant CurrentKeyVal = data(index(_Index.row(), KeyCol, _Index.parent()), Qt::DisplayRole);
	QModelIndexList IndexList = SourceModel->match(SourceModel->index(0, KeyCol), Qt::EditRole, CurrentKeyVal, 1, Qt::MatchExactly);
	if (!IndexList.isEmpty())
		Res = IndexList.first();
	return Res;
}

void FSqlTableTreeModel::setSourceTableModel(const QSqlTableModel* _Model, int _KeyCol, int _ParentKeyCol)
{
	KeyCol = _KeyCol;
	SourceModel = _Model;
	//Create rootitem with headerData
	QSqlRecord HeaderRecord;
	for (int Vfor = 0; Vfor < _Model->columnCount(); Vfor++)
	{
		QSqlField NewField(QString("Header_%1").arg(Vfor), QVariant::String);
		NewField.setValue(_Model->headerData(Vfor, Qt::Horizontal));
		HeaderRecord.append(NewField);
	}


	RootItem = new TreeItem(HeaderRecord);

	addChildItems(RootItem, QVariant(QVariant::Int), _Model, _KeyCol, _ParentKeyCol);

/*	QMap<QString, TreeItem*> Parents;


	for (int Vfor = 0; Vfor < _Model->rowCount(); Vfor++)
	{
		QVariant CParentId = _Model->record(Vfor).value(_ParentKeyCol);
		//If there is no parent The parent is root
		TreeItem* CParent = 0;
		if (CParentId.isNull())
			CParent = RootItem;
		else
		{
			if (Parents.contains(CParentId.toString()))
				CParent = Parents[CParentId.toString()];
			else
				CParent = RootItem;
		}

		TreeItem* NewItem = new TreeItem(_Model->record(Vfor), CParent);
		Parents.insert(_Model->record(Vfor).value(_KeyCol).toString(), NewItem);
	}*/

	reset();
}


FSqlTableTreeModel::~FSqlTableTreeModel()
{
	if (RootItem)
		delete RootItem;
}

int FSqlTableTreeModel::columnCount(const QModelIndex &parent) const
{
	if (!RootItem)
		return 0;

	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else
		return RootItem->columnCount();
}

QVariant FSqlTableTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
	return item->data(index.column());
}

Qt::ItemFlags FSqlTableTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FSqlTableTreeModel::headerData(int section, Qt::Orientation orientation,
							int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return RootItem->data(section);

	return QVariant();
}

QModelIndex FSqlTableTreeModel::index(int row, int column, const QModelIndex &parent)
		 const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = RootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex FSqlTableTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parent();

	if (parentItem == RootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int FSqlTableTreeModel::rowCount(const QModelIndex &parent) const
{
	if (!RootItem)
		return 0;

	TreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = RootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}
