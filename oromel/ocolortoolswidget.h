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
#ifndef OCOLORTOOLSWIDGET_H
#define OCOLORTOOLSWIDGET_H

#include <QWidget>
#include "odigikamtoolswidget.h" 
#include "storomelexport.h"

/**
Widget for color tools menu.

	@author
*/
class QAction;
class ST_OROMEL_EXPORT OColorToolsWidget : public ODigikamToolsWidget
{
	Q_OBJECT

	QAction* AutoCorrectionAct; 
	QAction* CurvesAdjustAct; 
	QAction* LevelsAdjustAct; 
	QAction* WhiteBalanceAct; 
	QAction* ChannelMixerAct; 
	QAction* BWSepiaAct; 
	QAction* ColorFxAct; 
	QAction* BCGAct; 
	QAction* HSLAct; 
	QAction* RGBAct; 

	
public:
	OColorToolsWidget(QWidget *parent = 0);
	~OColorToolsWidget();
	void setupActions(QToolBar* _ToolBar);

public slots:
	void slotDoToolAction();

};

#endif
