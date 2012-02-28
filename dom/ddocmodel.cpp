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

#include "ddocmodel.h"
#include <QApplication>
#include <QMimeData>
#include <QUrl>
#include <QTextStream>
#include <QTextCodec>
#include "ddoc.h"
#include "ddocfactory.h"

using namespace STDom;


//_____________________________________________________________________________
//
// Class ThumbnailLoadedEvent
//_____________________________________________________________________________

const QEvent::Type ThumbnailLoadedEvent::ThumbnailLoadedEventType = static_cast<QEvent::Type>(QEvent::User + 1);



void ThumbnailLoadedEvent::wakeUp()
{
	WaitCondition->wakeAll();
}


//_____________________________________________________________________________
//
// Class LoadThumbnailThread
//_____________________________________________________________________________

LoadThumbnailThread::LoadThumbnailThread(QObject* _Parent) : QThread(_Parent), IsRunning(false)
{}

void LoadThumbnailThread::loadDocThumbnail(DDoc* _Doc, const QModelIndex& _Index)
{
	TDocAndIndex DocAndIndex(_Doc, _Index);
	Mutex.lock();
	if (!ImagesToLoadList.contains(DocAndIndex))
		ImagesToLoadList.push_back(DocAndIndex);
	Mutex.unlock();
	if (!isRunning())
		start();
}

void LoadThumbnailThread::stop()
{
	if (IsRunning)
	{
		IsRunning = false;
		wait();
	}
}

void LoadThumbnailThread::removeDocThumbnailLoad(DDoc* _Doc)
{
	Mutex.lock();
	//Look for it
	bool Found = false;
	int CurrentIndex = 0;
	while (!Found && CurrentIndex < ImagesToLoadList.count())
	{
		Found = ImagesToLoadList[CurrentIndex].first == _Doc;
		if (!Found)
			CurrentIndex++;
	}
	if (Found)
		ImagesToLoadList.removeAt(CurrentIndex);
	Mutex.unlock();
}

void LoadThumbnailThread::clear()
{
	Mutex.lock();
	ImagesToLoadList.clear();
	Mutex.unlock();
}

void LoadThumbnailThread::run()
{
	bool IsRunning = true;
	bool Empty = false;
	while ( !Empty > 0 && IsRunning)
	{
		Mutex.lock();
		Empty = ImagesToLoadList.size() == 0;
		Mutex.unlock();
		if (!Empty)
		{
			Mutex.lock();
			TDocAndIndex DocAndIndex = ImagesToLoadList.first();
			ImagesToLoadList.pop_front();
			ThumbnailLoadedEvent* NewEvent = new ThumbnailLoadedEvent(DocAndIndex.second, DocAndIndex.first->loadThumbnail(), *DocAndIndex.first, &WaitCondition);
			QApplication::postEvent(parent(),NewEvent);
			WaitCondition.wait(&Mutex, 200);
			Mutex.unlock();
		}
	}
}

//_____________________________________________________________________________
//
// Class DDocModel
//_____________________________________________________________________________

DDocModel::DDocModel(QObject* _Parent) : QAbstractTableModel(_Parent)
{
	MLoadThread = new LoadThumbnailThread(this);
	ThumbSize = DDoc::thumbnailSize();
}

DDocModel::~DDocModel()
{
	//MLoadThread->stop();
	MLoadThread->terminate();
	MLoadThread->wait();
	//while (MLoadThread->isRunning())
	//	sleep(100);
}


int DDocModel::rowCount(const QModelIndex& _Parent) const
{
	return DocList.size();
}


bool DDocModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
	for (int Vfor = 0; Vfor < _Count; Vfor++)
	{
		MLoadThread->removeDocThumbnailLoad(DocList.dDocs()[_Row + Vfor]);
		DocList.removeAt(_Row + Vfor);
	}
	endRemoveRows();
	return true;
}

QVariant DDocModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (!_Index.isValid())
		return Res;
	if (_Index.row() > DocList.size())
		return Res;

	DDoc* Doc = DocList.dDocs()[_Index.row()];
	if (_Role == Qt::DisplayRole)
	{
		switch (_Index.column())
		{
			case ColFile :
			{
				Res = Doc->fileInfo().baseName();
			}
			break;
			case ColDateTime :
				Res = Doc->dateTime();
				break;
			case ColComment :
				Res = Doc->comment();
				break;
			case ColSize :
				Res = QString("%1 KByte").arg(Doc->size() / 1024);
				break;
			case ColKey :
				Res = Doc->key();
				break;
		}
	}
	else if (_Role == Qt::DecorationRole)
	{
		//!TODO Thumbnail calculation.
		if (!Doc->thumbnailLoaded())
			MLoadThread->loadDocThumbnail(Doc, _Index);
		else
		{
			QPixmap RPixmap = Doc->thumbnail();
			if (!RPixmap.isNull())
				Res = RPixmap.scaled(ThumbSize, Qt::KeepAspectRatio);
		}
		if (Res.isNull())
			Res = NoImagePixmap; //If there is no thumbnail returns standard image.

	}
	else if (_Role == Qt::ToolTipRole)
	{
		QString StrInfo;
		StrInfo = "<img src=\"" + Doc->filePath() + "\" width=300 height=200></img>" +
				"<table cellspacing=0 cellpadding=0><tr><td><b>" + tr("File:") + "</b></td><td>" + Doc->fileInfo().fileName() + "</td></tr>" +
				"<tr><td><b>" + tr("Modified:") + "</b></td><td>";
#if QT_VERSION >= 0x040400
		StrInfo += Doc->fileInfo().lastModified().toString(Qt::SystemLocaleShortDate);
#else
		StrInfo += IFInfo.lastModified().toString(Qt::SystemLocaleDate);
#endif

		StrInfo += "</td></tr></table>";
		Res = StrInfo;
	}
	else if (_Role == Qt::EditRole)
	{
		switch (_Index.column())
		{
			case ColFile :
			{
				QFileInfo IFInfo(Doc->filePath());
				Res = IFInfo.absoluteFilePath();
			}
			break;
			case ColDateTime :
				Res = Doc->dateTime();
				break;
			case ColComment :
				Res = Doc->comment();
				break;
			case ColSize :
				Res = Doc->size();
				break;
			case ColKey :
				Res = Doc->key();
				break;
		}
	}
	else if (_Role == Qt::SizeHintRole)
	{
		Res = QSize(ThumbSize.width() + 8, ThumbSize.height());
	}
	return Res;
}

void DDocModel::clear()
{
	MLoadThread->clear();
	MLoadThread->stop();
	DocList.clear();
	reset();
}

void DDocModel::setDocs(const QFileInfoList& _Files)
{
	MLoadThread->clear();
	MLoadThread->stop();
	DocList.clear();
	DocList.addDocs(_Files);
	reset();
}

QFileInfoList DDocModel::docs() const
{
	QFileInfoList Res;
	for (int Vfor = 0; Vfor < DocList.size(); Vfor++)
		Res.push_back(DocList.dDocs()[Vfor]->fileInfo());
	return Res;
}

QModelIndex DDocModel::addDoc(DDoc* _Doc)
{
	beginInsertRows(QModelIndex(), DocList.size(), DocList.size() );
	DocList.addDoc(_Doc);
	endInsertRows();
	return index(DocList.dDocs().count() -1, 0);
}

/*! Overloaded function provided for convenience */
QModelIndex DDocModel::addDoc(const QFileInfo& _Doc)
{
	STDom::DDoc* CurrDoc = DDocFactory::newDoc(_Doc);
	if (!DocList.contains(CurrDoc))
		addDoc(CurrDoc);
	else
		delete CurrDoc;
}

void DDocModel::removeDoc(const QModelIndex& _Index)
{
	if (_Index.isValid() && _Index.row() < DocList.size())
	{
		beginRemoveRows(QModelIndex(), _Index.row(), _Index.row() );
		DocList.removeAt(_Index.row());
		endRemoveRows();
	}
}

void DDocModel::insertDoc(const QModelIndex& _Index, DDoc* _Doc)
{
	if (_Index.isValid() && _Index.row() < DocList.size())
	{
		beginInsertRows(QModelIndex(), _Index.row(), _Index.row() );
		DocList.insertDoc(_Index.row(), _Doc);
		endInsertRows();
	}
	else
		qWarning("DDocModel::insertImage invalid index: %d. %d", _Index.row(), _Index.column());
}

//! \return null if index is not valid.
DDoc* DDocModel::doc(const QModelIndex& _Index) const
{
	DDoc* Res = NULL; //Null Image
	if (_Index.isValid() && _Index.row() < DocList.size() )
		Res = DocList.dDocs()[_Index.row()];

	return Res;
}


void DDocModel::setMD5Sum(const QModelIndex& _Index, const QString& _MD5Sum)
{
	if (_Index.isValid() && _Index.row() < DocList.size())
	{
		doc(_Index)->setMd5Sum(_MD5Sum);
	}
}

//! deletes the old doc and sets _Doc in index _Index.
void DDocModel::setDoc(const QModelIndex& _Index, DDoc* _Doc)
{
	if (_Index.isValid())
	{
		if (_Index.row() < DocList.size())
		{
			DocList.setDoc(_Index.row(), _Doc);
			dataChanged(_Index, _Index);
		}
	}
}

QModelIndex DDocModel::docIndex(const DDoc* _Doc) const
{
	QModelIndex Res;
	int ImageRow = DocList.indexOf(_Doc);
	if ( ImageRow >= 0 )
		Res = index(ImageRow, 0);

	return Res;
}

QModelIndex DDocModel::docIndex(const QFileInfo& _File) const
{
	QModelIndex Res;
	STDom::DDoc CDoc(_File);
	QModelIndex SourceIndex = docIndex(&CDoc);
	if (SourceIndex.isValid())
		Res = SourceIndex;
	return Res;
}


Qt::DropActions DDocModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags DDocModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;
	return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList DDocModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

QMimeData* DDocModel::mimeData(const QModelIndexList &indexes) const
{
	QList<QUrl> Urls;

	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid() && Index.column() == 0 && Index.model() == this && Index.row() < rowCount())
		{
			DDoc* Doc = doc(Index);
			Urls.push_back(QUrl::fromLocalFile(Doc->fileInfo().absoluteFilePath()));
		}
	}
	if (!Urls.isEmpty())
	{
		QMimeData *mimeData = new QMimeData();
		mimeData->setUrls(Urls);
		return mimeData;
	}
	return QAbstractTableModel::mimeData(indexes);

}

bool DDocModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
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

	if (data->hasUrls())
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
				{
					//OPhotoCollectionImage CurrentImage(ImagePath);
					STDom::DDoc* CurrDoc = DDocFactory::newDoc(QFileInfo(ImagePath));
					if (!DocList.contains(CurrDoc))
					{
						QModelIndex DestIndex = index(beginRow , 0);
						if (DestIndex.isValid())
							insertDoc(DestIndex, CurrDoc);
						else
							addDoc(CurrDoc);
						beginRow++;

						//emit imageDropped(ImagePath, STImage::hashString(ImagePath));
						Res = true;
					}
					else
						delete CurrDoc;
				}
			}
		}

	}
	if (Res)
		emit dataDropped();
	return Res;
}


void DDocModel::saveDocList(const QString& _FileName)
{
	QFile File(_FileName);
	if (File.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream Out(&File);
		Out.setCodec(QTextCodec::codecForName("UTF-8"));

		for (int Vfor = 0; Vfor < rowCount(); Vfor++)
		{
			Out << doc(index(Vfor, 0))->fileInfo().absoluteFilePath() << endl;
		}
	}
	File.close();
}

void DDocModel::loadDocList(const QString& _FileName)
{
	QFile File(_FileName);
	if (File.open(QIODevice::ReadOnly))
	{
		clear();
		QTextStream InStrm(&File);
		InStrm.setCodec(QTextCodec::codecForName("UTF-8"));
		QString CurrLine = InStrm.readLine();
		QFileInfoList Files;
		while (!CurrLine.isEmpty())
		{
			Files.push_back(QFileInfo(CurrLine));
			CurrLine = InStrm.readLine();
		}
		setDocs(Files);
	}
	File.close();
}

bool DDocModel::event(QEvent* _Event)
{

	if (_Event->type() == ThumbnailLoadedEvent::ThumbnailLoadedEventType)
	{
		ThumbnailLoadedEvent* CEvent = static_cast<ThumbnailLoadedEvent*>(_Event);
		QModelIndex ChandedIndex = CEvent->index();
		CEvent->doc().cacheThumbnail(CEvent->thumbnail());
		CEvent->wakeUp();
		emit dataChanged(ChandedIndex, ChandedIndex);

	}
	return QAbstractTableModel::event(_Event);
}
