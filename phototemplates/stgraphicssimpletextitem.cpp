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

#include "stgraphicssimpletextitem.h"
#include <QFontMetrics> 
#include <QBrush> 
#include <QPen> 
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

//Paint
#include <QPainter>
#include <QStyleOptionGraphicsItem>


#include "stcorneritem.h" 
#include "stgraphicsitemmodifier.h" 


void STGraphicsSimpleTextItem::init()
{
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
	BGBrush = QBrush(Qt::NoBrush);
	BorderPen.setStyle(Qt::NoPen);	
	Opacity = 1; 
}


STGraphicsSimpleTextItem::STGraphicsSimpleTextItem(const STPhotoLayoutTemplate::Frame& _Frame, QGraphicsItem* _Parent)
 : QGraphicsSimpleTextItem(_Parent), Frame(_Frame), STAbstractGraphicsItem(this)
{
	init();
	setFont(_Frame.font()); 
	setPos(_Frame.topLeft());
	//setPen(QPen(_Frame.textColor()));
	setBrush(QBrush(_Frame.textColor()));
	Modifier->rotate(_Frame.rotationAngle());
	setText(_Frame.text()); 	
	QPen BPen = borderPen(); 
	BPen.setColor(_Frame.borderColor()); 
	BPen.setWidth(_Frame.borderSize()); 
	setBorderPen(BPen); 

	//To adjust text size to frame size.
	if (!_Frame.isNull())
		//scale(_Frame.width() / boundingRect().width() , _Frame.height() / boundingRect().height() );
		Modifier->scale(_Frame.width() / boundingRect().width() , _Frame.height() / boundingRect().height() );
	updateToolTip(); 
	createStandardCorners();
	setControlsVisible(false);
}


STGraphicsSimpleTextItem::STGraphicsSimpleTextItem(QGraphicsItem* _Parent) : QGraphicsSimpleTextItem(_Parent), Frame(QRectF(), 0), STAbstractGraphicsItem(this)
{
	init();
	updateToolTip(); 
	createStandardCorners();
	setControlsVisible(false);
}

STGraphicsSimpleTextItem::~STGraphicsSimpleTextItem()
{
}

void STGraphicsSimpleTextItem::setText(const QString& _Text)
{
	QGraphicsSimpleTextItem::setText(_Text); 
/*	if (Frame.textAlignment() & Qt::AlignHCenter)
	{
		QFontMetrics FMetrics(font());
		QRect FontRect = FMetrics.boundingRect(text()); 
		QPointF Pos = pos();
		//TODO: reimplementar paint per primer alinear i després rotar.
		if (Frame.rotationAngle() == 90)
		{
			qreal PosCenterY = Pos.y() + (Frame.height() / 2) - (FontRect.width() / 2);
			Pos.setY(qMax(PosCenterY, Pos.y()));
		}
		else 
		{
			qreal PosCenterX = Pos.x() + (Frame.width() / 2) - (FontRect.width() / 2);
			Pos.setX(qMax(PosCenterX, Pos.x()));
		}
		//Pos = transform().map(Pos);
		setPos(Pos);
	}*/
}

void STGraphicsSimpleTextItem::loadElement(QDomElement& _Element)
{
	QGraphicsSimpleTextItem::setText(_Element.attribute("text", ""));
	setPos(_Element.attribute("x", "0").toDouble(), 
			_Element.attribute("y", "0").toDouble());
	Opacity = _Element.attribute("opacity", "1").toDouble();
	setTransform(STAbstractGraphicsItem::loadTransformElement(_Element)); 
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			STAbstractGraphicsItem* CItem;
			if (CEl.tagName().toLower() ==  "font" )
			{
				QFont MFont = font(); 
				MFont.setFamily(CEl.attribute("family", "")); 
				MFont.setPointSize(CEl.attribute("pointsize", "10").toInt()); 
				MFont.setWeight(CEl.attribute("weight", "").toInt()); 
				MFont.setItalic(CEl.attribute("italic", "").toInt());
				setFont(MFont); 
			}
			else 
			if (CEl.tagName().toLower() ==  "frame" )
			{
				Frame.setX(CEl.attribute("x", "0").toDouble());
				Frame.setX(CEl.attribute("y", "0").toDouble());
				Frame.setX(CEl.attribute("width", "0").toDouble());
				Frame.setX(CEl.attribute("height", "0").toDouble());
				Frame.setRotationAngle(CEl.attribute("rotationangle", "0").toDouble());
			}
			else 
			if (CEl.tagName().toLower() ==  "brush" )
			{
				QBrush MBrush(QColor(CEl.attribute("color", "")), 
					static_cast<Qt::BrushStyle>(CEl.attribute("stype", "").toInt()));
				setBrush(MBrush);
			}
			else 
			if (CEl.tagName().toLower() ==  "borderpen" )
			{
				QPen Pen = pen(); 
				Pen.setColor(CEl.attribute("color", "#000000"));
				Pen.setStyle(static_cast<Qt::PenStyle>(CEl.attribute("style", "0").toInt()));
				Pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(CEl.attribute("joinstyle", "0").toInt()));
				Pen.setWidth(CEl.attribute("width", "0").toInt());
				setBorderPen(Pen);
			}
			else 
			if (CEl.tagName().toLower() ==  "pen" )
			{
				QPen Pen = pen(); 
				Pen.setColor(CEl.attribute("color", "#000000"));
				Pen.setStyle(static_cast<Qt::PenStyle>(CEl.attribute("style", "0").toInt()));
				Pen.setWidth(CEl.attribute("width", "0").toInt());
				Pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(CEl.attribute("joinstyle", "0").toInt()));
				setPen(Pen);
			}
			else 
			if (CEl.tagName().toLower() ==  "bgbrush" )
			{
				BGBrush.setColor(QColor(CEl.attribute("color", "")));
				BGBrush.setStyle(static_cast<Qt::BrushStyle>(CEl.attribute("stype", "").toInt()));
			}
		}
		CNode = CNode.nextSibling();
	}	
	updateToolTip(); 
}
	
QDomElement STGraphicsSimpleTextItem::createElement(QDomDocument& _Doc)
{
	QDomElement MElement = _Doc.createElement(tagName());
	MElement.setAttribute("x", pos().x()); 
	MElement.setAttribute("y", pos().y()); 
	MElement.setAttribute("opacity", Opacity); 

	//Text 
	MElement.setAttribute("text", text()); 
	//Font 
	QDomElement FontEl = _Doc.createElement("font");
	QFont MFont = font(); 
	FontEl.setAttribute("family", MFont.family()); 
	FontEl.setAttribute("pointsize", MFont.pointSize()); 
	FontEl.setAttribute("weight", MFont.weight()); 
	FontEl.setAttribute("italic", MFont.italic()); 
	MElement.appendChild(FontEl); 
	
	//Frame 
	QDomElement FrameEl = _Doc.createElement("frame");
	FrameEl.setAttribute("x", Frame.x()); 
	FrameEl.setAttribute("y", Frame.y()); 
	FrameEl.setAttribute("width", Frame.width()); 
	FrameEl.setAttribute("height", Frame.height()); 
	FrameEl.setAttribute("rotationangle", Frame.rotationAngle()); 
	MElement.appendChild(FrameEl); 
	
	//Brush 
	QDomElement BrushElement = _Doc.createElement("brush"); 
	BrushElement.setAttribute("color", brush().color().name());
	BrushElement.setAttribute("stype", brush().style());
	MElement.appendChild(BrushElement);

	//Pen
	QDomElement BGBrushElement = _Doc.createElement("bgbrush"); 
	BGBrushElement.setAttribute("color", BGBrush.color().name());
	BGBrushElement.setAttribute("stype", BGBrush.style());
	MElement.appendChild(BGBrushElement);
	
	QDomElement BPenEl = _Doc.createElement("borderpen"); 
	BPenEl.setAttribute("color", BorderPen.color().name()); 
	BPenEl.setAttribute("style", BorderPen.style()); 
	BPenEl.setAttribute("joinstyle", BorderPen.joinStyle()); 
	BPenEl.setAttribute("width", BorderPen.width()); 
	MElement.appendChild(BPenEl); 
	
	
	QDomElement PenEl = _Doc.createElement("pen"); 
	PenEl.setAttribute("color", pen().color().name()); 
	PenEl.setAttribute("style", pen().style()); 
	PenEl.setAttribute("joinstyle", pen().joinStyle()); 
	PenEl.setAttribute("width", pen().width()); 
	MElement.appendChild(PenEl); 
	
	
	MElement.appendChild(STAbstractGraphicsItem::createTransformElement(this, _Doc));

	return MElement; 
}

void STGraphicsSimpleTextItem::setBackgroundBrush(const QBrush& _Brush)
{
	BGBrush = _Brush; 
	update(); 
}

void STGraphicsSimpleTextItem::setBorderPen(const QPen& _Pen)
{
	BorderPen = _Pen; 
	update();
}

void STGraphicsSimpleTextItem::setOpacity(qreal _Value)
{
	Opacity = _Value; 
	update(); 
}


QTextCharFormat STGraphicsSimpleTextItem::textCharFormat() const
{
	QTextCharFormat Res; 
	Res.setFont(font()); 
	Res.setTextOutline(pen()); 
	Res.setBackground(BGBrush); 
	Res.setForeground(brush());
	return Res; 
}

void STGraphicsSimpleTextItem::setTextCharFormat(const QTextCharFormat& _Format)
{
	setFont(_Format.font()); 
	QPen Pen(_Format.textOutline()); 
	Pen.setWidth(1); 
	setPen(Pen); 
	//setPen(_Format.textOutline());
	setBackgroundBrush(_Format.background()); 
	setBrush(_Format.foreground()); 
}

void STGraphicsSimpleTextItem::paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget)
{
	_P->setOpacity(Opacity); 
	_P->save();
	_P->fillRect(_Option->exposedRect, BGBrush); 
	QPen CPen = BorderPen; 
	_P->setPen(CPen);
	_P->drawRect(_Option->exposedRect);
	_P->restore();
	QGraphicsSimpleTextItem::paint(_P, _Option, _Widget);
}

QVariant STGraphicsSimpleTextItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged)
		updateToolTip(); 
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		setControlsVisible(isSelected());
	else
	if (change == ItemPositionChange && scene()) 
	{
		QPointF newPos = value.toPointF();
		if (SnapToGrid)
		{
			newPos.setX(snapToGridValue(newPos.x()));
			newPos.setY(snapToGridValue(newPos.y()));
			//return newPos; 
		}
		//FIXME: When items are rotated it does not runs well.
		/*QRectF rect = scene()->sceneRect();
		QRectF NewItemRect(sceneBoundingRect()); 
		NewItemRect.moveTo(newPos);
		if (!rect.contains(NewItemRect)) 
		{
			// Keep the item inside the scene rect.
			newPos.setX(qMin(rect.right() - sceneBoundingRect().width() , qMax(newPos.x(), rect.left())));
			newPos.setY(qMin(rect.bottom() - sceneBoundingRect().height(), qMax(newPos.y(), rect.top())));
		}*/
		return newPos; 
	}
	return QGraphicsItem::itemChange(change, value);
}

