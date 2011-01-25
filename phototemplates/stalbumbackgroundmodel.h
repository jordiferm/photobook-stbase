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

#ifndef STALBUMBACKGROUNDMODEL_H
#define STALBUMBACKGROUNDMODEL_H

#include <QAbstractListModel>
#include <QFileInfoList>
#include <QSize>
#include "stphotolayout.h"
#include "stphototemplatesexport.h"

/**
Model per als fons de la pàgina de l'àlbum.

@author Shadow
*/
class ST_PHOTOTEMPLATES_EXPORT STAlbumBackgroundModel : public QAbstractListModel
{

public:
	enum EnUserRoles
	{
		FileName = Qt::UserRole,
		ThumbnailFileName
	};

private:
	QSize ThumbnailMaxSize;
	QFileInfoList ImageFiles;

	QPixmap getThumbNail(const QString& _ImageName) const;

public:
	STAlbumBackgroundModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	bool removeRows(int _Row, int _Count, const QModelIndex& _Parent);
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void setAlbum(const STPhotoBookTemplate& _Album);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const;
};

#endif
