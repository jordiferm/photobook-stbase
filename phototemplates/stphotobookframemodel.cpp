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

#include "stphotobookframemodel.h"
#include <QPixmap>
#include <QDir>
#include <QFileInfo> 
#include <QPainter> 
#include <QRectF> 

#include "stpbmimetypes.h"
#include "stframemimedata.h"

QString STPhotoBookFrameModel::thumbnailFileName(const QString& _FileName) const
{
	QFileInfo FInfo(_FileName); 
	return FInfo.absolutePath() + "/" + FInfo.baseName() + ".png.thumb";
}

//! Si no existeix el thumbnail el crea i el guarda a disk.
QPixmap STPhotoBookFrameModel::getThumbNail(const QString& _ImageName) const
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

STPhotoBookFrameModel::STPhotoBookFrameModel(QObject* _Parent) : STPhotoBookFolderDocsModel(_Parent)
{
	ThumbnailMaxSize = QSize(150, 100);
	NoFrameThumbnail = QImage(ThumbnailMaxSize, QImage::Format_RGB32);
	QPainter Painter(&NoFrameThumbnail);
	Painter.fillRect(NoFrameThumbnail.rect(), Qt::white);
	Painter.setPen(QPen(QBrush(Qt::black), 3 )); 
	Painter.drawRect(NoFrameThumbnail.rect());
	QString Text = tr("No Frame");
	QRect TextBoundRect = Painter.fontMetrics().boundingRect(Text); 
	Painter.drawText(QPoint((NoFrameThumbnail.width() / 2) -  (TextBoundRect.width() / 2),
									(NoFrameThumbnail.height() / 2) -  (TextBoundRect.height() / 2)), Text);
	
}

int STPhotoBookFrameModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return ImageFiles.size() + 1;
}

QVariant STPhotoBookFrameModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() <= ImageFiles.size())
		{
			if (_Index.row() == 0) //No Frame
			{
				if (_Role == STPhotoBookFrameModel::FileName)
					Res = ""; 
				else 
				if (_Role == STPhotoBookFrameModel::ThumbnailFileName)
					Res = ""; 
				else 
				if (_Role == Qt::DecorationRole)
					Res = QPixmap::fromImage(NoFrameThumbnail);
			}
			else
			{
				QFileInfo CurrImage = ImageFiles[_Index.row() - 1];
				if (_Role == STPhotoBookFrameModel::FileName)
				{
					Res = CurrImage.absoluteFilePath();
				}
				else 
				if (_Role == STPhotoBookFrameModel::ThumbnailFileName)
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

bool STPhotoBookFrameModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
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

QVariant STPhotoBookFrameModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}


void STPhotoBookFrameModel::setPhotoBook(const STPhotoBookTemplate& _Album)
{
	//loadPath(_Album.maskDirPath());
	setPhotoBookTemplate(_Album);
}

void STPhotoBookFrameModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STPhotoBookFrameModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}

void STPhotoBookFrameModel::loadFromPath(const QString& _PathName)
{
	LastPath = _PathName;
	reload();
}

void STPhotoBookFrameModel::reload()
{
	if (!LastPath.isEmpty())
	{
		ImageFiles = loadPath(LastPath);
		reset();
	}
}

void STPhotoBookFrameModel::setPhotoBookTemplate(const STPhotoBookTemplate& _Template)
{
	loadFromPath(_Template.frameDirPath());
}

QStringList STPhotoBookFrameModel::fileFilter() const
{
	QStringList Filter;
	Filter << "*.png" << "*.PNG";
	return Filter;
}

QString STPhotoBookFrameModel::filePath(const QModelIndex& _Index) const
{
	return data(_Index, FileName).toString();
}

QStringList STPhotoBookFrameModel::mimeTypes() const
{
	QStringList types;
	types << FRAME_MIMETYPE;
	return types;
}

QMimeData* STPhotoBookFrameModel::mimeData(const QModelIndexList &indexes) const
{
	STFrameMimeData* MimeData = new STFrameMimeData();
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid())
		{
			MimeData->setFrameFilePath(filePath(Index));
		}
	}
	return MimeData;
}
