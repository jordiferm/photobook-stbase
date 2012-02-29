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

#include "frecdialog.h"
#include <QDataWidgetMapper>
#include <QSqlTableModel> 
#include "fstdactions.h"
#include "ftablemanager.h"
#include "fsqlrelationaltablemodel.h" 
#include "fsqlmodelviewutils.h"
#include "fsqlactiontableview.h"
#include "frecordwidget.h" 


FRecDialog::FRecDialog(QSqlTableModel* _Model, FTableManager* _Parent)
 : FTSDialog(_Parent), Model(_Model), ParentTManager(_Parent)
{
	addAction(new FAcceptAction(this)); 
	addAction(new FCancelAction(this)); 

	connect(ParentTManager->actionTableView(), SIGNAL(beforeInsertRow(int , bool& )), 
		this, SLOT(beforeInsertParentRow(int , bool& )));
}



/*!
	Te en compte el TableManager _TManager per fer l'edici�.
*/
void FRecDialog::addTableManager(FTableManager* _TManager)
{
	connect(_TManager->actionTableView(), SIGNAL(beforeInsertRow(int , bool& )), 
		this, SLOT(beforeInsertRelatedRow(int , bool& )));
	connect(_TManager->actionTableView(), SIGNAL(afterInsertRow(int )), 
		this, SLOT(disableCancel()));
	connect(_TManager->actionTableView(), SIGNAL(afterEditRow(int )), 
		this, SLOT(disableCancel()));
	connect(_TManager->actionTableView(), SIGNAL(afterRemoveRow(int )), 
		this, SLOT(disableCancel()));
}


void FRecDialog::hideEvent(QHideEvent* _Event )
{
	FAction* CancelAction = fAction(FAction::FCancelActionId);
	if (CancelAction)
		CancelAction->setEnabled(true);
	FTSDialog::hideEvent(_Event);
}

void FRecDialog::beforeInsertParentRow(int /*_Row*/, bool& /*_PerformOp*/)
{
	InsertedRowSaved = false;
}

void FRecDialog::disableCancel()
{
	if (isVisible())
	{
		FAction* CancelAction = fAction(FAction::FCancelActionId);
		if (CancelAction)
			CancelAction->setEnabled(false);
	}
}

/*! 
	Si el parentmanager inserta i encara no hem insertat primer guardarem els canvis i recurerarem la row del model... 
*/
void FRecDialog::beforeInsertRelatedRow(int /*_Row*/, bool& _PerformOp)
{
	if (FRecordWidget* CRecWidget = qobject_cast<FRecordWidget*>(mainWidget()))
	{
		if (ParentTManager->actionTableView()->isInserting() && !InsertedRowSaved)
		{
			QSqlRecord InsertedRec = Model->record(CRecWidget->mapper()->currentIndex());
	
			_PerformOp = ParentTManager->actionTableView()->commit();
			if (FSqlRelationalTableModel* FModel = qobject_cast<FSqlRelationalTableModel*>(Model))
			{
				QModelIndex PrevIndex = FModel->recordMatch(InsertedRec);
				if (PrevIndex.isValid())
					CRecWidget->mapper()->setCurrentIndex(PrevIndex.row());
			}
			if (_PerformOp)
				InsertedRowSaved = true;
		}
	}
}


