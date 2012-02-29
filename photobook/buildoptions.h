/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef BUILDOPTIONS_H
#define BUILDOPTIONS_H

#include "stphotobookexport.h"
#include "templateinfo.h"
#include <QString>
#include <QDate>

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT BuildOptions
{
	bool IgnoreExifRotation, AutoadjustFrames, UseTexts, AutoFillBackgrounds, PagesFromImages, ExpandImagesToFillFrames;
	int PagesToFill;
	QString Title, SubTitle, Author;
	QDate FromDate, ToDate;

	void clear();

public:
	BuildOptions();
	void setDefaults(MetaInfo::EnTemplateType _Type, int _MinPages = 1);
	void setIgnoreExifRotation(bool _Value) { IgnoreExifRotation = _Value; }
	bool ignoreExifRotation() const { return IgnoreExifRotation; }
	void setAutoadjustFrames(bool _Value) { AutoadjustFrames = _Value; }
	bool autoadjustFrames() const { return AutoadjustFrames; }
	void setUseTexts(bool _Value) { UseTexts = _Value; }
	bool useTexts() const { return UseTexts; }
	void setAutoFillBackgrounds(bool _Value) { AutoFillBackgrounds = _Value; }
	bool autoFillBackgrounds() const { return AutoFillBackgrounds; }
	void setPagesFromImages( bool _Value ) { PagesFromImages = _Value; }
	//!Indicates if builder calculates num pages from num selected images.
	bool pagesFromImages() const { return PagesFromImages; }
	void setExpandImagesToFillFrames(bool _Value) { ExpandImagesToFillFrames = _Value; }
	bool expandImagesToFillFrames() const { return ExpandImagesToFillFrames; }
	void setPagesToFill(int _Value) { PagesToFill = _Value; }
	int pagesToFill() const { return PagesToFill; }
	void setSubTitle(const QString& _Value) { SubTitle = _Value; }
	QString subTitle() const { return SubTitle; }
	void setTitle(const QString& _Title) { Title = _Title; }
	QString title() const { return Title; }
	void setAuthor(const QString& _Author) { Author = _Author; }
	QString author() const { return Author; }
	void setFromDate(const QDate& _Value) { FromDate = _Value; }
	QDate fromDate() const { return FromDate; }
	void setToDate(const QDate& _Value) { ToDate = _Value; }
	QDate toDate() const { return ToDate; }
};
}

#endif // BuildOptions_H
