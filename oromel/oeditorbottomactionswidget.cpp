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
#include "oeditorbottomactionswidget.h"
#include <QLayout> 
#include <QToolBar> 
#include <QAction> 
#include <QComboBox>
#include <QUndoStack>
#include <QUndoView> 
#include <QWhatsThis>

OEditorBottomActionsWidget::OEditorBottomActionsWidget(QUndoStack* _UndoStack, QWidget *parent)
 : QFrame(parent), UndoStack(_UndoStack), ResettingZoom(false)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setMargin(1);
	
	MUndoView = new QUndoView(_UndoStack);
	MUndoView->setVisible(false);
	
	QToolBar* ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	MLayout->addWidget(ToolBar);

	QAction* QWhatsThisAct = QWhatsThis::createAction(this); 
	ToolBar->addAction(QWhatsThisAct);

	
	QAction* UndoAct = UndoStack->createUndoAction(this);//new QAction(QIcon(":/undo.png"), tr("Undo"), this);
	UndoAct->setIcon(QIcon(":/undo.png")); 
	ToolBar->addAction(UndoAct);
	
	QAction* RedoAct = UndoStack->createRedoAction(this);//new QAction(QIcon(":/redo.png"), tr("Redo"), this);
	RedoAct->setIcon(QIcon(":/redo.png")); 
	ToolBar->addAction(RedoAct);
	
	QAction* UndoViewAct = new QAction(QIcon(":/st/oromel/undoview.png"), tr("History"), this);
	connect(UndoViewAct, SIGNAL(triggered( bool )), this, SLOT(showUndoView()));
	ToolBar->addAction(UndoViewAct);
	
	ToolBar->addSeparator();
	 
	QAction* ZoomMagAct = new QAction(QIcon(":/st/oromel/viewmag+.png"), tr("Zoom+"), this);
	connect(ZoomMagAct, SIGNAL(triggered( bool )), this, SLOT(zoomMag())); 
	ZoomMagAct->setStatusTip(tr("Zoom magnify."));
	ToolBar->addAction(ZoomMagAct);
	
	ZoomCombo = new QComboBox(this); 
	ZoomCombo->addItem("10%", 0.1);
	ZoomCombo->addItem("25%", 0.25);
	ZoomCombo->addItem("35%", 0.35);
	ZoomCombo->addItem("40%", 0.40);
	ZoomCombo->addItem("50%", 0.5);
	ZoomCombo->addItem("75%", 0.75);
	ZoomCombo->addItem("85%", 0.85);
	ZoomCombo->addItem("100%", 1);
	ZoomCombo->addItem("110%", 1.1);
	ZoomCombo->addItem("125%", 1.25);
	ZoomCombo->addItem("150%", 1.5);
	ZoomCombo->addItem("175%", 1.75);
	ZoomCombo->addItem("200%", 2);
	ZoomCombo->addItem("220%", 2.2);
	ZoomCombo->addItem("240%", 2.4);
	ZoomCombo->addItem("260%", 2.6);
	ZoomCombo->addItem("280%", 2.8);
	ZoomCombo->addItem("300%", 3);
	ZoomCombo->addItem("320%", 3.2);
	ZoomCombo->addItem("340%", 3.4);
	ZoomCombo->addItem("360%", 3.6);
	ZoomCombo->addItem("380%", 3.8);
	ZoomCombo->addItem("400%", 4);
	ZoomCombo->addItem("420%", 4.2);
	ZoomCombo->addItem("450%", 4.5);
	ToolBar->addWidget(ZoomCombo);
	connect(ZoomCombo, SIGNAL(currentIndexChanged( int )), this, SLOT(zoomComboChanged(int ))); 
	
	QAction* ZoomMinAct = new QAction(QIcon(":/st/oromel/viewmag-.png"), tr("Zoom-"), this);
	connect(ZoomMinAct, SIGNAL(triggered( bool )), this, SLOT(zoomMin())); 
	ZoomMinAct->setStatusTip(tr("Zoom minimize."));
	ToolBar->addAction(ZoomMinAct);
	
	QAction* FitToWinAct = new QAction(QIcon(":/st/oromel//fittowin.png"), tr("Window Fit"), this);
	connect(FitToWinAct, SIGNAL(triggered( bool )), this, SIGNAL(zoomFitToWindow())); 
	FitToWinAct->setStatusTip(tr("Fit To window."));
	ToolBar->addAction(FitToWinAct);
	
	QAction* FitToSelAct = new QAction(QIcon(":/st/oromel//fittosel.png"), tr("Selection Fit"), this);
	connect(FitToSelAct, SIGNAL(triggered( bool )), this, SIGNAL(zoomFitToSelection())); 
	FitToSelAct->setStatusTip(tr("Fit To selection."));
	//FitToSelAct->setVisible(false); 
	ToolBar->addAction(FitToSelAct);
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	
}


OEditorBottomActionsWidget::~OEditorBottomActionsWidget()
{
	delete MUndoView; 
}

void OEditorBottomActionsWidget::resetZoom()
{
	ResettingZoom = true; 
	int ResetIndex = qMax(ZoomCombo->findData(1), 0); 
	ZoomCombo->setCurrentIndex(ResetIndex); 
	ResettingZoom = false; 
}

void OEditorBottomActionsWidget::setZoomValue(double _Val)
{
	//If the zoom value does not exist we insert it.
	int IntPart = static_cast<int>(_Val * 100);
	double NVal2Dec = static_cast<double>(IntPart) / 100;
	int Cnt = 0; 
	bool Found = false; 
	double CurrentVal = 0; 
	while ( !Found && Cnt < ZoomCombo->count() && CurrentVal < NVal2Dec)
	{
		CurrentVal = ZoomCombo->itemData(Cnt).toDouble();
		Found = CurrentVal == NVal2Dec; 
		if (!Found && CurrentVal < NVal2Dec)
			Cnt++;
	}
	if (!Found)
	{
		ZoomCombo->insertItem(Cnt, QString("%1%").arg(IntPart), NVal2Dec);
	}
	ZoomCombo->setCurrentIndex(Cnt);  
}


void OEditorBottomActionsWidget::showUndoView()
{
	MUndoView->show();
}

void OEditorBottomActionsWidget::zoomComboChanged(int )
{
	if (!ResettingZoom)
		emit zoomValueChanged(ZoomCombo->itemData(ZoomCombo->currentIndex()).toDouble()); 
}

void OEditorBottomActionsWidget::zoomMag()
{
	if (ZoomCombo->currentIndex() < ZoomCombo->count())
		ZoomCombo->setCurrentIndex(ZoomCombo->currentIndex() + 1); 
}

void OEditorBottomActionsWidget::zoomMin()
{
	if (ZoomCombo->currentIndex() > 0)
		ZoomCombo->setCurrentIndex(ZoomCombo->currentIndex() -1); 
}

