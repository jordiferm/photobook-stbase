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

#include "fdialog.h"
#include <QLayout>
#include <QCloseEvent>


void FDialog::init(QWidget* _MainWidget)
{
	Result = FAction::FCancelActionId;
	MainWidget = 0;
	ButBox = 0;
	MLayout = new QVBoxLayout(this);
	MLayout->setMargin(2);
	MLayout->setSpacing(2);

	if (_MainWidget)
		setMainWidget(_MainWidget); 
	
	setButtonBox(new SActionButtonsFrame(this));
}

void FDialog::setButtonBox(SActionButtonsFrame* _ButBox)
{
	if (ButBox)
		MLayout->removeWidget(ButBox);
		
	ButBox = _ButBox;
	MLayout->insertWidget(1,ButBox);
}

void FDialog::reject()
{
	if (CancelAction)
	{
		if (CancelAction->isEnabled())
			QDialog::reject();
	}
	else 
		QDialog::reject();
}

void FDialog::closeEvent ( QCloseEvent * e )
{	
	//
	if (CancelAction)
		if (!CancelAction->isEnabled())
			e->ignore();
}

void FDialog::showEvent( QShowEvent *  )
{
	if (mainWidget())
		mainWidget()->setFocus();
}


FDialog::FDialog(QWidget* _Parent, Qt::WFlags _Flags) : QDialog(_Parent, _Flags), FAbstractActionContainer(this),
	CancelAction(0)
{
	init(0);
}

FDialog::FDialog(const QActionGroup* _Actions, QWidget* _MainWidget,  QWidget* _Parent, Qt::WFlags _Flags)
 : QDialog(_Parent, _Flags), FAbstractActionContainer(this), CancelAction(0)
{
	init( _MainWidget);
	addActions(*_Actions);
}

void FDialog::setMainWidget(QWidget* _Widget)
{
	if (MainWidget)
		MLayout->removeWidget(MainWidget);
	MainWidget = _Widget;
	MLayout->insertWidget(0,_Widget);
}

QWidget* FDialog::mainWidget() const
{
	return MainWidget;
}

void FDialog::addAction(QAction* _Action)
{
	ButBox->addAction(_Action);
	FAction* CurrAction = qobject_cast<FAction*>(_Action);
	if (CurrAction)
	{
		if (CurrAction->id() == FAction::FCancelActionId)
			CancelAction = CurrAction;
		connect(CurrAction, SIGNAL(triggered()), this, SLOT(actionTriggered()));
	}
	FAbstractActionContainer::addAction(_Action);
}

void FDialog::addActions(const QActionGroup& _Actions)
{
	FAbstractActionContainer::addActions(_Actions);
}

FAction::EnActionId FDialog::result() const
{
	return Result;
}

FAction::EnActionId FDialog::exec()
{
	int Res = QDialog::exec();
	Result = static_cast<FAction::EnActionId>(Res);
	return result();
}

QBoxLayout* FDialog::layout() const
{
	return MLayout;
}

void FDialog::actionTriggered()
{
	FAction* CurrAction = qobject_cast<FAction*>(sender());
	if (CurrAction)
		Result = static_cast<FAction::EnActionId>(CurrAction->id());
	if (Result == FAction::FCancelActionId || Result == FAction::FNoActionId)
		reject(); 
	else 
	if (Result == FAction::FAcceptActionId || Result == FAction::FYesActionId)
		accept();
}

