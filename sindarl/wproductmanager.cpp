
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

// _________________________________________________________________________
//
// Class WProductRecord
// _________________________________________________________________________

void WProductRecord::createWidget()
{
	setMinimumWidth(450); 
	QGridLayout* MLayout = new QGridLayout(this);
	MLayout->addWidget(createEditorsGroup("ref,privateref,templates_ref", new QGroupBox(tr("References"), this)),MLayout->rowCount(), 0, 1, 2);

	MLayout->addWidget(createHeaderLabel("type"), MLayout->rowCount(), 0);
	FIntValueSelector* CBType = new FIntValueSelector(this);
	CBType->addItem(tr("Digital Prints"), STDom::PublisherDatabase::DigiprintsProduct);
	CBType->addItem(tr("Decorations"), STDom::PublisherDatabase::DecorationsProduct);
	CBType->addItem(tr("PhotoBook"), STDom::PublisherDatabase::PhotoBookProduct);
	CBType->addItem(tr("CDRecord"), STDom::PublisherDatabase::CDRecordProduct);
	CBType->addItem(tr("Video"), STDom::PublisherDatabase::VideoProduct);
	CBType->addItem(tr("Gifts"), STDom::PublisherDatabase::GiftProduct);
	CBType->addItem(tr("PhotoIndex"), STDom::PublisherDatabase::PhotoIndexProduct);
	CBType->addItem(tr("PhotoId"), STDom::PublisherDatabase::PhotoIdProduct);
	MLayout->addWidget(CBType, MLayout->rowCount() - 1, 1);
	Mapper->addMapping(CBType, FSqlModelViewUtils::indexOf(Model,"type"));

	MLayout->addWidget(createEditorsGroup("description,label,width,height,ordering,fixedprice", new QGroupBox(tr("General"), this)),MLayout->rowCount(),0, 1, 2);

	QTabWidget* TWidget = new QTabWidget(this);
	MLayout->addWidget(TWidget,MLayout->rowCount(),0,1,2);

	setSuffix("fixedprice", EURO_UTF_STR);
	
	FGenManager* WPriceManager = new FGenManager("productprices", this, "quantity,price", false, Model->database());
	TWidget->addTab(WPriceManager, tr("Prices"));
	WPriceManager->tableManager()->searchWidget()->setSearchColumn(2);
 	(void*) new FSql1NMapperSync(Mapper, FSqlModelViewUtils::indexOf(Model, "ref"), WPriceManager->model(), FSqlModelViewUtils::indexOf( WPriceManager->model(), "products_ref"));
 	static_cast<FRecDialog*>(parentWidget())->addTableManager(WPriceManager->tableManager());	
}

WProductRecord::WProductRecord(FRecDialog* _Parent)
	: FRecordWidget(_Parent->model(), _Parent)
{
}

// _________________________________________________________________________
//
// Class WProductManager
// _________________________________________________________________________

WProductManager::WProductManager(QWidget* _Parent, const QSqlDatabase& _Database)
: FGenManager("products", _Parent, "ref,description,label,width,height,fixedprice", true, _Database)
{

	FRecDialog* MRecDialog = new FRecDialog(Model, TManager);
	WProductRecord* MRecWidget = new WProductRecord(MRecDialog);
	MRecDialog->setMainWidget(MRecWidget);
	TManager->setRecordWidget(MRecDialog);	
	TManager->setEditable(false);
	connect(TManager->actionTableView(), SIGNAL(beforeRemoveRow(int , bool& )),
	        this, SLOT(beforeRemoveRow(int , bool& )));
}

void WProductManager::beforeRemoveRow(int _Index, bool& _PerformOp)
{
 	if (_PerformOp)
 	{
 		FSqlQuery Query;
 		QSqlRecord CurRec = Model->record(_Index);
 		//qDebug(QString("WDigiPrintManager::::beforeRemoveRow:" + CurRec.value("ref").toString()).toLatin1() );
 		_PerformOp = _PerformOp && Query.exec("DELETE FROM productprices WHERE products_ref='" + CurRec.value("ref").toString() + "'");
 	}	
}


