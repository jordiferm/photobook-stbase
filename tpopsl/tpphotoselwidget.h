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
#ifndef TPPHOTOSELWIDGET_H
#define TPPHOTOSELWIDGET_H

#include <QWidget>
#include <QSqlRecord>
#include <QDir> 
#include "sttpopslexport.h"

#include "printjobmodel.h"

/**
Widget to Select Photos for TouchScreens.

	@author
*/
class QToolButton;
class QListView; 
class QAbstractItemModel; 
class STProductPrintsProxyModel; 
class TPPhotoListView; 
class TPProductListView; 
class QModelIndex; 
class QLabel; 
class TPPhotoEditor; 
class AddRemoveWidget; 
class ST_TPOPSL_EXPORT TPPhotoSelWidget : public QWidget
{
	Q_OBJECT
	bool NoCalcBill; 
	
	QToolButton* FilterBut;
	QToolButton* NoFilterBut;
	QToolButton* ZoomBut;
	QToolButton* CutNoCutBut;
	QToolButton* CancelBut; 
	QToolButton* AcceptBut; 

	TPProductListView* LVProducts;
	TPPhotoListView* LVImages;
	QLabel* ReceiptLabel; 
	QSqlRecord ShippingMethod;
	TPPhotoEditor* PEditor;
	AddRemoveWidget* ARWidget;
	int ImagesPerSheet; 
	STDom::PrintJobModel* Model;
	QDir RootDir;

	QToolButton* newActionButton(const QString& _Icon);
	QToolButton* newImageActionButton(const QString& _Icon);
	void incSelectedProduct(const QModelIndex& _Index, int _Quantity);
	void incAllProduct(int _Quantity);

public:
	TPPhotoSelWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~TPPhotoSelWidget();
	void init(); 
	void retranslateUi();
	void setModel(STDom::PrintJobModel* _Model);
	void setProductsModel(QAbstractItemModel* _Model); 
	void setRootDir(const QDir& _Dir) { RootDir = _Dir; }
	void setShippingMethod(const QSqlRecord& _Value) { ShippingMethod = _Value; } 
	void setDpis(int _Value); 
	//! To bill calculation.
	void setImagesPerSheet(int _Value) { ImagesPerSheet = _Value; }
	bool hasChanges() const;
	void setFilterEnabled(bool _Value);
	void setHasScrollBar(bool _Value);
	void setMinDpis(int _Dpis);


public slots: 
	void editAddProduct(const QModelIndex& _Index);
	void editAddProductToAll();
	void editDelProduct(const QModelIndex& _Index);
	void editCloneProduct(const QModelIndex& _Index);
	void editDelProductToAll();
	void calcBill();
	void editCurrent();
	void nextPage();
	void previousPage();
	void getFolderFilter();
	void clearFolderFilter();
	void slotAccepted(); 

signals: 
	void accepted(); 
	void rejected();
};

#endif
