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
#include "styearandmonthdialog.h"
#include <QLayout> 
#include <QDialogButtonBox>
#include <QPushButton> 
#include <QSpinBox> 
#include <QLabel> 
#include <QDate> 
#include "siconfactory.h" 

STYearAndMonthDialog::STYearAndMonthDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	QVBoxLayout* MainLayout = new QVBoxLayout(this); 
	QHBoxLayout* MLayout = new QHBoxLayout; 
	MainLayout->addLayout(MLayout); 
	MLayout->addWidget(new QLabel(tr("Year: "))); 
	SPYear = new QSpinBox(this);
	SPYear->setRange(1900, 3000);
	MLayout->addWidget(SPYear); 
		
	MLayout->addWidget(new QLabel(tr("Month: "))); 
	SPMonth = new QSpinBox(this);
	SPMonth->setRange(1, 12);
	MLayout->addWidget(SPMonth); 
	
	QDialogButtonBox* Buttons = new QDialogButtonBox(this);

	QPushButton* ButOk = Buttons->addButton(tr("Ok"), QDialogButtonBox::AcceptRole);
	ButOk->setIcon(stIcon(SIconFactory::Accept)); 

	QPushButton* ButCancel = Buttons->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
	ButCancel->setIcon(stIcon(SIconFactory::Cancel)); 
	
	MainLayout->addWidget(Buttons);
	connect(Buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));
	setYear(QDate::currentDate().year()); 
	setMonth(QDate::currentDate().month()); 
}


int STYearAndMonthDialog::year() const
{
	return SPYear->value(); 
}

void STYearAndMonthDialog::setYear(int _Value)
{
	SPYear->setValue(_Value); 
}

int STYearAndMonthDialog::month()
{
	return SPMonth->value(); 
}

void STYearAndMonthDialog::setMonth(int _Value)
{
	SPMonth->setValue(_Value); 
}



