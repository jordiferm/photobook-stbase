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
#ifndef STTHUMBNAILVIEW_H
#define STTHUMBNAILVIEW_H

#include <QListView>
#include "stwidgetsexport.h"

/**
Listview of image thumbnails.

	@author 
*/
class ST_WIDGETS_EXPORT STThumbnailView : public QListView
{
	Q_OBJECT

	int ThumbnailSize;

public:
    STThumbnailView(QWidget* parent);
    ~STThumbnailView();
	/**
	 * Returns the thumbnail size.
	 */
	void setThumbnailSize(int _Value);
	int thumbnailSize() const { return ThumbnailSize; }
	QPixmap thumbnailForIndex(const QModelIndex& index);
	void scheduleDelayedItemsLayout();

	/**
	 * Returns true if the document pointed by the index has been modified
	 * inside Gwenview.
	 */
	bool isModified(const QModelIndex&) const { return false; }

signals:
	void thumbnailSizeChanged(int);

};

#endif
