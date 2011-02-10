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
#include <QUrl>
#include "stprintkeeperexport.h"
#include "pkprinterinfo.h"

class QMessageBox;
class EPrintKeeper;
class ST_PRINTKEEPER_EXPORT PrintKeeper
{
public:
	static QMessageBox* MessageBox;
	static EPrintKeeper* EPKeeper;
	static qlonglong CurrentCopies;


public:
	//-- EPrintKeeper Related --
	static void enableEPrintKeeper(bool _Remote);
	static void setRemoteUrl(const QUrl& _RemoteUrl);
	static bool login(const QString& _User, const QString& _Password);
	//! We need to login first.
	static bool isValidKey(const QString& _Key);
	static QString userInfo();
	static bool userInfoIsComplete();
	static QString errorInfo();
	static void setPrintAccessKey(const QString& _Key);


	//! \return true if we have access to print with _Printer.
	static bool printAcces(const QString& _PrinterName, qlonglong _Copies = 0);
	static void clearPrinterInfo();
	static int printerInfoCount();
	static void setPrinterInfo(int _Index, const PKPrinterInfo& _Info);
	static PKPrinterInfo printerInfo(int _Index);
	static PKPrinterInfo printerInfo(const QString& _PrinterName);
	static bool isDisabled();
	static void setDisabled(bool _Value);
	static void accesDeniedWarning(const QString& _PrinterName);
};

#endif // PRINTKEEPER_H
