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

#ifndef STALBUMPAGEGRAPHICSVIEW_H
#define STALBUMPAGEGRAPHICSVIEW_H

#include <QGraphicsView>
#include "stphototemplatesexport.h"
#include "stphotolayout.h"

/**
	@author Jordi Fernandez <shadow@softwarelliure.com>
*/
class QShowEvent;
class STTemplateScene;
class ST_PHOTOTEMPLATES_EXPORT STAlbumPageGraphicsView : public QGraphicsView
{
Q_OBJECT
	QMatrix NoScaleMatrix; 
	STPhotoBookTemplate::TMarginRectList MarginRects; 
	bool DrawTwoPagesEffect; 
	qreal totalScaleFactor;
	
public:
	STAlbumPageGraphicsView(STTemplateScene* _Scene, QWidget* _Parent = 0);
	virtual ~STAlbumPageGraphicsView();
	void setScale(qreal _ScaleXY); 
	QRectF selectionBoundingRect() const;
	void selectAll();
	void setMarginRects(const STPhotoBookTemplate::TMarginRectList& _MarginRects) { MarginRects = _MarginRects; } 
	void setDrawTwoPagesEffect(bool _Value) { DrawTwoPagesEffect = _Value; }

protected:
	void showEvent(QShowEvent* _Event);
	void drawForeground(QPainter* _Painter, const QRectF& _Rect);
	bool viewportEvent(QEvent *event);
};

#endif
