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
#include "designinfo.h"

namespace STPhotoBook
{

class ST_PHOTOBOOK_EXPORT TemplateInfo
{
public:
	enum EnTemplateType
	{
		TypePhotoBook,
		TypeCalendar,
		TypeCard,
		TypeIdPhoto,
		TypeMultiPhoto
	};

	typedef QList<DesignInfo> TDesignList;

private:
	QString Name, BasePath;
	QSizeF Size;
	EnTemplateType Type;
	TDesignList Designs;


public:
	TemplateInfo(const QString& _BasePath, const QString& _Name, const QSizeF& _Size, EnTemplateType );
	void setName(const QString& _Value) { Name = _Value;  }
	QString name() const { return Name; }
	void setSize(const QSizeF& _Size) { Size = _Size; }
	QSizeF size() const { return Size; }
	QString absolutePath() const;
	QString absolutePath(const DesignInfo& _Path);
	TDesignList designs() const;
	void addDesign(const DesignInfo& _Design);
	QUrl infoUrl() const;
};

}

#endif // TEMPLATEINFO_H
