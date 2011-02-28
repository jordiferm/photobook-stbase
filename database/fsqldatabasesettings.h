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
#ifndef FSQLDATABASESETTINGS_H
#define FSQLDATABASESETTINGS_H

#include <QString>
#include <QSqlDatabase>
#include "sappsettings.h"
#include "stdatabaseexport.h"


/**
Classe per a manejar settings d'una base de dades.
Necessita la base de dades per a construir-se ja que utilitza el nom de la conexió per a poder guardar a QSettings la informació de vàries bases de dades.

@author Shadow
*/
class QSqlDatabase;
class ST_DATABASE_EXPORT FSqlDatabaseSettings : public SAppSettings
{
Q_OBJECT
	QString ConnectionName; 
	static QString DefaultDatabasePath;
	static bool EmbededResources;

public:
	FSqlDatabaseSettings(const QString& _ConnectionName = "", QObject* _Parent = 0);
	QString keyFullName(const QString& _Key) const;
	void setConnectionName(const QString& _ConnectionName);	
	QString connectionName() const {return ConnectionName;}
	//! list of different connections in Settings.
	static QStringList connections();
	void setDatabaseName(const QString& _Value);
	QString databaseName() const;
 	void setDriver(const QString& _Value);
 	QString driver() const;
	void setUserName(const QString& _Value);
	QString userName() const;
	void setPasswd(const QString& _Value);
	QString passwd() const;
	void setHost(const QString& _Value);
	QString host() const;
	void setPort(int _Value);
	int port() const;
	void setMetadataPath(const QString& _DataPath);
	QString metadataPath() const;
	void setDumpBinaryPath(const QString& _Value);
	QString dumpBinaryPath() const;
	void setResourcePrefix(const QString& _Prefix);
	QString resourcePrefix() const;
	
	//! Retorna false si algun dels valors requerits falta o és NULL.
	bool isValid() const;
	//! Crea una base de dades tinguent en compte el nom de la conexió.
	QSqlDatabase createDatabase() const;
	//! Retorna la base de dades corresponent a settings a partir del nom del a conexió.
	QSqlDatabase database() const;
	
	//! Configura la base de dades obviant el nom de la conexió i el tipus.
	void configure(QSqlDatabase& ) const;
	
	static QString defaultUserName(); 
	static QString defaultHost();
	static QString defaultDriver();
	static int defaultPort();
	static void setDefaultMetadataPath(const QString& _Value) { DefaultDatabasePath = _Value; }
	static QString defaultMetadataPath();
	static QString defaultDumpBinaryPath();
	void setDefaultSettings(); 
	static bool embededResources() { return EmbededResources; }
	//! If True, ST Database system look for .mtd files in resources.
	static void setEmbededResources(bool _Value) { EmbededResources = _Value; }
};

#endif
