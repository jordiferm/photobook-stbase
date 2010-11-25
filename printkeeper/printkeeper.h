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

#ifndef PRINTKEEPER_H
#define PRINTKEEPER_H

#include <QString>
#include "stprintkeeperexport.h"

class QMessageBox;
class ST_PRINTKEEPER_EXPORT PrintKeeper
{
public:
	class PrinterInfo
	{
		QString Name;
		QString Ip;
		QString MAC;

	public:
		PrinterInfo(const QString& _Name = "", const QString& _Ip = "", const QString& _MAC = "")
				: Name(_Name), Ip(_Ip), MAC(_MAC) {}
		QString name() const { return Name; }
		void setName(const QString& _Value) { Name = _Value; }
		QString ip() const { return Ip; }
		void setIp(const QString& _Value) { Ip = _Value; }
		QString mAC() const { return MAC; }
		void setMAC(const QString& _Value) { MAC = _Value; }
	};

	static QMessageBox* MessageBox;
public:
	//! \return true if we have access to print with _Printer.
	static bool printAcces(const QString& _PrinterName);
	static void clearPrinterInfo();
	static int printerInfoCount();
	static void setPrinterInfo(int _Index, const PrintKeeper::PrinterInfo& _Info);
	static PrinterInfo printerInfo(int _Index);
	static PrinterInfo printerInfo(const QString& _PrinterName);
	static bool isDisabled();
	static void setDisabled(bool _Value);
	static void accesDeniedWarning(const QString& _PrinterName);
};

#endif // PRINTKEEPER_H
