/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
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
class StringWheelWidget;
class ST_TPOPSL_EXPORT TPProductListView : public QWidget
{
	Q_OBJECT
	QListView* MListView; 
	QtScrollWheel* ScrWheel;
	StringWheelWidget* MWheelWidget;
	QAbstractItemModel* Model;

	int scrollToIndexValue(int _Value);

public:
	TPProductListView(QWidget *parent = 0, bool _EmbeddedWidgets = true);
	~TPProductListView();
	QSize sizeHint() const;
	void setModel(QAbstractItemModel* _Model);
	QAbstractItemModel* model() const { return Model; }

	//QListView* listView() { return MListView; }
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
