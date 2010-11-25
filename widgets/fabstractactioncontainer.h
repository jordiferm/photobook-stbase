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

#ifndef FABSTRACTACTIONCONTAINER_H
#define FABSTRACTACTIONCONTAINER_H
#include "stwidgetsexport.h"

/**
Contenidor abstracte d'actions.
Donat que addAction de QWidget no és virtual.
 
@author Shadow
*/
class QActionGroup;
class QAction;
class QWidget;
class FAction;
class ST_WIDGETS_EXPORT FAbstractActionContainer
{
	QWidget* Container;
	
public:
	FAbstractActionContainer(QWidget* _Container = 0);
	virtual ~FAbstractActionContainer() {}
	virtual void addAction(QAction* _Action);
	virtual void insertAction(int _Pos, QAction* _Action);
	void addActions(const QActionGroup& _Actions); 
	FAction* fAction(int _Id ) const;
	virtual void removeAction(int _ActionId); 

};

#endif
