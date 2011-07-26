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

#include "pagethumbnailproxymodel.h"
#include "pagethumbnailmodel.h"
#include "templatescenemimedata.h"
#include "photobookmimetypes.h"

using namespace SPhotoBook;

PageThumbnailProxyModel::PageThumbnailProxyModel(QObject* _Parent) : QSortFilterProxyModel(_Parent)
{
}

void PageThumbnailProxyModel::setSourceModel(PageThumbnailModel* _SourceModel)
{
	QSortFilterProxyModel::setSourceModel(_SourceModel);
	SourceModel = _SourceModel;
}

//Filtering
void PageThumbnailProxyModel::clearFilter()
{
	Filter.clear();
	invalidateFilter();
}

void PageThumbnailProxyModel::setPageFilter(const PageFilter& _Filter)
{
	Filter = _Filter;
	invalidateFilter();
}

bool PageThumbnailProxyModel::filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const
{
	bool Res = true;

	if (!Filter.isNull())
	{
		TemplateScene* Template = SourceModel->page(SourceModel->index(_SourceRow, 0, _SourceParent));
		int NumPhotoItems = Template->numPhotoItems();

		Res = NumPhotoItems <= Filter.maxNumFrames() && NumPhotoItems >= Filter.minNumFrames();
	}

	return Res;
}

Qt::ItemFlags PageThumbnailProxyModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;

	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
	{
		return defaultFlags;
	}
}

QStringList PageThumbnailProxyModel::mimeTypes() const
{
	QStringList types;
	types << TEMPLATE_MIMETYPE;
	return types;
}

QMimeData* PageThumbnailProxyModel::mimeData(const QModelIndexList &indexes) const
{
	TemplateSceneMimeData* mimeData = new TemplateSceneMimeData();
	if (SourceModel)
	{
		QModelIndex Index = indexes.at(0);
		if (Index.isValid())
		{
			QModelIndex SourceIndex = mapToSource(Index);
			if (SourceIndex.isValid())
				mimeData->setTemplate(SourceModel->page(SourceIndex));
		}
	}
	return mimeData;
}
