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
#include "sticketprinter.h"
#include <QPrinter>
#include <QFile> 
#include <QDomDocument>
#include "parameter.h"
#include "openreports.h"

#include "qextserialport.h"
#include "printerescpos.h"

STicketPrinter::STicketPrinter() : DPrintDevice(0)
{
}

void STicketPrinter::configure(const STicketPrinterSettings& _Settings)
{
	setDirectPrint(_Settings.directPrintMode()); 
	setPrinterName(_Settings.systemPrinterName());
	setReportFile(_Settings.ticketReportFile());
	IsDevidePrinter = _Settings.printIsDevicePrinter();
	PTDeviceType = _Settings.printTicketDeviceType(); 
	PrintTiquetDevice = _Settings.printTicketDevice();

	if (directPrint())
		DirectPrintBuffer.open(QIODevice::ReadWrite); 	
}

void STicketPrinter::setParameter(const QString& _Name, const QVariant& _Value)
{
	ParamList.insert(_Name, _Value); 
	//ParamList.append(_Name.toLocal8Bit(), _Value); 
}

void STicketPrinter::print()
{
	if (DirectPrint)
	{
		QFile PrintFile;
		if (IsDevidePrinter)
		{
			PortSettings PortSettings;
			if (PTDeviceType == STicketPrinterSettings::PrinterTypeFastEscPOS)
				PortSettings.BaudRate = BAUD19200;
			else 
				PortSettings.BaudRate = BAUD9600;
		
			PortSettings.Parity = PAR_NONE;
			PortSettings.DataBits = DATA_8;
			PortSettings.StopBits = STOP_1;
			PortSettings.FlowControl = FLOW_OFF;
			PortSettings.Timeout_Sec = 0;
			PortSettings.Timeout_Millisec = 500;
			PrintFile.setFileName(PrintTiquetDevice);
			QextSerialPort SPort = QextSerialPort(PrintTiquetDevice.toLatin1() , PortSettings);
						SPort.open();
		}
		else 
			PrintFile.setFileName("/tmp/printer.txt");
	
		//Assert(PrintFile.open(QIODevice::WriteOnly | QIODevice::Truncate), 
		//		Error(QString(tr("Could not open printer defined file %1")).arg(PrintFile.fileName())));
		if (PrintFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			PrinterESCPOS Printer(&PrintFile);
			Printer.device()->write(DirectPrintBuffer.data(), DirectPrintBuffer.size()); 
			PrintFile.close();
		}
		else 
			qWarning(QObject::tr("Could not open printer defined file %1").arg(PrintFile.fileName()).toLatin1());
// 	if (Settings.printIsLprPrinter())
// 	{
// 		QStringList Args; 
// 		Args << "-P" << Settings.printPrinterName() << PrintFile.fileName();
// 		Assert(QProcess::execute("/usr/bin/lpr", Args) == 0, Error(tr("Error running lpr")));
// 	}

	}
	else 
	{
		QDomDocument doc;
		QString errMsg;
		int errLine, errColm;
		QString Filename(ReportFile);
		QFile file(Filename);
		Assert(doc.setContent(&file, &errMsg, &errLine, &errColm), Error(QObject::tr("Error Loading File"),
										QObject::tr("There was an error opening the file %1."
											"\n\n%2 on line %3 column %4.")
										.arg(Filename).arg(errMsg).arg(errLine).arg(errColm)));
	
		QDomElement root = doc.documentElement();
		Assert(root.tagName() == "report", Error(QObject::tr("Not a Valid File"),
										QObject::tr("The file %1 does not appear to be a valid file."
											"\nThe root node is not 'report'.").arg(Filename)));
	
		orReport Report(Database);
		Report.setDom(doc);
		ParameterList ORParList; 
		TParamList::iterator it; 
		for (it = ParamList.begin(); it != ParamList.end(); ++it)
		{
			ORParList.append(it.key().toLocal8Bit(), *it);
		}
		
		Report.setParamList(ORParList);
	
		QPrinter Printer;
		Printer.setPrinterName(PrinterName); 
		Assert(Report.isValid(), Error(QObject::tr("Error in ticket report definition"))); 
		Report.print(&Printer);
	}
}
