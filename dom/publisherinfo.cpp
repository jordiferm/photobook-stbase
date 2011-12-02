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
#include "publisherinfo.h"

using namespace STDom;

PublisherInfo::PublisherInfo()
{
	//void setCollectionPoints
}

CollectionPoint PublisherInfo::getCollectionPoint(const IdDescTableModel::TKey& _Key)
{
	CollectionPoint Res;
	if (CollectionPointMap.contains(_Key))
		Res = CollectionPointMap[_Key];
	return Res;
}

PaymentType PublisherInfo::getPaymentType(const IdDescTableModel::TKey& _Key)
{
	PaymentType Res;
	if (PaymentTypeMap.contains(_Key))
		Res = PaymentTypeMap[_Key];
	return Res;
}

ShippingMethod PublisherInfo::getShippingMethod(const IdDescTableModel::TKey& _Key)
{
	ShippingMethod Res;
	if (ShippingMethodMap.contains(_Key))
		Res = ShippingMethodMap[_Key];
	return Res;
}

ShippingMethod PublisherInfo::getDefaultShippingMethod() const
{
	ShippingMethod Res;
	if (!ShippingMethodMap.isEmpty())
		Res = ShippingMethodMap[0];
	return Res;
}

void PublisherInfo::addShippingMethod(const ShippingMethod& _Value)
{
	ShippingMethodMap.insert(_Value.id(), _Value );
	ShippingMethods.push_back(IdDescTableModel::TKeyValue(_Value.id(), _Value.description()));
}

void PublisherInfo::addPaymentType(const PaymentType& _Value)
{
	PaymentTypeMap.insert(_Value.id(), _Value );
	PaymentTypes.push_back(IdDescTableModel::TKeyValue(_Value.id(), _Value.description()));
}

void PublisherInfo::addCollectionPoint(const CollectionPoint& _Value)
{
	CollectionPointMap.insert(_Value.id(), _Value );
	CollectionPoints.push_back(IdDescTableModel::TKeyValue(_Value.id(), _Value.name()));
}

