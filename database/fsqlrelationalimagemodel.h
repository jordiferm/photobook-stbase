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
#ifndef FSQLRELATIONALIMAGEMODEL_H
#define FSQLRELATIONALIMAGEMODEL_H

#include "fsqlrelationaltablemodel.h"
#include <QPixmap> 
#include "stdatabaseexport.h"


/**
Sql relational model with Images.
Warning: this class could not manage modifications of 2 rows at a time ! See saveLastImages .
 
@author Shadow
*/

class ST_DATABASE_EXPORT FSqlRelationalImageModel : public FSqlRelationalTableModel
{
Q_OBJECT
	struct Image 
	{
		QString Prefix; 
		int	RefFieldIndex; 
		QPixmap Pixmap;
		QString ImageFileName;
		QString Label;
		bool Modified;
	};

	typedef QList<Image> TImageList;
	TImageList Images;
	QString ImagesDir;
	static QString DefaultImagesDir;
	
	QString imageFileName(int _Row, int _ImgIndex) const;
	

public: 
	typedef QList<int> TImageIndexList;
	
public:
	FSqlRelationalImageModel(QObject* _Parent = 0);
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	//! \return The Image column related index.
	int addImage(const QString& _Prefix, int _RelFieldIndex, const QString& _Label);
	int columnCount(const QModelIndex & parent = QModelIndex());
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	QVariant data( const QModelIndex & item, int role = Qt::DisplayRole ) const;
	bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	bool saveLastImages();
	bool removeImages(int _Index);
	TImageIndexList imageIndexs() const;
	TImageIndexList imageIndexs(const QRegExp& _PrefixMatch ) const;
	void setImagesDir(const QString& _ImagesDir);
	QString imagesDir() const;
	static void setDefaultImagesDir(const QString& _ImagesDir);
};

#endif
