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
#ifndef PBIMAGESELECTWIDGET_H
#define PBIMAGESELECTWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include "stphototemplatesexport.h"
#include "ddocmodel.h"

/**
Widget to select images for photobook.

	@author 
*/
class QToolBar; 
class QAction; 
class STThumbnailView; 
class OPhotoCollectionImageModel;
class STPhotoBookCheckedProxyModel;
class STPhotoBook; 
class OPhotoCollectionImage;
class ST_PHOTOTEMPLATES_EXPORT PBImageSelectWidget : public QWidget
{
Q_OBJECT


	QAction* PreviousPageAct;
	QAction* AdvancedAct; 
	QAction* NextPageAct; 

	QToolBar* MToolBar;
	STThumbnailView* LVImages;
	STPhotoBookCheckedProxyModel* MProxyModel; 
	STDom::DDocModel* SourceModel;

	void updateActionState();
	void setupActions();
	void setupToolBars();

public:
	PBImageSelectWidget(QWidget *parent = 0);
	~PBImageSelectWidget();
	void setModel(STDom::DDocModel* _Model);
	void setPhotoBook(STPhotoBook* _PhotoBook);
	QModelIndex currentIndex() const; 
	void updateCheckedFiles(); 

public slots: 
	void imageDoubleClicked(const QModelIndex& _Index);
	void nextPage();
	void previousPage();

signals:
	void applyImage(const STDom::DDoc* _Image);
};

#endif
