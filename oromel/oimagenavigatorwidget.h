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
#ifndef OIMAGENAVIGATORWIDGET_H
#define OIMAGENAVIGATORWIDGET_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include "ddocmodel.h"
#include "storomelexport.h"
/**
Model that filters with SelectionModel.

	@author
*/
class ST_OROMEL_EXPORT OImageNavigatorProxyModel : public QSortFilterProxyModel
{
	
	QModelIndex CurrentItemIndex; 
public:
	OImageNavigatorProxyModel(QObject* _Parent = 0) : QSortFilterProxyModel(_Parent) {}
	QVariant data(const QModelIndex& _Index, int _Role) const;
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	void setCurrentItemIndex(const QModelIndex& _Index) { CurrentItemIndex = _Index; }
};


/**
	@author
*/
class OPhotoCollectionImageModel;
class QListView; 
class OImageNavigatorWidget : public QWidget
{
	Q_OBJECT
			
	QListView* MListView;		
	OImageNavigatorProxyModel* MImageModel;
				
public:
	OImageNavigatorWidget(QWidget *parent = 0);
	~OImageNavigatorWidget();

	virtual QSize sizeHint() const;
	void setSourceModel(STDom::DDocModel* _Model);
	void setCurrentItem(const QModelIndex& _Index);
	
public slots:
	void slotSetCurrentItem(const QModelIndex& _Index);
	void setPreviousImage();
	void setNextImage();
	
signals: 
	void currentItemChanged(const QModelIndex& _Index);
};

#endif
