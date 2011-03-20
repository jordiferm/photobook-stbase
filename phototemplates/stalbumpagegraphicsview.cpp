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

#include "stalbumpagegraphicsview.h"
#include <QTouchEvent>
#include "sttemplatescene.h"
#include "stgraphicsphotoitem.h"
#include "stgraphicspageitem.h"

STAlbumPageGraphicsView::STAlbumPageGraphicsView(STTemplateScene* _Scene, QWidget* _Parent)
		: QGraphicsView(_Scene, _Parent), DrawTwoPagesEffect(false)
{
	NoScaleMatrix = matrix();
	setBackgroundBrush(QBrush(QColor("#747470"))); 

	//viewport()->setAttribute(Qt::WA_AcceptTouchEvents);//For MultiTouch
	//setDragMode(ScrollHandDrag);//For MultiTouch
}

STAlbumPageGraphicsView::~STAlbumPageGraphicsView()
{
}

void STAlbumPageGraphicsView::setScale(qreal _ScaleXY)
{
	setMatrix(NoScaleMatrix);
	scale(_ScaleXY, _ScaleXY);
}


QRectF STAlbumPageGraphicsView::selectionBoundingRect() const
{
	QGraphicsScene* CScene = scene();
	QList<QGraphicsItem*> SelItems = CScene->selectedItems();
	QList<QGraphicsItem*>::iterator it;
	QRectF SelRect;
	for (it = SelItems.begin(); it != SelItems.end(); ++it)
	{
		SelRect = SelRect | (*it)->sceneBoundingRect();
	}
	return SelRect;
}


void STAlbumPageGraphicsView::showEvent(QShowEvent* _Event)
{
	STTemplateScene* Scene = static_cast<STTemplateScene*>(scene());
	QList<STGraphicsPhotoItem *> PhotoItems = Scene->photoItems();
	PhotoItems.push_back(Scene->pageItem());
	QList<STGraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		(*it)->loadImageSpawn();
	}

}

void STAlbumPageGraphicsView::drawForeground(QPainter* _Painter, const QRectF& _Rect)
{
	QGraphicsView::drawForeground(_Painter, _Rect);
	_Painter->save();
	_Painter->setClipRect(_Rect); 
	/*if (DrawTwoPagesEffect)
	{
		QRectF CPageRect = sceneRect();
		CPageRect.setWidth(sceneRect().width() / 25); 
		CPageRect.moveTo(sceneRect().x() + (sceneRect().width() / 2) - CPageRect.width(), CPageRect.y());  
		QLinearGradient Gradient(CPageRect.x(), CPageRect.y(), CPageRect.x() + CPageRect.width(), CPageRect.y());
		Gradient.setColorAt(0, QColor(0,0,0,0)); 
		Gradient.setColorAt(0.6, QColor(0,0,0,100)); 
		Gradient.setColorAt(1, QColor(0,0,0,255)); 
		_Painter->fillRect(CPageRect, QBrush(Gradient)); 
	
		CPageRect.moveTo(sceneRect().x() + (sceneRect().width() / 2), CPageRect.y());  
		QLinearGradient Gradient2(CPageRect.x(), CPageRect.y(), CPageRect.x() + CPageRect.width(), CPageRect.y());
		Gradient2.setColorAt(0, QColor(0,0,0,255)); 
		Gradient2.setColorAt(0.4, QColor(0,0,0,100)); 
		Gradient2.setColorAt(1, QColor(0,0,0,0)); 
		_Painter->fillRect(CPageRect, QBrush(Gradient2)); 
	}*/

	/*QPen MarginMarkPen(QColor(140, 140, 140, 120));
	MarginMarkPen.setWidthF(0);
	_Painter->setPen(MarginMarkPen);
	STPhotoBookTemplate::TMarginRectList::iterator it;
	for (it = MarginRects.begin(); it != MarginRects.end(); ++it)
	{
		_Painter->drawRect(*it);
	}*/

	QRegion ClipRegion;
	QPen MarginMarkPen = QPen(QColor(255, 0, 0, 170));
	MarginMarkPen.setWidthF(0);//Cosmetic pen
	//MarginMarkPen.setStyle(Qt::DashLine); //this is Too slow
	_Painter->setPen(MarginMarkPen);
	STPhotoBookTemplate::TMarginRectList::iterator it;
	for (it = MarginRects.begin(); it != MarginRects.end(); ++it)
	{
		_Painter->drawRect(*it);
		ClipRegion = ClipRegion.united(it->toRect()); 
	}
	QRegion InvClipRegion(sceneRect().toRect()); 
	InvClipRegion = InvClipRegion.subtract(ClipRegion);
	_Painter->setClipRegion(InvClipRegion, Qt::IntersectClip); 
	//_Painter->fillRect(_Rect, QBrush(QColor(255, 0, 0, 170), Qt::DiagCrossPattern));
	
	//_Painter->fillRect(_Rect, backgroundBrush());

		
	_Painter->restore();
}

bool STAlbumPageGraphicsView::viewportEvent(QEvent *event)
 {
     switch (event->type()) {
     case QEvent::TouchBegin:
     case QEvent::TouchUpdate:
     case QEvent::TouchEnd:
     {
         QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
         QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
         if (touchPoints.count() == 2) {
             // determine scale factor
             const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
             const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
             qreal currentScaleFactor =
                     QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                     / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
             if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                 // if one of the fingers is released, remember the current scale
                 // factor so that adding another finger later will continue zooming
                 // by adding new scale factor to the existing remembered value.
                 totalScaleFactor *= currentScaleFactor;
                 currentScaleFactor = 1;
             }
             setTransform(QTransform().scale(totalScaleFactor * currentScaleFactor,
                                             totalScaleFactor * currentScaleFactor));
         }
         return true;
     }
     default:
         break;
     }
     return QGraphicsView::viewportEvent(event);
 }
