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
#include "odigikamtoolswidget.h"
#include <QVBoxLayout>
#include <QToolBar> 
#include <QAction> 
#include <QLabel> 

#include "editortoolsettings.h" 


using namespace Digikam;

ODigikamToolsWidget::ODigikamToolsWidget(const QString& _HelpText, QWidget *parent)
 : QWidget(parent), CurrTool(0)
{
	MLayout = new QHBoxLayout(this); 
	MLabel = new QLabel(this); 
	MLabel->setText(_HelpText); 
	MLabel->setTextFormat(Qt::RichText); 
	MLabel->setWordWrap(true); 
	MLayout->addWidget(MLabel); 
	MLabel->setAlignment(Qt::AlignTop); 
}

ODigikamToolsWidget::~ODigikamToolsWidget()
{
}

void ODigikamToolsWidget::unloadTool(Digikam::EditorTool* _Tool)
{
	_Tool->toolSettings()->close(); 
	MLayout->removeWidget(_Tool->toolSettings()); 
	MLabel->show(); 
	delete _Tool; 
}

void ODigikamToolsWidget::loadTool(Digikam::EditorTool* _Tool)
{
	closeCurrentTool(); 
		
	CurrTool = _Tool; 
	connect(_Tool->toolSettings(), SIGNAL(signalCancelClicked()), 
				this, SLOT(closeCurrentTool()));
	connect(_Tool, SIGNAL(okClicked()), 
				this, SLOT(slotOkClicked()));

	MLayout->addWidget(CurrTool->toolSettings() ); 
	MLabel->hide(); 
	emit toolActivated(_Tool); 
}


void ODigikamToolsWidget::closeCurrentTool()
{
	if (CurrTool)
	{
		emit toolClosed(CurrTool); 
		unloadTool(CurrTool); 
		CurrTool = 0; 
	}
}

void ODigikamToolsWidget::slotOkClicked()
{
	if (CurrTool) //Defensive 
	{
		emit imageChanged(CurrTool->toolName()); 
		closeCurrentTool(); 
	}
}
