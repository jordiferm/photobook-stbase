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

#include "printjobmodel.h"
#include <QPixmap>
#include <QFontMetrics>
#include "ddocmodel.h"

//Drag and Drop
#include <QMimeData>
#include <QUrl>

using namespace STDom;

PrintJobModel::PrintJobModel(QObject* _Parent) : QSortFilterProxyModel(_Parent), FolderFilter(""), RitchTextDisplay(false)
{
	ThumbnailSize = QSize(60, 60);
}

void PrintJobModel::setPrintJob(const PrintJob& _PrintJob)
{
	MPrintJob = _PrintJob;
}

void PrintJobModel::setSourceModel(DDocModel* _SourceModel)
{
	QSortFilterProxyModel::setSourceModel(_SourceModel);
	SourceDocModel = _SourceModel;
	//connect(_SourceModel, SIGNAL(modelReset()), this, SLOT(clearChecked()));
}

bool PrintJobModel::validIndex(const QModelIndex& _Index) const
{
	return (_Index.isValid() && rowCount() > _Index.row() && _Index.row() >= 0);
}

//Data Access
QVariant PrintJobModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (_Index.isValid() && rowCount() > _Index.row())
	{
		if (_Role == Qt::DecorationRole)
		{
			QPixmap RPixmap = QSortFilterProxyModel::data(_Index, _Role).value<QPixmap>();
			if (!RPixmap.isNull())
				RPixmap = RPixmap.scaled(ThumbnailSize, Qt::KeepAspectRatio);
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

			DDoc* CurrDoc = doc(_Index);
			DDocPrintList DocPrints = MPrintJob.prints(CurrDoc->fileInfo());
			DDocPrintList::const_iterator it;
			for (it = DocPrints.begin(); it != DocPrints.end(); ++it)
			{
				if (RitchTextDisplay)
				{
					if (!StrRes.isEmpty())
						StrRes += "<br/>";
					StrRes += "<span style=\"background-color:'#FFF2CA';font-weight:bold;\">" + it->product().description() + " - <span style=\"font-size:17pt;\">" + QString::number(it->numCopies()) + "</span></span>";
				}
				else
				{
					if (!StrRes.isEmpty())
						StrRes += ", ";
					StrRes += it->product().description() + " - " + QString::number(it->numCopies());
				}
			}
			Res = StrRes;
		}
		else
		if (_Role == FilePathRole)
		{
			DDoc* CurrDoc = doc(_Index);
			Res = CurrDoc->fileInfo().absoluteFilePath();
		}
		else
		if (_Role == Qt::SizeHintRole)
		{
			QFontMetrics FMetrics(QFont(data(_Index, Qt::FontRole).value<QFont>()));
			int FontHeight = FMetrics.boundingRect("X").size().height();

			DDoc* CurrDoc = doc(_Index);
			DDocPrintList DocPrints = MPrintJob.prints(CurrDoc->fileInfo());

			if (!DocPrints.isEmpty())
				FontHeight = FontHeight * DocPrints.size();
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


void PrintJobModel::setProductCopies(const QModelIndex& _Index, const DDocProduct& _Product, int _Copies)
{
	if (validIndex(_Index))
	{
		MPrintJob.setCopies(_Product, doc(_Index)->fileInfo(), _Copies);
		dataChanged(_Index, _Index);
	}
}


void PrintJobModel::clearProductCopies()
{
	MPrintJob.clear();
	reset();
}

DDocPrintList PrintJobModel::productCopies(const QModelIndex& _Index) const
{
	DDocPrintList Res;
	if (validIndex(_Index))
		Res = MPrintJob.prints(doc(_Index)->fileInfo());
	return Res;
}

DDoc* PrintJobModel::doc(const QModelIndex& _Index) const
{
	return SourceDocModel->doc(mapToSource(_Index));
}

//! \return A list of most restrictive format for each image in the model according to selected products. (Only for images with copies).
QList<QRect> PrintJobModel::referenceFormatList(int _DPIS) const
{
	QList<QRect> Res;
	DDocFormatList Formats = MPrintJob.formats();
	DDocFormatList::const_iterator it;
	for (it = Formats.begin(); it != Formats.end(); ++it)
	{
		QSize CSize = it->pixelSize(_DPIS);
		Res.push_back(QRect(QPoint(0,0), CSize));
	}
	return Res;
}


int PrintJobModel::imagesWithCopiesCount() const
{
	return MPrintJob.prints().size();
}

	//! \return true if we have some copies of some product.
bool PrintJobModel::hasCopies() const
{
	return !MPrintJob.prints().isEmpty();
}

bool PrintJobModel::hasCopies(int _Row) const
{
	bool Res = false;
	QModelIndex Index = index(_Row, 0);
	if (validIndex(Index))
		Res = !MPrintJob.prints(doc(Index)->fileInfo()).isEmpty();
	return Res;
}

void PrintJobModel::incProductCopies(const QModelIndex& _Index, int _Quantity, const DDocProduct& _Product)
{
	if (validIndex(_Index))
	{
		MPrintJob.addCopies(_Product, doc(_Index)->fileInfo(), _Quantity);
		dataChanged(_Index, _Index);
	}
}

void PrintJobModel::incProductCopiesAll(int _Quantity, const DDocProduct& _Product)
{
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		MPrintJob.addCopies(_Product, doc(index(Vfor, 0))->fileInfo(), _Quantity);
	}
	dataChanged(index(0, 0), index(rowCount() -1, 0));
}

void PrintJobModel::cloneImage(const QModelIndex& _Index)
{
	if (validIndex(_Index) && SourceDocModel)
	{
		DDoc* CurrentDoc = doc(_Index);
		DDoc* NewDoc = new DDoc(*CurrentDoc);
		SourceDocModel->insertDoc(mapToSource(_Index), NewDoc);
	}
}

qint64 PrintJobModel::diskSize() const
{
	qint64 Res = 0;
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		if (hasCopies(Vfor))
		{
			QModelIndex CIndex = index(Vfor, 0);
			Res += doc(CIndex)->size();
		}
	}
	return Res;
}

void PrintJobModel::clearFolderFilter()
{
	FolderFilter = "";
	invalidateFilter();
}

void PrintJobModel::setFolderFilter(const QString& _Folder)
{
	FolderFilter = _Folder;
	invalidateFilter();
}

bool PrintJobModel::filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const
{
	bool Res = true;
	if (!FolderFilter.isEmpty())
	{
		DDoc* CurrentDoc = SourceDocModel->doc(SourceDocModel->index(_SourceRow, 0, _SourceParent));
		Res = CurrentDoc->fileInfo().absoluteFilePath().contains(FolderFilter);
	}
	return Res;
}

Qt::DropActions PrintJobModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags PrintJobModel::flags(const QModelIndex & index) const
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

QStringList PrintJobModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

QMimeData* PrintJobModel::mimeData(const QModelIndexList &indexes) const
{

	QList<QUrl> Urls;
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid() && Index.row() > -1 && Index.row() < rowCount())
		{
			QString text = data(index(Index.row(), 0, Index.parent()), FilePathRole).toString();
			Urls.push_back(QUrl::fromLocalFile(text));
		}
	}
	if (!Urls.isEmpty())
	{
		QMimeData *mimeData = new QMimeData();
		mimeData->setUrls(Urls);
		return mimeData;
	}
	return QSortFilterProxyModel::mimeData(indexes);
}

