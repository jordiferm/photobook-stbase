/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef STLOCALESTRING_H
#define STLOCALESTRING_H
#include "stcoreexport.h"

#include <QMap> 
#include <QString>
/**
Class to store translations strings for each locale.

	@author
*/
class ST_CORE_EXPORT STLocaleString
{

	typedef QMap<QString, QString> TStrMap; 
	TStrMap StrMap;
	QString DefaultValue;

public:
	STLocaleString(const QString& _DefaultValue = "");
	void setDefaultValue(const QString& _Value) { DefaultValue = _Value; }
	void addValue(const QString& _Locale, const QString& _Value);
	//!Value of current application locale. 
	QString value() const; 
	void clear(); 
};

#endif
