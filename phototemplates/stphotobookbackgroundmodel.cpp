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

#include "stphotobookbackgroundmodel.h"
#include <QPixmap>
#include <QDir>
//! Si no existeix el thumbnail el crea i el guarda a disk.
QPixmap STPhotoBookBackgroundModel::getThumbNail(const QString& _ImageName) const
{
	QString ThumbFileName = STPhotoLayoutTemplate::thumbnailImage(_ImageName);
	if (!QFile::exists(ThumbFileName))
	{
		qWarning(QString("Creating thumbnail for %1").arg(_ImageName).toLatin1());
		QPixmap BigPix(_ImageName);
		if (!BigPix.isNull())
		{
			BigPix = BigPix.scaled(400,300, Qt::KeepAspectRatio);
			if (!BigPix.save(ThumbFileName, "JPG"))
				qWarning(QString("Error saving thumbnail file %1").arg(ThumbFileName).toLatin1());
		}
	}
	QPixmap ColPixmap(ThumbFileName);
	return ColPixmap.scaled(ThumbnailMaxSize);
}

STPhotoBookBackgroundModel::STPhotoBookBackgroundModel(QObject* _Parent) : STPhotoBookFolderDocsModel(_Parent)
{
	ThumbnailMaxSize = QSize(150, 100);
}

int STPhotoBookBackgroundModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return ImageFiles.size();
}

QVariant STPhotoBookBackgroundModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < ImageFiles.size())
		{
			QFileInfo CurrImage = ImageFiles[_Index.row()];
			if (_Role == STPhotoBookBackgroundModel::FileName)
			{
				Res = CurrImage.absoluteFilePath();
			}
			else 
			if (_Role == STPhotoBookBackgroundModel::ThumbnailFileName)
			{
				Res = STPhotoLayoutTemplate::thumbnailImage(CurrImage.absoluteFilePath());
			}
			else 
			if (_Role == Qt::DecorationRole)
			{
// 				QPixmap ColPixmap(ThumbnailMaxSize);
// 				ColPixmap.fill(QColor("#000000"));
// 				Res = ColPixmap;

				Res = getThumbNail(CurrImage.absoluteFilePath());
			}
		}
	}
	return Res;
}

bool STPhotoBookBackgroundModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
 	for (int Vfor = 0; Vfor < _Count; Vfor++)
 		ImageFiles.removeAt(_Row + Vfor);
	endRemoveRows();
	return true;
}

QVariant STPhotoBookBackgroundModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}

void STPhotoBookBackgroundModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STPhotoBookBackgroundModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}

QPixmap STPhotoBookBackgroundModel::fileThumbnail(const QModelIndex& _Index) const
{
	return data(_Index, Qt::DecorationRole).value<QPixmap>();
}

void STPhotoBookBackgroundModel::setPhotoBookTemplate(const STPhotoBookTemplate& _Template)
{
	LastPath = _Template.bgImageDirPath();
	reload();
}

void STPhotoBookBackgroundModel::reload()
{
	if (!LastPath.isEmpty())
	{
		ImageFiles = loadPath(LastPath);
		reset();
	}
}


QStringList STPhotoBookBackgroundModel::fileFilter() const
{
	QStringList Filter;
	Filter << "*.jpg" << "*.png" << "*.JPG" << "*.PNG" ;
	return Filter;
}

QString STPhotoBookBackgroundModel::filePath(const QModelIndex& _Index) const
{
	return data(_Index, FileName).toString();
}

