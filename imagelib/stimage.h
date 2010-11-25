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
#ifndef STIMAGE_H
#define STIMAGE_H

#include <QImage>
#include <QColor> 
#include "stimagelibexport.h"

/**
Starblitz Toolkit Image class.

	@author
*/
class SProcessStatusWidget; 
class QFileInfo;
class ST_IMAGELIB_EXPORT STImage : public QImage
{
	
public: 
	enum EnTransformCodes 
	{
		Rotate90,
		Rotate270,
		FlipX, 
  		FlipY
	};
	
	enum EnAlignment
	{
		AlignCenter, 
		AlignTopLeft
	};

	static QString DefaultEncryptionKey;
	static QString formatForFileFilter(const QList<QByteArray>& _Formats);

public:
	STImage();
	STImage(const QSize& _Size, QImage::Format _Format);
	STImage(const QImage& );
	//! It uses STImage load.
	STImage(const QString & fileName, const char * format = 0);
	~STImage();
	//!Overload of QImage load.to prevent a 8 bit JPG problem.
	bool load(const QString & fileName, const char * format = 0);
	//! Loads the thumbnail from _FilePath into current Image with minimum size of _MinThumbSize.
	void loadThumbnail(const QString& _FilePath, QSize _MinThumbSize = QSize(100, 100));
	//!crops the image.
	STImage crop(const QPoint& topLeft, const QPoint& bottomRight); 
	//!rotate image by determining correct angle from two points
	STImage correctTilt(const QPoint& _P1, const QPoint& _P2);
	STImage transform(EnTransformCodes _TransformCode);
	STImage correctRedEye(QPoint topLeftExtreme, QPoint bottomRightExtreme,
                             SProcessStatusWidget* SProcessStatusWidget = 0);
	STImage improveColorBalance(SProcessStatusWidget* _SProcessStatusWidget = 0);
	STImage enhanceImageContrast(SProcessStatusWidget* _SProcessStatusWidget = 0);
	STImage contrast(const unsigned int _Sharpen );
	void blowFishEncode(const QString& _Key = DefaultEncryptionKey);
	void blowFishDecode(const QString& _Key = DefaultEncryptionKey);
	STImage sigmoidalContrast(const unsigned int _Sharpen, const double _Contrast, const double _MidPoint);
	STImage despeckle();
	STImage enhance(); 
	STImage normalize(); 
	STImage equalize();
	STImage sharpen(const double _Radius = 0.0, const double _Sigma = 1.0);
	//! Tipical values From 0.8 to 2.3
	STImage gamma(const double _Gamma);
	STImage gammaRGB(const double 	_Red, const double _Green, const double _Blue);
	STImage blackAndWhite(SProcessStatusWidget* _SProcessStatusWidget = 0); 
	STImage sepia(SProcessStatusWidget* _SProcessStatusWidget = 0);
	STImage applyMargin(const QSize& _DestSize, const QColor& _MarginColor = QColor("#FFFFFF"), STImage::EnAlignment _Align = AlignCenter);
	void multiplyImage(const QSize& _DestSize, QPainter* _Painter) const;
	STImage multiplyImage(const QSize& _DestSize) const;

	//! Prints the image using whole _Printer page.
	void print(QPainter& _Painter, bool _Center, bool _Cut = false);
	void print(QPrinter* _Printer, bool _Center, bool _Cut = false);
	//! \return true if _FInfo is a supportedImage
	static bool isSupportedImage(const QFileInfo& _FInfo);
	//! return a String containing all supported images formats to read as a QFileDialog filter syntax.
	static QString supportedFormatsToReadFilter();
	static QString supportedFormatsToWriteFilter();

	QString hashString() const;
	//! returns a hashName for _FilePath, returns an empty string if file does not exist.
	static QString hashString(const QString& _FilePath);
	static QString hashFileName(const QString& _FilePath); 

	static void setDefaultEncryptionKey(const QString& _Key) { DefaultEncryptionKey = _Key; }
	static QString defaultEncryptionKey() { return DefaultEncryptionKey; }
};

#endif
