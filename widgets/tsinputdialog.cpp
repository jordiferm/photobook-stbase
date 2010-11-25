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

#include "tsinputdialog.h"
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include "gkeyboard2.h"

TSInputDialog::TSInputDialog(const QString _LabelText, QWidget* _Parent)
 : QDialog(_Parent)
{
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	MainLayout->addWidget(new QLabel(_LabelText, this)); 
	QHBoxLayout* LELayout = new QHBoxLayout;
	MainLayout->addLayout(LELayout);
	LEUserInput = new QLineEdit(this);

	LELayout->addWidget(LEUserInput); 
	LELayout->addItem(new QSpacerItem(10,0));


	GKeyboard2* WKeyboard = new GKeyboard2(this);
	QPushButton* BEnter = WKeyboard->KbUi.benter;

	BEnter->setIcon(QIcon(":/st/accept.png"));
	BEnter->setIconSize(QSize(64,64));
	MainLayout->addWidget(WKeyboard);
	WKeyboard->setNumericKeyboardVisible(false);
	WKeyboard->setMinimumWidth(900);
	connect(LEUserInput, SIGNAL(returnPressed()), this, SLOT(accept()));
}

QString TSInputDialog::strResult() const
{
	return LEUserInput->text();
}

void TSInputDialog::setEchoMode(QLineEdit::EchoMode _EchoMode)
{
	LEUserInput->setEchoMode(_EchoMode);
}

void TSInputDialog::setText(const QString& _Text)
{
	LEUserInput->setText(_Text);
	LEUserInput->selectAll();
}

