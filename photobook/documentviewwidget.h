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

#ifndef STDocumentViewWidget_H
#define STDocumentViewWidget_H

#include <QWidget>
#include "stphotobookexport.h"

/**
PhotoBook Multiple Page Viewer.

@author Shadow
*/
class QStackedLayout;
class QToolButton;
class STClickableLabel;
class QGraphicsItem;
class QGraphicsScene;

namespace SPhotoBook
{
class Document;
class TemplateScene;
class DocumentPageView;
class GraphicsPhotoItem;
class GraphicsPageItem;
class ST_PHOTOBOOK_EXPORT DocumentViewWidget : public QWidget
{

Q_OBJECT
	QStackedLayout* StkLayout;
	const Document* PBDocument;
	QToolButton* createActionButton(QWidget* _Parent, const QString& _IconFile, const QString& _Text = "");
	void adjustViewToScene(DocumentPageView* _View, QGraphicsScene* _Scene);

public:

	DocumentViewWidget(QWidget* _Parent = 0);
	void clear();
	void reloadPages();
	void setPBDocument(const SPhotoBook::Document* _PBDocument);

	int sceneIndex(QGraphicsScene* _Scene);
	void setCurrentPage(int _PageIndex);
	//! Returns true if current page is the cover.
	bool showingCover() const;
	//! \return Number of model pages
	int numPages() const;
	int currentPageIndex();
	DocumentPageView* currentPage() const;
	TemplateScene* currentScene() const;
	//! Returns focus item of current page or first selected item if there is no focus item.
	QGraphicsItem* currentItem() const;
	GraphicsPhotoItem* currentPhotoItem() const;
	GraphicsPageItem* currentPageItem() const;
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
}
#endif
