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
#ifndef STPHOTOBOOKTHUMBNAILMODEL_H
#define STPHOTOBOOKTHUMBNAILMODEL_H

#include <QAbstractListModel>
#include <QSize> 
#include <QList> 
#include <QImage>
#include "stphototemplatesexport.h"

/**
Model that generates PhotoBook pages thumbnails.

	@author
*/
class STPhotoBook; 
class STTemplateScene; 
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookThumbnailModel : public QAbstractListModel
{
Q_OBJECT
	QSize ThumbnailMaxSize;
	QList<QImage> Thumbnails; 
	QImage getThumbnail(STTemplateScene* _Scene);
	
public:
	STPhotoBookThumbnailModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	void setPhotoBook(const STPhotoBook* _PhotoBook);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const;
	void updateThumbnail(const STPhotoBook* _PhotoBook, int _Page);
};

#endif
