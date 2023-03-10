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

#ifndef DESIGNINFO_H
#define DESIGNINFO_H

#include <QString>
#include "stphotobookexport.h"
#include "metainfo.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT DesignInfo
{
public:
	enum EnResourceType
	{
		Masks,
		Frames,
		Clipart,
		Backgrounds,
		Fonts
	};

private:
	QString Name, Description, ImageFile;
	int PublicVersion;
	bool IsPublic;
	MetaInfo MInfo;

public:
	DesignInfo(const QString& _Name = "");
	void setName(const QString& _Value) { Name = _Value; }
	QString name() const { return Name; }
	void setDescription(const QString& _Value) { Description = _Value; }
	QString description() const { return Description; }
	void setImageFile(const QString& _ImageFile) { ImageFile = _ImageFile; }
	QString imageFile() const;
	void setPublicVersion(int _Version)  { PublicVersion = _Version; }
	int publicVersion() const { return PublicVersion; }
	void setIsPublic(bool _Value) { IsPublic = _Value; }
	bool isPublic() const { return IsPublic || PublicVersion > 0; }
	bool isUpToDate() const { return (isPublic() && MInfo.version() == PublicVersion) || !isPublic(); }
	void setMetaInfo(const MetaInfo& _MInfo);
	MetaInfo metaInfo() const { return MInfo; }
	bool isNull() const { return Name == ""; }
};

}
#endif // DESIGNINFO_H
