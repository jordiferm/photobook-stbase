/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "fabstractactioncontainer.h"
#include <QActionGroup>
#include <QList>
#include "fstdactions.h"

FAbstractActionContainer::FAbstractActionContainer(QWidget* _Container) : Container(_Container)
{

}
	
void FAbstractActionContainer::addAction(QAction* _Action)
{
	if (Container)
	{
		if (!Container->actions().contains(_Action))
			Container->addAction(_Action);
	}
}

void FAbstractActionContainer::insertAction(int _Pos, QAction* _Action)
{
	addAction(_Action);
}

void FAbstractActionContainer::addActions(const QActionGroup& _Actions)
{
	QList<QAction*>::const_iterator it; 
	QList<QAction*> Actions = _Actions.actions();
	for (it = Actions.begin(); it != Actions.end(); ++it)
		addAction(*it);
}

void FAbstractActionContainer::removeAction(int _ActionId)
{
	QAction* RAction; 
	RAction = fAction(_ActionId); 
	if (RAction && Container)
		Container->removeAction(RAction); 
}


FAction* FAbstractActionContainer::fAction(int _Id ) const 
{
	FAction* Res = 0; 
	if (Container)
	{
		QList<QAction *> Actions = Container->actions(); 
		QList<QAction *>::iterator it = Actions.begin(); 
		bool Found = false; 
		while (it != Actions.end() && !Found)
		{
			if ( FAction* CAction = qobject_cast<FAction*>(*it) )
			{
				Found = CAction->id() == _Id;
				if (Found)
					Res = CAction;
			}
			++it;
		}
	}
	return Res;
}

