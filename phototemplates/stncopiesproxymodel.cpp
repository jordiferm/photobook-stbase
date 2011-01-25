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
#include "stncopiesproxymodel.h"
#include <QPixmap> 
#include <QPainter> 

STNCopiesProxyModel::STNCopiesProxyModel(QObject* _Parent, const QImage& _CheckedImage): STCheckedProxyModel(_Parent, _CheckedImage)
{
}

STNCopiesProxyModel::~STNCopiesProxyModel()
{
}

QVariant STNCopiesProxyModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (_Role == Qt::DecorationRole)
	{
		QPixmap RPixmap = STCheckedProxyModel::data(_Index, _Role).value<QPixmap>();
		int NCopies = nCopies(_Index); 
		if ( NCopies > 1 )
		{
			QPainter Painter(&RPixmap); 
			QFont Font("Arial Black", 20); 	
			Painter.setFont(Font); 
			QFontMetrics FMetrics = Painter.fontMetrics(); 
			QString NumCopiesStr = QString::number(NCopies);
			QRect NCopiesRect(CheckedImage.width(), 0, FMetrics.width(NumCopiesStr) + 4, FMetrics.ascent() + 4);
			
			Painter.fillRect(NCopiesRect, QBrush(Qt::white)); 
			Painter.drawText(CheckedImage.width() + 2, FMetrics.ascent() - 2 , NumCopiesStr); 
		}
		Res = RPixmap; 
	}
	else 
		Res = STCheckedProxyModel::data(_Index, _Role);

	return Res;
}

void STNCopiesProxyModel::setNCopies(const QModelIndex& _Index, int _NCopies)
{
	STCheckedProxyModel::setChecked(_Index, _NCopies > 0); 
	if (_NCopies > 0)
		CopiesMap.insert(_Index, _NCopies); 
	else 
		CopiesMap.remove(_Index); 
	emit dataChanged(_Index, _Index); 
}

int STNCopiesProxyModel::nCopies(const QModelIndex& _Index) const
{
	int Res = 0; 
	if (isChecked(_Index))
		Res = 1; 
	if (CopiesMap.contains(_Index))
		Res = CopiesMap[_Index]; 
	return Res; 
}

void STNCopiesProxyModel::setChecked(const QModelIndex& _Index, bool _Checked)
{
	if (!_Checked)
		setNCopies(_Index, 0); 
	else 
		STCheckedProxyModel::setChecked(_Index, _Checked); 
}

void STNCopiesProxyModel::incCheckedCopies()
{
	for (int Vfor = 0; Vfor < rowCount(); Vfor++)
	{
		QModelIndex CIndex = index(Vfor, 0);
		if (isChecked(CIndex))
			incNCopies(CIndex); 
	}
}


void STNCopiesProxyModel::incNCopies(const QModelIndex& _Index)
{
	setNCopies(_Index, nCopies(_Index) +1); 
}

void STNCopiesProxyModel::decNCopies(const QModelIndex& _Index)
{
	setNCopies(_Index, qMax(nCopies(_Index) -1, 0)); 
}

