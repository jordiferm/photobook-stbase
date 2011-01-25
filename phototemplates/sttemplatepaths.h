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
#ifndef STTEMPLATEPATHS_H
#define STTEMPLATEPATHS_H
#include <QStringList>
#include "stphototemplatesexport.h"
#include "stphotolayout.h"

/**
Class to store and search template paths.


	@author
*/
class ST_PHOTOTEMPLATES_EXPORT STTemplatePaths
{
	static QStringList PathList; 

public:
	static void init();
	static void addPath(const QString& _Path); 	
	static void addPaths(const QStringList& _Paths);
	static QString defaultTemplatesPath(); 
	static QString defaultTemplateTypePath(STPhotoLayout::EnLayoutType);
	static QString defaultTemplateResourcesPath(STPhotoLayout::EnResourceSubDirType _DirType);
	static QStringList pathList();
	static QString templateTypePath(const QString& _TemplatesDir, STPhotoLayout::EnLayoutType);
	static QString defaultExtraTemplatesPath();
	static QString remoteTemplateRelativePath(const QString& _LocalTemplatePath);
	static QStringList extraTemplatesPaths();
	static QStringList initialPaths();
	static QString templateAbsolutePath(const QString& _TemplateRelativePath);

	//void storePaths(); //Stores paths to settings.
	//void loadPaths(); //Load paths from settings.
	//void findTemplates();  //Find Paths from user disk.
};

#endif
