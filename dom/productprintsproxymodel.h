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
#ifndef PRODUCTPRINTSPROXYMODEL_H
#define PRODUCTPRINTSPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QMap> 
#include <QSize> 
#include "stdomexport.h"

namespace STDom
{

/**
Proxy model to store product prints.

	@author 
*/
class OPhotoCollectionImageModel;
class OPhotoCollectionImage; 
class ST_DOM_EXPORT ProductPrintsProxyModel : public QSortFilterProxyModel
{
public:
	typedef QMap<int, KProductList> TProductPrints;

	enum EnUserRoles
	{
		FilePathRole = Qt::UserRole + 1,
		HasPhotosRole
	};

private: 
	TProductPrints ProductPrints; 
	QSize ThumbnailSize;
	OPhotoCollectionImageModel* SourceImageModel;
	QString FolderFilter;
	bool RitchTextDisplay; 

public:
	ProductPrintsProxyModel(QObject* parent);
	QVariant data(const QModelIndex& _Index, int _Role) const;
	void setThumbnailSize(const QSize& _Size) { ThumbnailSize = _Size; }
	void setProductCopies(const QModelIndex& _Index, const KPhotoProduct& _Product, int _Copies);
	void clearProductCopies(); 
	void setSourceModel(OPhotoCollectionImageModel* _SourceModel);
	void setRitchTextDisplay(bool _Value) { RitchTextDisplay = _Value; }
	bool ritchTextDisplay() const { return RitchTextDisplay; }

	KProductList productCopies(const QModelIndex& _Index) const;
	KProductPrintList productPrintList() const; 
	OPhotoCollectionImage image(const QModelIndex& _Index) const; 

	OPhotoCollectionImageModel* sourceImageModel() const { return SourceImageModel; }
	//! \return A list of most restrictive format for each image in the model according to selected products. (Only for images with copies).
	QList<QRect> referenceFormatList(int _DPIS) const; 
	QFileInfoList imagesWithCopies() const; 
	//! \return true if we have some copies of some product.
	int imagesWithCopiesCount() const;
	bool hasCopies() const;
	bool hasCopies(int _Row) const;
	void incProductCopies(const QModelIndex& _Index, int _Quantity, const KPhotoProduct& _Product);
	void incProductCopiesAll(int _Quantity, const KPhotoProduct& _Product);
	void cloneImage(const QModelIndex& _Index); 
	//! \return size of all files in disk.
	qint64 diskSize() const; 

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

};
}//Namespace.

#endif
