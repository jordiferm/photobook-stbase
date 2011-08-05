/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STFOLDERIMAGEVIEW_H
#define STFOLDERIMAGEVIEW_H

#include <QWidget>
#include <QFileInfoList>
#include <QDir>
#include "storomelexport.h"

class QVBoxLayout;
class QToolBar;
class OBigImagePreviewWidget;
class QScrollArea;
class ST_OROMEL_EXPORT STFolderImageView : public QWidget
{
Q_OBJECT

	bool ShowEmptyFolders;
	QVBoxLayout* ViewLayout;
	QWidget* ViewsWidget;
	QDir CurrentDir;
	QToolBar* TBTopActions;
	bool CurrentDirIsNull;
	OBigImagePreviewWidget* IPWidget;
	QScrollArea* ScrArea;

public:
	STFolderImageView(QWidget *parent = 0);
	void showBigImagePreview(bool _Show);
	void setStoredDir();
	void setSaveDir();
	void storeCurrentDir();
	void storeDir(const QString& _Dir);
	void setShowEmptyFolders(bool _Value) { ShowEmptyFolders = _Value; }
	bool showEmptyFolders() const { return ShowEmptyFolders; }
	void clear();
	void addDirs(const QFileInfoList& _Files);
	void addDir(const QDir& _Dir, bool _HeaderMode = false);
	QDir currentDir() const { return CurrentDir; }
	bool isEmpty() const;
	QFileInfoList selectedFiles() const;

signals:
	void openDir(const QDir& _Dir);
	void editImage(const QString& _ImageFileName);
	void addImage(const QFileInfo& _Image);
	void removeImage(const QFileInfo& _Image);

public slots:	
	void setDir(const QDir& _Dir);
	void slotCDUpAction();
	void slotShowBigPreview(const QDir& _Dir);
	void slotHideBigPreview();
};

#endif // STFOLDERIMAGEVIEW_H
