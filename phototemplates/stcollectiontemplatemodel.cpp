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
#include "stcollectiontemplatemodel.h"
#include <QFile> 
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "sttemplatepaths.h" 
#include "sapplication.h"

//_____________________________________________________________________________
//
// STCollectionTemplateModel
//_____________________________________________________________________________
	
QString STCollectionTemplateInfo::BaseInfoUrl = TEMPLATEINFO_BASE_URL;
const QString STCollectionTemplateInfo::DefaultBaseInfoUrl = TEMPLATEINFO_BASE_URL;

STCollectionTemplateInfo::STCollectionTemplateInfo()
{
}

STCollectionTemplateInfo::STCollectionTemplateInfo(const QDir& _TemplateDir)
{
	bool XmlInfo = false;
	QDomDocument Doc("starphobinfo");
	BaseDir = _TemplateDir;
	LocaleDescription.clear();
	QFile InfoFile(_TemplateDir.absolutePath() + "/info.xml" );
	if (InfoFile.open(QIODevice::ReadOnly))
	{
		QTextStream StrIn(&InfoFile);
		StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
		XmlInfo = Doc.setContent(StrIn.readAll());
		InfoFile.close();
	}
	if (XmlInfo)
	{
		if (Doc.doctype().name().toLower() == "starphobinfo")
		{
			QDomElement RootEl = Doc.documentElement();
			Description = RootEl.attribute("description", QObject::tr("No Description"));
			LocaleDescription.setDefaultValue(Description);
			RemoteInfo = QUrl(BaseInfoUrl + "/" + RootEl.attribute("infourl"));

			QDomNodeList DescriptionNodes = RootEl.elementsByTagName("description");
			if (!DescriptionNodes.isEmpty())
			{
				QDomNode CurrNode = DescriptionNodes.item(0);
				QDomElement CurrEl = CurrNode.toElement();
				if (!CurrEl.isNull())
					foreach(QString Locale, SApplication::localeIds())
						LocaleDescription.addValue(Locale, CurrEl.attribute(Locale, Description));
			}


			QDomNodeList SizeNodes = RootEl.elementsByTagName("size");
			for (int Vfor = 0; Vfor < SizeNodes.size(); Vfor++)
			{
				QDomNode CurrNode = SizeNodes.item(Vfor);
				QDomElement CurrEl = CurrNode.toElement();
				if (!CurrEl.isNull())
				{
					STDom::DDocFormat CFormat(CurrEl.attribute("width", 0).toDouble(), CurrEl.attribute("height", 0).toDouble());
					CFormat.setDescription(CurrEl.attribute("name"));
					TFormatInfo FInfo;
					FInfo.Format = CFormat;
					FInfo.FormatDir = CurrEl.attribute("path");
					AvailableFormats.push_back(FInfo);
				}
			}
		}
	}
	Icon = QIcon(_TemplateDir.absolutePath() + "/icon.png");
	//TemplateXmlFile.setFile(_TemplateDir.absolutePath() + "/template.kpspltml");
}


QFileInfo STCollectionTemplateInfo::templateXmlFile(const STDom::DDocFormat& _Format) const
{
	QFileInfo Res;
	bool Found = false;
	TFormatList::const_iterator it = AvailableFormats.begin();
	while (!Found && it != AvailableFormats.end())
		{
		Found = it->Format == _Format;
		if (Found)
			Res = QFileInfo(BaseDir.absoluteFilePath(it->FormatDir) + "/template.kpspltml");
		++it;
	}
	return Res;
}

QDir STCollectionTemplateInfo::predesignDir(const QString& _PredesignName) const
{
	return QDir(BaseDir.absoluteFilePath(_PredesignName));
}

bool STCollectionTemplateInfo::isEmpty() const
{
	TFormatList::const_iterator it = AvailableFormats.begin();
	bool Found = false;
	while (!Found && it != AvailableFormats.end())
	{
		Found = templateXmlFile(it->Format).exists();
		++it;
	}
	return !Found;
}

STDom::DDocFormatList STCollectionTemplateInfo::availableFormats() const
{
	STDom::DDocFormatList Res;
	TFormatList::const_iterator it;
	for (it = AvailableFormats.begin(); it != AvailableFormats.end(); ++it)
	{
		if (templateXmlFile(it->Format).exists())
			Res.push_back(it->Format);
	}
	return Res;
}


//_____________________________________________________________________________
//
// STCollectionTemplateModel
//_____________________________________________________________________________

STCollectionTemplateModel::STCollectionTemplateModel(QObject* parent): QAbstractListModel(parent)
{
}

void STCollectionTemplateModel::loadTemplates(STPhotoLayout::EnLayoutType _Type)
{
	TemplateList.clear(); 
	QStringList PathList = STTemplatePaths::pathList(); 
	QStringList::iterator it; 
	for (it = PathList.begin(); it != PathList.end(); ++it)
	{
		QDir TDir(STTemplatePaths::templateTypePath(*it, _Type));
		
		QFileInfoList TDirList = TDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot); 
		QFileInfoList::iterator fit; 
		for (fit = TDirList.begin(); fit != TDirList.end(); ++fit)
		{
			QDir CurrDir(fit->absoluteFilePath()); 
			STCollectionTemplateInfo TInfo(CurrDir);
			if (!TInfo.isEmpty())
				TemplateList.push_back(TInfo); 
			
		}
	}
	reset();
}

int STCollectionTemplateModel::rowCount(const QModelIndex& _Parent) const
{
	return TemplateList.size(); 
}

QVariant STCollectionTemplateModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res; 
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		STCollectionTemplateInfo CCInfo = TemplateList[_Index.row()];
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

STCollectionTemplateInfo STCollectionTemplateModel::templateInfo(const QModelIndex& _Index) const
{
	STCollectionTemplateInfo Res;
	if (_Index.isValid() && _Index.row() < rowCount())
		Res = TemplateList[_Index.row()];
	return Res;
}

QUrl STCollectionTemplateModel::photoLayoutInfoUrl(const QModelIndex& _Index) const
{
	QUrl Res;
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		STCollectionTemplateInfo CCInfo = TemplateList[_Index.row()];
		Res = CCInfo.remoteInfo();
	}
	return Res;
}


QFileInfo STCollectionTemplateModel::photoLayoutFileInfo(const QModelIndex& _Index, const STDom::DDocFormat& _Format) const
{
	QFileInfo Res;
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		Res = TemplateList[_Index.row()].templateXmlFile(_Format);
	}
	return Res; 
}

STDom::DDocFormatList STCollectionTemplateModel::availableFormats(const QModelIndex& _Index) const
{
	STDom::DDocFormatList Res;
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		Res = TemplateList[_Index.row()].availableFormats();
	}
	return Res;
}
