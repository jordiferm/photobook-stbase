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
#include "sactionbuttonsframe.h"
#include <QLayout> 
#include <QToolButton> 
#include <QActionGroup>
#include "flowlayout.h"

SActionButtonsFrame::SActionButtonsFrame(QWidget* _Parent, SActionButtonsFrame::EnButtonsLayout _ButLayout)
		: QFrame(_Parent), ButLayout(_ButLayout)
{
	switch (_ButLayout)
	{
		case LayoutHorizontal : 
			MLayout = new QHBoxLayout(this); 
		break; 
		case LayoutVertical: 
			MLayout = new QVBoxLayout(this); 
		break; 
		case LayoutFlow: 
			MLayout = new FlowLayout(this); 
		break; 
	}
	MLayout->setMargin(2);
	setIconSize(QSize(32, 32));
	setButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void SActionButtonsFrame::setIconSize(const QSize& _Size)
{
	IconSize = _Size; 
}

void SActionButtonsFrame::setButtonStyle(Qt::ToolButtonStyle _Style)
{
	ButtonStyle = _Style; 
}

void SActionButtonsFrame::addAction(QAction* _Action)
{
	QToolButton* AButton = new QToolButton(this); 
	AButton->setObjectName("SActionButton");
	AButton->setFocusPolicy(Qt::StrongFocus);
	AButton->setDefaultAction(_Action); 
	AButton->setToolButtonStyle(ButtonStyle); 
	AButton->setIconSize(IconSize);
	MLayout->addWidget(AButton);
}

void SActionButtonsFrame::addActions(QList<QAction *> _Actions)
{
	QList<QAction *>::iterator it; 
	for (it = _Actions.begin(); it != _Actions.end(); ++it)
		addAction(*it); 
}

void SActionButtonsFrame::addActions(QActionGroup* _ActionGroup)
{
	addActions(_ActionGroup->actions());
}
