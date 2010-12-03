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

#include "orderdatabase.h"
#include <QObject>
#include <QSqlRecord>
#include "xmlorder.h"
#include "fsqlquery.h"
#include "ddoc.h"

using namespace STDom;

const QString OrderDatabase::DefaultOrderDBConnectionName = "OrderDatabase";

OrderDatabase::OrderDatabase(const QString& _DatabaseName): QSqlDatabase("QSQLITE")
{
	setDatabaseName(_DatabaseName);
}

OrderDatabase::OrderDatabase(const QSqlDatabase& _Other) : QSqlDatabase(_Other)
{}

/*!
	\param _Receiving On receiver side this parameter must be true.
*/

QString OrderDatabase::statusToStr(OrderDatabase::EnOrderStatus _Status, bool _Receiving)
{
	QString Res = QObject::tr("Unknown");
	switch(_Status)
	{
		case StatusUnknown :
			Res = QObject::tr("Unknown");
		break;
		case StatusSended :
			if (_Receiving)
				Res = QObject::tr("Received");
			else
				Res = QObject::tr("Sended");
		break;
		case StatusSending :
			if (_Receiving)
				Res = QObject::tr("Receiving");
			else
				Res = QObject::tr("Sending");
		break;
		case StatusInSpooler :
			Res = QObject::tr("In Spooler");
		break;
		case StatusPrinting :
			Res = QObject::tr("Printing");
		break;
		case StatusPrinted :
			Res = QObject::tr("Printed");
		break;
		case StatusPrintFailed :
			Res = QObject::tr("PrintFailed");
		break;
		case StatusCancelled :
			Res = QObject::tr("Cancelled");
		break;
		case StatusDeleted :
			Res = QObject::tr("Deleted");
		break;
	}
	return Res;
}

QString OrderDatabase::insertOrder(const STDom::XmlOrder& _Order,const STDom::PublisherDatabase& _PublisherDb)
{
	FSqlQuery Query(*this);
	Query.prepare("INSERT INTO orders(idorders,status,creationdatetime,"
			" senderorderid,senderid,senderaddress,sendername,senderphone,senderemail,sendercomment,"
			" customername,customeraddress,customerphone,customeremail) "
			"VALUES(:idorders,:status,:creationdatetime,  "
			" :senderorderid,:senderid,:senderaddress,:sendername,:senderphone,:senderemail,:sendercomment, "
			" :customername,:customeraddress,:customerphone,:customeremail)");
	//Query.bindValue(":idcommands", value("idcommands"));
	QString NewOrderId = FSqlQuery::sequenceNextVal("orders", "idorders").toString();
	Query.bindValue(":idorders", NewOrderId);
	Query.bindValue(":status", StatusSended);
	Query.bindValue(":creationdatetime", QDateTime::currentDateTime());

	STDom::XmlOrderDealer Customer = _Order.customer();
	Query.bindValue(":customeraddress", Customer.address());
	Query.bindValue(":customername", Customer.name());
	Query.bindValue(":customerphone", Customer.phone());
	Query.bindValue(":customeremail", Customer.email());

	STDom::XmlOrderDealer Sender = _Order.sender();
	Query.bindValue(":senderorderid", _Order.id() );
	Query.bindValue(":senderid",  Sender.id());
	Query.bindValue(":senderaddress", Sender.address());
	Query.bindValue(":sendername", Sender.name());
	Query.bindValue(":senderphone", Sender.phone());
	Query.bindValue(":senderemail", Sender.email());
	Query.bindValue(":sendercomment", _Order.senderComment());

	if (!Query.exec())
	{
		QSqlError LastErr = Query.lastError();
		Assert(LastErr.type() == QSqlError::NoError, Error(QString("Error: %1 running query %2 ").arg(LastErr.text()).arg(Query.executedQuery())));
	}

	QSqlRecord LineRecord = record("orderlines");
	STDom::XmlOrder::TProductPrints PrintList = _Order.prints();
	STDom::XmlOrder::TProductPrints::iterator it;
	for (it = PrintList.begin(); it != PrintList.end(); ++it)
	{
		LineRecord.setValue("orders_idorders", NewOrderId);
		LineRecord.setValue("idorderlines", FSqlQuery::sequenceNextVal("orderlines", "idorderlines"));
		STDom::DDocProduct Product = _PublisherDb.getProduct(it.key());
		int Quantity = it.value().size();
		LineRecord.setValue("price", _PublisherDb.priceOf(Product.ref(), Quantity));
		LineRecord.setValue("description", Product.description());
		LineRecord.setValue("quantity", Quantity);
		LineRecord.setValue("ref", Product.ref());
		Query.insert(LineRecord, "orderlines", *this);
	}

	return NewOrderId;
}
