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
#include "fsqldatabasesettings.h"
#include <QDir>
#include <QSettings>
#include <QApplication>

/*! 
	Settings per la base de dades amb el nom de conexio _ConnectionName.
	Si no li passem el nom de conexió interpreta que és la conexió per defecte. 
	\sa QSqlDatabase 
*/

QString FSqlDatabaseSettings::DefaultDatabasePath = ""; 


FSqlDatabaseSettings::FSqlDatabaseSettings(const QString& _ConnectionName, QObject* _Parent) : SAppSettings(_Parent) ,  ConnectionName(_ConnectionName)
{
	if (!keySaved(keyFullName("databasename")))
	{
		addKey(keyFullName("databasename"),  "", QVariant::String, 
				tr("Database Name"), tr("Name of the database to conect to."));
		addKey(keyFullName("driver"),  defaultDriver(), QVariant::String, 
				tr("Driver"), tr("Qt Driver to connect to a database."));
		addKey(keyFullName("username"),  defaultUserName(), QVariant::String, 
				tr("User Name"), tr("The username for the database account."));
		addKey(keyFullName("passwd"),  "", QVariant::String, 
				tr("Password"), tr("Connection account password."));
		addKey(keyFullName("host"),  defaultHost(), QVariant::String, 
				tr("Host"), tr("Database host."));
		addKey(keyFullName("port"),  defaultPort(), QVariant::Int, 
				tr("Port"), tr("Host port to connect to."));
		addKey(keyFullName("metadatapath"),  defaultMetadataPath(), QVariant::String, 
				tr("Metadata path"), tr("Path of metadata location."));
		addKey(keyFullName("dumpbinarypath"),  defaultDumpBinaryPath(), QVariant::String, 
				tr("Path of dump tool"), tr("Path of dump tool for backups."));
		SettingsModel->fetch();
	}
}	
	
QString FSqlDatabaseSettings::keyFullName(const QString& _Key) const
{
	QString Res = "/database/";
	if (!ConnectionName.isEmpty())
		Res += ConnectionName + "/";		 
			
	Res += _Key;
	return Res;
}	

void FSqlDatabaseSettings::setConnectionName(const QString& _ConnectionName)
{
	ConnectionName = _ConnectionName;
}

/*!
	Retorna una llista de totes les conexions que hi ha a més de la Default.
	O sigui els childGroups de database.
*/
QStringList FSqlDatabaseSettings::connections()
{
	QSettings Settings;
	Settings.beginGroup("database");
	return Settings.childGroups();
}

void FSqlDatabaseSettings::setDatabaseName(const QString& _Value) 
{
	SettingsModel->setData(SettingsModel->index(keyFullName("databasename")), _Value);
}

QString FSqlDatabaseSettings::databaseName() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("databasename"))).toString();
}

void FSqlDatabaseSettings::setDriver(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("driver")), _Value);
}		
		
QString FSqlDatabaseSettings::driver() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("driver"))).toString();
}

void FSqlDatabaseSettings::setUserName(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("username")), _Value);
}
		
QString FSqlDatabaseSettings::userName() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("username"))).toString();
}
		
void FSqlDatabaseSettings::setPasswd(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("passwd")), _Value);
}
		
QString FSqlDatabaseSettings::passwd() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("passwd"))).toString();
}
		
void FSqlDatabaseSettings::setHost(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("host")), _Value);
}
		
QString FSqlDatabaseSettings::host() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("host"))).toString();
}
		
void FSqlDatabaseSettings::setPort(int _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("port")), _Value);
}
		
int FSqlDatabaseSettings::port() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("port"))).toInt();
}
		
void FSqlDatabaseSettings::setMetadataPath(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("metadatapath")), _Value);
}
		
QString FSqlDatabaseSettings::metadataPath() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("metadatapath"))).toString();
}
		
void FSqlDatabaseSettings::setDumpBinaryPath(const QString& _Value)
{
	SettingsModel->setData(SettingsModel->index(keyFullName("dumpbinarypath")), _Value);
}

QString FSqlDatabaseSettings::dumpBinaryPath() const
{
	return SettingsModel->data(SettingsModel->index(keyFullName("dumpbinarypath"))).toString();
}


bool FSqlDatabaseSettings::isValid() const
{
	return !databaseName().isEmpty() && !driver().isEmpty();
}

QSqlDatabase FSqlDatabaseSettings::createDatabase() const
{
	QSqlDatabase Res;
	if (ConnectionName.isEmpty())
		Res = QSqlDatabase::addDatabase(driver());
	else 
		Res = QSqlDatabase::addDatabase(driver(), ConnectionName);
		
	configure(Res);	
	return Res;
}

QSqlDatabase FSqlDatabaseSettings::database() const
{
	if (ConnectionName.isEmpty())
		return QSqlDatabase::database(); 
	else 
		return QSqlDatabase::database(ConnectionName);
}


void FSqlDatabaseSettings::configure(QSqlDatabase& _Database) const  
{
	_Database.setDatabaseName(databaseName()); 
	_Database.setUserName(userName()); 
	_Database.setPassword(passwd()); 
	_Database.setHostName(host()); 
	_Database.setPort(port());
}


QString FSqlDatabaseSettings::defaultUserName()
{
	return QDir().home().dirName();
}

QString FSqlDatabaseSettings::defaultHost()
{
	return "localhost";
}

QString FSqlDatabaseSettings::defaultDriver()
{
	return "QSQLITE";
}

int FSqlDatabaseSettings::defaultPort()
{
	return 5432; 
}

QString FSqlDatabaseSettings::defaultMetadataPath() 
{
	if (DefaultDatabasePath.isEmpty())
	{
		#ifdef Q_OS_WIN32
			DefaultDatabasePath = QCoreApplication::applicationDirPath() + "/../share/st/database";
		#else 
			DefaultDatabasePath = "/usr/share/st/database/" + QCoreApplication::applicationName();
		#endif 
		//DefaultDatabasePath = QApplication::applicationDirPath() + "/share"; 
	}
	return DefaultDatabasePath; 
}

QString FSqlDatabaseSettings::defaultDumpBinaryPath()
{
	return "pg_dump";
}


void FSqlDatabaseSettings::setDefaultSettings()
{
	setUserName(defaultUserName()); 
	setHost(defaultHost());
	setDriver(defaultDriver());
	setPort(defaultPort());
}

