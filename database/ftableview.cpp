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

#include "ftableview.h"
#include <QSqlRelationalDelegate>
#include <QSqlError>
#include <QMetaObject>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QHeaderView>
#include "fsqlrelationaltablemodel.h"
#include "fsqlrelationaldelegate.h"


FTableView::FTableView(QWidget* _Parent)
 : QTableView(_Parent)
{
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
	horizontalHeader()->setSortIndicatorShown(true);
	horizontalHeader()->setMovable(true);
	#if QT_VERSION >= 0x040200
		setSortingEnabled(true);
	#else 
		connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));	
	#endif
}

void FTableView::setModel(FSqlRelationalTableModel* _Model)
{
	QTableView::setModel(_Model);
	connect(_Model, SIGNAL(errorOnSubmit(const QSqlError&)), this, SLOT(submitError(const QSqlError&)));
	
	//!!!! connect(_Model, SIGNAL(submitAllOnIndex(const QModelIndex& )), this, SLOT(setCurrentIndex(const QModelIndex&)));
	
/*	if (QString(itemDelegate()->metaObject()->className()) != "FSqlRelationalDelegate")
		setItemDelegate(new FSqlRelationalDelegate(this));*/
}

void FTableView::setModel(QAbstractItemModel * _Model)
{
	QTableView::setModel(_Model);
}


/*!
	La implementació per defecte mostra un dialeg amb l'error i recula ens canvis.
*/

void FTableView::submitError(const QSqlError& _Error)
{
	QMessageBox::information(this, "Error en operacio a BD:", _Error.text());
	if (model()->inherits("QSqlTableModel"))
		static_cast<QSqlTableModel*>(model())->revertAll();
}

void FTableView::headerClicked(int _LogicalIndex)
{
	sortByColumn(_LogicalIndex);
}

