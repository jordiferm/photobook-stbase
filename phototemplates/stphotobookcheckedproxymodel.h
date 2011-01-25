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
#ifndef STPHOTOBOOKCHECKEDPROXYMODEL_H
#define STPHOTOBOOKCHECKEDPROXYMODEL_H

#include "stcheckedproxymodel.h"
#include <QFuture>
#include "stphototemplatesexport.h"
#include "ddocmodel.h"

/**
Model to manage images that appears in photobooks. And performs drag and drop.

	@author
*/
class STPhotoBook; 
class OPhotoCollectionImageModel; 
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookCheckedProxyModel : public STCheckedProxyModel
{
	Q_OBJECT

	const STPhotoBook* PhotoBook;
	bool ReportDropsMd5;
	QFuture<void> UpdateCheckedFuture;
	bool ChecksPending;
	bool UpdateCheckCanceled;
	void cancelPendingOps();
	void intUpdateCheckedFiles();

public:
	STPhotoBookCheckedProxyModel( QObject* _Parent, const QImage& _CheckedImage );
	~STPhotoBookCheckedProxyModel();
	void setPhotoBook(const STPhotoBook* _PhotoBook);
	void setSourceModel(STDom::DDocModel* _SourceModel);
	STDom::DDocModel* sourceModel() const;
	void updateMD5SumImageMatches(const QString& _MD5Sum, bool _Added);
	//! emits imageDropped and calc md5 for each drop.
	void setReportDropsMd5(bool _Value) { ReportDropsMd5 = _Value; }
	//Drag & Drop.
//	Qt::DropActions supportedDropActions() const;
//	Qt::ItemFlags flags(const QModelIndex & index) const;
//	QStringList mimeTypes() const;
//	QMimeData* mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
	void calcNumMatches(const QModelIndex& _Index);

public slots:
	void updateCheckedFiles();
	void slotCheckImage(const QString&, const QString&);
	void slotUncheckImage(const QString&, const QString&);
	void slotRowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );

signals:
	void imageDropped(const QString& _FileName, const QString& _Md5);
	void imageRemoved(const QString& _FileName, const QString& _Md5);
};

#endif
