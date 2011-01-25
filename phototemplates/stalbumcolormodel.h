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

#ifndef STALBUMCOLORMODEL_H
#define STALBUMCOLORMODEL_H

#include <QAbstractListModel>
#include <QSize>
#include "stphotolayout.h"
#include "stphototemplatesexport.h"

/**
Model de colors per a un �lbum. Crea petits pixmaps amb els colors de l'�lbum.

@author Shadow
*/
class STPhotoBookTemplate;
class ST_PHOTOTEMPLATES_EXPORT STAlbumColorModel : public QAbstractListModel
{
public:
	enum EnUserRoles
	{
		ColorRole = Qt::UserRole
	};

private:
	QSize ThumbnailMaxSize;
	STPhotoBookTemplate::TColorList Colors;

public:
	STAlbumColorModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void setAlbum(const STPhotoBookTemplate& _Album);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const;
};

#endif
