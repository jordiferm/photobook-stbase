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

#ifndef STGRAPHICSSUPERIMPOSEITEM_H
#define STGRAPHICSSUPERIMPOSEITEM_H

#include "stgraphicsphotoitem.h"
#include "stabstractgraphicsitem.h"
#include "stphototemplatesexport.h"

/**
Item class for pages.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STGraphicsSuperImposeItem : public STGraphicsPhotoItem
{
Q_OBJECT
public:	
	enum { Type = UserType + 5 };
	
private:
	void init();
	
public:
	STGraphicsSuperImposeItem(const QRectF & rect, QGraphicsItem * parent = 0);
	STGraphicsSuperImposeItem(QGraphicsItem * parent = 0);
	int type() const { return Type; }	
	void loadElement(QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc);
	static QString tagName() { return "superimposeitem"; }		


};

#endif
