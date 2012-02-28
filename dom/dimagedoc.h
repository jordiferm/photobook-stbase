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

#ifndef DIMAGEDOC_H
#define DIMAGEDOC_H

#include "stdomexport.h"
#include "ddoc.h"
#include "sterror.h"
#include "exifmetadata.h"

namespace STDom
{
/*!
Image Digital Document with thumbnail capabilities.

@author Shadow
*/
class ST_DOM_EXPORT DImageDoc : public DDoc
{
public:
	class CInfo
	{
		bool IsNull;
		QSize Size;
		ExifMetadata::EnOrientation Orientation;

	public:
		CInfo() : IsNull(true) {}
		void setSize(const QSize& _Value) { Size = _Value; IsNull = false; }
		QSize size() const { return Size; }
		void setOrientation(ExifMetadata::EnOrientation _Value) { Orientation = _Value; IsNull = false;}
		ExifMetadata::EnOrientation orientation() const { return Orientation; }
		bool isNull() const { return IsNull; }
	};

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

	//Image Info
	CInfo getInfo(const ExifMetadata& _ExMData) const;
	CInfo getInfo() const;

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
