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

#include "templateinfolist.h"

using namespace SPhotoBook;

TemplateInfoList::TemplateInfoList()
{
}

void TemplateInfoList::load(bool _Sync)
{

}

void TemplateInfoList::sync()
{

}

bool TemplateInfoList::isPublic()
{

}

QFileInfoList TemplateInfoList::globalResources(DesignInfo::EnResourceType _Type)
{

}

TemplateInfoList TemplateInfoList::subList(MetaInfo::EnTemplateType _Type)
{
	TemplateInfoList Res;
	TemplateInfoList::iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if (it->type() == _Type)
			Res.push_back(*it);
	}
	return Res;
}


TemplateInfoList TemplateInfoList::sizes(const QString& _TemplateName, MetaInfo::EnTemplateType _Type)
{
	TemplateInfoList Res;
	TemplateInfoList SameTypeList = subList(_Type);
	TemplateInfoList::iterator it;
	for (it = SameTypeList.begin(); it != SameTypeList.end(); ++it)
	{
		if (it->name() == _TemplateName)
			Res.push_back(*it);
	}
	return Res;
}

