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
#ifndef TPPHOTOLISTVIEW_H
#define TPPHOTOLISTVIEW_H

#include <QListView>
#include "sttpopslexport.h"


/**
TouchScreen ListView.

	@author
*/
class ST_TPOPSL_EXPORT TPPhotoListView : public QListView
{
	Q_OBJECT
	static int DefaultThumbnailSize; 
	int ThumbnailSize; 

public:
	TPPhotoListView(QWidget* parent);
	~TPPhotoListView();
	 //! Returns the thumbnail size.
	void setThumbnailSize(int _Size) { ThumbnailSize = _Size; }
	int thumbnailSize() const;
	QPixmap thumbnailForIndex(const QModelIndex& index);
	bool isModified(const QModelIndex&) const { return false; }
	bool hasPhotos(const QModelIndex&) const; 
};

#endif
