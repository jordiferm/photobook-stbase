/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "resourcelist.h"
#include <QImage>
#include <QFile>
#include <QSvgRenderer>
#include <QPainter>

using namespace SPhotoBook;

ResourceList::ResourceList()
{
	ThumbnailMaxSize = QSize(100, 100);
}

ResourceList ResourceList::subList(Resource::EnResourceType _Type) const
{
	ResourceList Res;
	ResourceList::const_iterator it = begin();
	while (it != end())
	{
		if (it->type() == _Type)
			Res.push_back(*it);

		++it;
	}
}

void ResourceList::load(const QDir& _Dir)
{
	QFileInfoList PhotoFiles = _Dir.entryInfoList(QDir::Files, QDir::Time & QDir::Reversed);
	QFileInfoList::iterator it = PhotoFiles.begin();

	while (it != PhotoFiles.end())
	{
		if (Resource::isResource(*it))
			push_back(Resource(*it));
		++it;
	}

	LoadedDir = _Dir;
}

void ResourceList::save(const QDir& _Dir)
{
	ResourceList::iterator it;
	for (it = begin(); it != end(); ++it)
	{
		QFile SourceFile(it->fileInfo().absoluteFilePath());
		if (SourceFile.exists())
		{
			QString DestFileName = _Dir.absoluteFilePath(it->fileInfo().fileName());
			if (!QFile::exists(DestFileName))
				Assert(SourceFile.copy(DestFileName), Error(QObject::tr("Error saving resource file: %1 -> %2").arg(SourceFile.fileName()).arg(DestFileName)));
		}
	}
}

Resource ResourceList::resource(Resource::EnResourceType _Type, const QString& _Name) const
{
	Resource Res;
	bool Found = false;
	ResourceList::const_iterator it = begin();
	while (it != end() && !Found)
	{
		Found = it->type() == _Type && it->name() == _Name;
		if (Found)
			Res = *it;
	}
	return Res;
}


QString ResourceList::thumbnailAbsoluteFileName(const Resource& _Resource) const
{
	return _Resource.fileInfo().absoluteFilePath() + ".thumb";
}


QImage ResourceList::getThumbNail(const Resource& _Resource) const
{
	QString ThumbFileName = thumbnailAbsoluteFileName(_Resource);
	if (!QFile::exists(ThumbFileName))
	{
		//Let's generate a resource thumbnail
		if (_Resource.type() == Resource::TypeClipart)
		{
			QSvgRenderer Renderer;
			if (Renderer.load(_Resource.fileInfo().absoluteFilePath()))
			{
				QImage Img(ThumbnailMaxSize, QImage::Format_RGB32);

				QPainter Painter(&Img);
				Painter.fillRect(0, 0, Img.width(), Img.height(), QBrush(Qt::white));
				Renderer.render(&Painter, QRectF(0, 0, Img.width(), Img.height()));
				if (!Img.save(ThumbFileName))
					qWarning(QString("Error saving thumbnail file %1").arg(ThumbFileName).toLatin1());
			}
			else
				qWarning(QString("Error creating thumbnail for file %1").arg(ThumbFileName).toLatin1());

		}
		else if (_Resource.type() == Resource::TypeBackground || _Resource.type() == Resource::TypeFrame)
		{
			QImage Img(_Resource.fileInfo().absoluteFilePath());
			Img = Img.scaled(ThumbnailMaxSize);
			if (!Img.save(ThumbFileName, "PNG"))
				qWarning(QString("Error saving thumbnail file %1").arg(ThumbFileName).toLatin1());
		}
	}
	QImage ColPixmap(ThumbFileName);
	return ColPixmap.scaledToWidth(ThumbnailMaxSize.width());
}

void ResourceList::push_backResource(const QFileInfo& _ResourceFile)
{
	QFile SourceFile(_ResourceFile.absoluteFilePath());
	if (SourceFile.exists())
	{
		if (Resource::isResource(_ResourceFile))
		{
			Resource NewRes(_ResourceFile);
			QString DestFileName = NewRes.fileInfo().absoluteFilePath();
			if (!QFile::exists(DestFileName))
			{
				Assert(SourceFile.copy(DestFileName), Error(QObject::tr("Error saving resource file: %1 -> %2").arg(SourceFile.fileName()).arg(DestFileName)));
				push_back(NewRes);
			}
		}
	}
}
