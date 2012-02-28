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

#ifndef GRAPHICSPAGEITEM_H
#define GRAPHICSPAGEITEM_H

#include "graphicsphotoitem.h"
#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"

/**
Item class for pages.

*/
namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsPageItem : public GraphicsPhotoItem
{
public:	
	enum { Type = UserType + 4 };

private:
	void init();
	
protected:
	virtual void updateCursor();

public:
	GraphicsPageItem(const QRectF & rect, QGraphicsItem * parent = 0);
	GraphicsPageItem(QGraphicsItem * parent = 0);
	int type() const { return Type; }	
	void loadElement(QDomElement& _Element, const QString& _LoadDir = "");
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const;
	void setResizeAllowed(bool _Value);
	QStringList saveResources(const QDir& _StoreDir, bool _SaveImageRes = false);
	static QString tagName() { return "pageitem"; }
	
};
}
#endif
