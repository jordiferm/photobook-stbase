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
#ifndef TPPRODUCTLISTVIEW_H
#define TPPRODUCTLISTVIEW_H

#include <QWidget>
#include "sttpopslexport.h"
#include "ddoc.h"

/**
List View with touch controls to scroll in touchscreens.

	@author
*/
class QListView; 
class QModelIndex; 
class QtScrollWheel;
class QAbstractItemModel;
class ST_TPOPSL_EXPORT TPProductListView : public QWidget
{
	Q_OBJECT
	QListView* MListView; 
	QtScrollWheel* ScrWheel;

	int scrollToIndexValue(int _Value);

public:
	TPProductListView(QWidget *parent = 0, bool _EmbeddedWidgets = true);
	~TPProductListView();
	void setModel(QAbstractItemModel* _Model);
	QListView* listView() { return MListView; }
	QModelIndex currentIndex() const;
	void setCurrentIndex(const QModelIndex& _Index);
	STDom::DDocProduct currentProduct() const;
	STDom::DDocProduct product(const QModelIndex& _Index) const;

private slots: 
	void scrollUpClicked();
	void scrollDownClicked();
	void slotScrollValueChanged(int _Value);
	void slotListClicked(const QModelIndex& _Index);

signals:
	void clicked(const QModelIndex& ); 

};

#endif
