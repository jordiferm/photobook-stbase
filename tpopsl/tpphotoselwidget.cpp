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
#include "tpphotoselwidget.h"
#include <QLayout> 
#include <QListView> 
#include <QToolButton>
#include <QLabel> 
#include <QApplication> 
#include <QKeyEvent>
#include <QDebug>
//#include <QFileDialog> 

#include "stfolderselector.h" 

#include "tpphotolistview.h" 
#include "tpphotodelegate.h" 
#include "tpproductlistview.h" 

#include "tpphotoeditor.h"  
#include "addremovewidget.h" 
#include "printjobmodel.h"

#include "stutils.h"
#include "smessagebox.h"

//Calc Bill
#include "publisherdatabase.h"

void TPPhotoSelWidget::incSelectedProduct(const QModelIndex& _Index, int _Quantity)
{
	//if (STDom::PrintJobModel* PrPrModel = static_cast<STDom::PrintJobModel*>(LVImages->model()))
	if (Model)
		Model->incProductCopies(_Index, _Quantity, currentProduct());
}

void TPPhotoSelWidget::incAllProduct(int _Quantity)
{
	//if (STDom::PrintJobModel* PrPrModel = static_cast<STDom::PrintJobModel*>(LVImages->model()))
	if (Model)
	{
		NoCalcBill = true;
		Model->incProductCopiesAll(_Quantity, currentProduct());
		NoCalcBill = false; 
		calcBill(); 
	}
}

STDom::DDocProduct TPPhotoSelWidget::currentProduct() const
{
	STDom::DDocProduct Res;
	if (SingleProduct.isNull())
		Res = LVProducts->currentProduct();
	else
		Res = SingleProduct;
	return Res;
}


QToolButton* TPPhotoSelWidget::newActionButton(const QString& _Icon)
{
	QToolButton* Res = new QToolButton(this); 
	Res->setObjectName("ActionButton"); 
 	Res->setIcon(QIcon(_Icon));
 	Res->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 
 	Res->setIconSize(QSize(64,64)); 
	return Res; 
}

QToolButton* TPPhotoSelWidget::newImageActionButton(const QString& _Icon)
{
	QToolButton* Res = newActionButton(_Icon); 
	Res->setObjectName("ImageActionButton"); 
	return Res; 
}

TPPhotoSelWidget::TPPhotoSelWidget(QWidget* parent, bool _ShowNavigation, Qt::WindowFlags f): QWidget(parent, f), NoCalcBill(false), ImagesPerSheet(1), Model(0)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(2); 
	MLayout->setSpacing(2); 
	QHBoxLayout* TopLayout = new QHBoxLayout; 
	MLayout->addLayout(TopLayout); 

	LVImages = new TPPhotoListView(this); 
	PDelegate = new TPPhotoDelegate(LVImages);
	connect(PDelegate, SIGNAL(editAddRequested(const QModelIndex&)), this, SLOT(editAddProduct(const QModelIndex&))); 
	connect(PDelegate, SIGNAL(editDelRequested(const QModelIndex&)), this, SLOT(editDelProduct(const QModelIndex&))); 
	connect(PDelegate, SIGNAL(editCloneRequested(const QModelIndex&)), this, SLOT(editCloneProduct(const QModelIndex&))); 
	LVImages->setItemDelegate(PDelegate); 

	//Left Action Buttons
	QVBoxLayout* RButLayout = new QVBoxLayout; 
	TopLayout->addLayout(RButLayout); 

	TopLayout->addWidget(LVImages); 

	QToolButton* PrevPageBut = newImageActionButton(":/st/tpopsl/1uparrow.png");
	connect(PrevPageBut, SIGNAL(clicked( bool )), this, SLOT(previousPage())); 
	RButLayout->addWidget(PrevPageBut); 

	RButLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding)); 

	ZoomBut = newImageActionButton(":/st/tpopsl/zoom.png");
	connect(ZoomBut, SIGNAL(clicked( bool )), this, SLOT(editCurrent())); 
	RButLayout->addWidget(ZoomBut); 

	FilterBut = newImageActionButton(":/st/tpopsl/filter.png");
	connect(FilterBut, SIGNAL(clicked( bool)), this, SLOT(getFolderFilter())); 
	RButLayout->addWidget(FilterBut); 

	NoFilterBut = newImageActionButton(":/st/tpopsl/nofilter.png");
	connect(NoFilterBut, SIGNAL(clicked( bool)), this, SLOT(clearFolderFilter())); 
	NoFilterBut->setVisible(false); 
	RButLayout->addWidget(NoFilterBut); 

	RButLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding)); 

	QToolButton* NextPageBut = newImageActionButton(":/st/tpopsl/1downarrow.png");
	connect(NextPageBut, SIGNAL(clicked( bool )), this, SLOT(nextPage())); 
	RButLayout->addWidget(NextPageBut); 


	CutNoCutBut = newImageActionButton(":/st/tpopsl/nocut.png");
	RButLayout->addWidget(CutNoCutBut); 
	CutNoCutBut->setVisible(false); 


	// Bottom Frame 
	QFrame* BottomFrame = new QFrame(this); 
	MLayout->addWidget(BottomFrame);
	if (_ShowNavigation)
	{
		BottomFrame->setMinimumHeight(200);
	}
	MLayout->setStretchFactor(TopLayout, 5);
	MLayout->setStretchFactor(BottomFrame, 1);
	QHBoxLayout* BFrameLayout = new QHBoxLayout(BottomFrame);

	CancelBut = newActionButton(":/st/tpopsl/previous.png");
	CancelBut->setVisible(_ShowNavigation);
	connect(CancelBut, SIGNAL(clicked( bool )), this, SIGNAL(rejected())); 
	BFrameLayout->addWidget(CancelBut); 

	BFrameLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Preferred)); 

	LVProducts = new TPProductListView(this); 
	BFrameLayout->addWidget(LVProducts); 

	if (_ShowNavigation)
		ARWidget = new AddRemoveWidget(this);
	else
		ARWidget = new AddRemoveWidget(this, Qt::Horizontal);

	connect(ARWidget, SIGNAL(addClicked()), this, SLOT(editAddProductToAll())); 
	connect(ARWidget, SIGNAL(removeClicked()), this, SLOT(editDelProductToAll())); 
	BFrameLayout->addWidget(ARWidget); 

	ReceiptLabel = new QLabel(tr("Receipt"), this); 
	ReceiptLabel->setObjectName("ReceiptLabel");
	BFrameLayout->addWidget(ReceiptLabel); 

	BFrameLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Preferred)); 
	
	AcceptBut = newActionButton(":/st/tpopsl/printer.png");
	AcceptBut->setVisible(_ShowNavigation);
	connect(AcceptBut, SIGNAL(clicked( bool )), this, SLOT(slotAccepted())); 
	BFrameLayout->addWidget(AcceptBut); 

	BFrameLayout->setStretchFactor(ReceiptLabel, 3); 
	BFrameLayout->setStretchFactor(ARWidget, 4); 
	BFrameLayout->setStretchFactor(LVProducts, 2); 

	PEditor = new TPPhotoEditor(this); 
	connect(PEditor, SIGNAL(orderChanged()), this, SLOT(calcBill()));
	retranslateUi();
}

TPPhotoSelWidget::~TPPhotoSelWidget()
{
}


void TPPhotoSelWidget::init()
{
	if (Model)
	{
		if (Model->rowCount() > 0)
		{
			NoFilterBut->setVisible(false); 
			calcBill();
		}
	}

	STUtils::rmDir(PEditor->tempPath()); 
	retranslateUi();
}

void TPPhotoSelWidget::retranslateUi()
{
	ZoomBut->setText(tr("Zoom")); 
	FilterBut->setText(tr("Folders")); 
	NoFilterBut->setText(tr("No Filter"));
	CutNoCutBut->setText(tr("No Cut")); 
	CancelBut->setText(tr("Back")); 
	AcceptBut->setText(tr("Checkout"));
	ARWidget->setAddLabel(tr("All")); 
	ARWidget->setRemoveLabel(tr("All")); 
}

void TPPhotoSelWidget::setModel(STDom::PrintJobModel* _Model)
{
	Model = _Model; 
	LVImages->setModel(_Model); 
	PEditor->setModel(_Model); 
	LVImages->setThumbnailSize(_Model->thumbnailSize().width());
	//connect(_Model, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )), this, SLOT(calcBill()));
}

void TPPhotoSelWidget::setProductsModel(QAbstractItemModel* _Model)
{
	LVProducts->setVisible(true);
	LVProducts->setModel(_Model);
	PEditor->setProductsModel(_Model); 
}

void TPPhotoSelWidget::setSingleProduct(const STDom::DDocProduct& _Product)
{
	SingleProduct = _Product;
	LVProducts->setVisible(false);
	PEditor->setSingleProduct(_Product);
}


void TPPhotoSelWidget::setDpis(int _Value)
{
	PEditor->setDpis(_Value); 
}

bool TPPhotoSelWidget::hasChanges() const
{
	return Model->hasCopies(); 
}

void TPPhotoSelWidget::setFilterEnabled(bool _Value)
{
	FilterBut->setVisible(_Value);
}

void TPPhotoSelWidget::setBillEnabled(bool _Enabled)
{
	ReceiptLabel->setVisible(_Enabled);
}


void TPPhotoSelWidget::setHasScrollBar(bool _Value)
{
	if (_Value)
		LVImages->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	else
		LVImages->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TPPhotoSelWidget::setMinDpis(int _Dpis)
{
	PEditor->setMinDpis(_Dpis);
}


void TPPhotoSelWidget::setAtomicPrintSelection(bool _Value)
{
	PDelegate->setEditActionsVisible(!_Value);
	PDelegate->setCopyActionVisible(!_Value);
}


void TPPhotoSelWidget::editAddProduct(const QModelIndex& _Index)
{
	incSelectedProduct(_Index, 1); 
	calcBill();
}


void TPPhotoSelWidget::editAddProductToAll()
{
	incAllProduct(1); 
}

void TPPhotoSelWidget::editDelProduct(const QModelIndex& _Index)
{
	incSelectedProduct(_Index, -1); 
	calcBill();
}

void TPPhotoSelWidget::editCloneProduct(const QModelIndex& _Index)
{
	if (STDom::PrintJobModel* PrPrModel = static_cast<STDom::PrintJobModel*>(LVImages->model()))
	{
		QModelIndex CIndex = LVImages->currentIndex(); 
		PrPrModel->cloneImage(LVImages->currentIndex()); 
		LVImages->setCurrentIndex(CIndex); 
	}
}

void TPPhotoSelWidget::editDelProductToAll()
{
	incAllProduct(-1); 
}

void TPPhotoSelWidget::calcBill()
{
	if (!NoCalcBill && ReceiptLabel->isVisible())
	{
		STDom::PublisherDatabase PublDB;
		//QString StrBill = PublDB.billRitchText(static_cast<STProductPrintsProxyModel*>(LVImages->model()), ShippingMethod, ImagesPerSheet);
		QString StrBill = PublDB.billRitchText(Model->printJob(), ShippingMethod, ImagesPerSheet);
		ReceiptLabel->setText(StrBill);
		PEditor->setReceiptText(StrBill); 
	}
}

void TPPhotoSelWidget::editCurrent()
{
	//PEditor->setModel(static_cast<STProductPrintsProxyModel*>(LVImages->model())); 
	try
	{
		PEditor->setCurrentProductIndex(LVProducts->currentIndex());
		PEditor->exec(LVImages->currentIndex()); 
	}
	catch(const STError& _Error)
	{
		QApplication::restoreOverrideCursor();
		SMessageBox::critical(this, tr("Error opening image"), _Error.description());
	}
}

void TPPhotoSelWidget::nextPage()
{
	qApp->notify(LVImages, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
}

void TPPhotoSelWidget::previousPage()
{
	qApp->notify(LVImages, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
}

void TPPhotoSelWidget::getFolderFilter()
{
	if (Model)
	{
		QString FFilter = STFolderSelector::getExistingDirectory(this, tr("Please select directory to filter"), RootDir.absolutePath());
		if (!FFilter.isEmpty())
		{
			Model->setFolderFilter(QDir::cleanPath(FFilter));
			NoFilterBut->setVisible(true); 
		}


/*		QFileDialog FDialog(this); 
		FDialog.setDirectory(CFilterDir); 
		FDialog.setWindowTitle(tr("Please select directory to filter")); 
		FDialog.setAcceptMode(QFileDialog::AcceptOpen); 
		FDialog.setFileMode(QFileDialog::DirectoryOnly); 
		FDialog.setViewMode(QFileDialog::Detail); 
		//FDialog.setOption(QFileDialog::ShowDirsOnly); 
		FDialog.setReadOnly(true); 
		if (FDialog.exec())
		{
			QString FFilter = FDialog.selectedFiles().first();
			if (!FFilter.isEmpty())
			{
				Model->setFolderFilter(FFilter); 
				NoFilterBut->setVisible(true); 
			}
		}	*/
	}
}

void TPPhotoSelWidget::clearFolderFilter()
{
	if (Model)
	{
		Model->clearFolderFilter(); 
		NoFilterBut->setVisible(false); 
		calcBill();
	}
}

void TPPhotoSelWidget::slotAccepted()
{
	bool Accepted = false; 
	if (Model)
	{
		clearFolderFilter();
		Accepted = Model->hasCopies(); 
	}
	if (Accepted)
		emit accepted(); 
	else
		SMessageBox::critical(this, tr("No products selected"), tr("You need to select at least one product"));

}
