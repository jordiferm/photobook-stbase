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
#ifndef STCOLLECTIONINFO_H
#define STCOLLECTIONINFO_H

#include "stphotobookexport.h"
#include <QString>
#include <QDir>

/**
	Stores some photobook information like files on disk, metadata, etc...
	@author Shadow
*/
class STImage;
class QSize;

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT CollectionInfo
{
	QString PhotoBookName; 
	QString RootPathName; 
	static QString DefaultRootPathName; 
	
public:
	CollectionInfo(const QString& _PhotoBookName = "noname",
									  const QString& _RootPathName = DefaultRootPathName);
	//! Gets the name from last Dir on path.
	CollectionInfo(const QDir& _Path);
	void setRootPathName(const QString& _PathName) { RootPathName = _PathName; }
	QString rootPathName() const { return RootPathName; } 
	void setPhotoBookName(const QString& _Name) { PhotoBookName = _Name; }
	QString photoBookName() const { return PhotoBookName; } 
	QString photoBookPath() const { return rootPathName() + "/" + photoBookName(); }
	QString thumbnailFileName() const;
	QString xmlFileName() const;
	QString xmlAutoSaveFileName() const;
	QString xmlLayoutsFileName() const;
	QString xmlCoversFileName() const;
	QString xmlBackCoverFileName() const;
	QString xmlMetaInfoFileName() const;
	static QString xmlMetaInfoFileNameFromPath(const QDir& _BasePath);
	QString trayImagesFileName() const;


	//! returns the hash name for _SourceFileName including the path. 
	QString imageFilePath(const QString& _SourceFileName) const;
	QString imageFilePath(const STImage& _Image, const QString& _Format) const;
	static QString defaultRootPathName() { return DefaultRootPathName; }
};
}
#endif
