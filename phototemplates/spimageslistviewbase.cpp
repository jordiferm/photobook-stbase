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

#include "spimageslistviewbase.h"

#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QDir>

#include "stphotobookcheckedproxymodel.h"

#include "checkitemdelegate.h"
#include "stthumbnailview.h"
#include "spthumbnailsizewidget.h"
//#include "ophotocollection.h"
#include "stimagetools.h"


SPImagesListViewBase::SPImagesListViewBase(QWidget* _Parent, bool _ToolBarOnTop) :
		SPToolbarListView(Qt::Horizontal, _Parent, _ToolBarOnTop)
{
	SelectAllAction = new QAction(QIcon(":/phototemplates/rating.png"), tr("Select All (Ctrl+A)"), this);
	connect(SelectAllAction , SIGNAL(triggered()), this, SLOT(slotSelectAll()));
	toolBar()->addAction(SelectAllAction);
	SelectAllAction->setVisible(false);

	toolBar()->addSeparator();

	ThumbnailSizeWidget = new SPThumbnailSizeWidget(this);
	connect(ThumbnailSizeWidget, SIGNAL(valueChanged(int )), this, SLOT(slotThumbnailSizeChanged(int )));
	toolBar()->addWidget(ThumbnailSizeWidget);

	listView()->setThumbnailSize(100);
	listView()->setSpacing(0);
	CheckItemDelegate* PDelegate = new CheckItemDelegate(listView());
	//PDelegate->setShowToolTips(true);
	listView()->setItemDelegate(PDelegate);

	ImageModel = new STDom::DDocModel(this);
	ImageModel->setNoImagePixmap(QPixmap(":/phototemplates/hourglass.png"));
	ImageProxyModel = new STPhotoBookCheckedProxyModel(this, QImage(":/phototemplates/checked.png"));
	ImageProxyModel->setSourceModel(ImageModel);
	ImageProxyModel->setThumbnailSize(QSize(100, 100));
	setModel(ImageProxyModel);


//	ImageModel = new OPhotoCollectionImageModel(this);
//	ImageModel->setNoImagePixmap(QPixmap(":/phototemplates/hourglass.png"));
//	ImageProxyModel = new STPhotoBookCheckedProxyModel(this, QImage(":/phototemplates/checked.png"));
//	ImageProxyModel->setSourceModel(ImageModel);
//	ImageProxyModel->setThumbnailSize(QSize(100, 100));
//	setModel(ImageProxyModel);

}

void SPImagesListViewBase::setPath(const QString& _Path)
{
	CurrentPath = _Path;
	QFileInfoList ImageInfoList = STImageTools::recursiveImagesEntryInfoList(QDir(_Path), false);
	ImageModel->setDocs(ImageInfoList);
	updateCheckedFiles();
}

void SPImagesListViewBase::setImages(const QFileInfoList& _Images)
{
	ImageModel->setDocs(_Images);
	updateCheckedFiles();
}

void SPImagesListViewBase::addImage(const QFileInfo& _Image)
{
	ImageModel->addDoc(_Image);
	updateCheckedFiles();
}

void SPImagesListViewBase::removeImage(const QFileInfo& _Image)
{
	QModelIndex ImageIndex = ImageModel->docIndex(_Image);
	if (ImageIndex.isValid())
		ImageModel->removeDoc(ImageIndex);
}

void SPImagesListViewBase::setPhotoBook(const STPhotoBook* _PhotoBook)
{
	ImageProxyModel->setPhotoBook(_PhotoBook);
}

void SPImagesListViewBase::showSelectAllAction(bool _Show)
{
	SelectAllAction->setVisible(_Show);
}


void SPImagesListViewBase::selectImage(const QString& _ImageFileName)
{
	STDom::DDoc CDoc(_ImageFileName);
	QModelIndex SourceIndex = ImageModel->docIndex(&CDoc);
	if (SourceIndex.isValid())
	{
		QModelIndex Index = ImageProxyModel->mapFromSource(SourceIndex);
		if (Index.isValid())
		{
			listView()->selectionModel()->select(Index, QItemSelectionModel::SelectCurrent);
			listView()->scrollTo(Index, QAbstractItemView::PositionAtCenter);
		}
	}
}

#include <QScrollBar>
void SPImagesListViewBase::selectImageMD5(const QString& _Md5Sum)
{
	int Cnt = 0;
	bool Found = false;
	QModelIndex CurrIndex;
	while(!Found && Cnt < ImageModel->rowCount())
	{
		CurrIndex = ImageModel->index(Cnt, 0);
		Found = ImageModel->doc(CurrIndex)->md5Sum() == _Md5Sum;
		Cnt++;
	}
	if (Found && CurrIndex.isValid())
	{
		listView()->selectionModel()->select(CurrIndex, QItemSelectionModel::SelectCurrent);
		//listView()->scrollTo(CurrIndex, QAbstractItemView::PositionAtCenter);
		listView()->scrollTo(CurrIndex);
	}
}

void SPImagesListViewBase::setThumbnailSize(int _Value)
{
	ThumbnailSizeWidget->setValue(_Value);
	slotThumbnailSizeChanged(_Value);
}

QFileInfoList SPImagesListViewBase::selectedImages()
{
	QFileInfoList Res;

	QModelIndexList SelIndexs = listView()->selectionModel()->selectedRows(0);
	QModelIndexList::iterator it;
	for (it = SelIndexs.begin(); it != SelIndexs.end(); ++it)
	{
		QModelIndex CurrIndex = ImageProxyModel->mapToSource(*it);
		Res.push_back(ImageModel->doc(CurrIndex)->fileInfo());
	}
	return Res;
}

void SPImagesListViewBase::updateCheckedFiles()
{
	ImageProxyModel->updateCheckedFiles();
}

void SPImagesListViewBase::slotSelectAll()
{
	listView()->selectAll();
}

void SPImagesListViewBase::slotThumbnailSizeChanged(int _Value)
{
	int NewSize = _Value * 40;
	ImageProxyModel->setThumbnailSize(QSize(NewSize, NewSize));
	listView()->setThumbnailSize(NewSize);
	listView()->setIconSize(QSize(NewSize, NewSize));
	listView()->update();
}

void SPImagesListViewBase::clearImages()
{
	ImageModel->clear();
}
