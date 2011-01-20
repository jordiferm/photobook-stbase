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
#include "fsearchdialog.h"
#include <QPushButton>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QDesktopWidget>
#include "fsearchwidget.h"

void FSearchDialog::init(FSearchWidget* _SearchWidget)
{
	connect(_SearchWidget->view(), SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(accept()));
	connect(_SearchWidget->lineEditSearch(), SIGNAL(returnPressed()), this, SLOT(accept()));
}


/*!
	Crea un QTableView.
*/

FSearchDialog::FSearchDialog(FSearchWidget* _SearchWidget, QWidget* _Parent) : FDialog(_Parent),
	FSearchDialogBase(_SearchWidget)
{
	init(_SearchWidget);
}


int FSearchDialog::exec()
{
	//show();
	int Width = 750;
	int Height = qMin(500, searchWidget()->model()->rowCount() * 80);
	QDesktopWidget DWidget;
	QRect ScreenRect = DWidget.screenGeometry(this);
	setGeometry(ScreenRect.width() / 2 - Width / 2 , ScreenRect.height() / 2 - Height / 2, Width, Height );

	return FDialog::exec();
}

/*!
	\return Si el dialeg no s'accepta retornen un index null. 
	Sinó retorna l'index actual del view.
*/

QModelIndex FSearchDialog::exec(QAbstractTableModel* _Model, QWidget* _Parent)
{
	QModelIndex Res;
	FSearchWidget* SWidg = new FSearchWidget(_Model, _Parent); 
	FSearchDialog SDialog(SWidg, _Parent); 
	//SWidg->setParent(&SDialog);
	SDialog.setMainWidget(SWidg);
	if (SDialog.FSearchDialog::exec() == QDialog::Accepted)
		Res = SDialog.searchWidget()->view()->currentIndex();
	
	return Res;
}

QModelIndex FSearchDialog::exec(QAbstractItemView* _View, QAbstractItemModel* _Model, QWidget* _Parent)
{
	FSearchWidget* SWidg = new FSearchWidget(_View, _Model, _Parent); 
	FSearchDialog SDialog(SWidg, _Parent); 
	SWidg->setParent(&SDialog);
	SDialog.FSearchDialog::exec();
	return SDialog.searchWidget()->view()->currentIndex();
}

