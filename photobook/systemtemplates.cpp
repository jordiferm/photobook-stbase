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
#include "systemtemplates.h"
#include "templatepaths.h"
#include <QDir>

using namespace SPhotoBook;

SPhotoBook::TemplateInfoList SystemTemplates::load()
{
	SPhotoBook::TemplateInfoList Res;
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
				Res.addDesignNames(BaseDir, NameDir.dirName(), QDir(*snit).dirName());
				++snit;
			}
			++tnit;
		}
		++pit;
	}
	return Res;
}

void SystemTemplates::import(const QDir& _SourceDir)
{
	//Todo
}

void SystemTemplates::exportTemplates(const QDir& _DestDir, const TemplateInfo& _Template, const DesignInfo& _Design)
{
	//Todo
}
