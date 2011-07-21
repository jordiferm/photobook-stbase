/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "designinfomodel.h"

using namespace SPhotoBook;

DesignInfoModel::DesignInfoModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int DesignInfoModel::rowCount(const QModelIndex& _Parent) const
{
	return DIList.size();
}

QVariant DesignInfoModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	 if (_Index.isValid() && _Index.row() < rowCount())
	 {
			 DesignInfo DInfo = DIList[_Index.row()];
			 if (_Role == Qt::DisplayRole)
			 {
					 Res = DInfo.name();
			 }
			 /*else
			 if (_Role == Qt::DecorationRole)
			 {
					 Res = TInfo.icon();
			 }*/
	 }
	 return Res;
}

DesignInfo DesignInfoModel::designInfo(const QModelIndex& _Index) const
{
	DesignInfo Res;
	if (_Index.row() >= 0 && _Index.row() < DIList.size())
		Res = DIList[_Index.row()];
	return Res;
}

void DesignInfoModel::setDesignInfoList(const DesignInfoList& _List)
{
	DIList = _List;
}

