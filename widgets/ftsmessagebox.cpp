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

#include "ftsmessagebox.h"
#include <QLayout>
#include <QLabel> 
#include <QList>

//TODO: This class is obsolete, please use SMessageBox instead.

void FTSMessageBox::init(const QActionGroup& _Actions, EnIcon _Icon)
{
	QWidget* MWidget = new QWidget(this); 
	QHBoxLayout* MWLayout = new QHBoxLayout(MWidget);
	LabIcon = new QLabel(MWidget); 
	setIcon(_Icon);
	MWLayout->addWidget(LabIcon);

	LabText = new QLabel(MWidget);
	LabText->setWordWrap(true);
	MWLayout->addWidget(LabText);
	LabText->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	FTSDialog::setMainWidget(MWidget);

// 	LabText = new QLabel(this);
// 	FTSDialog::setMainWidget(LabText);

	addActions(_Actions);	
}

FTSMessageBox::FTSMessageBox(QWidget* _Parent)
 : FTSDialog(_Parent)
{
	QActionGroup AgNothing(this);
	init(AgNothing, Information);
}

FTSMessageBox::FTSMessageBox(const QString& _Caption, const QString& _Text, EnIcon _Icon, const QActionGroup& _Actions, 
	QWidget* _Parent, Qt::WFlags f)
 : FTSDialog(_Parent, f | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	init(_Actions, _Icon);
   setWindowTitle(_Caption);
	setText(_Text);
}



FTSMessageBox::~FTSMessageBox()
{
}

QString FTSMessageBox::text() const
{
	return LabText->text();  
}

void FTSMessageBox::setText(const QString& _Text)
{
	LabText->setText("<h2>" + _Text + "</h2>");
}

void FTSMessageBox::setIcon(EnIcon _Icon)
{
	switch (_Icon)
	{
		case NoIcon: 
		break;
		case Question: 
			setIcon(QPixmap(":/icons/question.png"));
		break;
		case Information: 
			setIcon(QPixmap(":/icons/info.png"));
		break;
		case Warning: 
			setIcon(QPixmap(":/icons/warning.png"));
		break;
		case Critical: 
			setIcon(QPixmap(":/icons/fatal.png"));
		break;
	}
}

void FTSMessageBox::setIcon(const QPixmap& _Pixmap)
{
	LabIcon->setPixmap(_Pixmap);
}

FAction::EnActionId FTSMessageBox::information(QWidget* _Parent, const QString& _Caption, const QString& _Text)
{
	FGeneralActionGroup Actions(0, FGeneralActionGroup::Accept);
	return general(_Parent, _Caption, _Text, Information, Actions); 
}

FAction::EnActionId FTSMessageBox::critical(QWidget* _Parent, const QString& _Caption, const QString& _Text)
{
	FGeneralActionGroup Actions(0, FGeneralActionGroup::Accept);
	return general(_Parent, _Caption, _Text, Critical, Actions); 
}


FAction::EnActionId FTSMessageBox::question(QWidget* _Parent, const QString& _Caption, const QString& _Text)
{
	FGeneralActionGroup Actions(0, FGeneralActionGroup::Yes | FGeneralActionGroup::No);
	return general(_Parent, _Caption, _Text, Question, Actions); 
}

//TODO: Aquí es podria millorar el tema amb una estàtica per a tots els messagebox on es passes el Messagebox.
FAction::EnActionId FTSMessageBox::general(QWidget* _Parent, const QString& _Caption, const QString& _Text, EnIcon _Icon, const QActionGroup& _ActGroup)
{
	FTSMessageBox MBInfo(_Caption, _Text, _Icon, _ActGroup, _Parent);
	return MBInfo.FDialog::exec();
}



// _________________________________________________________________________*/
//
//	Class 
// _________________________________________________________________________*/



FTSAdvancedMessageBox::FTSAdvancedMessageBox(const QString& caption,
                          const QString &text, FTSMessageBox::EnIcon icon,
                          const QActionGroup& _Actions,
                          QWidget *parent, Qt::WFlags f) : FTSMessageBox(caption, text, icon, _Actions, parent, f), ShortText(text)
{}

void FTSAdvancedMessageBox::setAdvancedText(const QString& _AdvancedText)
{
	AdvancedText = _AdvancedText;
}

QString FTSAdvancedMessageBox::advancedText() const
{
	return AdvancedText;
}


FAction::EnActionId FTSAdvancedMessageBox::advanced(QWidget* _Parent, const QString& _Caption, const QString& _Text, 
		const QString& _AdvancedText, QActionGroup& _ActGroup, FTSMessageBox::EnIcon _Icon)
{
	FTSAdvancedMessageBox MBInfo(_Caption, _Text, _Icon, _ActGroup, _Parent);
	MBInfo.setAdvancedText(_AdvancedText);
	FAction* AdvAction = new FAdvancedAction(&MBInfo);
	AdvAction->setCheckable(true);
	MBInfo.addAction(AdvAction);
	connect(AdvAction, SIGNAL(toggled(bool )), &MBInfo, SLOT(showAdvancedText(bool )));
	return MBInfo.FDialog::exec();
}

FAction::EnActionId FTSAdvancedMessageBox::advanced(QWidget* _Parent, const QString& _Caption, const QString& _Text, 
		const QString& _AdvancedText, FTSMessageBox::EnIcon _Icon)
{
	FGeneralActionGroup Actions(0, FGeneralActionGroup::Accept);
	return advanced(_Parent, _Caption, _Text, _AdvancedText, Actions, _Icon);	
}



void FTSAdvancedMessageBox::showAdvancedText(bool _Show)
{
	if (_Show)
	{ 
		ShortText = text();
		setText(ShortText + AdvancedText);
	}
	else 
		setText(ShortText);
}
