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

#ifndef DDOCMODEL_H
#define DDOCMODEL_H

#include <QAbstractTableModel>
#include <QEvent>
#include <QImage>
#include <QThread>
#include <QPixmap>
#include <QMutex>
#include <QWaitCondition>
#include "stdomexport.h"
#include "ddoclist.h"
#include "ddoc.h"


namespace STDom
{

/**
Event to inform thumbnail updates.

	@author jordif.starblitz@gmail.com
*/

class ST_DOM_EXPORT ThumbnailLoadedEvent : public QEvent
{
public:
	static const QEvent::Type ThumbnailLoadedEventType;

private:
	QModelIndex Index;
	QImage Thumbnail;
	DDoc Doc;
	QWaitCondition* WaitCondition;

public:
	ThumbnailLoadedEvent(const QModelIndex& _Index, const QImage& _Thumbnail, DDoc _Doc, QWaitCondition* _WaitCondition)
			: QEvent(ThumbnailLoadedEventType), Index(_Index), Thumbnail(_Thumbnail), Doc(_Doc), WaitCondition(_WaitCondition) {}
	QModelIndex index() { return Index; }
	QImage thumbnail() { return Thumbnail; }
	DDoc doc() { return Doc; }
	void wakeUp();
};


/**
Thread to load images in background.

	@author jordif.starblitz@gmail.com
*/

class ST_DOM_EXPORT LoadThumbnailThread : public QThread
{
	typedef QPair<DDoc*, QModelIndex> TDocAndIndex;
	typedef QList<TDocAndIndex> TImagesToLoadList;
	TImagesToLoadList ImagesToLoadList;
	QMutex Mutex;
	bool IsRunning;
	QWaitCondition WaitCondition;

public:
	LoadThumbnailThread(QObject* _Parent = 0);
	void loadDocThumbnail(DDoc* _Doc, const QModelIndex& _Index);
	void stop();
	void removeDocThumbnailLoad(DDoc* _Doc);
	void clear();
	void run();
};

/**
DDoc Item Model.

	@author jordif.starblitz@gmail.com
*/

class DDoc;
class ST_DOM_EXPORT DDocModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum EnColumns
	{
		ColFile,
		ColDateTime,
		ColComment,
		ColSize,
		ColKey
	};

private:
	QPixmap NoImagePixmap;
	LoadThumbnailThread* MLoadThread;
	DDocList DocList;
	QSize ThumbSize;

public:
	DDocModel(QObject* _Parent = 0);
	~DDocModel();

	int columnCount(const QModelIndex& _Parent = QModelIndex()) const { return 5; }
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	bool removeRows(int _Row, int _Count, const QModelIndex& _Parent = QModelIndex());
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	void setThumbnailSize(const QSize& _Size) { ThumbSize = _Size; }
	QSize thumbnailSize() const { return ThumbSize; }

	//!Change DataSet.
	void clear();
	void setDocs(const QFileInfoList& _Images);
	QFileInfoList docs() const;
	QModelIndex addDoc(DDoc* _Doc);
	void setNoImagePixmap(const QPixmap& _Pixmap) { NoImagePixmap = _Pixmap; }
	void insertDoc(const QModelIndex& _Index, DDoc* _Doc);
	DDoc* doc(const QModelIndex& _Index) const;
	void setMD5Sum(const QModelIndex& _Index, const QString& _MD5Sum);
	void setDoc(const QModelIndex& _Index, DDoc* _Doc);
	QModelIndex docIndex(const DDoc* _Doc);

	//Drag & Drop.
	Qt::DropActions supportedDropActions() const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

	//Save on disk
	void saveDocList(const QString& _FileName);
	void loadDocList(const QString& _FileName);

protected:
	bool event(QEvent* _Event);

signals:
	void dataDropped();
};
}


#endif // DDOCMODEL_H
