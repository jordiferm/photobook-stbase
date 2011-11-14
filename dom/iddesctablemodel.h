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
#ifndef IDESCTABLEMODEL_H
#define IDESCTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>
#include "stdomexport.h"

namespace STDom
{

class ST_DOM_EXPORT IdDescTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	enum EnColumns
	{
		ColId = 0,
		ColDescription = 1
	};
	typedef QPair<QVariant, QString>  TKeyValue;
	typedef QList<TKeyValue> TKeyValueList;
private:

	TKeyValueList PairList;

public:
	explicit IdDescTableModel(QObject *parent = 0);
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	void addItem(const QVariant& _Id, const QString& _Description);
	void setValues(const TKeyValueList& _Values);
	void clear();

signals:

public slots:

};
}

#endif // IDESCTABLEMODEL_H
