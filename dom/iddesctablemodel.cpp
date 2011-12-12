/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "iddesctablemodel.h"

using namespace STDom;
IdDescTableModel::IdDescTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}


int IdDescTableModel::rowCount(const QModelIndex& _Parent) const
{
	return PairList.size();
}

int IdDescTableModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant IdDescTableModel::headerData(int _Section, Qt::Orientation _Orientation, int _Role) const
{
	QVariant Res;
	if (_Orientation == Qt::Horizontal && _Role == Qt::DisplayRole)
	{
		switch (_Section)
		{
			case ColId :
				Res = tr("Id");
			break;
			case ColDescription :
				Res = tr("Description");
			break ;
		}
	}
	return Res;
}

QVariant IdDescTableModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	 if (_Index.isValid() && _Index.row() < rowCount())
	 {
			 TKeyValue Pair = PairList[_Index.row()];
			 if (_Role == Qt::DisplayRole)
			 {
				 switch (_Index.column())
				 {
					case ColId :
						Res = Pair.first;
					break;
					case ColDescription :
						Res = Pair.second;
					break;
				 }

			 }
	 }
	 return Res;
}

void IdDescTableModel::addItem(const QString& _Id, const QString& _Description)
{
	PairList.push_back(TKeyValue(_Id, _Description));
	reset();
}

void IdDescTableModel::setValues(const IdDescTableModel::TKeyValueList& _Values)
{
	PairList = _Values;
	reset();
}


void IdDescTableModel::clear()
{
	PairList.clear();
}

IdDescTableModel::TKey IdDescTableModel::key(const QModelIndex& _Index)
{
	IdDescTableModel::TKey Res;
	if (_Index.isValid() && _Index.row() >= 0 && _Index.row() < rowCount())
	{
		Res = PairList.value(_Index.row()).first;
	}

	return Res;
}
