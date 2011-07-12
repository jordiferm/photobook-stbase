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
#include "corneritem.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <math.h>

#include "graphicsitemmodifier.h"

using namespace STPhotoBook;

CornerItem::CornerItem(Qt::Corner corner, int _OpMask, GraphicsItemModifier* _Modifier)
    : QGraphicsItem(_Modifier->item())
    , m_modifier(_Modifier)
    , m_opMask(_OpMask)
    , m_corner(corner)
    , m_side(8)
    , m_operation(Off)
{
    setAcceptsHoverEvents(true);
}

void CornerItem::relayout(const QRect & rect)
{
    // change side, if needed
    int side = 1 + (int)sqrt((float)qMin(rect.width(), rect.height()));
    if (side != m_side) {
        prepareGeometryChange();
        m_side = side;
    }

    // place at the right corner
    switch (m_corner) {
        case Qt::TopLeftCorner: setPos(rect.topLeft() + QPoint(m_side, m_side)); break;
        case Qt::TopRightCorner: setPos(rect.topRight() + QPoint(-m_side + 1, m_side)); break;
        case Qt::BottomLeftCorner: setPos(rect.bottomLeft() + QPoint(m_side, -m_side + 1)); break;
        case Qt::BottomRightCorner: setPos(rect.bottomRight() + QPoint(-m_side + 1, -m_side + 1)); break;
    }
}

QRectF CornerItem::boundingRect() const
{
    return QRectF(-m_side, -m_side, 2*m_side, 2*m_side);
}

void CornerItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
//     event->accept();
// 
//     // do the right op
//     switch (event->button()) {
//         case Qt::LeftButton:    if (m_opMask & Scale) m_modifier->resetContentsRatio(); break;
//         case Qt::RightButton:   if (m_opMask & Rotate) m_modifier->setRotation(0.0, Qt::ZAxis); break;
//         default:                break;
//     }
}

void CornerItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();

    // do the right op
    switch (event->button()) {
		case Qt::LeftButton:    m_operation = Rotate | Scale | Move; break;
        case Qt::RightButton:   m_operation = Scale | FixScale; break;
        case Qt::MidButton:     m_operation = Scale; break;
        default:                m_operation = Off; return;
    }

    // filter out unwanted operations
    m_operation &= m_opMask;

    // intial parameters
    QRect contentsRect = m_modifier->contentsRect();
    m_startRatio = (double)contentsRect.width() / (double)contentsRect.height();

    update();
}

void CornerItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{

    if (m_operation == Off)
        return;
    event->accept();

    // modify the operation using the shortcuts
     int op = m_operation;
    if (event->modifiers() & Qt::ShiftModifier)
        op &= ~FixScale & ~Rotate;
    if (event->modifiers() & Qt::ControlModifier)
        op |= Rotate;
    if (event->modifiers() & Qt::AltModifier)
        op |= FixRotate;
    op &= m_opMask;
    if ((op & (Rotate | Scale)) == (Rotate | Scale))
        op |= FixScale;
	if ((op & (Rotate | Scale | Move)) == Off)
        return;

    // vector relative to the centre
    QPointF v = pos() + event->pos();
    if (v.isNull())
        return;

    // do scaling
//     //m_modifier->delayedDirty();
//     if (op & Scale) {
//         if (op & FixScale) {
//             const double r = m_startRatio;
//             const double D = sqrt(v.x()*v.x() + v.y()*v.y());
//             const double K = sqrt(1 + 1/(r * r));
//             int W = qMax((int)((2*D)/(K)), 50);
//             int H = qMax((int)((2*D)/(r*K)), 40);
//             m_modifier->resizeContents(QRect(-W / 2, -H / 2, W, H));
//         } else {
//             int W = qMax(2 * fabs(v.x()), 50.0); //(m_modifiersRect.width() * v.x()) / oldPos.x();
//             int H = qMax(2 * fabs(v.y()), 40.0); //(m_modifiersRect.height() * v.y()) / oldPos.y();
//             //if (W != (int)cRect.width() || H != (int)cRect.height())
//                 m_modifier->resizeContents(QRect(-W / 2, -H / 2, W, H));
//         }
//     }
	//op |= FixRotate;
	if (m_corner == Qt::BottomRightCorner && (op & Scale))
	{
		// do scaling
			m_modifier->setRectBottomRight(event->pos(), this);
	}
	else 
	if (op & Move )
	{
		// do moving
			m_modifier->setPos(QPointF(event->pos().x() - boundingRect().width() / 2,
									   event->pos().y() - boundingRect().height() / 2), this);
	}
	else
	{
		// do rotation
		if (op & Rotate) 
		{
			QPointF refPos = pos();
	
			// set item rotation (set rotation relative to current)
			qreal refAngle = atan2(refPos.y(), refPos.x());
			qreal newAngle = atan2(v.y(), v.x());
			double dZr = 57.29577951308232 * (newAngle - refAngle); // 180 * a / M_PI
			double zr = m_modifier->rotation(Qt::ZAxis) + dZr;
	
			// snap to M_PI/4
				if ((op & FixRotate) || (abs(zr) < 3.0) ) {
					int fracts = (int)((zr - 7.5) / 15.0);
					zr = (qreal)fracts * 15.0;
				}
	
			// apply rotation
			m_modifier->setRotation(zr, Qt::ZAxis);
		}
	}
}

void CornerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    bool accepted = m_operation != Off;
    m_operation = Off;
    update();

    // clicked
    if (accepted) {
    }
}

void CornerItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
//     if (RenderOpts::HQRendering)
//         return;

//    QColor color = RenderOpts::hiColor;
	QColor color = Qt::red; 
    if (option->state & QStyle::State_MouseOver) {
        if (m_operation != Off)
            color.setAlpha(250);
        else
            color.setAlpha(196);
    } else
        color.setAlpha(128);
    painter->fillRect(boundingRect(), color);
	if (!PixmapName.isEmpty())
	{
		QPixmap Pix(PixmapName);
		painter->drawPixmap(boundingRect(), Pix, Pix.rect() );
	}
}
