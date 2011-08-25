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

#ifndef TEMPLATEINFOMODEL_H
#define TEMPLATEINFOMODEL_H

#include <QAbstractListModel>
#include <QModelIndexList>
#include <QStringList>
#include <QList>
#include <QSizeF>
#include "stphotobookexport.h"
#include "templateinfolist.h"
#include "metainfo.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT TemplateInfoModel : public QAbstractListModel
{
    Q_OBJECT
	TemplateInfoList TemplateList;
	QStringList TemplateNameList;
	MetaInfo::EnTemplateType TemplateType;

public:
    explicit TemplateInfoModel(QObject *parent = 0);
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	TemplateInfo templateInfo(const QModelIndex& _Index, const QSizeF& _Sizes) const;
	void setTemplateList(const TemplateInfoList& _List, MetaInfo::EnTemplateType _Type);
	QList<QSizeF> sizes(const QModelIndex& _Index) const;

signals:

public slots:

};
}
#endif // TEMPLATEINFOMODEL_H
