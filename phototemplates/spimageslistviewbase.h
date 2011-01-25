/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SPIMAGESLISTVIEWBASE_H
#define SPIMAGESLISTVIEWBASE_H
#include <QFileInfoList>

#include "sptoolbarlistview.h"
#include "stphototemplatesexport.h"
#include "ddocmodel.h"

class OPhotoCollectionImageModel;
class QAction;
class STPhotoBookCheckedProxyModel;
class STPhotoBook;
class SPThumbnailSizeWidget;
class ST_PHOTOTEMPLATES_EXPORT SPImagesListViewBase : public SPToolbarListView
{
Q_OBJECT

	QAction* SelectAllAction;

protected:
	STDom::DDocModel* ImageModel;
	STPhotoBookCheckedProxyModel* ImageProxyModel;
	QString CurrentPath;
	SPThumbnailSizeWidget* ThumbnailSizeWidget;

public:
	SPImagesListViewBase(QWidget* _Parent = 0, bool _ToolBarOnTop = true);
	void setPath(const QString& _Path);
	QString currentPath() const { return CurrentPath; }
	void setImages(const QFileInfoList& _Images);
	void setPhotoBook(const STPhotoBook* _PhotoBook); //To check files.
	void showSelectAllAction(bool _Show);
	void selectImage(const QString& _ImageFileName);
	void selectImageMD5(const QString& _Md5Sum);
	STDom::DDocModel* imageModel() const { return ImageModel; }
	STPhotoBookCheckedProxyModel* model() const { return ImageProxyModel; }
	void setThumbnailSize(int _Value);
	QFileInfoList selectedImages();

public slots:
	void updateCheckedFiles();
	void slotSelectAll();
	void slotThumbnailSizeChanged(int _Value);
	void clearImages();

};



#endif // SPIMAGESLISTVIEWBASE_H
