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

#include "sttemplatemodel.h"
#include "sttemplatescene.h"

#include <QPixmap>
#include <QPainter>
#include <QtConcurrentRun>
#include <QApplication>


const QEvent::Type LayoutThumbnailLoadedEvent::LayoutThumbnailLoadedEventType = static_cast<QEvent::Type>(QEvent::User + 10);


void LayoutThumbnailLoadedEvent::wakeUp()
{
	WaitCondition->wakeAll();
}


//_____________________________________________________________________________
//
// Class LoadLayoutThumbnailThread
//_____________________________________________________________________________

LoadLayoutThumbnailThread::LoadLayoutThumbnailThread(QObject* _Parent) : QThread(_Parent), IsRunning(false)
{}

void LoadLayoutThumbnailThread::loadDocThumbnail(STPhotoLayoutTemplate _Template, int _Index)
{
	TDocAndIndex DocAndIndex(_Template, _Index);
	Mutex.lock();
	if (!ImagesToLoadList.contains(DocAndIndex))
		ImagesToLoadList.push_back(DocAndIndex);
	Mutex.unlock();
	if (!isRunning())
		start();
}

void LoadLayoutThumbnailThread::stop()
{
	if (IsRunning)
	{
		IsRunning = false;
		wait();
	}
}

void LoadLayoutThumbnailThread::removeDocThumbnailLoad(STPhotoLayoutTemplate _Template)
{
	Mutex.lock();
	//Look for it
	bool Found = false;
	int CurrentIndex = 0;
	while (!Found && CurrentIndex < ImagesToLoadList.count())
	{
		Found = ImagesToLoadList[CurrentIndex].first == _Template;
		if (!Found)
			CurrentIndex++;
	}
	if (Found)
		ImagesToLoadList.removeAt(CurrentIndex);
	Mutex.unlock();
}

void LoadLayoutThumbnailThread::clear()
{
	Mutex.lock();
	ImagesToLoadList.clear();
	Mutex.unlock();
}

void LoadLayoutThumbnailThread::run()
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
			LayoutThumbnailLoadedEvent* NewEvent = new LayoutThumbnailLoadedEvent(DocAndIndex.first, DocAndIndex.second, &WaitCondition);
			QApplication::postEvent(parent(),NewEvent);
			WaitCondition.wait(&Mutex, 200);
			Mutex.unlock();
		}
	}
}

//_____________________________________________________________________________
//
// Class STTemplateModel
//_____________________________________________________________________________

void STTemplateModel::cancelPendingOps()
{
	CreateThumbsCanceled = true;
	if (CreateThumbsFuture.isRunning())
		CreateThumbsFuture.waitForFinished();
}

void STTemplateModel::createThumbnails()
{
	CreateThumbsCanceled = false;
	//TemplateThumbnails.clear();

	int Vfor = 0;
	while (!CreateThumbsCanceled && Vfor < rowCount())
	{
		QModelIndex CIndex = index(Vfor, 0);

		STPhotoLayoutTemplate CurrentTemplate = indexTemplate(CIndex);
		//TemplateThumbnails.insert(Vfor, createSampleThumbnail(CurrentTemplate));

		if (!CurrentTemplate.hasClipartFrames())
		{
			TemplateThumbnails[Vfor] = createSampleThumbnail(indexTemplate(CIndex));
			emit dataChanged(CIndex, CIndex);
		}
		Vfor++;
	}
}

QImage STTemplateModel::createSampleThumbnail(const STPhotoLayoutTemplate _Template) const
{
	QImage Res = _Template.thumbnailImage();
	if (Res.isNull())
	{
		STTemplateScene TemplScene; 
		TemplScene.setAutoAdjustFrames(false);
		TemplScene.loadPageTemplate(_Template);
		TemplScene.setDummyImages(DummyImages);
		TemplScene.prepareForPrint();
		TemplScene.setWarningsVisible(false);
		
		QSize TMaxSize = ThumbnailMaxSize;
		if (_Template.description().isEmpty())
			TMaxSize = TMaxSize + QSize(40, 40);
		if (TemplScene.width() > TemplScene.height() && TMaxSize.width() < TMaxSize.height() ||
			TemplScene.height() > TemplScene.width() && TMaxSize.height() < TMaxSize.width())
			TMaxSize.transpose();
		QSize ThumbSize(TMaxSize.width(), (_Template.size().height() * TMaxSize.width()) / _Template.size().width());
		QImage ResImg(ThumbSize, QImage::Format_RGB32);
		ResImg.fill(0);
		QPainter Painter(&ResImg);
		TemplScene.render(&Painter, ResImg.rect());
		Res = ResImg; 
	}
	return Res; 
}

STTemplateModel::STTemplateModel(QObject* _Parent) : QAbstractListModel(_Parent)
{
	ThumbnailMaxSize = QSize(152,102);
	MLoadThread = new LoadLayoutThumbnailThread(this);
}

int STTemplateModel::rowCount(const QModelIndex& /*_Parent*/) const
{
	return Templates.size();
}

QVariant STTemplateModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < Templates.size())
		{
			if (_Role == Qt::DisplayRole)
			{
				STPhotoLayoutTemplate CurrTemplate = indexTemplate(_Index);
				Res = CurrTemplate.description(); 
			}
			else
			if (_Role == Qt::DecorationRole)
			{
				if (TemplateThumbnails.count() > _Index.row()) //Defensive
				{
					if (!TemplateThumbnails[_Index.row()].isNull())
						Res = QPixmap::fromImage(TemplateThumbnails[_Index.row()]);
					else
					{
						STPhotoLayoutTemplate CurrTemplate = indexTemplate(_Index);
						if (CurrTemplate.hasClipartFrames())
							MLoadThread->loadDocThumbnail(CurrTemplate, _Index.row());
					}
				}
				if (Res.isNull())
					Res = NoImagePixmap;
			}
			else
			if (_Role == Qt::FontRole)
				Res = displayFont();
			else
			if (_Role == Qt::ToolTipRole)
			{
				STPhotoLayoutTemplate CurrTemplate = indexTemplate(_Index);
				Res = CurrTemplate.description();
			}
		}
	}
	return Res;
}

QFont STTemplateModel::displayFont() const
{
	return QFont("Arial", 10);
}

QVariant STTemplateModel::headerData(int /*_Section*/, Qt::Orientation /*_Orientation*/, int /*_Role*/) const
{
	QVariant Res;
	return Res;
}


void STTemplateModel::setLayout(const STPhotoLayout& _Layout)
{
	setTemplateList(_Layout.templates());
}

void STTemplateModel::setTemplateList(const STPhotoLayout::TTemplateList& _TemplateList)
{
	cancelPendingOps();
	Templates = _TemplateList;
	qSort(Templates.begin(), Templates.end());

	TemplateThumbnails.clear();
	for(int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		TemplateThumbnails.push_back(QImage());
	}
	CreateThumbsFuture = QtConcurrent::run(this, &STTemplateModel::createThumbnails);
	//STTemplateModel::createThumbnails();
	reset();
}

void STTemplateModel::setAlbum(const STPhotoBookTemplate& _Album)
{
	setTemplateList(_Album.templates());
}

void STTemplateModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STTemplateModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
} 

STPhotoLayoutTemplate STTemplateModel::indexTemplate(const QModelIndex& _Index) const
{
	return Templates[_Index.row()];
}

bool STTemplateModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
 	for (int Vfor = 0; Vfor < _Count; Vfor++)
	{
		TemplateThumbnails.removeAt(_Row + Vfor);
		Templates.removeAt(_Row + Vfor);
	}
	endRemoveRows();
	return true;
}

void STTemplateModel::addTemplate(const STPhotoLayoutTemplate& _Template)
{
	beginInsertRows(QModelIndex(), Templates.size(), Templates.size()); 
		TemplateThumbnails.insert(Templates.size(), createSampleThumbnail(_Template));
		Templates.insert(Templates.size(), _Template);
	endInsertRows(); 
}

void STTemplateModel::addTemplateList(const STPhotoLayout::TTemplateList& _TemplateList)
{
	STPhotoLayout::TTemplateList::const_iterator it; 
	for (it = _TemplateList.begin(); it != _TemplateList.end(); ++it)
	{
		addTemplate(*it); 
	}
}

bool STTemplateModel::event(QEvent* _Event)
{

	if (_Event->type() == LayoutThumbnailLoadedEvent::LayoutThumbnailLoadedEventType)
	{
		LayoutThumbnailLoadedEvent* CEvent = static_cast<LayoutThumbnailLoadedEvent*>(_Event);
/*		QModelIndex ChandedIndex = CEvent->index();
		CEvent->doc().cacheThumbnail(CEvent->thumbnail());
		CEvent->wakeUp();*/
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		TemplateThumbnails[CEvent->index()] = createSampleThumbnail(CEvent->templateLayout());
		CEvent->wakeUp();
		emit dataChanged(index(CEvent->index(), 0), index(CEvent->index(), 0));
		QApplication::restoreOverrideCursor();
	}
	return QAbstractListModel::event(_Event);
}
