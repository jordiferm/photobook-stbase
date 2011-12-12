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

#include "stfolderimagesingleview.h"
#include <QLayout>
#include <QToolBar>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QScrollBar>
#include <QApplication>

#include "stthumbnailview.h"
#include "previewitemdelegate.h"
#include "stimagetools.h"
#include "thumbnailsizewidget.h"
#include "dimagedoc.h"
#include "smessagebox.h"

void STFolderImageSingleView::setupToolBarActions()
{
	OpenFolderAction = new QAction(QIcon(":/st/fileopen.png"), tr("Folder"), this);
	MToolBar->addAction(OpenFolderAction);
	connect(OpenFolderAction, SIGNAL(triggered()), this, SLOT(openFolderAction()));
}

void STFolderImageSingleView::setupImageActions()
{
	QAction* SelectAllAction = new QAction(QIcon(":/photobook/rating.png"), tr("Select All (Ctrl+A)"), this);
	ImageActionsToolBar->addAction(SelectAllAction);
	connect(SelectAllAction, SIGNAL(triggered()), LView, SLOT(selectAll()));

	ThumbnailSizeWidget* TSWidget = new ThumbnailSizeWidget(this);
	connect(TSWidget, SIGNAL(valueChanged(int )), this, SLOT(slotThumbnailSizeChanged(int )));
	ImageActionsToolBar->addWidget(TSWidget);
	TSWidget->setValue(3);
	slotThumbnailSizeChanged(3);

	QAction* BigViewAction = new QAction(QIcon(":/photobook/preview.png"), tr("Big view"), this);
	ImageActionsToolBar->addAction(BigViewAction);
	connect(BigViewAction, SIGNAL(triggered()), this, SLOT(slotBigPreviewFolder()));
}

void STFolderImageSingleView::setEmptyMode(bool _Value)
{
	LView->setVisible(!_Value);
	ImageActionsToolBar->setVisible(!_Value);
}

void STFolderImageSingleView::setLabel(const QString& _Label)
{
	if (HeaderMode)
		HeaderLabel->setText("<h1>" + _Label + "</h1>");
	else
		OpenFolderAction->setText(_Label);
}

void STFolderImageSingleView::transformImage(STImage::EnTransformCodes _Transform, const QModelIndex& _Index)
{
	if (_Index.isValid())
	{
		STDom::DDoc* CurrentDoc = DocModel->doc(_Index);
		if (CurrentDoc->type() == STDom::DImageDoc::Type)
		{
			QApplication::setOverrideCursor(Qt::WaitCursor);
			QApplication::processEvents();
			STImage Img(CurrentDoc->fileInfo().absoluteFilePath());
			Img = Img.transform(_Transform);

			if (!Img.save(CurrentDoc->fileInfo().absoluteFilePath()))
				SMessageBox::critical(this, tr("Transforming Image"), tr("Error saving image %1, please check file permissions").arg(CurrentDoc->fileInfo().absoluteFilePath()));
			else
				CurrentDoc->updateThumbnail();
			QApplication::restoreOverrideCursor();
		}
	}
}

STFolderImageSingleView::STFolderImageSingleView(QWidget *parent, int _DesiredWidth, bool _HeaderMode) :
	QWidget(parent), DesiredWidth(_DesiredWidth), HeaderMode(_HeaderMode)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setSpacing(0);
	MLayout->setMargin(0);

	QHBoxLayout* TopLayout = new QHBoxLayout();
	MLayout->addLayout(TopLayout);
	if (_HeaderMode)
	{
		HeaderLabel = new QLabel(this);
		TopLayout->addWidget(HeaderLabel);
	}
	else
	{
		MToolBar = new QToolBar(this);
		MToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		TopLayout->addWidget(MToolBar);
		setupToolBarActions();
	}

	MProxModel = new QSortFilterProxyModel(this);
	MProxModel->setSortRole(Qt::EditRole);

	DocModel = new STDom::DDocModel(this);
	DocModel->setNoImagePixmap(QPixmap(":/st/wizards/nopreview.png"));
	MProxModel->setSourceModel(DocModel);

	LView = new STThumbnailView(this); //new QListView(this);
	LView->setViewMode(QListView::IconMode); //In iconmode DropIndicator isn't an image.
	LView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	LView->setModel(MProxModel);
	MLayout->addWidget(LView);
	PreviewItemDelegate* Delegate = new PreviewItemDelegate(LView);
	Delegate->setThumbnailDetails(PreviewItemDelegate::FileNameDetail | PreviewItemDelegate::DateDetail);
	connect(LView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditIndex(QModelIndex)));
	connect(Delegate, SIGNAL(editDocumentRequested(const QModelIndex&)), this, SLOT(slotEditIndex(const QModelIndex&)));
	connect(Delegate, SIGNAL(rotateDocumentRightRequested(const QModelIndex&)), this, SLOT(slotRotateRightIndex(const QModelIndex&)));
	connect(Delegate, SIGNAL(rotateDocumentLeftRequested(const QModelIndex&)), this, SLOT(slotRotateLeftIndex(const QModelIndex&)));

	LView->setItemDelegate(Delegate);
	LView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	LView->setSelectionBehavior(QAbstractItemView::SelectRows);
	LView->setGridSize(QSize(150, 200));
	connect(LView->selectionModel(),
			  SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& )),
			 this, SLOT( slotSelectionChanged(const QItemSelection& , const QItemSelection&)));
	connect(LView, SIGNAL(doubleClicked(const QModelIndex&)),
			  this, SIGNAL(editItem(const QModelIndex&)));

	ImageActionsToolBar = new QToolBar(this);
	ImageActionsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	TopLayout->addWidget(ImageActionsToolBar);
	setupImageActions();

}


void STFolderImageSingleView::updateGeometries()
{
	if (DocModel->rowCount())
	{
		if (!DesiredWidth)
			DesiredWidth = LView->viewport()->width();
		int Columns = DesiredWidth / LView->gridSize().width();
		if (Columns) //Defensive
		{
			int NumRows = (LView->model()->rowCount() / Columns) + ((LView->model()->rowCount() % Columns) != 0 );
			int MinHeight =  NumRows * LView->gridSize().height();
			LView->setMinimumHeight(MinHeight);
		}
	}
}

void STFolderImageSingleView::setDir(const QDir& _Dir)
{
	DocModel->clear();
	DocModel->setDocs(STImageTools::recursiveImagesEntryInfoList(_Dir.absolutePath(), false));
	setLabel(_Dir.dirName());
	setEmptyMode(DocModel->rowCount() == 0);
	MDir = _Dir;
	updateGeometries();
}

QFileInfoList STFolderImageSingleView::selectedFiles() const
{
	QFileInfoList Res;
	QModelIndexList SelIndexList = LView->selectionModel()->selectedRows(0);
	QModelIndexList::iterator it;
	for (it = SelIndexList.begin(); it != SelIndexList.end(); ++it)
	{
		Res.push_back(DocModel->doc(MProxModel->mapToSource(*it))->fileInfo());
	}
	return Res;
}


void STFolderImageSingleView::openFolderAction()
{
	emit openDir(MDir);
}

void STFolderImageSingleView::slotBigPreviewFolder()
{
	emit bigPreviewDir(MDir);
}

void STFolderImageSingleView::slotThumbnailSizeChanged(int _Value)
{
	int NewSize = _Value * 40;

	DocModel->setThumbnailSize(QSize(NewSize, NewSize));
	LView->setThumbnailSize(NewSize);
	LView->setIconSize(QSize(NewSize, NewSize));
	LView->setGridSize(QSize(NewSize + 20, NewSize + 70));
	updateGeometries();
	LView->update();
}

void STFolderImageSingleView::slotEditIndex(const QModelIndex& _Index)
{
	emit editImage(DocModel->doc(MProxModel->mapToSource(_Index))->fileInfo().absoluteFilePath());
}


void STFolderImageSingleView::slotRotateRightIndex(const QModelIndex& _Index)
{
	transformImage(STImage::Rotate90, _Index);
}

void STFolderImageSingleView::slotRotateLeftIndex(const QModelIndex& _Index)
{
	transformImage(STImage::Rotate270, _Index);
}
