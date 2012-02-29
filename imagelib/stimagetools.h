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
#ifndef STIMAGETOOLS_H
#define STIMAGETOOLS_H
#include "stimagelibexport.h"

#include <QFileInfoList> 

/**
Tools to manage images.

	@author
*/
class QDir; 
class ST_IMAGELIB_EXPORT STImageTools
{
public:
	//! \returns the common dir for all _Files.
	static QString commonDir(const QFileInfoList& _Files);
	//! \returns a QFileInfoList with all readable images inside _Dir.
	static QFileInfoList recursiveImagesEntryInfoList(const QDir& _Dir, bool _Recursive);
	//! Search for images in all system dirs except _ExcludedDirs
	static QFileInfoList searchDeviceImages(const QFileInfoList& _ExcludedDirs);
	static QFileInfo searchDriveWithFiles(const QFileInfoList& _ExcludedDrives);
	static QDir searchFirstPathWithImages(const QDir& _SourceDir, bool& _Found);
	static QDir searchFirstDrivePathWithImages(bool& _Found);
};

#endif
