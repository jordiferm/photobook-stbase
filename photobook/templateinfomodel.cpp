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

#include "templateinfomodel.h"

using namespace SPhotoBook;
TemplateInfoModel::TemplateInfoModel(QObject *parent) :
    QAbstractListModel(parent)
{
}


int TemplateInfoModel::rowCount(const QModelIndex& _Parent ) const
{
	return TemplateList.size();
}


QVariant TemplateInfoModel::data(const QModelIndex& _Index, int _Role ) const
{
	QVariant Res;
	 if (_Index.isValid() && _Index.row() < rowCount())
	 {
			 TemplateInfo TInfo = TemplateList[_Index.row()];
			 if (_Role == Qt::DisplayRole)
			 {
					 Res = TInfo.name();
			 }
			 /*else
			 if (_Role == Qt::DecorationRole)
			 {
					 Res = TInfo.icon();
			 }*/
	 }
	 return Res;

}

TemplateInfo TemplateInfoModel::templateInfo(const QModelIndex& _Index) const
{
	TemplateInfo Res;
	if (_Index.row() >= 0 && _Index.row() < TemplateList.size())
		Res = TemplateList[_Index.row()];
	return Res;
}


void TemplateInfoModel::setTemplateList(const TemplateInfoList& _List)
{
	TemplateList = _List;
	reset();
}

QModelIndexList TemplateInfoModel::sizes(const QModelIndex& _Index) const
{
	QModelIndexList Res;
	if (_Index.isValid())
	{
		TemplateInfo SourceTemplate = templateInfo(_Index);
		int Cnt = 0;
		while (Cnt < rowCount())
		{
			QModelIndex CIndex = index(Cnt, 0);
			TemplateInfo CTemplate = templateInfo(CIndex);
			if (CTemplate.name() == SourceTemplate.name() && CTemplate.size() == SourceTemplate.size())
				Res.push_back(CIndex);
			Cnt++;
		}
	}
	return Res;
}
