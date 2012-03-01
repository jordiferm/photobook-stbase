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
#include "smultiprocessstatuswidget.h"
#include <QLayout>
#include "sprocessstatuswidget.h" 

SMultiProcessStatusWidget::SMultiProcessStatusWidget(int _NBars, QWidget* parent, Qt::WindowFlags f, Qt::Orientation _Orientation): QWidget(parent, f)
{
	QLayout* MLayout;
	if (_Orientation == Qt::Horizontal)
		MLayout = new QHBoxLayout(this); 
	else 
		MLayout = new QVBoxLayout(this); 
	for (int Vfor = 0; Vfor < _NBars; Vfor++)
	{
		SProcessStatusWidget* NSProcStatusW = new SProcessStatusWidget(this, 0, _Orientation); 
		MLayout->addWidget(NSProcStatusW); 
		ProcList.push_back(NSProcStatusW); 
	}
}

SProcessStatusWidget* SMultiProcessStatusWidget::procStatusWidget(int _Bar)
{
	SProcessStatusWidget* Res = 0; 
	if (_Bar >= 0 && _Bar < ProcList.size())
		Res = ProcList[_Bar];
	return Res; 
}


