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
#include "fsearchwidget.h"

#include <QLayout>
#include <QFrame>
#include <QIcon>
#include <QAbstractItemView>
#include <QLabel>
#include <QTableView>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QKeyEvent> 

// _________________________________________________________________________*/
//
//	Class FSearchWidget
// _________________________________________________________________________*/


QFSLineEdit::QFSLineEdit(QWidget* _Parent) : QLineEdit(_Parent)
{}

void QFSLineEdit::keyPressEvent(QKeyEvent * _Event)
{
	if (_Event->key() == Qt::Key_Up)
		emit keyUp();
	if (_Event->key() == Qt::Key_Down)
		emit keyDown();
	if (_Event->key() == Qt::Key_PageDown)
		emit pageDown();
	if (_Event->key() == Qt::Key_PageUp)
		emit pageUp();
	if (_Event->key() == Qt::Key_Right)
		emit keyRight();
	if (_Event->key() == Qt::Key_Left)
		emit keyLeft();


	QLineEdit::keyPressEvent(_Event);
}

// _________________________________________________________________________*/
//
//	Class FSearchWidget
// _________________________________________________________________________*/

void FSearchWidget::createControls(QAbstractItemView* _View)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
	LEditFrame = new QFrame(this);
	QHBoxLayout* LabLayout = new QHBoxLayout(LEditFrame);
	LabLayout->setMargin(2);
	MLayout->addWidget(LEditFrame);

	QLabel* LabSearchIco = new QLabel(LEditFrame);	
	LabSearchIco->setPixmap(QPixmap(":/st/smallsearch.png"));
	LabLayout->addWidget(LabSearchIco); 
	LabSearch = new QLabel(LEditFrame);	
	QFont LSFont = LabSearch->font();
	LSFont.setBold(true); 
	LabSearch->setFont(LSFont);
	LabLayout->addWidget(LabSearch); 
	setSearchLabel("");
	
	LEditSearch = new QFSLineEdit(LEditFrame);
	LabLayout->addWidget(LEditSearch); 
	connect(LEditSearch, SIGNAL(textChanged(const QString&)), this, SLOT(search(const QString&)));
	
	View = _View; 
	if (!View) 
	{
		QTableView* TView = new QTableView(this); 
		TView->setModel(Model);
		TView->verticalHeader()->hide();
		TView->horizontalHeader()->setStretchLastSection(true);
		TView->setSelectionBehavior(QAbstractItemView::SelectRows);
		TView->setAlternatingRowColors(true);
		View = TView;
	}
	else 
		View->setParent(this);

	connect(LEditSearch, SIGNAL(keyDown()), this, SLOT(selectNext()));
	connect(LEditSearch, SIGNAL(keyUp()), this, SLOT(selectPrevious()));
	connect(LEditSearch, SIGNAL(pageDown()), this, SLOT(scrollDown()));
	connect(LEditSearch, SIGNAL(pageUp()), this, SLOT(scrollUp()));
	connect(LEditSearch, SIGNAL(keyRight()), this, SLOT(nextSearchColumn()));
	connect(LEditSearch, SIGNAL(keyLeft()), this, SLOT(previousSearchColumn()));
	
	//No permetre edicions.
	setEditable(false);
	//connect(View, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(accept()));
	
	MLayout->addWidget(View); 
	setFocusProxy(LEditSearch);
}


/*!
	Crea un QTableView.
*/

FSearchWidget::FSearchWidget(QAbstractTableModel* _Model, QWidget* _Parent) : QWidget(_Parent), Model(_Model)
{
	createControls();
	setSearchDefaults();
}
	
FSearchWidget::FSearchWidget(QAbstractItemView* _View, QAbstractItemModel* _Model, QWidget* _Parent) 
	: QWidget(_Parent), Model(_Model)
{
	createControls(_View);
	if (_Model && _View)
	{
		_Model->setParent(this);
		_View->setModel(_Model);
	}
	setSearchDefaults();
}

void FSearchWidget::setSearchDefaults()
{
	if (Model)
		SearchStart = Model->index(0,0);
	SearchRole = Qt::DisplayRole;
	SearchHits = 10;
	SearchMatchFlags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap);
	//SearchMatchFlags = Qt::MatchFlags( Qt::MatchExactly | Qt::MatchWrap);
}

/*!
	Indica per on ha de començar a buscar.
	Per defecte la primera fila i columna.
	(Buscarà per files a la columna indicada a _Start. Començant per la fila 
	indicada a _Start).
	\sa QAbstractItemModel::match
*/

void FSearchWidget::setSearchStart(const QModelIndex& _Start)
{
	SearchStart = _Start;
}

QModelIndex FSearchWidget::searchStart() const
{
	return SearchStart;
}

/*!
	Indica a quin role del model ha de buscar.
	\sa QAbstractItemModel::match
*/

void FSearchWidget::setSearchRole(int _Role)
{
	SearchRole = _Role;
}

int FSearchWidget::searchRole() const
{
	return SearchRole;
}

/*!
	Indica el nombre d'incidències que s'han de complir perque es pari la cerca.
	Per defecte 10.
	\sa QAbstractItemModel::match
*/

void FSearchWidget::setSearchHits(int _Hits)
{
	SearchHits = _Hits;
}

int FSearchWidget::searchHits() const
{
	return SearchHits;
}

Qt::MatchFlags FSearchWidget::searchMatchFlags() const
{
	return SearchMatchFlags;
}

void FSearchWidget::setSearchMatchFlags(Qt::MatchFlags _Flags)
{
	SearchMatchFlags = _Flags;
}

void FSearchWidget::setSearchLabel(const QString& _Label)
{
	LabSearch->setText(_Label + " ?");
}

/*!
	Indica si el viewer es podrà editar o no.
*/
void FSearchWidget::setEditable(bool _Editable)
{
	if (_Editable)
		View->setEditTriggers(QAbstractItemView::DoubleClicked);
	else 
		View->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

bool FSearchWidget::editable() const
{
	return View->editTriggers() == QAbstractItemView::DoubleClicked;
}


void FSearchWidget::showEvent( QShowEvent *  )
{
	//LEditSearch->setFocus();
}


void FSearchWidget::search(const QString& _SearchText)
{
	QModelIndexList FoundIndx;
	if (!_SearchText.isEmpty())
	{
		FoundIndx = Model->match(SearchStart, SearchRole, _SearchText, SearchHits, SearchMatchFlags);
	}
	else 
		View->selectionModel()->clear();	
	
	if (!FoundIndx.isEmpty())
	{
		View->selectionModel()->clear();
		View->setCurrentIndex(FoundIndx.first());
		
		QModelIndexList::const_iterator it;
		for (it = FoundIndx.begin(); it != FoundIndx.end(); ++it)
			View->selectionModel()->select(*it, QItemSelectionModel::Select | QItemSelectionModel::Rows);
		
	}
	else 
		View->selectionModel()->clear();	

}

void FSearchWidget::showSearchInput(bool _Value)
{
	LEditFrame->setVisible(_Value);
}


void FSearchWidget::scrollDown()
{
	int HeightDec = 0;
	//! L'ultim element visible
	QModelIndex Indx = View->indexAt(QPoint(0, View->viewport()->height() - HeightDec));
	if (Indx.isValid())
		View->scrollTo(Model->index(Indx.row() + 1, Indx.column()));
	else 
		View->scrollToBottom();
	
}

void FSearchWidget::scrollUp()
{
	//! El primer element visible
	QModelIndex Indx = View->indexAt(QPoint(0, 0));
	if (Indx.isValid() && Indx.row() > 0 )
		View->scrollTo(Model->index(Indx.row() - 1, Indx.column()));
	else 
		View->scrollToTop();
	
}

void FSearchWidget::selectFirst()
{
	if (QItemSelectionModel* SelModel = View->selectionModel())
	{
		if (Model) //Defensiva
			SelModel->setCurrentIndex(Model->index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
	}
}

void FSearchWidget::selectNext()
{
	if (QItemSelectionModel* SelModel = View->selectionModel())
	{
		QModelIndex CIndex = Model->index(0,0);
		QModelIndex SCIndex = SelModel->currentIndex();
		if (SCIndex.isValid())
		{
			QModelIndex NewIndex = Model->index(SCIndex.row() + 1, SCIndex.column());
			if (NewIndex.isValid())
				CIndex = NewIndex;
		} 
		SelModel->setCurrentIndex(CIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
		
		//!!! Hi ha views que no fan scroll, no se quin pot ser el problema... ::exec de FMVUtils per ex no fa show i un FSqlSearchLineEdit amb una searchAction si... (Veure SSales2).
		QModelIndex Indx = View->indexAt(QPoint(0, 0));
		if (CIndex.row() != 0) //Si no al canviar de l'ultim al primer no es veu el primer.
			View->scrollTo(Model->index(CIndex.row() + 1, Indx.column()));
		else
			View->scrollTo(Model->index(CIndex.row(), Indx.column()));
		
	}
}

void FSearchWidget::selectPrevious()
{
	if (QItemSelectionModel* SelModel = View->selectionModel())
	{
		QModelIndex CIndex = Model->index(Model->rowCount()-1,0);
		QModelIndex SCIndex = SelModel->currentIndex();
		if (SCIndex.isValid())
		{
			QModelIndex NewIndex = Model->index(SCIndex.row() - 1, SCIndex.column());
			if (NewIndex.isValid())
				CIndex = NewIndex;
		} 
		SelModel->setCurrentIndex(CIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
		QModelIndex Indx = View->indexAt(QPoint(0, 0));
		if (SCIndex.row() != 0) //Si no al canviar del primer a l'últim no es veu el primer.
			View->scrollTo(Model->index(CIndex.row() - 1, Indx.column()));
		else
			View->scrollTo(Model->index(CIndex.row(), Indx.column()));
	}
}

void FSearchWidget::nextSearchColumn()
{
//It does nothing.
}

void FSearchWidget::previousSearchColumn()
{
//It does nothing.
}
