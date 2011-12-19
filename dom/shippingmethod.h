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
#ifndef SHIPPINGMETHOD_H
#define SHIPPINGMETHOD_H
#include "stdomexport.h"
#include <QDomElement>
#include "publisherdatabase.h"

namespace STDom
{

class ST_DOM_EXPORT ShippingMethod : public PublisherBill::PublisherShippingMethod
{
	QString Id;

public:
	ShippingMethod(const QString& _Id = "NoId") : Id(_Id) {}
	ShippingMethod(const QDomElement& _Element);
	void setId(const QString& _Id) {  Id = _Id; }
	QString id() const { return Id; }
	void loadXml(const QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc, const QString& _TagName) const;
};

}
#endif // SHIPPINGMETHOD_H
