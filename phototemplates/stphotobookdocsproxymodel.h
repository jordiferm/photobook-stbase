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

#ifndef STPHOTOBOOKDOCSPROXYMODEL_H
#define STPHOTOBOOKDOCSPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "stphototemplatesexport.h"

class STPhotoBookFolderDocsModel;
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookDocsProxyModel : public QSortFilterProxyModel
{
	QString FolderFilter;
	STPhotoBookFolderDocsModel* SourceModel;

public:
	STPhotoBookDocsProxyModel(QObject* parent);
	//SourceModel
	void setSourceModel(STPhotoBookFolderDocsModel* _Model);
	STPhotoBookFolderDocsModel* sourceModel() const { return SourceModel; }

	//Filtering
	void clearFolderFilter();
	QString folderFilter() const { return FolderFilter; }
	void setFolderFilter(const QString& _Folder);
	virtual bool filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const;
	//Drag And Drop
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;


};

#endif // STPHOTOBOOKDOCSPROXYMODEL_H
