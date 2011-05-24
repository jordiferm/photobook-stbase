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
#include "stphotobookcollectioninfo.h"
#include "stimage.h"
#include "sapplication.h"
#include <QDir> 


QString STPhotobookCollectionInfo::DefaultRootPathName = SApplication::dbFilesPath() + "/photobooks";
		
STPhotobookCollectionInfo::STPhotobookCollectionInfo(const QString& _PhotoBookName, const QString& _RootPathName) : 
		PhotoBookName(_PhotoBookName), RootPathName(_RootPathName)
{
}

STPhotobookCollectionInfo::STPhotobookCollectionInfo(const QDir& _Path)
{
	QString CleanPath = QDir::fromNativeSeparators(QDir::cleanPath(_Path.absolutePath()));
	if (CleanPath.endsWith('/'))
		CleanPath = CleanPath.left(CleanPath.length() - 1);
	int LastSepPos = CleanPath.lastIndexOf('/');  
	PhotoBookName = CleanPath.right(CleanPath.length() - LastSepPos - 1); 
	RootPathName = CleanPath.left(LastSepPos); 
}


QString STPhotobookCollectionInfo::thumbnailFileName() const
{
	return photoBookPath() + "/thumbnail.jpg";
}

QString STPhotobookCollectionInfo::xmlFileName() const
{
	return photoBookPath() + "/photobook.xml";
}

QString STPhotobookCollectionInfo::xmlAutoSaveFileName() const
{
	return photoBookPath() + "/autosave.xml";
}

QString STPhotobookCollectionInfo::trayImagesFileName() const
{
	return photoBookPath() + "/trayimages.txt";
}

//! returns the hash name for _SourceFileName including the path. 
QString STPhotobookCollectionInfo::imageFilePath(const QString& _SourceFileName) const
{
	return  photoBookPath() + "/" + STImage::hashFileName(_SourceFileName); 
}

QString STPhotobookCollectionInfo::imageFilePath(const STImage& _Image, const QString& _Format) const
{
	return photoBookPath() + "/" + _Image.hashString() + "." + _Format; 
}

QString STPhotobookCollectionInfo::bookmarkTemplatesFile(const QSize& _Size)
{
	return 	DefaultRootPathName + QString("/templateBookmark_%1_%2.kpspltml").arg(_Size.width()).arg(_Size.height());
}
