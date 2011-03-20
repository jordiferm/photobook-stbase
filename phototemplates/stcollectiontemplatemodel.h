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
#ifndef STCOLLECTIONTEMPLATEMODEL_H
#define STCOLLECTIONTEMPLATEMODEL_H

#include <QAbstractListModel>
#include <QFileInfo> 
#include <QIcon>
#include <QUrl>
#include <QList>
#include <QDir>
#include "stphototemplatesexport.h"
#include "ddoc.h"
#include "stphotolayout.h"
#include "stlocalestring.h"

//#include "stphotolayout.h"

class ST_PHOTOTEMPLATES_EXPORT  STCollectionTemplateInfo
{
	struct TFormatInfo
	{
		QString FormatDir;
		STDom::DDocFormat Format;
	};
	typedef QList<TFormatInfo> TFormatList;



private:
	QFileInfo TemplateXmlFile; 
	QIcon Icon; 
	QString Description;
	STLocaleString LocaleDescription;
	QUrl RemoteInfo;
	TFormatList AvailableFormats;
	QDir BaseDir;

	static QString BaseInfoUrl;
	static const QString DefaultBaseInfoUrl;

public:	
	STCollectionTemplateInfo(); //Null template info
	STCollectionTemplateInfo(QDir& _TemplateDir); 
	void setIcon(const QIcon& _Icon) { Icon = _Icon; }
	QIcon icon() const {return Icon; } 
	void setDescription(const QString& _Description) { Description = _Description; }
	QString description() const { return Description; }
	STLocaleString localeDescription() const { return LocaleDescription; }
	void setTemplateXml( const QFileInfo& _TemplateXmlFile ) { TemplateXmlFile = _TemplateXmlFile; }
	QFileInfo templateXmlFile(const STDom::DDocFormat& _Format) const;
	//! \return true if there is no template.kpstml files in some path sizes.
	bool isEmpty() const;
	QUrl remoteInfo() const { return RemoteInfo; }
	STDom::DDocFormatList availableFormats() const;
	static QString defaultBaseInfoUrl() { return DefaultBaseInfoUrl; }
	static void setBaseInfoUrl(const QString& _Value) { BaseInfoUrl = _Value; }
	static QString baseInfoUrl() { return BaseInfoUrl; }
};

/**
Model for list of templates on collection structure.

	@author
*/
class ST_PHOTOTEMPLATES_EXPORT STCollectionTemplateModel : public QAbstractListModel
{
	
private:
	typedef QList<STCollectionTemplateInfo> TTemplateList; 
	
	TTemplateList TemplateList; 
	
public:
	STCollectionTemplateModel(QObject* parent = 0);
	void loadTemplates(STPhotoLayout::EnLayoutType _Type);
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	STCollectionTemplateInfo templateInfo(const QModelIndex& _Index) const;
	QUrl photoLayoutInfoUrl(const QModelIndex& _Index) const;
	QFileInfo photoLayoutFileInfo(const QModelIndex& _Index, const STDom::DDocFormat& _Format) const;
	STDom::DDocFormatList availableFormats(const QModelIndex& _Index) const;
};

#endif
