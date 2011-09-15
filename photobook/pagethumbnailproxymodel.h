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

#ifndef PAGETHUMBNAILPROXYMODEL_H
#define PAGETHUMBNAILPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "stphotobookexport.h"

namespace SPhotoBook
{
class PageThumbnailModel;
class ST_PHOTOBOOK_EXPORT PageThumbnailProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:

	class PageFilter
	{
		int MaxNumFrames;
		int MinNumFrames;
		bool IsNull;

	public:
		PageFilter(int _MinNumFrames, int _MaxNumFrames) : MinNumFrames(_MinNumFrames), MaxNumFrames(_MaxNumFrames), IsNull(false) {}
		PageFilter() { clear(); }
		int maxNumFrames() const { return MaxNumFrames; }
		int minNumFrames() const { return MinNumFrames; }
		void clear() { MaxNumFrames = 0; MinNumFrames = 0; IsNull = true; }
		bool isNull() const { return IsNull; }
	};



private:
	PageThumbnailModel* SourceModel;
	PageFilter Filter;

public:
	PageThumbnailProxyModel(QObject* _Parent = 0);
	PageThumbnailModel* sourceModel() { return SourceModel; }
	void setSourceModel(PageThumbnailModel* _SourceModel);

	//Filtering
	void clearFilter();
	PageFilter filter() const { return Filter; }
	void setPageFilter(const PageFilter& _Filter);
	virtual bool filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const;
	//Drag and Drop
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
};

}
Q_DECLARE_METATYPE(SPhotoBook::PageThumbnailProxyModel::PageFilter)

#endif // PAGETHUMBNAILPROXYMODEL_H
