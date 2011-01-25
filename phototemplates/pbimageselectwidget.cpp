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
#include "pbimageselectwidget.h"
#include <QLayout> 
#include <QToolBar>
#include <QAction> 
#include <QApplication> 
#include <QKeyEvent> 
#include <QScrollBar>

//#include "tpphotolistview.h"
#include "stthumbnailview.h" 
#include "siconfactory.h" 
#include "stphotobookcheckedproxymodel.h" 
#include "checkitemdelegate.h" 

void PBImageSelectWidget::updateActionState()
{
	PreviousPageAct->setEnabled(LVImages->verticalScrollBar()->value() > LVImages->verticalScrollBar()->minimum()); 
	NextPageAct->setEnabled(LVImages->verticalScrollBar()->value() < LVImages->verticalScrollBar()->maximum()); 
}

void PBImageSelectWidget::setupActions()
{
	PreviousPageAct = new QAction(QIcon(":/phototemplates/go-up.png"), tr("Previous page"), this);
	PreviousPageAct->setStatusTip(tr("Goes to the previous page of current photobook."));
	connect(PreviousPageAct, SIGNAL(triggered()), this, SLOT(previousPage()));

	NextPageAct = new QAction(QIcon(":/phototemplates/go-down.png"), tr("Next page"), this);
	NextPageAct->setStatusTip(tr("Goes to the next page of current photobook."));
	connect(NextPageAct, SIGNAL(triggered()), this, SLOT(nextPage()));

	AdvancedAct = new QAction(QIcon(":/open_moreimages.png"), tr("Advanced"), this);
	AdvancedAct->setStatusTip(tr("Opens advanced actions and shows more images."));
	AdvancedAct->setVisible(false); 
	connect(AdvancedAct, SIGNAL(triggered()), this, SLOT(previousPage()));
}

void PBImageSelectWidget::setupToolBars()
{
	MToolBar->addAction(PreviousPageAct); 
	MToolBar->addAction(NextPageAct); 
	MToolBar->addAction(AdvancedAct); 
}

PBImageSelectWidget::PBImageSelectWidget(QWidget *parent)
 : QWidget(parent), SourceModel(0)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	//LVImages = new TPPhotoListView(this);
	LVImages = new STThumbnailView(this);  
	LVImages->setWrapping(false);

	CheckItemDelegate* PDelegate = new CheckItemDelegate(LVImages); 
	LVImages->setItemDelegate(PDelegate); 
	
	connect(LVImages, SIGNAL(doubleClicked( const QModelIndex& )), this, SLOT(imageDoubleClicked(const QModelIndex&)));

	MLayout->addWidget(LVImages); 

	MToolBar = new QToolBar(this); 
	//MToolBar->setObjectName("PBEditorToolBar"); 
	MToolBar->setIconSize(QSize(64,64)); 
	MLayout->addWidget(MToolBar); 

	setupActions();
	setupToolBars();

	MProxyModel = new STPhotoBookCheckedProxyModel(this, QImage(":/checked.png")); 
	MProxyModel->setThumbnailSize(QSize(128, 128)); 
	LVImages->setModel(MProxyModel); 
}


PBImageSelectWidget::~PBImageSelectWidget()
{
}

void PBImageSelectWidget::setModel(STDom::DDocModel* _Model)
{
	//LVImages->setModel(_Model); 
	SourceModel = _Model;
	MProxyModel->setSourceModel(_Model);
}

void PBImageSelectWidget::setPhotoBook(STPhotoBook* _PhotoBook)
{
	MProxyModel->setPhotoBook(_PhotoBook); 
}

QModelIndex PBImageSelectWidget::currentIndex() const
{
	return MProxyModel->mapToSource(LVImages->currentIndex());
}


void PBImageSelectWidget::nextPage()
{
	qApp->notify(LVImages, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::ShiftModifier));
	updateActionState(); 
}

void PBImageSelectWidget::previousPage()
{
	qApp->notify(LVImages, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::ShiftModifier));
	updateActionState(); 
}

void PBImageSelectWidget::updateCheckedFiles()
{
	MProxyModel->updateCheckedFiles(); 
}

void PBImageSelectWidget::imageDoubleClicked(const QModelIndex& _Index)
{
	if (MProxyModel && SourceModel && _Index.isValid())
	{
		emit applyImage(SourceModel->doc(MProxyModel->mapToSource(_Index)));
	}
}
