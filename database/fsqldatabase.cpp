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

#include "fsqldatabase.h"
#include "sdbsettingsdialog.h"
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase> 
#include <QDataWidgetMapper> 
#include <QProcess>
#include <QFile>
#include <QTextStream> 
#include "fsqlquery.h"
#include "stlog.h"


//TODO: Execute _SqlInitFile
//TODO: Translate comments.
/*! 
	Afegeix una base de dades utilitzant els settings. 
	Si els _Settings no son valids mira de carregar-los amb QSettings.
	Si no els pot carregar els demana a l'usuari. 
	Si l'usuari entra un _Settings correctes els guarda a la configuració de l'aplicació.
	S'ha de tenir en compte que es guarden settings diferents per a cada connectionName. 
		
	\example 
	// Crearà la base de dades per defecte.
	FSqlDatabase::start(); 
	// Per recuperar la base de dades ho farem:
	QSqlDatabase::database();
	
	// Crearà una base de dades amb el nom de conexió "testdb"
	FSqlDatabase::start(FSqlDatabaseSettings("testdb")); 
	// Per recuperar la base de dades ho farem:
	QSqlDatabase::database("testdb");
	
	\return 
	Retorna true si estem conectats.
*/

bool FSqlDatabase::start(FSqlDatabaseSettings& _Settings, const QString& _SqlInitFile)
{
	bool Connected = false;
	QSqlDatabase NDatabase;
	

	NDatabase = _Settings.createDatabase();
	
	bool GetAccepted = true;
	bool AskForCreation = false;
	
	QString WarnMessage;
	// Ask settings untill it will be valid or the user cancel operation.
	while (!(_Settings.isValid() && NDatabase.isValid() && Connected) && GetAccepted)
	{
		bool AskSettings = true;
		QDataWidgetMapper Mapper;
		Mapper.setModel(_Settings.model());
		SDBSettingsDialog SWidget(_Settings, &Mapper);
		_Settings.model()->fetch();
		Mapper.toFirst();
		
		SWidget.setAllowCreate(AskForCreation);
		AskForCreation = false;
		if (!_Settings.isValid())
			SWidget.setWarningMessage(QApplication::tr("Connection settings are invalid or never assigned. \n"
			                           "Please correct the above settings to connect to database"));
		else 
		{	
			if (!NDatabase.isValid())
			{
				SWidget.setWarningMessage(QApplication::tr("Failed to load Database driver") + ":' \n" + NDatabase.driverName() + "'." +
													QApplication::tr("Please be sure that your system configuration is correct."));
				SWidget.setInvalidValue(SDBSettingsDialog::Driver, true);
			}
			else //Es que no estem conectats
			{
				SWidget.setWarningMessage(WarnMessage);
				AskSettings = !WarnMessage.isEmpty(); //Si no hi ha queixa de la conexió vol dir que no hem pas de demanar res de nou.
			}
		}
		int DResult;
		DResult = SDBSettingsDialog::Accepted;
		
		if (AskSettings)
		{
			DResult = SWidget.getSettings();
			
			GetAccepted = (DResult != SDBSettingsDialog::Rejected);
			if (GetAccepted)
			{
				Mapper.submit();
				_Settings.model()->submit();
			}
			else 
				_Settings.model()->revert();
			//! L'hem d'anar creant perque ens poden haver canviat el Driver.
			NDatabase = _Settings.createDatabase();
		}
		
		//Si tot e correcte provem de conectar.
		if (_Settings.isValid() && NDatabase.isValid() && GetAccepted)
		{
			AskForCreation = true;
			bool TryConnect = true;
			//Si l'hem de crear la creem primer.
			if (DResult == SDBSettingsDialog::CreateDB)
				TryConnect = create(_Settings, WarnMessage, _SqlInitFile);

			// Ara ja tenim una configuració correcte i la base de dades existeix. Provem de conectar:
			if (TryConnect)
			{
				Connected = NDatabase.open();
				if (!Connected)
				{
					WarnMessage = QApplication::tr("Failed to open database connection: <br/>") + NDatabase.lastError().databaseText(); 
				}
			}
		}
	}
	return Connected;
}


//! Assumeix que els _Settings Son correctes.
bool FSqlDatabase::create(const FSqlDatabaseSettings& _Settings, QString& _WarningMessage, const QString& _SqlInitFile)
{
	bool Res = false;
	if (_Settings.driver().left(5) == "QPSQL") //Es postgres.
	{
		QSqlDatabase TmpDb = QSqlDatabase::addDatabase(_Settings.driver(), "tmpconnection");
		_Settings.configure(TmpDb);
		TmpDb.setDatabaseName("template1"); 
		
		if (!TmpDb.open())
			_WarningMessage = QApplication::tr("user: ") + _Settings.userName() + QApplication::tr(" has not enought privileges to create "
			"databases.\n Please contact your system administrator.");
		else
		{
			TmpDb.exec("CREATE DATABASE " + _Settings.databaseName() + " WITH ENCODING = 'UTF8';");
			if(TmpDb.lastError().type()!= QSqlError::NoError)
			{
				if( TmpDb.lastError().databaseText().contains("permission denied") )
					_WarningMessage = QApplication::tr("user: ") + _Settings.userName() + QApplication::tr(" has not enought privileges \n "
						"Please contact your system administrator.");
				else
					_WarningMessage = QApplication::tr("Error creating database '") + _Settings.databaseName() + "':"
											+ "\n" + TmpDb.lastError().driverText() + "<br/>" + TmpDb.lastError().databaseText();
			}
			else
				Res = true;
			TmpDb.close();
			QSqlDatabase::removeDatabase("tmpconnection");
		}
	}
	//SUPPORT PER MYSQL A LA BD. ( Alerta només hi ha implementada la part de la connexió no la part de creació i
	//gestió de taules.
	else if ( _Settings.driver().left(6) == "QMYSQL" )
	{
		QSqlDatabase TmpDb = QSqlDatabase::addDatabase(_Settings.driver(), "tmpconnection");
		_Settings.configure(TmpDb);
		TmpDb.setDatabaseName("test"); 
		if (!TmpDb.open())
			_WarningMessage = QApplication::tr("user: ") + _Settings.userName() + QApplication::tr(" has not enought privileges to create "
			"databases.\n Please contact your system administrator.");
		else
		{
			TmpDb.exec("CREATE DATABASE " + _Settings.databaseName() + ";");
			if(TmpDb.lastError().type()!= QSqlError::NoError)
			{
				if( TmpDb.lastError().databaseText().contains("permission denied") )
					_WarningMessage = QApplication::tr("user: ") + _Settings.userName() + QApplication::tr(" has not enought privileges \n "
						"Please contact your system administrator.");
				else
					_WarningMessage = QApplication::tr("Error creating database '") + _Settings.databaseName() + "':"
											+ "\n" + TmpDb.lastError().driverText() + "<br/>" + TmpDb.lastError().databaseText();
			}
			else
				Res = true;
	
			TmpDb.close();
			QSqlDatabase::removeDatabase("tmpconnection");
		}
	}
	else if ( _Settings.driver().left(7) == "QSQLITE" )
	{
		QSqlDatabase TmpDb = QSqlDatabase::addDatabase(_Settings.driver(), "tmpconnection");
		_Settings.configure(TmpDb);
		if (!TmpDb.open())
			_WarningMessage = QApplication::tr("user: ") + _Settings.userName() + QApplication::tr(" has not enought privileges to create "
			"databases.\n Please contact your system administrator.");
		else 
			TmpDb.close();
		TmpDb.close();
		QSqlDatabase::removeDatabase("tmpconnection");
	}
	
	return Res;
}

void FSqlDatabase::backup(const FSqlDatabaseSettings& _Settings, QString& _FileName)
{
	if (_Settings.driver().contains("PSQL"))
	{
		QStringList Args; 
		if (!_Settings.host().isEmpty())
			Args << "-h" << _Settings.host();
		Args << "-p" << QString::number(_Settings.port());
		if (!_Settings.userName().isEmpty())
			Args << "-U" << _Settings.userName(); 
		if (!_Settings.passwd().isEmpty())
			Args << "-W" << _Settings.passwd();
		
		Args << "-a" << "-D"; //DataOnly and inserts with column names
		Args << "-f" << _FileName;
		Args << _Settings.databaseName();		
		
		Assert(QProcess::execute(_Settings.dumpBinaryPath(), Args) == 0, 
				Error(QString(QObject::tr("Error executing command %1")).arg(
				_Settings.dumpBinaryPath() + " " + Args.join(" "))));
	}
	else 
	if (_Settings.driver().contains("SQLITE"))
	{
		QStringList Args; 
		Args << _Settings.databaseName();
		Args << ".dump";
		
		QProcess SQLite; 
		SQLite.setProcessChannelMode(QProcess::MergedChannels);
		SQLite.start(_Settings.dumpBinaryPath(), Args);
		Assert(SQLite.waitForFinished(), 
				 Error(QString(QObject::tr("Error executing command %1 : %2")).arg(
					_Settings.dumpBinaryPath() + " " + Args.join(" ")).arg(SQLite.errorString())));
		QFile FOut(_FileName);
		Assert(FOut.open(QFile::WriteOnly | QFile::Truncate), 
				Error(QString(QObject::tr("Error opening file %1")).arg(_FileName)));
		QTextStream Out(&FOut);
		Out << SQLite.readAll();
		FOut.close();
	}
	else 
		throw Error(QString(QObject::tr("Database driver %1 is not compatible for backups.")).arg(_Settings.driver()));
	
}

void FSqlDatabase::deleteAllTables(const FSqlDatabaseSettings& _Settings)
{
	QSqlDatabase CurrDb = _Settings.database(); 
	QStringList Tables = CurrDb.tables(); 
	QStringList::iterator it; 
	for (it = Tables.begin(); it != Tables.end(); ++it)
	{
		FSqlQuery Query(CurrDb);
		Query.prepare("DELETE FROM " + *it);
		Query.exec();
	}
}

void FSqlDatabase::restore(const FSqlDatabaseSettings& _Settings, QString& _FileName)
{
	deleteAllTables(_Settings);
	QFile BackupFile(_FileName);
	Assert(BackupFile.open(QFile::ReadOnly), 
			Error(QString(QObject::tr("Error opening file: '%1'")).arg(_FileName)));
	QTextStream StrmIn(&BackupFile);
	QString CurrLine = StrmIn.readLine(); ; 
	FSqlQuery Query(_Settings.database());
	bool OneOpWithNoError = false;
	bool SQLite = _Settings.driver().contains("SQLITE");
	while (!CurrLine.isNull())
	{
		if (CurrLine.toUpper().contains("INSERT"))
		{
			CurrLine = CurrLine.replace("'true'", "true");
			CurrLine = CurrLine.replace("'false'", "false");
			if (SQLite)
			{
				CurrLine = CurrLine.replace("true", "'true'");
				CurrLine = CurrLine.replace("false", "'false'");
			}
			Query.prepare(CurrLine);
			try
			{
				Query.exec();
				OneOpWithNoError = true;
			}	
			catch(FSqlQuery::Error& _Error)
			{
				stLog << ISTLogIO::L2 << QObject::tr("Error executing query") << _Error.description() << ISTLogIO::Endl; 
			}
		}
		CurrLine = StrmIn.readLine(); 
	} 
	Assert(OneOpWithNoError, Error(QObject::tr("There was errors restoring database.")));
	BackupFile.close();
}

