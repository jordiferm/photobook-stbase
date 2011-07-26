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

#include "resource.h"
#include <QStringList>
#include <QFileInfo>

using namespace SPhotoBook;


Resource::Resource(const QFileInfo& _FileInfo)
{
	if (isResource(_FileInfo))
	{
		QString FileName = _FileInfo.baseName();
		Name = FileName.right(FileName.length() - ( FileName.indexOf("_") + 1));
		Type = fileResourceType(FileName);
		Dir = _FileInfo.dir();
		FInfo = _FileInfo;
	}
}


Resource::Resource(const QDir& _Dir, const QString& _Name , EnResourceType _Type ) : Name(_Name), Type(_Type), Dir(_Dir)
{
	FInfo = QFileInfo(Dir.absoluteFilePath(QString("%1_%2.%3").arg(filePrefix(Type)).arg(Name).arg(fileExtension(Type))));;
}

QFileInfo Resource::fileInfo() const
{
	return FInfo;
}

QStringList Resource::fileFilter(EnResourceType _Type)
{
	QStringList Filter;
	Filter << QString("*.%1").arg(fileExtension(_Type).toLower()) << QString("*.%1").arg(fileExtension(_Type).toUpper());
	return Filter;
}

QString Resource::filePrefix(EnResourceType _Type)
{
	QString Res;
	switch (_Type)
	{
		case TypeBackground :
			Res = "bg";
		break;
		case TypeClipart :
			Res = "clipart";
		break;
		case TypeFrame :
			Res = "frame";
		break;
		case TypeFrameMask :
			Res = "mask";
		break;
		case TypeFont :
			Res = "fnt";
		break;
	}
	return Res;
}

Resource::EnResourceType Resource::fileResourceType(const QString& _FileName)
{
	Resource::EnResourceType Res;

	if (_FileName.startsWith("bg"))
		Res = TypeBackground;
	else
	if (_FileName.startsWith("clipart"))
		Res = TypeClipart;
	else
	if (_FileName.startsWith("frame"))
		Res = TypeFrame;
	else
	if (_FileName.startsWith("mask"))
		Res = TypeFrameMask;
	else
	if (_FileName.startsWith("fnt"))
		Res = TypeFont;
	return Res;
}

QString Resource::fileExtension(EnResourceType _Type)
{
	QString Res;
	switch (_Type)
	{
		case TypeBackground :
			Res = "JPG";
		break;
		case TypeClipart :
			Res = "SVG";
		break;
		case TypeFrame :
			Res = "PNG";
		break;
		case TypeFrameMask :
			Res = "PNG";
		break;
		case TypeFont :
			Res = "TTF";
		break;
	}
	return Res;
}

bool Resource::isResource(const QFileInfo& _FileInfo)
{
	QString FileName = _FileInfo.fileName();

	int Cnt = TypeBackground;
	bool Found = false;
	while (!Found && Cnt <= TypeFont)
	{
		Found = FileName.startsWith(filePrefix(static_cast<EnResourceType>(Cnt)) + "_");
		Found = Found && _FileInfo.suffix().toUpper() == fileExtension(static_cast<EnResourceType>(Cnt));
		Cnt++;
	}
	return Found;
}
