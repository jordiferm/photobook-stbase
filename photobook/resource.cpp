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
#include <QDebug>

#include "stimage.h"

using namespace SPhotoBook;

void Resource::init(const QFileInfo& _FileInfo, EnResourceType _Type )
{
	Name = _FileInfo.baseName().right(_FileInfo.baseName().length() - ( _FileInfo.baseName().indexOf("_") + 1));
	FInfo = _FileInfo;
	Dir = _FileInfo.dir();
	Type = _Type;
}

Resource::Resource(const QFileInfo& _FileInfo)
{
	if (isResource(_FileInfo))
	{
		Type = fileResourceType(_FileInfo.fileName());
		init(_FileInfo, Type);
	}
}

//Resource file with no standard filename
Resource::Resource(const QFileInfo& _FileInfo, EnResourceType _Type )
{
	init(_FileInfo, _Type);
}

Resource::Resource(const QDir& _Dir, const QString& _NameAndSuffix , EnResourceType _Type )
{
	if (!_NameAndSuffix.isEmpty())
	{
		init(QFileInfo(_Dir.absoluteFilePath(QString("%1_%2").arg(filePrefix(_Type)).arg(_NameAndSuffix))), _Type);
	}
	else
		Type = _Type; //Null resource of a specified type.
}


QStringList Resource::save(const QFileInfo& _DestFileInfo)
{
	QFile SourceFile(FInfo.absoluteFilePath());
	if (!QFile::exists(_DestFileInfo.absoluteFilePath()) && SourceFile.exists())
		Assert(SourceFile.copy(_DestFileInfo.absoluteFilePath()),
			   Error(QObject::tr("Error saving resource file: %1 -> %2").arg(SourceFile.fileName()).arg(_DestFileInfo.absoluteFilePath())));

	FInfo = _DestFileInfo; //The mother of the eggs !
	return QStringList() << _DestFileInfo.absoluteFilePath();
}

QStringList Resource::save(const QDir& _StoreDir)
{
	QStringList Res;
	QString HashName = STImage::hashFileName(FInfo.absoluteFilePath());
	QFileInfo DestFileInfo = Resource(_StoreDir, HashName, type()).fileInfo();
	if (type() == TypeFrame) //Save FrameMask with the same hash
	{
		QFileInfo FrameMaskFinfo = frameMaskFile(*this);
		if (FrameMaskFinfo.exists())
		{
			Resource FrameMaskResource(FrameMaskFinfo);
			Res << FrameMaskResource.save(Resource(_StoreDir, HashName, TypeFrameMask).fileInfo());
		}
	}

	Res << save(DestFileInfo);
	return Res;
}

QFileInfo Resource::fileInfo() const
{
	return FInfo;
}

void Resource::calcHashCode()
{
	HashCode = getHashCode();
}

QString Resource::getHashCode() const
{
	return STImage::hashFileName(FInfo.absoluteFilePath());
}

QString Resource::hashCode() const
{
	QString Res;
	if (HashCode.isEmpty())
		Res = getHashCode();
	else
		Res = HashCode;
	return Res;
}

bool Resource::operator==(const Resource& _Other ) const
{
	return hashCode() == _Other.hashCode();//You have to previously call to calcHashCode to improve speed.
}

bool Resource::operator!=(const Resource& _Other ) const
{
	return !operator==(_Other);
}

QStringList Resource::fileFilter(EnResourceType _Type)
{
	QStringList Filter;
	Filter << QString("%1_*").arg(filePrefix(_Type));
	return Filter;
}

QString Resource::suffixFilter(EnResourceType _Type)
{
	QString Res;
	switch (_Type)
	{
		case TypeBackground :
			Res = QObject::tr("Images (*.png *.xpm *.jpg)");
		break;
		case TypeClipart :
			Res = QObject::tr("Clipart (*.svg)");
		break;
		case TypeFrame :
			Res = QObject::tr("Frames (*.png)");
		break;
		case TypeMask :
			Res = QObject::tr("Masks (*.png)");
		break;
		case TypeFrameMask :
			Res = QObject::tr("Frame Mask (framemask_*.png)");
		break;
		case TypeFont :
			Res = QObject::tr("True Type Font (*.ttf)");
		break;
		case TypeImage :
			Res = QObject::tr("Images (*.png *.xpm *.jpg)");
		break;
	}
	return Res;
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
		case TypeMask :
			Res = "mask";
		break;
		case TypeFrameMask :
			Res = "framemask";
		break;
		case TypeFont :
			Res = "fnt";
		break;
		case TypeImage :
			Res = "img";
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
	if (_FileName.startsWith("framemask"))
		Res = TypeFrameMask;
	else
	if (_FileName.startsWith("frame"))
		Res = TypeFrame;
	else
	if (_FileName.startsWith("fnt"))
		Res = TypeFont;
	else
	if (_FileName.startsWith("mask"))
		Res = TypeMask;
	else
	if (_FileName.startsWith("img"))
		Res = TypeImage;
	return Res;
}


bool Resource::isResource(const QFileInfo& _FileInfo)
{
	QString FileName = _FileInfo.fileName();

	int Cnt = TypeBackground;
	bool Found = false;
	while (!Found && Cnt <= TypeImage)
	{
		Found = FileName.startsWith(filePrefix(static_cast<EnResourceType>(Cnt)) + "_");
		Found = Found && _FileInfo.suffix().toLower() != "thumb";//fileExtension(static_cast<EnResourceType>(Cnt));
		Cnt++;
	}
	return Found;
}

QFileInfo Resource::frameMaskFile(const Resource& _FrameResource)
{
	return Resource(_FrameResource.fileInfo().dir(), _FrameResource.name() + "." + _FrameResource.fileInfo().suffix(), TypeFrameMask ).fileInfo();
}

Resource Resource::resourceFromXmlSrc(const QString& _XmlSrc, const QString& _LoadDir)
{
	Resource Res;
	if (!_XmlSrc.isEmpty())
		Res =  Resource(fileInfoFromXmlSrc(_XmlSrc, _LoadDir));
	return Res;
}


QFileInfo Resource::fileInfoFromXmlSrc(const QString& _XmlSrc, const QString& _LoadDir)
{
	QFileInfo Res;
	QFileInfo XmlFInfo(_XmlSrc);
	if (_LoadDir.isEmpty())
		Res = XmlFInfo;
	else
		Res = QFileInfo(QDir(_LoadDir).absoluteFilePath(_XmlSrc));

	return Res;
}

QString Resource::fileInfoToXmlSrc(const QFileInfo& _FileInfo, const QString& _SaveDir)
{
	QString Res;
	if (_SaveDir.isEmpty())
		Res = _FileInfo.absoluteFilePath();
	else
		Res = _FileInfo.fileName();
		//Res = QDir(_SaveDir).absoluteFilePath(_FileInfo.fileName());


	return Res;
}
