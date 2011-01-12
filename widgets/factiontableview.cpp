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

#include "factiontableview.h"
#include <QLayout>
#include <QAction>
#include <QFrame>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QTableView>
#include <QItemSelectionModel>
#include <QHeaderView>

#include "sactionbuttonsframe.h"
#include "fsearchwidget.h"
#include "fstdactions.h"

void FActionTableView::init(FSearchWidget* _SearchWidget)
{
	SearchWidget = _SearchWidget;
	AddAction = 0; 
	EditAction = 0; 
	DeleteAction = 0; 

	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);

	MLayout->setMargin(0);
	MLayout->setSpacing(0);
		
	QFrame* MFrame = new QFrame(this);
	MLayout->addWidget(MFrame);
	QHBoxLayout* ActLayout = new QHBoxLayout(MFrame);
	MActionBBox = new SActionButtonsFrame(MFrame); 
	MActionBBox->setIconSize(QSize(32, 32));
	ActLayout->addWidget(MActionBBox);
	ActLayout->addItem(new QSpacerItem(10,0));
	ActLayout->setMargin(2);
	ActLayout->setSpacing(0);
	MActionBBox->layout()->setMargin(0);
	MFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken); 
	
	_SearchWidget->layout()->removeWidget(_SearchWidget->searchFrame());
	_SearchWidget->layout()->setMargin(0);
	_SearchWidget->layout()->setSpacing(0);
	_SearchWidget->searchFrame()->setParent(this);
	MLayout->addWidget(_SearchWidget);
	ActLayout->addWidget(_SearchWidget->searchFrame());
	ActLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	EditColumn = 0;
	if (_SearchWidget->view()) //Defensiva 
	{
		connect(_SearchWidget->view()->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
			this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}
}

FActionTableView::FActionTableView(QAbstractTableModel* _Model, QWidget* _Parent) : QWidget(_Parent), RecDialog(0), 
	 FAbstractActionContainer(this)
{
	init(new FSearchWidget(_Model));
	SearchWidget->setEditable(true);
	Model = _Model;
}

FActionTableView::FActionTableView(FSearchWidget* _SearchWidget, QWidget* _Parent) : QWidget(_Parent),
	Model(0), RecDialog(0), FAbstractActionContainer(this)
{
	init(_SearchWidget);
	Model = _SearchWidget->model();
}

FActionTableView::~FActionTableView()
{
}

void FActionTableView::setRecordDialog(QDialog* _RecDialog)
{
	RecDialog = _RecDialog;
}

void FActionTableView::addAction(QAction* _Action)
{
/*	_Action->setIconText("");
	_Action->setText("");*/
	MActionBBox->addAction(_Action);
	connect(_Action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
	if (FAction* Action = qobject_cast<FAction*>(_Action))
	{
		switch (Action->id())
		{
			case FAction::FEditAddActionId : 
				AddAction = Action;
			break;
			case FAction::FEditActionId : 
				EditAction = Action;
				Action->setEnabled(false);
			break;
			case FAction::FEditDeleteActionId : 
				DeleteAction = Action;
				Action->setEnabled(false);
			break;
		}
	}
	FAbstractActionContainer::addAction(_Action);
}

void FActionTableView::removeAction(int _ActionId)
{
	QAction* RAction = fAction(_ActionId); 
	if (RAction)
	{
		MActionBBox->removeAction(RAction); 
		FAbstractActionContainer::removeAction(_ActionId); 
	}
}


void FActionTableView::slotSelectionChanged(const QItemSelection& _Selected, const QItemSelection& _Deselected)
{
	bool ItemsSelected = _Selected.indexes().size() > 0;
	
	if (EditAction)
		EditAction->setEnabled(ItemsSelected);
	
	if (DeleteAction)
		DeleteAction->setEnabled(ItemsSelected);

	emit selectionChanged(_Selected, _Deselected);
}

void FActionTableView::actionTriggered()
{
	FAction* Action = qobject_cast<FAction*>(sender());
	bool PerformOp = true;
	if (Action && Model)
	{
		switch (Action->id())
		{
			case FAction::FEditAddActionId : 
			{
				State = StateInserting;
				emit beforeInsertRow(0, PerformOp); 
				if (PerformOp)
					insertRow(0);
				State = StateNone;
			}
			break;
			case FAction::FEditActionId : 
			{
				State = StateEditing;
				QModelIndex CIndex = SearchWidget->view()->currentIndex();
				if (CIndex.isValid())
				{
					emit beforeEditRow(CIndex.row(), PerformOp); 
					if (PerformOp)
					{
						editCurrentRow(CIndex.row());
					}
				}
				State = StateNone;
			}
			break; 
			case FAction::FEditDeleteActionId : 
			{
				State = StateDeleting;
				QModelIndex CIndex = SearchWidget->view()->currentIndex();
				
				if (CIndex.isValid())
				{
					emit beforeRemoveRow(CIndex.row(), PerformOp); 
					if (PerformOp)
						removeCurrentRow(CIndex.row());
/*					if (QTableView* View = qobject_cast<QTableView*>(SearchWidget->view()))
						View->hideRow(CIndex.row());*/
				}
				else 
					qWarning("Trying to remove an invalid index.");
				State = StateNone;
			}
			break; 
			default:
			break;
		};
	}
}

void FActionTableView::editCurrentRow(int _Index)
{

	if (RecDialog)
		RecDialog->exec();
	else 
		if (Model)
		{
			int Col = EditColumn;
			if (QTableView* TView = qobject_cast<QTableView*>(SearchWidget->view()))
			{
				Col = TView->horizontalHeader()->logicalIndex(0);
				bool Trobat = false;
				while (!Trobat)
				{
					Trobat = !TView->isColumnHidden(Col); 
					if (!Trobat) 
						Col++;
				}
			}
			SearchWidget->view()->edit(Model->index(_Index, Col));
		}
	emit afterEditRow(_Index);
}

void FActionTableView::insertRow(int _Row)
{
	Model->insertRows(_Row,1);
	emit rowInserted(_Row);
	if (RecDialog)
	{
		if (RecDialog->exec() == QDialog::Accepted)
			emit afterInsertRow(0);
	}
	else 
		emit afterInsertRow(0);
}

void FActionTableView::removeCurrentRow(int _Index)
{
	Model->removeRow(_Index);
	emit afterRemoveRow(_Index);
}
