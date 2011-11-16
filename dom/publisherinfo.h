/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PUBLISHERINFO_H
#define PUBLISHERINFO_H

#include "iddesctablemodel.h"
#include <QMap>
#include "stdomexport.h"
#include "publisherdatabase.h"
#include "publisher.h"
#include "paymenttype.h"
#include "collectionpoint.h"
#include "shippingmethod.h"

namespace STDom
{

/**
	@author Shadow
*/
class ST_DOM_EXPORT PublisherInfo
{
	STDom::PublisherDatabase Database;
	QString DatabaseFilePath;
	STDom::Publisher Publisher;
	typedef QMap<QString, CollectionPoint> TCollectionPointMap;
	typedef QMap<QString, PaymentType> TPaymentTypeMap;
	typedef QMap<QString, ShippingMethod> TShippingMethodMap;
	IdDescTableModel::TKeyValueList CollectionPoints;
	IdDescTableModel::TKeyValueList PaymentTypes;
	IdDescTableModel::TKeyValueList ShippingMethods;
	TCollectionPointMap CollectionPointMap;
	TPaymentTypeMap PaymentTypeMap;
	TShippingMethodMap ShippingMethodMap;

public:
    PublisherInfo();
	IdDescTableModel::TKeyValueList collectionPoints() const { return CollectionPoints; }
	IdDescTableModel::TKeyValueList paymentTypes() const  { return PaymentTypes; }
	IdDescTableModel::TKeyValueList shippingMethods() const { return ShippingMethods; }

	CollectionPoint getCollectionPoint(const IdDescTableModel::TKey& _Key);
	PaymentType getPaymentType(const IdDescTableModel::TKey& _Key);
	ShippingMethod getShippingMethod(const IdDescTableModel::TKey& _Key);

	void addShippingMethod(const ShippingMethod& _Value);
	void addPaymentType(const PaymentType& _Value);
	void addCollectionPoint(const CollectionPoint& _Value);

	void setDatabase(const STDom::PublisherDatabase& _Database, const QString& _FilePath) { Database = _Database; DatabaseFilePath = _FilePath; }
	STDom::PublisherDatabase publisherDatabase() const { return Database; }
	QString publisherDatabaseFilePath() const { return DatabaseFilePath; }
	void setPublisher(const STDom::Publisher& _Value) { Publisher = _Value; }
	STDom::Publisher publisher() const { return Publisher; }

};
}

#endif // PUBLISHERINFO_H
