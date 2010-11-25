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
#ifndef STICKETPRINTER_H
#define STICKETPRINTER_H
#include <QMap> 
#include <QBuffer> 
#include <QSqlDatabase>
#include "stticketprinterexport.h"
#include "sticketprintersettings.h"
#include "sterror.h"

/**
	@author
*/
class ST_TICKETPRINTER_EXPORT STicketPrinter
{
public:
	ST_DECLARE_ERRORCLASS();
	
private:
	QIODevice* DPrintDevice;
	QString ReportFile; 
	QString PrinterName; 
	QBuffer DirectPrintBuffer;
	bool DirectPrint; 
	typedef QMap<QString, QVariant> TParamList;
	TParamList ParamList; 
	bool IsDevidePrinter;
	STicketPrinterSettings::TPrinterType PTDeviceType;
	QString PrintTiquetDevice;
	QSqlDatabase Database; 
	
public:
	STicketPrinter();
	void configure(const STicketPrinterSettings& _Settings);
	void setPrinterName(const QString& _Name) { PrinterName = _Name; }
	void setDirectPrintDevice(QIODevice* _Device) { DPrintDevice = _Device; }
	void setReportFile(const QString& _ReportFile) { ReportFile = _ReportFile; }
	void setDirectPrint(bool _DirectPrint) { DirectPrint = _DirectPrint; }
	bool directPrint() const { return DirectPrint; }
	QIODevice* directPrintBufferDevice() { return &DirectPrintBuffer; }
	void setParameter(const QString& _Name, const QVariant& _Value); 
	void setDatabase(QSqlDatabase _Database ) { Database = _Database; } 
	void print(); 
};

#endif
