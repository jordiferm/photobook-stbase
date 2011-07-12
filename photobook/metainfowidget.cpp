/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "metainfowidget.h"
#include <QLayout>
#include <QToolBar>
#include <QTabWidget>
#include <QAction>

using namespace SPhotoBook;

void MetaInfoWidget::createActions()
{
	QAction* DefaultAct = new QAction(QIcon(":/defaults.png"), tr("Reset to defaults"), this);
	connect(DefaultAct, SIGNAL(triggered()), this, SLOT(slotHelp()));
	ToolBar->addAction(DefaultAct);

	QAction* HelpAct = new QAction(QIcon(":/help.png"), tr("&Help"), this);
	connect(HelpAct, SIGNAL(triggered()), this, SLOT(slotHelp()));
	ToolBar->addAction(HelpAct);

}

MetaInfoWidget::MetaInfoWidget(QWidget *parent) :
    QWidget(parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolBar->setIconSize(QSize(32,32));
	MLayout->addWidget(ToolBar);

	createActions();

	QTabWidget* TabWidget = new QTabWidget(this);
	MLayout->addWidget(TabWidget);

	setGeometry(0, 0, 200, 200);
}

//TODO: Accept Cancel Dialog in widgets !!!!.
