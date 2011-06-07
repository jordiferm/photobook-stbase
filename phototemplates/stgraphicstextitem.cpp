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

#include "stgraphicstextitem.h"
#include <QFontMetrics> 

void STGraphicsTextItem::init()
{
	QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable);

	setTextInteractionFlags(Qt::NoTextInteraction);
  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, true);
  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, false);
}

STGraphicsTextItem::STGraphicsTextItem(const STPhotoLayoutTemplate::Frame& _Frame, QGraphicsItem* _Parent)
 : QGraphicsTextItem(_Parent), Frame(_Frame), STAbstractGraphicsItem(this)
{
	//QGraphicsItem::setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
	//QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable);
	init();
	setFont(_Frame.font()); 
	setPos(_Frame.topLeft());
	rotate(_Frame.rotationAngle());
	setHtml(_Frame.text()); 	
	//setText(_Frame.text()); 	
	updateToolTip(); 
	createStandardCorners();
	setControlsVisible(false);

}

STGraphicsTextItem::STGraphicsTextItem(QGraphicsItem* _Parent)
 : QGraphicsTextItem(_Parent), Frame(QRectF(), 0), STAbstractGraphicsItem(this)
{
	init();
	STAbstractGraphicsItem::updateToolTip(); 
	createStandardCorners();
	setControlsVisible(false);
}

void STGraphicsTextItem::setMovable(bool _Movable)
{
	//Does nothing.
//  	if (_Movable)
//  		setTextInteractionFlags(Qt::NoTextInteraction);
//  	else 
//  		setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
// 	
//  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, _Movable);
//  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, !_Movable);
	
}

// void STGraphicsTextItem::setText(const QString& _Text)
// {
// 	QGraphicsTextItem::setText(_Text); 
// 	if (Frame.textAlignment() & Qt::AlignHCenter)
// 	{
// 		QFontMetrics FMetrics(Frame.font());
// 		QRect FontRect = FMetrics.boundingRect(Frame.text()); 
// 		QPointF Pos = Frame.topLeft();
// 		//TODO: reimplementar paint per primer alinear i despr�s rotar.
// 		if (Frame.rotationAngle() == 90)
// 		{
// 			qreal PosCenterY = Pos.y() + (Frame.height() / 2) - (FontRect.width() / 2);
// 			Pos.setY(qMax(PosCenterY, Pos.y()));
// 		}
// 		else 
// 		{
// 			qreal PosCenterX = Pos.x() + (Frame.width() / 2) - (FontRect.width() / 2);
// 			Pos.setX(qMax(PosCenterX, Pos.x()));
// 		}
// 		//Pos = transform().map(Pos);
// 		setPos(Pos);
// 	}	
// }

void STGraphicsTextItem::loadElement(QDomElement& _Element)
{
	setPos(_Element.attribute("x", "0").toDouble(), 
			_Element.attribute("y", "0").toDouble());
	setHtml(_Element.text());
	setTextWidth(_Element.attribute("textwidth", "-1").toDouble());
	setTransform(STAbstractGraphicsItem::loadTransformElement(_Element)); 
	STAbstractGraphicsItem::updateToolTip(); 
}

QDomElement STGraphicsTextItem::createElement(QDomDocument& _Doc)
{
	QDomElement MElement = _Doc.createElement(tagName());
	MElement.setAttribute("x", pos().x()); 
	MElement.setAttribute("y", pos().y()); 
	MElement.setAttribute("textwidth", textWidth());
	QDomText CText = _Doc.createTextNode(toHtml());
	MElement.appendChild(CText);
	MElement.appendChild(STAbstractGraphicsItem::createTransformElement(this, _Doc));

	return MElement; 
}

QVariant STGraphicsTextItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged)
		STAbstractGraphicsItem::updateToolTip();  
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		setControlsVisible(isSelected());
	else
	if (change == ItemPositionChange)
	{
		QPointF newPos = value.toPointF();
		if (SnapToGrid)
		{
			newPos.setX(snapToGridValue(newPos.x()));
			newPos.setY(snapToGridValue(newPos.y()));
			return newPos; 
		}
		modified();
	}
	return QGraphicsItem::itemChange(change, value);
}
 
