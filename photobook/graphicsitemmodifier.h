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
#ifndef GRAPHICSITEMMODIFIER_H
#define GRAPHICSITEMMODIFIER_H

#include <Qt> 
#include <QRect>
#include <QTransform>
#include "ichangescontrol.h"
#include "stphotobookexport.h"

/**
	@author
*/
class QGraphicsItem; 
class QRect; 
class QGraphicsSceneMouseEvent; 

namespace SPhotoBook
{
class GraphicsPhotoItem;
class ST_PHOTOBOOK_EXPORT GraphicsItemModifier : public IChangesControl
{
    QGraphicsItem* Item;
    double              m_xRotationAngle;
	double              m_yRotationAngle;
	double              m_zRotationAngle;
	double              CScaleX;
    double              CScaleY;
    QTransform          InitTransform;
	QRect               m_contentsRect;

	void applyTransformations();

public:
	GraphicsItemModifier(QGraphicsItem* _Item);
	QGraphicsItem* item() const { return Item; }
    void updateInitTransform();
    QRect contentsRect() const;
	void updateToolTip();
	void updateToolTip(const QRectF& _Rect);
	void resetContentsRatio();
	QPointF snapToBoundsBottomRight(QPointF _Point, GraphicsPhotoItem* _Item);
	QPointF snapToBounds(QPointF _Point, GraphicsPhotoItem* _Item);
	void scale(double _Sx, double _Sy);
	void setPos(const QPointF& _Pos, QGraphicsItem* _Sender);
    void setItemWidth(const QPointF& _Pos, QGraphicsItem* _Sender);
    void setRectBottomRight(const QPointF& _Pos, QGraphicsItem* _Sender, bool _FixScale);
	void resizeContents(const QRect & rect, bool keepRatio = false);
	void layoutChildren();
	void rotate(double _Angle);
    double zRotationAngle() const { return m_zRotationAngle; }
	void setRotation(double angle, Qt::Axis axis);
	double rotation(Qt::Axis axis) const;
	void setChildrenVisible(bool _Value); 
	//IChangesControl interface
	void modified();
	void clearChanges();
};
}
#endif
