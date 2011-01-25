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
#include "stphotolayout.h"
#include "stphototemplatesexport.h"

/**
Model for album templates.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STTemplateModel : public QAbstractListModel
{
private:
	QSize ThumbnailMaxSize;
	STPhotoLayout::TTemplateList Templates;
	QList<QImage>	TemplateThumbnails; 
	QList<QImage> DummyImages; 

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
	STPhotoLayout::TTemplateList templateList() const { return Templates; }
	void setDummyImages(const QList<QImage>& _ImageList) { DummyImages = _ImageList; }
};

#endif
