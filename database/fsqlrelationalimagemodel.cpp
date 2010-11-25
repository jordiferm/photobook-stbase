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
#include "fsqlrelationalimagemodel.h"
#include <QPixmap> 
#include <QFile>
#include <QRegExp> 
#include <QDir>

QString FSqlRelationalImageModel::DefaultImagesDir = "/tmp" ;


QString FSqlRelationalImageModel::imageFileName(int _Row, int _ImgIndex) const
{
	QString RelFieldDataStr = FSqlRelationalTableModel::data( 
		FSqlRelationalTableModel::index(_Row, Images[_ImgIndex].RefFieldIndex)).toString();
	QString ImageFileName = ImagesDir +  QDir::separator() + Images[_ImgIndex].Prefix + "_" + RelFieldDataStr + ".png";
	return ImageFileName;
}

FSqlRelationalImageModel::FSqlRelationalImageModel(QObject* _Parent)
 : FSqlRelationalTableModel(_Parent), ImagesDir(DefaultImagesDir)
{
}

int FSqlRelationalImageModel::addImage(const QString& _Prefix, int _RelFieldIndex, const QString& _Label)
{
	Image CImage; 
	CImage.Prefix = _Prefix; 
	CImage.RefFieldIndex = _RelFieldIndex;
	CImage.Label = _Label;
	CImage.Modified = false;
	Images.push_back(CImage);
	return columnCount()-1;
}

QModelIndex FSqlRelationalImageModel::index(int row, int column, const QModelIndex& /*parent*/) const
{
    return createIndex(row, column, 0);
}


int FSqlRelationalImageModel::columnCount(const QModelIndex& _Parent)
{
	return FSqlRelationalTableModel::columnCount(_Parent) + Images.size();
}

QVariant FSqlRelationalImageModel::headerData(int _Section, Qt::Orientation _Orientation, int _Role) const
{
	QVariant Res;
	if (_Section >= FSqlRelationalTableModel::columnCount() && (_Role == Qt::DisplayRole))
	{
		int ImgIndex = _Section - FSqlRelationalTableModel::columnCount();
		if (Images.size() > ImgIndex)
		{
			Res = Images[ImgIndex].Label;
		}
	}
	else 
		Res = FSqlRelationalTableModel::headerData(_Section, _Orientation, _Role);
	
	return Res;
}

QVariant FSqlRelationalImageModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res; 
	
	if (_Index.column() >= FSqlRelationalTableModel::columnCount() && (_Role == Qt::EditRole || _Role == Qt::DisplayRole))
	{
		int ImgIndex = _Index.column() - FSqlRelationalTableModel::columnCount();
		if (Images.size() > ImgIndex)
		{
			Res = QPixmap(imageFileName(_Index.row(), ImgIndex));
			//qDebug(QString("Returning image %1").arg(imageFileName(_Index.row(), ImgIndex)).toLatin1());
		}
	}
	else 
	{
		//qDebug("Getting other fields...");
		Res = FSqlRelationalTableModel::data(_Index, _Role);
	}
	
	return Res;
}

bool FSqlRelationalImageModel::setData( const QModelIndex& _Index, const QVariant& _Value, int _Role)
{
	if (_Index.column() >= FSqlRelationalTableModel::columnCount() && (_Role == Qt::EditRole || _Role == Qt::DisplayRole))
	{
		int ImgIndex = _Index.column() - FSqlRelationalTableModel::columnCount();
		if (Images.size() > ImgIndex)
		{
			Images[ImgIndex].ImageFileName = imageFileName(_Index.row(), ImgIndex);
			QVariant Val = _Value;
			
			Images[ImgIndex].Pixmap = Val.value<QPixmap>();
			Images[ImgIndex].Modified = true;
		}		
	}
	//qDebug(QString("FSqlRelationalImageModel::setData(%1)").arg(_Value.toString()).toLatin1());
	//qDebug(QString("FSqlRelationalImageModel::setData type (%1)").arg(_Value.typeName()).toLatin1());
	return FSqlRelationalTableModel::setData(_Index, _Value, _Role);
}

bool FSqlRelationalImageModel::saveLastImages()
{
	bool Res = true; 
	for (int Vfor = 0; Vfor < Images.size(); Vfor++)
	{
		if (Images[Vfor].Modified)
		{
			Images[Vfor].Modified = false;
			if (!Images[Vfor].Pixmap.isNull())
			{
				//qDebug(QString("Saving image: %1").arg(Images[Vfor].ImageFileName).toAscii());
				Res = Res && Images[Vfor].Pixmap.save(Images[Vfor].ImageFileName, "PNG");
				Images[Vfor].Pixmap = QPixmap();
			}
			else 
			{
				QFile ImgFile(Images[Vfor].ImageFileName);
				Res = ImgFile.remove();
			}
		}
	}
	return Res;
}

bool FSqlRelationalImageModel::removeImages(int _Index)
{
	bool Res = true;
	for (int Vfor = 0; Vfor < Images.size(); Vfor++)
	{
		QFile ImgFile(imageFileName(_Index, Vfor));
		if (ImgFile.exists())
			Res = Res && ImgFile.remove();
		Images[Vfor].Pixmap = QPixmap();
	}
	return Res;
}

FSqlRelationalImageModel::TImageIndexList FSqlRelationalImageModel::imageIndexs() const
{
	TImageIndexList Res; 
	for (int Vfor=0; Vfor < Images.size(); Vfor ++)
	{
		Res.push_back(Vfor + FSqlRelationalTableModel::columnCount());
	}
	return Res;
}

FSqlRelationalImageModel::TImageIndexList FSqlRelationalImageModel::imageIndexs(const QRegExp& _PrefixMatch ) const
{
	TImageIndexList Res; 
	for (int Vfor=0; Vfor < Images.size(); Vfor ++)
	{
		if (_PrefixMatch.indexIn(Images[Vfor].Prefix) != -1 )
			Res.push_back(Vfor + FSqlRelationalTableModel::columnCount());
	}
	return Res;
}


void FSqlRelationalImageModel::setImagesDir(const QString& _ImagesDir) 
{ 
	ImagesDir = _ImagesDir; 
}

QString FSqlRelationalImageModel::imagesDir() const 
{ 
	return ImagesDir; 
}


void FSqlRelationalImageModel::setDefaultImagesDir(const QString& _ImagesDir)
{
	DefaultImagesDir = _ImagesDir;
}
