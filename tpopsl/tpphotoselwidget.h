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
class TPPhotoDelegate;
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
	TPPhotoEditor* PEditor;
	AddRemoveWidget* ARWidget;
	int ImagesPerSheet; 
	STDom::PrintJobModel* Model;
	TPPhotoDelegate* PDelegate;
	QDir RootDir;
	STDom::DDocProduct SingleProduct;

	QToolButton* newActionButton(const QString& _Icon);
	QToolButton* newImageActionButton(const QString& _Icon);
	void incSelectedProduct(const QModelIndex& _Index, int _Quantity);
	void incAllProduct(int _Quantity);
	STDom::DDocProduct currentProduct() const;

public:
	TPPhotoSelWidget(QWidget* parent = 0, bool _ShowNavigation = true, Qt::WindowFlags f = 0);
	~TPPhotoSelWidget();
	void init(); 
	void retranslateUi();
	void setModel(STDom::PrintJobModel* _Model);
	void setProductsModel(QAbstractItemModel* _Model); 
	//! Instead of use productmodes uses only this product.
	void setSingleProduct(const STDom::DDocProduct& _Product);
	void setRootDir(const QDir& _Dir) { RootDir = _Dir; }
	void setDpis(int _Value); 
	//! For bill calculation.
	void setImagesPerSheet(int _Value) { ImagesPerSheet = _Value; }
	bool hasChanges() const;
	//Configuration
	void setFilterEnabled(bool _Value);
	void setBillEnabled(bool _Enabled);
	void setHasScrollBar(bool _Value);
	void setMinDpis(int _Dpis);
	void setAtomicPrintSelection(bool _Value);


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
