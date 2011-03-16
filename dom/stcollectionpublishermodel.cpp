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
#include "stcollectionpublishermodel.h"
#include <QFile> 
#include <QDir>
#include "sapplication.h"

using namespace STDom;

//_____________________________________________________________________________
//
// STCollectionPublisherModel
//_____________________________________________________________________________
	
STCollectionPublisherInfo::STCollectionPublisherInfo(const QDir& _PublisherDir)
{
	QFile DescFile(_PublisherDir.absolutePath() + "/info.html" ); 
	if (DescFile.open(QFile::ReadOnly))
		Description = DescFile.readAll(); 
	DescFile.close();
	Icon = QIcon(_PublisherDir.absolutePath() + "/icon.png");
	PublisherXmlFile.setFile(_PublisherDir.absolutePath() + "/settings.xml"); 
}

//_____________________________________________________________________________
//
// STCollectionPublisherModel
//_____________________________________________________________________________

STCollectionPublisherModel::STCollectionPublisherModel(QObject* parent): QAbstractListModel(parent)
{
	SharedDataPath = SApplication::userSharedPath();
}

void STCollectionPublisherModel::loadPublishers()
{
	QDir TDir(publisherPath()); 
	PublisherList.clear(); 
	

	QFileInfoList TDirList = TDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot); 
	QFileInfoList::iterator it; 
	for (it = TDirList.begin(); it != TDirList.end(); ++it)
	{
		QDir CurrDir(it->absoluteFilePath()); 
		PublisherList.push_back(STCollectionPublisherInfo(CurrDir)); 
	}
	reset();
}

int STCollectionPublisherModel::rowCount(const QModelIndex& _Parent) const
{
	return PublisherList.size(); 
}

QVariant STCollectionPublisherModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res; 
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		STCollectionPublisherInfo CCInfo = PublisherList[_Index.row()];
		if (_Role == Qt::DisplayRole)
		{
			Res = CCInfo.description(); 
		}	
		else 
		if (_Role == Qt::DecorationRole)
		{
			Res = CCInfo.icon(); 
		}
	}
	return Res; 
}


QString STCollectionPublisherModel::publisherPath()
{
	QString Res = SharedDataPath + "/publishers"; 
	return Res; 
}		

STCollectionPublisherInfo STCollectionPublisherModel::publisherInfo(const QModelIndex& _Index)
{
	STCollectionPublisherInfo Res;
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		Res = PublisherList[_Index.row()]; 
	}
	return Res; 
}

STCollectionPublisherInfo STCollectionPublisherModel::defaultPublisherInfo()
{
	STCollectionPublisherInfo  PubInfo;
	STDom::STCollectionPublisherModel Model;
	Model.loadPublishers();
	if(Model.rowCount() > 0)
		PubInfo= Model.publisherInfo(Model.index(0,0));
	return PubInfo;
}
