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

#include "dimagedoc.h"

#include <QPixmapCache>
#include <QDir>
#include <QDebug>

#include "imageTools.h"
#include "stimage.h"

using namespace STDom;

QImage DImageDoc::loadThumbnailImage(bool _CreateIfNotExist)
{
	QImage Res;
	//If we have exif, use exif thumbnail for thumbnail.
	QFileInfo FInfo(FilePath);
	ExifMetadata ExMData;
	QImage TmpThumb;
	bool ThumbnailReaded = false;
	try
	{
		if (ExMData.load(FInfo.absoluteFilePath()))
		{
			TmpThumb = ExMData.getThumbnail();
			ThumbnailReaded = !TmpThumb.isNull();
			CInfo MInfo = getInfo(ExMData);

			if (ThumbnailReaded)
				Res = TmpThumb.scaled(ThumbNailSize, Qt::KeepAspectRatio);
		}
	}
	catch (...)
		{}
	if (!ThumbnailReaded) //Else we try to load .thumbnail file
	{
		QFileInfo ThumbFInfo = thumbNailFileName(FInfo);
		if (!ThumbFInfo.exists() && _CreateIfNotExist)
		{
			if (!createThumbnail(false))
				return QImage(FInfo.absoluteFilePath()).scaled(ThumbNailSize, Qt::KeepAspectRatio);
		}
		Res = QImage(ThumbFInfo.absoluteFilePath()).scaled(ThumbNailSize, Qt::KeepAspectRatio);
	}
	return Res;
}


DImageDoc::DImageDoc(const QString& _FilePath, qint64 _Key, qint64 _ParentKey) : DDoc(_FilePath, _Key, _ParentKey)
{
}

DImageDoc::DImageDoc(const QFileInfo& _File, qint64 _Key, qint64 _ParentKey) : DDoc(_File, _Key, _ParentKey)
{}

void DImageDoc::createBigThumbnail() const
{
	QFileInfo FInfo(FilePath);
	QImage TmpThumb;
	if (!scaleImage(FInfo.absoluteFilePath(), bigThumbNailFileName(FilePath).absoluteFilePath(), BigThumbNailSize.width(), BigThumbNailSize.height()), BigThumbnailQuality)
		qWarning(QObject::tr("Could not save big thumbnail for image ").arg(FilePath).toLatin1());
}

QImage DImageDoc::loadBigThumbnailImage(bool _CreateIfNotExist) const
{
	QImage Res;
	QFileInfo FInfo(FilePath);
	QImage TmpThumb;
	QFileInfo ThumbFInfo = bigThumbNailFileName(FInfo);
	if (!ThumbFInfo.exists())
	{
		if (_CreateIfNotExist)
			createBigThumbnail();
		else
			ThumbFInfo = FInfo;
	}

	Res = QImage(ThumbFInfo.absoluteFilePath()).scaled(BigThumbNailSize, Qt::KeepAspectRatio);
	return Res;
}

void DImageDoc::removeBigThumbnail()
{
	QFileInfo FInfo = bigThumbNailFileName(FilePath);
	if (FInfo.exists())
		QFile::remove(FInfo.absoluteFilePath());
}

bool DImageDoc::hasBigThumbnail() const
{
	QFileInfo FInfo = bigThumbNailFileName(FilePath);
	return FInfo.exists();
}

bool DImageDoc::createThumbnail(bool _CheckForMetadata) const
{
	bool Res = false;
	QFileInfo FInfo(FilePath);
	QImage TmpThumb;
	bool ThumbnailReaded = false;
	if (_CheckForMetadata)
	{
		ExifMetadata ExMData;
		try
		{

			if (ExMData.load(FInfo.absoluteFilePath()))
			{
				TmpThumb = ExMData.getThumbnail();
				ThumbnailReaded = !TmpThumb.isNull();
				Res = true;
			}
		}
		catch (...)
			{}
	}
	if (!ThumbnailReaded) //Let's generate the thumbnail.
	{
		if (QFileInfo(thumbNailFileName(FilePath).dir().absolutePath()).isWritable())
			Res = scaleImage(FInfo.absoluteFilePath(), thumbNailFileName(FilePath).absoluteFilePath(), ThumbNailSize.width(), ThumbNailSize.height());
	}
	return Res;
}

DImageDoc::CInfo DImageDoc::getInfo(const ExifMetadata& _ExMData) const
{
	CInfo Res;
	Res.setOrientation(_ExMData.getOrientation());
	Res.setSize(_ExMData.getSize());
	return Res;
}

DImageDoc::CInfo DImageDoc::getInfo() const
{
	ExifMetadata ExMData;
	DImageDoc::CInfo Info;
	try
	{
		QFileInfo FInfo(FilePath);
		if (ExMData.load(FInfo.absoluteFilePath()))
			Info = getInfo(ExMData);
	}
	catch (...)
		{}
	return Info;
}

void DImageDoc::save(const QImage& _Image, bool _SaveOriginal)
{
	if (_SaveOriginal)
		Assert(originalImage().save(originalImageFileName()), Error(QString(QObject::tr("Error saving image %1")).arg(originalImageFileName())));

	QFileInfo FInfo(FilePath);
	Assert(_Image.save(FInfo.absoluteFilePath()), Error(QString(QObject::tr("Error saving image %1")).arg(FInfo.absoluteFilePath())));
	updateThumbnail();
	removeBigThumbnail();
}

QImage DImageDoc::originalImage() const
{
	QImage Res(originalImageFileName());
	if (Res.isNull())
		Res.load(FilePath);
	return Res;
}

bool DImageDoc::originalSaved() const
{
	return QFile::exists(originalImageFileName());
}

void DImageDoc::removeOriginal()
{
	QFile OrigFile(originalImageFileName());
	if (OrigFile.exists())
		Assert(OrigFile.remove(), Error(QString(QObject::tr("Could not remove original file %1")).arg(OrigFile.fileName())));
}

bool DImageDoc::remove()
{
	bool Res = true;
	if (originalSaved())
		removeOriginal();
	removeBigThumbnail();
	removeThumbnail();
	QFile ImageFile(FilePath);
	if (ImageFile.exists())
		Res = ImageFile.remove();
	return Res;
}

QString DImageDoc::mkOriginalImagesPath() const
{
	QDir ColDir(QDir::home().absoluteFilePath(".originals"));
	if (!ColDir.exists())
		Assert(ColDir.mkpath(ColDir.absolutePath()), Error(QString(QObject::tr("Error creating directory %1")).arg(ColDir.absolutePath())));
	return ColDir.absolutePath();
}

QString DImageDoc::originalImageFileName() const
{
	QString Res;
	if (Key != -1)
		Res = mkOriginalImagesPath() + "/" + QString::number(Key) + ".png";
	else
		Res = mkOriginalImagesPath() + "/" + pathHash() + ".png";
	return Res;
}

bool DImageDoc::supportedFileType(const QFileInfo& _File)
{
	return STImage::isSupportedImage(_File);
}
