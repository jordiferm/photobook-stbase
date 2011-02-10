/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PKPRINTERINFO_H
#define PKPRINTERINFO_H

#include <QString>


class PKPrinterInfo
{
public:
	QString Name;
	QString Ip;
	QString MAC;

public:
	PKPrinterInfo(const QString& _Name = "", const QString& _Ip = "", const QString& _MAC = "")
			: Name(_Name), Ip(_Ip), MAC(_MAC) {}
	QString name() const { return Name; }
	void setName(const QString& _Value) { Name = _Value; }
	QString ip() const { return Ip; }
	void setIp(const QString& _Value) { Ip = _Value; }
	QString mAC() const { return MAC; }
	void setMAC(const QString& _Value) { MAC = _Value; }
};

#endif // PKPRINTERINFO_H
