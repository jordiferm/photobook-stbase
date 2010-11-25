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

#include "fdbconfigwidget.h"

#include <QTabWidget>
#include <QLayout>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QFileDialog> 
#include <QPushButton> 
#include <QCoreApplication> 
#include "fsqldatabasesettings.h"
#include "smappablecombobox.h"
#include "fileselectlineedit.h"
#include "fsqldatabase.h"
#include "smessagebox.h"


// _________________________________________________________________________*/
//
//	Class FSingleDBConfigWidget
// _________________________________________________________________________*/

FSingleDBConfigWidget::FSingleDBConfigWidget(FSqlDatabaseSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent) 
 : SMappedWidget(_Settings.model(), _Mapper, _Parent), ConnectionName(_Settings.connectionName())
{
	SAppSettingsModel* Model = _Settings.model();
	Model->fetch();
	QGridLayout* MLayout = new QGridLayout(this);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("databasename"))), MLayout->rowCount(), 0);
	//MLayout->addWidget(createEditor(Model->index(_Settings.keyFullName("databasename"))), MLayout->rowCount() - 1, 1);

	FileSelectLineEdit* LEDBPath = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LEDBPath, Model->index(_Settings.keyFullName("databasename")).column());
	MLayout->addWidget(LEDBPath, MLayout->rowCount() - 1, 1);

	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("driver"))), MLayout->rowCount(), 0);
	SMappableComboBox* CBDriver = new SMappableComboBox(this); 
	CBDriver->setUseTextAsData(true);
	CBDriver->addItems(QSqlDatabase::drivers());
	_Mapper->addMapping(CBDriver, Model->index(_Settings.keyFullName("driver")).column());
	MLayout->addWidget(CBDriver, MLayout->rowCount() - 1, 1);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("username"))), MLayout->rowCount(), 0);
	MLayout->addWidget(createEditor(Model->index(_Settings.keyFullName("username"))), MLayout->rowCount() - 1, 1);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("passwd"))), MLayout->rowCount(), 0);
	MLayout->addWidget(createEditor(Model->index(_Settings.keyFullName("passwd"))), MLayout->rowCount() - 1, 1);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("host"))), MLayout->rowCount(), 0);
	MLayout->addWidget(createEditor(Model->index(_Settings.keyFullName("host"))), MLayout->rowCount() - 1, 1);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("port"))), MLayout->rowCount() , 0);
	MLayout->addWidget(createEditor(Model->index(_Settings.keyFullName("port"))), MLayout->rowCount() - 1, 1);

	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("metadatapath"))), MLayout->rowCount() , 0);
	FileSelectLineEdit* LEPath = new FileSelectLineEdit(FileSelectLineEdit::GetExistingDirectory, this);
	_Mapper->addMapping(LEPath, Model->index(_Settings.keyFullName("metadatapath")).column());
	MLayout->addWidget(LEPath, MLayout->rowCount() - 1, 1);
	
	MLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("dumpbinarypath"))), MLayout->rowCount() , 0);
	FileSelectLineEdit* LEDumpPath = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LEDumpPath, Model->index(_Settings.keyFullName("dumpbinarypath")).column());
	MLayout->addWidget(LEDumpPath, MLayout->rowCount() - 1, 1);
	
	QPushButton* ButBackup = new QPushButton(tr("Backup"), this);
	MLayout->addWidget(ButBackup, MLayout->rowCount(), 0);
	connect(ButBackup, SIGNAL(clicked( bool )), this , SLOT(slotBackupDb()));
	
	QPushButton* ButRestore = new QPushButton(tr("Restore"), this);
	MLayout->addWidget(ButRestore, MLayout->rowCount() - 1, 1);
	connect(ButRestore, SIGNAL(clicked( bool )), this , SLOT(slotRestoreDb()));
	
	MLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding), MLayout->rowCount(), 0);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	//setMinimumHeight(250);
}
	
QSize FSingleDBConfigWidget::sizeHint() const
{
	return QSize(400, 350);
}

void FSingleDBConfigWidget::slotBackupDb()
{
	QString FileName = QFileDialog::getSaveFileName(this, tr("Database backup file"),
										QCoreApplication::applicationDirPath(),
										tr("Sql files (*.sql)"));
	FSqlDatabaseSettings Settings(ConnectionName);
	try
	{
		FSqlDatabase::backup(Settings, FileName); 
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error in database backup."), _Error.description());
	}
}

void FSingleDBConfigWidget::slotRestoreDb()
{
	if (SMessageBox::question(this, tr("Restoring database"), tr("All the data in current database will be lost. Are you sure you want to restore ?"), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes)
	{
		QString FileName = QFileDialog::getOpenFileName(this, tr("Database backup file"),
											QCoreApplication::applicationDirPath(),
											tr("Sql files (*.sql)"));
		FSqlDatabaseSettings Settings(ConnectionName);
	
		try
		{
			FSqlDatabase::restore(Settings, FileName); 
			SMessageBox::information(this, tr("Restoring database"), tr("Database restored OK"));
		}
		catch (STError& _Error)
		{
			SMessageBox::critical(this, tr("Error in database backup."), _Error.description());
		}
	}
}

// _________________________________________________________________________*/
//
//	Class FDBConfigWidget
// _________________________________________________________________________*/


FDBConfigWidget::FDBConfigWidget(QDataWidgetMapper* _Mapper, QWidget* _Parent)
 : QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
	TWidget = new QTabWidget(this); 
	MLayout->addWidget(TWidget);
	FSqlDatabaseSettings DefSettings;
	FSingleDBConfigWidget* ConfWidget = new FSingleDBConfigWidget(DefSettings, _Mapper, this);
	TWidget->addTab(ConfWidget, tr("Default"));
	QStringList DBConnections = FSqlDatabaseSettings::connections();
	QStringList::iterator it; 
	for (it = DBConnections.begin(); it != DBConnections.end(); ++it)
	{
		FSqlDatabaseSettings NSettings(*it);
		FSingleDBConfigWidget* ConfWidget = new FSingleDBConfigWidget(NSettings, _Mapper, this);
		TWidget->addTab(ConfWidget, *it);
	}
}	

