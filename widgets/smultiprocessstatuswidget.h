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
#ifndef SMULTIPROCESSSTATUSWIDGET_H
#define SMULTIPROCESSSTATUSWIDGET_H

#include <QWidget>
#include <QList>
#include "stwidgetsexport.h"

/**
Multiple label and progressbar widgets.

	@author 
*/
class SProcessStatusWidget;
class ST_WIDGETS_EXPORT SMultiProcessStatusWidget : public QWidget
{
Q_OBJECT
	QList<SProcessStatusWidget*> ProcList; 

public:
    SMultiProcessStatusWidget(int _NBars, QWidget* parent = 0, Qt::WindowFlags f = 0, Qt::Orientation _Orientation = Qt::Vertical );
	SProcessStatusWidget* procStatusWidget(int _Bar); 
};

#endif
