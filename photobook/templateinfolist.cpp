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
#include <QDir>
#include <QMap>
#include <QDebug>
#include "templatepaths.h"
#include "collectioninfo.h"

using namespace SPhotoBook;

TemplateInfoList::TemplateInfoList()
{
}

void TemplateInfoList::addDesignNames(const QDir& _BaseDir, const QString& _Name, const QString& _SizeName)
{
	QMap <MetaInfo::EnTemplateType, TemplateInfo> TInfoMap;
	//For each design_name
	QDir BaseDir(_BaseDir.absoluteFilePath(_Name + "/" + _SizeName));
	QStringList DesignList = BaseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::iterator it = DesignList.begin();
	while (it != DesignList.end())
	{
		//Load metainfo for current design.
		QString MetaInfoFileName = CollectionInfo::xmlMetaInfoFileNameFromPath(QDir(BaseDir.absoluteFilePath(*it)));
		MetaInfo MInfo;
		try
		{
			MInfo.load(MetaInfoFileName);
			TemplateInfo NewTInfo(_BaseDir.absolutePath(), _Name, _SizeName, MInfo.templateType());
			//Check if there is templates of current type
			if (!TInfoMap.contains(MInfo.templateType()))
				TInfoMap.insert(NewTInfo.type(), NewTInfo);
			DesignInfo CDesignInfo(*it);
			CDesignInfo.setMetaInfo(MInfo);
			CDesignInfo.setDescription(MInfo.description());
			//TODO: set Design image
			TInfoMap[NewTInfo.type()].addDesign(CDesignInfo);
		}
		catch (...) //Ignore errors loading metainfo.xml
		{}

		++it;
	}
	*this += TInfoMap.values();
}

void TemplateInfoList::load(bool _Sync)
{
	//For each path
	QStringList PathList = TemplatePaths::pathList();
	QStringList::iterator pit = PathList.begin();
	while (pit != PathList.end())
	{
		//For each template name
		QDir BaseDir(*pit);
		QStringList TNameList = BaseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList::iterator tnit = TNameList.begin();
		while (tnit != TNameList.end())
		{
			//For each size
			QDir NameDir(BaseDir.absoluteFilePath(*tnit));
			QStringList TSizeList = NameDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			QStringList::iterator snit = TSizeList.begin();
			while (snit != TSizeList.end())
			{
				addDesignNames(BaseDir, NameDir.dirName(), QDir(*snit).dirName());
				++snit;
			}
			++tnit;
		}
		++pit;
	}


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

TemplateInfoList TemplateInfoList::subList(MetaInfo::EnTemplateType _Type) const
{
	TemplateInfoList Res;
	TemplateInfoList::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if (it->type() == _Type)
			Res.push_back(*it);
	}
	return Res;
}


TemplateInfoList TemplateInfoList::sizes(const QString& _TemplateName, MetaInfo::EnTemplateType _Type) const
{
	TemplateInfoList Res;
	TemplateInfoList SameTypeList = subList(_Type);
	TemplateInfoList::const_iterator it;
	for (it = SameTypeList.begin(); it != SameTypeList.end(); ++it)
	{
		if (it->name() == _TemplateName)
			Res.push_back(*it);
	}
	return Res;
}

