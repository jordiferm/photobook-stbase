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

#include "sporomeimageslistview.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include "stphotobookcheckedproxymodel.h"

SPOromeImagesListView::SPOromeImagesListView(QWidget* _Parent ) : SPImagesListViewBase(_Parent, false)
{
	QAction* ClearAction = new QAction(QIcon(":/st/oromel/edit_clear_list.png"), tr("Clear"), this);
	connect(ClearAction, SIGNAL(triggered()), this, SLOT(clearImages()));
	toolBar()->addAction(ClearAction);

	QAction* AutoFillAction = new QAction(QIcon(":/st/oromel/autofill.png"), tr("AutoFill"), this);
	connect(AutoFillAction, SIGNAL(triggered()), this, SIGNAL(autoFillImages()));
	toolBar()->addAction(AutoFillAction);


	QAction* ExportAction = new QAction(QIcon(":/st/oromel/export.png"), tr("Export"), this);
	connect(ExportAction, SIGNAL(triggered()), this, SIGNAL(exportImages()));
	toolBar()->addAction(ExportAction);

	QAction* IndexAction = new QAction(QIcon(":/st/oromel/photoindex.png"), tr("Create Index"), this);
	connect(IndexAction, SIGNAL(triggered()), this, SIGNAL(createIndexImages()));
	toolBar()->addAction(IndexAction);
	setThumbnailSize(2);
	IndexAction->setVisible(false);
}
