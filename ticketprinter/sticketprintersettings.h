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
#ifndef STICKETPRINTERSETTINGS_H
#define STICKETPRINTERSETTINGS_H

#include "sappsettings.h"
#include "stticketprinterexport.h"

/**
	@author
*/
class ST_TICKETPRINTER_EXPORT STicketPrinterSettings : public SAppSettings
{
public: 
	typedef enum EnPrinterType
	{
		PrinterTypeEscPOS,
		PrinterTypeFastEscPOS
	} TPrinterType;

	
private:
Q_OBJECT
	QString PrinterName;

public:
	STicketPrinterSettings(const QString& _PrinterName = "default",  QObject* _Parent = 0);
	~STicketPrinterSettings();
	QString keyFullName(const QString& _Key) const;
	//void setDirectPrintMode(bool _PrintMode); 
	bool directPrintMode() const; 
	//void setSystemPrinterName(const QString& _Key); 
	QString systemPrinterName() const; 
	QString ticketReportFile() const; 
	QString printTicketDevice() const;
	TPrinterType printTicketDeviceType() const;
 	QString printBoxConnector() const;
 	bool printIsDevicePrinter() const;
};

#endif
