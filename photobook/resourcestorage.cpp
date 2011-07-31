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

#include "resourcestorage.h"
#include "resource.h"
#include "stimage.h"

using namespace SPhotoBook;

ResourceStorage::ResourceStorage() : Saved(false)
{
}

QStringList ResourceStorage::save(const QFileInfo& _SourceImage, const QImage& _DestImage)
{
	QString DestFileName = SaveDir.absoluteFilePath(_SourceImage.fileInfo().fileName());
	//qDebug(QString("Saving Mask %1").arg(MaskImageFileName).toLatin1());
	//Res.push_back(MaskImageFileName);
	if (!QFile::exists(DestFileName))
		Assert(_DestImage.save(DestFileName),
			Error(QString(tr("Error storing mask image '%1'")).arg(DestFileName)));

	Saved = true;

	return QStringList() << ResFileName;

}

//For already loaded resources
QStringList ResourceStorage::save(const Resource& _Resource, const QImage& _Image)
{
	QStringList Res;
	STImage SMImage = _Image;
	Resource DestResource(_Destdir, SMImage.hashString(), _Resource.type());

	Res << save(Resource);

	return

}

QStringList ResourceStorage::save(const QFileInfo& _SourceImage)

QStringList ResourceStorage::save(const Resource& _Resource)
{
	//Hash filename
	//With frames save his mask with frameMaskName but use mask hash
	//Only save if dest file not exist.

}

QDir ResourceStorage::resourcesDir() const
{
	QDir Res;
	if (Saved)
		Res = SaveDir;
	else
		Res = LoadDir;
	return Res;
}

//createElement calls this => We need to call save before createElement.
QString ResourceStorage::xmlSourceName(const QFileInfo& _FileInfo) const
{
	QString Res;
	if (Saved)
		Res = _FileInfo.fileName();
	else
		Res = _FileInfo.absoluteFilePath();
	return Res;
}

//loadElement calls this
QFileInfo ResourceStorage::fileInfo(const QString& _XmlSourceName) const
{
	QFileInfo Res;
	QFileInfo SourceFileInfo(_XmlSourceName);
	if (SourceFileInfo.isAbsolute())
		Res = SourceFileInfo;
	else
	{
		if (Saved)
			Res.setFile(SaveDir.absoluteFilePath(_XmlSourceName));
		else
			Res.setFile(LoadDir.absoluteFilePath(_XmlSourceName));
	}
	return Res;
}
