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
#ifndef COLLECTIONPOINT_H
#define COLLECTIONPOINT_H
#include "xmlorder.h"
#include "stdomexport.h"
#include <QUrl>

namespace STDom
{

/**
	@author Shadow
*/
class ST_DOM_EXPORT CollectionPoint : public STDom::XmlOrderDealer
{
	QUrl Web;
	QString WebInfo;

public:
	CollectionPoint(const QString& _Id = "NoId");
	void setWebInfo(const QString& _WebInfo) { WebInfo = _WebInfo; }
	QString webInfo() { return WebInfo; }
	void setWeb(const QUrl& _Value) { Web = _Value; }
	QUrl web() { return Web; }
};

}
#endif // COLLECTIONPOINT_H
