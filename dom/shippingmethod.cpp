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
#include "shippingmethod.h"

using namespace STDom;

ShippingMethod::ShippingMethod(const QDomElement& _Element)
{
	loadXml(_Element);
}


void ShippingMethod::loadXml(const QDomElement& _Element)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "description" )
				setDescription(CEl.text());
			else
			if (CEl.tagName().toLower() == "price" )
				setPrice(CEl.text().toDouble());
			else
			if (CEl.tagName().toLower() == "id" )
				Id = CEl.text();
		}
		CNode = CNode.nextSibling();
	}

}

QDomElement ShippingMethod::createElement(QDomDocument& _Doc, const QString& _TagName) const
{
	QDomElement RootEl = _Doc.createElement(_TagName);

	if (!isEmpty())
	{
		QDomElement CElement = _Doc.createElement("id");
		CElement.appendChild(_Doc.createTextNode(Id));
		RootEl.appendChild(CElement);

		CElement = _Doc.createElement("description");
		CElement.appendChild(_Doc.createTextNode(description()));
		RootEl.appendChild(CElement);

		CElement = _Doc.createElement("price");
		CElement.appendChild(_Doc.createTextNode(QString::number(price())));
		RootEl.appendChild(CElement);
	}
	return RootEl;
}
