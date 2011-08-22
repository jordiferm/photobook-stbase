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

#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include "stphotobookexport.h"
#include "resourcelist.h"
#include "resource.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT ResourceModel : public QAbstractListModel
{
    Q_OBJECT

	ResourceList RList;
	QImage NullItemThumbnail;
	Resource NullItem;
	bool HasNullItem;

public:
    explicit ResourceModel(QObject *parent = 0);
	int rowCount(const QModelIndex& /*_Parent*/) const;
	QVariant data(const QModelIndex& _Index, int _Role) const;
	bool removeRows(int _Row, int _Count, const QModelIndex& _Parent);
	QVariant headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QStringList mimeTypes() const;
	void addResource(const Resource& _Resource);
	void setResourceList(const ResourceList& _List);
	ResourceList resourceList() const { return RList; }
	void setHasNullItem(const QImage& _NullItemThumbnail, const Resource& _NullItem);
	Resource resource(const QModelIndex& _Index) const;

signals:

public slots:

};
}
#endif // RESOURCEMODEL_H
