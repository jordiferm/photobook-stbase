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

#ifndef STSTAlbumWidget_H
#define STSTAlbumWidget_H

#include <QWidget>
#include "stphototemplatesexport.h"

/**
PhotoBook Multiple Page Viewer.

@author Shadow
*/
class QStackedLayout;
class QToolButton;
class STAlbumPageGraphicsView;
class STPhotoBookTemplate;
class STClickableLabel;
class QGraphicsItem;
class STGraphicsSimpleTextItem;
class STGraphicsPhotoItem;
class QGraphicsScene;
class STTemplateScene;
class STGraphicsPageItem; 
class STPhotoBook;
class ST_PHOTOTEMPLATES_EXPORT STAlbumWidget : public QWidget
{

Q_OBJECT
	QStackedLayout* StkLayout;
	const STPhotoBook* PhotoBook;
	QToolButton* createActionButton(QWidget* _Parent, const QString& _IconFile, const QString& _Text = "");
	void adjustViewToScene(STAlbumPageGraphicsView* _View, QGraphicsScene* _Scene);

public:

	STAlbumWidget(QWidget* _Parent = 0);
	void clear();
	void setPhotoBook(const STPhotoBook* _PhotoBook);
	int sceneIndex(QGraphicsScene* _Scene);
	void setCurrentPage(int _PageIndex);
	//! Returns true if current page is the cover.
	bool showingCover() const;
	//! \return Number of model pages
	int numPages() const;
	int currentPageIndex();
	STAlbumPageGraphicsView* currentPage() const;
	STTemplateScene* currentScene() const; 
	//! Returns focus item of current page or first selected item if there is no focus item.
	QGraphicsItem* currentItem() const;
	STGraphicsPhotoItem* currentPhotoItem() const;
	STGraphicsSimpleTextItem* currentTextItem() const;
	STGraphicsPageItem* currentPageItem() const;
	QString currentPageInfo();
	QString pageBoundariesInfo();
	QString templateInfo();
	
public slots:
	void firstPage();
	void nextPage();
	void previousPage();
	void lastPage();
	void showPage(int _Page); 
	void selectCurrentPageBackground();

signals:
	void pageChanged(int _FromPage, int _ToPage);
	void photoBookInfoChanged();
};

#endif
