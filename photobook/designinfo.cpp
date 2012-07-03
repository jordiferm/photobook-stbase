/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "designinfo.h"
#include "collectioninfo.h"
#include <QDebug>

using namespace SPhotoBook;

DesignInfo::DesignInfo(const QString& _Name) : Name(_Name), PublicVersion(0), IsPublic(false)
{
}


QString DesignInfo::imageFile() const
{
	QString Res;
	bool TheresDesignThumbnail = false;
	if (!ImageFile.isEmpty())
		TheresDesignThumbnail = QFile::exists(ImageFile);

	if (!TheresDesignThumbnail)
	{
		CollectionInfo CInfo;
		QFileInfo ThumbnailFile(CInfo.thumbnailFileName());
		Res = ThumbnailFile.fileName();
	}
	else
	{
		QFileInfo ImageFileInfo(ImageFile);
		Res = ImageFileInfo.fileName();
	}

	return Res;
}

void DesignInfo::setMetaInfo(const MetaInfo& _MInfo)
{
	MInfo = _MInfo;
	setImageFile(MInfo.imagePath());
}

