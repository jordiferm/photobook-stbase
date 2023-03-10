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
#include "pagethumbnailmodel.h"
#include <QPainter> 
#include <QDebug>

#include "templatescene.h"

using namespace SPhotoBook;
QImage PageThumbnailModel::getThumbnail(TemplateScene* _Scene)
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

PageThumbnailModel::PageThumbnailModel(QObject* _Parent, bool _DisplayLabels)
 : QAbstractListModel(_Parent), ThumbnailMaxSize(120, 80)
{
    DisplayLabels = _DisplayLabels;
}

int PageThumbnailModel::rowCount(const QModelIndex& ) const
{
	return Thumbnails.size(); 
}

QVariant PageThumbnailModel::data(const QModelIndex& _Index, int _Role) const
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
            if (_Role == Qt::DisplayRole && DisplayLabels)
			{
                if (_Index.row() == 0)
                    Res = QString(tr("Cover"));
                else
                    Res = QString(tr("Sheet %1").arg(_Index.row()));
			}
		}
	}
	return Res;
}

void PageThumbnailModel::setPages(const PageList& _Pages)
{
	Thumbnails.clear();
	Pages.clear();
	PageList::const_iterator it;
	for (it = _Pages.begin(); it != _Pages.end(); ++it)
	{
		Thumbnails.push_back(getThumbnail(*it));
		Pages.push_back(*it);
	}
	reset();
}

TemplateScene* PageThumbnailModel::page(const QModelIndex& _Index) const
{
	TemplateScene* Res = 0;
	if (_Index.isValid() && Pages.size() > _Index.row())
	{
		Res = Pages[_Index.row()];
	}
	return Res;
}

void PageThumbnailModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value; 
}

QSize PageThumbnailModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize; 
}

void PageThumbnailModel::updateThumbnail(const PageList& _Pages, int _Index)
{
	if (_Index >= 0 && _Index < Thumbnails.size())
		Thumbnails[_Index] = getThumbnail(_Pages[_Index]);
	emit dataChanged(index(_Index, 0), index(_Index, 0));

}

bool PageThumbnailModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
	for (int Vfor = 0; Vfor < _Count; Vfor++)
	{
		Thumbnails.removeAt(_Row);
		Pages.removeAt(_Row);
	}
	endRemoveRows();
	return true;
}

void PageThumbnailModel::addPage(TemplateScene* _Page)
{
	beginInsertRows(QModelIndex(), Thumbnails.size(), Thumbnails.size());
		Thumbnails.push_back(getThumbnail(_Page));
		Pages.push_back(_Page);
		//Thumbnails.insert(Thumbnails.size(), getThumbnail(_Page));
		//Pages.insert(Pages.size(), _Page);
	endInsertRows();
}
