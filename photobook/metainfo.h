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

#ifndef METAINFO_H
#define METAINFO_H
#include "stphotobookexport.h"
#include <QDomElement>
#include <QList>
#include <QRectF>
#include "sterror.h"
#include "rendersettings.h"
#include "templateinfo.h"


namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT MetaInfo
{
public:
	ST_DECLARE_ERRORCLASS();
	typedef QList<QRectF> TMarginRectList;

private:
	QString DesignName, Name, Description, ImagePath, SourceImagesPath;
	QSizeF Size, PrintPageSize;
	int Version, Dpis, ModPages, MaxPages, MinPages;
	RenderSettings::EnPrintPreProcessType PrintPreprocessType;
	TemplateInfo::EnTemplateType TemplateType;
	bool MultiPhoto, AutogenerateLayouts, PreferMinPages, Atomic, Cyphered, CutPagesOnPrint;
	TMarginRectList PageMarginRects, CoverMarginRects;
	int NumOptimalImagesPerPage;

	QDomElement createGlobalInfo(QDomDocument& _Doc) const;

public:
    MetaInfo();
	void setDefaults();
	void save(const QString& _XmlFilePath);
	void load(const QString& _XmlFilePath);

	//------- Global info ----------
	void setName(const QString& _Value) { Name = _Value; }
	QString name() const { return Name; }

	void setDesignName(const QString& _Value) { DesignName = _Value; }
	QString designName() const { return DesignName; }

	void setDescription(const QString& _Value) { Description = _Value; }
	QString description() const { return Description; }

	void setImagePath(const QString& _Value) { ImagePath = _Value; }
	QString imagePath() const { return ImagePath; }

	void setSize(const QSizeF& _Value) { Size = _Value; }
	QSizeF size() const { return Size; }

	void setVersion(int _Value) { Version = _Value; }
	int version() const { return Version; }

	void setTemplateType(TemplateInfo::EnTemplateType _Value) { TemplateType = _Value; }
	TemplateInfo::EnTemplateType templateType() const { return TemplateType; }

	void setSourceImagesPath(const QString& _Value) { SourceImagesPath = _Value; }
	QString sourceImagesPath() const { return SourceImagesPath; }

	void setCyphered(bool _Value) { Cyphered = _Value; }
	bool cyphered() const { return Cyphered; }


	//------- Render info ----------
	void setPrintPageSize(const QSizeF& _Value) { PrintPageSize = _Value; }
	QSizeF printPageSize() const { return PrintPageSize; }

	void setDpis(int _Value) { Dpis = _Value; }
	int dpis() const { return Dpis; }
	int dpM() const;

	void setPrintPreprocessType(RenderSettings::EnPrintPreProcessType _Value) { PrintPreprocessType = _Value; }
	RenderSettings::EnPrintPreProcessType printPreprocessType() const { return PrintPreprocessType; }

	void setCutPagesOnPrint(bool _Value) { CutPagesOnPrint = _Value; }
	bool cutPagesOnPrint() const { return CutPagesOnPrint; }


	//------- Behavior info ----------
	void setModPages(int _Value) { ModPages = _Value; }
	//! The number of pages mod modPages() must be 0.
	int modPages() const { return ModPages; }

	void setMaxPages(int _Value) { MaxPages = _Value; }
	int maxPages() const { return MaxPages; }

	void setMinPages(int _Value) { MinPages = _Value; }
	int minPages() const { return MinPages; }

	void setMultiPhoto(bool _Value) { MultiPhoto = _Value; }
	bool multiPhoto() const { return MultiPhoto; }

	void setAutogenerateLayouts(bool _Value) { AutogenerateLayouts = _Value; }
	bool autogenerateLayouts() const { return AutogenerateLayouts; }

	void setPreferMinPages(bool _Value) { PreferMinPages = _Value; }
	bool preferMinPages() const { return PreferMinPages; }

	void setAtomic(bool _Value) { Atomic = _Value; }
	bool atomic() const { return Atomic; }

	void setPageMarginRects(const TMarginRectList& _Value) { PageMarginRects = _Value; }
	TMarginRectList pageMarginRects() const { return PageMarginRects; }

	void setCoverMarginRects(const TMarginRectList& _Value) { CoverMarginRects = _Value; }
	TMarginRectList coverMarginRects() const { return CoverMarginRects; }

	int numOptimalImagesPerPage() const { return NumOptimalImagesPerPage; }
	void setNumOptimalImagesPerPage(int _Value) { NumOptimalImagesPerPage = _Value; }
};

}

#endif // METAINFO_H
