/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "appversion.h"
#include "sapplication.h"
#include <QRegExp>

AppVersion::AppVersion()
{
	setVersion(0,0,0);
}

AppVersion::AppVersion(int _Major, int _Minor, int _Release)
{
	setVersion(_Major, _Minor, _Release);
}

AppVersion::AppVersion(const QString& _FileName)
{
	setFileName(_FileName);
}

void AppVersion::setFileName(const QString& _FileName)
{
	setVersion(0,0,0);
	QRegExp RExp("([0-9]+)[.]([0-9]+)[.]([0-9])");
	int pos = RExp.indexIn(_FileName);
	if (pos > -1)
		setVersion(RExp.cap(1).toInt(), RExp.cap(2).toInt(), RExp.cap(3).toInt());
}

void AppVersion::setVersion(int _Major, int _Minor, int _Release)
{
	Major = _Major;
	Minor = _Minor;
	Release = _Release;
}

bool AppVersion::operator<(const AppVersion& _Other) const
{
	return (_Other.Major > Major) || (_Other.Major == Major && _Other.Minor > Minor) ||
		  (_Other.Major == Major && _Other.Minor == Minor && _Other.release() > Release);
}

AppVersion AppVersion::current()
{
	return AppVersion(SApplication::major(), SApplication::minor(), SApplication::release());
}

QString AppVersion::toString() const
{
	return QString::number(Major) + "." + QString::number(Minor) + "." + QString::number(Release);
}
