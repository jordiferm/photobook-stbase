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

#include "wprintworkflowmanager.h"
#include <QLayout> 
#include <QSqlQueryModel> 
#include <QSqlTableModel>
#include <QLabel>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QSqlQuery>

#include <QMetaObject>
#include <QMetaProperty>

#include "fsqlquery.h"
#include "factiontableview.h"
#include "frecdialog.h"
#include "fgenmanager.h"
#include "ftablemanager.h"

// ____________________________________________________________________________________
//
// Class RefColComboBox
// ____________________________________________________________________________________


RefColComboBox::RefColComboBox(int _RefCol, QWidget* _Parent) : QComboBox(_Parent), RefCol(_RefCol)
{
	if (_RefCol < 0)
		qWarning("Warning! : RefColComboBox RefCol is not valid.");
}

QVariant RefColComboBox::value() const
{
	return model()->data(model()->index(currentIndex(), RefCol), Qt::EditRole);
}

void RefColComboBox::setValue(const QVariant& _Value)
{
	if (_Value.isNull())
		setCurrentIndex(model()->rowCount() -1 );
	else 
	{
		QModelIndex Res;
		QModelIndexList IList = model()->match(model()->index(0, RefCol), Qt::EditRole, 
															_Value, 1,  Qt::MatchExactly);
		if (IList.size() > 0)
			setCurrentIndex(IList.first().row());
	}
}

void RefColComboBox::setModel(QAbstractItemModel* _Model, const QString& _NullLabel, int _ModelColumn)
{
	QComboBox::setModel(_Model);
	setModelColumn(_ModelColumn);
	addItem(_NullLabel, QVariant());
}

// ____________________________________________________________________________________
//
// Class WPrintWorkflowRecord
// ____________________________________________________________________________________


QWidget* WPrintWorkflowRecord::newLookUpCombo(const QString& _LookupTable, const QString& _LookupCol, const QString& _DisplayCol, 
															 const QString& _SourceCol)
{
	QSqlTableModel* LUModel = new QSqlTableModel(this); 
	LUModel->setTable(_LookupTable);
	LUModel->select();
	RefColComboBox* PriorityEdit = new RefColComboBox(LUModel->record().indexOf(_LookupCol), this);
	PriorityEdit->setModel(LUModel, tr("All"), LUModel->record().indexOf(_DisplayCol));
	Mapper->addMapping(PriorityEdit, Model->record().indexOf(_SourceCol));
	return PriorityEdit;
}

void WPrintWorkflowRecord::createWidget()
{
	QGridLayout* MLayout = new QGridLayout(this);
	
	
	MLayout->addWidget(createHeaderLabel("products_ref"), MLayout->rowCount(), 0);
	MLayout->addWidget(newLookUpCombo("products", "ref", "description", "products_ref"), 
							 MLayout->rowCount(), 0);

	MLayout->addWidget(createHeaderLabel("formats_idformats"), MLayout->rowCount(), 0);
	MLayout->addWidget(newLookUpCombo("formats", "idformats", "description", "formats_idformats"),
							 MLayout->rowCount(), 0);

	
	MLayout->addWidget(createHeaderLabel("senderid"), MLayout->rowCount(), 0);
	MLayout->addWidget(createEditor("senderid"), MLayout->rowCount(), 0);

	QLabel* LabArrow = new QLabel(this);
	LabArrow->setPixmap(QPixmap(":/images/editredo.png"));
	MLayout->addWidget(LabArrow, 0, 1, MLayout->rowCount(), 1);
	MLayout->addWidget(createHeaderLabel("printers_idprinters"), 0, 2, MLayout->rowCount(), 1);
	MLayout->addWidget(newLookUpCombo("printers", "idprinters", "name", "printers_idprinters"), 
							 0, 3, MLayout->rowCount(), 1);
}

WPrintWorkflowRecord::WPrintWorkflowRecord(FRecDialog* _Parent)
: FRecordWidget(_Parent->model(), _Parent)
{

}
// ____________________________________________________________________________________
//
// Class SqlLookupTableModel
// ____________________________________________________________________________________

SqlLookupTableModel::SqlLookupTableModel(QObject* _Parent, QSqlDatabase _Db) : 
		QSqlTableModel(_Parent, _Db)
{
}

QVariant SqlLookupTableModel::data(const QModelIndex& _Item, int _Role ) const
{
	QVariant Res;
	if (Lookups.contains(record().fieldName(_Item.column())) && _Role == Qt::DisplayRole)
	{
		LookUp CLookup = Lookups[record().fieldName(_Item.column())];
		FSqlQuery Query;
		Query.prepare(QString("SELECT %1 FROM %2 WHERE %3=:%3").arg(CLookup.displayCol()).arg(CLookup.lookupTable()).arg(CLookup.lookupCol()));
		Query.bindValue(":" + CLookup.lookupCol() , QSqlTableModel::data(_Item, _Role));
		Query.exec();
		if (Query.next())
			Res = Query.value(0);
	}
	else 
	{
		if (Icons.contains(record().fieldName(_Item.column())) && _Role == Qt::DecorationRole)
			Res = Icons[record().fieldName(_Item.column())];
		else 
			Res = QSqlTableModel::data(_Item, _Role);
	}
	
	if (Res.isNull() && _Role == Qt::DisplayRole)
		Res = NullLabel;
	
	return Res;
}

void SqlLookupTableModel::addLookup(const QString& _SourceField, const LookUp& _Lookup)
{
	Lookups.insert(_SourceField, _Lookup); 
}

void SqlLookupTableModel::addIcon(const QString& _FieldName, const QIcon& _Icon)
{
	Icons.insert(_FieldName, _Icon); 
}

// ____________________________________________________________________________________
//
// Class WPrintWorkflowManager
// ____________________________________________________________________________________

WPrintWorkflowManager::WPrintWorkflowManager(QWidget* parent): QWidget(parent)
{
	
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
 	
	SqlLookupTableModel* RecModel = new SqlLookupTableModel(this);
	RecModel->setNullLabel(tr("All"));
	RecModel->addLookup("printers_idprinters", SqlLookupTableModel::LookUp("printers", "idprinters", "name"));
	RecModel->addLookup("products_ref", SqlLookupTableModel::LookUp("products", "ref", "description"));
	RecModel->addLookup("formats_idformats", SqlLookupTableModel::LookUp("formats", "idformats", "description"));
	RecModel->addIcon("printers_idprinters", QIcon(":/images/printers.png"));
	
	
	FGenManager* MManager = new FGenManager(RecModel, "printworkflows", this, 
		"products_ref,formats_idformats,senderid,printers_idprinters", true);

	
	RecModel->setSort(RecModel->record().indexOf("idworkflows"), Qt::AscendingOrder);
	RecModel->select();//To Enable setSort.
	
	setWindowTitle(MManager->windowTitle());
	MLayout->addWidget(MManager);
	
  	FRecDialog* MRecDialog = new FRecDialog(RecModel, MManager->tableManager());
  	MRecDialog->setWindowTitle(windowTitle() + tr("<Record>"));
  	WPrintWorkflowRecord* MRecWidget = new WPrintWorkflowRecord(MRecDialog);
  	MRecDialog->setMainWidget(MRecWidget);
	//MManager->tableManager()->enableSaveState("printworkflowsmanager");
	MManager->tableManager()->setRecordWidget(MRecDialog);
	MManager->tableManager()->setEditable(false);
	MManager->tableManager()->setSearchEnabled(false);
}
