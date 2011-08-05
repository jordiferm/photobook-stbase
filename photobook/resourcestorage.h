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

#ifndef RESOURCESTORAGE_H
#define RESOURCESTORAGE_H
#include "stphotobookexport.h"
#include <QDir>
#include <QFileInfo>

namespace SPhotoBook
{
class Resource;
class ST_PHOTOBOOK_EXPORT ResourceStorage
{
	bool Saved;
	QDir LoadDir;
	QDir SaveDir;

public:
    ResourceStorage();
	void setLoadDir(const QDir& _LoadDir) { LoadDir = _LoadDir; }
	void setSaveDir(const QDir& _SaveDir) { SaveDir = _SaveDir; }
	void save(const QFileInfo& _SourceImage);
	void save(const Resource& _Resource);
	QDir resourcesDir() const;
	QString xmlSourceName(const QFileInfo& _FileInfo) const;
	QFileInfo fileInfo(const QString& _XmlSourceName) const;
};
}

#endif // RESOURCESTORAGE_H
