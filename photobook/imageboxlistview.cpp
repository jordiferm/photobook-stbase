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

#include "imageboxlistview.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>

#include "ddocmodel.h" //ImageModel
#include "doccheckedproxymodel.h"

#include "checkitemdelegate.h"
#include "stthumbnailview.h"

using namespace SPhotoBook;

ImageBoxListView::ImageBoxListView(QWidget* _Parent) : SPToolbarListView(Qt::Horizontal, _Parent)
{

	QAction* RemoveImagesAction = new QAction(QIcon(":/photobook/list-remove.png"), tr("Remove"), this);
	connect(RemoveImagesAction, SIGNAL(triggered()), this, SLOT(slotRemoveSelectedImages()));
	toolBar()->addAction(RemoveImagesAction);

	QAction* SelectAllAction = new QAction(QIcon(":/photobook/rating.png"), tr("Select All (Ctrl+A)"), this);
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
	ImageModel->setNoImagePixmap(QPixmap(":/photobook/hourglass.png"));
	ImageProxyModel = new DocCheckedProxyModel(this, QImage(":/photobook/checked.png"));
	ImageProxyModel->setSourceModel(ImageModel);
	ImageProxyModel->setThumbnailSize(QSize(100, 100));
	setModel(ImageProxyModel);
}


void ImageBoxListView::slotRemoveSelectedImages()
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

