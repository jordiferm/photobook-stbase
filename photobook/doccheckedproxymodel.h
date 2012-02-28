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
#ifndef DOCCHECKEDPROXYMODEL_H
#define DOCCHECKEDPROXYMODEL_H

#include "checkedproxymodel.h"
#include <QFuture>
#include "stphotobookexport.h"
#include "ddocmodel.h"

/**
Model to manage images that appears in photobooks. And performs drag and drop.

	@author
*/

namespace SPhotoBook
{
class Document;
class ST_PHOTOBOOK_EXPORT DocCheckedProxyModel : public CheckedProxyModel
{
	Q_OBJECT

	const Document* PBDocument;
	bool ReportDropsMd5;
	QFuture<void> UpdateCheckedFuture;
	bool ChecksPending;
	bool UpdateCheckCanceled;
	void cancelPendingOps();
	void intUpdateCheckedFiles();

public:
	DocCheckedProxyModel( QObject* _Parent, const QImage& _CheckedImage );
	~DocCheckedProxyModel();
	void setDocument(const Document* _PBDocument);
	void setSourceModel(STDom::DDocModel* _SourceModel);
	STDom::DDocModel* sourceModel() const;
	QFileInfoList uncheckedImages() const;

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
}
#endif
