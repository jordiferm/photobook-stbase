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
#include "tpproductlistview.h"
#include <QLayout> 
#include <QListView> 
#include <QToolButton> 

#include "publisherdatabase.h"

TPProductListView::TPProductListView(QWidget *parent)
 : QWidget(parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	MLayout->setMargin(1); 
	MLayout->setSpacing(1);
	MListView = new QListView(this);
	connect(MListView, SIGNAL(clicked( const QModelIndex& )), this, SIGNAL(clicked(const QModelIndex&))); 
	MListView->setObjectName("ProductsListView"); 

	MLayout->addWidget(MListView); 
	QVBoxLayout* TBLayout = new QVBoxLayout; 
	TBLayout->setMargin(1); 
	TBLayout->setSpacing(5); 
	MLayout->addLayout(TBLayout);

	QToolButton* ButScrollUp = new QToolButton(this); 
	ButScrollUp->setIcon(QIcon(":/st/tpopsl/1uparrow.png"));
	ButScrollUp->setIconSize(QSize(32,32)); 
	connect(ButScrollUp, SIGNAL(clicked( bool )), this, SLOT(scrollUpClicked())); 
	TBLayout->addWidget(ButScrollUp); 
	

	TBLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding)); 
	QToolButton* ButScrollDown = new QToolButton(this);
	ButScrollDown->setIcon(QIcon(":/st/tpopsl/1downarrow.png"));
	ButScrollDown->setIconSize(QSize(32,32)); 
	connect(ButScrollDown, SIGNAL(clicked( bool )), this, SLOT(scrollDownClicked())); 
	TBLayout->addWidget(ButScrollDown); 
	
}


TPProductListView::~TPProductListView()
{
}

STDom::DDocProduct TPProductListView::currentProduct() const
{
	STDom::DDocProduct Res;
	STDom::PublisherDatabase PublDB;
	if (MListView->model())
	{
		if (MListView->currentIndex().row() < MListView->model()->rowCount())
			Res = PublDB.getProduct(MListView->model(), MListView->currentIndex().row());
	}
	return Res;
}

STDom::DDocProduct TPProductListView::product(const QModelIndex& _Index) const
{
	STDom::PublisherDatabase PublDB;
	return PublDB.getProduct(MListView->model(), _Index.row());	
}

void TPProductListView::scrollUpClicked()
{
	if (MListView->model()) //Defensive 
	{
		QModelIndex CurrentIndex = MListView->currentIndex(); 
		if (CurrentIndex.row() > 0) 
		{
			QModelIndex NIndex = MListView->model()->index(CurrentIndex.row() -1, CurrentIndex.column()); 
			MListView->setCurrentIndex(NIndex); 
			clicked(NIndex); 
		}
	}
}

void TPProductListView::scrollDownClicked()
{
	if (MListView->model()) //Defensive 
	{
		QModelIndex CurrentIndex = MListView->currentIndex(); 
		if (CurrentIndex.row() < MListView->model()->rowCount() -1) 
		{
			QModelIndex NIndex = MListView->model()->index(CurrentIndex.row() +1, CurrentIndex.column()); 
			MListView->setCurrentIndex(NIndex); 
			clicked(NIndex); 
		}
	}
}

