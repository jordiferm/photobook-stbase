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

#include "sdbsettingsdialog.h"
#include <QLabel>
#include <QLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QDataWidgetMapper> 

#include "fdbconfigwidget.h"
#include "sappsettings.h"
#include "fsqldatabasesettings.h"
#include "siconfactory.h"

#include <QApplication>

void SDBSettingsDialog::setLabelStyle(QLabel* _Label, bool _Error)
{
	QPalette PalLabel; 
   PalLabel.setColor(QPalette::Active, QPalette::Text, QColor("#FF0000"));
   PalLabel.setColor(QPalette::Active, QPalette::Foreground, QColor("#FF0000"));
	PalLabel.setColor(QPalette::Active, QPalette::Background, QColor("#F9F9F9"));
	QFont Font = _Label->font();
	if (_Error)
	{
		Font.setBold(true);
		_Label->setPalette(PalLabel);
	}
	else 
	{
		Font.setBold(false);
		_Label->setPalette(NormalLabPalette);
	}
	_Label->setFont(Font);
}


SDBSettingsDialog::SDBSettingsDialog(FSqlDatabaseSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent)
 : QDialog(_Parent), ButCreatePressed(false), Mapper(_Mapper), Settings(_Settings)
{

	QVBoxLayout* MLayout = new QVBoxLayout(this);
	LabIcon = new QLabel(this); 
	LabIcon->setPixmap(stIcon(SIconFactory::DatabaseSettings).pixmap(QSize(32, 32)));
	LabIcon->setAlignment(Qt::AlignHCenter);
	MLayout->addWidget(LabIcon);

	LabWarning = new QLabel(this); 
	setLabelStyle(LabWarning, true); 
	LabWarning->setFrameShape(QFrame::Box);
	LabWarning->setWordWrap(true);
	MLayout->addWidget(LabWarning);	
	LabWarning->hide();
	
	QHBoxLayout* DBNameLayout = new QHBoxLayout;
	LabDbName = new QLabel("Database name", this);
	DBNameLayout->addWidget(LabDbName);
	
	LeDbName = new QLineEdit(this); 
	Model = _Settings.model();
	_Mapper->addMapping(LeDbName, Model->index(_Settings.keyFullName("databasename")).column());
	
	DBNameLayout->addWidget(LeDbName);
	
	MLayout->addLayout(DBNameLayout);

	DBCWidget = new FSingleDBConfigWidget(_Settings, _Mapper, this);
	
	GBAdvanced = new QGroupBox(tr("Advanced"), this); 
	GBAdvanced->hide();
	MLayout->addWidget(GBAdvanced);
	QHBoxLayout* GBLayout = new QHBoxLayout(GBAdvanced);
	GBLayout->setMargin(2);
	GBLayout->addWidget(DBCWidget);	
	
	QHBoxLayout* ButLayout = new QHBoxLayout; 
	ButLayout->addItem(new QSpacerItem(10,0));
	
	TBAdvanced = new QToolButton(this); 
	TBAdvanced->setText(tr("Advanced"));
	TBAdvanced->setIcon(stIcon(SIconFactory::Advanced));
	TBAdvanced->setIconSize(QSize(32,32));
	TBAdvanced->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	TBAdvanced->setCheckable(true);
	connect(TBAdvanced, SIGNAL(toggled(bool )), this, SLOT(toggleAdvanced(bool )));
	ButLayout->addWidget(TBAdvanced);

	TBCreateDB = new QToolButton(this);
	TBCreateDB->setText(tr("Create Database"));
	TBCreateDB->setIcon(stIcon(SIconFactory::EditAdd));
	TBCreateDB->setIconSize(QSize(32,32));
	TBCreateDB->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(TBCreateDB, SIGNAL(clicked()), this, SLOT(butCreatePressed()));
	connect(TBCreateDB, SIGNAL(clicked()), this, SLOT(accept()));
	ButLayout->addWidget(TBCreateDB);
	
	QToolButton* TBOk = new QToolButton(this);
	TBOk->setText(tr("Ok"));
	TBOk->setIcon(stIcon(SIconFactory::Accept));
	TBOk->setIconSize(QSize(32,32));
	TBOk->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(TBOk, SIGNAL(clicked()), this, SLOT(accept()));
	ButLayout->addWidget(TBOk);

	QToolButton* TBCancel = new QToolButton(this);
	TBCancel->setText(tr("Cancel"));
	TBCancel->setIcon(stIcon(SIconFactory::Cancel));
	TBCancel->setIconSize(QSize(32,32));
	TBCancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(TBCancel, SIGNAL(clicked()), this, SLOT(reject()));
	ButLayout->addWidget(TBCancel);
			
	MLayout->addLayout(ButLayout);
	
	NormalLabPalette = LabDbName->palette();
	connect(LeDbName, SIGNAL(textChanged(const QString& )), this, SLOT(dbNameChanged(const QString& )));
	setAllowCreate(false);
}


void SDBSettingsDialog::setWarningMessage(const QString& _WarningMessage)
{
	if (!_WarningMessage.isEmpty())
	{
		setState(SDBSettingsDialog::StateWarning);
		LabWarning->setText("<h2><center>! Warning !</center></h2><br/>" + _WarningMessage);
	}
}

void SDBSettingsDialog::setState(SDBSettingsDialog::EnState _State)
{
	State = _State; 
	switch (_State)
	{
		case SDBSettingsDialog::StateNormal:
			LabIcon->setPixmap(stIcon(SIconFactory::DatabaseSettings).pixmap(QSize(32, 32)));
			LabWarning->hide();
		break;
		case SDBSettingsDialog::StateWarning:
			LabIcon->setPixmap(stIcon(SIconFactory::Warning).pixmap(QSize(64, 64)));
			LabWarning->show();
		break;
	}
}

void SDBSettingsDialog::setAllowCreate(bool _Value)
{
	TBCreateDB->setVisible(_Value);
}

void SDBSettingsDialog::setInvalidValue(SDBSettingsDialog::EnValTypes _ValueType,  bool _Value)
{
	switch (_ValueType)
	{
		case DbName:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("databasename"))), _Value);
		break;
		case Driver:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("driver"))), _Value);
		break;
		case UserName:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("username"))), _Value);
		break;
		case Password:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("passwd"))), _Value);
		break;
		case Host:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("host"))), _Value);
		break;
		case Port:
			setLabelStyle(DBCWidget->sectionLabel(Model->index(Settings.keyFullName("port"))), _Value);
		break;
	}
}

/*! Si s'accepta el dialeg d'emplenen els settings.
\return EnDialogCode::Accepted si s'ha acceptat 
EnDialogCode::Rejected si s'ha cancelat,
EnDialogCode::CreateDB si es vol crear la base de dades.
*/
int SDBSettingsDialog::getSettings()
{
	ButCreatePressed = false;
	int Res = exec();
	
	if (Res == QDialog::Accepted)
	{
// 		Mapper->submit();
// 		Model->submit();
		if (ButCreatePressed)
			Res = CreateDB;
	}
	else
	{
		//Mapper->revert();
		Model->revert();
	}
	return Res;
}


void SDBSettingsDialog::toggleAdvanced(bool _Toggled)
{
	GBAdvanced->setVisible(_Toggled);
	LabDbName->setVisible(!_Toggled);
	LeDbName->setVisible(!_Toggled);
	if (_Toggled)
	{
		TBAdvanced->setIcon(stIcon(SIconFactory::AdvancedUp));
		NoAdvancedHeight = 250;//FIXME: This is not a good resizing policy.
		setMinimumHeight( NoAdvancedHeight + DBCWidget->sizeHint().height());
	}
	else 
	{
		TBAdvanced->setIcon(stIcon(SIconFactory::Advanced));
		setMinimumHeight(NoAdvancedHeight);
		resize(width(), minimumHeight());
	}
}


void SDBSettingsDialog::dbNameChanged(const QString& _Text)
{
	setLabelStyle(LabDbName, _Text.isEmpty());
}
void SDBSettingsDialog::butCreatePressed()
{
	ButCreatePressed = true;
}
