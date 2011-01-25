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

#ifndef STPHOTOBOOKFOLDERDOCSMODEL_H
#define STPHOTOBOOKFOLDERDOCSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFileInfoList>
#include "stphototemplatesexport.h"

class STPhotoBookTemplate;
class QString;
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookFolderDocsModel : public QAbstractListModel
{
	QStringList FolderList;

public:
	STPhotoBookFolderDocsModel(QObject* _Parent = 0);
	virtual void setPhotoBookTemplate(const STPhotoBookTemplate& _Template) = 0;
	virtual QStringList folderList() { return FolderList; }
	virtual QStringList fileFilter() const = 0;
	virtual QString filePath(const QModelIndex& _Index) const = 0;
	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
	virtual void reload() = 0;

protected:
	QFileInfoList loadPathRecursive(const QString& _PathName, const QStringList& _Filter, QStringList& _Directories);
	virtual QFileInfoList loadPath(const QString& _PathName);
};

#endif // STPHOTOBOOKFOLDERDOCSMODEL_H
