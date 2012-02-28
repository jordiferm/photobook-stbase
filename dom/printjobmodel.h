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

#ifndef PRINTJOBMODEL_H
#define PRINTJOBMODEL_H
#include <QSortFilterProxyModel>
#include "printjob.h"
#include "stdomexport.h"

namespace STDom
{
class DDocModel;
class DDocProduct;
class ST_DOM_EXPORT PrintJobModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	enum EnUserRoles
	{
		FilePathRole = Qt::UserRole + 1,
		HasPhotosRole
	};

private:
	static QImage CutWarningImage;
	static QImage LowResWarningImage;
	static int LowResWarningLimit;
	PrintJob MPrintJob;
	QSize ThumbnailSize;
	DDocModel* SourceDocModel;
	QString FolderFilter;
	bool RitchTextDisplay;

	void drawCut(QPixmap& _Pixmap, const QSize& _CutSize) const;

public:
	PrintJobModel(QObject* parent);

	void setPrintJob(const PrintJob& _PJob);
	PrintJob printJob() const { return MPrintJob; }
	void setSourceModel(DDocModel* _SourceModel);
	DDocModel* sourceDocModel() const { return SourceDocModel; }
	bool validIndex(const QModelIndex& _Index) const;

	//Data Access
	QVariant data(const QModelIndex& _Index, int _Role) const;
	void setProductCopies(const QModelIndex& _Index, const DDocProduct& _Product, int _Copies);
	//! Changes the file for _Index.
	void changeDocFile(const QModelIndex& _Index, const QFileInfo& _File);
	void clearProductCopies();
	DDocPrintList productCopies(const QModelIndex& _Index) const;
	DDoc* doc(const QModelIndex& _Index) const;
	//! \return A list of most restrictive format for each image in the model according to selected products. (Only for images with copies).
	QList<QRect> referenceFormatList(int _DPIS) const;
	//! \return true if we have some copies of some product.
	int imagesWithCopiesCount() const;
	bool hasCopies() const;
	bool hasCopies(int _Row) const;
	void incProductCopies(const QModelIndex& _Index, int _Quantity, const DDocProduct& _Product);
	void incProductCopiesAll(int _Quantity, const DDocProduct& _Product);
	void cloneImage(const QModelIndex& _Index);
	//! \return size of all files in disk.
	qint64 diskSize() const;
	void clearAll();

	//Data config
	void setRitchTextDisplay(bool _Value) { RitchTextDisplay = _Value; }
	bool ritchTextDisplay() const { return RitchTextDisplay; }
	void setThumbnailSize(const QSize& _Size) { ThumbnailSize = _Size; }
	QSize thumbnailSize() const { return ThumbnailSize; }

	//Filtering
	void clearFolderFilter();
	QString folderFilter() const { return FolderFilter; }
	void setFolderFilter(const QString& _Folder);
	virtual bool filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const;

	//Drag and drop
	Qt::DropActions supportedDropActions() const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	QStringList mimeTypes() const;

	//Info Display
	static void setCutWarningImage(const QImage& _Image);
	static void setLowResWarningImage(const QImage& _Image);
	//! 0 (The Default) Means no limit.
	static void setLowResWarningLimit(int _Dpis);

};

}//Namespace
#endif // PRINTJOBMODEL_H
