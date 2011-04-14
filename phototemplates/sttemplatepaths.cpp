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
#include "sttemplatepaths.h"
#include <QDir>
#include <QDesktopServices>

#ifdef Q_OS_WIN32
 #include <QApplication> 
#endif 
#ifdef Q_OS_MAC
 #include <QApplication>
#endif

#include "sapplication.h"

QStringList STTemplatePaths::PathList; 


void STTemplatePaths::init()
{
	PathList << initialPaths();
}

void STTemplatePaths::addPath(const QString& _Path)
{
	PathList.push_back(_Path);
}

void STTemplatePaths::addPaths(const QStringList& _Paths)
{
	QStringList::const_iterator it;
	for (it = _Paths.begin(); it != _Paths.end(); ++it)
	{
		addPath(*it);
	}
}


QStringList STTemplatePaths::pathList()
{
	if (PathList.isEmpty())
		init(); 
	return PathList;
}

QString STTemplatePaths::defaultTemplatesPath()
{
	//Note that insinde default there is no data subdir.
	return SApplication::userSharedPath() + "/templates/default";
}

QString STTemplatePaths::defaultTemplateTypePath(STPhotoLayout::EnLayoutType _Type)
{
	return templateTypePath(defaultTemplatesPath(), _Type);
}

QString STTemplatePaths::defaultTemplateResourcesPath(STPhotoLayout::EnResourceSubDirType _DirType)
{
	return defaultTemplatesPath() + "/" + STPhotoLayout::resourceSubDirName(_DirType);
}

QString STTemplatePaths::templateTypePath(const QString& _TemplatesDir, STPhotoLayout::EnLayoutType _Type)
{
	QString Res;
	switch (_Type)
	{
		case STPhotoLayout::TypePhotoBook:
			Res = _TemplatesDir + "/photobooks";
		break;
		case STPhotoLayout::TypeCard:
			Res = _TemplatesDir + "/cards";
		break;
		case STPhotoLayout::TypeCalendar:
			Res = _TemplatesDir + "/calendars";
		break;
		case STPhotoLayout::TypeIdPhoto:
			Res = _TemplatesDir + "/idphoto";
		break;
		case STPhotoLayout::TypeMultiPhoto:
			Res = _TemplatesDir + "/multiphoto";
		break;
	}
	return Res;
}

QString STTemplatePaths::defaultExtraTemplatesPath()
{
	#ifdef Q_OS_WIN32
	//return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/starblitz-templates/";
	return QApplication::applicationDirPath() + "/../../starblitz-templates";
	#else 
	return SApplication::userSharedPath() + "/templates/";
	#endif 
}

QString STTemplatePaths::remoteTemplateRelativePath(const QString& _LocalTemplatePath)
{
#ifdef Q_OS_WIN32
	QString Res = _LocalTemplatePath.toLower();
	Res = Res.remove(SApplication::userSharedPath().toLower());
#else
	QString Res = _LocalTemplatePath;
	Res = Res.remove(SApplication::userSharedPath());
#endif
	return Res;
}

QStringList STTemplatePaths::extraTemplatesPaths()
{
	QDir ExtraTempl(defaultExtraTemplatesPath());
	QStringList ExtraSubDir = ExtraTempl.entryList(QDir::Dirs | QDir::NoDotAndDotDot); 
	
	#ifndef Q_OS_WIN32
	int DefaultPathIndex = ExtraSubDir.indexOf("default");
	if ( DefaultPathIndex >= 0)
		ExtraSubDir.removeAt(DefaultPathIndex);
	#endif 
	QStringList Res; 
	QStringList::iterator it = ExtraSubDir.begin(); 
	while (it != ExtraSubDir.end())
	{
		Res << defaultExtraTemplatesPath() + "/" + *it + "/data" ; 
		it++;
	}
	return Res; 
}

QStringList STTemplatePaths::initialPaths()
{
	QStringList Res; 
	Res <<  defaultTemplatesPath();
	Res << extraTemplatesPaths(); 
	return Res; 
}

/*! Search for path of _TEmplateRelativePath in PathList, if cant found it returns an empty string. */
QString STTemplatePaths::templateAbsolutePath(const QString& _TemplateRelativePath)
{
	QString Res; 

	QStringList PList = pathList(); 
	if (QDir::isRelativePath(_TemplateRelativePath))
	{
		QStringList::iterator it; 
		bool Found = false; 
		it = PList.begin(); 
		while (it != PList.end() && !Found)
		{
			QDir PathDir(*it + "/" + _TemplateRelativePath); 
			if (PathDir.exists())
			{
				Res = PathDir.absolutePath(); 
				Found = true; 
			}
			else 
			if (it->endsWith(_TemplateRelativePath))
			{
				Res = *it;
				Found = true; 
			}
			++it; 
		}
	}
	else 
		Res = _TemplateRelativePath; 
	return Res; 
}
