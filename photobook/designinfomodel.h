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

#ifndef DESIGNINFOMODEL_H
#define DESIGNINFOMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include "stphotobookexport.h"
#include "designinfolist.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT DesignInfoModel : public QAbstractListModel
{
    Q_OBJECT
	DesignInfoList DIList;
	QImage ItemIcon;

public:
    explicit DesignInfoModel(QObject *parent = 0);
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	DesignInfo designInfo(const QModelIndex& _Index) const;
	void setDesignInfoList(const DesignInfoList& _List);

signals:

public slots:

};
}
#endif // DESIGNINFOMODEL_H
