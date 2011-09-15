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

#include "ddoc.h"
#include <QPixmapCache>
#include <QCryptographicHash>

using namespace STDom;

// _________________________________________________________________________*/
//
// CLASS OPhotoCollectionImage
// _________________________________________________________________________*/

QSize DDoc::ThumbNailSize = QSize(128, 128);
QSize DDoc::BigThumbNailSize = QSize(300, 200);
int DDoc::BigThumbnailQuality = 70;
QImage DDoc::NoThumbnailImage;

DDoc::DDoc(const QString& _FilePath, qint64 _Key, qint64 _ParentKey)
		: Key(_Key), FilePath(_FilePath), ParentKey(_ParentKey)
{}

DDoc::DDoc(const QFileInfo& _File, qint64 _Key, qint64 _ParentKey)
		: Key(_Key), FilePath(_File.absoluteFilePath()), ParentKey(_ParentKey)
{}



//! returns a big thumbnail image, if it does not exist on disk it creates it and stores into cache.
QPixmap DDoc::bigThumbnail(bool _CreateIfNotExist)
{
	QPixmap Res;

	QFileInfo FInfo(FilePath);
	//See if it's allready in cache
	if (!QPixmapCache::find(FInfo.absoluteFilePath(), Res)) //If not we loads it.
	{
		QImage ThumbImage = loadBigThumbnailImage(_CreateIfNotExist);
		Res = QPixmap::fromImage(ThumbImage);
		cacheThumbnail(ThumbImage);
	}
	return Res;
}

void DDoc::updateThumbnail()
{
	QFileInfo FInfo(FilePath);
	QPixmapCache::remove(FInfo.absoluteFilePath());
	CachedThumbnailImage = QImage();
	createThumbnail();
}

QImage DDoc::loadThumbnail(bool _CreateIfNotExist)
{
	if (CachedThumbnailImage.isNull())
		CachedThumbnailImage = loadThumbnailImage(_CreateIfNotExist);

	return CachedThumbnailImage;
}

void DDoc::removeThumbnail()
{
	QFileInfo FInfo = thumbNailFileName(FilePath);
	if (FInfo.exists())
		QFile::remove(FInfo.absoluteFilePath());
}

bool DDoc::thumbnailLoaded() const
{
	QPixmap TmpPixmap;
	QFileInfo FInfo(FilePath);
	return QPixmapCache::find(FInfo.absoluteFilePath(), TmpPixmap) || !CachedThumbnailImage.isNull();
}

void DDoc::cacheThumbnail(const QImage& _Image)
{
	QFileInfo FInfo(FilePath);
	QPixmapCache::insert(FInfo.absoluteFilePath(), QPixmap::fromImage(_Image));
	CachedThumbnailImage = QImage();
}

QPixmap DDoc::thumbnail()
{
	QPixmap Res;

	QFileInfo FInfo(FilePath);
	//See if it's allready in cache
	if (!QPixmapCache::find(FInfo.absoluteFilePath(), Res))
	{
		Res = QPixmap::fromImage(loadThumbnail());
	}
	return Res;
}

QFileInfo DDoc::thumbNailFileName(const QFileInfo& _File) const
{
	return QFileInfo(_File.absolutePath() + "/." + _File.fileName() + ".thumb");
}

bool DDoc::operator==(const DDoc& _Other) const
{
	return fileInfo() == _Other.fileInfo();
}

bool DDoc::remove()
{
	bool Res = true;
	removeBigThumbnail();
	removeThumbnail();
	QFile ImageFile(FilePath);
	if (ImageFile.exists())
		Res = ImageFile.remove();
	return Res;
}

QString DDoc::pathHash() const
{
	QCryptographicHash CryptoHash(QCryptographicHash::Md5);
	CryptoHash.addData(FilePath.toAscii(), FilePath.size());
	QByteArray HashRes = CryptoHash.result();
	return QString(HashRes.toHex());
}


// _________________________________________________________________________*/
//
// CLASS DDocFormat
// _________________________________________________________________________*/

DDocFormat::DDocFormat(double _Width, double _Height, const QString& _Key)
	: Width(_Width), Height(_Height), Key(_Key)
{}

DDocFormat::DDocFormat(const QSizeF& _Size, const QString& _Key)
	: Width(_Size.width()), Height(_Size.height()), Key(_Key)
{}

DDocFormat::DDocFormat(const QString& _Format, const QString& _Key)
{
	Width = _Format.left(_Format.indexOf("x")).toInt();
	Height = _Format.right(_Format.length() - (_Format.indexOf("x") + 1)).toInt();
	Key = _Key;
}

double DDocFormat::width() const
{
	return Width;
}

double DDocFormat::height() const
{
	return Height;
}

double DDocFormat::widthInch() const
{
	return mmToInch(Width);
}

void DDocFormat::setWidth(double _Width)
{
	Width = _Width;
}

double DDocFormat::heightInch() const
{
	return mmToInch(Height);
}

void DDocFormat::setHeight(double _Height)
{
	Height = _Height;
}

double DDocFormat::bigSide() const
{
	return qMax(width(), height());
}

double DDocFormat::smallSide() const
{
	return qMin(width(), height());
}

bool DDocFormat::isNull() const
{
	return (width() <= 0 && height() <= 0);
}


bool DDocFormat::operator==(const DDocFormat& _Format) const
{
	bool Res;
	if (!Key.isEmpty())
		Res = _Format.key() == key();
	else
		Res = (_Format.Width == Width && _Format.Height == Height);
	return Res;
}

bool DDocFormat::operator<(const DDocFormat& _Format) const
{
	return _Format.toString() < toString(); //return ((_Format.Width * _Format.Height > Width * Height) && _Format.Width > Width );
}

bool DDocFormat::operator>(const DDocFormat& _Format) const
{
	return !operator<(_Format);
}

/*!
	\return Width and Height DPI depending on _PixelSize;
*/
QSize DDocFormat::dpis(const QSize& _PixelSize)
{
	QSize Res(static_cast<int>(_PixelSize.width() / widthInch()), static_cast<int>(_PixelSize.height() / heightInch()));
	return Res;
}

QSize DDocFormat::pixelSize(int _Dpis) const
{
	return QSize(static_cast<int>(_Dpis * widthInch()), static_cast<int>(_Dpis * heightInch()));
}

QSize DDocFormat::pixelSize(const QSizeF& _Size, int _Dpi)
{
	// Convertim els mm a Inches.
	float InchWidth = _Size.width() * 0.03937;
	float InchHeight = _Size.height() * 0.03937;
	QSize Res(static_cast<int>(InchWidth * _Dpi), static_cast<int>(InchHeight * _Dpi));
	return Res;
}


/*!
	\return Set fisical size from PixelSize and _Dpis;
*/
void DDocFormat::setPixelSize(const QSize _PixelSize, int _Dpis)
{
	setWidth(static_cast<double>(_PixelSize.width()) / _Dpis / 0.03937);
	setHeight(static_cast<double>(_PixelSize.height()) / _Dpis / 0.03937);
}

QString DDocFormat::toString() const
{
	QString Res;
	if (Description.isEmpty())
		Res = QString::number(Width) + "x" + QString::number(Height);
	else
		Res = Description;
	return Res;
}

double DDocFormat::mmToInch(double _Mm)
{
	return _Mm * 0.03937;
}

int DDocFormat::mmToPixels(double _Mm, int _Dpis)
{
	return mmToInch(_Mm) * _Dpis;
}

void DDocFormat::copyOrientation(const DDocFormat& _Format)
{
	if ((_Format.width() > _Format.height() && width() < height()) ||
		(_Format.width() < _Format.height() && width() > height()))
		transpose();
}

/*!
  Swaps the width and the height values.
 */
void DDocFormat::transpose()
{
	double OldWidth = Width;
	Width = Height;
	Height = OldWidth;
}

// _________________________________________________________________________*/
//
// CLASS DDocFormatList
// _________________________________________________________________________*/

DDocFormat DDocFormatList::nearestRatioFormat(const DDocFormat& _Reference) const
{
	DDocFormat Res;
	double RefAspectRatio = _Reference.bigSide() / _Reference.smallSide();
	DDocFormatList::const_iterator it = begin();
	double LastDiff = -1;
	while (it != end())
	{
		double CurrAspectRatio = it->bigSide() / it->smallSide();
		double Diff = qMax(RefAspectRatio, CurrAspectRatio) - qMin(RefAspectRatio, CurrAspectRatio);
		if (LastDiff == -1 || Diff < LastDiff)
		{
			LastDiff = Diff;
			Res = *it;
		}
		++it;
	}
	return Res;
}

DDocFormat DDocFormatList::farestRatioFormat(const DDocFormat& _Reference) const
{
	DDocFormat Res;
	double RefAspectRatio = _Reference.bigSide() / _Reference.smallSide();
	DDocFormatList::const_iterator it = begin();
	double LastDiff = 0;
	while (it != end())
	{
		double CurrAspectRatio = it->bigSide() / it->smallSide();
		double Diff = qMax(RefAspectRatio, CurrAspectRatio) - qMin(RefAspectRatio, CurrAspectRatio);
		if (Diff > LastDiff)
		{
			LastDiff = Diff;
			Res = *it;
		}
		++it;
	}
	return Res;

}


DDocFormat DDocFormatList::biggestFormat() const
{
	DDocFormat Res;
	DDocFormatList::const_iterator it = begin();
	double LastArea = 0;
	while (it != end())
	{
		double CurrArea = it->width() * it->height();
		if (CurrArea > LastArea)
		{
			LastArea = CurrArea;
			Res = *it;
		}
		++it;
	}
	return Res;
}

DDocFormat DDocFormatList::smallestFormat() const
{
	DDocFormat Res;
	DDocFormatList::const_iterator it = begin();
	double LastArea = -1;
	while (it != end())
	{
		double CurrArea = it->width() * it->height();
		if (CurrArea < LastArea || LastArea == -1)
		{
			LastArea = CurrArea;
			Res = *it;
		}
		++it;
	}
	return Res;
}

// _________________________________________________________________________*/
//
// CLASS DDocFormatList
// _________________________________________________________________________*/

int DDocPrintList::numCopies() const
{
	int Res = 0;
	DDocPrintList::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		Res += it->numCopies();
	}
	return Res;
}
