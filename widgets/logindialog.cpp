/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "logindialog.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include "siconfactory.h"

LoginDialog::LoginDialog(bool _AskForKey, QWidget *parent) :
	QDialog(parent), LEKey(0)
{
	setWindowTitle(tr("Please, sign in."));

	QVBoxLayout* MLayout = new QVBoxLayout(this);

	NewAccountLabel = new QLabel(this);
	NewAccountLabel->setAlignment(Qt::AlignCenter);
	MLayout->addWidget(NewAccountLabel);
	NewAccountLabel->setVisible(false);
	NewAccountLabel->setOpenExternalLinks(true);

	QFormLayout* FormLayout = new QFormLayout;
	MLayout->addLayout(FormLayout);
	LEUser = new QLineEdit(this);
	FormLayout->addRow(tr("User"), LEUser);
	LEPassword = new QLineEdit(this);
	LEPassword->setEchoMode(QLineEdit::Password);
	FormLayout->addRow(tr("Password"), LEPassword);

	if (_AskForKey)
	{
		LEKey = new QLineEdit(this);
		FormLayout->addRow(tr("Key"), LEKey);
	}

	LostPasswordLabel = new QLabel(this);
	LostPasswordLabel->setTextFormat(Qt::RichText);
	MLayout->addWidget(LostPasswordLabel);
	LostPasswordLabel->setVisible(false);
	LostPasswordLabel->setOpenExternalLinks(true);

	QDialogButtonBox* ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										  | QDialogButtonBox::Cancel);	

	QPushButton* OkButton = ButtonBox->button(QDialogButtonBox::Ok);
	OkButton->setIcon(stIcon(SIconFactory::Accept));
	OkButton->setIconSize(QSize(32, 32));

	QPushButton* CancelButton = ButtonBox->button(QDialogButtonBox::Cancel);
	CancelButton->setIcon(stIcon(SIconFactory::Cancel));
	CancelButton->setIconSize(QSize(32, 32));


	MLayout->addWidget(ButtonBox);
	connect(ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void LoginDialog::setNewAccountUrl(const QUrl& _Url)
{
	NewAccountLabel->setText(tr("<i>Don't have account ?</i><br></br><a href=\"%1\">Create a new one</a>").arg(_Url.toString()));
	NewAccountLabel->setVisible(true);
}

void LoginDialog::setForgetPassword(const QUrl& _Url)
{
	LostPasswordLabel->setText(tr("Lost password?,<a href=\"%1\">Click here</a>").arg(_Url.toString()));
	LostPasswordLabel->setVisible(true);
}


QString LoginDialog::userName() const
{
	return LEUser->text();
}

void LoginDialog::setUserName(const QString& _Value)
{
	LEUser->setText(_Value);
	LEPassword->setFocus();
}


QString LoginDialog::password() const
{
	return LEPassword->text();
}

void LoginDialog::setPassword(const QString& _Value)
{
	LEPassword->setText(_Value);
}

QString LoginDialog::key() const
{
	if (LEKey)
		return LEKey->text();
}

void LoginDialog::setKey(const QString& _Value)
{
	if (LEKey)
		LEKey->setText(_Value);
}

