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
#ifndef DESIGNINFOTABLEMODEL_H
#define DESIGNINFOTABLEMODEL_H

#include <QAbstractTableModel>
#include "stphotobookexport.h"
#include "designinfolist.h"
#include "templateinfo.h"
#include "stphotobookexport.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT DesignInfoTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
	enum EnColumns
	{
		ColName,
		ColVer,
		ColPublicVer,
		ColInDisk,
		ColIsPublic
	};

private:
	DesignInfoList DIList;
	TemplateInfo Template;
	QImage ItemIcon;
	QList<bool> InDiskList;



public:
	explicit DesignInfoTableModel(QObject *parent = 0);
	int rowCount(const QModelIndex& _Parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QVariant data(const QModelIndex& _Index, int _Role = Qt::DisplayRole) const;
	DesignInfo designInfo(const QModelIndex& _Index) const;
	void setTemplate(const TemplateInfo& _Template);
	TemplateInfo templateInfo() const { return Template; }
	void setDesignInfo(const QModelIndex& _Index, const DesignInfo& _DesignInfo);

signals:

public slots:

};
}

#endif // DESIGNINFOTABLEMODEL_H
