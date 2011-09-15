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
#ifndef STPHOTOBOOKCOLLECTIONMODEL_H
#define STPHOTOBOOKCOLLECTIONMODEL_H

#include <QAbstractListModel>
#include <QDir> 
#include <QFileInfoList> 
#include <QSize>
#include "stphototemplatesexport.h"

/**
	@author 
*/
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookCollectionModel : public QAbstractListModel
{
	QDir PBDir; 
	QFileInfoList PBFInfoList; 
	QSize ThumbnailSize;
	void loadDir(const QDir& _PBDir, bool _Recursive);


public:
	STPhotoBookCollectionModel(QObject* _Parent = 0, bool _Load = true);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	void setRootDir(const QDir& _Dir) { PBDir = _Dir; }
	QDir rootDir() const { return PBDir; }
	void setFiles(const QStringList& _Files);
	void load(bool _Recursive = false);
	QString photoBookPath(const QModelIndex& _Index) const;
	void setThumbnailSize(const QSize& _Size) { ThumbnailSize = _Size; }
	static bool areTherePhotoBooks();

};

#endif
