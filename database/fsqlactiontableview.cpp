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

#include "fsqlactiontableview.h"
#include "fsqlrelationaltablemodel.h"
#include "fsqlsearchwidget.h"
#include <QSqlError>
#include <QItemSelectionModel>
#include <QAbstractItemView>

void FSqlActionTableView::init()
{
/*	if (Model)
	{
		if (FSqlRelationalTableModel* FModel = qobject_cast<FSqlRelationalTableModel*>(Model))
			   connect(FModel, SIGNAL(errorOnSubmit(const QSqlError&)), this, SLOT(slotErrorOnSubmit(const QSqlError& )));
	}*/
}

/*!
	Per un bon control d'errors _Model ha de ser FSqlRelationalTableModel.
*/
FSqlActionTableView::FSqlActionTableView(QSqlTableModel* _Model, QWidget* _Parent): FActionTableView(_Model, _Parent),
		Model(_Model), Inserting(false)
{
	init();	
}

FSqlActionTableView::FSqlActionTableView(FSqlSearchWidget* _SearchWidget, QWidget* _Parent): FActionTableView(_SearchWidget,
	_Parent), Model(0), Inserting(false)
{
	Model = qobject_cast<QSqlTableModel*>(_SearchWidget->model());
	init();
}

/*!
	\return true si s'ha acceptat la operació.
*/
bool FSqlActionTableView::execDialog()
{
	bool Cancelled = false;
	bool Submitted = false;
	bool Res = false;
	while (!Submitted && !Cancelled)
	{ 
		Cancelled = RecDialog->exec() != QDialog::Accepted;
		if (!Cancelled)
		{
			//Submitted = Model->QSqlTableModel::submitAll();
			Submitted = Model->submitAll();
			if (!Submitted)
				emit submitError(Model->lastError());	
			else 
				Res = true;
		}
		else
			Model->revertAll();
	}
	
	return Res;
}

/*!
	Si hi ha diàleg, l'acceptació  
*/

void FSqlActionTableView::insertRow(int _Row)
{
	if (Model)
	{
		if (RecDialog)
		{
			//FActionTableView::insertRow();
			QSqlTableModel::EditStrategy LStrategy = Model->editStrategy();
			Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
			Model->insertRows(_Row,1);
			emit rowInserted(_Row);
			if (execDialog())
				emit afterInsertRow(_Row);
			Model->setEditStrategy(LStrategy);
		}
		else
		{	
			if (Inserting)
				commit();
			Inserting = false;
			LastStrategy = Model->editStrategy();
			//!! El seguent comentari està En proves
			Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
			
			//FActionTableView::insertRow();
			Model->insertRows(_Row,1);
			//emit afterInsertRow(_Row);

			SearchWidget->view()->selectionModel()->setCurrentIndex(Model->index(_Row,0), 
				QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
			editCurrentRow(_Row);
			emit afterInsertRow(_Row);
		disconnect(SearchWidget->view()->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex& )), 
			this, SLOT(commitInsert()));
		connect(SearchWidget->view()->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex& )), 
			this, SLOT(commitInsert()));
			
			Inserting = true;
		}		
	}	
	//return FActionTableView::insertRow();
}

void FSqlActionTableView::editCurrentRow(int _Index)
{
	if (RecDialog)
	{
		QSqlTableModel::EditStrategy LStrategy = Model->editStrategy();
		Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		if (execDialog())
			emit afterEditRow(_Index);
		Model->setEditStrategy(LStrategy);
	}
	else 
	{
		if (Inserting)
			commit();
		FActionTableView::editCurrentRow(_Index);
	}
}

void FSqlActionTableView::removeCurrentRow(int _Index)
{
	FActionTableView::removeCurrentRow(_Index);
	commit();
}

bool FSqlActionTableView::commit()
{
	bool Res = true;
	if (!Model->submitAll())
	{
		Res = false;
		QSqlError Error = Model->lastError();
		//Model->revertAll();
				//Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
				//Model->revertAll();
		emit submitError(Error);	
	}
	if (Inserting)
		Model->setEditStrategy(LastStrategy);
	//Model->setEditStrategy(QSqlTableModel::OnRowChange);
	return Res;
}

bool FSqlActionTableView::isInserting() const
{
	return Inserting || state() == StateInserting;
}


void FSqlActionTableView::hideEvent( QHideEvent * _Event )
{
	FActionTableView::hideEvent(_Event);
	if (Inserting)
		commitInsert();
}

// void FSqlActionTableView::focusOutEvent(QFocusEvent* _Event)
// {
// 	FActionTableView::focusOutEvent(_Event);
// 	qDebug("FSqlActionTableView::focusOutEvent");
// 	if (Inserting)
// 		commitInsert();
// }

void FSqlActionTableView::commitInsert()
{
	if (Inserting)
	{
		commit(); //Sinó el primer delete després de un insert falla !
		emit afterInsertRow(0);
		Inserting = false;
	}
}

