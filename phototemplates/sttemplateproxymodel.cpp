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

#include "sttemplateproxymodel.h"
#include "stphotolayout.h"
#include "sttemplatemodel.h"
#include "sttemplatemimedata.h"
#include "stpbmimetypes.h"

STTemplateProxyModel::STTemplateProxyModel(QObject* _Parent) : QSortFilterProxyModel(_Parent)
{
}


void STTemplateProxyModel::setSourceModel(STTemplateModel* _SourceModel)
{
	QSortFilterProxyModel::setSourceModel(_SourceModel);
	SourceModel = _SourceModel;
}

	//Filtering
void STTemplateProxyModel::clearFilter()
{
	Filter.clear();
	invalidateFilter();
}

void STTemplateProxyModel::setTemplateFilter(const STTemplateProxyModel::TemplateFilter& _Filter)
{
	Filter = _Filter;
	invalidateFilter();
}

bool STTemplateProxyModel::filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const
{
	bool Res = true;

	if (!Filter.isNull())
	{
		STPhotoLayoutTemplate Template = SourceModel->indexTemplate(SourceModel->index(_SourceRow, 0, _SourceParent));

		if (Filter.type() == CoverTemplate)
			Res = Template.isFirstPage();
		else
			Res = Template.numPhotoFrames() <= Filter.maxNumFrames() && Template.numPhotoFrames() >= Filter.minNumFrames() && !Template.isFirstPage();
	}

	return Res;
}

Qt::ItemFlags STTemplateProxyModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) ;

	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
	{
		return defaultFlags;
	}
}

QStringList STTemplateProxyModel::mimeTypes() const
{
	QStringList types;
	types << TEMPLATE_MIMETYPE;
	return types;
}

QMimeData* STTemplateProxyModel::mimeData(const QModelIndexList &indexes) const
{
	STTemplateMimeData *mimeData = new STTemplateMimeData();
	if (SourceModel)
	{
		QModelIndex Index = indexes.at(0);
		if (Index.isValid())
		{
			QModelIndex SourceIndex = mapToSource(Index);
			if (SourceIndex.isValid())
				mimeData->setTemplate(SourceModel->indexTemplate(SourceIndex));
		}
	}
	return mimeData;
}
