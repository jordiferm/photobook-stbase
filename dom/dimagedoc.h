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

#ifndef DIMAGEDOC_H
#define DIMAGEDOC_H

#include "stdomexport.h"
#include "ddoc.h"
#include "sterror.h"

namespace STDom
{
/*!
Image Digital Document with thumbnail capabilities.

@author Shadow
*/

class ST_DOM_EXPORT DImageDoc : public DDoc
{

public:
	ST_DECLARE_ERRORCLASS();
	enum { Type = 1 };

protected:
	virtual QImage loadThumbnailImage(bool _CreateIfNotExist = true);

public:
	DImageDoc(const QString& _FilePath = "", qint64 _Key = -1, qint64 _ParentKey = -1);
	DImageDoc(const QFileInfo& _File, qint64 _Key = -1, qint64 _ParentKey = -1);
	virtual int type() const { return Type; }

	//Big Thumbnails
	virtual void createBigThumbnail() const;
	virtual QImage loadBigThumbnailImage(bool _CreateIfNotExist = true) const;
	virtual void removeBigThumbnail();
	virtual bool hasBigThumbnail() const;

	//Thumbnails
	virtual bool createThumbnail(bool _CheckForMetadata = true) const;

	//Original image.
	void save(const QImage& _Image, bool _SaveOriginal);
	QImage originalImage() const;
	bool originalSaved() const;
	void removeOriginal();
	virtual bool remove();
	QString mkOriginalImagesPath() const;
	QString originalImageFileName() const;
	static bool supportedFileType(const QFileInfo& _File);
};
} //namespace
#endif // DIMAGEDOC_H
