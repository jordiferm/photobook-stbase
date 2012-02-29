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
** Aili Image (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "buildoptions.h"

using namespace SPhotoBook;

void BuildOptions::clear()
{
	IgnoreExifRotation = false;
	AutoadjustFrames = true;
	UseTexts = false;
	AutoFillBackgrounds = false;
	PagesFromImages = false;
	ExpandImagesToFillFrames = false;
	int PagesToFill = 0;
	QString Title = "";
	FromDate = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
	ToDate = QDate(QDate::currentDate().year(), 12, 31);
}


BuildOptions::BuildOptions()
{
	clear();
}

void BuildOptions::setDefaults(MetaInfo::EnTemplateType _Type, int _MinPages)
{
	clear();
	PagesToFill = _MinPages;
	switch (_Type)
	{
		case MetaInfo::TypeMultiPhoto :
			PagesToFill = 9999;
			PagesFromImages = true;
		case MetaInfo::TypeIdPhoto :
			IgnoreExifRotation = true;
			AutoadjustFrames = false;
			ExpandImagesToFillFrames = true;
		break;
		case MetaInfo::TypeCard :
			AutoadjustFrames = false;
			PagesFromImages = true;
			UseTexts = true;
		break;
		case MetaInfo::TypePhotoBook :
			AutoadjustFrames = true;
		break;
		case MetaInfo::TypeCalendar :
			AutoadjustFrames = true;
			UseTexts = true;
		break;
	}
}
