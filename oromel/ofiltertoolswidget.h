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
#ifndef OFILTERTOOLSWIDGET_H
#define OFILTERTOOLSWIDGET_H

#include "odigikamtoolswidget.h"
#include "storomelexport.h"

/**
	@author
*/
class QAction;
class ST_OROMEL_EXPORT OFilterToolsWidget : public ODigikamToolsWidget
{
	Q_OBJECT

	QAction* OilPaintAct;
	QAction* RainDropAct; 
	QAction* BlurEffectsAct; 
	QAction* CharcoalAct; 
	QAction* DistortionAct; 

public:
	OFilterToolsWidget(QWidget* parent = 0);
	~OFilterToolsWidget();
	void setupActions(QToolBar* _ToolBar);

public slots:
	void slotDoToolAction();
};

#endif
