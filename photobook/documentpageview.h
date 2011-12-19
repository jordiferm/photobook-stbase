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

#ifndef DocumentPageView_H
#define DocumentPageView_H

#include <QGraphicsView>
#include "stphotobookexport.h"
#include "metainfo.h"

/**
	@author Jordi Fernandez <shadow@softwarelliure.com>
*/
class QShowEvent;
namespace SPhotoBook
{
class TemplateScene;
class ST_PHOTOBOOK_EXPORT DocumentPageView : public QGraphicsView
{
Q_OBJECT
public:
	typedef QList<QRectF> TMarginRectList;

private:
	QMatrix NoScaleMatrix; 
	TMarginRectList MarginRects;
	bool DrawTwoPagesEffect; 
	qreal totalScaleFactor;
	
public:
	DocumentPageView(TemplateScene* _Scene, QWidget* _Parent = 0);
	virtual ~DocumentPageView();
	void setScale(qreal _ScaleXY); 
	QRectF selectionBoundingRect() const;
	void selectAll();
	void setMarginRects(const TMarginRectList& _MarginRects) { MarginRects = _MarginRects; }
	void setDrawTwoPagesEffect(bool _Value) { DrawTwoPagesEffect = _Value; }

protected:
	void showEvent(QShowEvent* _Event);
	void drawForeground(QPainter* _Painter, const QRectF& _Rect);
	bool viewportEvent(QEvent *event);
};
}
#endif
