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

#ifndef PAYMENTTYPE_H
#define PAYMENTTYPE_H
#include "stdomexport.h"
#include <QUrl>
#include <QDomElement>

namespace STDom
{

/**
	@author Shadow
*/
class ST_DOM_EXPORT PaymentType
{
	QString Id;
	QString Description;
	bool PayRequired;
	QUrl PaymentUrl;

public:
	PaymentType(const QString& _Id = "Unknown") : Id(_Id) {}
	PaymentType(const QDomElement& _Element);
	void setId(const QString& _Value) { Id = _Value; }
	QString id() const { return Id; }

	void loadXml(const QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc, const QString& _TagName) const;

	void setDescription(const QString& _Value) { Description = _Value; }
	QString description() const { return Description; }

	void setPayRequired(bool _Value) { PayRequired = _Value; }
	bool payRequired() { return PayRequired; }

	void setPaymentUrl(const QUrl& _Value) { PaymentUrl = _Value; }
	QUrl paymentUrl() const { return PaymentUrl; }
	bool isEmpty() const { return Id.isEmpty(); }

};
}

#endif // PAYMENTTYPE_H
