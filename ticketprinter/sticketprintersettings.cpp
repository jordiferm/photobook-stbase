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
#include "sticketprintersettings.h"
#include <QCoreApplication>

STicketPrinterSettings::STicketPrinterSettings(const QString& _PrinterName, QObject* _Parent) : SAppSettings(_Parent), PrinterName(_PrinterName)
{
	if (!keySaved(keyFullName("directprintmode")))
	{
 		addKey(keyFullName("directprintmode"),  true, QVariant::Bool, 
 				tr("Use Direct Print Mode"), "");
 		addKey(keyFullName("systemticketprintername"),  "", QVariant::String, 
 				tr("System Ticket Printer Name"), "");
		addKey(keyFullName("ticketreportfile"),  QCoreApplication::applicationDirPath() + "/reports/ticket.xml", QVariant::String,
 				tr("Ticket report file"), "");

		addKey(keyFullName("ticketdevice"), "/dev/ttyS0", QVariant::String, tr("Ticket printer device"), 
			tr("Ticket printer device"));

		addKey(keyFullName("ticketdevicetype"), PrinterTypeEscPOS, QVariant::Int, tr("Type"), 
			tr("Ticket printer device type"));

		addKey(keyFullName("boxconnector"), 0, QVariant::Int, tr("Box connector"), 
			tr("Box connector pin"));

		addKey(keyFullName("isdeviceprinter"), false, QVariant::Bool, tr("Device printer"),
			tr("Indicates if it is a linux device printer"));

		//SettingsModel->fetch();
	}
	
}


STicketPrinterSettings::~STicketPrinterSettings()
{
}

QString STicketPrinterSettings::keyFullName(const QString& _Key) const
{
	QString Res = "/ticketprinter/";
	if (!PrinterName.isEmpty())
		Res += PrinterName + "/";		 
			
	Res += _Key;
	return Res;
}	


bool STicketPrinterSettings::directPrintMode() const
{
	return value(keyFullName("directprintmode")).toBool(); 
}

QString STicketPrinterSettings::systemPrinterName() const
{
	return value(keyFullName("systemticketprintername")).toString(); 		
}

QString STicketPrinterSettings::ticketReportFile() const
{
	return value(keyFullName("ticketreportfile")).toString(); 		
}

QString STicketPrinterSettings::printTicketDevice() const
{
	return value(keyFullName("ticketdevice")).toString();
}

STicketPrinterSettings::TPrinterType STicketPrinterSettings::printTicketDeviceType() const
{
	return static_cast<STicketPrinterSettings::TPrinterType>(value(keyFullName("ticketdevicetype")).toInt());
}

QString STicketPrinterSettings::printBoxConnector() const
{
	return value(keyFullName("boxconnector")).toString();
}

bool STicketPrinterSettings::printIsDevicePrinter() const
{
	return value(keyFullName("isdeviceprinter")).toBool();
}


