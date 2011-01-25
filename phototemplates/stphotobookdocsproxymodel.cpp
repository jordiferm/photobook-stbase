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

#include "stphotobookdocsproxymodel.h"
#include <QMimeData>
#include <QUrl>

#include "stphotobookfolderdocsmodel.h"

STPhotoBookDocsProxyModel::STPhotoBookDocsProxyModel(QObject* _Parent) : QSortFilterProxyModel(_Parent), SourceModel(0)
{
}

void STPhotoBookDocsProxyModel::setSourceModel(STPhotoBookFolderDocsModel* _Model)
{
	QSortFilterProxyModel::setSourceModel(_Model);
	SourceModel = _Model;
}

void STPhotoBookDocsProxyModel::clearFolderFilter()
{
	FolderFilter = "";
	invalidateFilter();
}

void STPhotoBookDocsProxyModel::setFolderFilter(const QString& _Folder)
{
	FolderFilter = _Folder;
	invalidateFilter();
}

bool STPhotoBookDocsProxyModel::filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const
{
	bool Res = true;

	if (!FolderFilter.isEmpty())
		Res = SourceModel->filePath(SourceModel->index(_SourceRow, 0, _SourceParent)).contains(FolderFilter);

	return Res;
}

Qt::ItemFlags STPhotoBookDocsProxyModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;

	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
	{
		return defaultFlags;
	}
}

QStringList STPhotoBookDocsProxyModel::mimeTypes() const
{
	return SourceModel->mimeTypes();
}

QMimeData* STPhotoBookDocsProxyModel::mimeData(const QModelIndexList &indexes) const
{
	QModelIndexList SourceIndexes;
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid() && Index.row() > -1 && Index.row() < rowCount())
			SourceIndexes.push_back(mapToSource(Index));
	}
	if (!SourceIndexes.isEmpty())
		return SourceModel->mimeData(SourceIndexes);
	return QSortFilterProxyModel::mimeData(indexes);
}

