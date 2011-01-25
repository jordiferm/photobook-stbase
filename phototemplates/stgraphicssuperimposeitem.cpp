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

#include "stgraphicssuperimposeitem.h"
#include <QBrush> 

#include <QStyleOptionGraphicsItem>


void STGraphicsSuperImposeItem::init()
{
	//setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
	setAspectRatioMode(Qt::IgnoreAspectRatio); 
	#if QT_VERSION >= 0x040600
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemUsesExtendedStyleOption);
	#else
	setFlags(QGraphicsItem::ItemIsSelectable);
	#endif
	setBrush(QBrush(QColor(255,255,255,0)));
	setAcceptsHoverEvents (false);
	setAcceptedMouseButtons(Qt::NoButton); 
	setAcceptDrops(false); 
}


STGraphicsSuperImposeItem::STGraphicsSuperImposeItem(const QRectF& _Rect, QGraphicsItem* _Parent) : STGraphicsPhotoItem(_Rect, _Parent)
{
	init();
}

STGraphicsSuperImposeItem::STGraphicsSuperImposeItem(QGraphicsItem* _Parent) : STGraphicsPhotoItem(_Parent)
{
	init();
}


QDomElement STGraphicsSuperImposeItem::createElement(QDomDocument& _Doc)
{
	QDomElement  Res = STGraphicsPhotoItem::createElement(_Doc);
	Res.setTagName(tagName()); 
	
	return Res; 
}

void STGraphicsSuperImposeItem::loadElement(QDomElement& _Element)
{
	STGraphicsPhotoItem::loadElement(_Element);
}

