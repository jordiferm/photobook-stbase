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

#ifndef STGRAPHICSPAGEITEM_H
#define STGRAPHICSPAGEITEM_H

#include "stgraphicsphotoitem.h"
#include "stabstractgraphicsitem.h"
#include "stphototemplatesexport.h"

/**
Item class for pages.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STGraphicsPageItem : public STGraphicsPhotoItem
{
public:	
	enum { Type = UserType + 4 };
	
	

private:
	void init();
	
protected:
	virtual void updateCursor();

public:
	STGraphicsPageItem(const QRectF & rect, QGraphicsItem * parent = 0);
	STGraphicsPageItem(QGraphicsItem * parent = 0);
	int type() const { return Type; }	
	void loadElement(QDomElement& _Element);
	void setResizeAllowed(bool _Value);
	QDomElement createElement(QDomDocument& _Doc);
	static QString tagName() { return "pageitem"; }	
	
};

#endif
