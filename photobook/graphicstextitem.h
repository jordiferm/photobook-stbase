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

#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>
#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"

/**
Class to create graphics text items.

*/
namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsTextItem : public QGraphicsTextItem, public AbstractGraphicsItem
{
public:	
	enum { Type = UserType + 3 };
	
private:
	void init();
	
public:
	GraphicsTextItem(QGraphicsItem * parent = 0);
//	void setText(const QString& _Text);
	int type() const { return Type; }	
	void setMovable(bool );
	void loadElement(QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc);
	static QString tagName() { return "textitem"; }	
	Qt::Alignment alignment() const;
	void setAlignment(Qt::Alignment alignment);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent* _Event);
};
}
#endif
