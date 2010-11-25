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
#ifndef SACTIONBUTTONSFRAME_H
#define SACTIONBUTTONSFRAME_H

#include <QFrame>
#include "stwidgetsexport.h"

/**
Widget that creates a QToolButton for each action that we add with addAction.

	@author
*/
class QBoxLayout;
class QActionGroup; 
class ST_WIDGETS_EXPORT SActionButtonsFrame : public QFrame
{
	Q_OBJECT
			
public:
	typedef enum EnButtonsLayout
	{
		LayoutHorizontal, 
  		LayoutVertical, 
	 	LayoutFlow
	} TEnButtonsLayout;
	
private:
	QLayout* MLayout; 
	QSize IconSize;
	Qt::ToolButtonStyle ButtonStyle;
	//EnButtonsLayout ButLayout;
	TEnButtonsLayout ButLayout;

public:
	SActionButtonsFrame(QWidget* _Parent = 0, TEnButtonsLayout _ButLayout = SActionButtonsFrame::LayoutHorizontal);
	~SActionButtonsFrame() {};
	void setIconSize(const QSize& _Size);
	void setButtonStyle(Qt::ToolButtonStyle _Style);
	void addAction(QAction* _Action);
	void addActions(QList<QAction *> _Actions);
	void addActions(QActionGroup* _ActionGroup);
};

#endif
