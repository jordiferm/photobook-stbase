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

#ifndef SDBSETTINGSDIALOG_H
#define SDBSETTINGSDIALOG_H

#include <QDialog>
#include "stdatabaseexport.h"



/**
Dialog widget to ask user Database Settings.

@author Shadow
*/
class QDataWidgetMapper;
class SAppSettingsModel; 
class FSqlDatabaseSettings;
class QToolButton;
class QGroupBox;
class QLineEdit; 
class QLabel;
class FSingleDBConfigWidget;
class ST_DATABASE_EXPORT SDBSettingsDialog : public QDialog
{
	Q_OBJECT

public: 
	enum EnState
	{
		StateNormal, 
		StateWarning
	};
	
	enum EnDialogCode
	{
		Accepted = QDialog::Accepted, 
		Rejected = QDialog::Rejected, 
		CreateDB = QDialog::Accepted + 10
	};
	
	enum EnValTypes
	{
		DbName,
		Driver, 
		UserName,
		Password, 
		Host,
		Port
	};

private:	
	
	QToolButton* TBAdvanced;
	QToolButton* TBCreateDB;
	QGroupBox*   GBAdvanced;
	QLineEdit* LeDbName;
	QLabel* LabDbName;
	QLabel* LabIcon;
	QLabel* LabWarning;
	QPalette NormalLabPalette;
	EnState State;
	bool ButCreatePressed;
	QDataWidgetMapper* Mapper;
	SAppSettingsModel* Model;
	FSqlDatabaseSettings& Settings;
	FSingleDBConfigWidget* DBCWidget;
	int NoAdvancedHeight;
	
private: 
	void setLabelStyle(QLabel* _Label, bool _Error);
	
public:
	SDBSettingsDialog(FSqlDatabaseSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget *parent = 0);
	void setWarningMessage(const QString& _WarningMessage);
	void setState(SDBSettingsDialog::EnState _State);
	//! Si es true ensenya el botó de creació de la BD.
	void setAllowCreate(bool _Value);
	//! Marca la label _ValueType com a invàlida depenent de _Value.
	void setInvalidValue(SDBSettingsDialog::EnValTypes _ValueType,  bool _Value);
	int getSettings();
	
public slots: 
	void toggleAdvanced(bool );
	void dbNameChanged(const QString& );
	void butCreatePressed();
	
};

#endif
