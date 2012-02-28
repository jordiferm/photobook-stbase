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
#ifndef SPRINTERSETTINGS_H
#define SPRINTERSETTINGS_H

#include "sappsettings.h"
#include "stwidgetsexport.h"

/**
	@author
*/
class ST_WIDGETS_EXPORT SPrinterSettings : public SAppSettings
{
Q_OBJECT
public:
	static QString LabPrinterName;

private:
	QString PrinterName;

public:
	SPrinterSettings(const QString& _PrinterName = "default",  QObject* _Parent = 0);
	~SPrinterSettings();
	QString keyFullName(const QString& _Key) const;
	QString systemPrinterName() const; 
	bool hasToStore(const QString& _Product) const;
	QString printerToPrint(const QString& _Product) const;
	bool printerByProductEnabled() const;
	bool printerByFormatEnabled() const;
	static void clearProductPrinters();
	static void setProductPrinter(int _Index, const QString& _ProductId, const QString& _PrinterName);
	static int numProdPrinters();
	static QString prodId(int _Index);
	static QString printerName(int _Index);
	static QString printerForProduct(const QString& _ProdId); 
	static void clearFormatPrinters();
	static void setFormatPrinter(int _Index, const QString& _ProductId, const QString& _PrinterName);
	static int numFormatPrinters();
	static QString formatId(int _Index);
	static QString formatPrinterName(int _Index);
	static QString printerForFormat(const QString& _FormatId);

};

#endif
