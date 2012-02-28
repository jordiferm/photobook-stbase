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
#include "stsendersettingsdialog.h"
#include <QLayout> 
#include <QDialogButtonBox>
#include <QPushButton>
#include "siconfactory.h"


STSenderSettingsDialog::STSenderSettingsDialog(QWidget *parent)
 : QWizard(parent)
{
	addPage(new MyOPWUserDataPage(this));
	//setStartId(Page_UserData);
	setWindowTitle(tr("User Settings Wizard"));
	setOption(HaveHelpButton, false);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/personal.png"));
	setButtonText(QWizard::FinishButton, tr("Confirm")); 
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif


// 	QVBoxLayout* MLayout = new QVBoxLayout(this);
// 
// 	PubWidget = new OPWUserDataPage(this); 
// 	MLayout->addWidget(PubWidget);	
// 
// 	MLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
// 
// 	QDialogButtonBox* Buttons = new QDialogButtonBox(this);
// 	
// 	QPushButton* ButOk = Buttons->addButton(tr("Ok"), QDialogButtonBox::AcceptRole);
// 	ButOk->setIcon(stIcon(SIconFactory::Accept));
// 	ButOk->setIconSize(QSize(32,32));
// 
//  	QPushButton* ButCancel = Buttons->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
//  	ButCancel->setIcon(stIcon(SIconFactory::Cancel));
// 	ButCancel->setIconSize(QSize(32,32));
// 	
// 	MLayout->addWidget(Buttons);
// 
// 	PubWidget->initializePage(); 
// 	connect(Buttons, SIGNAL(accepted()), this, SLOT(saveAndAccept()));
// 	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


STSenderSettingsDialog::~STSenderSettingsDialog()
{
}

