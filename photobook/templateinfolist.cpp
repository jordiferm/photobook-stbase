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

void TemplateInfoList::addDesignNames(const QDir& _BaseDir, const QString& _Name, const QString& _SizeName, bool _OnDiskOnly)
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
			DesignInfo CDesignInfo(*it);
			CDesignInfo.setMetaInfo(MInfo);
			CDesignInfo.setDescription(MInfo.description());

			bool AddDesign;
			if (_OnDiskOnly)
				AddDesign = NewTInfo.designOnDisk(CDesignInfo);
			else
				AddDesign = true;

			if (AddDesign)
			{
				//Check if there is templates of current type
				if (!TInfoMap.contains(MInfo.templateType()))
					TInfoMap.insert(NewTInfo.type(), NewTInfo);
				//TODO: set Design image
				TInfoMap[NewTInfo.type()].addDesign(CDesignInfo);
			}
		}
		catch (...) //Ignore errors loading metainfo.xml
		{}

		++it;
	}
	addTemplateInfoList(TInfoMap.values());
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
	return subList(_Type).sizes(_TemplateName);
}

TemplateInfoList TemplateInfoList::sizes(const QString& _TemplateName) const
{
	TemplateInfoList Res;
	TemplateInfoList::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if (it->name() == _TemplateName)
			Res.push_back(*it);
	}
	return Res;
}


//! Returns -1 if template does not exist.
int TemplateInfoList::indexOf(const TemplateInfo& _TemplateInfo)
{
	int Res = -1;
	bool Found = false;
	int Cnt = 0;
	while (!Found && Cnt < size())
	{
		Found = (value(Cnt) == _TemplateInfo);
		if (!Found)
			Cnt++;
	}

	if (Found)
		Res = Cnt;
	else
		Res = -1;
	return Res;

}

void TemplateInfoList::addTemplateInfo(const TemplateInfo& _TemplateInfo)
{
	int Index = indexOf(_TemplateInfo);
	if (Index != -1)
		(*this)[Index] = _TemplateInfo;
	else
		push_back(_TemplateInfo);
}

void TemplateInfoList::addTemplateInfoList(const TemplateInfoList& _TemplateInfoList)
{
	TemplateInfoList::const_iterator it;
	for (it = _TemplateInfoList.begin(); it != _TemplateInfoList.end(); ++it)
	{
		addTemplateInfo(*it);
	}
}

void TemplateInfoList::addTemplateInfoList(const QList<TemplateInfo> _List)
{
	QList<TemplateInfo>::const_iterator it;
	for (it = _List.begin(); it != _List.end(); ++it)
	{
		addTemplateInfo(*it);
	}
}

void TemplateInfoList::updateTemplateInfo(const TemplateInfo& _Old, const TemplateInfo& _New)
{
	int Index = indexOf(_Old);
	if (Index != -1)
		(*this)[Index] = _New;
}

void TemplateInfoList::mergePublicInfo(const TemplateInfoList& _PublicTemplates)
{
	TemplateInfoList::const_iterator it;
	for (it = _PublicTemplates.begin(); it != _PublicTemplates.end(); ++it)
	{
		TemplateInfo PubTemplate = *it;
		int CIndex = indexOf(PubTemplate);
		if (CIndex != -1)
		{
			TemplateInfo& TRef = (*this)[CIndex];
			DesignInfoList& DesignListRef = TRef.designsRef();
			DesignInfoList PubDesigns = PubTemplate.designs();
			DesignInfoList::const_iterator dit;
			for (dit = PubDesigns.begin(); dit != PubDesigns.end(); ++dit)
			{
				int DesignIndex = DesignListRef.findDesignName(dit->name());
				if (DesignIndex != -1)
				{
					DesignListRef[DesignIndex].setPublicVersion(dit->publicVersion());
				}
			}
		}
	}
}
