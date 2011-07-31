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

#include "graphicspageitem.h"
#include <QBrush> 
#include <QDomElement>

#include <QStyleOptionGraphicsItem>

using namespace SPhotoBook;

void GraphicsPageItem::init()
{
	//setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
	//setAspectRatioMode(Qt::IgnoreAspectRatio); 
	#if QT_VERSION >= 0x040600
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemUsesExtendedStyleOption);
	#else
	setFlags(QGraphicsItem::ItemIsSelectable);
	#endif
	setBrush(QBrush(Qt::white));
	setAcceptsHoverEvents (false);
	setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
	setToolTip("");
	ResizeAllowed = false; 
	setControlsEnabled(false); 
	setShowNoImageMessage(false);
	setAutoAdjustFramesToImages(false);
}

void GraphicsPageItem::updateCursor()
{
	//FIXME No es crida mai !
	if (PanningEnabled && ImageLoaded)
		setCursor(Qt::OpenHandCursor); 
	else 
		unsetCursor(); 
}

GraphicsPageItem::GraphicsPageItem(const QRectF& _Rect, QGraphicsItem* _Parent) : GraphicsPhotoItem(_Parent)
{
	init();
	setRect(_Rect);
}

GraphicsPageItem::GraphicsPageItem(QGraphicsItem* _Parent) : GraphicsPhotoItem(_Parent)
{
	init();
}


void GraphicsPageItem::setResizeAllowed(bool _Value)
{ //Does nothing  
} 

QDomElement GraphicsPageItem::createElement(QDomDocument& _Doc, const QString& _StoreDir) const
{
	QDomElement  Res = GraphicsPhotoItem::createElement(_Doc, _StoreDir);
	Res.setTagName(tagName()); 
	Res.setAttribute("bgcolor", brush().color().name());
	
	return Res; 
}

void GraphicsPageItem::loadElement(QDomElement& _Element, const QString& _LoadDir)
{
	GraphicsPhotoItem::loadElement(_Element, _LoadDir);
	setBrush(QBrush(QColor(_Element.attribute("bgcolor", "#FFFFFF"))));
	setToolTip("");
}
