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
#ifndef DISKSOURCESELECTIONWIDGET_H
#define DISKSOURCESELECTIONWIDGET_H

#include <QWidget> 
#include <QFileInfoList>
#include "stwizardsexport.h"


/**
To select sources from disk.

	@author
*/
class QDirModel; 
class QTreeView; 
class QModelIndex; 
class QMenu; 
class ST_WIZARDS_EXPORT DiskSourceSelectionWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath USER true)

	QDirModel* Model;
	QTreeView* View;
	QMenu* ContextMenu; 

	void createContextMenu();

public:
	DiskSourceSelectionWidget(QWidget* _Parent = 0);
	void setCurrentPath(const QString& _DirPath);
	QString currentPath() const;
	QMenu* contextMenu() const { return ContextMenu; } 

protected:
	void contextMenuEvent(QContextMenuEvent* _Event);


private slots: 
	void nodeClicked(const QModelIndex& );
	void findMedia();
	void slideShow();


signals:
	void imagesSelected(const QFileInfoList& _Images);
	void pathClicked(const QString& ); 
	void selectAllPictures();
	void clearSelection();
	void invertSelection();
	void startSlideShow();

};

#endif 
