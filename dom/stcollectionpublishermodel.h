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
#ifndef STCOLLECTIONPUBLISHERMODEL_H
#define STCOLLECTIONPUBLISHERMODEL_H

#include <QAbstractListModel>
#include <QFileInfo> 
#include <QIcon>
#include <QDir>
#include "stdomexport.h"

namespace STDom
{

//#include "stphotolayout.h"

class ST_DOM_EXPORT STCollectionPublisherInfo
{
	QFileInfo PublisherXmlFile; 
	QIcon Icon; 
	QString Description;
	 
public:	
	STCollectionPublisherInfo(const QDir& _PublisherDir = QDir()); 
	void setIcon(const QIcon& _Icon) { Icon = _Icon; }
	QIcon icon() const {return Icon; } 
	void setDescription(const QString& _Description) { Description = _Description; }
	QString description() const { return Description; }
	void setPublisherXml( const QFileInfo& _PublisherXmlFile ) { PublisherXmlFile = _PublisherXmlFile; }
	QFileInfo publisherXmlFile() const { return PublisherXmlFile; }
	QFileInfo publisherDatabaseFile() const { return PublisherXmlFile.dir().absolutePath() + "/publisher.db"; } 
};

/**
Model for list of Publishers on collection structure.

	@author
*/
class ST_DOM_EXPORT STCollectionPublisherModel : public QAbstractListModel
{
private:
	typedef QList<STCollectionPublisherInfo> TPublisherList; 
	
	TPublisherList PublisherList; 
	QString SharedDataPath; 
	
public:
	STCollectionPublisherModel(QObject* parent = 0);
	void loadPublishers();
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	QString publisherPath(); 
	STCollectionPublisherInfo publisherInfo(const QModelIndex& _Index); 
	static STCollectionPublisherInfo defaultPublisherInfo();
	QString sharedDataPath() const { return SharedDataPath; } 
	void setSharedDataPath(const QString& _Value) { SharedDataPath = _Value; }
};
}

#endif
