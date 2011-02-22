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
#include "ocheckeddirmodel.h"
#include <QDebug>

OCheckedDirModel::OCheckedDirModel(QObject* parent): QDirModel(parent)
{
}

OCheckedDirModel::OCheckedDirModel(const QStringList& nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject* parent): QDirModel(nameFilters, filters, sort, parent)
{
}

OCheckedDirModel::~OCheckedDirModel()
{
}

bool OCheckedDirModel::setData(const QModelIndex& _Index, const QVariant& _Value, int _Role)
{
	bool Res; 
	if (_Role == Qt::CheckStateRole )
	{
		QFileInfo CFile = fileInfo(_Index); 
		int CFileIndex = CheckedDirs.indexOf(CFile.absoluteFilePath());
		if (_Value.toInt() == Qt::Checked)
		{
			if (CFileIndex == -1)
				CheckedDirs.push_back(CFile.absoluteFilePath());
		}
		else
		{
			if (CFileIndex != -1)
				CheckedDirs.removeAt(CFileIndex);
		}
		Res = true; 
	}
	else 
		Res = QDirModel::setData(_Index, _Value, _Role);
	return Res;
}

Qt::ItemFlags OCheckedDirModel::flags(const QModelIndex& index) const
{
	return QDirModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant OCheckedDirModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res; 
	if (_Role == Qt::CheckStateRole && _Index.column() == 0)
	{
		if (CheckedDirs.contains(fileInfo(_Index).absoluteFilePath()))
			Res = Qt::Checked; 
		else 
			Res = Qt::Unchecked; 
	}
	else Res = QDirModel::data(_Index, _Role);
	return Res; 
}


void OCheckedDirModel::setCheckedDirs(const QStringList& _Dirs)
{
	CheckedDirs = _Dirs;
	reset();
}
