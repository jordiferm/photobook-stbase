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

#include "imageslistview.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QDesktopServices>
#include <QFileDialog>

#include "doccheckedproxymodel.h"
#include "stimagetools.h"

#include "checkitemdelegate.h"
#include "stthumbnailview.h"
#include "thumbnailsizewidget.h"

using namespace SPhotoBook;
ImagesListView::ImagesListView(QWidget* _Parent) : ImagesListViewBase(_Parent)
{
	DesktopAction = new QAction(QIcon(":/photobook/desktop.png"), tr("Desktop"), this);
	connect(DesktopAction, SIGNAL(triggered()), this, SLOT(setDesktopPath()));
	//DesktopAction->setCheckable(true);
	toolBar()->addAction(DesktopAction);

	DiskAction = new QAction(QIcon(":/photobook/drive-harddisk.png"), tr("Disk"), this);
	connect(DiskAction, SIGNAL(triggered()), this, SLOT(selectDiskFolder()));
	DiskAction->setCheckable(true);
	DiskAction->setVisible(false);

	CollectionAction = new QAction(QIcon(":/phototemplates/collection.png"), tr("Collection"), this);
	connect(CollectionAction, SIGNAL(triggered()), this, SLOT(selectCollectionFolder()));
	CollectionAction->setCheckable(true);
	CollectionAction->setVisible(false); //TODO:Change it for Favourites !

	ImageSourceActions = new QActionGroup(this);
	//ImageSourceActions->addAction(DesktopAction);
	ImageSourceActions->addAction(DiskAction);
	ImageSourceActions->addAction(CollectionAction);
	ImageSourceActions->setExclusive(true);
	toolBar()->addActions(ImageSourceActions->actions());

	QAction* SelectFolderAction = new QAction(QIcon(":/photobook/folder.png"), tr("Select Folder"), this);
	connect(SelectFolderAction, SIGNAL(triggered()), this, SLOT(selectFolder()));
	toolBar()->addAction(SelectFolderAction);

//	SelectAllAction = new QAction(QIcon(":/phototemplates/rating.png"), tr("Select All (Ctrl+A)"), this);
//	connect(SelectAllAction , SIGNAL(triggered()), this, SLOT(slotSelectAll()));
//	toolBar()->addAction(SelectAllAction);
//	SelectAllAction->setVisible(false);

//	toolBar()->addSeparator();

//	QAction* AddImagesAction = new QAction(QIcon(":/phototemplates/list-add.png"), tr("Add"), this);
//	toolBar()->addAction(AddImagesAction);
//	AddImagesAction->setVisible(false);
//
//	QAction* RemoveImagesAction = new QAction(QIcon(":/phototemplates/list-remove.png"), tr("Remove"), this);
//	toolBar()->addAction(RemoveImagesAction);
//	RemoveImagesAction->setVisible(false);
	activateItemCounter(tr("Image(s)"));

	setDesktopPath();
}

void ImagesListView::setDiskPath(const QString& _CurrentPath)
{
	DiskAction->setChecked(true);
	setPath(_CurrentPath);
}

//void ImagesListView::setCollectionFolderKey(quint64 _Key)
//{
//	CollectionAction->setChecked(true);
//	OPhotoCollectionModel* ColModel = OPhotoCollection::model();
//	QModelIndex NodeIndex = ColModel->indexById(_Key);
//	ImageModel->setImages(ColModel->nodeImages(NodeIndex));
//	updateCheckedFiles();
//}


QAction* ImagesListView::addImageSourceAction(const QIcon& _Icon, const QString& _Text)
{
	QAction* NewAction = new QAction(_Icon, _Text, this);
	toolBar()->insertAction(DesktopAction, NewAction);
	//ImageSourceActions->addAction(NewAction);
	//NewAction->setCheckable(true);
	return NewAction;
}

void ImagesListView::setDesktopPath()
{
	//DesktopAction->setChecked(true);
	setDiskPath(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
}

void ImagesListView::selectFolder()
{
//	if (CollectionAction->isChecked())
//		selectCollectionFolder();
//	else
		selectDiskFolder();
}

void ImagesListView::selectDiskFolder()
{
	QString NewPath = QFileDialog::getExistingDirectory(this, tr("Please select a directory"), CurrentPath);
	if (!NewPath.isEmpty())
		setDiskPath(NewPath);

}

//void ImagesListView::selectCollectionFolder()
//{
//	OPhotoCollectionTreeDialog Dialog;
//	if (Dialog.exec() == QDialog::Accepted)
//		setCollectionFolderKey(Dialog.currentNodeKey());
//}

