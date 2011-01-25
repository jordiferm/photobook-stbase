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
#include "stphotobookthumbnailmodel.h"
#include <QPainter> 

#include "stphotobook.h"
#include "sttemplatescene.h"

QImage STPhotoBookThumbnailModel::getThumbnail(STTemplateScene* _Scene)
{
	QSize ThumbSize = ThumbnailMaxSize;
	if (_Scene->width() > _Scene->height() && ThumbSize.width() < ThumbSize.height() ||
		_Scene->width() < _Scene->height() && ThumbSize.width() > ThumbSize.height() )
		ThumbSize.transpose();
	ThumbSize.setHeight(_Scene->height() * ThumbSize.width() / _Scene->width());
	QImage ResImg(ThumbSize, QImage::Format_RGB32);

	ResImg.fill(Qt::white);
	QPainter Painter(&ResImg);
	_Scene->prepareForPrint();
	_Scene->render(&Painter, ResImg.rect());
	return ResImg; 
}

STPhotoBookThumbnailModel::STPhotoBookThumbnailModel(QObject* _Parent)
 : QAbstractListModel(_Parent), ThumbnailMaxSize(120, 80)
{
}

int STPhotoBookThumbnailModel::rowCount(const QModelIndex& ) const
{
	return Thumbnails.size(); 
}

QVariant STPhotoBookThumbnailModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < Thumbnails.size())
		{
			if (_Role == Qt::DecorationRole)
			{
				Res = Thumbnails[_Index.row()];
			}
			else 
			if (_Role == Qt::DisplayRole)
			{
				Res = QString(tr("Sheet %1").arg(_Index.row()));
			}
		}
	}
	return Res;
}

void STPhotoBookThumbnailModel::setPhotoBook(const STPhotoBook* _PhotoBook)
{
	Thumbnails.clear();
	STPhotoBook::TPagesList Pages = _PhotoBook->pages(); 
	STPhotoBook::TPagesList::const_iterator it; 
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		Thumbnails.push_back(getThumbnail(*it));
	}
	reset();
}

void STPhotoBookThumbnailModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value; 
}

QSize STPhotoBookThumbnailModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize; 
}

void STPhotoBookThumbnailModel::updateThumbnail(const STPhotoBook* _PhotoBook, int _Page)
{
	if (_Page >= 0 && _Page < Thumbnails.size())
		Thumbnails[_Page] = getThumbnail(_PhotoBook->pages()[_Page]);
	emit dataChanged(index(_Page, 0), index(_Page, 0)); 
}

