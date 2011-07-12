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

#ifndef UPDATEITEMEVENT_H
#define UPDATEITEMEVENT_H
#include <QEvent>
#include <QPointer>
#include "stphotobookexport.h"
#include "graphicsphotoitem.h"


/**
	Event to inform an intem update.
*/
namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT UpdateItemEvent : public QEvent
{
public:
	static const QEvent::Type UpdateItemEventType;
	
private:
	QPointer<GraphicsPhotoItem> Item;
	
public:
	UpdateItemEvent(GraphicsPhotoItem* _Item) : QEvent(UpdateItemEventType), Item(_Item) {}
	QPointer<GraphicsPhotoItem> item() const { return Item; }
};
}
#endif
