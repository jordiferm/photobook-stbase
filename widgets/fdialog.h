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

#ifndef FDIALOG_H
#define FDIALOG_H

#include "stwidgetsexport.h"
#include "fabstractactioncontainer.h"
#include <qdialog.h>
#include "sactionbuttonsframe.h"
#include "fstdactions.h"
/**
Dialeg amb les accions Accept i Cancel per defecte i amb possibilitat d'afegir accions de flam.
Connecta automàticament les actions Accept i Cancel als slots corresponents de QDialog.
@author Shadow
*/
class QVBoxLayout;
class QBoxLayout;
class QShowEvent; 
class ST_WIDGETS_EXPORT FDialog : public QDialog, public FAbstractActionContainer
{
	Q_OBJECT

	SActionButtonsFrame* ButBox;
	QVBoxLayout* MLayout;
	QWidget* MainWidget;
	FAction::EnActionId Result;
	FAction* CancelAction;

private:
	void init(QWidget* _MainWidget);

protected:
	void setButtonBox(SActionButtonsFrame* _ButBox);
	//! \reimpl
	void reject();
	//! \reimpl
	void closeEvent(QCloseEvent * e);
	void showEvent(QShowEvent*);


public:
	//! Dialeg amb _MainWidget
	FDialog(QWidget* _Parent = 0, Qt::WFlags f = 0);
	FDialog(const QActionGroup* _Actions, QWidget* _MainWidget,  QWidget *parent = 0, Qt::WFlags f = 0);
	void setMainWidget(QWidget* _Widget);
	QWidget* mainWidget() const;
	void addAction(QAction* _Action);
	void addActions(const QActionGroup& _Actions);
	FAction::EnActionId result() const;
	//! Retorna id de la action
	FAction::EnActionId exec();
	QBoxLayout* layout() const;

private slots:
	virtual void actionTriggered();
};

#endif
