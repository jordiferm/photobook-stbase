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
#include "stgraphicsclipartitem.h"
#include <QDomElement> 
#include <QFileInfo> 
#include <QSvgRenderer> 
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "stgraphicsitemmodifier.h" 

void STGraphicsClipartItem::init()
{
	QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable);
  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable);
	Opacity = 1; 
	STAbstractGraphicsItem::updateToolTip(); 
	createStandardCorners();
	setControlsVisible(false);
}



STGraphicsClipartItem::STGraphicsClipartItem(const QString& _FileName, QGraphicsItem* _Parent) 
 : QGraphicsSvgItem(_FileName, _Parent), FileName(_FileName), STAbstractGraphicsItem(this)
{
	init(); 
}

STGraphicsClipartItem::STGraphicsClipartItem(QGraphicsItem* _Parent) 
 : QGraphicsSvgItem(_Parent), STAbstractGraphicsItem(this)
{
	init(); 
}

void STGraphicsClipartItem::loadElement(QDomElement& _Element)
{
	setPos(_Element.attribute("x", "0").toDouble(), 
			_Element.attribute("y", "0").toDouble());
	Opacity = _Element.attribute("opacity", "1").toDouble();
	//renderer()->load(ImageSourcePath + "/" + _Element.attribute("src", ""));
	setTransform(STAbstractGraphicsItem::loadTransformElement(_Element)); 
	STAbstractGraphicsItem::updateToolTip();  
}

QDomElement STGraphicsClipartItem::createElement(QDomDocument& _Doc)
{
	QDomElement MElement = _Doc.createElement(tagName());
	MElement.setAttribute("x", pos().x()); 
	MElement.setAttribute("y", pos().y()); 
	MElement.setAttribute("opacity", Opacity); 
	QFileInfo FNInfo(FileName); 
	MElement.setAttribute("src", FNInfo.fileName()); 
	MElement.appendChild(STAbstractGraphicsItem::createTransformElement(this, _Doc));

	return MElement; 	
}

void STGraphicsClipartItem::setOpacity(qreal _Value)
{
	Opacity = _Value; 
	qDebug("Set Opacity %f", _Value);
	update(); 
}


void STGraphicsClipartItem::scaleToHeight(qreal _Height)
{
	QRectF BRect = boundingRect(); 
	qreal SY = _Height / BRect.height(); 
	Modifier->scale(SY, SY); 
}

void STGraphicsClipartItem::scaleToWidth(qreal _Width)
{
	QRectF BRect =  transform().mapRect(boundingRect()); 
	qreal SX = _Width / BRect.width(); 
	Modifier->scale(SX, SX); 
}


void STGraphicsClipartItem::paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget)
{
	_P->setOpacity(Opacity);
	QGraphicsSvgItem::paint(_P, _Option, _Widget); 	
}


QVariant STGraphicsClipartItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		setControlsVisible(isSelected());
	else
	if (change == ItemPositionHasChanged)
		STAbstractGraphicsItem::updateToolTip(); 
	return QGraphicsItem::itemChange(change, value);	
}
