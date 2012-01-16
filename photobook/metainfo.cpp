/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "metainfo.h"
#include <QTextStream>
#include <QFile>
#include <QTextCodec>
#include <QDir>
#include "stimage.h"

using namespace SPhotoBook;

void MetaInfo::setMarginAttributes(QDomElement& _Element, const QString& _MarginName, const QRectF& _MarginRect) const
{
	_Element.setAttribute(QString("%1_top").arg(_MarginName), _MarginRect.top());
	_Element.setAttribute(QString("%1_left").arg(_MarginName), _MarginRect.left());
	_Element.setAttribute(QString("%1_bottom").arg(_MarginName), _MarginRect.bottom());
	_Element.setAttribute(QString("%1_right").arg(_MarginName), _MarginRect.right());
}

QRectF MetaInfo::loadMarginAttributes(const QDomElement& _Element, const QString& _MarginName) const
{
	QRectF Res;
	Res.setTop(_Element.attribute(QString("%1_top").arg(_MarginName), "0").toDouble());
	Res.setLeft(_Element.attribute(QString("%1_left").arg(_MarginName), "0").toDouble());
	Res.setBottom(_Element.attribute(QString("%1_bottom").arg(_MarginName), "0").toDouble());
	Res.setRight(_Element.attribute(QString("%1_right").arg(_MarginName), "0").toDouble());
	return Res;
}

QDomElement MetaInfo::createGlobalInfo(QDomDocument& _Doc) const
{
	QDomElement Element = _Doc.createElement("global");

	Element.setAttribute("name", Name);
	Element.setAttribute("designname", DesignName);
	Element.setAttribute("description", Description);
	Element.setAttribute("imagepath", ImagePath);
	Element.setAttribute("sourceimagespath", SourceImagesPath);
	Element.setAttribute("size_width", Size.width());
	Element.setAttribute("size_height", Size.height());
	Element.setAttribute("printpagesize_width", PrintPageSize.width());
	Element.setAttribute("printpagesize_height", PrintPageSize.height());
	Element.setAttribute("version", Version);
	Element.setAttribute("dpis", Dpis);
	Element.setAttribute("modpages", ModPages);
	Element.setAttribute("maxpages", MaxPages);
	Element.setAttribute("minpages", MinPages);
	Element.setAttribute("printpreprocesstype", PrintPreprocessType);
	Element.setAttribute("templatetype", TemplateType);
	Element.setAttribute("multiphoto", MultiPhoto);
	Element.setAttribute("autogeneratelayouts", AutogenerateLayouts);
	Element.setAttribute("preferminpages", PreferMinPages);
	Element.setAttribute("atomic", Atomic);
	Element.setAttribute("cyphered", Cyphered);
	Element.setAttribute("cutpagesonprint", CutPagesOnPrint);

	setMarginAttributes(Element, "pagemargin", PageMarginRect);
	setMarginAttributes(Element, "covermargin", CoverMarginRect);
	setMarginAttributes(Element, "spinemargin", SpineMarginRect);

	//Element.setAttribute("", CoverMarginRects);
	Element.setAttribute("numoptimalimagesperpage", NumOptimalImagesPerPage);


	return Element;
}

void MetaInfo::loadGlobalInfo(const QDomElement& _Element)
{
	Name = _Element.attribute("name", "noname");
	DesignName = _Element.attribute("designname", "noname");
	Description = _Element.attribute("description", "");
	ImagePath = _Element.attribute("imagepath", "");
	SourceImagesPath = _Element.attribute("sourceimagespath", "");
	Size.setWidth(_Element.attribute("size_width", "210").toInt());
	Size.setHeight(_Element.attribute("size_height", "297").toInt());

	PrintPageSize.setWidth( _Element.attribute("printpagesize_width", QString::number(Size.width())).toInt());
	PrintPageSize.setHeight(_Element.attribute("printpagesize_height", QString::number(Size.height())).toInt());

	Version= _Element.attribute("version", "1").toInt();
	Dpis= _Element.attribute("dpis", "200").toInt();
	ModPages= _Element.attribute("modpages", "0").toInt();
	MaxPages= _Element.attribute("maxpages", "100").toInt();
	MinPages= _Element.attribute("minpages", "1").toInt();
	PrintPreprocessType = static_cast<RenderSettings::EnPrintPreProcessType>(_Element.attribute("printpreprocesstype", QString::number(RenderSettings::TypeNone)).toInt());
	TemplateType = static_cast<EnTemplateType>(_Element.attribute("templatetype", QString::number(TypePhotoBook)).toInt());
	MultiPhoto = _Element.attribute("multiphoto", "0") == "1";
	AutogenerateLayouts = _Element.attribute("autogeneratelayouts", "0") == "1";
	PreferMinPages = _Element.attribute("preferminpages", "1") == "1";
	Atomic = _Element.attribute("atomic", "1") == "1";
	Cyphered = _Element.attribute("cyphered", "0") == "1";

	PageMarginRect = loadMarginAttributes(_Element, "pagemargin");
	CoverMarginRect = loadMarginAttributes(_Element, "covermargin");
	SpineMarginRect = loadMarginAttributes(_Element, "spinemargin");
	//PageMarginRects.clear();
	//CoverMarginRects.clear();
	NumOptimalImagesPerPage = _Element.attribute("numoptimalimagesperpage", "3").toInt();
}

MetaInfo::MetaInfo()
{
	setDefaults();
}

void MetaInfo::setDefaults()
{
	DesignName = "noname";
	Name = "noname";
	Description = "";
	ImagePath = "";
	SourceImagesPath = "";
	Size = QSizeF(210, 297);
	PrintPageSize = Size;
	Version=1;
	Dpis=200;
	ModPages=0;
	MaxPages=100;
	MinPages=1;
	PrintPreprocessType = RenderSettings::TypeNone;
	TemplateType = TypePhotoBook;
	MultiPhoto = false;
	AutogenerateLayouts = false;
	PreferMinPages = false;
	Atomic = true;
	Cyphered = false;
	PageMarginRect.setRect(0, 0, 0, 0);
	CoverMarginRect.setRect(0, 0, 0, 0);
	SpineMarginRect.setRect(0, 0, 0, 0);
	NumOptimalImagesPerPage = 3;
}

void MetaInfo::checkVersion(const QString& _XmlFilePath)
{
	QFile TextFile(_XmlFilePath);

	//If file exist try to load it and check for version
	if (TextFile.exists())
	{
		//Save To temp file.
		QString TempFileName = QDir::temp().absoluteFilePath("pbmetainfotmp.xml");
		save(TempFileName, false);
		if (STImage::hashString(TempFileName) != STImage::hashString(_XmlFilePath))
			setVersion(version() + 1);
	}
}

void MetaInfo::save(const QString& _XmlFilePath, bool _CheckVersion)
{
	if (_CheckVersion)
		checkVersion(_XmlFilePath);

	QFile TextFile(_XmlFilePath);

	Assert(TextFile.open(QFile::WriteOnly | QFile::Truncate), STError(QObject::tr("Error saving file: %1").arg(_XmlFilePath)));
	QTextStream Out(&TextFile);

	//Out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
	Out.setCodec(QTextCodec::codecForName("UTF-8"));
	QDomDocument Doc("starphob_metainfo");
	QDomElement Root = Doc.createElement("info");
	Doc.appendChild(Root);
	Root.setAttribute("version", "1.0.0");
	Root.appendChild(createGlobalInfo(Doc));

	Out << Doc.toString();
}

void MetaInfo::load(const QString& _XmlFilePath)
{
	QDomDocument Doc;
	QFile File(_XmlFilePath);
	Assert(File.open(QIODevice::ReadOnly), STError(QObject::tr("Error loading file: %1").arg(_XmlFilePath)));
	QTextStream StrIn(&File);
	StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
	Assert(Doc.setContent(StrIn.readAll()), STError(QObject::tr("Error loading file: %1").arg(_XmlFilePath)));
	File.close();
	Assert(Doc.doctype().name().toLower() == "starphob_metainfo", STError(QObject::tr("Wrong file type loading %1").arg(_XmlFilePath)));
	QDomElement RootEl = Doc.documentElement();
	//TODO: Check document version.
	QString Version = RootEl.attribute("version", "0.0.0");

	QDomNode CNode = RootEl.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "global" )
				loadGlobalInfo(CEl);
		}
		CNode = CNode.nextSibling();
	}
}

QUrl MetaInfo::infoUrl() const
{
	return infoUrl(name());
}

QUrl MetaInfo::infoUrl(const QString& _Name)
{
	return QUrl(QString("%1/%2").arg(TEMPLATEINFO_URLBASE).arg(_Name));
}

//!Dots per meter;
int MetaInfo::dpM() const
{
	return Dpis / 0.0254;
}

QPixmap MetaInfo::typePixmap(EnTemplateType _Type)
{
	QPixmap Res;
	switch (_Type)
	{
		case TypePhotoBook :
			Res = QPixmap(":/photobook/typephotobook.png");
		break;
		case TypeCalendar :
			Res = QPixmap(":/photobook/typecalendar.png");
		break;
		case TypeCard :
			Res = QPixmap(":/photobook/typecard.png");
		break;
		case TypeIdPhoto :
			Res = QPixmap(":/photobook/typephotoid.png");
		break;
		case TypeMultiPhoto :
			Res = QPixmap(":/photobook/typeother.png");
		break;
	}
	return Res;
}

QString MetaInfo::typeString(EnTemplateType _Type)
{
	QString Res;
	switch (_Type)
	{
		case TypePhotoBook :
			Res = QObject::tr("PhotoBook");
		break;
		case TypeCalendar :
			Res = QObject::tr("Calendar");
		break;
		case TypeCard :
			Res = QObject::tr("Card");
		break;
		case TypeIdPhoto :
			Res = QObject::tr("Id Photo");
		break;
		case TypeMultiPhoto :
			Res = QObject::tr("MultiPhoto");
		break;
	}
	return Res;
}
