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

#include "fsqldatabasemanager.h"
#include "stlog.h"
#include <QStringList>
#include <QVariant>
#include <QApplication>
#include <QDateTime>
#include "mtdversion.h"
#include "fsqlquery.h"
#include <QProgressDialog>

// _________________________________________________________________________*/
//
// Class FDatabaseManager
// _________________________________________________________________________*/


/*!
 
	- Retorna true si s'ha fet l'update sense cap error.
 
	Deixa els errors en el log.
*/


QString FSqlDatabaseManager::DefaultManager = "flam_sql_default_manager";
QMap<QString, FSqlDatabaseManager> FSqlDatabaseManager::ManagerList;

FSqlDatabaseManager::FSqlDatabaseManager(const FSqlDatabaseSettings& _Settings) : 
		Database(_Settings.database()), MetadataPath(_Settings.metadataPath())
{
}

FSqlDatabaseManager::FSqlDatabaseManager(QSqlDatabase _Database, const QString& _MetadataPath) : 
		Database(_Database), MetadataPath(_MetadataPath)
{
}

bool FSqlDatabaseManager::updateDB()
{
	return updateDB(Database, MetadataPath);
}

FSqlDbTableManager* FSqlDatabaseManager::newTableManager(const FTableMetaData& _MetaData)
{
	return newTableManager(_MetaData, Database);
}

FTableMetaData FSqlDatabaseManager::tableMetaData(const QString& _TableName) const
{
	return tableMetaData(_TableName, MetadataPath);
}

QString FSqlDatabaseManager::tableFile( const QString& _TableName)
{
	return tableFile(_TableName, MetadataPath);
}

bool FSqlDatabaseManager::updateDB(QSqlDatabase _Database, const QString& _MetadataPath)
{
	bool Res = true;
	//Recollim la metainformació:
	//Mirem si s'han de crear o actualitzar els MTD que tenim:
	FSqlMetaData MetaData;
	MetaData.load(_MetadataPath + "/" + FSqlMetaData::dataSubDir(FSqlMetaData::TablePath));
	
	
	//Ensure that fmetadata table is created.
	if (!_Database.tables().contains("fmetadata"))
	{
		FTableMetaData MetadataTable;
		MetadataTable.load(tableFile("fmetadata", _MetadataPath));
		FSqlDbTableManager* TmpManager = FSqlDatabaseManager::newTableManager(MetadataTable, _Database);
		TmpManager->create();
		delete TmpManager;
	}
	
	//Per cada taula de les taules a metadades per ordre d'actualització
	QStringList::const_iterator strit;
	for (strit = MetaData.orderedTables.begin(); strit != MetaData.orderedTables.end(); ++strit)
	{
		FSqlMetaData::TTablesList::const_iterator it;
		it = MetaData.tables.find(*strit);
		QString ErrDesc;
		if (it->name != "fmetadata")
		{
			FSqlDbTableManager* CManager = NULL;
			try
			{
				CManager = newTableManager(*it, _Database);
				if (CManager)
				{
					CManager->update();
				}
				else
					Res = false;
			}
			catch (FSqlDbTableManager::Error& _Err)
			{
				Res = false;
				ErrDesc = _Err.description();
			}
			catch (FSqlQuery::Error& _Err)
			{
				Res = false;
				ErrDesc = _Err.description();
			}
			if (! ErrDesc.isEmpty())
			{
					
				stLog << ISTLogIO::L2 << ISTLogIO::Warning << "Error updating table: " << it->name << "->"
				<< ErrDesc << ISTLogIO::Endl;
			}
			if (CManager)
				delete CManager;
		}
	}
	return Res;
}

bool FSqlDatabaseManager::updateDB(const FSqlDatabaseSettings& _Settings)
{
	return updateDB(_Settings.database(), _Settings.metadataPath());
}


/*!
	\return 
	retorna un nou FSqlDbTableManager segons el nom del driver o NULL en cas de que no es reconegui el nom del driver
	o no hi hagi cap manager per aquest driver.
	\sa QSqlDatabase
*/

FSqlDbTableManager* FSqlDatabaseManager::newTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database)
{
	FSqlDbTableManager* Res = NULL;
	if (_Database.driverName().contains("PSQL"))
		Res = new FSqlPsqlDbTableManager(_MetaData, _Database);
	else
	if (_Database.driverName().contains("SQLITE"))
		Res = new FSqlSqliteDbTableManager(_MetaData, _Database);
	else		
		stLog << ISTLogIO::L0 << ISTLogIO::Warning << QApplication::tr("Unsuported Sql Driver: ") <<
		_Database.driverName() <<  ISTLogIO::Endl;

	return Res;
}

/*!
	\return 
	Retorna les metadades de la taula _TableName carregades.
*/

FTableMetaData FSqlDatabaseManager::tableMetaData(const QString& _TableName, const QString& _MetadataPath)
{
	FTableMetaData Res; 
	Res.load(tableFile(_TableName, _MetadataPath));
	return Res;
}

QString FSqlDatabaseManager::tableFile( const QString& _TableName, const QString& _MetadataPath)
{
	return _MetadataPath + "/" + FSqlMetaData::dataSubDir(FSqlMetaData::TablePath) + "/" + _TableName + ".mtd";
}
	
/*!
	Retorna el Manager amb el nom _Name, que ha d'haver estat previament afegit.
	Si no indiquem _Name
	Si no hem fet addManager(Manager) ens retornarà un Manager amb els DefaultSettings.
	Si hi hem afegit un Manager amb addManager ens retorna el defaultManager.
	Si indiquem un nom inexistent ens retornarà un Manager amb els DefaultSettings.
*/

FSqlDatabaseManager FSqlDatabaseManager::manager(const QString& _Name)
{
	FSqlDatabaseManager Res; 
	if (ManagerList.find(_Name) != ManagerList.end()) 
		Res = ManagerList[_Name];
	return Res;
}

/*!
	Afegeix un manager a la llista de managers. 
	Si el nom ja existeix aquest es substituirà per el nou _NewManager.
	Si no hi indiquem _Name s'insertarà el manager per defecte.
*/

void FSqlDatabaseManager::addManager(const FSqlDatabaseManager& _NewManager, const QString& _Name)
{
	ManagerList.insert(_Name, _NewManager);
}

void FSqlDatabaseManager::removeManager(const QString& _Name)
{
	ManagerList.take(_Name);
}



// _________________________________________________________________________*/
//
// Class FSqlDbTableManager
// _________________________________________________________________________*/

FSqlDbTableManager::FSqlDbTableManager(const FTableMetaData& _MetaData, QSqlDatabase _Database)
		: MetaData(_MetaData), Database(_Database)
{}


void FSqlDbTableManager::update()
{
	FTableMetaData CurrTableMetaData;
	MtdVersion Revision;
	Revision = MtdVersion::smallestVersion();

	//Mirem si hi ha les metadades guardades a la base de dades.
	FSqlQuery Query(Database);
	Query.prepare("SELECT revision, xml FROM fmetadata WHERE name='" + MetaData.name +  "'");
	Query.exec();
	if (Query.next())
	{
		if (Database.tables().contains(MetaData.name)) //Si no existeix la taula no fem cas de la revisió de metadata => Forçar update.
			Revision = Query.value(0).toString();
			
		CurrTableMetaData.loadFromStr(Query.value(1).toString());
	}
	else //Li posem
	{
		Query.prepare("INSERT INTO fmetadata ( name, xml, revision ) VALUES ( :name, :xml, :revision )");
		Query.bindValue(":name", MetaData.name);
		Query.bindValue(":xml", MetaData.xml());
		Query.bindValue(":revision", MetaData.revision.toString());
		Query.exec();//TODO: Ha de fer log o anunciar dels errors.
		CurrTableMetaData.name = MetaData.name; //AlterTable comprova que no s'hagi canviat el nom.
	}
	Query.clear(); //SQLite needs no active queries to commit works.
	//Comprovem si tenim una versió superior a les metadades. Si és així actualitzem.
	if (MetaData.revision > Revision) // Actualitzem la taula
	{
		stLog << ISTLogIO::L2 << ISTLogIO::DateTime << QApplication::tr("Updating table") + ":" <<
		MetaData.name << QApplication::tr(" From revision: ") << Revision.toString()  << 
		QApplication::tr(" To revision: ") << MetaData.revision.toString() << ISTLogIO::Endl;

		//Actualitzem la taula
		alterTable(CurrTableMetaData);

		//updateMetadata
		Query.prepare("UPDATE fmetadata set  xml = :xml, revision = :revision WHERE name = :name");
		Query.bindValue(":name", MetaData.name);
		Query.bindValue(":xml", MetaData.xml());
		Query.bindValue(":revision", MetaData.revision.toString());
		Query.exec(); //TODO: Ha de fer log o anunciar dels errors.

	}
}

/*!
	Executa les instruccions de la llista _UpdateList. Retorna cert 
	si la última instrucció executada és exlusive;
*/

bool FSqlDbTableManager::execUpdateInstrList(const FTableMetaData::TUpdateStrInstList& _UpdateList, 
	const FTableMetaData& _OldMetaData)
{
	bool ExclusiveUpdate = false;
	FTableMetaData::TUpdateStrInstList::const_iterator it;
	for (it = _UpdateList.begin(); it != _UpdateList.end(); ++it)
	{
		FTableMetaData::SSqlUpdateInstr BeforeUpdate = *it;
		MtdVersion OldRevision = _OldMetaData.revision;
		if (BeforeUpdate.Sql != "" && OldRevision == MtdVersion (BeforeUpdate.FromRev) | BeforeUpdate.FromRev == "") //TODO: Controlar el driver 
		{
			FSqlQuery Query(Database);
			stLog << ISTLogIO::L2 << QApplication::tr(" Running explicit SQL instruction:") <<
			BeforeUpdate.Sql << ISTLogIO::Endl;
			
			Query.exec(BeforeUpdate.Sql);
			ExclusiveUpdate = BeforeUpdate.Exclusive;
		}
	}			
	return ExclusiveUpdate;
}

//TODO: Tenir en compte les metadata de alterTable !
void FSqlDbTableManager::alterTable(const FTableMetaData& _OldMetaData)
{
	Assert(_OldMetaData.name == MetaData.name, Error( "FSqlDbTableManager::alterTable->" + QApplication::tr("New table name: ") +
	        MetaData.name + QApplication::tr(" differs from old table name:") + _OldMetaData.name ));

	//FSqlQuery Query(Database);
	Database.transaction();
	try
	{
		if (!Database.tables().contains(MetaData.name))
		{
			create(); //Si encara no hi és la creem.
			Assert(Database.commit(), 
				Error(QString(QApplication::tr("Error '%1' creating table '%2' to database")).arg(
						Database.lastError().text()).arg(MetaData.name)));
		}
		else
		{
			//int TotalSteps = 0;
			if (!execUpdateInstrList(MetaData.sqlBeforeUpdateList(), _OldMetaData))
			{
			//Còpia de seguretat.
			backupTable();
			dbAlterTable(_OldMetaData);
				
				
				
/*				Query.exec("ALTER TABLE " + MetaData.name + " DROP CONSTRAINT " +
							MetaData.name + "_pkey;");
				Query.exec("ALTER TABLE " + MetaData.name + " RENAME TO " + BackupName + ";");
				stLog << ISTLogIO::L2 << QApplication::tr("Old table") << " '" << MetaData.name << "' " << QApplication::tr(" renamed to")  <<
					BackupName << "'" << ISTLogIO::Endl;
				create();
	
	
				//Restaurem les dades de la taula vella.
				FSqlQuery OldData("Select * from " + BackupName, Database);
				QApplication::setOverrideCursor(Qt::WaitCursor);
				TotalSteps = OldData.size();
				QProgressDialog PDialog(QApplication::tr("Updating records..."), QApplication::tr("Abort"),
												0, TotalSteps);
				//PDialog.setLabelText(QApplication::tr("Tabla modificada"));
	
				QSqlTableModel NewDataModel(0, Database);
				NewDataModel.setTable(MetaData.name);
				NewDataModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
				while(OldData.next())
				{
					FTableMetaData::TFieldList::const_iterator it;
					QSqlRecord NewRecord = NewDataModel.record() ;
	
					for (it=MetaData.fields.begin(); it != MetaData.fields.end(); ++it)
					{
						//NewRecord.append(it->name());
						//Si les dades velles i son les recollim.
						if (OldData.record().contains(it->name()))
							NewRecord.setValue(it->name(), OldData.record().value(it->name()));
						else 
						{
							//Si no pot ser null i no te valor per defecte: Warning a Log
							if (it->requiredStatus() == QSqlField::Required && it->defaultValue().isNull())
							{
								stLog << ISTLogIO::L2 << QApplication::tr("Unable to set <non-null> field value of field,"
								" default value is null") << ":'" 
								<< it->name() << "' - Throwing an error." << ISTLogIO::Endl;
								throw Error(QApplication::tr("Error inserting record on table: '") + 
									MetaData.name + " see log file for details !"); //No se perque pero inserrecord no falla i no inserta res...
							}
							else 
								NewRecord.setValue(it->name(), it->defaultValue());
						}
					}
					
					Assert(NewDataModel.insertRecord(0, NewRecord), Error(QApplication::tr("Error inserting record on table: '") + MetaData.name + 
						" -> " + NewDataModel.lastError().text() ));
					NewDataModel.submitAll();
					PDialog.setValue(OldData.at());
					qApp->processEvents();
				}
				PDialog.setValue(TotalSteps);
				qApp->processEvents();
				QApplication::restoreOverrideCursor();*/
			} //Exclusive
			execUpdateInstrList(MetaData.sqlAfterUpdateList(), _OldMetaData);
			
			Assert(Database.commit(), 
				Error(QString(QApplication::tr("Error commiting changes of table '%1' to database")).arg(MetaData.name)));
			stLog << ISTLogIO::L2 << QApplication::tr("table: ") << MetaData.name  << 
				QApplication::tr(" updated successfull.") << ISTLogIO::Endl;
		}
	}
	catch (...)
	{
		Database.rollback();
		QApplication::restoreOverrideCursor();
		throw;
	}
}



// _________________________________________________________________________*/
//
// Class FSqlPsqlDbTableManager
// _________________________________________________________________________*/


// TODO: No se si hem de posar coses com lookup i tipus unlock.


QString FSqlPsqlDbTableManager::columnNameAndType(const FFieldMetaData& _Field)
{
	QString Sql;
	Sql += _Field.name();
	switch (_Field.type())
	{
		case QVariant::Int:
		Sql += " INT2";
		break;
		case QVariant::UInt:
		Sql += " INT4";
		break;
		case QVariant::Bool:
		Sql += " BOOLEAN";
		break;

		case QVariant::Double:
		Sql += " FLOAT8";
		break;

		case QVariant::Time:
		Sql += " TIME";
		break;

		case QVariant::Date:
		Sql += " DATE";
		break;

		case QVariant::DateTime:
		Sql += " TIMESTAMP";
		break;

		case QVariant::Pixmap:
		Sql += " TEXT";
		break;

		case QVariant::String:
		Sql += " VARCHAR";
		break;

		case QVariant::StringList:
		Sql += " TEXT";
		break;

		default:
		Sql += " VARCHAR";
		break;

	}
	
	int Length = _Field.length();

	if (Length > 0)
		Sql += "(" + QString::number(Length) + ")";
	return Sql;
}

QString FSqlPsqlDbTableManager::columnSql(const FFieldMetaData& _Field)
{
	QString Sql = columnNameAndType(_Field);
	if (_Field.isSerial)
	{
		FSqlQuery Query(Database);
		QString Seq =  MetaData.name + "_" + _Field.name() + "_seq";
		Query.exec("SELECT relname FROM pg_class WHERE relname='" + Seq +
						"';");
		if (!Query.next())
			Query.exec("CREATE SEQUENCE " + Seq + ";");
		Sql += " DEFAULT NEXTVAL('" + Seq + "')";
	}
	else 
	{
		if (!_Field.defaultValue().isNull())
			Sql += " DEFAULT " + _Field.defaultValue().toString();
	}
	
/*	if (_Field.isPrimaryKey)
	{
		Sql += " PRIMARY KEY";
	}
	else*/
	if (!_Field.isPrimaryKey)
	{
		if (_Field.requiredStatus() == QSqlField::Required)
			Sql += " NOT NULL";
		else
			Sql += " NULL";
	}
	if (MetaData.createRelations() && !_Field.relation().isNull())
		Sql += " REFERENCES " +  _Field.relation().tableName + " (" + _Field.relation().indexColumn + ")";

	return Sql;
}

/*!
	retorna false si no hi ha cap error en la creació.
*/
void FSqlPsqlDbTableManager::create()
{
	QString PrimaryKey;
	QString Seq;
	QString Sql = "CREATE TABLE " + MetaData.name + " (";
	FSqlQuery Query(Database);
	FTableMetaData::TFieldList::const_iterator it;
	bool FirstField = true;
	QStringList TablePrimaryKeys;
	for (it = MetaData.fields.begin(); it != MetaData.fields.end(); ++it)
	{
		if (FirstField)
			FirstField = false;
		else
			Sql += ",";
		Sql += columnSql(*it);
		if (it->isPrimaryKey)
			TablePrimaryKeys << it->name();
	}
	
	if (!TablePrimaryKeys.isEmpty())
		Sql += ", PRIMARY KEY (" + TablePrimaryKeys.join(",") + ")";
		
	Sql += ")";
	if (!MetaData.baseClass.isEmpty())
		Sql += " INHERITS (" + MetaData.baseClass + ")";
	
	FTableMetaData::TSqlInstr BeforeCreate = MetaData.sqlBeforeCreate();
	if (BeforeCreate.Sql != "" ) //TODO: Controlar el driver
		Query.exec(BeforeCreate.Sql);

	if (!BeforeCreate.Exclusive)
		Query.exec(Sql);

	FTableMetaData::TSqlInstr AfterCreate = MetaData.sqlAfterCreate();
	if (AfterCreate.Sql != "" ) //TODO: Controlar el driver i la versio
		Query.exec(AfterCreate.Sql);
	
	
		
		stLog << ISTLogIO::L2 << QApplication::tr("Table created") + ":" <<
	MetaData.name << QApplication::tr(" using: ") << Sql << ISTLogIO::Endl;

	/*	if (!Res)
		{
			stLog << ISTLogIO::L2 << ISTLogIO::DateTime << QApplication::tr("Could not create table ") +
			MetaData.name << ISTLogIO::Sql << Sql << ISTLogIO::EndSql << ISTLogIO::Endl;
		}
		else 
			stLog << ISTLogIO::L1 << ISTLogIO::DateTime << QApplication::tr("Table '") << MetaData.name << 
			QApplication::tr("' created! ")  <<	ISTLogIO::Sql << Sql << ISTLogIO::EndSql << ISTLogIO::Endl;*/
}


void FSqlPsqlDbTableManager::backupTable()
{
	FSqlQuery Query(Database);
	QString BackupName =
		MetaData.name.left(6) +
		QDateTime::currentDateTime().toString("ddhhssz");
	Query.exec("LOCK TABLE " + MetaData.name + " IN ACCESS EXCLUSIVE MODE;");
	//Creem una taula iguala i la copiem.
	Query.exec("CREATE TABLE " +  BackupName + " (LIKE " + MetaData.name + ")");
	Query.exec("INSERT INTO  " +  BackupName + " SELECT * FROM  " + MetaData.name );
}


void FSqlPsqlDbTableManager::dbAlterTable(const FTableMetaData& _OldMetaData)
{
	FSqlQuery Query(Database);
	Query.setLogExecs(true);
	//Comparem diferències i fem alters...
	FTableMetaData::TFieldList::const_iterator it;
	for (it=MetaData.fields.begin(); it != MetaData.fields.end(); ++it)
	{
		bool Created = false;
		//Si no existeix a la vella la creem.
		FFieldMetaData OldField;
		if (!_OldMetaData.fields.contains(it->name()))
		{
			Query.exec("ALTER TABLE " + MetaData.name + " ADD COLUMN " + columnNameAndType(*it)); 
			OldField = *it;
			Created = true;
		}
		else 
		{
			OldField = _OldMetaData.fields[it->name()];
			//Si el tipus és diferent: 
			if (OldField.type() != it->type())
			{
				//Renombrem la vella 
				FFieldMetaData RenOldField = OldField;
				RenOldField.setName(OldField.name() + "_flam_tmpback");
				Query.exec("ALTER TABLE " + MetaData.name + " RENAME COLUMN " + it->name() + " TO "  +
					RenOldField.name());
				//Creem la nova 
				Query.exec("ALTER TABLE " + MetaData.name + " ADD COLUMN " + columnNameAndType(*it)); 
				//Copiem els valors 
				Query.exec("UPDATE " + MetaData.name + " set " + it->name() + "=" + RenOldField.name()); 
				//Borrem la vella 
				Query.exec("ALTER TABLE " + MetaData.name + " DROP COLUMN " + RenOldField.name()); 
				Created = true;
			}
		}
		//Si canvia el Serial: 
		if (OldField.isSerial != it->isSerial || Created)
		{
			if (it->isSerial)
			{
				QString Seq =  MetaData.name + "_" + it->name() + "_seq";
				Query.exec("SELECT relname FROM pg_class WHERE relname='" + Seq +
								"';");
				if (!Query.next())
					Query.exec("CREATE SEQUENCE " + Seq + ";");
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " SET DEFAULT " +
				" NEXTVAL('" + Seq + "')");
			}
			else 
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " DROP DEFAULT ");
		}
			
		if ((OldField.defaultValue() != it->defaultValue() || Created)  && OldField.isSerial == it->isSerial)
		{
			if (it->defaultValue().isNull())
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " DROP DEFAULT ");
			else 
			{
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " SET DEFAULT " +
					it->defaultValue().toString());
				//Actualitzem els valors que son nulls sinó no podriem assignar un SET NOT NULL:
				Query.exec("UPDATE " + MetaData.name + " SET " + it->name() + "=" + it->defaultValue().toString());
			}
		}
		if (OldField.requiredStatus() != it->requiredStatus() || Created)
		{
			if (it->requiredStatus())
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " SET NOT NULL ");
			else 
				Query.exec("ALTER TABLE " + MetaData.name + " ALTER COLUMN " + it->name() + " DROP NOT NULL ");
		}
		if (OldField.isPrimaryKey != it->isPrimaryKey )
		{
			if (it->isPrimaryKey)
				Query.exec("ALTER TABLE " + MetaData.name + " ADD PRIMARY KEY " + MetaData.name + "_pkey;");
			else 
				Query.exec("ALTER TABLE " + MetaData.name + " DROP CONSTRAINT " + MetaData.name + "_pkey;");
		}
	} //For
	// Borrem totes les que hi eren a la vella i ja no hi son
	FTableMetaData::TFieldList::const_iterator oldit;
	for (oldit=_OldMetaData.fields.begin(); oldit != _OldMetaData.fields.end(); ++oldit)
	{
		if (!MetaData.fields.contains(oldit->name()) && Database.record(MetaData.name).contains(oldit->name()))
			Query.exec("ALTER TABLE " + MetaData.name + " DROP COLUMN " + oldit->name() );
	}
}


// _________________________________________________________________________*/
//
// Class FSqlSqliteDbTableManager
// _________________________________________________________________________*/


// TODO: No se si hem de posar coses com lookup i tipus unlock.


QString FSqlSqliteDbTableManager::columnNameAndType(const FFieldMetaData& _Field)
{
	QString Sql;
	Sql += _Field.name();
	switch (_Field.type())
	{
		case QVariant::Int:
		Sql += " INT2";
		break;
		case QVariant::UInt:
		Sql += " INT4";
		break;
		case QVariant::Bool:
		Sql += " BOOLEAN";
		break;

		case QVariant::Double:
		Sql += " FLOAT8";
		break;

		case QVariant::Time:
		Sql += " TEXT";
		break;

		case QVariant::Date:
		Sql += " TEXT";
		break;

		case QVariant::DateTime:
		Sql += " TEXT";
		break;

		case QVariant::Pixmap:
		Sql += " TEXT";
		break;

		case QVariant::String:
		Sql += " VARCHAR";
		break;

		case QVariant::StringList:
		Sql += " TEXT";
		break;

		default:
		Sql += " VARCHAR";
		break;

	}
	
	int Length = _Field.length();

	if (Length > 0)
		Sql += "(" + QString::number(Length) + ")";
	return Sql;
}

QString FSqlSqliteDbTableManager::columnSql(const FFieldMetaData& _Field)
{
	QString Sql = columnNameAndType(_Field);
	if (_Field.isSerial)
	{
// 		FSqlQuery Query(Database);
// 		QString Seq =  MetaData.name + "_" + _Field.name() + "_seq";
// 		Query.exec("SELECT relname FROM pg_class WHERE relname='" + Seq +
// 						"';");
// 		if (!Query.next())
// 			Query.exec("CREATE SEQUENCE " + Seq + ";");
// 		Sql += " DEFAULT NEXTVAL('" + Seq + "')";
		Sql += " DEFAULT NULL ";
	}
	else 
	{
		if (!_Field.defaultValue().isNull())
			Sql += " DEFAULT " + _Field.defaultValue().toString();
	}
	
/*	if (_Field.isPrimaryKey)
	{
		Sql += " PRIMARY KEY";
	}
	else*/
	if (!_Field.isPrimaryKey)
	{
		if (_Field.requiredStatus() == QSqlField::Required)
			Sql += " NOT NULL";
		else
			Sql += " NULL";
	}
	//if (MetaData.createRelations() && !_Field.relation().isNull())
	//	Sql += " REFERENCES " +  _Field.relation().tableName + " (" + _Field.relation().indexColumn + ")";

	return Sql;
}

/*!
	retorna false si no hi ha cap error en la creació.
*/
void FSqlSqliteDbTableManager::create()
{
	QString PrimaryKey;
	QString Seq;
	QString Sql = "CREATE TABLE " + MetaData.name + " (";
	FSqlQuery Query(Database);
	FTableMetaData::TFieldList::const_iterator it;
	bool FirstField = true;
	QStringList TablePrimaryKeys;
	for (it = MetaData.fields.begin(); it != MetaData.fields.end(); ++it)
	{
		if (FirstField)
			FirstField = false;
		else
			Sql += ",";
		Sql += columnSql(*it);
		if (it->isPrimaryKey)
			TablePrimaryKeys << it->name();
	}
	
	if (!TablePrimaryKeys.isEmpty())
		Sql += ", PRIMARY KEY (" + TablePrimaryKeys.join(",") + ")";
		
	Sql += ")";
	//if (!MetaData.baseClass.isEmpty())
	//	Sql += " INHERITS (" + MetaData.baseClass + ")";
	
 	FTableMetaData::TSqlInstr BeforeCreate = MetaData.sqlBeforeCreate();
 	if (BeforeCreate.Sql != "" ) //TODO: Controlar el driver
 		Query.execLines(BeforeCreate.Sql);

	if (!BeforeCreate.Exclusive)
		Query.exec(Sql);
 	
	FTableMetaData::TSqlInstr AfterCreate = MetaData.sqlAfterCreate();
 	if (AfterCreate.Sql != "" ) //TODO: Controlar el driver i la versio
 		Query.execLines(AfterCreate.Sql);	//SQlite does not support multiple queries at a time.
		
		stLog << ISTLogIO::L2 << QApplication::tr("Table created") + ":" <<
	MetaData.name << QApplication::tr(" using: ") << Sql << ISTLogIO::Endl;

	/*	if (!Res)
		{
			stLog << ISTLogIO::L2 << ISTLogIO::DateTime << QApplication::tr("Could not create table ") +
			MetaData.name << ISTLogIO::Sql << Sql << ISTLogIO::EndSql << ISTLogIO::Endl;
		}
		else 
			stLog << ISTLogIO::L1 << ISTLogIO::DateTime << QApplication::tr("Table '") << MetaData.name << 
			QApplication::tr("' created! ")  <<	ISTLogIO::Sql << Sql << ISTLogIO::EndSql << ISTLogIO::Endl;*/
}


void FSqlSqliteDbTableManager::backupTable()
{
	FSqlQuery Query(Database);
	QString BackupName =
		MetaData.name.left(6) +
		QDateTime::currentDateTime().toString("ddhhssz");
	//Query.exec("LOCK TABLE " + MetaData.name + " IN ACCESS EXCLUSIVE MODE;");
	//Creem una taula iguala i la copiem.
	Query.exec("CREATE TABLE " +  BackupName + " (LIKE " + MetaData.name + ")");
	Query.exec("INSERT INTO  " +  BackupName + " SELECT * FROM  " + MetaData.name );
}


void FSqlSqliteDbTableManager::dbAlterTable(const FTableMetaData& _OldMetaData)
{
	FSqlQuery Query(Database);
	Query.setLogExecs(true);
	//Comparem diferències i fem alters...
	FTableMetaData::TFieldList::const_iterator it;
	for (it=MetaData.fields.begin(); it != MetaData.fields.end(); ++it)
	{
		bool Created = false;
		//Si no existeix a la vella la creem.
		FFieldMetaData OldField;
		if (!_OldMetaData.fields.contains(it->name()))
		{
			Query.exec("ALTER TABLE " + MetaData.name + " ADD COLUMN " + columnNameAndType(*it)); 
			OldField = *it;
			Created = true;
		}
		else 
		{
			QString BackupTableName = MetaData.name + "_old";
			Query.exec("ALTER TABLE " + MetaData.name + " RENAME TO " + BackupTableName);
			create();
			Query.exec("INSERT INTO " + MetaData.name + " SELECT * FROM " + BackupTableName);
			Query.exec("DROP TABLE " + BackupTableName );
		}
			
	} //For
	// Borrem totes les que hi eren a la vella i ja no hi son
// 	FTableMetaData::TFieldList::const_iterator oldit;
// 	for (oldit=_OldMetaData.fields.begin(); oldit != _OldMetaData.fields.end(); ++oldit)
// 	{
// 		if (!MetaData.fields.contains(oldit->name()) && Database.record(MetaData.name).contains(oldit->name()))
// 			Query.exec("ALTER TABLE " + MetaData.name + " DROP COLUMN " + oldit->name() );
// 	}
}
