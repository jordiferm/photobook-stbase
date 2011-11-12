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

#include "designinfotablemodel.h"
#include <QDir>

using namespace SPhotoBook;

DesignInfoTableModel::DesignInfoTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int DesignInfoTableModel::rowCount(const QModelIndex& _Parent) const
{
	return DIList.size();
}


int DesignInfoTableModel::columnCount(const QModelIndex &parent) const
{
	return 5;
}

QVariant DesignInfoTableModel::headerData(int _Section, Qt::Orientation _Orientation, int _Role) const
{
	QVariant Res;
	if (_Orientation == Qt::Horizontal && _Role == Qt::DisplayRole)
	{
		switch (_Section)
		{
			case ColName :
				Res = tr("Name");
			break;
			case ColVer :
				Res = tr("Version");
			break;
			case ColPublicVer :
				Res = tr("Public Version");
			break;
			case ColInDisk :
				Res = tr("On Disk");
			break;
			case ColIsPublic :
				Res = tr("Public");
			break;
		}
	}

	return Res;
}


QVariant DesignInfoTableModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	 if (_Index.isValid() && _Index.row() < rowCount())
	 {
			 DesignInfo DInfo = DIList[_Index.row()];
			 if (_Role == Qt::DisplayRole)
			 {
				 switch (_Index.column())
				 {
					case ColName :
						Res = DInfo.name();
					break;
					case ColVer :
						Res = DInfo.metaInfo().version();
					break;
					case ColPublicVer :
						Res = DInfo.publicVersion();
					break;
					case ColInDisk :
						Res = InDiskList[_Index.row()];
					break;
					case ColIsPublic :
						Res = DInfo.isPublic();
					break;
				 }

			 }
			 else
			 if (_Role == Qt::DecorationRole)
			 {
				Res = QPixmap::fromImage(ItemIcon);
			 }
			 if (_Role == Qt::BackgroundColorRole)
			 {
				 if (DInfo.isUpToDate())
					 Res = QColor(Qt::white);
				 else
					 Res = QColor(Qt::yellow);
			 }
	 }
	 return Res;
}

DesignInfo DesignInfoTableModel::designInfo(const QModelIndex& _Index) const
{
	DesignInfo Res;
	if (_Index.row() >= 0 && _Index.row() < DIList.size())
		Res = DIList[_Index.row()];
	return Res;
}

void DesignInfoTableModel::setTemplate(const TemplateInfo& _Template)
{
	Template = _Template;
	DIList = _Template.designs();
	DesignInfoList::Iterator it;
	InDiskList.clear();
	for (it=DIList.begin(); it != DIList.end(); ++it )
	{
		QDir DesignDir(Template.absolutePath(*it));
		InDiskList.push_back(DesignDir.entryList(QDir::Files).size() > 1);
	}
	reset();
}

void DesignInfoTableModel::setDesignInfo(const QModelIndex& _Index, const DesignInfo& _DesignInfo)
{
	if (_Index.row() >= 0 && _Index.row() < DIList.size())
	{
		DIList[_Index.row()] = _DesignInfo;
		emit dataChanged(_Index, _Index);
	}
}

TemplateInfo  DesignInfoTableModel::templateInfo() const
{
	TemplateInfo Res = Template;
	Res.setDesigns(DIList);
	return Res;
}


bool DesignInfoTableModel::removeRows(int _Row, int _Count, const QModelIndex& _Parent)
{
	beginRemoveRows(_Parent, _Row, _Row + _Count - 1);
	for (int Vfor = 0; Vfor < _Count; Vfor++)
	{
		DIList.removeAt(_Row + Vfor);
	}
	endRemoveRows();
	return true;
}
