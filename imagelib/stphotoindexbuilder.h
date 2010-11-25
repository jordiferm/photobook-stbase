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
#ifndef STPHOTOINDEXBUILDER_H
#define STPHOTOINDEXBUILDER_H
#include <QFileInfoList>
#include <QColor>
#include <QImage>
#include <QList> 
#include "sterror.h"
#include "stimagelibexport.h"

/**
A class to build photoíndex of images.

	@author
*/
class QProgressBar;
class ST_IMAGELIB_EXPORT STPhotoIndexBuilder
{
public:
	ST_DECLARE_ERRORCLASS();
	typedef QList<int>  TNumberList;

private:
	int MaxPageImages; 
	int Dpis;
	int MarginPix;
	int NRowImages;
	int ThumbWidth, ThumbHeight;
	qreal MarginMm, SpacingMm, ThumbRatio;
	QFileInfoList Images; 
	QString BackgroundImageFileName; 
	QColor BackgroundColor; 
	QSizeF RenderSize;
	QSize RenderPixelSize;
	TNumberList Numbers; 
	bool DrawFrames; 
	bool DrawNumbers; 
	bool DrawFileNames; 


	QSize calcLayout(int _DesiredPageImages, const QSizeF& _RenderSize, int _Dpis);
	void calcLayout();

public:
	STPhotoIndexBuilder(int _MaxPageImages = 15, const QSizeF& _RenderSize = QSizeF(150,100), qreal _MarginMm = 2, qreal _SpacingMm = 1, int Dpis = 300);
	void setMaxPageImages(int _MaxPageImages); 
	void setMargin(qreal _MarginMm); 
	void setThumbRatio(qreal _Ratio); 
	void setSpacing(qreal _SpacingMm); 
	void setImages(const QFileInfoList& _FileInfo); 
	void setImageNumbers(const TNumberList& _Numbers); 
	void setDrawFrames(bool _Value) { DrawFrames = _Value; }
	void setDrawNumbers(bool _Value) { DrawNumbers = _Value;} 
	void setDrawFileNames(bool _Value) { DrawFileNames = _Value; }
	QFileInfoList images() const { return Images; }
	void setBackgroundImageFileName(const QString& _BackgroundImageFileName);
	void setBackgroundColor(const QColor&	 _Color); 
	int numPages() const;
	//! \param _RenderSize is the result image size in mm.
	QImage render(int _Page, QProgressBar* _PrgBar = 0) const; 
};

#endif
