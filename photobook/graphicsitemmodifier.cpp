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
#include "graphicsitemmodifier.h"
#include <QGraphicsItem> 
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene> 
#include <QGraphicsView> 
#include <QToolTip> 
#include <QDebug>
#include <QTextDocument>
#include <math.h>

#include "corneritem.h"
#include "templatescene.h"
#include "graphicsphotoitem.h"
#include "graphicstextitem.h"

using namespace SPhotoBook;

void GraphicsItemModifier::applyTransformations()
{

	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		QTransform TransForm; 
		TransForm = TransForm.translate(CItem->rect().topLeft().x(), CItem->rect().topLeft().y());
		TransForm = TransForm.rotate(m_yRotationAngle, Qt::XAxis).rotate(m_xRotationAngle, Qt::YAxis).rotate(m_zRotationAngle);
		TransForm = TransForm.translate(-CItem->rect().topLeft().x(), -CItem->rect().topLeft().y());
		Item->setTransform(TransForm); 
		/*QRectF OrigRect = CItem->rect();
		QRectF NRect = OrigRect; 
		NRect.moveTopLeft(QPointF(0, 0));
		CItem->setRect(NRect);
		CItem->rotate(m_zRotationAngle);
		QMatrix Mat; 
		Mat.rotate(-m_zRotationAngle);
		QPointF NewCenter = Mat.map(OrigRect.topLeft());
		NRect.moveTopLeft(NewCenter);
		CItem->setRect(NRect);
		layoutChildren();*/
	}
	else 
	{
		Item->setTransform(QTransform().rotate(m_yRotationAngle, Qt::XAxis).rotate(m_xRotationAngle, Qt::YAxis).rotate(m_zRotationAngle).scale(CScaleX, CScaleY));
	}
}

GraphicsItemModifier::GraphicsItemModifier(QGraphicsItem* _Item) : Item(_Item)
	, m_xRotationAngle(0)
	, m_yRotationAngle(0)
	, m_zRotationAngle(0)
	, CScaleX(1)
	, CScaleY(1)

{
	m_contentsRect = Item->boundingRect().toRect(); 
}

QRect GraphicsItemModifier::contentsRect() const
{
    return m_contentsRect;
}

void GraphicsItemModifier::updateToolTip()
{
	updateToolTip(QRectF(Item->pos(), Item->transform().mapRect(Item->boundingRect()).size()));
}

void GraphicsItemModifier::updateToolTip(const QRectF& _Rect)
{
	QString StrToolTip = QString(QObject::tr("X:%1, Y:%2, W:%3, H:%4  (mm)")).arg(_Rect.x()).arg(_Rect.y()).arg(_Rect.width()).arg(_Rect.height());
	Item->setToolTip(StrToolTip);
	QWidget* FirstView = 0; 
	if (Item->scene())
		if (Item->scene()->views().size() > 0)
			FirstView = Item->scene()->views().first();
	if (FirstView)
		QToolTip::showText(FirstView->mapToGlobal(FirstView->geometry().topLeft()), StrToolTip, FirstView); 

}

void GraphicsItemModifier::resetContentsRatio()
{
    resizeContents(m_contentsRect, true);
}

void GraphicsItemModifier::scale(double _Sx, double _Sy)
{
	CScaleX = _Sx;
	CScaleY = _Sy;
	applyTransformations();
	modified();
}

void GraphicsItemModifier::setPos(const QPointF& _Pos, QGraphicsItem* _Sender)
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		QPointF Pos = _Sender->mapToScene(_Pos);
		Pos.setX(Pos.x() - CItem->pos().x());
		Pos.setY(Pos.y() - CItem->pos().y());
		QTransform TransForm;
		TransForm = TransForm.rotate(-m_yRotationAngle, Qt::XAxis).rotate(-m_xRotationAngle, Qt::YAxis).rotate(-m_zRotationAngle);

		Pos = TransForm.map(Pos);
		QRectF CurrRect = CItem->rect();
		Pos.setX(CItem->snapToGridValue(Pos.x()));
		Pos.setY(CItem->snapToGridValue(Pos.y()));
		CurrRect.moveTopLeft(Pos);
		if (CurrRect != CItem->rect())
		{
			CItem->setRect(CurrRect);
			layoutChildren();
		}
	}
	else
	{
		Item->setPos(_Sender->mapToScene(_Pos));
	}
	modified();
	updateToolTip();
}


void GraphicsItemModifier::setRectBottomRight(const QPointF& _Pos, QGraphicsItem* _Sender)
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		QPointF Pos = _Sender->mapToScene(_Pos); 
		Pos.setX(Pos.x() - CItem->pos().x()); 
		Pos.setY(Pos.y() - CItem->pos().y());
		QTransform TransForm; 
		TransForm = TransForm.rotate(-m_yRotationAngle, Qt::XAxis).rotate(-m_xRotationAngle, Qt::YAxis).rotate(-m_zRotationAngle);

		Pos = TransForm.map(Pos); 
		//qDebug("CurrRect: %f, %f, %f, %f", CItem->rect().x(), CItem->rect().y(), CItem->rect().width(), CItem->rect().height()); 
		//qDebug("Event Pos: %f, %f", Pos.x(), Pos.y()); 
		QRectF CurrRect = CItem->rect();
		CurrRect.setRight(CItem->snapToGridValue(Pos.x()));
		CurrRect.setBottom(CItem->snapToGridValue(Pos.y()));
		if (CurrRect != CItem->rect())
		{
			CItem->setRect(CurrRect);
			layoutChildren();
		}
	}
	else
	if (GraphicsTextItem* CItem = qgraphicsitem_cast<GraphicsTextItem*>(Item))
	{
		QPointF Pos = _Sender->mapToScene(_Pos);


		qreal Height  = Item->boundingRect().height();
		qreal Diag = QLineF(Pos, Item->pos()).length();
		if (Diag > Height)
		{
			qreal NewWidth = sqrt((Diag * Diag) - (Height * Height));
			//int NewTextWidth =  Pos.y() - Item->pos().y();
			//CItem->document()->setTextWidth(NewWidth);
			//qDebug() << "Item Scale: " << CItem->scale();
#if QT_VERSION >= 0x040600
			CItem->setTextWidth(NewWidth * 1 / CItem->scale());
#else
			CItem->setTextWidth(NewWidth * 1 / CScaleX);
#endif
			layoutChildren();
		}
	}
	else 
	{
		//if (MouseDownPosInt != MouseOnNoEdge && ResizeAllowed)
		//{
		QRectF CurrRect = Item->boundingRect();
		QRectF LastRect = CurrRect; 
		CurrRect.setRight(_Pos.x() + CurrRect.width());
		CurrRect.setBottom(_Pos.y() + CurrRect.height());
		
		qreal Sx = CurrRect.width() / LastRect.width();
		qreal Sy = CurrRect.height() / LastRect.height();
		CScaleX = Sx * CScaleX;
		CScaleY = Sy * CScaleY;
		applyTransformations();
	}

	updateToolTip(); 
	modified();
}


void GraphicsItemModifier::resizeContents(const QRect & rect, bool keepRatio)
{

	//m_contentsRect = Item->boundingRect().toRect();
//     if (!rect.isValid())
//         return;
// 
// //    prepareGeometryChange();
// 
	m_contentsRect = rect;
// //     if (keepRatio) {
// //         int hfw = contentHeightForWidth(rect.width());
// //         if (hfw > 1) {
// //             m_contentsRect.setTop(-hfw / 2);
// //             m_contentsRect.setHeight(hfw);
// //         }
// //     }
// 
// //     if (m_frame)
// //         m_frameRect = m_frame->frameRect(m_contentsRect);
// //     else
// //         m_frameRect = m_contentsRect;

//    Item->update();

	//layoutChildren();
	
//    GFX_CHANGED();
	modified();
}

void GraphicsItemModifier::layoutChildren()
{
	// layout corners
	QList<QGraphicsItem *> Items = Item->childItems(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{		
		if (CornerItem* CItem = qgraphicsitem_cast<CornerItem*>(*it))
			CItem->relayout(Item->boundingRect().toRect());
	}
}


void GraphicsItemModifier::rotate(double _Angle)
{
	m_zRotationAngle = _Angle; 
	applyTransformations(); 
	modified();
}

void GraphicsItemModifier::setRotation(double angle, Qt::Axis axis)
{
    switch (axis) {
        case Qt::XAxis: if (m_xRotationAngle == angle) return; m_xRotationAngle = angle; break;
        case Qt::YAxis: if (m_yRotationAngle == angle) return; m_yRotationAngle = angle; break;
        case Qt::ZAxis: if (m_zRotationAngle == angle) return; m_zRotationAngle = angle; break;
    }
    applyTransformations();
	modified();
}

double GraphicsItemModifier::rotation(Qt::Axis axis) const
{
    switch (axis) {
        case Qt::XAxis: return m_xRotationAngle;
        case Qt::YAxis: return m_yRotationAngle;
        case Qt::ZAxis: return m_zRotationAngle;
    }
    // suppress warnings, can't reach here
    return 0.0;
}

void GraphicsItemModifier::setChildrenVisible(bool _Value)
{
	QList<QGraphicsItem *> Items = Item->childItems(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{		
		(*it)->setVisible(_Value); 
	}

}

void GraphicsItemModifier::modified()
{
	if (TemplateScene* Scene = qobject_cast<TemplateScene*>(item()->scene()))
	{
		Scene->modified();
	}
}

void GraphicsItemModifier::clearChanges()
{
	if (TemplateScene* Scene = qobject_cast<TemplateScene*>(item()->scene()))
	{
		Scene->clearChanges();
	}
}
