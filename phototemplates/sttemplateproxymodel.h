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

#ifndef STTEMPLATEPROXYMODEL_H
#define STTEMPLATEPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "stphototemplatesexport.h"

class STTemplateModel;
class ST_PHOTOTEMPLATES_EXPORT STTemplateProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	enum EnTemplateType
	{
		CoverTemplate,
		PageTemplate
	};

	class TemplateFilter
	{
		EnTemplateType Type;
		int MaxNumFrames;
		int MinNumFrames;
		bool IsNull;

	public:
		TemplateFilter(EnTemplateType _Type, int _MinNumFrames, int _MaxNumFrames) : Type(_Type), MinNumFrames(_MinNumFrames), MaxNumFrames(_MaxNumFrames), IsNull(false) {}
		TemplateFilter() { clear(); }
		EnTemplateType type() const { return Type; }
		int maxNumFrames() const { return MaxNumFrames; }
		int minNumFrames() const { return MinNumFrames; }
		void clear() { MaxNumFrames = 0; MinNumFrames = 0; IsNull = true; }
		bool isNull() const { return IsNull; }
	};



private:
	STTemplateModel* SourceModel;
	TemplateFilter Filter;

public:
	STTemplateProxyModel(QObject* _Parent = 0);
	STTemplateModel* sourceModel() { return SourceModel; }
	void setSourceModel(STTemplateModel* _SourceModel);

	//Filtering
	void clearFilter();
	TemplateFilter filter() const { return Filter; }
	void setTemplateFilter(const TemplateFilter& _Filter);
	virtual bool filterAcceptsRow ( int _SourceRow, const QModelIndex& _SourceParent ) const;
	//Drag and Drop
	Qt::ItemFlags flags(const QModelIndex & index) const;
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
};

Q_DECLARE_METATYPE(STTemplateProxyModel::TemplateFilter)

#endif // STTEMPLATEPROXYMODEL_H
