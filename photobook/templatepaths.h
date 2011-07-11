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
#ifndef TemplatePaths_H
#define TemplatePaths_H
#include <QStringList>
#include "stphotobookexport.h"

#include "designinfo.h"

namespace STPhotoBook
{

/**
Class to store and search template paths.


	@author
*/
class ST_PHOTOBOOK_EXPORT TemplatePaths
{
	static QStringList PathList; 

public:
	static void init();
	static void addPath(const QString& _Path); 	
	static void addPaths(const QStringList& _Paths);
	static QString defaultTemplatesPath(); 
	static QString defaultTemplateResourcesPath(DesignInfo::EnResourceType _DirType);
	static QStringList pathList();
	static QString defaultExtraTemplatesPath();
	static QString remoteTemplateRelativePath(const QString& _LocalTemplatePath);
	static QStringList extraTemplatesPaths();
	static QStringList initialPaths();
	static QString templateAbsolutePath(const QString& _TemplateRelativePath);
};

}

#endif
