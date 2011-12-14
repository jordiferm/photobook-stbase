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
#include "qtscrollwheel.h"
#ifdef ST_TTPOPSL_KINETICSCROLL
#include "wheelwidget.h"
#endif

int TPProductListView::scrollToIndexValue(int _Value)
{
	int Res = _Value;
	if (model()) //Defensive
		Res = (model()->rowCount() - _Value) -1;
	return Res;
}

TPProductListView::TPProductListView(QWidget *parent, bool _EmbeddedWidgets)
	: QWidget(parent), ScrWheel(0), Model(0)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	MLayout->setMargin(1); 
	MLayout->setSpacing(1);

#ifdef ST_TTPOPSL_KINETICSCROLL
	MWheelWidget = new StringWheelWidget(false, this);
	MLayout->addWidget(MWheelWidget);
#else
	MListView = new QListView(this);
	MListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(MListView, SIGNAL(clicked( const QModelIndex& )), this, SIGNAL(clicked(const QModelIndex&))); 
	connect(MListView, SIGNAL(clicked( const QModelIndex& )), this, SLOT(slotListClicked(const QModelIndex&)));
	MListView->setObjectName("ProductsListView");

	MLayout->addWidget(MListView);

	if (_EmbeddedWidgets)
	{
		ScrWheel = new QtScrollWheel(this);
		MLayout->addWidget(ScrWheel);
		connect(ScrWheel, SIGNAL(valueChanged(int)), this, SLOT(slotScrollValueChanged(int)));
	}
	else
	{
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
#endif
}

TPProductListView::~TPProductListView()
{
}

QSize TPProductListView::sizeHint() const
{
	return QSize(300, 80);
}

void TPProductListView::setModel(QAbstractItemModel* _Model)
{
	Model = _Model;
#ifdef ST_TTPOPSL_KINETICSCROLL
	QStringList ModelDataStringList;
	for (int Vfor = 0; Vfor < _Model->rowCount(); Vfor++)
	{
		ModelDataStringList << _Model->data(_Model->index(Vfor, 0), Qt::DisplayRole).toString();
	}
	MWheelWidget->setItems(ModelDataStringList);
#else
	MListView->setModel(_Model);
	if (ScrWheel)
	{
		ScrWheel->setRange(0, _Model->rowCount() -1);
		ScrWheel->setValue(scrollToIndexValue(0));
	}
#endif
	setCurrentIndex(_Model->index(0,0));
}

QModelIndex TPProductListView::currentIndex() const
{
#ifdef ST_TTPOPSL_KINETICSCROLL
	return model()->index(MWheelWidget->currentIndex(), 0);
#else
	return MListView->currentIndex();
#endif
}

void TPProductListView::setCurrentIndex(const QModelIndex& _Index)
{
#ifdef ST_TTPOPSL_KINETICSCROLL
	MWheelWidget->setCurrentIndex(_Index.row());
#else
	MListView->setCurrentIndex(_Index);
#endif
	slotListClicked(_Index);
}

STDom::DDocProduct TPProductListView::currentProduct() const
{
	STDom::DDocProduct Res;
	STDom::PublisherDatabase PublDB;
	if (model())
	{
		if (currentIndex().row() < model()->rowCount())
			Res = PublDB.getProduct(model(), currentIndex().row());
	}
	return Res;
}

STDom::DDocProduct TPProductListView::product(const QModelIndex& _Index) const
{
	STDom::PublisherDatabase PublDB;
	return PublDB.getProduct(model(), _Index.row());
}

void TPProductListView::scrollUpClicked()
{
	if (model()) //Defensive
	{
		QModelIndex CurrentIndex = currentIndex();
		if (CurrentIndex.row() > 0) 
		{
			QModelIndex NIndex = model()->index(CurrentIndex.row() -1, CurrentIndex.column());
			setCurrentIndex(NIndex);
			clicked(NIndex); 
		}
	}
}

void TPProductListView::scrollDownClicked()
{
	if (model()) //Defensive
	{
		QModelIndex CurrentIndex = currentIndex();
		if (CurrentIndex.row() < model()->rowCount() -1)
		{
			QModelIndex NIndex = model()->index(CurrentIndex.row() +1, CurrentIndex.column());
			setCurrentIndex(NIndex);
			clicked(NIndex); 
		}
	}
}

void TPProductListView::slotScrollValueChanged(int _Value)
{
	if (model()) //Defensive
	{
		QModelIndex NewIndex = model()->index(scrollToIndexValue(_Value), currentIndex().column());
		if (NewIndex.row() < model()->rowCount() && NewIndex.row() >= 0)
		{
			setCurrentIndex(NewIndex);
			clicked(NewIndex);
		}
	}
}

void TPProductListView::slotListClicked(const QModelIndex& _Index)
{
	if (_Index.isValid() && ScrWheel)
		ScrWheel->setValue(scrollToIndexValue(_Index.row()));
}
