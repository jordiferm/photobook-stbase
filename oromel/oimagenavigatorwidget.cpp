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
#include "oimagenavigatorwidget.h"
#include <QLayout> 
#include <QToolButton> 
#include <QListView> 

#include "siconfactory.h"

//_____________________________________________________________________________
//
// Class OSelectionFilterProxyModel
//_____________________________________________________________________________

QVariant OImageNavigatorProxyModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	
	if (_Role == Qt::DecorationRole)
	{
		QSize ThumbnailSize(48, 48); 
		if (_Index.row() ==  CurrentItemIndex.row())
			ThumbnailSize = QSize(65, 65);
		
		QPixmap RPixmap = QSortFilterProxyModel::data(_Index, _Role).value<QPixmap>();
		Res = RPixmap.scaled(ThumbnailSize, Qt::KeepAspectRatio);
	}
	return Res;

}

bool OImageNavigatorProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	return true; 
}


OImageNavigatorWidget::OImageNavigatorWidget(QWidget *parent)
 : QWidget(parent)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	
	MLayout->setMargin(0);
			
	QToolButton* ButBack = new QToolButton(this);
	ButBack->setIcon(stIcon(SIconFactory::Rew));
	MLayout->addWidget(ButBack);
	ButBack->setIconSize(QSize(32, 32));
	connect(ButBack, SIGNAL(clicked( bool )), this, SLOT(setPreviousImage()));
	
	MImageModel = new OImageNavigatorProxyModel(this);
	MListView = new QListView(this); 
	MListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MListView->setViewMode(QListView::IconMode);	
	MListView->setModel(MImageModel);
	MListView->setMaximumHeight(60);
	connect(MListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotSetCurrentItem(const QModelIndex&)));
	MLayout->addWidget(MListView); 
	
	QToolButton* ButNext = new QToolButton(this);
	ButNext->setIcon(stIcon(SIconFactory::Fwd));
	MLayout->addWidget(ButNext);
	ButNext->setIconSize(QSize(32, 32));
	connect(ButNext, SIGNAL(clicked( bool )), this, SLOT(setNextImage()));
}


OImageNavigatorWidget::~OImageNavigatorWidget()
{
}


QSize OImageNavigatorWidget::sizeHint() const
{
	return QSize(400, 60);
}

void OImageNavigatorWidget::setCurrentItem(const QModelIndex& _Index)
{
	QModelIndex CurrIndex = MListView->model()->index(_Index.row(), _Index.column());
	MImageModel->setCurrentItemIndex(CurrIndex);
	MListView->setWrapping(false);
	MListView->setCurrentIndex(CurrIndex);
	MListView->scrollTo(CurrIndex, QAbstractItemView::PositionAtCenter);	
}

void OImageNavigatorWidget::setSourceModel(STDom::DDocModel* _Model)
{
	MImageModel->setSourceModel(_Model);

}

void OImageNavigatorWidget::slotSetCurrentItem(const QModelIndex& _Index)
{
	setCurrentItem(_Index);
	emit currentItemChanged(_Index);
}

void OImageNavigatorWidget::setPreviousImage()
{
	QModelIndex CurrIndex = MListView->currentIndex(); 
	QAbstractItemModel* Model = MImageModel->sourceModel();
	if (CurrIndex.row() > 0)
		slotSetCurrentItem(Model->index(CurrIndex.row() - 1, CurrIndex.column())); 
}

void OImageNavigatorWidget::setNextImage()
{
	QModelIndex CurrIndex = MListView->currentIndex(); 
	QAbstractItemModel* Model = MImageModel->sourceModel();
	if (CurrIndex.row() < Model->rowCount() - 1)
		slotSetCurrentItem(Model->index(CurrIndex.row() + 1, CurrIndex.column())); 
}
