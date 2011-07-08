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

#ifndef STTEMPLATEMODEL_H
#define STTEMPLATEMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QFuture>
#include <QEvent>
#include <QWaitCondition>
#include <QThread>
#include "stphotolayout.h"
#include "stphototemplatesexport.h"


/**
Event to inform thumbnail updates.

	@author jordif.starblitz@gmail.com
*/

class LayoutThumbnailLoadedEvent : public QEvent
{
public:
	static const QEvent::Type LayoutThumbnailLoadedEventType;

private:
	STPhotoLayoutTemplate Template;
	int Index;
	QWaitCondition* WaitCondition;

public:
	LayoutThumbnailLoadedEvent(const STPhotoLayoutTemplate& _Template, int _Index, QWaitCondition* _WaitCondition)
			: QEvent(LayoutThumbnailLoadedEventType), Template(_Template), Index(_Index), WaitCondition(_WaitCondition) {}
	int index() const { return Index; }
	STPhotoLayoutTemplate templateLayout() const { return Template; }
	void wakeUp();
};

/**
Thread to load images in background.

	@author jordif.starblitz@gmail.com
*/

class LoadLayoutThumbnailThread : public QThread
{
	typedef QPair<STPhotoLayoutTemplate, int> TDocAndIndex;
	typedef QList<TDocAndIndex> TImagesToLoadList;
	TImagesToLoadList ImagesToLoadList;
	QMutex Mutex;
	bool IsRunning;
	QWaitCondition WaitCondition;

public:
	LoadLayoutThumbnailThread(QObject* _Parent = 0);
	void loadDocThumbnail(STPhotoLayoutTemplate _Template, int _Index);
	void stop();
	void removeDocThumbnailLoad(STPhotoLayoutTemplate _Template);
	void clear();
	void run();
};


/**
Model for album templates.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STTemplateModel : public QAbstractListModel
{
private:
	QSize ThumbnailMaxSize;
	QPixmap NoImagePixmap;
	STPhotoLayout::TTemplateList Templates;
	QList<QImage>	TemplateThumbnails; 
	QList<QImage> DummyImages; 
	QFuture<void> CreateThumbsFuture;
	bool CreateThumbsCanceled;
	LoadLayoutThumbnailThread* MLoadThread;

	void cancelPendingOps();
	void createThumbnails();
	QImage createSampleThumbnail(const STPhotoLayoutTemplate _Template) const;

public:
	STTemplateModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QFont displayFont() const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void setLayout(const STPhotoLayout& _PhotoLayout);
	void setTemplateList(const STPhotoLayout::TTemplateList& _TemplateList);
	void setAlbum(const STPhotoBookTemplate& _Album);
	void setThumbnailMaxSize(QSize _Value);
	QSize thumbnailMaxSize() const; 
	STPhotoLayoutTemplate indexTemplate(const QModelIndex& _Index) const;
	bool removeRows(int _Row, int _Count, const QModelIndex& _Parent);
	void addTemplate(const STPhotoLayoutTemplate& _Template); 
	void addTemplateList(const STPhotoLayout::TTemplateList& _TemplateList);
	bool event(QEvent* _Event);
	STPhotoLayout::TTemplateList templateList() const { return Templates; }
	void setDummyImages(const QList<QImage>& _ImageList) { DummyImages = _ImageList; }
	void setNoImagePixmap(const QPixmap& _Pixmap) { NoImagePixmap = _Pixmap; }
};

#endif
