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
#include "stimage.h"
#include <QImageReader>
#include <QImageWriter>
#include <QFile> 
#include <QCryptographicHash>
#include <QFileInfo> 
#include <QDebug>

#include <QPainter> //Print method.
#include <QPrinter> 

#include "crop.h"
#include "tilt.h"
#include "redEye.h"
#include "imageTools.h"
#include "ilcolor.h"
#include "contrast.h"
#include "imimage.h"
#include "blackWhite.h"
#include "sepia.h"
#include "manipulationOptions.h"
#include "exifmetadata.h"

//cipher
#include <QBuffer>

//#define USE_PRINTKEEPER

#ifdef USE_PRINTKEEPER
	#include "printkeeper.h"
#endif

QString STImage::DefaultEncryptionKey = "stimagekey";

QString STImage::formatForFileFilter(const QList<QByteArray>& _Formats)
{
	QString Res;
	QList<QByteArray>::const_iterator it;
	for (it = _Formats.begin(); it != _Formats.end(); ++it)
	{
		if (!Res.isEmpty())
			Res += " ";
		Res += "*." + (*it).toLower() + " *." + (*it).toUpper();
	}
	return Res;
}

STImage::STImage()
		: QImage()
{}

STImage::STImage(const QSize& _Size, QImage::Format _Format) : QImage(_Size, _Format)
{}

STImage::STImage(const QImage& _Image) : QImage(_Image)
{}

STImage::STImage(const QString& _FileName, const char* _Format)
{
	load(_FileName, _Format); 
}

STImage::~STImage()
{}

bool STImage::load(const QString& _FileName, const char* _Format)
{
	QString FileName = _FileName;
	//If image does not exist tries to load encoded version.
	if (!QFile::exists(FileName))
	{
		FileName = encodedFileName(FileName);
		if (!QFile::exists(FileName))
			return false;
	}

	bool Res = false;
	if (isEncoded(FileName))
		Res = loadEncoded(FileName);
	else
		Res = QImage::load(FileName, _Format);

	if (Res) //A little trick to solve an strange issue with 8 bit images.
	{
		if (format() == QImage::Format_Indexed8 || format() == QImage::Format_MonoLSB || format() == QImage::Format_Mono)
			(*this) = convertToFormat(QImage::Format_RGB32); 
	}
	return Res; 
}

void STImage::loadThumbnail(const QString& _FilePath, QSize _MinThumbSize )
{
	QFileInfo FInfo(_FilePath);
	ExifMetadata ExMData;
	QImage TmpThumb; 
	bool ThumbnailReaded = false;
	try
	{
		if(ExMData.load(FInfo.absoluteFilePath()))
		{
			TmpThumb = ExMData.getThumbnail();
			ThumbnailReaded = !TmpThumb.isNull();
		}
	}
	catch(...)
	{}
	if (!ThumbnailReaded)
		TmpThumb = QImage(FInfo.absoluteFilePath()).scaled(_MinThumbSize, Qt::KeepAspectRatioByExpanding); 
	
	*this = TmpThumb; 
}


STImage STImage::crop(const QPoint& _TopLeft, const QPoint& _BottomRight)
{
	QImage Res;
	QImage* ModImage = ::cropImage(*this, _TopLeft, _BottomRight);
	if (ModImage)
	{
		Res = *ModImage;
		delete ModImage;
	}
	return Res;
}

STImage STImage::correctTilt(const QPoint& _P1, const QPoint& _P2)
{
	QImage Res;
	QImage* ModImage = correctImageTilt(*this, _P1, _P2);
	if (ModImage)
	{
		Res = *ModImage;
		delete ModImage;
	}
	return Res;
}

STImage STImage::transform(EnTransformCodes _TransformCode)
{
	QImage Res;
	switch (_TransformCode)
	{
		case Rotate90 :
			::transformImage(*this, Res, ROTATE_90);
			break;
		case Rotate270 :
			::transformImage(*this, Res, ROTATE_270);
			break;
		case FlipX :
			::transformImage(*this, Res, FLIP_V);
			break;
		case FlipY :
			::transformImage(*this, Res, FLIP_H);
			break;
	}
	return Res;
}

STImage STImage::correctRedEye(QPoint _TopLeft, QPoint _BottomRight,
                               SProcessStatusWidget* _SProcessStatusWidget)
{
	QImage Res;
	RedEye cRedEye;
	QImage* ModImage = cRedEye.removeRedeyeRegions(this, _TopLeft, _BottomRight, _SProcessStatusWidget);
	if (ModImage)
	{
		Res = *ModImage;
		delete ModImage;
	}
	return Res;
}

STImage STImage::improveColorBalance(SProcessStatusWidget* _SProcessStatusWidget)
{
	QImage Res;
	QImage* ModImage = ::improveColorBalance(this, _SProcessStatusWidget);
	if (ModImage)
	{
		Res = *ModImage;
		if (ModImage != this)
			delete ModImage;
	}
	return Res;
}

STImage STImage::enhanceImageContrast(SProcessStatusWidget* _SProcessStatusWidget)
{
	::enhanceImageContrast(this, _SProcessStatusWidget);
	return *this;
}

STImage STImage::contrast(const unsigned int _Sharpen)
{
	IMImage IImage(*this);
	for (int Vfor = 0; Vfor < _Sharpen; Vfor++)
		IImage.contrast(1);
	return IImage.image();
}




STImage STImage::sigmoidalContrast(const unsigned int _Sharpen, const double _Contrast, const double _MidPoint)
{
	IMImage IImage(*this);
	IImage.sigmoidalContrast(_Sharpen, _Contrast, _MidPoint);
	return IImage.image();
}

STImage STImage::despeckle()
{
	IMImage IImage(*this);
	IImage.despeckle();
	return IImage.image();
}

STImage STImage::enhance()
{
	IMImage IImage(*this);
	IImage.enhance();
	return IImage.image();
}

STImage STImage::normalize()
{
	IMImage IImage(*this);
	IImage.normalize();
	return IImage.image();
}

STImage STImage::equalize()
{
	IMImage IImage(*this);
	IImage.equalize();
	return IImage.image();
}

STImage STImage::sharpen(const double _Radius, const double _Sigma)
{
	IMImage IImage(*this);
	IImage.sharpen(_Radius, _Sigma);
	return IImage.image();
}

STImage STImage::gamma(const double _Gamma)
{
	IMImage IImage(*this);
	IImage.gamma(_Gamma);
	return IImage.image();
}

STImage STImage::gammaRGB(const double _Red, const double _Green, const double _Blue)
{
	IMImage IImage(*this);
	IImage.gamma(_Red, _Green, _Blue);
	return IImage.image();
}

STImage STImage::blackAndWhite(SProcessStatusWidget* _SProcessStatusWidget)
{
	ManipulationOptions Options(_SProcessStatusWidget);
	::blackWhiteEffect(*this, &Options);
	return *this;
}


STImage STImage::sepia(SProcessStatusWidget* _SProcessStatusWidget)
{
	ManipulationOptions Options(_SProcessStatusWidget);
	QImage Res;
	QImage* ModImage = ::sepiaEffect(this, &Options);
	if (ModImage)
	{
		Res = *ModImage;
		if (ModImage != this)
			delete ModImage;
	}
	return Res;
}

/*!
	Creates a new STImage of size _DestSize, with current image inside and _MarginColor Margin.
*/

STImage STImage::applyMargin(const QSize& _DestSize, const QColor& _MarginColor, STImage::EnAlignment _Align)
{
	QSize TmpSize = _DestSize; 
	//Align image size with the same orientation that image.
	if ((size().width() > size().height() && TmpSize.width() <= TmpSize.height()) ||
		(size().height() > size().width() && TmpSize.height() <= TmpSize.width()))
		TmpSize.transpose(); 
	
	STImage ResImage(TmpSize, format());
	ResImage.fill(_MarginColor.rgb());
	QPainter Painter(&ResImage);
	QImage TmpImage = scaled(TmpSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if (_Align == STImage::AlignCenter)
		Painter.drawImage(QPoint((ResImage.width() - TmpImage.width()) / 2, (ResImage.height() - TmpImage.height()) / 2 ), TmpImage);
	else 
		Painter.drawImage(QPoint(0, 0), TmpImage);
	return ResImage; 
}

void STImage::multiplyImage(const QSize& _DestSize, QPainter* _Painter) const
{
	int NImagesX = _DestSize.width() / width();
	int NImagesY = _DestSize.height() / height();
	int MarginX = (_DestSize.width() - (width() * NImagesX)) / (NImagesX + 1);
	int MarginY = (_DestSize.height() - (height() * NImagesY)) / (NImagesY + 1);
	for (int X=0; X < NImagesX; ++X)
	{
		for (int Y=0; Y < NImagesY; ++Y)
		{
			//qDebug("X, Y: %d, %d", X * (width() + MarginX) +  MarginX, Y * (height() + MarginY) +  MarginY);
			_Painter->drawImage(QPoint(X * (width() + MarginX) +  MarginX,  Y * (height() + MarginY) +  MarginY), *this);
		}
	}
}

STImage STImage::multiplyImage(const QSize& _DestSize) const
{
	if (_DestSize.isValid() && _DestSize.width() >= width() && _DestSize.height() >= height() )
	{
		//qDebug("DestSize %d, %d", _DestSize.width(), _DestSize.height()); 
		//! Lets construct a multiimage with ResImg into a new BaseSize Image:
		STImage BigResImage(_DestSize, QImage::Format_RGB32);
		BigResImage.fill(QColor(255, 255, 255).rgb());
		QPainter MPainter(&BigResImage);
		multiplyImage(_DestSize, &MPainter);
		MPainter.end();
		return BigResImage; 
	}
	else 
		return *this; 
}

//!Dots per meter;
int STImage::dpM(int _Dpis) const
{
    return Dpis / 0.0254;
}

void STImage::print(QPainter& _Painter, bool _Center, bool _Cut)
{
	_Painter.save();
	QRect ViewpRect = _Painter.viewport();

	QPoint DrawPoint = QPoint(0, 0); 
	QImage CImage = *this; 
// 	qDebug("PageSize %ld", _Printer->pageSize()); 
// 	qDebug("FullPage%ld", _Printer->fullPage()); 
// 	qDebug("Orientation %ld", _Printer->orientation()); 
// 	qDebug("Resolution %ld", _Printer->resolution()); 
// 	qDebug("OutputFormat %ld", _Printer->outputFormat()); 
// 	qDebug("PaperRect %ld, %ld, %ld, %ld", _Printer->paperRect().x(), _Printer->paperRect().y(), _Printer->paperRect().width(), _Printer->paperRect().height()); 
	if ((CImage.size().width() > CImage.size().height() && ViewpRect.size().width() <= ViewpRect.size().height()) ||
			(CImage.size().height() > CImage.size().width() && ViewpRect.size().height() <= ViewpRect.size().width()))
	{
		QMatrix PrintMatrix;
		PrintMatrix.rotate(90);
		CImage = CImage.transformed(PrintMatrix, Qt::SmoothTransformation);
	}

	QSize ImgSize = CImage.size(); 
	if (_Cut)
	{
		QSize ImgPaperSize = ViewpRect.size();
		ImgPaperSize.scale(CImage.size(), Qt::KeepAspectRatio);
		CImage = CImage.copy((ImgSize.width() - ImgPaperSize.width()) / 2, (ImgSize.height() - ImgPaperSize.height()) / 2,
							  ImgPaperSize.width(), ImgPaperSize.height());
		ImgSize = ImgPaperSize;
	}
	ImgSize.scale(ViewpRect.size(), Qt::KeepAspectRatio);
	_Painter.setViewport(ViewpRect.x(), ViewpRect.y(), ImgSize.width(), ImgSize.height()); 
	_Painter.setWindow(CImage.rect());
	
	QSize LogicPaperSize = ViewpRect.size();
	LogicPaperSize.scale(CImage.size(), Qt::KeepAspectRatioByExpanding); // ???'' I think this doesn't nothing...
	if (_Center)
		DrawPoint = QPoint((LogicPaperSize.width() - CImage.size().width()) / 2, (LogicPaperSize.height() - CImage.size().height()) / 2); 
	_Painter.drawImage(DrawPoint, CImage);
	_Painter.restore();
}

void STImage::print(QPrinter* _Printer, bool _Center, bool _Cut)
{
	QPainter Painter(_Printer);
#ifdef USE_PRINTKEEPER
	if (PrintKeeper::printAcces(_Printer->printerName()))
		print(Painter, _Center, _Cut);
	else
		PrintKeeper::accesDeniedWarning(_Printer->printerName());
#else
	print(Painter, _Center, _Cut);
#endif
}

bool STImage::isSupportedImage(const QFileInfo& _FInfo)
{
	return QImageReader::supportedImageFormats().contains(_FInfo.suffix().toLower().toAscii());
}

QString STImage::supportedFormatsToReadFilter()
{
	return formatForFileFilter(QImageReader::supportedImageFormats());
}

QString STImage::supportedFormatsToWriteFilter()
{
	return formatForFileFilter(QImageWriter::supportedImageFormats());
}

SimpleCrypt STImage::getCrypto(const QString& _Key)
{
	//setup our objects
	SimpleCrypt Crypto(keyToInt64(_Key));
	Crypto.setCompressionMode(SimpleCrypt::CompressionAlways); //always compress the data, see section below
	Crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);  //properly protect the integrity of the data
	return Crypto;
}


bool STImage::saveEncoded(const QString& _FileName, const QString& _Key)
{
#if QT_VERSION >= 0x040600
	//setup our objects
	SimpleCrypt Crypto = getCrypto(_Key);
	QBuffer Buffer;
	Buffer.open(QIODevice::WriteOnly);
	QDataStream Stream(&Buffer);
	//stream the data into our buffer
	Stream.setVersion(QDataStream::Qt_4_6);
	//s << myString; //stream in a string
	//s << myUint16; //stream in an unsigned integer
	Stream << *this; //stream in an image

	QByteArray myCypherText = Crypto.encryptToByteArray(Buffer.data());
	if (Crypto.lastError() != SimpleCrypt::ErrorNoError)
	return false;

	// Save FileName:
	QFile DestFile(encodedFileName(_FileName));
	if (!DestFile.open(QIODevice::WriteOnly))
	{
	 qDebug() << "Could not open dest file for write .";
	 return false;
	}

	DestFile.write(myCypherText, myCypherText.size());
	DestFile.close();
	Buffer.close();
#endif
	return true;
}

bool STImage::loadEncoded(const QString& _FileName, const QString& _Key)
{
#if QT_VERSION >= 0x040600
	QFile SourceFile(_FileName);
	if (!SourceFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Could not open dest file for read:" << _FileName;
		return false;
	}
	SimpleCrypt Crypto = getCrypto(_Key);
	QByteArray mySourceCypherText = SourceFile.readAll();
	QByteArray plaintext = Crypto.decryptToByteArray(mySourceCypherText);
	if (!Crypto.lastError() == SimpleCrypt::ErrorNoError) {
	 // check why we have an error, use the error code from crypto.lastError() for that
		qDebug() << "Error decoding!:" << Crypto.lastError();
	 return false;
	}
	QBuffer rbuffer(&plaintext);
	rbuffer.open(QIODevice::ReadOnly);
	QDataStream ls(&rbuffer);
	ls.setVersion(QDataStream::Qt_4_6);
	//s >> myString; //stream in a string
	//s >> myUint16; //stream in an unsigned integer
	ls >> *this; //stream in an image
	//s >> someMoreData;
	//etc.

	rbuffer.close();
#endif
	return true;
}

quint64 STImage::keyToInt64(const QString& _Key) const
{
	bool ok;
	QString Hex = QString("0x%1").arg(QString(_Key.toLatin1().toHex()).right(16));
	return Hex.toLongLong(&ok, 16);
}

bool STImage::isEncoded(const QString& _FileName)
{
	QFileInfo FInfo(_FileName);
	return FInfo.suffix().toLower() == encodedSuffix();
}


QString STImage::encodedFileName(const QString& _ImageFileName)
{
	return _ImageFileName + "." + encodedSuffix();
}



QString STImage::hashString() const
{
	QCryptographicHash CryptoHash(QCryptographicHash::Md5);
	CryptoHash.addData((char* )bits(), numBytes()); 
	QByteArray HashRes = CryptoHash.result();
	return QString(HashRes.toHex());
}

QString STImage::hashString(const QString& _FilePath)
{
	QString Res; 
	QFile File(_FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		QCryptographicHash CryptoHash(QCryptographicHash::Md5);
		int BuffSize = 16384;
		QByteArray CBuff = File.read(BuffSize);
		while (!CBuff.isEmpty())
		{
			CryptoHash.addData(CBuff);
			CBuff = File.read(BuffSize);
		}
		QByteArray HashRes = CryptoHash.result();
		Res = QString(HashRes.toHex());
	}
	return Res;
}

QString STImage::hashFileName(const QString& _FilePath)
{
	QString Res = hashString(_FilePath); 
	if (!Res.isEmpty())
	{
		QFileInfo FInfo(_FilePath); 
		Res += "." + FInfo.completeSuffix();
	}
	return Res;
}

