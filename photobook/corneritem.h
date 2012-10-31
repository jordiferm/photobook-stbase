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
#ifndef CORNERITEM_H
#define CORNERITEM_H

#include <QGraphicsItem>


#include <QGraphicsItem>
#include <QPointF>


/**
	@author
*/
namespace SPhotoBook
{
class GraphicsItemModifier;
class CornerItem : public QGraphicsItem
{
public:	
	enum { Type = UserType + 7 };

public:
	enum Operation
	{
		Off         = 0x0000,
		Rotate      = 0x0001,
		FixRotate   = 0x0002,
		Scale       = 0x0010,
		FixScale    = 0x0020,
		Move		= 0x0040,
        SetWidth	= 0x0080,
        AllowAll    = 0xFFFF
	};

private:
	GraphicsItemModifier* m_modifier;
	Qt::Corner m_corner;
	int m_opMask;
	int m_side;
	int m_operation;
	double m_startRatio;
	QString PixmapName;


public:
	CornerItem(Qt::Corner corner, int _OpMask, GraphicsItemModifier* _Modifier);
	void relayout(const QRect & rect);
	int type() const { return Type; }	
	// ::QGraphicsItem
	QRectF boundingRect() const;
	void setPixmapName(const QString& _Value) { PixmapName = _Value; }
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	void mousePressEvent(QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};
}
#endif
