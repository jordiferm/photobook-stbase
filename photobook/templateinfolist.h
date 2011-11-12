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

#ifndef TEMPLATEINFOLIST_H
#define TEMPLATEINFOLIST_H

#include <QList>
#include <QFileInfoList>
#include "stphotobookexport.h"
#include "templateinfo.h"


namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT TemplateInfoList : public QList<TemplateInfo>
{

public:
    TemplateInfoList();
	void addDesignNames(const QDir& _BaseDir, const QString& _Name, const QString& _SizeName);
	QFileInfoList globalResources(DesignInfo::EnResourceType _Type);
	TemplateInfoList subList(MetaInfo::EnTemplateType _Type) const;
	TemplateInfoList sizes(const QString& _TemplateName, MetaInfo::EnTemplateType _Type) const;
	TemplateInfoList sizes(const QString& _TemplateName) const;
	int indexOf(const TemplateInfo& _TemplateInfo);
	void addTemplateInfo(const TemplateInfo& _TemplateInfo);
	void addTemplateInfoList(const TemplateInfoList& _TemplateInfoList);
	void addTemplateInfoList(const QList<TemplateInfo> _List);
	void updateTemplateInfo(const TemplateInfo& _Source, const TemplateInfo& _Dest);
};

}

#endif // TEMPLATEINFOLIST_H
