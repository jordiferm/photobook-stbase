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
** Starblitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "stphotobookbuildoptions.h"

void STPhotoBookBuildOptions::clear()
{
	IgnoreExifRotation = false;
	AutoadjustFrames = true;
	UseTexts = false;
	AutoFillBackgrounds = false;
	int PagesToFill = 0;
	QString Title = "";
	FromDate = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
	ToDate = QDate(QDate::currentDate().year(), 12, 31);
}


STPhotoBookBuildOptions::STPhotoBookBuildOptions()
{
	clear();
}

void STPhotoBookBuildOptions::setDefaults(STPhotoLayout::EnLayoutType _Type, int _MinPages)
{
	clear();
	PagesToFill = _MinPages;
	switch (_Type)
	{
		case STPhotoLayout::TypeMultiPhoto :
			PagesToFill = 9999;
		case STPhotoLayout::TypeIdPhoto :
			IgnoreExifRotation = true;
			AutoadjustFrames = false;
		break;
		case STPhotoLayout::TypeCard :
			AutoadjustFrames = false;
			UseTexts = true;
		break;
		case STPhotoLayout::TypePhotoBook :
			AutoadjustFrames = true;
		break;
		case STPhotoLayout::TypeCalendar :
			AutoadjustFrames = true;
			UseTexts = true;
		break;
	}
}
