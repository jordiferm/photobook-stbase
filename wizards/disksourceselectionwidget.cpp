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
#include "disksourceselectionwidget.h"

#include <QDirModel>
#include <QTreeView>
#include <QLayout> 
#include <QToolButton> 
#include <QHeaderView>
#include <QApplication>
#include <QAction> 
#include <QMenu> 
#include <QContextMenuEvent>
#include "stimagetools.h"
#include "smessagebox.h" 

//_____________________________________________________________________________
//
// Class DiskSourceSelectionWidget
//_____________________________________________________________________________


void DiskSourceSelectionWidget::createContextMenu()
{	
	ContextMenu = new QMenu(tr("&Folder"), this);
/*	QAction* RenameAction = new QAction(QIcon(":/rename.png"), tr("Rename Folder"), this);
	connect(RenameAction, SIGNAL(triggered( bool )), this, SLOT(renameCurrentFolder()));
	ContextMenu->addAction(RenameAction);

	ContextMenu->addSeparator();*/

	QAction* SelectAllAction = new QAction(tr("Select all Pictures"), this);
	connect(SelectAllAction, SIGNAL(triggered( bool )), this, SIGNAL(selectAllPictures()));
	ContextMenu->addAction(SelectAllAction);

	QAction* ClearSelectionAction = new QAction(tr("Clear selection"), this);
	connect(ClearSelectionAction, SIGNAL(triggered( bool )), this, SIGNAL(clearSelection()));
	ContextMenu->addAction(ClearSelectionAction);

	QAction* InvertSelectionAction = new QAction(tr("Invert selection"), this);
	connect(InvertSelectionAction, SIGNAL(triggered( bool )), this, SIGNAL(invertSelection()));
	ContextMenu->addAction(InvertSelectionAction);

	ContextMenu->addSeparator();

	QAction* SlideshowAction = new QAction(tr("Start Slideshow"), this);
	connect(SlideshowAction, SIGNAL(triggered( bool )), this, SLOT(slideShow())); 
	ContextMenu->addAction(SlideshowAction);

// 	ContextMenu->addSeparator();
// 
// 
// 	QAction* DeleteDiskAction = new QAction(stIcon(SIconFactory::EditDelete), tr("Delete"), this);
// 	connect(DeleteDiskAction, SIGNAL(triggered(bool)), this, SLOT(deleteCurrentFolder()));
// 	ContextMenu->addAction(DeleteDiskAction);
}

DiskSourceSelectionWidget::DiskSourceSelectionWidget(QWidget* _Parent) 
	: QWidget(_Parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(1);
	MLayout->setSpacing(0); 
	View = new QTreeView(this);
	View->header()->hide();
	connect(View, SIGNAL(clicked( const QModelIndex& )), this, SLOT(nodeClicked(const QModelIndex& )));
	MLayout->addWidget(View);
	
	Model = new QDirModel(this); 
	Model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	View->setModel(Model); 
	for ( int Vfor = 0; Vfor < Model->columnCount(); Vfor ++ )
	{
		View->setColumnHidden(Vfor, true); 
	}
	View->setColumnHidden(0, false); 
	View->setCurrentIndex(Model->index(QDir::homePath(), 0));

	QHBoxLayout* ButLayout = new QHBoxLayout; 
	MLayout->addLayout(ButLayout); 

	QToolButton* ButRefresh = new QToolButton(this); 
	ButRefresh->setIcon(QIcon(":/st/wizards/view_refresh.png")); 
	ButRefresh->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	ButRefresh->setText(tr("Refresh")); 
	ButRefresh->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred); 
	ButLayout->addWidget(ButRefresh); 
	connect(ButRefresh, SIGNAL(clicked()), Model, SLOT(refresh()));

	QToolButton* ButFindMedia = new QToolButton(this); 
	ButFindMedia->setIcon(QIcon(":/st/wizards/find_media.png")); 
	ButFindMedia->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	ButFindMedia->setText(tr("Find Media")); 
	ButFindMedia->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred); 
	ButLayout->addWidget(ButFindMedia); 
	connect(ButFindMedia, SIGNAL(clicked()), this, SLOT(findMedia()));
	createContextMenu();
}

void DiskSourceSelectionWidget::setCurrentPath(const QString& _DirPath)
{
	QModelIndex CurrPath = Model->index(_DirPath);
	View->setCurrentIndex(CurrPath); 
	nodeClicked(CurrPath); 
}

QString DiskSourceSelectionWidget::currentPath() const
{
	return Model->filePath(View->currentIndex());
}

void DiskSourceSelectionWidget::contextMenuEvent(QContextMenuEvent* _Event)
{
	if (View->currentIndex().isValid())
		ContextMenu->popup(_Event->globalPos());
}

void DiskSourceSelectionWidget::nodeClicked(const QModelIndex& _Index)
{
	emit pathClicked(Model->filePath(_Index)); 
	QFileInfoList ImageInfoList = STImageTools::recursiveImagesEntryInfoList(Model->filePath(_Index), false); 
	emit imagesSelected(ImageInfoList);
}

void DiskSourceSelectionWidget::findMedia()
{
	bool Found; 
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor)); 
#ifndef Q_OS_WIN32
	//TODO !!!!!: put /media on QSettings.
	QDir SPath = STImageTools::searchFirstPathWithImages(QDir("/media"), Found);
#else 
	QDir SPath = STImageTools::searchFirstDrivePathWithImages(Found);
#endif 
	qApp->restoreOverrideCursor(); 
	if (Found)
	{
		QModelIndex FoundIndex = Model->index(SPath.absolutePath()); 
		View->setCurrentIndex(FoundIndex); 
		nodeClicked(FoundIndex); 
	}
	else 
	{
		qApp->restoreOverrideCursor(); 
		SMessageBox::information(this, tr("Media finder."), tr("Sorry, could not find any media with images."));
	}
	qApp->restoreOverrideCursor(); 
}

void DiskSourceSelectionWidget::slideShow()
{
	emit selectAllPictures();
	emit startSlideShow();
}

