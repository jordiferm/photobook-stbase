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

#include "stphotobookclipartmodel.h"
#include <QPixmap>
#include <QDir>
#include <QFileInfo> 
#include <QSvgRenderer>
#include <QPainter> 
#include <QRectF> 

QString STPhotoBookClipartModel::thumbnailFileName(const QString& _FileName) const
{
	QFileInfo FInfo(_FileName); 
	return FInfo.absolutePath() + "/" + FInfo.baseName() + ".png";
}

//! Si no existeix el thumbnail el crea i el guarda a disk.
QPixmap STPhotoBookClipartModel::getThumbNail(const QString& _ImageName) const
{
	QString ThumbFileName = thumbnailFileName(_ImageName);
	if (!QFile::exists(ThumbFileName))
	{
		//qWarning(QString("Creating thumbnail for %1").arg(_ImageName).toLatin1());
		
		QSvgRenderer Renderer; 
		if (Renderer.load(_ImageName))
		{
			QImage Img(ThumbnailMaxSize, QImage::Format_RGB32);
 
			QPainter Painter(&Img); 
			Painter.fillRect(0, 0, Img.width(), Img.height(), QBrush(Qt::white)); 
			Renderer.render(&Painter, QRectF(0, 0, Img.width(), Img.height())); 
			if (!Img.save(ThumbFileName))
				qWarning(QString("Error saving thumbnail file %1").arg(ThumbFileName).toLatin1());
		}
		else 
			qWarning(QString("Error creating thumbnail for file %1").arg(ThumbFileName).toLatin1());
	}
	QPixmap ColPixmap(ThumbFileName);
	return ColPixmap.scaledToWidth(ThumbnailMaxSize.width());
}

STPhotoBookClipartModel::STPhotoBookClipartModel(QObject* _Parent) : STPhotoBookFolderDocsModel(_Parent)
{
	ThumbnailMaxSize = QSize(100, 100);
}

int STPhotoBookClipartModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return ImageFiles.size();
}

QVariant STPhotoBookClipartModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < ImageFiles.size())
		{
			QFileInfo CurrImage = ImageFiles[_Index.row()];
			if (_Role == STPhotoBookClipartModel::FileName)
			{
				Res = CurrImage.absoluteFilePath();
			}
			else 
			if (_Role == STPhotoBookClipartModel::ThumbnailFileName)
			{
				Res = thumbnailFileName(CurrImage.absoluteFilePath());
			}
			else 
			if (_Role == Qt::DecorationRole)
			{
				Res = getThumbNail(CurrImage.absoluteFilePath());
			}
		}
	}
	return Res;
}

bool STPhotoBookClipartModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
 	for (int Vfor = 0; Vfor < _Count; Vfor++)
 		ImageFiles.removeAt(_Row + Vfor);
	endRemoveRows();
	return true;
}

QVariant STPhotoBookClipartModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}

void STPhotoBookClipartModel::setPhotoBook(const STPhotoBookTemplate& _Album)
{
	setPhotoBookTemplate(_Album);
}

void STPhotoBookClipartModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STPhotoBookClipartModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}

void STPhotoBookClipartModel::loadFromPath(const QString& _PathName)
{
	LastPath = _PathName;
	reload();
}

void STPhotoBookClipartModel::reload()
{
	if (!LastPath.isEmpty())
	{
		ImageFiles = loadPath(LastPath);
		reset();
	}
}

void STPhotoBookClipartModel::setPhotoBookTemplate(const STPhotoBookTemplate& _Template)
{
	loadFromPath(_Template.clipartDirPath());
}

QStringList STPhotoBookClipartModel::fileFilter() const
{
	QStringList Filter;
	Filter << "*.svg" << "*.SVG";
	return Filter;
}

QString STPhotoBookClipartModel::filePath(const QModelIndex& _Index) const
{
	return data(_Index, FileName).toString();
}

