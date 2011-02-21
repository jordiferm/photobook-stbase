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
#include "sprintersettingswidget.h"
#include "sprintersettings.h"
#include <QLayout>
#include <QDataWidgetMapper> 
#include <QLabel> 
#include <QPushButton> 
#include <QPrintDialog> 
#include <QPrinter> 

//For ProdPrintSelector
#include <QComboBox> 
#include <QToolButton> 
#include <QPrintDialog>


#include "smappablecombobox.h" 

#include "siconfactory.h" 
#include "qxtgroupbox.h"

//__________________________________________________________________________
//
// Class ProdPrintSelector
//__________________________________________________________________________

ProdPrintSelector::ProdPrintSelector(QWidget* _Parent) : QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	CBProduct = new QComboBox(this); 
	MLayout->addWidget(CBProduct); 

	#if QT_VERSION >= 0x040400
		CBPrinters = new QComboBox(this); 
		MLayout->addWidget(CBPrinters);
	#else 
		QLabel* PrinterLabel = new QLabel(this); 
		MLayout->addWidget(PrinterLabel);
		QPushButton* PBPDialog = new QPushButton(tr("Get from system print dialog"), this); 
		connect(PBPDialog, SIGNAL(clicked( bool )), this, SLOT(getPrinterName())); 
		MLayout->addWidget(PBPDialog); 	
	#endif 

	QToolButton* DeleteButton = new QToolButton(this); 
	MLayout->addWidget(DeleteButton); 
	DeleteButton->setIcon(stIcon(SIconFactory::EditDelete)); 
	connect(DeleteButton, SIGNAL(clicked( bool )), this, SLOT(slotDeleteMe())); 	
}

void ProdPrintSelector::setProduct(const QModelIndex& _Index)
{
	CBProduct->setCurrentIndex(_Index.row()); 
}

QModelIndex ProdPrintSelector::product() const
{
	QModelIndex Res; 
	if (CBProduct->model())
		Res = CBProduct->model()->index(CBProduct->currentIndex(), CBProduct->modelColumn());
	return Res; 
}

void ProdPrintSelector::setProductModel(QAbstractItemModel* _Model)
{
	CBProduct->setModel(_Model); 
}

void ProdPrintSelector::setProductModelColumn(int _VisibleColumn)
{
	CBProduct->setModelColumn(_VisibleColumn); 
}


void ProdPrintSelector::setPrinterName(const QString& _PrinterName)
{
	#if QT_VERSION >= 0x040400
		CBPrinters->setCurrentIndex(CBPrinters->findText(_PrinterName)); 
	#else 
		PrinterLabel->setText(_PrinterName);
	#endif
}

QString ProdPrintSelector::printerName() const
{
	QString Res; 
	#if QT_VERSION >= 0x040400
		Res = CBPrinters->currentText(); 
	#else 
		Res = PrinterLabel->text();
	#endif
	return Res; 	
}

#if QT_VERSION >= 0x040400
void ProdPrintSelector::setPrinterInfo(const QList<QPrinterInfo>& _PrinterList)
{
	QList<QPrinterInfo>::const_iterator it; 
	for (it = _PrinterList.begin(); it != _PrinterList.end(); ++it)
	{
		CBPrinters->addItem(it->printerName()); 
	}
	CBPrinters->addItem(SPrinterSettings::LabPrinterName);
}
#endif 


void ProdPrintSelector::getPrinterName()
{
	#if QT_VERSION >= 0x040400
	QPrinter Printer; 
	QPrintDialog PrnDialog(&Printer); 
	if (PrnDialog.exec() == QDialog::Accepted)
	{
		PrinterLabel->setText(Printer.printerName()); 
	}
	#endif
}

void ProdPrintSelector::slotDeleteMe()
{
	emit removeMe(this);
}

//__________________________________________________________________________
//
// Class ProdPrintSettingsWidget
//__________________________________________________________________________


ProdPrintSettingsWidget::ProdPrintSettingsWidget(QWidget* _Parent) : QWidget(_Parent), ProdModel(0)
{
	MLayout = new QVBoxLayout(this); 
	QHBoxLayout* ButLayout = new QHBoxLayout; 
	MLayout->addLayout(ButLayout); 
	QToolButton* MAddButton = new QToolButton(this); 
	MAddButton->setIcon(stIcon(SIconFactory::EditAdd)); 
	connect(MAddButton, SIGNAL(clicked( bool )), this, SLOT(slotAddLine())); 
	ButLayout->addWidget(MAddButton); 
	ButLayout->addWidget(new QLabel(tr("Click to add an item"), this)); 
	clear(); 
}

#if QT_VERSION >= 0x040400
void ProdPrintSettingsWidget::setPrinterList(QList<QPrinterInfo> _PrinterList)
{
	PrinterList = _PrinterList; 
}
#endif 

void ProdPrintSettingsWidget::setProductModel(QAbstractItemModel* _Model, int _DisplayCol, int _KeyCol)
{
	ProdModel = _Model; 
	DisplayCol = _DisplayCol; 
	KeyCol = _KeyCol; 
}


void ProdPrintSettingsWidget::addLine(const QString& _ProdId, const QString& _PrinterName)
{
	if (ProdModel)
	{
		QModelIndexList ProdList = ProdModel->match(ProdModel->index(0, KeyCol), Qt::EditRole, _ProdId); 
		if (ProdList.size() > 0)
		{
			ProdPrintSelector* NewWidg = slotAddLine(); 
			NewWidg->setProduct(ProdList.first());
			NewWidg->setPrinterName(_PrinterName);
		}
		else 
			qWarning(tr("No match for product %1 in model").arg(_ProdId).toLatin1());
	}
}


void ProdPrintSettingsWidget::clear()
{
	int Cnt = 0; 
	while (Cnt < ProdPrSelList.size())
	{
		delete ProdPrSelList[Cnt++]; 
	}
	ProdPrSelList.clear(); 
}


int ProdPrintSettingsWidget::linesCount() const
{
	return ProdPrSelList.size(); 
}

QString ProdPrintSettingsWidget::prodId(int _Index) const
{
	QString Res; 
	if (ProdModel)
		Res = ProdModel->data(ProdModel->index(ProdPrSelList[_Index]->product().row(), KeyCol)).toString();
	return Res; 
}

QString ProdPrintSettingsWidget::printerName(int _Index) const 
{
	return ProdPrSelList[_Index]->printerName(); 
}

ProdPrintSelector* ProdPrintSettingsWidget::slotAddLine()
{
	ProdPrintSelector* NewWidget = new ProdPrintSelector(this); 
	NewWidget->setProductModel(ProdModel); 
	NewWidget->setProductModelColumn(DisplayCol); 
#if QT_VERSION >= 0x040400
	NewWidget->setPrinterInfo(PrinterList); 
#endif 
	connect(NewWidget, SIGNAL(removeMe(ProdPrintSelector*)), this, SLOT(slotDeleteLine(ProdPrintSelector*))); 
	MLayout->addWidget(NewWidget); 
	ProdPrSelList.push_back(NewWidget); 
	return NewWidget; 
}

void ProdPrintSettingsWidget::slotDeleteLine(ProdPrintSelector* _LineWidget)
{
	int WIndex = ProdPrSelList.indexOf(_LineWidget); 
	if (WIndex != -1)
		ProdPrSelList.removeAt(WIndex); 
	delete _LineWidget;
}


//_____________________________________________________________________________
//
// Class SPrinterSettingsWidget
//_____________________________________________________________________________



SPrinterSettingsWidget::SPrinterSettingsWidget(SPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent, bool _LabEnabled) : SMappedWidget(_Settings.model(), _Mapper, _Parent)
{
	Model = _Settings.model();
	QGridLayout* MLayout = new QGridLayout(this); 
	MLayout->setSpacing(1); 
	PrnNameIndex = Model->index(_Settings.keyFullName("systemprintername")); 
	MLayout->addWidget(createHeaderLabel(PrnNameIndex), 0, 0);
	MProdPrSettingsW = new ProdPrintSettingsWidget(this); 
	#if QT_VERSION >= 0x040400
		QList<QPrinterInfo> PrinterList = QPrinterInfo::availablePrinters(); 
		SMappableComboBox* PrnCombo = new SMappableComboBox(this); 
		PrnCombo->setUseTextAsData(true); 
		MLayout->addWidget(PrnCombo, 1, 0);
		QList<QPrinterInfo>::iterator it; 
		for (it = PrinterList.begin(); it != PrinterList.end(); ++it)
		{
			PrnCombo->addItem(it->printerName()); 
		}
		if (_LabEnabled)
			PrnCombo->addItem(SPrinterSettings::LabPrinterName);
		_Mapper->addMapping(PrnCombo, PrnNameIndex.column()); 
		MProdPrSettingsW->setPrinterList(PrinterList); 
	#else 
		MLayout->addWidget(createEditor(PrnNameIndex), 1, 0);
		QPushButton* PBPDialog = new QPushButton(tr("Get from system print dialog"), this); 
		connect(PBPDialog, SIGNAL(clicked( bool )), this, SLOT(getPrinterName())); 
		MLayout->addWidget(PBPDialog, 1, 1); 	
	#endif 

	// -- Printer by product -- 
	QxtGroupBox* GBProdPrint = new QxtGroupBox(headerLabelText(Model->index(_Settings.keyFullName("printerbyproductenabled"))), this);
	_Mapper->addMapping(GBProdPrint, 	Model->index(_Settings.keyFullName("printerbyproductenabled")).column()); 
	MLayout->addWidget(GBProdPrint, 2, 0, 1, 2); 

	QHBoxLayout* GBProdPrintLayout = new QHBoxLayout(GBProdPrint); 

	GBProdPrintLayout->addWidget(MProdPrSettingsW);
}

void SPrinterSettingsWidget::setProductModel(QAbstractItemModel* _Model, int _DisplayCol, int _KeyCol)
{
	MProdPrSettingsW->setProductModel(_Model, _DisplayCol, _KeyCol); 
}

void SPrinterSettingsWidget::saveProductPrinters()
{
	SPrinterSettings::clearProductPrinters();
	for (int Vfor = 0; Vfor < MProdPrSettingsW->linesCount(); Vfor++)
	{
		SPrinterSettings::setProductPrinter(Vfor, MProdPrSettingsW->prodId(Vfor), MProdPrSettingsW->printerName(Vfor));
	}
}

void SPrinterSettingsWidget::loadProductPrinters()
{
	for (int Vfor = 0; Vfor < SPrinterSettings::numProdPrinters(); Vfor++)
	{
		MProdPrSettingsW->addLine(SPrinterSettings::prodId(Vfor), SPrinterSettings::printerName(Vfor));
	}
}

void SPrinterSettingsWidget::getPrinterName()
{
	QPrinter Printer; 
	QPrintDialog PrnDialog(&Printer); 
	if (PrnDialog.exec() == QDialog::Accepted)
	{
		Model->setData(PrnNameIndex, Printer.printerName());
	}
}

