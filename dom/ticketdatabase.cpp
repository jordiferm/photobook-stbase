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
#include "ticketdatabase.h"
#include <QDir> 
#include <QSqlRecord> 

#include "fsqlquery.h" 
#include "publisherdatabase.h"


//Tiquet 
#include "sticketprinter.h" 
#include "printerescpos.h"
#include "stutils.h" 

using namespace STDom;

const QString TicketDatabase::DefaultTicketDBConnectionName = "TicketDatabase";

TicketDatabase::TicketDatabase(const QSqlDatabase& _Other) : QSqlDatabase (_Other)
{}

QString TicketDatabase::dbFilePath()
{
	return QDir::temp().absoluteFilePath("tmpsttiquet.db");
}

QSqlDatabase TicketDatabase::addDatabase(const QString& _ConnectionName)
{
	QSqlDatabase Res = QSqlDatabase::addDatabase("QSQLITE", _ConnectionName); 
	Res.setDatabaseName(dbFilePath()); 
	return Res; 
	
}

void TicketDatabase::removeDatabase(const QString& _ConnectionName)
{
	QSqlDatabase::removeDatabase(_ConnectionName); 
}

void TicketDatabase::createTables()
{
	FSqlQuery Query(*this);
	if (tables().contains("company"))
		Query.exec("DROP TABLE company"); 

	if (tables().contains("orders"))
		Query.exec("DROP TABLE orders"); 

	if (tables().contains("orderlines"))
		Query.exec("DROP TABLE orderlines"); 

	Query.exec("CREATE TABLE IF NOT EXISTS company (idcompany INTEGER  PRIMARY KEY AUTOINCREMENT, "
					"name TEXT, address TEXT, header TEXT, footer TEXT)");
	Query.exec("CREATE TABLE IF NOT EXISTS orders (idorders INTEGER  PRIMARY KEY AUTOINCREMENT, "
					"orderid TEXT, processdate TEXT, name TEXT, phone TEXT, amount REAL)");
	Query.exec("CREATE TABLE IF NOT EXISTS orderlines (idorderlines INTEGER  PRIMARY KEY AUTOINCREMENT, "
					"ref TEXT, description TEXT, quantity INTEGER, price REAL, total REAL)");
}


void TicketDatabase::insertCompany(const QString& _Name, const QString& _Address, const QString& _Header, const QString& _Footer)
{
	FSqlQuery Query(*this);
	QSqlRecord RecOrder = record("company"); 
	RecOrder.setValue("name", _Name); 
	RecOrder.setValue("address", _Address); 
	RecOrder.setValue("header", _Header); 
	RecOrder.setValue("footer", _Footer); 
	Query.prepareInsert(RecOrder, "company"); 
	Query.exec(); 
}

void TicketDatabase::insertOrder(const QString& _OrderId, const QString& _Name, const QString& _Phone, double _Amount)
{
	FSqlQuery Query(*this);
	QSqlRecord RecOrder = record("orders"); 
	RecOrder.setValue("processdate", QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")); 
	RecOrder.setValue("orderid", _OrderId); 
	RecOrder.setValue("name", _Name); 
	RecOrder.setValue("phone", _Phone); 
	RecOrder.setValue("amount", _Amount); 
	Query.prepareInsert(RecOrder, "orders"); 
	Query.exec(); 
}

void TicketDatabase::insertOrderLine(const QString& _Ref, const QString& _Description, int _Quantity, double _Price, double _Total)
{
	FSqlQuery Query(*this);
	QSqlRecord RecOrder = record("orderlines"); 
	RecOrder.setValue("ref", _Ref); 
	RecOrder.setValue("description", _Description); 
	RecOrder.setValue("quantity", _Quantity); 
	RecOrder.setValue("price", _Price); 
	RecOrder.setValue("total", _Total); 
	Query.prepareInsert(RecOrder, "orderlines"); 
	Query.exec(); 
}

void TicketDatabase::printTicket(PrinterESCPOS& _Printer)
{
	FSqlQuery Query(*this);
	_Printer.cmdFontSize(0,0);
	_Printer.string(TicketHeader);
	
	_Printer.cmdFontSize(1,1);
	_Printer.string(CompanyName + "\n");
	_Printer.cmdFontSize(0,0);
	_Printer.cmdLineFeed();
	_Printer.cmdFontSize(0,1);
 	
	_Printer.string(CompanyAddress + "\n");
	_Printer.cmdFontSize(0,0);
	_Printer.cmdLineFeed();
// 	Printer.string(Settings.accountCompanyCity() + "\n");
	_Printer.string(QObject::tr("Date:") + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm") + "\n");
	
	QSqlRecord RecOrder; 
	Query.exec("SELECT * FROM orders");	
	if (Query.next())
		RecOrder = Query.record(); 

	_Printer.string(QObject::tr("Name: ") + RecOrder.value("name").toString() + "\n");
	_Printer.string(QObject::tr("Phone: ") + RecOrder.value("phone").toString() + "\n");
	_Printer.cmdLineFeedn(2);
	_Printer.string(QString(QObject::tr("Artic.")).leftJustified(7,' '));
	_Printer.string(QString(QObject::tr("Descr.")).leftJustified(15,' '));
	_Printer.string(QString(QObject::tr("Quant")).rightJustified(4,' '));
	_Printer.string(QString(QObject::tr("Price")).rightJustified(6,' '));
	_Printer.string(QString(QObject::tr("Total")).rightJustified(6,' ') + "\n");
	_Printer.string(QString().fill('-', 40) + "\n");
	
	Query.exec("SELECT * FROM orderlines ");
	double Total = 0;	
	while (Query.next())
	{
		QSqlRecord RecLine = Query.record();
		
 		double Pvp = RecLine.value("price").toDouble();
 		double Import = RecLine.value("total").toDouble();
		QString Refefence = RecLine.value("ref").toString();
 		QString Description = RecLine.value("description").toString();
 
 		_Printer.string(Refefence.leftJustified(7));
		
 		_Printer.string(Description.leftJustified(15));
 		if (Description.length() > 15) 
 			_Printer.string("\n" + QString().fill(' ',22));
 		
		_Printer.string(QString::number(RecLine.value("quantity").toDouble()).rightJustified(4)); 
 		_Printer.string(STUtils::formatCurrency(Pvp).rightJustified(6));
 		_Printer.string(STUtils::formatCurrency(Import).rightJustified(6) + "\n");
		
	}
 	_Printer.cmdLineFeed();
	_Printer.cmdFontSize(0,1);
	_Printer.string(QString(QObject::tr("Total :")).leftJustified(15) + STUtils::formatCurrency(RecOrder.value("amount")).rightJustified(25) + "\n");
	
	_Printer.cmdLineFeedn(2);
 
	_Printer.string(QObject::tr("Order: ") + OrderId + "\n");
	_Printer.cmdBarCodePrint(PrinterESCPOS::BCTypeEan13, OrderId, 70);
 	_Printer.cmdLineFeedn(2);
	
	_Printer.string(TicketFooter);
	_Printer.cmdLineFeedn(6);
	
	_Printer.cmdCut();
}

void TicketDatabase::prepareForPrint(STicketPrinter& _Printer, const PublisherBill& _Bill )
{
	insertCompany(CompanyName, CompanyAddress, TicketHeader, TicketFooter); 
	insertOrder(OrderId, _Bill.customerName(), _Bill.customerPhone(), _Bill.totalAmount()); 

	PublisherBill::TLines Lines = _Bill.lines();
	PublisherBill::TLines::const_iterator it;
	for (it = Lines.begin(); it != Lines.end(); ++it)
	{
		insertOrderLine(it->ref(), it->description(), it->quantity(), it->price(), it->total()); 
	}
	if (!_Bill.shippingMethod().isEmpty())
		insertOrderLine("", _Bill.shippingMethod().description(), 1, _Bill.shippingMethod().price(), _Bill.shippingMethod().price());

	if (_Printer.directPrint())
	{
		PrinterESCPOS ESCPrinter(_Printer.directPrintBufferDevice());
		printTicket(ESCPrinter); 
	}
	else 
	{
		_Printer.setDatabase(*this); 
	}
}

