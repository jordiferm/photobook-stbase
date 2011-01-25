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
#include "stcheckedproxymodel.h"
#include <QPixmap> 
#include <QPainter> 

STCheckedProxyModel::STCheckedProxyModel(QObject* _Parent, const QImage& _CheckedImage)
 : QSortFilterProxyModel(_Parent), CheckedImage(_CheckedImage)
{
	ThumbnailSize = QSize(60, 60);
}

QVariant STCheckedProxyModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (_Role == Qt::DecorationRole)
	{
		QPixmap RPixmap = QSortFilterProxyModel::data(_Index, _Role).value<QPixmap>();
		if (!RPixmap.isNull())
			RPixmap = RPixmap.scaled(ThumbnailSize, Qt::KeepAspectRatio);
		if (isChecked(_Index) && !CheckedImage.isNull())
		{
			QPainter Painter(&RPixmap); 
			QFont Font("Arial", 12);
			Font.setBold(true);
			Painter.setFont(Font);
			Painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
			Painter.drawEllipse(0, 0, 16, 16);
			Painter.setPen(QPen(Qt::white));
			Painter.drawText(5, 12, QString::number(CheckedImages[_Index]));
			Painter.drawImage(0, 0, CheckedImage);
		}
		Res = RPixmap; 
	}
	else 
	if (_Role == Qt::SizeHintRole)
		Res = QSize(ThumbnailSize.width() + 4, ThumbnailSize.height() + 4); 	
	else
	if (_Role == Qt::ToolTipRole)
		Res = sourceModel()->data(mapToSource(_Index), _Role);
	else 
		Res = QSortFilterProxyModel::data(_Index, _Role);


	return Res;
}

void STCheckedProxyModel::setChecked(const QModelIndex& _Index, bool _Checked)
{
	if (_Checked)
	{
		if (!CheckedImages.contains(_Index))
		{
			CheckedImages[_Index] = 1;
			emit dataChanged(_Index, _Index); 
			emit checksChanged();
		}
	}
	else 
	{
		if (CheckedImages.contains(_Index))
		{
			CheckedImages.remove(_Index);
			emit dataChanged(_Index, _Index); 
			emit checksChanged();
		}
	}	
}

void STCheckedProxyModel::setNumMatches(const QModelIndex& _Index, int _NumMatches)
{
	bool Changed = false;
	if (CheckedImages.contains(_Index))
	{
		Changed = CheckedImages[_Index] == _NumMatches;
	}
	else Changed = (_NumMatches > 0);

	if (_NumMatches > 0)
		CheckedImages[_Index] = _NumMatches;
	else
		CheckedImages.remove(_Index);


	if (Changed)
	{
		emit dataChanged(_Index, _Index);
		emit checksChanged();
	}
}

int STCheckedProxyModel::numMatches(const QModelIndex& _Index) const
{
	int Res = 0;
	if (CheckedImages.contains(_Index))
		Res = CheckedImages[_Index];
	return Res;
}

void STCheckedProxyModel::incNumMatches(const QModelIndex& _Index)
{
	setNumMatches(_Index, numMatches(_Index) + 1);
}

void STCheckedProxyModel::decNumMatches(const QModelIndex& _Index)
{
	setNumMatches(_Index, numMatches(_Index) - 1);
}

void STCheckedProxyModel::setSourceModel(QAbstractItemModel* _SourceModel) 
{
	clearChecked();
 	QSortFilterProxyModel::setSourceModel(_SourceModel); 
	connect(_SourceModel, SIGNAL(modelReset()), this, SLOT(clearChecked()));
}

bool STCheckedProxyModel::isChecked(const QModelIndex& _Index) const
{
	return CheckedImages.contains(_Index); 
}

int STCheckedProxyModel::numCheckedImages() const
{
	return CheckedImages.size(); 
}

void STCheckedProxyModel::clearChecked()
{
 	CheckedImages.clear();
	reset();
	emit checksChanged();
}

void STCheckedProxyModel::checkAll()
{
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		QModelIndex CurrIndex = index(Vfor, 0);
		setChecked(CurrIndex, true);
	}
}

void STCheckedProxyModel::invertChecked()
{
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		QModelIndex CurrIndex = index(Vfor, 0);
		setChecked(CurrIndex, !isChecked(CurrIndex));
	}
}
