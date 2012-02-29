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
#ifndef TICKETDATABASE_H
#define TICKETDATABASE_H

#include <QSqlDatabase>
#include "stdomexport.h"

/**
Database to store ticket information and to print ticket on ticketprinters.

	@author
*/
class STicketPrinter;
class PrinterESCPOS;

namespace STDom
{
class PublisherBill;
class ST_DOM_EXPORT TicketDatabase : public QSqlDatabase
{
public:

	static const QString DefaultTicketDBConnectionName; 
	QString TicketHeader; 
	QString TicketFooter; 
	QString CompanyName; 
	QString CompanyAddress; 
	QString OrderId; 

public:
	TicketDatabase(const QSqlDatabase& _Other = QSqlDatabase::database(DefaultTicketDBConnectionName));
	static QString dbFilePath();
	static QSqlDatabase addDatabase(const QString& _ConnectionName = DefaultTicketDBConnectionName); 
	static void removeDatabase(const QString& _ConnectionName = DefaultTicketDBConnectionName);
	void setOrderId(const QString& _Value) { OrderId = _Value; }
	void setTicketHeader(const QString& _Value) { TicketHeader = _Value; }	
	void setTicketFooter(const QString& _Value) { TicketFooter = _Value; }
	void setCompanyName(const QString& _Value) { CompanyName = _Value; }
	void setCompanyAddress(const QString& _Value) { CompanyAddress = _Value; }
	void createTables(); 
	void insertCompany(const QString& _Name, const QString& _Address, const QString& _Header, const QString& _Footer);
	void insertOrder(const QString& _OrderId, const QString& _Name, const QString& _Phone, double _Amount);
	void insertOrderLine(const QString& _Ref, const QString& _Description, int _Quantity, double _Price, double _Total);
	void printTicket(PrinterESCPOS& _Printer);
	void prepareForPrint(STicketPrinter& _Printer, const PublisherBill& _Bill );
};
}
#endif
