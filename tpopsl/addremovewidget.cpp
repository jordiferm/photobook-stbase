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
#include "addremovewidget.h"
#include <QToolButton> 
#include <QLayout> 

AddRemoveWidget::AddRemoveWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
	QVBoxLayout* ARItemsLayout = new QVBoxLayout(this); 
	ARItemsLayout->setMargin(0);
	ARItemsLayout->setSpacing(0);
	AddItemsBut = new QToolButton(this);
	connect(AddItemsBut, SIGNAL(clicked( bool )), this, SIGNAL(addClicked())); 
	AddItemsBut->setIcon(QIcon(":/st/tpopsl/add.png"));
	AddItemsBut->setIconSize(QSize(48, 48)); 
	ARItemsLayout->addWidget(AddItemsBut); 

	RemoveItemsBut = new QToolButton(this);
	connect(RemoveItemsBut, SIGNAL(clicked( bool )), this, SIGNAL(removeClicked())); 
	RemoveItemsBut->setIcon(QIcon(":/st/tpopsl/remove.png"));
	RemoveItemsBut->setIconSize(QSize(48, 48)); 
	ARItemsLayout->addWidget(RemoveItemsBut); 
}

AddRemoveWidget::~AddRemoveWidget()
{
}

void AddRemoveWidget::setAddLabel(const QString& _Label)
{
	AddItemsBut->setText(_Label); 
	AddItemsBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
}

void AddRemoveWidget::setRemoveLabel(const QString& _Label)
{
	RemoveItemsBut->setText(_Label); 
	RemoveItemsBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
}
