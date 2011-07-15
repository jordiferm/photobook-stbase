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

#include "templateinfo.h"
#include "templatepaths.h"
#include <QDir>

using namespace SPhotoBook;

TemplateInfo::TemplateInfo() :
		BasePath(""), Name(""), Type(TypePhotoBook)
{
}

TemplateInfo::TemplateInfo(const QString& _Name, const QSizeF& _Size, EnTemplateType _Type) :
		BasePath(TemplatePaths::defaultTemplatesPath()), Name(_Name), Size(_Size), Type(_Type)
{
}

TemplateInfo::TemplateInfo(const QString& _BasePath, const QString& _Name, const QSizeF& _Size, EnTemplateType _Type) :
		BasePath(_BasePath), Name(_Name), Size(_Size), Type(_Type)
{
}

QString TemplateInfo::absolutePath() const
{
	QDir BaseDir(BasePath);
	QSize IntSize = Size.toSize();
	return BaseDir.absoluteFilePath(Name + QString("/%1x%2").arg(IntSize.width()).arg(IntSize.height()));
}

QString TemplateInfo::absolutePath(const DesignInfo& _Design)
{
	QDir BaseDir(absolutePath());
	return BaseDir.absoluteFilePath(_Design.name());
}

TemplateInfo::TDesignList TemplateInfo::designs() const
{
	return Designs;
}

void TemplateInfo::addDesign(const DesignInfo& _Design)
{
	Designs.push_back(_Design);
}

QUrl TemplateInfo::infoUrl() const
{
	return QUrl(QString("%1/%2").arg(TEMPLATEINFO_URLBASE).arg(name()));
}

