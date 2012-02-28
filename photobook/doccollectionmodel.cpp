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
#include "doccollectionmodel.h"
#include <QDebug>
#include <QPixmap>
#include "collectioninfo.h"

using namespace SPhotoBook;

void DocCollectionModel::loadDir(const QDir& _PBDir, bool _Recursive)
{
	QFileInfoList TmpInfoList = _PBDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList::const_iterator it = TmpInfoList.begin();
	while (it != TmpInfoList.end())
	{
		CollectionInfo MInfo(QDir(it->absoluteFilePath()));
		if (QFile::exists(MInfo.xmlFileName()))
			PBFInfoList.push_back(*it);
		else
		{
			if (_Recursive)
				loadDir(QDir(it->absoluteFilePath()), true);
		}
		++it;
	}
}

DocCollectionModel::DocCollectionModel(QObject* _Parent, bool _Load)
	: QAbstractListModel(_Parent), PBDir(CollectionInfo::defaultRootPathName()), ThumbnailSize(150, 150)
{
	PBFInfoList.clear();
	if (_Load)
		load();
}

int DocCollectionModel::rowCount(const QModelIndex& ) const
{
	return PBFInfoList.size();
}

QVariant DocCollectionModel::data(const QModelIndex& _Index, int _Role ) const
{
	QVariant Res; 
	if (_Index.isValid())
	{
		if (_Index.row() >= 0 && _Index.row() < PBFInfoList.size())
		{
			QString CPBookName = PBFInfoList[_Index.row()].fileName(); 
			if (_Role == Qt::DisplayRole)
				Res = CPBookName; 
			else 
			if (_Role == Qt::DecorationRole)
			{
				//CollectionInfo CInfo(CPBookName, PBDir.absolutePath());
				CollectionInfo CInfo(QDir(PBFInfoList[_Index.row()].absoluteFilePath()));
				Res = QPixmap(CInfo.thumbnailFileName()).scaled(ThumbnailSize, Qt::KeepAspectRatio);
				//Res = QPixmap(CInfo.thumbnailFileName()).scaled(150, 100);
				//qDebug(CInfo.thumbnailFileName().toLatin1()); 
			}
			if (_Role == Qt::SizeHintRole)
				Res = QSize(ThumbnailSize.width() + 5, ThumbnailSize.height() + 5);
			//TODO Put the description on ToolTip Role.
		}
	}
	
	return Res; 
}

void DocCollectionModel::setFiles(const QStringList& _Files)
{
	PBFInfoList.clear();
	QStringList::const_iterator it = _Files.begin();
	while(it != _Files.end())
	{
		PBFInfoList.push_back(QFileInfo(*it));
		++it;
	}
	reset();
}

void DocCollectionModel::load(bool _Recursive)
{
	PBFInfoList.clear();
	loadDir(PBDir, _Recursive);
	reset();
}

		
QString DocCollectionModel::photoBookPath(const QModelIndex& _Index) const
{
	QString Res;
	if (_Index.row() >= 0 && _Index.row() < PBFInfoList.size())
		Res = PBFInfoList[_Index.row()].absoluteFilePath();

	return Res;
}

bool DocCollectionModel::areTherePhotoBooks()
{
	QDir LPBDir(CollectionInfo::defaultRootPathName());
	return !LPBDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty();
}
