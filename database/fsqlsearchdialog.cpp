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

#include "fsqlsearchdialog.h"
#include "ftableview.h"
#include "fsqlrelationaltablemodel.h"
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QSqlTableModel>

#include "fsqlsearchwidget.h"

/*!
	Crea un model a partir de un FSqlSearchWidget.
*/

FSqlSearchDialog::FSqlSearchDialog(FSqlSearchWidget* _SqlSearchWidget, QWidget* _Parent) 
	: FSearchDialog(_SqlSearchWidget, _Parent)
{
	QAction* AcceptAction = new QAction(QIcon(":/st/accept.png"), tr("Accept"), this);
	connect(AcceptAction, SIGNAL(triggered()), this, SLOT(accept()));
	QAction* CancelAction = new QAction(QIcon(":/st/cancel.png"), tr("Cancel"), this);
	connect(CancelAction, SIGNAL(triggered()), this, SLOT(reject()));
	addAction(AcceptAction);
	addAction(CancelAction);
}


QSqlRecord FSqlSearchDialog::exec(const QString& _Sql, QWidget* _Parent, QSqlDatabase _Database)
{
	QSqlQueryModel* SqlModel = new QSqlQueryModel(_Parent);
	SqlModel->setQuery(_Sql, _Database);
	FSearchWidget* SWidget = new FSearchWidget(SqlModel); 
	
	FSearchDialog SDialog(SWidget, _Parent); 
	SDialog.setMainWidget(SWidget);
	SWidget->setParent(&SDialog);
	QSqlRecord Res;
	if (SDialog.FSearchDialog::exec() == QDialog::Accepted)
	{
		QModelIndex Index = SWidget->view()->currentIndex();
		if (Index.isValid())
			Res = static_cast<QSqlQueryModel*>(SWidget->model())->record(Index.row());
	}
	return Res;
}

/*!
	Retorna un QSqlRecord amb el registre seleccionat en cas de que s'accepti el dialeg.
	Sin� retorna un QSqlRecord empty. Tamb� retorna un QSqlRecord empty en cas de que no hi hagi seleccionat res 	
*/

QSqlRecord FSqlSearchDialog::exec(const QString& _TableName, const QString& _Columns, 
	const QString& _Filter, int _SortColumn, QWidget* _Parent, QSqlDatabase _Database )
{
	QSqlRecord Res;

	FSqlSearchWidget* SWidget = new FSqlSearchWidget(_TableName, _Filter, _Parent, _Database); 
	FSqlSearchDialog SDialog(SWidget, _Parent); 
	SDialog.setMainWidget(SWidget);
	SWidget->setParent(&SDialog);
	Res = exec(SDialog, _Columns, _SortColumn);
	return Res;
}


QSqlRecord FSqlSearchDialog::exec(FSqlSearchDialog& _Dialog, 
	const QString& _Columns, int _SortColumn)
{
	QSqlRecord Res;
	
	FSqlSearchWidget* SWidget = static_cast<FSqlSearchWidget*>(_Dialog.searchWidget()); 
	static_cast<QTableView*>(SWidget->view())->sortByColumn(_SortColumn);
	SWidget->setSearchColumn(_SortColumn);
	SWidget->setColumns(_Columns);
	SWidget->select();
	if (_Dialog.FSearchDialog::exec() == QDialog::Accepted)
	{
		QModelIndex Index = SWidget->view()->currentIndex();
		if (Index.isValid())
			Res = static_cast<QSqlTableModel*>(SWidget->model())->record(Index.row());
	}
	return Res;
}



