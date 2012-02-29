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
#include "fsqlsearchwidget.h"

#include "ftableview.h"
#include "fsqlrelationaltablemodel.h"
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QSqlTableModel>
#include <QStringList>
#include <QSqlField>
#include <QHeaderView>
#include <QLineEdit>

#include "ftableview.h"
#include "fsqlmodelviewutils.h"


QAbstractItemView* FSqlSearchWidget::createView()
{
	return new FTableView;
}

QSqlTableModel* FSqlSearchWidget::createModel(const QString& _TableName, const QString& _Filter, QObject* _Parent, 
															 QSqlDatabase _Database)
{
	Model = new QSqlTableModel(_Parent, _Database);
	Model->setTable(_TableName);
	Model->setFilter(_Filter);
	return Model;
}

void FSqlSearchWidget::init()
{
	ColumnToContentSize = true;
	if (QTableView* TView = qobject_cast<QTableView*>(view()))
	{
		connect(TView->horizontalHeader(), SIGNAL(sectionClicked(int)),
			this, SLOT(slotSetSearchColumn(int)));
		TView->verticalHeader()->hide();
	}
	
	setSearchColumn(0);
	SearchMode = FilterSearchMode;
}


/*!
	Crea un model i un viewer per defecte. El model cont� les dades de _TableName filtrades per _Filter.
	Nom�s crea el model, no l'emplena. Per a emplenar-lo hem de cridar a select().
*/

FSqlSearchWidget::FSqlSearchWidget(const QString& _TableName, const QString& _Filter, QWidget* _Parent, QSqlDatabase _Database) 
	: FSearchWidget(createView(), createModel(_TableName, _Filter, 0, _Database), _Parent)
{
	view()->setParent(this);
	init();
}

/*!
	Permet passar-li en viewer de dades.
*/

FSqlSearchWidget::FSqlSearchWidget(QTableView* _View, const QString& _TableName, const QString& _Filter, QWidget* _Parent, QSqlDatabase _Database)
	: FSearchWidget(_View, createModel(_TableName, _Filter, 0, _Database), _Parent)
{
	init();
}


/*!
	Crea un viewer per defecte i permet passar-li el model.
*/

FSqlSearchWidget::FSqlSearchWidget(QSqlTableModel* _Model, QWidget* _Parent)
	: FSearchWidget(createView(), _Model, _Parent), Model(_Model)
{
	init();
}

FSqlSearchWidget::~FSqlSearchWidget()
{
	if (Model)
		delete Model;
}


/*!
	Permet passar-li el model i el viewer.
*/

FSqlSearchWidget::FSqlSearchWidget(QTableView* _View, QSqlTableModel* _Model, QWidget* _Parent)
	: FSearchWidget(_View, _Model, _Parent), Model(_Model)
{
	init();
}

/*!
	Assigna les columnes que seran visibles.
*/

void FSqlSearchWidget::setColumns(const QStringList& _Cols, bool _Sort)
{
 	#if QT_VERSION >= 0x040100
		if (QTableView* TView = qobject_cast<QTableView*>(view()))
		{
			if (!_Cols.empty())
			{
				FSqlModelViewUtils::setColumns(TView, _Cols);
				setSearchColumn(FSqlModelViewUtils::indexOf(Model, _Cols.first()));
			}
		}
		else 
			qWarning("FLAM WARNING: Could not set columns with non QTableView Object");
	#else 
		//!!!!!!!! Problemill amb les qt 4.0.*
		if (model()->inherits("QSqlRelationalTableModel"))
			FSqlModelViewUtils::setColumns(static_cast<QTableView*>(view()), _Cols);
		else 
			FSqlModelViewUtils::setColumns(static_cast<QSqlTableModel*>(model()), _Cols);
	#endif 
	if (_Sort) 
		FSqlModelViewUtils::sortColumns(static_cast<QTableView*>(view()), _Cols);
		
}

void FSqlSearchWidget::setColumns(const QString& _Cols, bool _Sort)
{
 	#if QT_VERSION >= 0x040100
		QString Cols = _Cols;
		setColumns(Cols.replace(" ", "").split(","), _Sort);
		//FSqlModelViewUtils::setColumns(static_cast<QTableView*>(view()), _Cols);
	#else 
		//!!!!!!!! Problemill amb les qt 4.0.*
		if (model()->inherits("QSqlRelationalTableModel"))
			FSqlModelViewUtils::setColumns(static_cast<QTableView*>(view()), _Cols);
		else 
			FSqlModelViewUtils::setColumns(static_cast<QSqlTableModel*>(model()), _Cols);
	#endif 
	if (_Sort) 
		FSqlModelViewUtils::sortColumns(static_cast<QTableView*>(view()), _Cols);
}

/*! 
	Crida a select del seu model 
*/
void FSqlSearchWidget::select()
{
	static_cast<QSqlTableModel*>(model())->select();
	if (ColumnToContentSize)
	{
		if (QTableView* MView = qobject_cast<QTableView*>(view()))
		{
			QHeaderView* HHeader = MView->horizontalHeader();
			for (int Vfor=0; Vfor< HHeader->count(); Vfor++)
				MView->resizeColumnToContents(Vfor);
		}
	}	
}

//! Assignaci� manual de columna de cerca.
void FSqlSearchWidget::setSearchColumn(int _Column)
{
	if (model())
	{
		slotSetSearchColumn(_Column);
		if (QTableView* MView = qobject_cast<QTableView*>(view()))
			MView->sortByColumn(_Column);
	}
}

/*! Si _Active == true cada cop que es necessita fer un select es redimensionaran 
	autom�ticament les columnes a la mida dels continguts.
	Per defecte �s true.
*/
void FSqlSearchWidget::setColumnToContentSize(bool _Active)
{
	ColumnToContentSize = _Active;
}

/*!
	\sa setColumnToContentSize
*/
bool FSqlSearchWidget::columnToContentSize() const 
{
	return ColumnToContentSize;
}

void FSqlSearchWidget::setSearchMode(FSqlSearchWidget::EnSearchMode _SearchMode)
{
	SearchMode = _SearchMode;
}

FSqlSearchWidget::EnSearchMode FSqlSearchWidget::searchMode() const
{
	return SearchMode;
}

QSqlRecord FSqlSearchWidget::currentRecord() const
{
	QSqlRecord Res;
	QModelIndex Index = view()->currentIndex();
	if (Index.isValid())
		Res = Model->record(Index.row());
	return Res;
}

void FSqlSearchWidget::slotSetSearchColumn(int _Column)
{
	if (Model)
	{
		CurrentSearchColumn = _Column;
		setSearchStart(Model->index(0,_Column));
		setSearchLabel(Model->headerData(_Column, Qt::Horizontal).toString());
		LEditSearch->setFocus();
	}
}


void FSqlSearchWidget::search(const QString& _SearchText)
{
	if (SearchMode == SelectionSearchMode)
	{
		FSearchWidget::search(_SearchText);
	}
	else 
	{
		QString OldFilter = Model->filter().replace(CurrFilter, "");
		if (!_SearchText.isEmpty())
		{
			if (searchStart().isValid())
			{
				QString FieldName = Model->record().fieldName(searchStart().column());
				//Si el model �s relacional:
				if (QSqlRelationalTableModel* RelModel = qobject_cast<QSqlRelationalTableModel*>(Model))	
				{
					QSqlRelation Rel = RelModel->relation(searchStart().column());
					if (Rel.isValid())
						FieldName = Rel.tableName() + "." + FieldName;
					else 
						FieldName = RelModel->tableName() + "." + FieldName;
				}
				QString Filter = FieldName + " like '" + _SearchText + "%'";
				CurrFilter = Filter;
				if (!OldFilter.isEmpty())
				{
					CurrFilter = " AND " +  Filter;
					Filter = OldFilter + CurrFilter;
				}
				Model->setFilter(Filter );
				//qDebug(Model->filter().replace("%","X").toLatin1());
			}
		}
		else 
			Model->setFilter(OldFilter);

		Model->select();
		selectFirst();
	}	
}

void FSqlSearchWidget::nextSearchColumn()
{
	if (QTableView* TView = qobject_cast<QTableView*>(view()))
	{
		int CurrCol = CurrentSearchColumn + 1;
		bool Found = false;
		while(CurrCol < Model->rowCount() && !Found)
		{
			Found = !TView->isColumnHidden(CurrCol);
			if (!Found)
				CurrCol++;
		}
		if (Found)
			setSearchColumn(CurrCol);
	}
}

void FSqlSearchWidget::previousSearchColumn()
{
	if (QTableView* TView = qobject_cast<QTableView*>(view()))
	{
		int CurrCol = CurrentSearchColumn - 1;
		bool Found = false;
		while(CurrCol >= 0 && !Found)
		{
			Found = !TView->isColumnHidden(CurrCol);
			if (!Found)
				CurrCol--;
		}
		if (Found)
			setSearchColumn(CurrCol);
	}

}
