/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef CHECKEDPROXYMODEL_H
#define CHECKEDPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QSize>
#include <QMap>
#include <QImage> 
#include <QModelIndex>
#include "stphotobookexport.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT CheckedProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT 
			
	QSize ThumbnailSize; 
	QMap<QModelIndex, int> CheckedImages;

protected:
	QImage CheckedImage; 
	int NumMatchLimit;
	
public:
	CheckedProxyModel(QObject* _Parent, const QImage& _CheckedImage);
	QVariant data(const QModelIndex& _Index, int _Role) const;
	void setThumbnailSize(const QSize& _Size) { ThumbnailSize = _Size; }
	virtual void setChecked(const QModelIndex& _Index, bool _Checked);
	void setSourceModel(QAbstractItemModel* _SourceModel);
	void setNumMatches(const QModelIndex& _Index, int _NumMatches);
	int numMatches(const QModelIndex& _Index) const;
	void incNumMatches(const QModelIndex& _Index);
	void decNumMatches(const QModelIndex& _Index);
	bool isChecked(const QModelIndex& _Index) const;
	int numCheckedImages() const;
	//! If _Value > 0 => numMatches will never been greater than _Value.
	void setNumMatchLimit(int _Value) { NumMatchLimit = _Value; }
	int numMatchLimit() const { return NumMatchLimit; }
	
public slots:
	void clearChecked();
	void checkAll();
	void invertChecked();	

signals:
	void checksChanged();
};
}
#endif
