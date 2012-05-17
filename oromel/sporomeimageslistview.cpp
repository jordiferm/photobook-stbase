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

#include "sporomeimageslistview.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include "doccheckedproxymodel.h"

SPOromeImagesListView::SPOromeImagesListView(QWidget* _Parent ) : SPhotoBook::ImagesListViewBase(_Parent, false)
{
    QAction* AutoFillAction = new QAction(QIcon(":/st/oromel/autofill.png"), tr("AutoFill"), this);
    connect(AutoFillAction, SIGNAL(triggered()), this, SIGNAL(autoFillImages()));
    toolBar()->addAction(AutoFillAction);

    QAction* DeleteSelectedAction = new QAction(QIcon(":/st/oromel/viewmag-.png"), tr("Clear Selected"), this);
    connect(DeleteSelectedAction, SIGNAL(triggered()), this, SLOT(clearSelectedImages()));
    toolBar()->addAction(DeleteSelectedAction);

    QAction* ClearAction = new QAction(QIcon(":/st/oromel/edit_clear_list.png"), tr("Clear All"), this);
	connect(ClearAction, SIGNAL(triggered()), this, SLOT(clearImages()));
	toolBar()->addAction(ClearAction);

	QAction* ExportAction = new QAction(QIcon(":/st/oromel/export.png"), tr("Export"), this);
	connect(ExportAction, SIGNAL(triggered()), this, SIGNAL(exportImages()));
	toolBar()->addAction(ExportAction);

	QAction* IndexAction = new QAction(QIcon(":/st/oromel/photoindex.png"), tr("Create Index"), this);
	connect(IndexAction, SIGNAL(triggered()), this, SIGNAL(createIndexImages()));
	toolBar()->addAction(IndexAction);
	setThumbnailSize(2);
	IndexAction->setVisible(false);
}
