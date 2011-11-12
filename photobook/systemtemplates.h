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

#ifndef SYSTEMTEMPLATES_H
#define SYSTEMTEMPLATES_H
#include "stphotobookexport.h"
#include "templateinfolist.h"
#include "sterror.h"

class QDir;
namespace SPhotoBook
{

class TemplateInfo;
class DesignInfo;
class ST_PHOTOBOOK_EXPORT SystemTemplates
{
	ST_DECLARE_ERRORCLASS();

public:
	static TemplateInfoList load();
	static void import(const QDir& _SourceDir, const QString& _NewName = "");
	static TemplateInfo templateInfo(const QDir& _Sourcedir);
	static bool existMetaInfo(const QDir& _SourceDir);
	static MetaInfo templateMetaInfo(const QDir& _Sourcedir);
	static bool existTemplate(const MetaInfo& _MetaInfo);
	static void exportTemplateDesign(const QDir& _DestDir, const TemplateInfo& _Template, const DesignInfo& _Design);
	static void deleteTemplateDesign(const TemplateInfo& _Template, const DesignInfo& _Design);
	static void deleteTemplateDesign(const MetaInfo& _MetaInfo);
};

}
#endif // SYSTEMTEMPLATES_H
