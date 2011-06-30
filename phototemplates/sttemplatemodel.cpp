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


void STTemplateModel::cancelPendingOps()
{
	CreateThumbsCanceled = true;
	if (CreateThumbsFuture.isRunning())
		CreateThumbsFuture.waitForFinished();
}

void STTemplateModel::createThumbnails()
{
	CreateThumbsCanceled = false;
	TemplateThumbnails.clear();

	int Vfor = 0;
	while (!CreateThumbsCanceled && Vfor < rowCount())
	{
		QModelIndex CIndex = index(Vfor, 0);
		TemplateThumbnails.push_back(createSampleThumbnail(indexTemplate(CIndex)));
		emit dataChanged(CIndex, CIndex);
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
						Res = NoImagePixmap;
				}
				else
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
	CreateThumbsFuture = QtConcurrent::run(this, &STTemplateModel::createThumbnails);
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
 		Templates.removeAt(_Row + Vfor);
		TemplateThumbnails.removeAt(_Row + Vfor); 
	}
	endRemoveRows();
	return true;
}

void STTemplateModel::addTemplate(const STPhotoLayoutTemplate& _Template)
{
	beginInsertRows(QModelIndex(), Templates.size(), Templates.size()); 
		Templates.push_back(_Template); 
		TemplateThumbnails.push_back(createSampleThumbnail(_Template)); 
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
