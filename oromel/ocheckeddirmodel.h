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
#ifndef OCHECKEDDIRMODEL_H
#define OCHECKEDDIRMODEL_H

#include <QDirModel>
#include <QString>
#include "storomelexport.h"

/**
	@author
*/
class ST_OROMEL_EXPORT OCheckedDirModel : public QDirModel
{
	QStringList CheckedDirs; 

public:
	OCheckedDirModel(QObject* parent);
	OCheckedDirModel(const QStringList& nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject* parent);

	~OCheckedDirModel();

	bool setData(const QModelIndex& index, const QVariant& value, int role);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role) const;
	void setCheckedDirs(const QStringList& _Dirs); 
	QStringList checkedDirs() const { return CheckedDirs; }
};

#endif
