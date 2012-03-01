/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STFOLDERIMAGESINGLEVIEW_H
#define STFOLDERIMAGESINGLEVIEW_H

#include <QWidget>
#include <QDir>
#include <QFileInfoList>

#include "ddocmodel.h"
#include "storomelexport.h"
#include "stimage.h"

class QToolBar;
class QSortFilterProxyModel;
class STThumbnailView;
class QLabel;
class ST_OROMEL_EXPORT STFolderImageSingleView : public QWidget
{
Q_OBJECT

	QToolBar* MToolBar;
	QToolBar* ImageActionsToolBar;
	QSortFilterProxyModel* MProxModel;
	STDom::DDocModel* DocModel;
	STThumbnailView* LView;
	QAction* OpenFolderAction;
	QDir MDir;
	int DesiredWidth;
	QLabel* HeaderLabel;
	bool HeaderMode;

	void setupToolBarActions();
	void setEmptyMode(bool _Value);
	void setupImageActions();
	void transformImage(STImage::EnTransformCodes _Transform, const QModelIndex& _Index);

public:
	explicit STFolderImageSingleView(QWidget *parent = 0, int _DesiredWidth = 0, bool _HeaderMode = false);
	void setLabel(const QString& _Label);
	void updateGeometries();
	void setDir(const QDir& _Dir);
	QFileInfoList selectedFiles() const;

signals:
	void openDir(const QDir& _Dir);
	void editImage(const QString& _ImageFileName);
	void bigPreviewDir(const QDir& _Dir);

public slots:
	void openFolderAction();
	void slotBigPreviewFolder();
	void slotThumbnailSizeChanged(int _Value);
	void slotEditIndex(const QModelIndex& _Index);
	void slotRotateRightIndex(const QModelIndex& _Index);
	void slotRotateLeftIndex(const QModelIndex& _Index);
};

#endif // STFOLDERIMAGESINGLEVIEW_H
