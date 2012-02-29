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

#include "stfolderimageview.h"
#include <QLayout>
#include <QScrollArea>
#include <QDir>
#include <QDebug>
#include <QToolBar>
#include <QAction>
#include <QSettings>
#include <QDesktopServices>
#include <QApplication>

#include "stfolderimagesingleview.h"
#include "obigimagepreviewwidget.h"


STFolderImageView::STFolderImageView(QWidget *parent) :
	QWidget(parent), CurrentDirIsNull(true)
{
	ScrArea = new QScrollArea(this);
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	IPWidget = new OBigImagePreviewWidget(this);
	connect(IPWidget, SIGNAL(includeImage(QFileInfo)), this, SIGNAL(addImage(QFileInfo)));
	connect(IPWidget, SIGNAL(excludeImage(QFileInfo)), this, SIGNAL(removeImage(QFileInfo)));
	connect(IPWidget, SIGNAL(closeRequest()), this, SLOT(slotHideBigPreview()));
	MLayout->addWidget(IPWidget);

	MLayout->setSpacing(0);
	MLayout->setMargin(0);
	MLayout->addWidget(ScrArea);

	ViewsWidget = new QWidget(this);
	ViewLayout = new QVBoxLayout(ViewsWidget);
	ViewLayout->setMargin(0);
	ViewLayout->setSpacing(2);
	ScrArea->setWidget(ViewsWidget);
	ScrArea->setWidgetResizable(true);

	TBTopActions = new QToolBar(ViewsWidget);
	TBTopActions->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ViewLayout->addWidget(TBTopActions);
	QAction* CDUpAction = new QAction(QIcon(":/cdup.png"), tr("Parent Folder"), this);
	TBTopActions->addAction(CDUpAction);
	connect(CDUpAction, SIGNAL(triggered()), this, SLOT(slotCDUpAction()));
	TBTopActions->setVisible(false);

	QSpacerItem* BottomSpacer = new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	ViewLayout->addItem(BottomSpacer);

	showBigImagePreview(false);
}

void STFolderImageView::showBigImagePreview(bool _Show)
{
	IPWidget->setVisible(_Show);
	ScrArea->setVisible(!_Show);
}

void STFolderImageView::setStoredDir()
{
	QSettings Settings;
	QDir StoredDir(Settings.value("imagesdir", QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)).toString());

	if (StoredDir.exists())
		setDir(StoredDir);
}

void STFolderImageView::storeDir(const QString& _Dir)
{
	QSettings Settings;
	Settings.setValue("imagesdir", _Dir);
}

void STFolderImageView::storeCurrentDir()
{
	QSettings Settings;
	if (currentDir().exists() && !CurrentDirIsNull) //Defensive
		storeDir(currentDir().absolutePath());
}

void STFolderImageView::setSaveDir()
{
	setDir(QApplication::applicationDirPath());
}

void STFolderImageView::clear()
{
	QList<STFolderImageSingleView *> ViewsList = ViewsWidget->findChildren<STFolderImageSingleView *>();
	QList<STFolderImageSingleView *>::iterator it;
	for (it = ViewsList.begin(); it != ViewsList.end(); it++)
	{
		ViewLayout->removeWidget(*it);
		(*it)->close();
		(*it)->deleteLater();
	}
}

void STFolderImageView::addDirs(const QFileInfoList& _Files)
{
	QStringList Dirs;
	QFileInfoList::const_iterator it;
	for (it = _Files.begin(); it != _Files.end(); ++it)
	{
		if (it->isDir())
			addDir(QDir(it->absoluteFilePath()));
	}

}

void STFolderImageView::addDir(const QDir& _Dir, bool _HeaderMode)
{
	STFolderImageSingleView* NView = new STFolderImageSingleView(ViewsWidget, width() - 50, _HeaderMode);
	//NView->setAttribute(Qt::WA_DeleteOnClose);
	connect(NView, SIGNAL(openDir(QDir)), this, SLOT(setDir(QDir)));
	connect(NView, SIGNAL(editImage(QString)), this, SIGNAL(editImage(QString)));
	connect(NView, SIGNAL(bigPreviewDir(QDir)), this, SLOT(slotShowBigPreview(QDir)));
	//ViewLayout->addWidget(NView);
	ViewLayout->insertWidget(ViewLayout->count() -1, NView);
	NView->setDir(_Dir);
}

bool STFolderImageView::isEmpty() const
{
	return ViewsWidget->findChildren<STFolderImageSingleView *>().size() == 0;
}

QFileInfoList STFolderImageView::selectedFiles() const
{
	QFileInfoList Res;
	QList<STFolderImageSingleView *> ViewsList = ViewsWidget->findChildren<STFolderImageSingleView *>();
	QList<STFolderImageSingleView *>::const_iterator it;
	for (it = ViewsList.begin(); it != ViewsList.end(); it++)
	{
		Res+= (*it)->selectedFiles();
	}
	return Res;
}

void STFolderImageView::setDir(const QDir& _Dir)
{
	TBTopActions->setVisible(!_Dir.isRoot());
	CurrentDirIsNull = false;
	CurrentDir = _Dir;
	QFileInfoList FileList;
	FileList += _Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	clear(); //Do not refer to _Dir parameter after clear !
	addDir(CurrentDir, true);
	addDirs(FileList);
}

void STFolderImageView::slotCDUpAction()
{
	QDir ParentDir = currentDir();
	if (ParentDir.cdUp())
		setDir(ParentDir);
}

void STFolderImageView::slotShowBigPreview(const QDir& _Dir)
{
	IPWidget->clear();
	QApplication::processEvents();
	showBigImagePreview(true);
	QApplication::processEvents();
	IPWidget->setDir(_Dir);
}

void STFolderImageView::slotHideBigPreview()
{
	showBigImagePreview(false);
}
