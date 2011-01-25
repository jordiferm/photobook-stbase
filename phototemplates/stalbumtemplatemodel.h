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

#ifndef STALBUMTEMPLATEMODEL_H
#define STALBUMTEMPLATEMODEL_H

#include <QAbstractListModel>
#include "stphotolayout.h"
#include "stphototemplatesexport.h"

//!!!! D E P R E C A T E D !!!!
//! Use STTemplateModel instead.
/**
Model for album templates.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STAlbumTemplateModel : public QAbstractListModel
{
private:
	QSize ThumbnailMaxSize;
	STPhotoBookTemplate::TTemplateList Templates;

public:
	STAlbumTemplateModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void setAlbum(const STPhotoBookTemplate& _Album);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const; 
	STPhotoLayoutTemplate indexTemplate(const QModelIndex& _Index) const;

};

#endif
