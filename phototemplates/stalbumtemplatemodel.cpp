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

#include "stalbumtemplatemodel.h"

#include <QPixmap>

STAlbumTemplateModel::STAlbumTemplateModel(QObject* _Parent) : QAbstractListModel(_Parent)
{
	ThumbnailMaxSize = QSize(152,102);
}


int STAlbumTemplateModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return Templates.size();
}


QVariant STAlbumTemplateModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < Templates.size())
		{
			
			if (_Role == Qt::DecorationRole)
			{
				STPhotoLayoutTemplate CurrTemplate = indexTemplate(_Index);
				Res = QPixmap::fromImage(CurrTemplate.sampleThumbnail(ThumbnailMaxSize));
			}
		}
	}
	return Res;
}

QVariant STAlbumTemplateModel::headerData(int /*_Section*/, Qt::Orientation /*_Orientation*/, int /*_Role*/) const
{
	QVariant Res;
	return Res;
}


void STAlbumTemplateModel::setAlbum(const STPhotoBookTemplate& _Album)
{
	Templates = _Album.templates();
	reset();
}

void STAlbumTemplateModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STAlbumTemplateModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
} 

STPhotoLayoutTemplate STAlbumTemplateModel::indexTemplate(const QModelIndex& _Index) const
{
	return Templates[_Index.row()];
}

