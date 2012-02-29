/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "exifmetadata.h"
#include <QVariant>
#include <QDebug>

#include "image.hpp"
#include "tags.hpp"


ExifMetadata::ExifMetadata()
{
}

ExifMetadata::~ExifMetadata()
{
}

bool ExifMetadata::load(const QString& _FileName)
{
	bool Res = false;
	try 
	{
		Exiv2::Image::AutoPtr Image = Exiv2::ImageFactory::open(_FileName.toStdString());

		//Exceptions throw dynamic libs causes problems in some compilers.
		//Assert(Image.get() != 0, Error(QString("Could not open image: %1").arg(_FileName)));
		if (Image.get() == 0)
			return false;

		Image->readMetadata();
	
		CExifData = Image->exifData();

		//Assert(!CExifData.empty(), Error(": No Exif data found in the file"));
		if (CExifData.empty())
			return false;
		
//		Exiv2::ExifData::const_iterator i;
//		for (i=CExifData.begin(); i!=CExifData.end(); ++i )
//			qDebug(QString("EXIF Key: %1 = %2").arg( QString::fromStdString(i->key())).arg(QString::fromStdString(i->value().toString())).toLatin1());
		
// 		Exiv2::ExifData::const_iterator i = CExifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
// //		for (i=CExifData.begin(); i!=CExifData.end(); ++i )
// 		if (i != CExifData.end())
// 		{
// 			qDebug(QString("Orientation: %1").arg( i->value().toLong()).toLatin1());
// 			//std::cout << i->key() << " " << std::hex << i->value() << "\n";
// 		}


		//Exiv2::Thumbnail::AutoPtr ThumbnailPtr = CExifData.getThumbnail();
		//Assert(ThumbnailPtr.get() != 0, Error(": No thumbnail found."));
		//Exiv2::DataBuf Buf = ThumbnailPtr->copy(CExifData);
#ifdef EXIV2_VERSIONGT_018_FOUND

		Exiv2::ExifThumbC ExThumbNail(CExifData); 
		Exiv2::DataBuf Buf = ExThumbNail.copy();
		//Assert(Buf.size_ > 0 && Buf.pData_ != 0, Error(": No thumbnail found."));
		if (Buf.size_ > 0 && Buf.pData_ != 0)
			ThumbNail = QImage::fromData( Buf.pData_, Buf.size_, ExThumbNail.mimeType());
#else 
		Exiv2::DataBuf Buf = CExifData.copyThumbnail();
		Assert(Buf.size_ > 0 && Buf.pData_ != 0, Error(": No thumbnail found.")); 
		//ThumbNail = QImage::fromData( Buf.pData_, Buf.size_, ThumbnailPtr->format());
		ThumbNail = QImage::fromData( Buf.pData_, Buf.size_, CExifData.thumbnailFormat());
#endif 
		if (!ThumbNail.isNull())
			ThumbNail = correctOrientation(ThumbNail, getOrientation());
		Res = true;
	}
	catch (Exiv2::AnyError& e) 
	{
		throw Error(QString("Caught Exiv2 exception '" + QString::fromStdString(e.what())));
	}	
	return Res;
}

QImage ExifMetadata::getThumbnail() const
{
	return ThumbNail;
}

QVariant ExifMetadata::ExifValueToVariant(const Exiv2::Value& _Val) const
{
	QVariant Res;
	switch(_Val.typeId())
	{
		case Exiv2::invalidTypeId : 
		break; 
		case Exiv2::unsignedByte :
			Res = static_cast<unsigned char>(_Val.toLong());
		break; 
		case Exiv2::asciiString :
			Res = QString::fromStdString(_Val.toString());
		break; 
		case Exiv2::unsignedShort :
			Res = static_cast<unsigned short>(_Val.toLong());
		break; 
		case Exiv2::unsignedLong :		
			Res = static_cast<qulonglong>(_Val.toLong());
		break; 
		case Exiv2::unsignedRational :
			Res = QString::fromStdString(_Val.toString());
			//TODO: Conversion...
		break; 
// Removed on latest exiv2 versions.
//		case Exiv2::invalid6 :
//		break; 
		case Exiv2::undefined :
		break; 
		case Exiv2::signedShort :
			Res = static_cast<short>(_Val.toLong());
		break; 
		case Exiv2::signedLong :
			Res = static_cast<qlonglong>(_Val.toLong());
		break; 
		case Exiv2::signedRational :
			Res = static_cast<qlonglong>(_Val.toLong());
		break; 
		case Exiv2::string :
			Res = QString::fromStdString(_Val.toString());
		break; 
		case Exiv2::date :
			Res = QString::fromStdString(_Val.toString());
			//TODO: Conversion...
		break; 
		case Exiv2::time :
			Res = QString::fromStdString(_Val.toString());
			//TODO: Conversion...
		break; 
		case Exiv2::comment :
			Res = QString::fromStdString(_Val.toString());
		break; 
		case Exiv2::directory :
			Res = QString::fromStdString(_Val.toString());
		break; 
	}
	return Res;
}

QVariant ExifMetadata::getKeyValue(const QString& _Key) const
{
	QVariant Res;
/*	qDebug(_Key.toLatin1());
 		Exiv2::ExifData::const_iterator i;
 		for (i=CExifData.begin(); i!=CExifData.end(); ++i )
 			qDebug(QString("EXIF Key: %1 = %2").arg( QString::fromStdString(i->key())).arg(QString::fromStdString(i->value().toString())).toLatin1());*/
	Exiv2::ExifData::const_iterator i = CExifData.findKey(Exiv2::ExifKey(_Key.toAscii().data()));
	//i = CExifData.findKey(Exiv2::ExifKey(_Key.toAscii().data()));
	if (i != CExifData.end())
		Res = ExifValueToVariant(i->value());
	return Res;	
}

ExifMetadata::EnOrientation ExifMetadata::getOrientation() const
{
	return static_cast<ExifMetadata::EnOrientation>(getKeyValue("Exif.Image.Orientation").toInt());
}

QSize ExifMetadata::getSize() const
{
	return adjustSizeToOrientation(QSize(getKeyValue("Exif.Photo.PixelXDimension").toInt(), getKeyValue("Exif.Photo.PixelYDimension").toInt()), getOrientation());
}

QSize ExifMetadata::adjustSizeToOrientation(const QSize& _Size, EnOrientation _Orientation) const
{
	QSize Res = _Size;
	if (_Orientation > Orientation_Bottom_Left)
		Res.transpose();
	return Res;
}

QImage ExifMetadata::correctOrientation(const QImage& _Image, EnOrientation _Orientation)
{
	QImage Res = _Image;
	switch (_Orientation)
	{
		case Orientation_Right_Bottom:
		case Orientation_Left_Bottom :
		{
			QTransform Transform;
			Transform.rotate(-90);
			Res = _Image.transformed(Transform, Qt::SmoothTransformation);
		}
		break;

		case Orientation_Left_Top :
		case Orientation_Right_Top :
		{
			QTransform Transform;
			Transform.rotate(90);
			Res = _Image.transformed(Transform, Qt::SmoothTransformation);
		}
		break;

		case Orientation_Bottom_Left :
		case Orientation_Bottom_Right :
		{
			QTransform Transform;
			Transform.rotate(-180);
			Res = _Image.transformed(Transform, Qt::SmoothTransformation);
		}
	}

	switch (_Orientation)
	{
		case Orientation_Bottom_Left :
		case Orientation_Top_Right :
		case Orientation_Left_Top :
		case Orientation_Right_Bottom :
			Res = Res.mirrored(true, false);
		break;
	}
	return Res;
}
