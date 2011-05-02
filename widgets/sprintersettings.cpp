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
#include "sprintersettings.h"

#if QT_VERSION >= 0x040400
 #include <QPrinterInfo> 
#endif 

QString SPrinterSettings::LabPrinterName = "*** Lab Printer ***";

SPrinterSettings::SPrinterSettings(const QString& _PrinterName, QObject* _Parent) : SAppSettings(_Parent), PrinterName(_PrinterName)
{
	if (!keySaved(keyFullName("systemprintername")))
	{
		QString DefaultPrinterName = ""; 
#if QT_VERSION >= 0x040400
		DefaultPrinterName = QPrinterInfo::defaultPrinter().printerName();
#endif 
		addKey(keyFullName("systemprintername"),  DefaultPrinterName, QVariant::String,
				tr("System Printer Name"), "");
		addKey(keyFullName("printerbyproductenabled"),  false, QVariant::Bool,
				tr("Set printer by product"), "");

		addKey(keyFullName("printerbyformatenabled"),  false, QVariant::Bool,
				tr("Set printer by format"), "");
	}
}


SPrinterSettings::~SPrinterSettings()
{
}

QString SPrinterSettings::keyFullName(const QString& _Key) const
{
	QString Res = "/printer/";
	if (!PrinterName.isEmpty())
		Res += PrinterName + "/";		 
			
	Res += _Key;
	return Res;
}	

QString SPrinterSettings::systemPrinterName() const
{
	return value(keyFullName("systemprintername")).toString(); 		
}

bool SPrinterSettings::hasToStore(const QString& _Product) const
{
	return (printerToPrint(_Product) == LabPrinterName);
}

QString SPrinterSettings::printerToPrint(const QString& _Product) const 
{
	QString Res = ""; 
	if (printerByProductEnabled())
		Res = printerForProduct(_Product);

	if (Res.isEmpty())
		Res = systemPrinterName(); 
	return Res; 
}

bool SPrinterSettings::printerByProductEnabled() const
{
	return value(keyFullName("printerbyproductenabled")).toBool(); 		
}

bool SPrinterSettings::printerByFormatEnabled() const
{
	return value(keyFullName("printerbyformatenabled")).toBool();
}

void SPrinterSettings::clearProductPrinters()
{
	QSettings Settings;
	Settings.remove("prodprinters");
}

void SPrinterSettings::setProductPrinter(int _Index, const QString& _ProductId, const QString& _PrinterName)
{
	QSettings Settings;
	Settings.beginWriteArray("prodprinters"); 
	Settings.setArrayIndex(_Index);
	Settings.setValue("productid", _ProductId); 
	Settings.setValue("printername", _PrinterName); 
	Settings.endArray();

// 	beginWriteArray(keyFullName("prodprinters")); 
// 	setArrayIndex(_Index);
// 	setValue("productid", _ProductId); 
// 	setValue("printername", _PrinterName); 
// 	endArray();
}

int SPrinterSettings::numProdPrinters()
{
	QSettings Settings;
	int Res = Settings.beginReadArray("prodprinters"); 
	Settings.endArray();
	return Res;
}

QString SPrinterSettings::prodId(int _Index)
{
	QSettings Settings;
	QString Res; 
	Settings.beginReadArray("prodprinters"); 
	Settings.setArrayIndex(_Index); 
	Res = Settings.value("productid").toString(); 
	Settings.endArray(); 
	return Res; 
}

QString SPrinterSettings::printerName(int _Index)
{
	QSettings Settings;
	QString Res; 
	Settings.beginReadArray("prodprinters"); 
	Settings.setArrayIndex(_Index); 
	Res = Settings.value("printername").toString(); 
	Settings.endArray(); 
	return Res; 
}

QString SPrinterSettings::printerForProduct(const QString& _ProdId)
{
	QSettings Settings;
	QString Res = ""; 
	int NumEntries = Settings.beginReadArray("prodprinters"); 
	bool Found = false; 
	int Cnt = 0; 
	while (!Found && Cnt < NumEntries)
	{
		Settings.setArrayIndex(Cnt);
		Found = (Settings.value("productid").toString() == _ProdId); 
		if (!Found)
			Cnt++;
		else 
			Res = Settings.value("printername").toString(); 
	}
	return Res; 
}

//---- Printers for formats

void SPrinterSettings::clearFormatPrinters()
{
	QSettings Settings;
	Settings.remove("formatprinters");
}

void SPrinterSettings::setFormatPrinter(int _Index, const QString& _ProductId, const QString& _PrinterName)
{
	QSettings Settings;
	Settings.beginWriteArray("formatprinters");
	Settings.setArrayIndex(_Index);
	Settings.setValue("formatid", _ProductId);
	Settings.setValue("printername", _PrinterName);
	Settings.endArray();
}

int SPrinterSettings::numFormatPrinters()
{
	QSettings Settings;
	int Res = Settings.beginReadArray("formatprinters");
	Settings.endArray();
	return Res;
}

QString SPrinterSettings::formatId(int _Index)
{
	QSettings Settings;
	QString Res;
	Settings.beginReadArray("formatprinters");
	Settings.setArrayIndex(_Index);
	Res = Settings.value("formatid").toString();
	Settings.endArray();
	return Res;
}

QString SPrinterSettings::formatPrinterName(int _Index)
{
	QSettings Settings;
	QString Res;
	Settings.beginReadArray("formatprinters");
	Settings.setArrayIndex(_Index);
	Res = Settings.value("printername").toString();
	Settings.endArray();
	return Res;
}

QString SPrinterSettings::printerForFormat(const QString& _FormatId)
{
	QSettings Settings;
	QString Res = "";
	int NumEntries = Settings.beginReadArray("formatprinters");
	bool Found = false;
	int Cnt = 0;
	while (!Found && Cnt < NumEntries)
	{
		Settings.setArrayIndex(Cnt);
		Found = (Settings.value("formatid").toString() == _FormatId);
		if (!Found)
			Cnt++;
		else
			Res = Settings.value("printername").toString();
	}
	return Res;
}




