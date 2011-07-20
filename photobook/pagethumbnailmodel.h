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
#ifndef PAGETHUMBNAILMODEL_H
#define PAGETHUMBNAILMODEL_H

#include <QAbstractListModel>
#include <QSize> 
#include <QList> 
#include <QImage>
#include "stphotobookexport.h"

/**
Model that generates Document page thumbnails.

	@author
*/
namespace SPhotoBook
{
class TemplateScene;
class PageList;
class ST_PHOTOBOOK_EXPORT PageThumbnailModel : public QAbstractListModel
{
Q_OBJECT
	QSize ThumbnailMaxSize;
	QList<QImage> Thumbnails; 
	QImage getThumbnail(TemplateScene* _Scene);
	
public:
	PageThumbnailModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	void setPages(const PageList& _Pages);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const;
	void updateThumbnail(const PageList& _Pages, int _Index);
};
}
#endif
