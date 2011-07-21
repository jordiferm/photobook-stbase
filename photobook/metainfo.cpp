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

using namespace SPhotoBook;

QDomElement MetaInfo::createGlobalInfo(QDomDocument& _Doc) const
{
	QDomElement Element = _Doc.createElement("global");

	Element.setAttribute("name", Name);
	return Element;
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
	PreferMinPages = true;
	Atomic = true;
	Cyphered = false;
	CutPagesOnPrint = false;
	PageMarginRects.clear();
	CoverMarginRects.clear();
	NumOptimalImagesPerPage = 3;
}

void MetaInfo::save(const QString& _XmlFilePath)
{
	bool Res = false;

	QFile TextFile(_XmlFilePath);
	Assert(TextFile.open(QFile::WriteOnly | QFile::Truncate), STError(QObject::tr("Error loading file: %1").arg(_XmlFilePath)));
	QTextStream Out(&TextFile);

	//Out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
	Out.setCodec(QTextCodec::codecForName("UTF-8"));
	QDomDocument Doc("starphob_metainfo");
	QDomElement Root = Doc.createElement("info");
	Doc.appendChild(Root);
	Root.setAttribute("version", "1.0.0");
	Root.appendChild(createGlobalInfo(Doc));

	Out << Doc.toString();
	Res = true;
}

void MetaInfo::load(const QString& _XmlFilePath)
{

}


//!Dots per meter;
int MetaInfo::dpM() const
{
	return Dpis / 0.0254;
}

