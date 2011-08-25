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

#ifndef RESOURCELIST_H
#define RESOURCELIST_H
#include "stphotobookexport.h"

#include <QList>
#include <QDir>
#include <QSize>
#include <QImage>
#include "resource.h"
#include "sterror.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT ResourceList : public QList<Resource>
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	QDir LoadedDir;
	QSize ThumbnailMaxSize;

public:
	ResourceList();
	ResourceList subList(Resource::EnResourceType _Type) const;
	QDir loadDir() const { return LoadedDir; }
	void load(const QDir& _Dir);
	QStringList save(const QDir& _Dir);
	Resource resource(Resource::EnResourceType _Type, const QString& _Name) const;
	QString thumbnailAbsoluteFileName(const Resource& _Resource) const;
	QImage getThumbNail(const Resource& _Resource) const;
	QSize thumbnailMaxSize() const { return ThumbnailMaxSize; }
	void calcHashCodes();
	void addNonExist(const ResourceList& _RecToAdd);
};
}
#endif // RESOURCELIST_H
