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
#include "publisherdatabase.h"

#include <QSqlQueryModel> 
#include <QSqlTableModel> 
#include <QTextStream>
#include <QDebug>

#include "fsqlquery.h"
#include "fsqldatabasemanager.h"
#include "xmlorder.h"
#include "ddoc.h"
#include "printjob.h"

//Calc bill
#include "stutils.h" 
#include "shippingmethod.h"

using namespace STDom;
//_____________________________________________________________________________
//
// Class PublisherBillLine
//_____________________________________________________________________________

PublisherBill::PublisherBillLine::PublisherBillLine(const QString& _Ref, const QString& _Description, int _Quantity, double _Price)
	: Ref(_Ref), Description(_Description), Quantity(_Quantity), Price(_Price)
{
}


QString PublisherBill::PublisherBillLine::ritchText() const
{
	QString Res; 
	QTextStream Strm(&Res); 
	Strm << "<tr><td align=left>" << Ref << "</td><td align=left>" << Description << "</td><td align=right>" << Quantity << "</td><td align=right>" << STUtils::formatCurrency(Price) << "</td><td align=right>" << STUtils::formatCurrency(total()) << "</td></tr>";
	return Res; 
}

QString PublisherBill::PublisherBillLine::plainText() const
{
	QString Res;
	QTextStream Strm(&Res);
	Strm << qSetFieldWidth(10) << left << Ref << qSetFieldWidth(40) << left <<  Description;
	Strm << qSetFieldWidth(5) << right  << Quantity ;
	Strm << qSetFieldWidth(9) << right << STUtils::formatCurrency(Price) << qSetFieldWidth(9) << STUtils::formatCurrency(total()) << endl;
	return Res;


}

//_____________________________________________________________________________
//
// Class PublisherShippingMethod
//_____________________________________________________________________________

QString PublisherBill::PublisherShippingMethod::ritchText() const
{
	QString Res; 
	if (Price > 0)
	{
		QTextStream Strm(&Res); 
		Strm << "<tr><td>--SHIP--</td><td align=left>" << Description << "</td><td></td><td></td><td align=right>" << STUtils::formatCurrency(Price) << "</td></tr>";
	}
	return Res; 
}

QString PublisherBill::PublisherShippingMethod::plainText() const
{
	QString Res;
	if (Price > 0)
	{
		QTextStream Strm(&Res);
		Strm << "--SHIP--" << Description <<  qSetFieldWidth(10) << right << STUtils::formatCurrency(Price) << endl;
	}
	return Res;
}

//_____________________________________________________________________________
//
// Class PublisherBill
//_____________________________________________________________________________

PublisherBill::PublisherBill()
{
	TotalAmount = 0; 
}

void PublisherBill::addLine(const PublisherBillLine& _Line)
{
	Lines.push_back(_Line); 
	TotalAmount += _Line.total(); 
}

double PublisherBill::totalAmount() const
{
	return TotalAmount + ShippingMethod.price(); 
}

QString PublisherBill::plainText() const
{
	QString Res;
	QTextStream Strm(&Res);

	Strm << ::QObject::tr("Order Invoice") << endl;
	Strm << QObject::tr("Product. Description.            Quantity.    Price    Total") << endl;
	TLines::const_iterator it;
	for (it = Lines.begin(); it != Lines.end(); ++it)
	{
		Strm << it->plainText();
	}
	Strm << ShippingMethod.plainText() << endl;
	Strm << qSetFieldWidth(88) << right << "--------------" << endl;
	Strm << qSetFieldWidth(15) << left << QObject::tr("Total amount:") << qSetFieldWidth(62) << right << STUtils::formatCurrency(totalAmount()) << endl;

	return Res;
}

QString PublisherBill::ritchText() const
{
	QString Res; 
	QTextStream Strm(&Res); 
	
	Strm << ::QObject::tr("<h2>Order Invoice</h2>");
	Strm << QObject::tr("<table>");
	Strm << QObject::tr("<tr><th>Prod.</th><th>Descr.</th><th>Qnt.</th><th> Price</th><th>Total</th></tr>");
	TLines::const_iterator it; 
	for (it = Lines.begin(); it != Lines.end(); ++it)
	{
		Strm << it->ritchText(); 
	}
	Strm << ShippingMethod.ritchText(); 
	Strm << "<tr><td><b></b></td><td></td><td></td><td></td><td align=center>--------------</td></tr>";
	Strm << QObject::tr("<tr><td><b>Total amount:</b></td><td></td><td></td><td></td><td align=right>") << STUtils::formatCurrency(totalAmount()) << "</td></tr>";
	Strm << "</table>";

	return Res; 
}


//_____________________________________________________________________________
//
// Class PublisherDatabase
//_____________________________________________________________________________

const QString PublisherDatabase::DefaultPublisherDBConnectionName = "PublisherDatabase"; 

PublisherDatabase::PublisherDatabase(const QString& _DatabaseName): QSqlDatabase("QSQLITE")
{
	setDatabaseName(_DatabaseName); 
}

PublisherDatabase::PublisherDatabase(const QSqlDatabase& _Other) : QSqlDatabase(_Other)
{}


void PublisherDatabase::importLocalFormats(const QSqlDatabase& _SourceDB)
{
	FSqlQuery SourceDBQuery(_SourceDB);
	FSqlQuery DestQuery(*this);
	SourceDBQuery.exec("SELECT * FROM formats WHERE localadded!=0");
	while(SourceDBQuery.next())
	{
		DestQuery.prepareInsert(SourceDBQuery.record(), "formats");
		DestQuery.exec();
	}
}


//TODO: Not tested, please test it
void PublisherDatabase::importNewProducts(const QSqlDatabase& _SourceDB)
{
	FSqlQuery SourceDBQuery(_SourceDB);
	FSqlQuery DestQuery(*this);
	SourceDBQuery.exec("SELECT * FROM products");
	while(SourceDBQuery.next())
	{
		QSqlRecord SourceRecord = SourceDBQuery.record();
		//Check if we have this product:
		DestQuery.prepare("SELECT * FROM products WHERE ref=:ref");
		DestQuery.bindValue(":ref", SourceRecord.value("ref"));
		DestQuery.exec();

		if (!DestQuery.next())
		{
			//Get the Source Format
			FSqlQuery SourceFormatQuery(_SourceDB);
			SourceFormatQuery.prepare("SELECT * FROM formats WHERE idformats=:idformats");
			SourceFormatQuery.bindValue("idformats", SourceRecord.value("formats_idformats"));
			SourceFormatQuery.exec();
			Assert(SourceFormatQuery.next(), Error(QObject::tr("Error getting product format on product: %1").arg(SourceRecord.value("ref").toString())));

			//Check if we have a format like source product.
			QVariant IdFormats;
			DestQuery.prepare("SELECT idformats FROM formats WHERE width=:width AND height=:height");
			DestQuery.bindValue(":width", SourceFormatQuery.record().value("width"));
			DestQuery.bindValue(":height", SourceFormatQuery.record().value("height"));

			if (!DestQuery.next()) //Lets insert a new format
			{
				DestQuery.prepare("INSERT INTO formats(idformats, description, width, height) "
							   "VALUES( :idformats, :description, :width, :height)");
				IdFormats = FSqlQuery::sequenceNextVal("formats", "idformats", *this);
				DestQuery.bindValue(":idformats", IdFormats);
				DestQuery.bindValue(":description", SourceFormatQuery.record().value("description"));
				DestQuery.bindValue(":width", SourceFormatQuery.record().value("width"));
				DestQuery.bindValue(":height", SourceFormatQuery.record().value("height"));
				DestQuery.exec();
			}
			else
				IdFormats = DestQuery.value(0);


			QSqlRecord RecProduct = DestQuery.database().record("products");
			RecProduct.setValue("ref", SourceRecord.value("ref"));
			RecProduct.setValue("description", SourceRecord.value("description"));
			RecProduct.setValue("fixedprice", SourceRecord.value("fixedprice"));
			RecProduct.setValue("label", SourceRecord.value("description"));
			RecProduct.setValue("templates_ref", SourceRecord.value("name"));
			RecProduct.setValue("formats_idformats", IdFormats);
			RecProduct.setValue("type", SourceRecord.value("type"));
			DestQuery.prepareInsert(RecProduct, "products");
			DestQuery.exec();
		}
	}
}


void PublisherDatabase::importLocalProducts(const QSqlDatabase& _SourceDB)
{
	FSqlQuery SourceDBQuery(_SourceDB);
	FSqlQuery DestQuery(*this);
	SourceDBQuery.exec("SELECT * FROM products WHERE localadded!=0");
	while(SourceDBQuery.next())
	{
		DestQuery.prepareInsert(SourceDBQuery.record(), "products");
		DestQuery.exec();
	}
}

void PublisherDatabase::importTable(const FSqlDatabaseManager& _SourceDBManager, const QString& _TableName, const QString& _ImportOrderBy)
{
	QSqlDatabase SourceDB = _SourceDBManager.database(); 
	FTableMetaData  NewTMetadata = _SourceDBManager.tableMetaData(_TableName);
	FSqlQuery PublDBQuery(*this); 
	if (tables().contains(_TableName))
	{
		PublDBQuery.exec("DROP table " + _TableName); 
	}
	FSqlSqliteDbTableManager TManager(NewTMetadata, *this);
	TManager.create(); 
	PublDBQuery.exec("DELETE from " + _TableName); //Delete aftercreate inserts

	FSqlQuery SourceQuery(SourceDB); 
	QString ImportOrderBy; 
	if (!_ImportOrderBy.isEmpty())
		ImportOrderBy = " ORDER BY " + _ImportOrderBy; 
	SourceQuery.exec("SELECT * FROM " + _TableName + ImportOrderBy); 
	while (SourceQuery.next())
	{
		PublDBQuery.prepareInsert(SourceQuery.record(), _TableName); 
		PublDBQuery.exec(); 
	}
}

void PublisherDatabase::importAll(const FSqlDatabaseManager& _SourceDBManager)
{
	importTable(_SourceDBManager, "formats");
	importTable(_SourceDBManager, "products");
	importTable(_SourceDBManager, "productprices"); 
	importTable(_SourceDBManager, "shippingmethods"); 
	importTable(_SourceDBManager, "templates");
}


QAbstractItemModel* PublisherDatabase::newProductsTemplateModel(QObject* _Parent, PublisherDatabase::EnProductType _ProductType, const QString& _TemplateRef) const
{
	QString Filter = QString("templates_ref='%1'").arg(_TemplateRef);
	return newProductsModel(_Parent, _ProductType, Filter);
}

QAbstractItemModel* PublisherDatabase::newProductsModel(QObject* _Parent, PublisherDatabase::EnProductType _ProductType, const QString& _Filter) const
{
	QSqlQueryModel* Res = new QSqlQueryModel(_Parent);
	QString Sql = "SELECT label, ref, ordering FROM products";
	QString Filter = _Filter; 


	if (_ProductType != PublisherDatabase::AllProducts)
	{
		if (!Filter.isEmpty())
			Filter += " AND "; 
		Filter += QString("type=%1").arg(_ProductType);
	}

	if (!Filter.isEmpty())
		Sql += " WHERE " + Filter;
	
	Sql += " ORDER BY ordering, label";
	//qDebug(Sql.toLatin1());
	Res->setQuery(Sql, *this);
	return Res; 
}

QAbstractItemModel* PublisherDatabase::newFormatsModel(QObject* _Parent, const QString& _Filter) const
{
	QSqlQueryModel* Res = new QSqlQueryModel(_Parent);
	QString Sql = "SELECT description, idformats FROM formats";
	QString Filter = _Filter;

	if (!Filter.isEmpty())
		Sql += " WHERE " + Filter;

	Sql += " ORDER BY description";
	Res->setQuery(Sql, *this);
	return Res;
}

QSqlTableModel* PublisherDatabase::newShippingMethodModel(QObject* _Parent) const
{
	QSqlTableModel* Model = new QSqlTableModel(_Parent, *this); 
	Model->setTable("shippingmethods"); 
	Model->select(); 
	return Model; 
}


DDocProduct PublisherDatabase::getProduct(QAbstractItemModel*  _ProdModel, int _Row) const
{
	QModelIndex ProdIndex = _ProdModel->index(_Row, 1); 
	QString ProdRef = _ProdModel->data(ProdIndex).toString(); 
	return getProduct(ProdRef);
}

DDocProduct PublisherDatabase::getProduct(const QString& _ProductRef) const
{
	DDocProduct Res;
	FSqlQuery Query(*this);
	Query.prepare("SELECT formats.width, formats.height, products.label, products.ordering, formats.idformats "
				  "FROM products INNER JOIN formats ON  products.formats_idformats = formats.idformats "
				  "WHERE ref=:ref");
	Query.bindValue(":ref", _ProductRef);
	Query.exec();
	if (Query.next())
	{
		Res = DDocProduct(_ProductRef, Query.value(2).toString(), DDocFormat(Query.value(0).toInt(), Query.value(1).toInt(), Query.value(4).toString()), Query.value(3).toInt());
	}
	return Res;
}

QSqlDatabase PublisherDatabase::addDatabase(const QString& _DatabaseName, const QString& _ConnectionName)
{
	return QSqlDatabase::cloneDatabase(PublisherDatabase(_DatabaseName), _ConnectionName); 
}

PublisherDatabase::EnProductType PublisherDatabase::productType(const QString& _Ref)
{
	PublisherDatabase::EnProductType Res = PublisherDatabase::AllProducts;
	FSqlQuery Query(*this);

	Query.prepare("SELECT type FROM products WHERE ref=:ref");
	Query.bindValue(":ref", _Ref);
	Query.exec();
	if (Query.next())
		Res = static_cast<PublisherDatabase::EnProductType>(Query.value(0).toInt());
	return Res;
}

QString PublisherDatabase::billRitchText(const PrintJob& _Job, const PublisherBill::PublisherShippingMethod& _ShippingMethod, int _ImagesPerSheet)
{
	XmlOrder Order;
	_Job.addOrderPrints(Order);
	return billRitchText(Order, _ShippingMethod, _ImagesPerSheet);
}

QString PublisherDatabase::billRitchText(const XmlOrder& _Order, const PublisherBill::PublisherShippingMethod& _ShippingMethod, int _ImagesPerSheet)
{
	PublisherBill Bill = calcBill(_Order, _ShippingMethod, _ImagesPerSheet);
	return Bill.ritchText();
}


PublisherBill PublisherDatabase::calcBill(const PrintJob& _Job, const PublisherBill::PublisherShippingMethod& _ShippingMethod, int _ImagesPerSheet)
{
	XmlOrder Order;
	_Job.addOrderPrints(Order);
	return calcBill(Order, _ShippingMethod, _ImagesPerSheet);
}

/*!
\return Bill in Ritch Text format.
*/
PublisherBill PublisherDatabase::calcBill(const XmlOrder& _Order, const PublisherBill::PublisherShippingMethod& _ShippingMethod, int _ImagesPerSheet)
{
	PublisherBill Res; 
	STDom::XmlOrder::TProductPrints ProductPrints = _Order.prints();
	STDom::XmlOrder::TProductPrints::const_iterator it;
	for (it = ProductPrints.begin(); it != ProductPrints.end(); ++it)
	{
		STDom::DDocProduct CProduct = getProduct(it.key());
		STDom::XmlOrder::TPrintList CurrentPrintList = it.value();
		STDom::XmlOrder::TPrintList::const_iterator lit;
		int CNCopies = 0;
		qulonglong DiskSize = 0;
		for (lit = CurrentPrintList.begin(); lit != CurrentPrintList.end(); ++lit)
		{
			CNCopies += lit->printsRequested();
			DiskSize+= lit->imageSize();
		}


		PublisherDatabase::EnProductType ProdType = productType(CProduct.ref());
		switch (ProdType)
		{
			case PublisherDatabase::PhotoBookProduct :
				{
					int NPages = CurrentPrintList.size() -1;
					if (NPages > 1)
						CNCopies = CNCopies / NPages;
					double PagePrice = 0; //Price depends on command type.
					double Price = 0;
					FSqlQuery Query(*this);
					Query.prepare("SELECT fixedprice FROM products WHERE ref=:ref");
					Query.bindValue(":ref", CProduct.ref());
					Query.exec();
					if (Query.next())
					{
						double PagePrice = 0; //Price depends on command type.
						PagePrice = priceOf(CProduct.ref(), NPages, false);
						Price = Query.value(0).toDouble() + PagePrice * NPages;
					}
					PublisherBill::PublisherBillLine Line(CProduct.ref(),
						CProduct.description() + " - " + QObject::tr("with ") + QString::number(NPages) + QObject::tr(" Pages "), CNCopies, Price);
					Res.addLine(Line);
				}
			break;
			case  PublisherDatabase::PhotoIndexProduct :
				{
					int NumImages = CNCopies;
					CNCopies = (NumImages/ _ImagesPerSheet);
					if (NumImages % _ImagesPerSheet > 0)
						CNCopies++;

					double Price = 0; //Price depends on command type.
					Price = priceOf(CProduct.ref(), CNCopies);

					PublisherBill::PublisherBillLine Line(CProduct.ref(), CProduct.description(), CNCopies, Price);
					Res.addLine(Line);
				}
			break;
			case  PublisherDatabase::CDRecordProduct :
				{
					CNCopies = CNCopies / CurrentPrintList.size();
					int ProdDiskSize = DiskSize / (1024 * 1024);
					double Price = priceOf(CProduct.ref(), ProdDiskSize);
					PublisherBill::PublisherBillLine Line(CProduct.ref(),
						CProduct.description() + " - " + QString::number(ProdDiskSize) + QObject::tr(" (MBytes) "), CNCopies, Price);
					Res.addLine(Line);
				}
			break;
			default:
				{
					double Price = 0; //Price depends on command type.
					Price = priceOf(CProduct.ref(), CNCopies);

					PublisherBill::PublisherBillLine Line(CProduct.ref(), CProduct.description(), CNCopies, Price);
					Res.addLine(Line);
				}
		}
	} // For each product.
	if (!_ShippingMethod.isEmpty())
	{
		Res.setShippingMethod(_ShippingMethod);
	}
	return Res; 
}


int PublisherDatabase::digiprintProdDisplayColumn()
{
	return 0; 
}

int PublisherDatabase::digiprintProdKeyColumn()
{
	return 1;
}

int PublisherDatabase::digiprintFormatDisplayColumn()
{
	return 0;
}

int PublisherDatabase::digiprintFormatKeyColumn()
{
	return 1;
}


//! \return 0 if no price is found

double PublisherDatabase::priceOf(const QString& _Ref, int _Quantity, bool _UseFixedPrice ) const
{
	double Res = 0;
	FSqlQuery Query(*this);
	
	if (_UseFixedPrice)
	{
		Query.prepare("SELECT fixedprice FROM products WHERE ref=:ref");
		Query.bindValue(":ref", _Ref);
		Query.exec();
		if (Query.next())
			Res = Query.value(0).toDouble();
	}
	
	Query.prepare("SELECT min(quantity) FROM productprices WHERE " 
		"quantity >= :quantity and products_ref=:products_ref "
		"GROUP BY products_ref;");
	
	Query.bindValue(":products_ref", _Ref);
	Query.bindValue(":quantity", _Quantity);
	Query.exec();
	QVariant QuantityBound;
	if (Query.next())
	{
		QuantityBound = Query.value(0);
	}
	else 
	{
		Query.prepare("SELECT max(quantity) FROM productprices WHERE " 
		 "products_ref=:products_ref GROUP BY products_ref");
		Query.bindValue(":products_ref", _Ref);
		Query.exec();
		if (Query.next())
			QuantityBound = Query.value(0);
	}	
	
	Query.prepare("SELECT price FROM productprices WHERE products_ref=:products_ref AND "
		" quantity=:quantity");
	Query.bindValue(":products_ref", _Ref);
	Query.bindValue(":quantity", QuantityBound);
	Query.exec();
	if (Query.next())
		Res = Query.value(0).toDouble();
	
	return Res;
}

QString PublisherDatabase::productSqlFilter(const DDocProductList& _Products)
{
	QString Res; 
	DDocProductList::const_iterator it;
	for (it = _Products.begin(); it != _Products.end(); ++it)
	{
		if (!Res.isEmpty())
			Res += " OR "; 
		Res += QString("ref='%1'").arg(it->ref());
	}
	return Res; 
}

QString PublisherDatabase::templateSqlFilter(const QString& _TemplateRef)
{
		QString Res;
		if (!_TemplateRef.isEmpty())
				Res = QString(" templates_ref='%1'").arg(_TemplateRef);
		return Res;
}

PrintJob PublisherDatabase::getPrintJob(const XmlOrder& _Order) const
{
	PrintJob Res;

	XmlOrder::TProductPrints ProductPrints = _Order.prints();
	XmlOrder::TProductPrints::const_iterator it;
	for (it = ProductPrints.begin(); it != ProductPrints.end(); ++it)
	{
		DDocProduct CurrentProduct = getProduct(it.key());
		XmlOrder::TPrintList Prints = it.value();
		XmlOrder::TPrintList::const_iterator pit;
		for (pit = Prints.begin(); pit != Prints.end(); ++pit)
		{
			XmlOrderPrint CurrentPrint = *pit;
                        DDocPrint NewPrint(QFileInfo(QDir(_Order.orderInfo().orderPath()).absoluteFilePath(CurrentPrint.src())), CurrentProduct, CurrentPrint.printsRequested());
			Res.addPrint(NewPrint);
		}
	}

	return Res;
}

QAbstractItemModel* PublisherDatabase::newProductsModelBySize(QObject* _Parent, PublisherDatabase::EnProductType _ProductType, const QSize& _Size) const
{
	QSqlQueryModel* Res = new QSqlQueryModel(_Parent);
	QString Sql = "SELECT products.label, products.ref "
				  "FROM products INNER JOIN formats ON  products.formats_idformats = formats.idformats ";
	QString Filter;

	if (_ProductType != PublisherDatabase::AllProducts)
	{
			if (!Filter.isEmpty())
					Filter += " AND ";
			Filter += QString("type=%1").arg(_ProductType);
	}

	Sql += " WHERE " + Filter;
	Sql += QString(" AND ((formats.width=%1 AND formats.height=%2) OR (formats.width=%2 AND formats.height=%1))").arg(_Size.width()).arg(_Size.height());
	Sql += " ORDER BY ordering, ref";

	Res->setQuery(Sql, *this);
	return Res;
}

void PublisherDatabase::deleteTemplateProductsByType(PublisherDatabase::EnProductType _Type)
{
		FSqlQuery Query;
		Query.prepare("DELETE FROM productprices WHERE products_ref IN (SELECT ref FROM products WHERE type=:type)");
		Query.bindValue(":type", _Type);
		Query.exec();
		Query.prepare("DELETE FROM products WHERE type=:type");
		Query.bindValue(":type", _Type);
		Query.exec();
}

void PublisherDatabase::deleteTemplateProducts()
{
		deleteTemplateProductsByType(PublisherDatabase::DecorationsProduct);
		deleteTemplateProductsByType(PublisherDatabase::PhotoBookProduct);
		deleteTemplateProductsByType(PublisherDatabase::PhotoIdProduct);
}

void PublisherDatabase::exportCSV(const QString& _AbsoluteFilePath, EnProductType _ProductType, const QString& _TemplateRef)
{
	QFile OutFile(_AbsoluteFilePath);
	Assert(OutFile.open(QFile::WriteOnly | QFile::Truncate), Error(QObject::tr("Error creating file %1").arg(_AbsoluteFilePath)));

	QTextStream Strm(&OutFile);

	QAbstractItemModel* TModel = newProductsTemplateModel(0, _ProductType, _TemplateRef);
	for (int Vfor = 0; Vfor < TModel->rowCount(); Vfor++)
	{
		QStringList StrList;
		DDocProduct CProduct = getProduct(TModel, Vfor);
		StrList << CProduct.ref();
		StrList << CProduct.description();
		StrList << QString::number(_ProductType);
		StrList << QString::number(CProduct.format().width());
		StrList << QString::number(CProduct.format().height());
		StrList << QString::number(CProduct.ordering());

		FSqlQuery Query(*this);
		Query.prepare("SELECT fixedprice FROM products WHERE ref=:ref");
		Query.bindValue(":ref", CProduct.ref());
		Query.exec();
		if (Query.next())
			StrList << Query.value(0).toString();
		else
			StrList << "0";

		//Now all product prices
		Query.prepare("SELECT price, quantity FROM productprices WHERE "
			"products_ref=:products_ref "
			"ORDER BY quantity;");
		Query.bindValue(":products_ref", CProduct.ref());
		Query.exec();
		while (Query.next())
		{
			StrList << Query.value(0).toString() + ":" + Query.value(1).toString();
		}

		//New Line to file
		Strm << StrList.join(";") + "\n";
	}
	OutFile.close();
	delete TModel;
}
