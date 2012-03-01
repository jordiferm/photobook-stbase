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
#ifndef PublisherDatabase_H
#define PublisherDatabase_H
#include "stdomexport.h"

#include <QSqlDatabase>
#include <QList> 
#include <QSqlRecord> 
#include <QSize>
#include "sterror.h"

/**
	Abstraction of a publisher bill.
	@author jfernandez@ailiimage.com
	
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
		void setDescription(const QString& _Value) { Description = _Value; }
		double price() const { return Price; }
		void setPrice(double _Price) { Price = _Price; }
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
	ST_DECLARE_ERRORCLASS();
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
	void importNewProducts(const QSqlDatabase& _SourceDB);
	void importLocalFormats(const QSqlDatabase& _SourceDB);
	void importLocalProducts(const QSqlDatabase& _SourceDB);
	void importTable(const FSqlDatabaseManager& _SourceDBManager, const QString& _TableName, const QString& _ImportOrderBy = "");
	void importAll(const FSqlDatabaseManager& _SourceDBManager);
	QAbstractItemModel* newProductsTemplateModel(QObject* _Parent , EnProductType _ProductType, const QString& _TemplateRef) const;
	QAbstractItemModel* newProductsModel(QObject* _Parent = 0, EnProductType _ProductType = AllProducts, const QString& _Filter = "") const;
	QAbstractItemModel* newFormatsModel(QObject* _Parent = 0, const QString& _Filter = "") const;
	QSqlTableModel* newShippingMethodModel(QObject* _Parent = 0) const;
	DDocProduct getProduct(QAbstractItemModel*  _ProdModel, int _Row) const;
	DDocProduct getProduct(const QString& _ProductRef) const;
	//Adds database to QSqlDatabase active connections or replaces the old connection if a connection with _ConnectionName already exists.
	static QSqlDatabase addDatabase(const QString& _DatabaseName, const QString& _ConnectionName = DefaultPublisherDBConnectionName); 
	EnProductType productType(const QString& _ProductRef);
	//! Overloaded function provided for convenience.
	QString billRitchText(const PrintJob& _Job, const PublisherBill::PublisherShippingMethod& _ShippingMethod, int _ImagesPerSheet = 1);
	QString billRitchText(const XmlOrder& _Order, const PublisherBill::PublisherShippingMethod& _ShippingMethod = PublisherBill::PublisherShippingMethod(), int _ImagesPerSheet = 1);
	PublisherBill calcBill(const PrintJob& _Job, const PublisherBill::PublisherShippingMethod& _ShippingMethod= PublisherBill::PublisherShippingMethod(), int _ImagesPerSheet = 1);
	PublisherBill calcBill(const XmlOrder& _Order, const PublisherBill::PublisherShippingMethod& _ShippingMethod = PublisherBill::PublisherShippingMethod(), int _ImagesPerSheet = 1);
	static int digiprintProdDisplayColumn();
	static int digiprintProdKeyColumn();
	static int digiprintFormatDisplayColumn();
	static int digiprintFormatKeyColumn();
	double priceOf(const QString& _Ref, int _Quantity, bool _UseFixedPrice = true) const;
	static QString productSqlFilter(const DDocProductList& _Products);
	static QString templateSqlFilter(const QString& _TemplateRef);
	PrintJob getPrintJob(const XmlOrder& _Order) const;
	QAbstractItemModel* newProductsModelBySize(QObject* _Parent, PublisherDatabase::EnProductType _ProductType, const QSize& _Size) const;
	void deleteTemplateProductsByType(PublisherDatabase::EnProductType _Type);
	void deleteTemplateProducts();
	//returns true if database has some products.
	bool hasProducts() const;
	//!Exports products of one template
	void exportCSV(const QString& _AbsoluteFilePath, EnProductType _ProductType, const QString& _TemplateRef);
};
} //namespace
#endif
