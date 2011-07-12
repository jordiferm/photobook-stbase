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

#include "abstractgraphicsitem.h"
#include <QDomElement> 
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent> 
#include <QGraphicsSceneHoverEvent> 
#include <QStyleOptionGraphicsItem>
#include <QToolTip>
#include <QGraphicsScene>
#include <QGraphicsView> 
#include <QGraphicsWidget> 
#include <QGraphicsDropShadowEffect>
#include <QDebug>

#include "graphicsitemmodifier.h"
#include "corneritem.h"

using namespace SPhotoBook;

int AbstractGraphicsItem::GridValue = 5;
bool AbstractGraphicsItem::SnapToGrid = true;

AbstractGraphicsItem::AbstractGraphicsItem(QGraphicsItem* _Item) : ResizeAllowed(true), MControlGWidget(0), ControlsEnabled(true), Modified(false)
{
	Modifier = new GraphicsItemModifier(_Item);
}

AbstractGraphicsItem::~AbstractGraphicsItem()
{
	delete Modifier;
}

void AbstractGraphicsItem::createStandardCorners()
{
	CornerItem* Corner = new CornerItem(Qt::BottomLeftCorner, CornerItem::Rotate , Modifier);
	Corner->setPixmapName(":/phototemplates/object-rotate-right.png");

	Corner = new CornerItem(Qt::BottomRightCorner, CornerItem::Scale, Modifier);
	Corner->setPixmapName(":/phototemplates/transform-scale.png");

	Corner = new CornerItem(Qt::TopLeftCorner, CornerItem::Move, Modifier);
	Corner->setPixmapName(":/phototemplates/transform-move.png");

	Modifier->layoutChildren();
}


bool AbstractGraphicsItem::resizeAllowed() const
{ 
	return ResizeAllowed; 
}

void AbstractGraphicsItem::setControlsVisible(bool _Value)
{
	if (ControlsEnabled)
	{
		if (MControlGWidget)
			MControlGWidget->setVisible(_Value); 
		
		if (Modifier)
		{
			if (_Value)
				Modifier->layoutChildren();
			Modifier->setChildrenVisible(_Value);
		}
	}
}

bool AbstractGraphicsItem::controlCollided(qreal& _TopZValue) const
{
	bool Res = false; 
	if (MControlGWidget)
	{
		int Cnt = 0;
		qreal CZValue = MControlGWidget->zValue() -1; 
		QList<QGraphicsItem *> CollItems = MControlGWidget->collidingItems();
		while (Cnt < CollItems.count())
		{
			if (CollItems[Cnt]->zValue() >= CZValue)
				CZValue = CollItems[Cnt]->zValue();
			
			Cnt++;
		}
		Res = CZValue >= MControlGWidget->zValue(); 
		_TopZValue = CZValue; 
	}
	return Res;
}

void AbstractGraphicsItem::setControlsWidget(QGraphicsWidget* _GWidget)
{
	MControlGWidget = _GWidget;
}

QDomElement AbstractGraphicsItem::createTransformElement(const QGraphicsItem* _Item, QDomDocument& _Doc)
{
	QDomElement TransElement = _Doc.createElement("transform"); 
	QTransform Trans = _Item->transform(); 
	TransElement.setAttribute("m11", Trans.m11()); 
	TransElement.setAttribute("m21", Trans.m21()); 
	TransElement.setAttribute("m31", Trans.m31()); 
	TransElement.setAttribute("m12", Trans.m12()); 
	TransElement.setAttribute("m22", Trans.m22()); 
	TransElement.setAttribute("m32", Trans.m32()); 
	TransElement.setAttribute("m13", Trans.m13()); 
	TransElement.setAttribute("m23", Trans.m23()); 
	TransElement.setAttribute("m33", Trans.m33()); 
	return TransElement; 
}

void AbstractGraphicsItem::appendEffectElements(QDomElement& _Parent, const QGraphicsItem* _Item, QDomDocument& _Doc)
{
	if (QGraphicsDropShadowEffect* NewEffect = qobject_cast<QGraphicsDropShadowEffect*>(_Item->graphicsEffect()))
	{
		QDomElement ShadowElement = _Doc.createElement("shadow");
		ShadowElement.setAttribute("xoffset", NewEffect->xOffset());
		ShadowElement.setAttribute("yoffset", NewEffect->yOffset());
		ShadowElement.setAttribute("blurradius", NewEffect->blurRadius());
		ShadowElement.setAttribute("color", NewEffect->color().name());
		_Parent.appendChild(ShadowElement);
	}
}

void AbstractGraphicsItem::loadEffectElements(QGraphicsItem* _Item, QDomElement& _Element)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "shadow")
			{
				QGraphicsDropShadowEffect* NewEffect = new QGraphicsDropShadowEffect;
				NewEffect->setXOffset(CEl.attribute("xoffset", "0").toDouble());
				NewEffect->setYOffset(CEl.attribute("yoffset", "0").toDouble());
				NewEffect->setBlurRadius(CEl.attribute("blurradius", "0").toDouble());
				NewEffect->setColor(CEl.attribute("color", "#000000"));
				_Item->setGraphicsEffect(NewEffect);
			}
		}
		CNode = CNode.nextSibling();
	}
}

QTransform AbstractGraphicsItem::loadTransformElement(QDomElement& _Element)
{
	QTransform Res;
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "transform")
			{
				Res.setMatrix(CEl.attribute("m11", "0").toDouble(), 
									CEl.attribute("m12", "0").toDouble(),
									CEl.attribute("m13", "0").toDouble(),
									CEl.attribute("m21", "0").toDouble(),
									CEl.attribute("m22", "0").toDouble(),
									CEl.attribute("m23", "0").toDouble(),
									CEl.attribute("m31", "0").toDouble(),
									CEl.attribute("m32", "0").toDouble(),
									CEl.attribute("m33", "0").toDouble());
			}
		}
		CNode = CNode.nextSibling();		
	}
	return Res; 
}

qreal AbstractGraphicsItem::snapToGridValue(qreal _Value)
{
	qreal Res = _Value;
	if (SnapToGrid)
		Res = static_cast<qreal>(static_cast<int>(_Value) - static_cast<int>(_Value) % GridValue);
	return Res; 
}

QSizeF AbstractGraphicsItem::mouseIntersecsThreshold(QGraphicsItem* _Item)
{
// 	bool Invertible; 
// 	QSizeF ISThres = _Item->transform().inverted(&Invertible).mapRect(QRectF(0, 0, 5, 5)).size();
// 	if (!Invertible)
// 		ISThres = QSizeF(5, 5); 
// 	//QSizeF ISThres = _Item->transform().mapRect(QRectF(0, 0, 5, 5)).size();
// 	return ISThres; 
	return QSizeF(5, 5); 
}

AbstractGraphicsItem::EnMousePosIntersects AbstractGraphicsItem::getMouseIntersects(const QPointF& _Pos, const QRectF& _ItemBoudingRect, const QSizeF& _Threshold)
{
	QRectF CurrRect = _ItemBoudingRect;
	QSizeF MThresH = _Threshold; 
	int Res = MouseOnNoEdge; 
	if (_Pos.x() > CurrRect.x()  && _Pos.x() < CurrRect.x() + MThresH.width())
		Res |= MouseOnLeftEdge;
	if (_Pos.x() > CurrRect.right() - MThresH.width() && _Pos.x() < CurrRect.right())
		Res |= MouseOnRightEdge;
	if (_Pos.y() > CurrRect.y()  && _Pos.y() < CurrRect.y() + MThresH.height())
		Res |= MouseOnTopEdge;
	if (_Pos.y() > CurrRect.bottom() - MThresH.height() && _Pos.y() < CurrRect.bottom())
		Res |= MouseOnBottomEdge;
	return static_cast<EnMousePosIntersects>(Res); 
}

//!!! OBSOLETE !!!
void AbstractGraphicsItem::scaleOnMouseMoveEvent(QGraphicsSceneMouseEvent* _Event, QGraphicsItem* _Item)
{
	if (MouseDownPosInt != MouseOnNoEdge && ResizeAllowed)
	{
		QRectF CurrRect = _Item->boundingRect();
		QRectF LastRect = CurrRect; 
		if (MouseDownPosInt & MouseOnRightEdge)
			CurrRect.setRight(_Event->pos().x());
		if (MouseDownPosInt & MouseOnBottomEdge)
			CurrRect.setBottom(_Event->pos().y());
		
		qreal Sx = CurrRect.width() / LastRect.width();
		qreal Sy = CurrRect.height() / LastRect.height();
		if (_Event->modifiers() & Qt::ShiftModifier)
		{
			qreal Scaled = qMax(Sx, Sy);
			_Item->scale(Scaled, Scaled);
		}
		else 
			_Item->scale(Sx, Sy);
		//setRect(CurrRect);
		updateToolTip(); 
	}
}

void AbstractGraphicsItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * _Event, QGraphicsItem* _Item)
{
	EnMousePosIntersects MPosInt = getMouseIntersects(_Event->pos(), _Item->boundingRect(), mouseIntersecsThreshold(_Item)); 
	if (((MPosInt & MouseOnRightEdge)) && (!(MPosInt & MouseOnTopEdge) && !(MPosInt & MouseOnBottomEdge)))
		_Item->setCursor(Qt::SizeHorCursor);
	if (((MPosInt & MouseOnBottomEdge)) && (!(MPosInt & MouseOnLeftEdge) && !(MPosInt & MouseOnRightEdge)))
		_Item->setCursor(Qt::SizeVerCursor);
	if (((MPosInt & MouseOnRightEdge) && (MPosInt & MouseOnBottomEdge)))
		_Item->setCursor(Qt::SizeFDiagCursor);
	if (MPosInt == MouseOnNoEdge)
		_Item->unsetCursor();

}

void AbstractGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _Event, QGraphicsItem* _Item)
{
	_Item->unsetCursor();
	if (!_Item->isSelected())
		setControlsVisible(false);
}

void AbstractGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _Event, QGraphicsItem* _Item)
{
    setControlsVisible(true);
}

void AbstractGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* _Event, QGraphicsItem* _Item)
{
	MouseDownPosInt = getMouseIntersects(_Event->pos(), _Item->boundingRect(), mouseIntersecsThreshold(_Item)); 
}

void AbstractGraphicsItem::updateToolTip()
{
	Modifier->updateToolTip(); 
}

void AbstractGraphicsItem::updateToolTip(const QRectF& _Rect)
{
	Modifier->updateToolTip(_Rect); 
}

void AbstractGraphicsItem::modified()
{
	Modifier->modified();
}

void AbstractGraphicsItem::clearChanges()
{
	Modifier->clearChanges();
}




/*void AbstractGraphicsItem::updateToolTip(QGraphicsItem* _Item)
{
	updateToolTip(_Item, QRectF(_Item->pos(), _Item->transform().mapRect(_Item->boundingRect()).size()));
}
*/
// void AbstractGraphicsItem::updateToolTip(QGraphicsItem* _Item, const QRectF& _Rect)
// {
// 	QString StrToolTip = QString(QObject::tr("X:%1, Y:%2, W:%3, H:%4  (mm)")).arg(_Rect.x()).arg(_Rect.y()).arg(_Rect.width()).arg(_Rect.height());
// 	_Item->setToolTip(StrToolTip);
// 	QWidget* FirstView = 0; 
// 	if (_Item->scene())
// 		if (_Item->scene()->views().size() > 0)
// 			FirstView = _Item->scene()->views().first();
// 	if (FirstView)
// 		QToolTip::showText(FirstView->mapToGlobal(FirstView->geometry().topLeft()), StrToolTip, FirstView); 
// 
// }
