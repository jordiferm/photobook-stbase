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

#include "stphotobookmaskmodel.h"
#include <QPixmap>
#include <QDir>
#include <QFileInfo> 
#include <QPainter> 
#include <QRectF> 

#include "stpbmimetypes.h"
#include "stmaskmimedata.h"

QString STPhotoBookMaskModel::thumbnailFileName(const QString& _FileName) const
{
	QFileInfo FInfo(_FileName); 
	return FInfo.absolutePath() + "/" + FInfo.baseName() + ".png.thumb";
}

//! Si no existeix el thumbnail el crea i el guarda a disk.
QPixmap STPhotoBookMaskModel::getThumbNail(const QString& _ImageName) const
{
	QString ThumbFileName = thumbnailFileName(_ImageName);
	if (!QFile::exists(ThumbFileName))
	{
		QImage Img(_ImageName);
		Img = Img.scaled(ThumbnailMaxSize);
		if (!Img.save(ThumbFileName, "PNG"))
			qWarning(QString("Error saving thumbnail file %1").arg(ThumbFileName).toLatin1());
	}
	QPixmap ColPixmap(ThumbFileName);
	return ColPixmap.scaledToWidth(ThumbnailMaxSize.width());
}

STPhotoBookMaskModel::STPhotoBookMaskModel(QObject* _Parent) : STPhotoBookFolderDocsModel(_Parent)
{
	ThumbnailMaxSize = QSize(90, 60);
	NoMaskThumbnail = QImage(ThumbnailMaxSize, QImage::Format_RGB32); 
	QPainter Painter(&NoMaskThumbnail); 
	Painter.fillRect(NoMaskThumbnail.rect(), Qt::white); 
	Painter.setPen(QPen(QBrush(Qt::black), 3 )); 
	Painter.drawRect(NoMaskThumbnail.rect()); 
	QString Text = tr("No Mask");
	QRect TextBoundRect = Painter.fontMetrics().boundingRect(Text); 
	Painter.drawText(QPoint((NoMaskThumbnail.width() / 2) -  (TextBoundRect.width() / 2),
									(NoMaskThumbnail.height() / 2) -  (TextBoundRect.height() / 2)), Text); 
	
}

int STPhotoBookMaskModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return ImageFiles.size() + 1;
}

QVariant STPhotoBookMaskModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() <= ImageFiles.size())
		{
			if (_Index.row() == 0) //No Mask
			{
				if (_Role == STPhotoBookMaskModel::FileName)
					Res = ""; 
				else 
				if (_Role == STPhotoBookMaskModel::ThumbnailFileName)
					Res = ""; 
				else 
				if (_Role == Qt::DecorationRole)
					Res = QPixmap::fromImage(NoMaskThumbnail); 
			}
			else
			{
				QFileInfo CurrImage = ImageFiles[_Index.row() - 1];
				if (_Role == STPhotoBookMaskModel::FileName)
				{
					Res = CurrImage.absoluteFilePath();
				}
				else 
				if (_Role == STPhotoBookMaskModel::ThumbnailFileName)
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
	}
	return Res;
}

bool STPhotoBookMaskModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	bool Res;
	if (_Row > 0)
	{
		int PhisicRow = _Row -1;
		beginRemoveRows(_Parent, PhisicRow, PhisicRow + _Count - 1);
		for (int Vfor = 0; Vfor < _Count; Vfor++)
			ImageFiles.removeAt(PhisicRow + Vfor);
		endRemoveRows();
		Res = true;
	}
	else 
		Res = false; 
	return Res;
}

QVariant STPhotoBookMaskModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}


void STPhotoBookMaskModel::setPhotoBook(const STPhotoBookTemplate& _Album)
{
	//loadPath(_Album.maskDirPath());
	setPhotoBookTemplate(_Album);
}

void STPhotoBookMaskModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STPhotoBookMaskModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}

void STPhotoBookMaskModel::loadFromPath(const QString& _PathName)
{
	LastPath = _PathName;
	reload();
}

void STPhotoBookMaskModel::reload()
{
	if (!LastPath.isEmpty())
	{
		ImageFiles = loadPath(LastPath);
		reset();
	}
}

void STPhotoBookMaskModel::setPhotoBookTemplate(const STPhotoBookTemplate& _Template)
{
	loadFromPath(_Template.maskDirPath());
}

QStringList STPhotoBookMaskModel::fileFilter() const
{
	QStringList Filter;
	Filter << "*.png" << "*.PNG";
	return Filter;
}

QString STPhotoBookMaskModel::filePath(const QModelIndex& _Index) const
{
	return data(_Index, FileName).toString();
}

QStringList STPhotoBookMaskModel::mimeTypes() const
{
	QStringList types;
	types << MASK_MIMETYPE;
	return types;
}

QMimeData* STPhotoBookMaskModel::mimeData(const QModelIndexList &indexes) const
{
	STMaskMimeData* MimeData = new STMaskMimeData();
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid())
		{
			MimeData->setMaskFilePath(filePath(Index));
		}
	}
	return MimeData;
}
