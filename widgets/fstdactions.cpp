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
#include "fstdactions.h"

void FAction::addActions(const QActionGroup* _Source, QActionGroup* _Destination)
{
	QList<QAction*> SourceActions = _Source->actions();
	QList<QAction*>::const_iterator it; 
	for( it = SourceActions.begin(); it != SourceActions.end(); ++it)
		_Destination->addAction(*it);
}


FGeneralActionGroup::FGeneralActionGroup(QObject* _Parent, int _Actions)
	: QActionGroup(_Parent)
{
	if (_Actions & Accept)
		addAction(new FAcceptAction(this));	
	if (_Actions & Cancel)
		addAction(new FCancelAction(this));
	if (_Actions & Edit)
		addAction(new FEditAction(this));
	if (_Actions & EditAdd)
		addAction(new FEditAddAction(this));
	if (_Actions & EditDelete)
		addAction(new FEditDeleteAction(this));
	if (_Actions & Find)
		addAction(new FFindAction(this));
	if (_Actions & Configure)
		addAction(new FConfigureAction(this));
	if (_Actions & Help)
		addAction(new FHelpAction(this));
	if (_Actions & Print)
		addAction(new FPrintAction(this));
	if (_Actions & Exit)
		addAction(new FExitAction(this));
	if (_Actions & Yes)
		addAction(new FYesAction(this));
	if (_Actions & No)
		addAction(new FNoAction(this));
}


FPositionActionGroup::FPositionActionGroup(QObject* _Parent, int _Actions)
	: QActionGroup(_Parent)
{
	if (_Actions & Start)
		addAction(new FStartAction(this));
	if (_Actions & Rew)
		addAction(new FRewAction(this));
	if (_Actions & Previous)
		addAction(new FPreviousAction(this));
	if (_Actions & Pause)
		addAction(new FPauseAction(this));
	if (_Actions & Stop)
		addAction(new FStopAction(this));
	if (_Actions & Run)
		addAction(new FRunAction(this));
	if (_Actions & Next)
		addAction(new FNextAction(this));
	if (_Actions & Fwe)
		addAction(new FFweAction(this));
	if (_Actions & End)
		addAction(new FEndAction(this));
}

FFileActionGroup::FFileActionGroup(QObject* _Parent, int _Actions)
	: QActionGroup(_Parent)
{
	if (_Actions & Save)
		addAction(new FFileSaveAction(this));
	if (_Actions & Close)
		addAction(new FFileCloseAction(this));
	if (_Actions & SaveAs)
		addAction(new FFileSaveAsAction(this));
}

