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
#include "stlocalestring.h"
#include "sapplication.h" 

STLocaleString::STLocaleString(const QString& _DefaultValue) : DefaultValue(_DefaultValue)
{
}

void STLocaleString::addValue(const QString& _Locale, const QString& _Value)
{
	StrMap[_Locale] = _Value; 
}

QString STLocaleString::value() const
{
	QString Res = DefaultValue;
	if (StrMap.contains(SApplication::currentLocale()))
		Res = StrMap[SApplication::currentLocale()];
	return Res; 
}

void STLocaleString::clear()
{
	StrMap.clear(); 
}
