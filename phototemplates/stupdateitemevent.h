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

#ifndef STUPDATEITEMEVENT_H
#define STUPDATEITEMEVENT_H
#include <QEvent>
#include <QPointer>
#include "stphototemplatesexport.h"
#include "stgraphicsphotoitem.h" 


/**
	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STUpdateItemEvent : public QEvent
{
public:
	static const QEvent::Type STUpdateItemEventType;
	
private:
	QPointer<STGraphicsPhotoItem> Item;
	
public:
    STUpdateItemEvent(STGraphicsPhotoItem* _Item) : QEvent(STUpdateItemEventType), Item(_Item) {}
	QPointer<STGraphicsPhotoItem> item() const { return Item; }
};

#endif
