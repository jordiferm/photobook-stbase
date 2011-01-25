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

#include "stalbumcolormodel.h"
#include <QPixmap>

STAlbumColorModel::STAlbumColorModel(QObject* _Parent) : QAbstractListModel(_Parent)
{
	ThumbnailMaxSize = QSize(76, 46);
}


int STAlbumColorModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return Colors.size();
}


QVariant STAlbumColorModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < Colors.size())
		{
			QColor CurrColor = Colors[_Index.row()];
			if (_Role == STAlbumColorModel::ColorRole)
			{
				Res = CurrColor;
			}
			else 
			if (_Role == Qt::DecorationRole)
			{
				QPixmap ColPixmap(ThumbnailMaxSize);
				ColPixmap.fill(CurrColor);
				Res = ColPixmap;
			}
		}
	}
	return Res;
}

QVariant STAlbumColorModel::headerData(int /*_Section*/, Qt::Orientation /*_Orientation*/, int /*_Role*/) const
{
	QVariant Res;
	return Res;
}


void STAlbumColorModel::setAlbum(const STPhotoBookTemplate& _Album)
{
	Colors = _Album.colors();
	reset();
}

void STAlbumColorModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STAlbumColorModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}

