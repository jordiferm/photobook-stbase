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

#include "sptoolbarlistview.h"
#include <QToolBar>
#include <QListView>
#include <QLayout>
#include <QLabel>
#include <QAbstractItemModel>

#include "stthumbnailview.h"


SPToolbarListView::SPToolbarListView(Qt::Orientation _Orientation,  QWidget* _Parent, bool _ToolBarOnTop ) : QWidget(_Parent), ItemCounterLabel(0)
{
	QLayout* MLayout;
	if (_ToolBarOnTop)
		MLayout = new QVBoxLayout(this);
	else
		MLayout = new QHBoxLayout(this);

	ToolBar = new QToolBar(this);
	if (!_ToolBarOnTop)
	{
		ToolBar->setOrientation(Qt::Vertical);
		ToolBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	}
	MLayout->addWidget(ToolBar);
	MLayout->setMargin(2);
	MLayout->setSpacing(1);

	ListView = new STThumbnailView(this);
//	ListView->setViewMode(QListView::IconMode);
	ListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ListView->setWordWrap(true);
	//ListView->setWrapping(false);
	ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
//	ListView->setResizeMode(QListView::Adjust);
//	ListView->setMinimumHeight(100);

	if (_Orientation == Qt::Horizontal)
	{
		ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		ListView->setFlow(QListView::LeftToRight);
	}
	else
	{
		ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		ListView->setFlow(QListView::TopToBottom);
		ListView->setWrapping(false);
	}

	MLayout->addWidget(ListView);
}

void SPToolbarListView::setModel(QAbstractItemModel* _Model)
{
	ListView->setModel(_Model);
	connect(_Model, SIGNAL( rowsInserted(const QModelIndex&, int , int )), this, SLOT(updateItemCounter()));
	connect(_Model, SIGNAL(rowsRemoved(const QModelIndex&, int , int )), this, SLOT(updateItemCounter()));
	connect(_Model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateItemCounter()));
	connect(_Model, SIGNAL(modelReset()), this, SLOT(updateItemCounter()));
	updateItemCounter();
}

STThumbnailView* SPToolbarListView::listView()
{
	return ListView;
}

void SPToolbarListView::activateItemCounter(const QString& _ItemName)
{
	ItemCounterName = _ItemName;
	if (!ItemCounterLabel)
	{
		ItemCounterLabel = new QLabel(this);
		ToolBar->addWidget(ItemCounterLabel);
		updateItemCounter();
	}
}

void SPToolbarListView::updateItemCounter()
{
	if (ItemCounterLabel)
	{
		int NItems = 0;
		if (ListView->model())
			 NItems = ListView->model()->rowCount();
		if (NItems)
			ItemCounterLabel->setText(tr("Number of %1 : %2").arg(ItemCounterName).arg(NItems));
		else
			ItemCounterLabel->setText(tr("There are no %1").arg(ItemCounterName));
	}
}
