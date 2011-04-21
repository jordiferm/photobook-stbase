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
#ifndef EXIFMETADATA_H
#define EXIFMETADATA_H

#include <QImage>
#include "stimagelibexport.h"

#include "sterror.h"
#include "exif.hpp"

/**
Classe per a obtenir la metainformació de les extensions exif de JPG.

@author shadow
*/
class QString;
class ST_IMAGELIB_EXPORT ExifMetadata
{
public:	
	ST_DECLARE_ERRORCLASS();
/*
Value  0th Row  0th Column
  1  top  left side
  2  top  right side
  3  bottom  right side
  4  bottom  left side
  5  left side  top
  6  right side  top
  7  right side  bottom
  8  left side  bottom	
	
 1        2       3      4         5            6           7          8

888888  888888      88  88      8888888888  88                  88  8888888888
88          88      88  88      88  88      88  88          88  88      88  88
8888      8888    8888  8888    88          8888888888  8888888888          88
88          88      88  88
88          88  888888  888888
*/	
	enum EnOrientation
	{
		Orientation_Top_Left = 1,
		Orientation_Top_Right = 2,
		Orientation_Bottom_Right = 3,
		Orientation_Bottom_Left = 4,
		Orientation_Left_Top = 5,
		Orientation_Right_Top = 6,
		Orientation_Right_Bottom = 7,
		Orientation_Left_Bottom = 8
	};
	
private:
	QImage ThumbNail;
	Exiv2::ExifData CExifData;
	
public:
	ExifMetadata();
	~ExifMetadata();
	bool load(const QString& _FileName);
	QImage getThumbnail() const;
	QVariant ExifValueToVariant(const Exiv2::Value& _Val) const;
	QVariant getKeyValue(const QString& _Key) const;
	EnOrientation getOrientation() const;
	QSize getSize() const;
	QSize adjustSizeToOrientation(const QSize& _Size, EnOrientation _Orientation) const;
	static QImage correctOrientation(const QImage& _Image, EnOrientation _Orientation);

};

#endif
