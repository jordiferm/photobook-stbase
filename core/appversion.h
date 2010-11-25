/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef APPVERSION_H
#define APPVERSION_H
#include <QString>
#include "stcoreexport.h"

class ST_CORE_EXPORT AppVersion
{
	int Major, Minor, Release;

public:
	AppVersion();
	AppVersion(int _Major, int _Minor, int _Release);
	AppVersion(const QString& _FileName);
	void setFileName(const QString& _FileName);
	void setVersion(int _Major, int _Minor, int _Release);
	bool operator<(const AppVersion& _Other) const;
	int major() const { return Major; }
	int minor() const { return Minor; }
	int release() const { return Release; }
	static AppVersion current();
	QString toString() const;

};

#endif // APPVERSION_H
