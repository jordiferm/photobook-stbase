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
#ifndef OMAINTOOLBAR_H
#define OMAINTOOLBAR_H

#include <QToolBar>
#include "stwidgetsexport.h"

/**
	@author 
*/
class ST_WIDGETS_EXPORT OMainToolBar : public QToolBar
{
Q_OBJECT
public:
	OMainToolBar(QWidget *parent = 0);
	~OMainToolBar();

protected slots:	
	void openLink(const QString& _Link);
};

#endif
