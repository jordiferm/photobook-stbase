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

#include "tpinputdialog.h"
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QToolButton>
#include "gkeyboard2.h"


QToolButton* TPInputDialog::newBottomActionButton(const QIcon& _Icon, QWidget* _Parent)
{
	QToolButton* ButRes = new QToolButton(_Parent);
	ButRes->setIcon(_Icon);
	ButRes->setIconSize(QSize(128, 128));
	ButRes->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ButRes->setObjectName("ActionButton");
	return ButRes;
}

TPInputDialog::TPInputDialog(const QString _LabelText, QWidget* _Parent)
 : QDialog(_Parent)
{
	
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	TitleLabel = new QLabel(_LabelText, this);
	MainLayout->addWidget(TitleLabel);
	QHBoxLayout* LELayout = new QHBoxLayout;
	MainLayout->addLayout(LELayout);
	LEUserInput = new QLineEdit(this);

	LELayout->addWidget(LEUserInput); 
	LELayout->addItem(new QSpacerItem(10,0));

	GKeyboard2* WKeyboard = new GKeyboard2(this);
	BEnter = WKeyboard->KbUi.benter;

	MainLayout->addWidget(WKeyboard);
	WKeyboard->setNumericKeyboardVisible(false);
	WKeyboard->setMinimumWidth(900);
	connect(LEUserInput, SIGNAL(returnPressed()), this, SLOT(accept()));

	BottomFrame = new QFrame(this);
	MainLayout->addWidget(BottomFrame);

	QHBoxLayout* ButLayout = new QHBoxLayout(BottomFrame);

	QToolButton* ButCancel = newBottomActionButton(QIcon(":/st/tpopsl/cancel.png"), BottomFrame);
	connect(ButCancel, SIGNAL(clicked( bool )), this, SLOT(reject()));
	ButCancel->setText(tr("Cancel"));
	ButLayout->addWidget(ButCancel);

	ButLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

	QToolButton* ButAccept = newBottomActionButton(QIcon(":/st/tpopsl/accept.png"), BottomFrame);
	connect(ButAccept, SIGNAL(clicked( bool )), this, SLOT(accept()));
	ButAccept->setText(tr("Accept"));
	ButLayout->addWidget(ButAccept);

	setCancelEnabled(false);
}

QString TPInputDialog::strResult() const
{
	return LEUserInput->text();
}

void TPInputDialog::setEchoMode(QLineEdit::EchoMode _EchoMode)
{
	LEUserInput->setEchoMode(_EchoMode);
}

void TPInputDialog::setText(const QString& _Text)
{
	LEUserInput->setText(_Text);
}

void TPInputDialog::setCancelEnabled(bool _Enabled)
{
	BottomFrame->setVisible(_Enabled);
	if (_Enabled)
	{
		BEnter->setIcon(QIcon());
	}
	else
	{
		BEnter->setIcon(QIcon(":/st/tpopsl/accept.png"));
		BEnter->setIconSize(QSize(64,64));
	}
}

void TPInputDialog::setTitle(const QString& _Title)
{
	TitleLabel->setText(_Title);
}
