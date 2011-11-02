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
#include <QPixmap>

using namespace SPhotoBook;
TemplateInfoModel::TemplateInfoModel(QObject *parent) :
    QAbstractListModel(parent)
{
}


int TemplateInfoModel::rowCount(const QModelIndex& _Parent ) const
{
	return TemplateNameList.size();
}


QVariant TemplateInfoModel::data(const QModelIndex& _Index, int _Role ) const
{
	QVariant Res;
	 if (_Index.isValid() && _Index.row() < rowCount())
	 {
		 if (_Role == Qt::DisplayRole)
		 {
			Res = TemplateNameList[_Index.row()];
		 }
		 else
		 if (_Role == Qt::DecorationRole)
		 {
				 Res = MetaInfo::typePixmap(TemplateType).scaled(QSize(32, 32));
		 }
	 }
	 return Res;

}

TemplateInfo TemplateInfoModel::templateInfo(const QModelIndex& _Index, const QSizeF& _Size) const
{
	TemplateInfo Res;
	if (_Index.row() >= 0 && _Index.row() < rowCount())
	{
		QString CName = data(_Index).toString();
		bool Found = false;
		TemplateInfoList::const_iterator it = TemplateList.begin();
		while (it != TemplateList.end() && !Found)
		{
			Found = it->name() == CName && it->size() == _Size;
			if (!Found)
				++it;
		}
		if (Found)
			Res = *it;
	}
	return Res;
}


void TemplateInfoModel::setTemplateList(const TemplateInfoList& _List)
{
	TemplateNameList.clear();
	TemplateList = _List;
	TemplateInfoList::iterator it;
	for (it = TemplateList.begin(); it != TemplateList.end(); ++it)
	{
		if (!TemplateNameList.contains(it->name()))
			TemplateNameList.push_back(it->name());
	}
	reset();
}

QList<QSizeF> TemplateInfoModel::sizes(const QModelIndex& _Index) const
{
	QList<QSizeF> Res;
	if (_Index.isValid() && _Index.row() >= 0 && _Index.row() < rowCount())
	{
		TemplateInfoList TList = TemplateList.sizes(data(_Index).toString());
		TemplateInfoList::const_iterator it;
		for (it = TList.begin(); it != TList.end(); ++it)
		{
			Res.push_back(it->size());
		}
	}
/*	QModelIndexList Res;
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
	}*/
	return Res;
}
