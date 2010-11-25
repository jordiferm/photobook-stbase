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

#include "ftsdialog.h"
#include "sactionbuttonsframe.h"


FTSDialog::FTSDialog(QWidget* _Parent, Qt::WFlags _Flags) : FDialog(_Parent, _Flags)
{
	SActionButtonsFrame* BFrame = new SActionButtonsFrame(this);
	BFrame->setIconSize(QSize(32, 32));
	setButtonBox(BFrame);
}

FTSDialog::FTSDialog(const QActionGroup* _Actions, QWidget* _MainWidget, QWidget* _Parent, Qt::WFlags _Flags) 
	: FDialog(_Parent, _Flags)
{
	SActionButtonsFrame* BFrame = new SActionButtonsFrame(this);
	BFrame->setIconSize(QSize(32, 32));
	setButtonBox(BFrame);
	setMainWidget(_MainWidget);
	addActions(*_Actions);
}

