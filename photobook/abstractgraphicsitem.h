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
#ifndef ABSTRACTGRAPHICSITEM_H
#define ABSTRACTGRAPHICSITEM_H

#include <QTransform> 
#include "stphotobookexport.h"
#include "ichangescontrol.h"

/**
Abstract class for al St graphics items.

	@author 
*/
class QDomElement;
class QDomDocument; 
class QGraphicsItem; 
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent; 
class QGraphicsWidget; 

namespace SPhotoBook
{

class GraphicsItemModifier;

class ST_PHOTOBOOK_EXPORT AbstractGraphicsItem : public IChangesControl
{

public:
	enum EnMousePosIntersects
	{
		MouseOnNoEdge = 0,
		MouseOnLeftEdge = 1, 
		MouseOnRightEdge = 2, 
		MouseOnTopEdge = 4, 
		MouseOnBottomEdge = 8	
	};

protected:	
	static int GridValue; 
	static bool SnapToGrid; 
	EnMousePosIntersects MouseDownPosInt;
	bool ResizeAllowed;
	bool ControlsEnabled; 
	QGraphicsWidget* MControlGWidget;
	GraphicsItemModifier* Modifier;
	bool Modified;
	
public:
	AbstractGraphicsItem(QGraphicsItem* _Item);
	virtual ~AbstractGraphicsItem();
	void createStandardCorners();

	//! If _LoadDir is empty (default) all paths are handled as absolute, else all paths are relative to _StoreDir.
	virtual QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const = 0;
	//! If Storedir is empty (default) all paths are stored absolute, else path are stored relative to _LoadDir.
	virtual void loadElement(const QDomElement& _Element, const QString& _LoadDir = "") = 0;
	bool resizeAllowed() const;

	//Controls
	void setControlsEnabled(bool _Value) { ControlsEnabled = _Value; }
	void setControlsVisible(bool _Value);
	bool controlCollided(qreal& _TopZValue) const;
	void setControlsWidget(QGraphicsWidget* _GWidget); 

	virtual void setResizeAllowed(bool _Value) { ResizeAllowed = _Value; } 
	static QDomElement createTransformElement(const QGraphicsItem* _Item, QDomDocument& _Doc);
	static void appendEffectElements(QDomElement& _Parent, const QGraphicsItem* _Item, QDomDocument& _Doc);
	void loadEffectElements(QGraphicsItem* _Item, const QDomElement& _Element);
	static QTransform loadTransformElement(const QDomElement& _Element);
	static void setGridValue(int _Value) { GridValue = _Value; }
	static int gridValue() { return GridValue; }
	static void setSnapToGrid(bool _Value) { SnapToGrid = _Value; }
	static bool snapToGrid() { return SnapToGrid; }
	static qreal snapToGridValue(qreal _Value);
	GraphicsItemModifier* modifier() { return Modifier; }

	virtual AbstractGraphicsItem* clone() const = 0;

protected:
	virtual QSizeF mouseIntersecsThreshold(QGraphicsItem* _Item);
	EnMousePosIntersects getMouseIntersects(const QPointF& _Pos, const QRectF& _ItemBoudingRect, const QSizeF& _Threshold = QSizeF(5, 5));
	void scaleOnMouseMoveEvent(QGraphicsSceneMouseEvent* _Event, QGraphicsItem* _Item);
	void hoverMoveEvent(QGraphicsSceneHoverEvent * _Event, QGraphicsItem* _Item);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* _Event, QGraphicsItem* _Item);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* _Event, QGraphicsItem* _Item);
	void mousePressEvent(QGraphicsSceneMouseEvent* _Event, QGraphicsItem* _Item);
	void updateToolTip();
	void updateToolTip(const QRectF& _Rect);
	//IChangesControl interface
	void modified();
	void clearChanges();
};
}
#endif
