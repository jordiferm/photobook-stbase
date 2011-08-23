/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "resourcemodel.h"
#include <QPixmap>

#include "resourcemimedata.h"

using namespace SPhotoBook;

ResourceModel::ResourceModel(QObject *parent) :
	QAbstractListModel(parent), HasNullItem(false)
{
}

int ResourceModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	int Res;
	if (HasNullItem)
		Res = RList.size() + 1;
	else
		Res = RList.size();
	return Res;
}

QVariant ResourceModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() <= RList.size())
		{
			if (_Index.row() == 0 && HasNullItem) //Null Item
			{
				if (_Role == Qt::DecorationRole)
					Res = QPixmap::fromImage(NullItemThumbnail);
			}
			else
			{
				Resource CurrResource = resource(_Index);
				if (_Role == Qt::DisplayRole)
				{
					Res = CurrResource.name();
				}
				else
				if (_Role == Qt::DecorationRole)
				{
					Res = RList.getThumbNail(CurrResource);
				}
			}
		}
	}
	return Res;
}

bool ResourceModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	bool Res;
	if (HasNullItem)
	{
		if (_Row > 0 )
		{
			int PhisicRow = _Row -1;
			beginRemoveRows(_Parent, PhisicRow, PhisicRow + _Count - 1);
			for (int Vfor = 0; Vfor < _Count; Vfor++)
			{
				RList.removeAt(PhisicRow + Vfor);
			}
			endRemoveRows();
			Res = true;
		}
		else
			Res = false;
	}
	else
	{
		beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
		for (int Vfor = 0; Vfor < _Count; Vfor++)
			RList.removeAt(_Row + Vfor);
		endRemoveRows();
		Res = true;
	}
	return Res;
}

QVariant ResourceModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}

QMimeData* ResourceModel::mimeData(const QModelIndexList &indexes) const
{
	ResourceMimeData* MimeData = new ResourceMimeData();
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid())
		{
			MimeData->setResource(resource(Index));
		}
	}
	return MimeData;
}

Qt::ItemFlags ResourceModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;
	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
		return defaultFlags;
}

QStringList ResourceModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

void ResourceModel::addResource(const Resource& _Resource)
{
	beginInsertRows(QModelIndex(), RList.size(), RList.size() + 1);
	RList.push_back(_Resource);
	endInsertRows();
}

void ResourceModel::removeResource(const Resource& _Resource)
{
	int Index = RList.indexOf(_Resource);
	if (Index >= 0)
	{
		beginRemoveRows(QModelIndex(), Index, Index + 1);
		RList.removeAt(Index);
		endRemoveRows();
	}
}

void ResourceModel::setResourceList(const ResourceList& _List)
{
	RList = _List;
	reset();
}

void ResourceModel::setHasNullItem(const QImage& _NullItemThumbnail, const Resource& _NullItem)
{
	NullItem = _NullItem;
	HasNullItem = true;
	NullItemThumbnail = _NullItemThumbnail;
}

Resource ResourceModel::resource(const QModelIndex& _Index) const
{
	Resource Res;
	if (HasNullItem)
	{
		if (_Index.isValid() && _Index.row() > 0 && _Index.row() <= RList.size())
			Res = RList[_Index.row() -1];
		else
			Res = NullItem;
	}
	else
	{
		if (_Index.isValid() && _Index.row() < RList.size())
			Res = RList[_Index.row()];
	}
	return Res;
}
