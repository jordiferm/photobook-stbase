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
#ifndef GRAPHICSCLIPARTITEM_H
#define GRAPHICSCLIPARTITEM_H

#include <QGraphicsSvgItem>
#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"

/**
Class to create items for clipart.

	@author
*/
namespace STPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsClipartItem : public QGraphicsSvgItem, public AbstractGraphicsItem
{
	Q_OBJECT


public:
	//enum { Type = UserType + 4 };
	//FIXME: Use UserType Que no rutlla i no se per que !!!.
	enum { Type = 1046 };

private:
	QString FileName; 
	qreal Opacity; 
	
	void init(); 	

public:
	GraphicsClipartItem(const QString & fileName, QGraphicsItem * parent = 0);
	GraphicsClipartItem(QGraphicsItem * parent = 0);
	void loadElement(QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc);
	int type() const { return Type; }
	void setFileName(const QString& _FileName) { FileName = _FileName; }
	static QString tagName() { return "clipartitem"; }	
	QString fileName() const { return FileName; }
	void setOpacity(qreal _Value);
	qreal opacity() const { return Opacity; }
	void scaleToHeight(qreal _Height);
	void scaleToWidth(qreal _Width);

protected:
	void paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget = 0);	
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

};
}
#endif
