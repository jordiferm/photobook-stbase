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
#include "tpphotolistview.h"
#include "printjobmodel.h"

/** How many pixels between items */
const int SPACING = 11;
int TPPhotoListView::DefaultThumbnailSize = 128; 

TPPhotoListView::TPPhotoListView(QWidget* parent): QListView(parent), ThumbnailSize(DefaultThumbnailSize)
{
	
	setFrameShape(QFrame::NoFrame);
	setViewMode(QListView::IconMode);
	setResizeMode(QListView::Adjust);
	setSpacing(SPACING);

	setMovement(QListView::Static); 
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);

	viewport()->setMouseTracking(true);
	// Set this attribute, otherwise the item delegate won't get the
	// State_MouseOver state
	viewport()->setAttribute(Qt::WA_Hover);

	setVerticalScrollMode(ScrollPerPixel);
	setHorizontalScrollMode(ScrollPerPixel);
	setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
}

TPPhotoListView::~TPPhotoListView()
{
}

int TPPhotoListView::thumbnailSize() const 
{ 
	return ThumbnailSize; 
}

QPixmap TPPhotoListView::thumbnailForIndex(const QModelIndex& index) 
{
	return index.data(Qt::DecorationRole).value<QPixmap>(); 
}

bool TPPhotoListView::hasPhotos(const QModelIndex& _Index) const
{
	bool Res = false;
	STDom::PrintJobModel* CModel = qobject_cast<STDom::PrintJobModel*>(model()) ;
	if (CModel)
		Res = CModel->hasCopies(_Index.row());
	return Res;
}

