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
#ifndef STFOLDERSELECTOR_H
#define STFOLDERSELECTOR_H

#include <QDialog>
#include <QFileIconProvider> 
#include <QFileSystemModel>
#include "stwidgetsexport.h"


class STFSIconProvider : public QFileIconProvider
{
public: 
	STFSIconProvider(); 
	virtual QIcon icon ( IconType type ) const;
	virtual QIcon icon ( const QFileInfo & info ) const;
};

class STFSDirModel : public QFileSystemModel
{
public: 
	STFSDirModel(QObject* _Parent = 0); 
	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
};

/**
Dialog class to select folders on disk.

	@author
*/
class QTreeView; 
class ST_WIDGETS_EXPORT STFolderSelector : public QDialog
{
	Q_OBJECT
	QTreeView* TView; 
	STFSDirModel* DirModel;
	STFSIconProvider* IconProvider;

public:
	STFolderSelector(QWidget* parent, Qt::WindowFlags f = 0);
	~STFolderSelector();
	virtual QSize sizeHint () const;
	QString selectedDir() const;
	void setDirectory(const QString& _Dir);

	static QString getExistingDirectory(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString());
};

#endif
