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
#include "stphotobookcollectionmodel.h"
#include <QDebug>
#include <QPixmap>
#include "stphotobookcollectioninfo.h"

void STPhotoBookCollectionModel::loadDir(const QDir& _PBDir, bool _Recursive)
{
	QFileInfoList TmpInfoList = _PBDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList::const_iterator it = TmpInfoList.begin();
	while (it != TmpInfoList.end())
	{
		STPhotobookCollectionInfo MInfo(QDir(it->absoluteFilePath()));
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

STPhotoBookCollectionModel::STPhotoBookCollectionModel(QObject* _Parent, bool _Load)
	: QAbstractListModel(_Parent), PBDir(STPhotobookCollectionInfo::defaultRootPathName()), ThumbnailSize(150, 150)
{
	PBFInfoList.clear();
	if (_Load)
		load();
}

int STPhotoBookCollectionModel::rowCount(const QModelIndex& ) const
{
	return PBFInfoList.size();
}

QVariant STPhotoBookCollectionModel::data(const QModelIndex& _Index, int _Role ) const
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
				//STPhotobookCollectionInfo CInfo(CPBookName, PBDir.absolutePath());
				STPhotobookCollectionInfo CInfo(QDir(PBFInfoList[_Index.row()].absoluteFilePath()));
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

void STPhotoBookCollectionModel::setFiles(const QStringList& _Files)
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

void STPhotoBookCollectionModel::load(bool _Recursive)
{
	PBFInfoList.clear();
	loadDir(PBDir, _Recursive);
	reset();
}

		
QString STPhotoBookCollectionModel::photoBookPath(const QModelIndex& _Index) const
{
	QString Res;
	if (_Index.row() >= 0 && _Index.row() < PBFInfoList.size())
		Res = PBFInfoList[_Index.row()].absoluteFilePath();

	return Res;
}

bool STPhotoBookCollectionModel::areTherePhotoBooks()
{
	QDir LPBDir(STPhotobookCollectionInfo::defaultRootPathName());
	return !LPBDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty();
}
