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
#ifndef GRAPHICSCLIPARTITEM_H
#define GRAPHICSCLIPARTITEM_H

#include <QGraphicsSvgItem>
#include <QRectF>
#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"
#include "resource.h"

/**
Class to create items for clipart.

	@author
*/
namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsClipartItem : public QGraphicsSvgItem, public AbstractGraphicsItem
{
	Q_OBJECT


public:
	//enum { Type = UserType + 4 };
	//FIXME: Use UserType Que no rutlla i no se per que !!!.
	enum { Type = 1046 };

private:
	Resource ClipartResource;
	qreal Opacity; 
	QSvgRenderer* OldRenderer;

	void init(); 	

public:
	GraphicsClipartItem(const Resource& _Resource, QGraphicsItem * parent = 0);
	GraphicsClipartItem(QGraphicsItem * parent = 0);
	void loadElement(const QDomElement& _Element, const QString& _LoadDir = "");
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const;
	int type() const { return Type; }

	void setClipartResource(const Resource& _Resource);
	QStringList saveResources(const QDir& _StoreDir);

	static QString tagName() { return "clipartitem"; }
	void setOpacity(qreal _Value);
	qreal opacity() const { return Opacity; }
	void scaleToHeight(qreal _Height);
	void scaleToWidth(qreal _Width);
	virtual AbstractGraphicsItem* clone() const;

protected:
	void paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget = 0);	
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

};
}
#endif
