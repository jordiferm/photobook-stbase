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
#include "paymenttype.h"

using namespace STDom;

PaymentType::PaymentType(const QDomElement& _Element)
{
	loadXml(_Element);
}

void PaymentType::loadXml(const QDomElement& _Element)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "description" )
				Description = CEl.text();
			else
			if (CEl.tagName().toLower() == "payrequired" )
				PayRequired = CEl.text().toLower()=="true";
			else
			if (CEl.tagName().toLower() == "paymenturl" )
				PaymentUrl.setUrl(CEl.text());
			else
			if (CEl.tagName().toLower() == "id" )
				Id = CEl.text();
		}
		CNode = CNode.nextSibling();
	}

}

QDomElement PaymentType::createElement(QDomDocument& _Doc, const QString& _TagName) const
{
	QDomElement Dealer = _Doc.createElement(_TagName);

	if (!isEmpty())
	{
		QDomElement CElement = _Doc.createElement("id");
		CElement.appendChild(_Doc.createTextNode(Id));
		Dealer.appendChild(CElement);

		CElement = _Doc.createElement("description");
		CElement.appendChild(_Doc.createTextNode(Description));
		Dealer.appendChild(CElement);

		CElement = _Doc.createElement("payrequired");
		if (PayRequired)
			CElement.appendChild(_Doc.createTextNode("true"));
		else
			CElement.appendChild(_Doc.createTextNode("false"));
		Dealer.appendChild(CElement);

		CElement = _Doc.createElement("paymenturl");
		CElement.appendChild(_Doc.createTextNode(paymentUrl().toString()));
		Dealer.appendChild(CElement);

	}
	return Dealer;
}
