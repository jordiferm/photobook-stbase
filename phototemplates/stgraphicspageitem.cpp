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

#include "stgraphicspageitem.h"
#include <QBrush> 

#include <QStyleOptionGraphicsItem>


void STGraphicsPageItem::init()
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
}

void STGraphicsPageItem::updateCursor()
{
	//FIXME No es crida mai !
	if (PanningEnabled && ImageLoaded)
		setCursor(Qt::OpenHandCursor); 
	else 
		unsetCursor(); 
}

STGraphicsPageItem::STGraphicsPageItem(const QRectF& _Rect, QGraphicsItem* _Parent) : STGraphicsPhotoItem(_Rect, _Parent)
{
	init();
}

STGraphicsPageItem::STGraphicsPageItem(QGraphicsItem* _Parent) : STGraphicsPhotoItem(_Parent)
{
	init();
}


void STGraphicsPageItem::setResizeAllowed(bool _Value) 
{ //Does nothing  
} 

QDomElement STGraphicsPageItem::createElement(QDomDocument& _Doc)
{
	QDomElement  Res = STGraphicsPhotoItem::createElement(_Doc);
	Res.setTagName(tagName()); 
	Res.setAttribute("bgcolor", brush().color().name());
	
	return Res; 
}

void STGraphicsPageItem::loadElement(QDomElement& _Element)
{
	STGraphicsPhotoItem::loadElement(_Element);
	setBrush(QBrush(QColor(_Element.attribute("bgcolor", "#FFFFFF"))));
	setToolTip("");
}
