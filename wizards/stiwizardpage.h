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

#ifndef STIWIZARDPAGE_H
#define STIWIZARDPAGE_H

#include <QWidget>
#include "stwizardsexport.h"

/**
Interface for wizard page.

@author Shadow
*/
class ST_WIZARDS_EXPORT STIWizardPage : public QWidget
{
Q_OBJECT
public:
	STIWizardPage(QWidget* _Parent = 0);

protected:
	void paintEvent(QPaintEvent *);

public slots:
	virtual void init() = 0;
	virtual void setFromPrevious();
signals:
	void nextPage();
	void previousPage();
};

#endif
