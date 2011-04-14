
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

#include "wproductmanager.h"
#include <QLayout> 
#include <QGroupBox>
#include <QTabWidget> 
#include <QLabel>
#include <QDataWidgetMapper> 
#include <QToolButton>
#include <QDebug>

#include "frecdialog.h"
#include "ftablemanager.h"
#include "fsqlrelationaltablemodel.h"
#include "fsql1nmappersync.h"
#include "fsqlsearchwidget.h"
#include "fgenmanager.h"
#include "ftablemanager.h" 
#include "fsqlmodelviewutils.h"
#include "fsqlactiontableview.h" 
#include "fsqlquery.h" 
#include "publisherdatabase.h"
#include "fintvalueselector.h"
#include "globals.h"
#include "siconfactory.h"
#include "smessagebox.h"

// _________________________________________________________________________
//
// Class WProductRecord
// _________________________________________________________________________

void WProductRecord::createWidget()
{
	setMinimumWidth(450); 
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->addWidget(createEditorsGroup("ref,privateref,templates_ref", new QGroupBox(tr("References"), this)));

	QHBoxLayout* TypeLayout = new QHBoxLayout;
	MLayout->addLayout(TypeLayout);

	TypeLayout->addWidget(createHeaderLabel("type"));
	FIntValueSelector* CBType = new FIntValueSelector(this);
	CBType->addItem(tr("Digital Prints"), STDom::PublisherDatabase::DigiprintsProduct);
	CBType->addItem(tr("Decorations"), STDom::PublisherDatabase::DecorationsProduct);
	CBType->addItem(tr("PhotoBook"), STDom::PublisherDatabase::PhotoBookProduct);
	CBType->addItem(tr("CDRecord"), STDom::PublisherDatabase::CDRecordProduct);
	CBType->addItem(tr("Video"), STDom::PublisherDatabase::VideoProduct);
	CBType->addItem(tr("Gifts"), STDom::PublisherDatabase::GiftProduct);
	CBType->addItem(tr("PhotoIndex"), STDom::PublisherDatabase::PhotoIndexProduct);
	CBType->addItem(tr("PhotoId"), STDom::PublisherDatabase::PhotoIdProduct);
	TypeLayout->addWidget(CBType);
	Mapper->addMapping(CBType, FSqlModelViewUtils::indexOf(Model,"type"));
	TypeLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));

	MLayout->addWidget(createEditorsGroup("description,label,ordering,fixedprice", new QGroupBox(tr("General"), this)));

	//Formats
	QHBoxLayout* FormatLayout = new QHBoxLayout;
	MLayout->addLayout(FormatLayout);
	FormatLayout->addWidget(createHeaderLabel("formats_idformats"));
	FormatLayout->addWidget(createEditor("formats_idformats"));
	connect(TBFormatManager, SIGNAL(clicked()), this, SIGNAL(openFormatManager()));
	TBFormatManager->setIcon(stIcon(SIconFactory::Configure));
	FormatLayout->addWidget(TBFormatManager);
	FormatLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));


	QTabWidget* TWidget = new QTabWidget(this);
	MLayout->addWidget(TWidget);

	setSuffix("fixedprice", EURO_UTF_STR);
	
	FGenManager* WPriceManager = new FGenManager("productprices", this, "quantity,price", false, Model->database());
	TWidget->addTab(WPriceManager, tr("Prices"));
	WPriceManager->tableManager()->enableSaveState("pricesmanager");
	WPriceManager->tableManager()->searchWidget()->setSearchColumn(2);
 	(void*) new FSql1NMapperSync(Mapper, FSqlModelViewUtils::indexOf(Model, "ref"), WPriceManager->model(), FSqlModelViewUtils::indexOf( WPriceManager->model(), "products_ref"));
 	static_cast<FRecDialog*>(parentWidget())->addTableManager(WPriceManager->tableManager());	
}

WProductRecord::WProductRecord(FRecDialog* _Parent)
	: FRecordWidget(_Parent->model(), _Parent)
{
	TBFormatManager = new QToolButton(this);
}

void WProductRecord::setInserting(bool _Value)
{
	TBFormatManager->setEnabled(!_Value);
	FRecordWidget::setInserting(_Value);
}

// _________________________________________________________________________
//
// Class WProductManager
// _________________________________________________________________________

WFormatManager::WFormatManager(QWidget* _Parent, const QSqlDatabase& _Database)
: FGenManager("formats", _Parent, "description,width,height", true, _Database), LocalAdded(false)
{
	connect(TManager->actionTableView(), SIGNAL(beforeRemoveRow(int , bool& )),
			this, SLOT(beforeRemoveRow(int , bool& )));
}

void WFormatManager::primeInsert(int _Row, QSqlRecord& _Record)
{
	_Record.setValue(FSqlModelViewUtils::indexOf(Model, "localadded"), LocalAdded);
	FGenManager::primeInsert(_Row, _Record);

}

void WFormatManager::beforeRemoveRow(int _Index, bool& _PerformOp)
{
	if (_PerformOp)
	{
		FSqlQuery Query(Model->database());
		QSqlRecord CurRec = Model->record(_Index);
		Query.prepare("SELECT * FROM products WHERE formats_idformats=:idformats");
		Query.bindValue(":idformats", CurRec.value("idformats"));
		Query.exec();
		if (Query.next())
		{
			_PerformOp = false;
			SMessageBox::warning(this, tr("Removing formats"), tr("You can not remove this format because there are products that references it"));
		}
	}
}


// _________________________________________________________________________
//
// Class WProductManager
// _________________________________________________________________________

WProductManager::WProductManager(QWidget* _Parent, const QSqlDatabase& _Database)
: FGenManager("products", _Parent, "ref,description,label,formats_idformats,fixedprice", true, _Database), LocalAdded(false)
{

	FRecDialog* MRecDialog = new FRecDialog(Model, TManager);
	MRecWidget = new WProductRecord(MRecDialog);
	MRecDialog->setMainWidget(MRecWidget);
	TManager->setRecordWidget(MRecDialog);	
	//TManager->enableSaveState("productmanager");
	TManager->setEditable(false);
	connect(TManager->actionTableView(), SIGNAL(beforeRemoveRow(int , bool& )),
	        this, SLOT(beforeRemoveRow(int , bool& )));
	connect(MRecWidget, SIGNAL(openFormatManager()), this, SLOT(slotOpenFormatManager()));

	FormatsDialog = new FTSDialog(this);
	FormatsDialog->addAction(new FAcceptAction(this));

	FormatManager = new WFormatManager(FormatsDialog, _Database);
	FormatsDialog->setMainWidget(FormatManager);
}

	//! Means that products added from this manager are not published.
void WProductManager::setLocalAdded(bool _Value)
{
	LocalAdded = _Value;
	FormatManager->setLocalAdded(_Value);
}


void WProductManager::beforeRemoveRow(int _Index, bool& _PerformOp)
{
 	if (_PerformOp)
 	{
		FSqlQuery Query(Model->database());
 		QSqlRecord CurRec = Model->record(_Index);
 		_PerformOp = _PerformOp && Query.exec("DELETE FROM productprices WHERE products_ref='" + CurRec.value("ref").toString() + "'");
 	}	
}

void WProductManager::primeInsert(int /*_Row*/, QSqlRecord& _Record)
{
	_Record.setValue(FSqlModelViewUtils::indexOf(Model, "localadded"), LocalAdded);
}

void WProductManager::slotOpenFormatManager()
{
	FormatsDialog->exec();
	//Update formats model
	TManager->updateLookups();
	int CIndex = MRecWidget->mapper()->currentIndex();
	Model->select();
	MRecWidget->mapper()->setCurrentIndex(CIndex);
}
