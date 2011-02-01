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

#include "ftablemanager.h"
#include <QLayout>
#include <QAbstractItemView>
#include <QTableView>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "fsqlrelationaltablemodel.h"
#include "fsqlsearchwidget.h" 
#include "fsqlrelationaldelegate.h"
#include "fsqlactiontableview.h"
#include "fstdactions.h"
#include "smessagebox.h"
#include "ftsdialog.h"
#include "frecdialog.h"
#include "frecordwidget.h"


void FTableManager::init(QSqlTableModel* _Model, const QString& _Columns, bool _SortHeader)
{
	//_Model->QSqlRelationalTableModel::setEditStrategy(QSqlTableModel::OnRowChange);
	_Model->QSqlTableModel::setEditStrategy(QSqlTableModel::OnFieldChange);
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	MainLayout->setMargin(0);
	VarSearchWidget = new FSqlSearchWidget(_Model);
	
	VarSearchWidget->setSearchColumn(0);
	if (!_Columns.isEmpty())
		VarSearchWidget->setColumns(_Columns, _SortHeader);
	resizeColumns();
	VarView = new FSqlActionTableView(VarSearchWidget, this);

	VarView->addAction(new FEditAddAction(this));
	EditAction = new FEditAction(this);
	VarView->addAction(EditAction);
	VarView->addAction(new FEditDeleteAction(this));
	MainLayout->addWidget(VarView);
	setEditable(true);

	connect(VarView, SIGNAL(submitError(const QSqlError&)), this, SLOT(varViewError(const QSqlError&)));
	connect(VarView, SIGNAL(beforeEditRow(int , bool& )), this, SLOT(selectFormRow(int)));
	connect(VarView, SIGNAL(rowInserted(int)), this, SLOT(selectFormRow(int)));
	connect(VarView, SIGNAL(afterEditRow(int)), this, SLOT(resizeColumns())); 
	connect(VarView, SIGNAL(afterInsertRow(int)), this, SLOT(resizeColumns())); 
	connect(VarView, SIGNAL(afterRemoveRow(int)), this, SLOT(resizeColumns())); 
	connect(VarView, SIGNAL(beforeRemoveRow(int , bool& )), this, SLOT(beforeRemoveRow(int, bool& )));
	connect(VarView, SIGNAL(afterRemoveRow(int)), this, SLOT(afterRemoveRow())); 
}

FTableManager::FTableManager(QSqlTableModel* _Model, QWidget* _Parent, const QString& _Columns, 
bool _SortHeader)
	: QWidget(_Parent), MRecordWidget(0), Model(_Model), MRecDialog(0)
{
	init(_Model, _Columns, _SortHeader);
}

FTableManager::FTableManager(QSqlRelationalTableModel* _Model, QWidget* _Parent, const QString& _Columns, 
bool _SortHeader)
 : QWidget(_Parent), MRecordWidget(0), Model(_Model), MRecDialog(0)
{
	init(_Model, _Columns, _SortHeader);
	VarSearchWidget->view()->setItemDelegate(new QSqlRelationalDelegate(this));
}

void FTableManager::setSearchEnabled(bool _Enabled)
{
		static_cast<QTableView*>(VarSearchWidget->view())->setSortingEnabled(_Enabled);
		VarSearchWidget->searchFrame()->setVisible(_Enabled);
}

void FTableManager::setRecordWidget(FRecordWidget* _RecordWidget)
{
	if (!MRecDialog)
	{
		MRecDialog = new FTSDialog(this);
		MRecDialog->addAction(new FAcceptAction(this)); 
		MRecDialog->addAction(new FCancelAction(this)); 
		VarView->setRecordDialog(MRecDialog);
	}
	MRecDialog->setMainWidget(_RecordWidget);
	connect(VarView, SIGNAL(beforeInsertRow(int , bool& )),
				MRecordWidget, SLOT(clearEditors()));
	MRecordWidget = _RecordWidget;
}

void FTableManager::updateLookups()
{
	if (MRecordWidget)
		MRecordWidget->updateLookups();
}

void FTableManager::setRecordWidget(FRecDialog* _RecordDialog)
{
	VarView->setRecordDialog(_RecordDialog);
	MRecordWidget = qobject_cast<FRecordWidget*>(_RecordDialog->mainWidget());
	if (MRecordWidget)
		connect(VarView, SIGNAL(beforeInsertRow(int , bool& )),
					MRecordWidget, SLOT(clearEditors()));
}

void FTableManager::setEditable(bool _Editable)
{
	

	if (_Editable && !VarSearchWidget->editable())
	{
		disconnect(VarSearchWidget->view(), SIGNAL(doubleClicked(const QModelIndex& )), 
			EditAction, SLOT(trigger()));
	}else 
	{
		if (!_Editable && VarSearchWidget->editable())
		{
			connect(VarSearchWidget->view(), SIGNAL(doubleClicked(const QModelIndex& )), 
				EditAction, SLOT(trigger()));
		}
	}	
	VarSearchWidget->setEditable(_Editable);
}

void FTableManager::setPrimeInsertRecord(const QSqlRecord& _Record)
{
	if (MRecordWidget)
		MRecordWidget->setPrimeInsertRecord(_Record);
}

void FTableManager::editRow(int _Row)
{
	selectFormRow(_Row);
	VarView->editCurrentRow(_Row);
}

void FTableManager::showEvent(QShowEvent* /*event */)
{
	//Model->select();
	//!!!!!!!!!!!!!!!!! Les combos de productes no s'actualitzen !!!!!!!
}

void FTableManager::selectFormRow(int _Index)
{
	if (MRecordWidget)
		MRecordWidget->mapper()->setCurrentIndex(_Index);
}

void FTableManager::resizeColumns()
{
	if (QTableView* TView = qobject_cast<QTableView*>(VarSearchWidget->view()))
		TView->resizeColumnsToContents();
}

void FTableManager::varViewError(const QSqlError& _Error)
{
	SMessageBox::critical(this, tr("Error modifying table"), _Error.text());
}

void FTableManager::beforeRemoveRow(int _Row, bool& _PerformOp)
{
	//!!!!! Flam Necessita un messagebox sense touchscreen
	if (SMessageBox::question(this, tr("Data management"), 
		tr("Are you sure you want to delete selected record ?") ) == QMessageBox::Yes)
	{
		LastRemovedRow = Model->record(_Row);
	}
	else 
	{
		_PerformOp = false;
		LastRemovedRow = QSqlRecord();
	}
}

void FTableManager::afterRemoveRow()
{
	emit rowRemoved(LastRemovedRow);	
}

FSqlSearchWidget* FTableManager::searchWidget() const
{
	return VarSearchWidget;
}

FSqlActionTableView* FTableManager::actionTableView() const
{
	return VarView;
}

