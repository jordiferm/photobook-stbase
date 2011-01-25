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

#ifndef STPHOTOBOOKCLIPARTMODEL_H
#define STPHOTOBOOKCLIPARTMODEL_H

#include <QAbstractListModel>
#include <QFileInfoList>
#include <QSize>
#include "stphotolayout.h"
#include "stphototemplatesexport.h"
#include "stphotobookfolderdocsmodel.h"

/**
Cliparts Model.

@author Shadow
*/
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookClipartModel : public STPhotoBookFolderDocsModel
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
	QString LastPath;

	QString thumbnailFileName(const QString& _FileName) const;
	QPixmap getThumbNail(const QString& _ImageName) const;

public:
	STPhotoBookClipartModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	bool removeRows(int _Row, int _Count, const QModelIndex& _Parent);
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void setPhotoBook(const STPhotoBookTemplate& _Album); //!!! OBSOLETE !!!
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const;
	void loadFromPath(const QString& _PathName);
	void reload();

	void setPhotoBookTemplate(const STPhotoBookTemplate& _Template);
	QStringList fileFilter() const;
	QString filePath(const QModelIndex& _Index) const;
};

#endif
