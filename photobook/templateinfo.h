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

#ifndef TEMPLATEINFO_H
#define TEMPLATEINFO_H

#include <QString>
#include <QSizeF>
#include <QList>
#include <QUrl>

#include "stphotobookexport.h"
#include "designinfolist.h"
#include "designinfo.h"
#include "metainfo.h"

namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT TemplateInfo
{
private:
	QString Name, BasePath;
	QSizeF Size;
	MetaInfo::EnTemplateType Type;
	DesignInfoList Designs;


public:
	TemplateInfo();
	TemplateInfo(const QString& _Name, const QSizeF& _Size, MetaInfo::EnTemplateType );
	TemplateInfo(const QString& _BasePath, const QString& _Name, const QSizeF& _Size, MetaInfo::EnTemplateType );
	bool isNull() { return Name.isEmpty(); }
	void setName(const QString& _Value) { Name = _Value;  }
	QString name() const { return Name; }
	void setSize(const QSizeF& _Size) { Size = _Size; }
	QSizeF size() const { return Size; }
	void setType(MetaInfo::EnTemplateType _Type) { Type = _Type; }
	MetaInfo::EnTemplateType type() const { return Type; }
	QString absolutePath() const;
	QString absolutePath(const DesignInfo& _Path);
	DesignInfoList designs() const;
	void addDesign(const DesignInfo& _Design);
	QUrl infoUrl() const;
};

}

#endif // TEMPLATEINFO_H
