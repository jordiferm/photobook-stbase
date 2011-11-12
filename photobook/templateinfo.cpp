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
		BasePath(""), Name(""), Type(MetaInfo::TypePhotoBook)
{
}

TemplateInfo::TemplateInfo(const MetaInfo& _MetaInfo) :
	BasePath(TemplatePaths::defaultTemplatesPath()), Name(_MetaInfo.name()), Size(_MetaInfo.size()), Type(_MetaInfo.templateType())
{

}


TemplateInfo::TemplateInfo(const QString& _Name, const QSizeF& _Size, MetaInfo::EnTemplateType _Type) :
		BasePath(TemplatePaths::defaultTemplatesPath()), Name(_Name), Size(_Size), Type(_Type)
{
}

TemplateInfo::TemplateInfo(const QString& _BasePath, const QString& _Name, const QString& _SizeName, MetaInfo::EnTemplateType _Type) :
		BasePath(_BasePath), Name(_Name), Type(_Type)
{
	QString StrWidth = _SizeName.left(_SizeName.indexOf("x"));
	Size.setWidth(StrWidth.toInt());
	QString SizeName = _SizeName;
	Size.setHeight(SizeName.remove(StrWidth + "x").toInt());
}

QString TemplateInfo::absolutePath() const
{
	QDir BaseDir(BasePath);
	QSize IntSize = Size.toSize();
	return BaseDir.absoluteFilePath(Name + QString("/%1x%2").arg(IntSize.width()).arg(IntSize.height()));
}

QString TemplateInfo::absolutePath(const DesignInfo& _Design) const
{
	QDir BaseDir(absolutePath());
	return BaseDir.absoluteFilePath(_Design.name());
}

DesignInfoList TemplateInfo::designs() const
{
	return Designs;
}

void TemplateInfo::addDesign(const DesignInfo& _Design)
{
	Designs.push_back(_Design);
}


bool TemplateInfo::operator==(const TemplateInfo& _Other) const
{
	return _Other.size() == size() && _Other.name() == name() && _Other.type() == type();
}
