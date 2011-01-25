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

#include "stiwizard.h"
#include "stiwizardpage.h"

STIWizard::STIWizard(QWidget* _Parent)
 : QStackedWidget(_Parent)
{
}

bool STIWizard::setPage(int _Index)
{
	bool Res = false;
	QWidget* CWidget = widget(_Index); 
	if (STIWizardPage* Page = qobject_cast<STIWizardPage*>(CWidget))
	{
		setCurrentIndex(_Index);
		Page->init(); 
		Res = true;
	}
	return Res;
}

void STIWizard::firstPage()
{
	setPage(0);
}

//! Se salta els STIWizard
void STIWizard::nextPage()
{
	int CIndex = currentIndex() + 1;
	bool FindPage = false;
	while (!FindPage && CIndex < count())	
	{
		FindPage = setPage(CIndex);
		CIndex++;
	}
		
}

void STIWizard::previousPage()
{
	int CIndex = currentIndex() - 1;
	bool Found = false;
	//if (STIWizardPage* Page = qobject_cast<STIWizardPage*>(CWidget))
	while (!Found && CIndex >= 0)
	{
		QWidget* CWidget = widget(CIndex); 
		//Found = CWidget->inherits("STIWizardPage"); 
		if (STIWizardPage* Page = qobject_cast<STIWizardPage*>(CWidget))
		{
			Found = true;
			setCurrentWidget(Page);
			Page->setFromPrevious();
		}
		else
			CIndex --;
	}
}

void STIWizard::addPage(STIWizardPage* _Page, bool _LastPage)
{
	addWidget(_Page);
// 	if (_LastPage)
// 		connect(_Page, SIGNAL(nextPage()), this, SLOT(firstPage()));
// 	else 
	if (!_LastPage)
		connect(_Page, SIGNAL(nextPage()), this, SLOT(nextPage()));

	connect(_Page, SIGNAL(previousPage()), this, SLOT(previousPage()));
}

void STIWizard::addWizard(STIWizard* _Wizard)
{
	//connect(_Wizard, SIGNAL(finished()), this, SLOT(nextPage()));
	connect(_Wizard, SIGNAL(previousWizard()), this, SLOT(previousPage()));
	connect(_Wizard, SIGNAL(previousWizardNextPage()), this, SLOT(nextPage()));
	addWidget(_Wizard);
}
/*! 
	\param _Initialize incica si es cridarà a l'Init del Wizard o no...(Per defecte true)
*/
void STIWizard::showWizard(STIWizard* _Wizard, bool _Initialize)
{
	LastPage = currentWidget();
	setCurrentWidget(_Wizard); // Perquè l'ensenyi primer.
	if (_Initialize)
		_Wizard->init();
}

