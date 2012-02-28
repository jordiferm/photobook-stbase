/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#ifndef FDATABASEMANAGER_H
#define FDATABASEMANAGER_H
#include <QMap>
#include "fsqldatabasesettings.h"
#include "fsqlmetadata.h"
#include "sterror.h"
#include "stmetadataexport.h"

/**
Classe amb m�todes per a la creaci� i actualitzaci� de la base de dades a partir de la MetaData.
Mant� uns settings de la base de dades. 
Permet tenir una col.lecci� est�tica de inst�ncies FSqlDbTableManager, aix� �s �til quan tenim v�ris managers per a diferents settings i 
no volem passar cada vegada els settings als m�todes est�tics.
Cada manager est�tic s'identifica per un nom i es pot obtenir amb el m�tode FSqlDatabaseManager::manager() podem afegir managers est�tics 
amb addManager() i borrar-los amb removeManager(); 

D'altre banda la funcionalitat base de FSqlDatabaseManager �s actualitzar de forma autom�tica la base de dades a partir de la metainformaci� amb updateDB(), tamb� podem tenir acc�s a les estructures de metainformaci� amb tableMetadata().

\example 
	FSqlDatabaseSettings MSettings("testconnection")
	QSqlDatabase DB = FSqlDatabase::start(MSettings);
	FSqlDatabaseManager::addManager("test", FSqlDatabaseManager(DB, MSettings.metadataPath()));
	(...)
	FSqlDatabaseManager::manager("test").updateDB();

\example 
	
	Exemple d'utilitzaci� de forma est�tica: 
	
	FSqlDatabaseSettings Settings; 
	if (FSqlDatabase::start(Settings))
	{
		FSqlDatabaseManager::update(Settings)
	}
	FSqlDatabaseManager::manager().tableMetadata("tablename"); 
	// Aqui el manager utilitzar� els defaultSettings.
	// Si volem que el manager utilitzi ens nostres settings haurem d'afegir-lo amb addManager: 
	
	
	

	Exemple d'utilitzaci� de forma din�mica: 
	
	FSqlDatabaseSettings Settings("test_connection"); //Settings de la conexio "test_connection".
	QSqlDatabase DB = FSqlDatabase::start(Settings);
	if (DB.isConnected())
	{
		FSqlDatabaseManager Manager(DB, Settings.metadataPath());
		Manager.updateDB();
		FSqlDatabaseManager::addManager(Manager); //Afegim amb el nom default.
	}
	(...)
	
	FSqlDatabaseManager::manager().tableMetadata("tablename");


@author Shadow
*/
class FSqlDbTableManager;
class ST_METADATA_EXPORT FSqlDatabaseManager
{
	static QMap<QString, FSqlDatabaseManager> ManagerList;

	QSqlDatabase Database;
	QString MetadataPath;
	static QString DefaultManager; 
	
public:
	FSqlDatabaseManager(const FSqlDatabaseSettings& _Settings);
	FSqlDatabaseManager(QSqlDatabase _Database = QSqlDatabase(), const QString& _MetadataPath = "");
	bool updateDB();
	FSqlDbTableManager* newTableManager(const FTableMetaData& _MetaData);
	FTableMetaData tableMetaData(const QString& _TableName) const;
	QString tableFile( const QString& _TableName);
	QSqlDatabase database() const { return Database; }
	
	// M�todes est�tics, li hem de passar els settings.	
	static bool updateDB(QSqlDatabase _Database, const QString& _MetadataPath);
	static bool updateDB(const FSqlDatabaseSettings& _Settings);
	// El crea segons el driver.
	static FSqlDbTableManager* newTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database);
	static FTableMetaData tableMetaData(const QString& _TableName, const QString& _MetadataPath);
	static QString tableFile( const QString& _TableName, const QString& _MetadataPath);
	
	static FSqlDatabaseManager manager(const QString& _Name = DefaultManager);
	static void addManager(const FSqlDatabaseManager& _NewManager, const QString& _Name = DefaultManager);
	static void removeManager(const QString& _Name = DefaultManager);
		
};



/**
   Creaci� i actualitzacio d'una taula a partir de la metainformaci�.
	- Deixa els errors en el log.
*/
class ST_METADATA_EXPORT FSqlDbTableManager
{
public: 
	ST_DECLARE_ERRORCLASS();

protected:	
	FTableMetaData MetaData;
	QSqlDatabase Database;

public: 
	FSqlDbTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database);
	virtual ~FSqlDbTableManager() {}
	virtual void create() = 0;
	void alterTable(const FTableMetaData& _OldMetaData);
	void ensureHasMetadataTable();
	void update();
	bool execUpdateInstrList(const FTableMetaData::TUpdateStrInstList& _BefUpdateList, 
		const FTableMetaData& _OldMetaData);
	virtual void backupTable() = 0;
	//! alter table especific per bd
	virtual void dbAlterTable(const FTableMetaData& _OldMetaData) = 0;
};


class ST_METADATA_EXPORT FSqlPsqlDbTableManager : public FSqlDbTableManager 
{
public:
	FSqlPsqlDbTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database)
		: FSqlDbTableManager(_MetaData, _Database) {}
		
	QString columnNameAndType(const FFieldMetaData& _Field);
	QString columnSql(const FFieldMetaData& _Field);
	void create();
	void backupTable();
	void dbAlterTable(const FTableMetaData& _OldMetaData);
};

class ST_METADATA_EXPORT FSqlSqliteDbTableManager : public FSqlDbTableManager 
{
public:
	FSqlSqliteDbTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database)
		: FSqlDbTableManager(_MetaData, _Database) {}
		
	QString columnNameAndType(const FFieldMetaData& _Field);
	QString columnSql(const FFieldMetaData& _Field);
	void create();
	void backupTable();
	void dbAlterTable(const FTableMetaData& _OldMetaData);
};
#endif
