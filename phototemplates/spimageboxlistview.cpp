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

#include "spimageboxlistview.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>

#include "ddocmodel.h" //ImageModel
#include "stphotobookcheckedproxymodel.h"

#include "checkitemdelegate.h"
#include "stthumbnailview.h"


SPImageBoxListView::SPImageBoxListView(QWidget* _Parent) : SPToolbarListView(Qt::Horizontal, _Parent)
{

	QAction* RemoveImagesAction = new QAction(QIcon(":/phototemplates/list-remove.png"), tr("Remove"), this);
	connect(RemoveImagesAction, SIGNAL(triggered()), this, SLOT(slotRemoveSelectedImages()));
	toolBar()->addAction(RemoveImagesAction);

	QAction* SelectAllAction = new QAction(QIcon(":/phototemplates/rating.png"), tr("Select All (Ctrl+A)"), this);
	connect(SelectAllAction , SIGNAL(triggered()), listView(), SLOT(selectAll()));
	toolBar()->addAction(SelectAllAction);

	activateItemCounter(tr("Image(s)"));

	listView()->setAcceptDrops(true);
	listView()->setDropIndicatorShown(true);
	listView()->setThumbnailSize(100);
	listView()->setSpacing(0);

	CheckItemDelegate* PDelegate = new CheckItemDelegate(listView());
	//PDelegate->setShowToolTips(true);
	listView()->setItemDelegate(PDelegate);


	//OPhotoCollectionImageModel* ImageModel = new OPhotoCollectionImageModel(this);
	STDom::DDocModel* ImageModel = new STDom::DDocModel(this);
	ImageModel->setNoImagePixmap(QPixmap(":/phototemplates/hourglass.png"));
	ImageProxyModel = new STPhotoBookCheckedProxyModel(this, QImage(":/phototemplates/checked.png"));
	ImageProxyModel->setSourceModel(ImageModel);
	ImageProxyModel->setThumbnailSize(QSize(100, 100));
	setModel(ImageProxyModel);
}


void SPImageBoxListView::slotRemoveSelectedImages()
{
	QModelIndexList SelIndexes = listView()->selectionModel()->selectedRows(0);
	qSort(SelIndexes.begin(), SelIndexes.end());
	QModelIndexList::const_iterator it;
	int Cnt = 0;
	for (it = SelIndexes.begin(); it != SelIndexes.end(); ++it)
	{
		ImageProxyModel->removeRow(it->row() - Cnt);
		Cnt++;
	}
}

