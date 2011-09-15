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

#ifndef IMAGESLISTVIEW_H
#define IMAGESLISTVIEW_H

#include "imageslistviewbase.h"
#include "stphotobookexport.h"

class OPhotoCollectionImageModel;
class QAction;
class STPhotoBookCheckedProxyModel;
class STPhotoBook;
class QActionGroup;

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT ImagesListView : public ImagesListViewBase
{
Q_OBJECT

	QAction* DiskAction;
	QAction* CollectionAction;
	QAction* DesktopAction;
	QActionGroup* ImageSourceActions;

public:
	ImagesListView(QWidget* _Parent = 0);
	void setDiskPath(const QString& _CurrentPath);
	//void setCollectionFolderKey(quint64 _Key);
	QAction* addImageSourceAction(const QIcon& _Icon, const QString& _Text);

public slots:
	void setDesktopPath();
	void selectFolder();
	void selectDiskFolder();
	//void selectCollectionFolder();

};
}
#endif // ImagesListView_H
