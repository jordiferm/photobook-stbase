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
#include "stproductprintsproxymodel.h"
#include <QPixmap>
#include <QFontMetrics>
#include "ophotocollection.h"

//Drag and Drop
#include <QMimeData> 
#include <QUrl>

STProductPrintsProxyModel::STProductPrintsProxyModel(QObject* parent): QSortFilterProxyModel(parent), FolderFilter(""), RitchTextDisplay(false)
{
	ThumbnailSize = QSize(60, 60);
}

QVariant STProductPrintsProxyModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (_Index.isValid() && rowCount() > _Index.row())
	{
		if (_Role == Qt::DecorationRole)
		{
			QPixmap RPixmap = QSortFilterProxyModel::data(_Index, _Role).value<QPixmap>();
			if (!RPixmap.isNull())
				RPixmap = RPixmap.scaled(ThumbnailSize, Qt::KeepAspectRatio);
			/*if (isChecked(_Index) && !CheckedImage.isNull())
			{
				QPainter Painter(&RPixmap); 
				Painter.drawImage(0, 0, CheckedImage); 
			}*/
			Res = RPixmap; 
		}
		else 
		if (_Role == Qt::DisplayRole)
		{
			QString StrRes;
			if (RitchTextDisplay)
				StrRes = "<em style=\"font-size:7pt;\">" + QSortFilterProxyModel::data(_Index, Qt::DisplayRole).toString() + "</em>";
			else 
				StrRes = QSortFilterProxyModel::data(_Index, Qt::DisplayRole).toString();

			if (ProductPrints.contains(_Index.row()))
			{
				KProductList CMap = ProductPrints[_Index.row()]; 
				KProductList::iterator it; 
				for (it = CMap.begin(); it != CMap.end(); ++it)
				{
					if (RitchTextDisplay)
					{
						if (!StrRes.isEmpty())
							StrRes += "<br/>";
						StrRes += "<span style=\"background-color:'#FFF2CA';font-weight:bold;\">" + it.key().description() + " - <span style=\"font-size:17pt;\">" + QString::number(it.value()) + "</span></span>"; 
					}
					else 
					{
						if (!StrRes.isEmpty())
							StrRes += ", ";
						StrRes += it.key().description() + " - " + QString::number(it.value()); 
					}
				}
			}
			Res = StrRes; 
		}
		else 
		if (_Role == FilePathRole)
		{
			Res = QSortFilterProxyModel::data(index(_Index.row(), OPhotoCollectionImageModel::ColFile), Qt::EditRole).toString(); 
		}
		else 
		if (_Role == Qt::SizeHintRole)
		{
			QFontMetrics FMetrics(QFont(data(_Index, Qt::FontRole).value<QFont>()));
			int FontHeight = FMetrics.boundingRect("X").size().height();
			if (ProductPrints.contains(_Index.row()))
				FontHeight = FontHeight * ProductPrints[_Index.row()].size(); 
			Res = QSize(ThumbnailSize.width() + 4, qMax(ThumbnailSize.height() + 4, FontHeight + 4)); 	
		}
		else 
		if (_Role == HasPhotosRole)
		{
			Res = hasCopies(_Index.row()); 
		}
	}
	return Res;
}

void STProductPrintsProxyModel::setProductCopies(const QModelIndex& _Index, const KPhotoProduct& _Product, int _Copies)
{
	
	KProductList CMap; 
	if (ProductPrints.contains(_Index.row()))
		CMap = ProductPrints[_Index.row()];
	CMap.setCopies(_Product, _Copies); 

	if (!CMap.isEmpty())
		ProductPrints.insert(_Index.row(), CMap); 
	else 
		ProductPrints.remove(_Index.row());

	dataChanged(_Index, _Index);
}


void STProductPrintsProxyModel::clearProductCopies()
{
	ProductPrints.clear(); 
}

void STProductPrintsProxyModel::setSourceModel(OPhotoCollectionImageModel* _SourceModel) 
{
 	QSortFilterProxyModel::setSourceModel(_SourceModel); 
	SourceImageModel = _SourceModel; 
	//connect(_SourceModel, SIGNAL(modelReset()), this, SLOT(clearChecked()));
}

KProductList STProductPrintsProxyModel::productCopies(const QModelIndex& _Index) const
{
	KProductList  Res; 
	if (ProductPrints.contains(_Index.row()))
		Res = ProductPrints[_Index.row()];
	return Res;
}

KProductPrintList STProductPrintsProxyModel::productPrintList() const
{
	KProductPrintList Res; 
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		if (hasCopies(Vfor))
		{
			QModelIndex CIndex = index(Vfor, 0); 
			KPhotoPrint CPhotoPrint(sourceImageModel()->image(mapToSource(CIndex)).fileInfo());
			KProductList CImageProdCopiesMap = productCopies(CIndex);
			KProductList::const_iterator it; 
			for (it = CImageProdCopiesMap.begin(); it != CImageProdCopiesMap.end(); ++it)
			{
				Res[it.key()].addCopies(CPhotoPrint, it.value());
			}
		}
	}
	return Res; 
}

OPhotoCollectionImage STProductPrintsProxyModel::image(const QModelIndex& _Index) const
{
	return sourceImageModel()->image(mapToSource(_Index));
}

QList<QRect> STProductPrintsProxyModel::referenceFormatList(int _DPIS) const
{
	QList<QRect> Res; 
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		if (ProductPrints.contains(Vfor))
		{
			QSize CSize = ProductPrints[Vfor].referenceFormat().pixelSize(_DPIS); 
			Res.push_back(QRect(QPoint(0,0), CSize));
		}
	}
	return Res; 
}

QFileInfoList STProductPrintsProxyModel::imagesWithCopies() const
{
	QFileInfoList Res;
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		if (ProductPrints.contains(Vfor))
			Res.push_back(SourceImageModel->image(index(Vfor, 0)).fileInfo());
	}
	return Res; 
}

int STProductPrintsProxyModel::imagesWithCopiesCount() const
{
	return ProductPrints.size(); 
}

bool STProductPrintsProxyModel::hasCopies() const
{
	return !ProductPrints.isEmpty();
}

bool STProductPrintsProxyModel::hasCopies(int _Row) const
{
	return (ProductPrints.contains(_Row));
}

void STProductPrintsProxyModel::incProductCopies(const QModelIndex& _Index, int _Quantity, const KPhotoProduct& _Product)
{
	int CValue = 0; 
	KProductList PCMap = productCopies(_Index);
	if (PCMap.contains(_Product))
		CValue = PCMap[_Product];
	CValue += _Quantity; 
	setProductCopies(_Index, _Product, qMax(CValue, 0)); 
}

void STProductPrintsProxyModel::incProductCopiesAll(int _Quantity, const KPhotoProduct& _Product)
{
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		incProductCopies(index(Vfor, 0), _Quantity, _Product); 
	}	
}

void STProductPrintsProxyModel::cloneImage(const QModelIndex& _Index)
{
	if (_Index.isValid() && SourceImageModel)
	{
		QModelIndex SourceIndex =  mapToSource(_Index);
		SourceImageModel->insertImage(SourceIndex, SourceImageModel->image(SourceIndex));
		//TODO Arreglar els ProductPrints !!
	}
}

qint64 STProductPrintsProxyModel::diskSize() const
{
	qint64 Res = 0; 
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		if (hasCopies(Vfor))
		{
			
			QModelIndex CIndex = index(Vfor, 0); 
			Res += image(CIndex).fileInfo().size(); 
		}
	}
	return Res; 
}


void STProductPrintsProxyModel::clearFolderFilter()
{
	FolderFilter = ""; 
	invalidateFilter(); 
}

void STProductPrintsProxyModel::setFolderFilter(const QString& _Folder)
{
	FolderFilter = _Folder; 
	invalidateFilter(); 
}

bool STProductPrintsProxyModel::filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const
{
	bool Res = true; 
	
	if (!FolderFilter.isEmpty())
	{
		OPhotoCollectionImage CImage = SourceImageModel->image(SourceImageModel->index(_SourceRow, 0, _SourceParent));
		Res = CImage.fileInfo().absoluteFilePath().contains(FolderFilter);
	}

	return Res; 
}


//Drag and Drop
Qt::DropActions STProductPrintsProxyModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags STProductPrintsProxyModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;
	//return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable ;


	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
	{
		return defaultFlags;
	}
}

QStringList STProductPrintsProxyModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

QMimeData* STProductPrintsProxyModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();

	QList<QUrl> Urls; 
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid())
		{
			QString text = data(index(Index.row(), 0, Index.parent()), FilePathRole).toString();
			Urls.push_back(QUrl::fromLocalFile(text)); 
		}
	}
	mimeData->setUrls(Urls); 
	//mimeData->setData("text/uri-list", encodedData);
	return mimeData;
}


