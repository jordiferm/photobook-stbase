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

#ifndef STIWIZARD_H
#define STIWIZARD_H

#include <QStackedWidget>
#include "stwizardsexport.h"

/**
Interface per els wizards.
 
@author Shadow
*/
class STIWizardPage;
class QWidget;
class ST_WIZARDS_EXPORT STIWizard : public QStackedWidget
{

Q_OBJECT

public:
	STIWizard(QWidget* _Parent= 0);
	QWidget* LastPage;

public slots:
	bool setPage(int _Index);
	void firstPage();
	void nextPage();
	void previousPage();

protected:
	void addPage(STIWizardPage* _Page, bool _LastPage = false);
	void addWizard(STIWizard* _Wizard);
	void showWizard(STIWizard* _Wizard, bool _Initialize = true);
	virtual void init() = 0;

signals:
	void finished();
	void previousWizard();
	void previousWizardNextPage();
};

#endif
