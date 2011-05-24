/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STPHOTOBOOKBUILDOPTIONS_H
#define STPHOTOBOOKBUILDOPTIONS_H

#include "stphotolayout.h"
#include "stphototemplatesexport.h"

class ST_PHOTOTEMPLATES_EXPORT STPhotoBookBuildOptions
{
	bool IgnoreExifRotation, AutoadjustFrames, UseTexts, AutoFillBackgrounds;
	int PagesToFill;
	QString Title;
	QDate FromDate, ToDate;

	void clear();

public:
	STPhotoBookBuildOptions();
	void setDefaults(STPhotoLayout::EnLayoutType _Type, int _MinPages = 1);
	void setIgnoreExifRotation(bool _Value) { IgnoreExifRotation = _Value; }
	bool ignoreExifRotation() const { return IgnoreExifRotation; }
	void setAutoadjustFrames(bool _Value) { AutoadjustFrames = _Value; }
	bool autoadjustFrames() const { return AutoadjustFrames; }
	void setUseTexts(bool _Value) { UseTexts = _Value; }
	bool useTexts() const { return UseTexts; }
	void setAutoFillBackgrounds(bool _Value) { AutoFillBackgrounds = _Value; }
	bool autoFillBackgrounds() const { return AutoFillBackgrounds; }
	void setPagesToFill(int _Value) { PagesToFill = _Value; }
	int pagesToFill() const { return PagesToFill; }
	void setTitle(const QString& _Title) { Title = _Title; }
	QString title() const { return Title; }
	void setFromDate(const QDate& _Value) { FromDate = _Value; }
	QDate fromDate() const { return FromDate; }
	void setToDate(const QDate& _Value) { ToDate = _Value; }
	QDate toDate() const { return ToDate; }
};

#endif // STPHOTOBOOKBUILDOPTIONS_H
