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
#include "stthumbnailview.h"

//Drag and Drop
#include <QDropEvent>
#include <QUrl>

/** How many pixels between items */
const int SPACING = 4;

STThumbnailView::STThumbnailView(QWidget* parent): QListView(parent)
{
	setFrameShape(QFrame::NoFrame);
	//setViewMode(QListView::IconMode); //In iconmode DropIndicator isn't an image.
	setResizeMode(QListView::Adjust);
	setSpacing(SPACING);
	setWrapping(true);
	//setMovement(QListView::Static);
	setMovement(QListView::Free);
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	viewport()->setMouseTracking(true);
	// Set this attribute, otherwise the item delegate won't get the
	// State_MouseOver state
	viewport()->setAttribute(Qt::WA_Hover);

	setVerticalScrollMode(ScrollPerPixel);
	setHorizontalScrollMode(ScrollPerPixel);
	ThumbnailSize = 128;
}


STThumbnailView::~STThumbnailView()
{
}

void STThumbnailView::setThumbnailSize(int _Value)
{
	ThumbnailSize = _Value;
	emit thumbnailSizeChanged(_Value);
}

QPixmap STThumbnailView::thumbnailForIndex(const QModelIndex& index)
{
	return index.data(Qt::DecorationRole).value<QPixmap>(); 
}

void STThumbnailView::scheduleDelayedItemsLayout()
{
	QListView::scheduleDelayedItemsLayout();
}

