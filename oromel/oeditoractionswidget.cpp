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
#include "oeditoractionswidget.h"
#include <QLayout> 
#include <QLabel>
#include <QAction>
#include <QUndoStack> 
#include <QToolBar> 

#include "sactionbuttonsframe.h"
#include "oimagenavigatorwidget.h"
#include "siconfactory.h"

OEditorActionsWidget::OEditorActionsWidget(QUndoStack* _UndoStack, QWidget *parent)
 : QWidget(parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	MLayout->setMargin(0);
	QToolBar* MToolBar = new QToolBar(this); 
	MToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 
	MToolBar->setIconSize(QSize(32, 32));
	//SActionButtonsFrame* MActBFrame = new SActionButtonsFrame(this); 
	MLayout->addWidget(MToolBar); 
	
	MLayout->addItem(new QSpacerItem(25,0, QSizePolicy::Maximum, QSizePolicy::Preferred));
	
	//Image Navigator 
	//TODO: Utilitzar un ProxyModel que nom�s ensenyi l'actual +- 3 per a fer un preview de la navegaci�.
	ImageNavWidget = new OImageNavigatorWidget(this);
	MLayout->addWidget(ImageNavWidget);
		
	// Actions setup 
	QAction* BackAct = new QAction(stIcon(SIconFactory::Start), tr("Back"), this);
	BackAct->setToolTip(tr("Back to collection manager."));
	connect(BackAct, SIGNAL(triggered()), this, SIGNAL(backClicked()));	
	MToolBar->addAction(BackAct);
	
	SaveAct = new QAction(stIcon(SIconFactory::FileSave), tr("Save"), this);
	SaveAct->setToolTip(tr("Save edited image."));
	connect(SaveAct, SIGNAL(triggered()), this, SIGNAL(saveClicked()));	
	connect(_UndoStack, SIGNAL(cleanChanged( bool )), SaveAct, SLOT(setDisabled(bool )));
	MToolBar->addAction(SaveAct);
	
	QAction* SaveAsAct = new QAction(stIcon(SIconFactory::FileSaveAs), tr("Save As"), this);
	SaveAsAct->setToolTip(tr("Save edited image with new name."));
	connect(SaveAsAct, SIGNAL(triggered()), this, SIGNAL(saveAsClicked()));	
	MToolBar->addAction(SaveAsAct);
	
	QAction* OpenFileAct = new QAction(stIcon(SIconFactory::FileOpen), tr("Open"), this);
	OpenFileAct->setToolTip(tr("Open an image file to edit."));
	connect(OpenFileAct, SIGNAL(triggered()), this, SIGNAL(openClicked()));	
	MToolBar->addAction(OpenFileAct);
	
	OriginalAct = new QAction(QIcon(":/st/oromel/restore.png"), tr("Original"), this);
	OriginalAct->setToolTip(tr("Restore to oritinal image."));
	connect(OriginalAct, SIGNAL(triggered()), this, SIGNAL(revertOriginalClicked()));	
	connect(OriginalAct, SIGNAL(triggered()), this, SLOT(disableOriginalAct()));
	connect(_UndoStack, SIGNAL(indexChanged(int )), this, SLOT(undoIndexChanged(int )));
	MToolBar->addAction(OriginalAct);

	QAction* HistogramAct = new QAction(QIcon(":/st/oromel/view_statistics.png"), tr("Histogram"), this);
	HistogramAct->setToolTip(tr("Opens histogram dialog."));
	connect(HistogramAct, SIGNAL(triggered()), this, SIGNAL(histogramClicked()));	
	MToolBar->addAction(HistogramAct);

	AcceptAct = new QAction(stIcon(SIconFactory::Accept), tr("Accept"), this);
	AcceptAct->setToolTip(tr("Save changes and exists."));
	connect(AcceptAct, SIGNAL(triggered()), this, SIGNAL(acceptClicked()));	
	MToolBar->addAction(AcceptAct);
		
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
}

OEditorActionsWidget::~OEditorActionsWidget()
{
}

void OEditorActionsWidget::init()
{
	SaveAct->setEnabled(false);
	OriginalAct->setEnabled(false);
}

void OEditorActionsWidget::setOriginalEnabled(bool _Enabled)
{
	OriginalAct->setEnabled(_Enabled);
}

void OEditorActionsWidget::setOriginalVisible(bool _Visible)
{
	OriginalAct->setVisible(_Visible);
}

void OEditorActionsWidget::setAcceptVisible(bool _Visible)
{
	AcceptAct->setVisible(_Visible); 
}

void OEditorActionsWidget::disableOriginalAct()
{
	OriginalAct->setEnabled(false);
}

void OEditorActionsWidget::undoIndexChanged(int /*_Index*/)
{
	OriginalAct->setEnabled(true);
}

