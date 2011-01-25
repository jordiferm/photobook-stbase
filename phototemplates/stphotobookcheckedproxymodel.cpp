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
#include "stphotobookcheckedproxymodel.h"
#include <QApplication>
#include <QMimeData>  
#include <QUrl>
#include <QtConcurrentRun>

#include "stphotobook.h" 
#include "stimage.h" 

void STPhotoBookCheckedProxyModel::cancelPendingOps()
{
	UpdateCheckCanceled = true;
	if (UpdateCheckedFuture.isRunning())
		UpdateCheckedFuture.waitForFinished();
}

void STPhotoBookCheckedProxyModel::intUpdateCheckedFiles()
{
	UpdateCheckCanceled = false;
	ChecksPending = false;
	if (!PhotoBook)
		return;
	if (STDom::DDocModel* ColImageModel = qobject_cast<STDom::DDocModel*>(sourceModel()))
	{
		//QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		int Vfor = 0;
		while (Vfor < rowCount() && !UpdateCheckCanceled)
		{
			QModelIndex ProxIndex = index(Vfor, 0);
			QModelIndex CurrIndex = mapToSource(ProxIndex);
			STDom::DDoc* CurrDoc = ColImageModel->doc(CurrIndex);
			if (CurrDoc) //Defensive
			{
				if (CurrDoc->md5Sum().isEmpty())
				{
					CurrDoc->setMd5Sum(STImage::hashString(CurrDoc->filePath()));
					ColImageModel->setMD5Sum(CurrIndex, CurrDoc->md5Sum());
					//if (Vfor % qMax(1, (rowCount() / 10))) //To improve GUI speed.
					//	QApplication::processEvents();
				}
				setNumMatches(ProxIndex, PhotoBook->numImageMatches(CurrDoc->md5Sum()));
			}
			Vfor++;
		}
		//QApplication::restoreOverrideCursor();
	}
	if (ChecksPending && !UpdateCheckCanceled)
		intUpdateCheckedFiles();
}

STPhotoBookCheckedProxyModel::STPhotoBookCheckedProxyModel(QObject* _Parent, const QImage& _CheckedImage) :
		STCheckedProxyModel(_Parent, _CheckedImage), PhotoBook(0), ReportDropsMd5(false), ChecksPending(false)
{
	connect(this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex& , int , int )), this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));
}

STPhotoBookCheckedProxyModel::~STPhotoBookCheckedProxyModel()
{
	cancelPendingOps();
}

void STPhotoBookCheckedProxyModel::setPhotoBook(const STPhotoBook* _PhotoBook)
{
	if (PhotoBook)
	{
		disconnect(PhotoBook, SIGNAL(imageDropped(const QString&, const QString&)), this, SLOT(slotCheckImage(const QString&, const QString&)));
		disconnect(PhotoBook, SIGNAL(imageRemoved(const QString&, const QString&)), this, SLOT(slotUncheckImage(const QString&, const QString&)));
		cancelPendingOps();
	}

	PhotoBook = _PhotoBook; 
	connect(_PhotoBook, SIGNAL(imageDropped(const QString&, const QString&)), this, SLOT(slotCheckImage(const QString&, const QString&)));
	connect(_PhotoBook, SIGNAL(imageRemoved(const QString&, const QString&)), this, SLOT(slotUncheckImage(const QString&, const QString&)));
	updateCheckedFiles(); 
}

void STPhotoBookCheckedProxyModel::setSourceModel(STDom::DDocModel* _SourceModel)
{
	cancelPendingOps();
	STCheckedProxyModel::setSourceModel(_SourceModel);
	updateCheckedFiles(); 
}

STDom::DDocModel* STPhotoBookCheckedProxyModel::sourceModel() const
{
	return qobject_cast<STDom::DDocModel*>(STCheckedProxyModel::sourceModel());
}


void STPhotoBookCheckedProxyModel::updateMD5SumImageMatches(const QString& _MD5Sum, bool _Added)
{
	if (!UpdateCheckedFuture.isFinished())
	{
		ChecksPending = true;
		return;
	}
	if (STDom::DDocModel* ColImageModel = qobject_cast<STDom::DDocModel*>(sourceModel()))
	{
		//Lets look for image in sourceModel
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		int Cnt = 0; 
		bool Found = false;
		while (!Found && Cnt < rowCount())
		{
			QModelIndex ProxIndex = index(Cnt, 0);
			QModelIndex CurrIndex = mapToSource(ProxIndex);
			STDom::DDoc* CurrDoc = ColImageModel->doc(CurrIndex);
			if (CurrDoc->md5Sum().isEmpty())
			{
				CurrDoc->setMd5Sum(STImage::hashString(CurrDoc->filePath()));

				ColImageModel->setMD5Sum(CurrIndex, CurrDoc->md5Sum());
				if (Cnt % qMax(1, (rowCount() / 10))) //To improve GUI speed.
					QApplication::processEvents();
			}
			Found = CurrDoc->md5Sum() == _MD5Sum;
			if (Found)
			{
				if (_Added)
					incNumMatches(ProxIndex);
				else
					decNumMatches(ProxIndex);
			}
			Cnt++;
		}
		QApplication::restoreOverrideCursor();
	}
}

bool STPhotoBookCheckedProxyModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
{
	bool Res = STCheckedProxyModel::dropMimeData(data, action, row, column, parent);

	if (action != Qt::IgnoreAction && column <= 0 )
	{
		if (data->hasUrls() && ReportDropsMd5)
		{
			QList<QUrl> Urls = data->urls();
			QList<QUrl>::const_iterator it;
			for (it = Urls.begin(); it != Urls.end(); ++it)
			{
				QUrl CurrentUrl = *it;
				if (!CurrentUrl.isEmpty() && CurrentUrl.isValid())
				{
					//Only local file uris are supported.
					QString ImagePath = CurrentUrl.toLocalFile();
					if (!ImagePath.isEmpty())
						emit imageDropped(ImagePath, STImage::hashString(ImagePath));
				}
			}
		
		}
		updateCheckedFiles();
	}
	return Res;
}

/*
Qt::DropActions STPhotoBookCheckedProxyModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags STPhotoBookCheckedProxyModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;
	return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
//	if (index.isValid())
//		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
//	else
//	{
//		return defaultFlags;
//	}
}

QStringList STPhotoBookCheckedProxyModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

QMimeData* STPhotoBookCheckedProxyModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	if (OPhotoCollectionImageModel* ColImageModel = qobject_cast<OPhotoCollectionImageModel*>(sourceModel()))
	{
		QList<QUrl> Urls; 
		foreach(QModelIndex Index, indexes)
		{
			if (Index.isValid() && Index.model() == this)
			{				
				QModelIndex SourceIndex = mapToSource(Index); 
				if (SourceIndex.isValid())
				{
					OPhotoCollectionImage Image = ColImageModel->image(SourceIndex); 
					Urls.push_back(QUrl::fromLocalFile(Image.fileInfo().absoluteFilePath())); 
				}
			}
		}
		if (!Urls.isEmpty())
			mimeData->setUrls(Urls);
	}
	return mimeData;
}

bool STPhotoBookCheckedProxyModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
{
	if (action == Qt::IgnoreAction)
		return true;

//	if (!data->hasFormat("text/uri-list"))
//		return false;

	if (column > 0)
		return false;

	//For Item Drops
	int beginRow;

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = parent.row();
	else
		beginRow = rowCount(QModelIndex());

	bool Res = false;
	if (OPhotoCollectionImageModel* SourceModel = qobject_cast<OPhotoCollectionImageModel*>(sourceModel()))
	{
		if (data->hasUrls())
		{
			QList<QUrl> Urls = data->urls();
			QList<QUrl>::const_iterator it;
			for (it = Urls.begin(); it != Urls.end(); ++it)
			{
				QUrl CurrentUrl = *it;
				if (!CurrentUrl.isEmpty() && CurrentUrl.isValid())
				{
					QString LocalPath = CurrentUrl.toString();
					//Only local file uris are supported.
					QString ImagePath = CurrentUrl.toLocalFile();
					if (!ImagePath.isEmpty())
					{
						OPhotoCollectionImage CurrentImage(ImagePath);
						if (!SourceModel->contains(CurrentImage))
						{
							QModelIndex DestIndex = index(beginRow , 0);
							if (DestIndex.isValid())
								SourceModel->insertImage(mapToSource(DestIndex),  CurrentImage);
							else
								SourceModel->addImage(CurrentImage);

							emit imageDropped(ImagePath, STImage::hashString(ImagePath));
							Res = true;
						}
					}
				}
			}
			updateCheckedFiles();
		}
	}
	return Res;
}
*/


void STPhotoBookCheckedProxyModel::updateCheckedFiles()
{
	cancelPendingOps();
	UpdateCheckedFuture = QtConcurrent::run(this, &STPhotoBookCheckedProxyModel::intUpdateCheckedFiles);
}

void STPhotoBookCheckedProxyModel::slotCheckImage(const QString&, const QString& _MD5Sum)
{
	updateMD5SumImageMatches(_MD5Sum, true);
}

void STPhotoBookCheckedProxyModel::slotUncheckImage(const QString&, const QString& _MD5Sum)
{
	updateMD5SumImageMatches(_MD5Sum, false);
}

void STPhotoBookCheckedProxyModel::slotRowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{
	if (ReportDropsMd5)
	{
		if (STDom::DDocModel* SourceModel = qobject_cast<STDom::DDocModel*>(sourceModel()) )
		{
			for (int Vfor = start; Vfor <= end; Vfor++)
			{
				QString FilePath = SourceModel->doc(mapToSource(index(Vfor, 0)))->fileInfo().absoluteFilePath();
				emit imageRemoved(FilePath, STImage::hashString(FilePath));
			}
		}
	}
}
