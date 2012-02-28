/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SPOROMEIMAGESLISTVIEW_H
#define SPOROMEIMAGESLISTVIEW_H
#include "imageslistviewbase.h"
#include "storomelexport.h"

class ST_OROMEL_EXPORT SPOromeImagesListView : public SPhotoBook::ImagesListViewBase
{
	Q_OBJECT

public:
	SPOromeImagesListView(QWidget* _Parent = 0);
signals:
	void exportImages();
	void autoFillImages();
	void createIndexImages();
};

#endif // SPOROMEIMAGESLISTVIEW_H
