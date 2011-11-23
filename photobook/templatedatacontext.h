/****************************************************************************
**
** Copyright (C) 2006-2011 SoftTopia. All rights reserved.
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

#ifndef TEMPLATEDATACONTEXT_H
#define TEMPLATEDATACONTEXT_H
#include "stphotobookexport.h"
#include <QMap>
#include <QVariant>
#include <QStringList>

namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT TemplateDataContext
{
	QMap<QString, QString> VarMap;

	void setDefaultValues();

public:
    TemplateDataContext();
	//Var name is NOT case sensitive.
	QString value(const QString& _VarName) const;
	void setValue(const QString& _VarName, const QVariant& _Value);
	QStringList vars() const;
	void setDay(int _Value);
	void setMonth(int _Value);
	void setYear(int _Value);
	void setDate(const QDate& _Date);
	void setTitle(const QString& _Title);
	void setSubTitle(const QString& _SubTitle);
	void setAuthor(const QString& _Author);

	static QString varPattern(const QString& _VarName);

};
}

#endif // TEMPLATEDATACONTEXT_H
