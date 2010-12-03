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
#ifndef PublisherDatabase_H
#define PublisherDatabase_H
#include "stdomexport.h"

#include <QSqlDatabase>
#include <QList> 
#include <QSqlRecord> 

/**
	Abstraction of a publisher bill.
	@author jordif.starblitz@gmail.com
	
*/
class FSqlDatabaseManager;
class QAbstractItemModel;
class QObject;
class QSqlTableModel;

namespace STDom
{

class XmlOrder;
class DDocProduct;
class DDocProductList;
class PrintJob;

class ST_DOM_EXPORT PublisherBill
{
public:	
	class PublisherBillLine
	{
		QString Ref; 
		QString Description; 
		int Quantity; 
		double Price; 

	public: 
		PublisherBillLine(const QString& _Ref, const QString& _Description, int _Quantity, double _Price);
		QString ref() const { return Ref; }
		QString description() const { return Description; }
		int quantity() const { return Quantity; }
		double price() const { return Price; }
		double total() const { return Price * Quantity; }
		QString ritchText() const; 
		QString plainText() const;
	};

	class PublisherShippingMethod
	{
		QString Description; 
		double Price; 

	public:
		PublisherShippingMethod(const QString& _Description = "", double _Price = 0) : Description(_Description), Price(_Price) {}
		QString description() const { return Description; }
		double price() const { return Price; }
		QString ritchText() const;
		QString plainText() const;
		bool isEmpty() const { return Description.isEmpty(); }
	};

public:
	typedef QList<PublisherBillLine> TLines;

private:
	TLines Lines;
	PublisherShippingMethod ShippingMethod;
	QString CustomerName; 
	QString CustomerPhone; 
	double TotalAmount; 
	

public:
	PublisherBill();
	void setShippingMethod(const PublisherShippingMethod& _Value) { ShippingMethod = _Value; }
	PublisherShippingMethod shippingMethod() const { return ShippingMethod; }
	void setCustomerName(const QString& _Value) { CustomerName = _Value; }
	QString customerName() const { return CustomerName; }
	void setCustomerPhone(const QString& _Value) { CustomerPhone = _Value; }
	QString customerPhone() const { return CustomerPhone; }
	void addLine(const PublisherBillLine& _Line);
	TLines lines() const { return Lines; }
	double totalAmount() const; 
	QString ritchText() const;
	QString plainText() const;
};


/**
	@author
*/
class ST_DOM_EXPORT PublisherDatabase : public QSqlDatabase
{
public:
	enum EnProductType
	{
		AllProducts = -1,
		DigiprintsProduct = 0,
		DecorationsProduct,
		PhotoBookProduct,
		CDRecordProduct,
		VideoProduct,
		GiftProduct,
		PhotoIndexProduct, 
		PhotoIdProduct
	};

public:
	static const QString DefaultPublisherDBConnectionName; 

public:
	PublisherDatabase(const QString& _DatabaseName);
	PublisherDatabase(const QSqlDatabase& _Other = QSqlDatabase::database(DefaultPublisherDBConnectionName));
	void importTable(const FSqlDatabaseManager& _SourceDBManager, const QString& _TableName, const QString& _ImportOrderBy = "");
	void importAll(const FSqlDatabaseManager& _SourceDBManager);
	QAbstractItemModel* newProductsModel(QObject* _Parent = 0, EnProductType _ProductType = AllProducts, const QString& _Filter = "") const;
	QSqlTableModel* newShippingMethodModel(QObject* _Parent = 0) const;
	DDocProduct getProduct(QAbstractItemModel*  _ProdModel, int _Row) const;
	DDocProduct getProduct(const QString& _ProductRef) const;
	//Adds database to QSqlDatabase active connections or replaces the old connection if a connection with _ConnectionName already exists.
	static QSqlDatabase addDatabase(const QString& _DatabaseName, const QString& _ConnectionName = DefaultPublisherDBConnectionName); 
	EnProductType productType(const QString& _ProductRef);
	QString billRitchText(const XmlOrder& _Order, const QSqlRecord& _ShippingMethod = QSqlRecord(), int _ImagesPerSheet = 1);
	PublisherBill calcBill(const XmlOrder& _Order, const QSqlRecord& _ShippingMethod = QSqlRecord(), int _ImagesPerSheet = 1);
	static int digiprintProdDisplayColumn();
	static int digiprintProdKeyColumn();
	double priceOf(const QString& _Ref, int _Quantity, bool _UseFixedPrice = true) const;
	static QString productSqlFilter(const DDocProductList& _Products);
	PrintJob getPrintJob(const XmlOrder& _Order) const;
};
} //namespace
#endif
