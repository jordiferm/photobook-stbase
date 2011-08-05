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

#ifndef DDOC_H
#define DDOC_H
#include <QSize>
#include <QVariant>
#include <QFileInfo>
#include <QList>
#include <QDateTime>
#include <QImage>
#include "stdomexport.h"

namespace STDom {


class ST_DOM_EXPORT DDoc
{
public:
	enum { Type = 0 };

protected:
	qint64 Key;
	QString FilePath;
	qint64 ParentKey;
	QDateTime DateTime;
	QString Comment;
	qint64 Size;
	QString MD5Sum;
	QImage CachedThumbnailImage;
	static QSize ThumbNailSize;
	static QSize BigThumbNailSize;
	static int BigThumbnailQuality;
	static QImage NoThumbnailImage;


protected:
	virtual QImage loadThumbnailImage(bool _CreateIfNotExist = true) { return NoThumbnailImage; }

public:
	DDoc(const QString& _FilePath = "", qint64 _Key = -1, qint64 _ParentKey = -1);
	DDoc(const QFileInfo& _File, qint64 _Key = -1, qint64 _ParentKey = -1);
	bool isNull() { return FilePath.isEmpty(); }

	virtual int type() const { return Type; }

	//Accessors and Mutators
	void setKey(qint64 _Value) { Key = _Value; }
	qint64 key() const { return Key; }

	void setFilePath(const QString& _Value) { FilePath = _Value; }
	QString filePath() const { return FilePath; }
	QFileInfo fileInfo() const { return QFileInfo(FilePath); }

	void setParentKey(qint64 _Value) { ParentKey = _Value; }
	qint64 parentKey() const { return ParentKey; }

	void setDateTime(const QDateTime& _Value) { DateTime = _Value; }
	QDateTime dateTime() const { return DateTime; }

	void setComment(const QString& _Value) { Comment = _Value; }
	QString comment() const { return Comment; }

	void setSize(qint64 _Value) { Size = _Value; }
	qint64 size() const { return Size; }

	void setMd5Sum(const QString& _Value) { MD5Sum = _Value; }
	QString md5Sum() const { return MD5Sum; }

	//Big Thumbnails
	virtual void createBigThumbnail() const {}
	virtual QImage loadBigThumbnailImage(bool _CreateIfNotExist = true) const { return QImage(); }
	virtual void removeBigThumbnail() {}
	virtual bool hasBigThumbnail() const { return false; }
	QPixmap bigThumbnail(bool _CreateIfNotExist = false);

	static void setBigThumbnailSize(const QSize& _Size) { BigThumbNailSize = _Size; }
	static void setBigThumbnailQuality(int _Quality) { BigThumbnailQuality =_Quality; }
	static QSize bigThumbnailSize() { return BigThumbNailSize; }
	QFileInfo bigThumbNailFileName(const QFileInfo& _File) const {}

	//Thumbnails
	virtual void updateThumbnail();
	virtual bool createThumbnail(bool _CheckForMetadata = true) const {}
	QImage loadThumbnail(bool _CreateIfNotExist = true);
	void removeThumbnail();
	bool thumbnailLoaded() const;
	void cacheThumbnail(const QImage& _Image);
	QPixmap thumbnail();

	static void setThumbnailSize(const QSize& _Size) { ThumbNailSize = _Size; }
	static QSize thumbnailSize() { return ThumbNailSize; }
	QFileInfo thumbNailFileName(const QFileInfo& _File) const;
	static void setNoThumbnailImage(const QImage& _Image) { NoThumbnailImage = _Image; }

	//Same path => Same doc
	bool operator==(const DDoc& _Other) const;


	//Others
	virtual bool remove();
	QString pathHash() const;
};

/**
With and Height of a digital document physical size in mm.

@author jordif.starblitz@gmail.com
*/

class ST_DOM_EXPORT DDocFormat
{
	double Width; //!Width of the format in mm
	double Height;//!Height of the format in mm
	QString Key;
	QString Description;

public:
	//! Null format.
	DDocFormat(double _Width = 0, double _Height = 0, const QString& _Key = QString());
	DDocFormat(const QSizeF& _Size, const QString& _Key = QString());
	DDocFormat(const QString& _Format, const QString& _Key = QString());
	QSizeF size() const { return QSizeF(Width, Height); }
	double width() const;
	void setWidth(double );
	double height() const;
	void setHeight(double );
	//! Width in inches.
	double widthInch() const;
	//! Height in inches.
	double heightInch() const;
	//! Retorna el maxim entre width i height.
	double bigSide() const;
	//! Retorna el mínim entre width i height.
	double smallSide() const;
	void setKey(const QString& _Key) { Key = _Key; }
	QString key() const { return Key; }
	QString description() const { return Description; }
	void setDescription(const QString& _Value) { Description = _Value; }
	QSizeF landscapeSize() const { return QSizeF(bigSide(), smallSide()); }
	bool isNull() const;
	bool operator==(const DDocFormat& _Format) const;
	bool operator<(const DDocFormat& _Format) const;
	bool operator>(const DDocFormat& _Format) const;
	QSize dpis(const QSize& _PixelSize);
	//! Pixel Size depending on _Dpis.
	QSize pixelSize(int _Dpis) const;
	static QSize pixelSize(const QSizeF& _Size, int _Dpi);
	void setPixelSize(const QSize _PixelSize, int _Dpis);
	QString toString() const;
	static double mmToInch(double _Mm);
	static int mmToPixels(double _Mm, int _Dpis = 300);
	void copyOrientation(const DDocFormat& _Format);
	void transpose();

};

class ST_DOM_EXPORT DDocFormatList : public QList<DDocFormat>
{
public:
	//! \return The format in list than cuts the minimal portion of _Reference format.
	DDocFormat nearestRatioFormat(const DDocFormat& _Reference) const;
	//! \return The format in list that cuts the biggest portion of _Reference format.
	DDocFormat farestRatioFormat(const DDocFormat& _Reference) const;
	DDocFormat biggestFormat() const;
	DDocFormat smallestFormat() const;
};


class ST_DOM_EXPORT DDocProduct
{
	QString ProdRef;
	DDocFormat Format;
	QString Description;
	int Ordering;

public:
	DDocProduct(const QString& _ProdRef = "", const QString& _Description = "", const DDocFormat& _Format = DDocFormat(), int _Ordering = 0) : ProdRef(_ProdRef), Description(_Description), Format(_Format), Ordering(_Ordering) {}
	DDocFormat format() const { return Format; }
	bool isNull() const { return ProdRef.isEmpty(); }
	void setFormat(const DDocFormat& _Format) { Format = _Format; }
	QString ref() const { return ProdRef; }
	void setRef(const QString& _Ref) { ProdRef = _Ref; }
	void setDescription(const QString& _Description) { Description = _Description; }
	QString description() const { return Description; }
	int ordering() const { return Ordering; }
	bool operator==(const DDocProduct& _Product) const { return ProdRef == _Product.ProdRef; }
	bool operator<(const DDocProduct& _Product) const { return ProdRef < _Product.ProdRef; }
	bool operator>(const DDocProduct& _Product) const { return !operator<(_Product); }
};

class DDocProductList : public QList<DDocProduct>
{};


/**
Descrives the file and backtext of a print.
The file could be an image file or zip file. Zip files are normaly for products with multiple images.
Zip files will be normaly generated with image editors.

@author Shadow
*/

class ST_DOM_EXPORT DDocPrint
{
	QFileInfo FileInfo;
	QString BackPrintText;
	DDocProduct Product;
	int NumCopies;

public:
	DDocPrint(const QFileInfo& _File = QFileInfo(), const DDocProduct& _Product = DDocProduct(), int _NumCopies = 0)
			: FileInfo(_File), Product(_Product), NumCopies(_NumCopies), BackPrintText(_File.fileName()) {}
	QFileInfo fileInfo() const { return FileInfo; }
	void setFileInfo(const QFileInfo& _File) { FileInfo = _File; }

	QString backPrintText() const { return BackPrintText; }
	void setBackPrintText(const QString& _Value) { BackPrintText = _Value; }

	DDocProduct product() const { return Product; }
	void setProduct(const DDocProduct& _Product) { Product = _Product; }

	void setNumCopies(int _Value) { NumCopies = _Value; }
	int numCopies() const { return NumCopies; }

	//bool operator==(const DDocPrint& _Print) const { return FileInfo.absoluteFilePath() == _Print.FileInfo.absoluteFilePath(); }
	//bool operator<(const DDocPrint& _Print) const { return FileInfo.absoluteFilePath() < _Print.FileInfo.absoluteFilePath(); }
	//bool operator>(const DDocPrint& _Print) const { return !operator<(_Print); }
};

class DDocPrintList : public QList<DDocPrint>
{
public:
	int numCopies() const;
};


} //namespace STDom

Q_DECLARE_METATYPE(STDom::DDocFormat)

#endif // DDOC_H
