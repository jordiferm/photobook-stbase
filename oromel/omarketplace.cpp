/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "omarketplace.h"

#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

#include "fsqldatabasemanager.h"
#include "fsqlquery.h"
#include "sttemplatepaths.h"
#include "stftpstatuswidget.h"
#include "stutils.h"
#include "siconfactory.h"

//_____________________________________________________________________________
//
// Class OMarketPlaceItem
//_____________________________________________________________________________

OMarketPlaceItem::OMarketPlaceItem(QSqlRecord _Record)
{
	Type = static_cast<OMarketPlaceItem::EnType>(_Record.value("type").toInt());
	Package = _Record.value("src").toString();
	Description = _Record.value("description").toString();
	Version = _Record.value("ver").toInt();
	InstalledVersion = _Record.value("installedver").toInt();
	UrlInfo = _Record.value("urlinfo").toString();
}

QIcon OMarketPlaceItem::iconByType(OMarketPlaceItem::EnType _Type)
{
	QIcon Res;
	switch(_Type)
	{
		case TypeBook:
			Res = QIcon(":/st/oromel/typebook.png");
		break;
		case TypeCalendar:
			Res = QIcon(":/st/oromel/typecalendar.png");
		break;
		case TypeCard:
			Res = QIcon(":/st/oromel/typecard.png");
		break;
		case TypeIdPhoto:
			Res = QIcon(":/st/oromel/typeidphoto.png");
		break;
		case TypeMultiPhoto:
			Res = QIcon(":/st/oromel/typemultiphoto.png");
		break;
		case TypeBackgrounds:
			Res = QIcon(":/st/oromel/typebackground.png");
		break;
		case TypeFrames:
			Res = QIcon(":/st/oromel/typeframes.png");
		break;
		case TypeCliparts:
			Res = QIcon(":/st/oromel/typecliparts.png");
		break;
		case TypeMasks:
			Res = QIcon(":/st/oromel/typemasks.png");
		break;
	}

	return Res;
}

QString OMarketPlaceItem::installationPath() const
{
	QString Res;
	switch(Type)
	{
		case TypeBook:
			Res = STTemplatePaths::templateTypePath(STTemplatePaths::defaultTemplatesPath(), STPhotoLayout::TypePhotoBook);
		break;
		case TypeCalendar:
			Res = STTemplatePaths::templateTypePath(STTemplatePaths::defaultTemplatesPath(), STPhotoLayout::TypeCalendar);
		break;
		case TypeCard:
			Res = STTemplatePaths::templateTypePath(STTemplatePaths::defaultTemplatesPath(), STPhotoLayout::TypeCard);
		break;
		case TypeIdPhoto:
			Res = STTemplatePaths::templateTypePath(STTemplatePaths::defaultTemplatesPath(), STPhotoLayout::TypeIdPhoto);
		break;
		case TypeMultiPhoto:
			Res = STTemplatePaths::templateTypePath(STTemplatePaths::defaultTemplatesPath(), STPhotoLayout::TypeMultiPhoto);
		break;
		case TypeBackgrounds:
			Res = STTemplatePaths::defaultTemplateResourcesPath(STPhotoLayout::BackgroundsDir);
		break;
		case TypeFrames:
			Res = STTemplatePaths::defaultTemplateResourcesPath(STPhotoLayout::FramesDir);
		break;
		case TypeCliparts:
			Res = STTemplatePaths::defaultTemplateResourcesPath(STPhotoLayout::ClipartDir);
		break;
		case TypeMasks:
			Res = STTemplatePaths::defaultTemplateResourcesPath(STPhotoLayout::MasksDir);
		break;
	}
	return Res;
}


//_____________________________________________________________________________
//
// Class OMItemListModel
//_____________________________________________________________________________

OMItemListModel::OMItemListModel(QObject* _Parent) : QAbstractListModel(_Parent), ShowInstalledVer(true)
{
}

int OMItemListModel::rowCount(const QModelIndex & _Parent) const
{
	return ItemList.size();

}

QVariant OMItemListModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	if (_Index.isValid() && _Index.row() < rowCount())
	{
		OMarketPlaceItem CurrentItem = ItemList[_Index.row()];
		if (_Role == Qt::DisplayRole)
		{
			int NumVer;
			if (ShowInstalledVer)
				NumVer = CurrentItem.InstalledVersion;
			else
				NumVer = CurrentItem.Version;

			Res = CurrentItem.Description + " - Ver:" + QString::number(NumVer);
		}
		else
		if (_Role == Qt::DecorationRole)
		{
			Res = OMarketPlaceItem::iconByType(CurrentItem.Type);
		}
	}

	return Res;
}

void OMItemListModel::setItemList(const OMPItemList& _List)
{
	ItemList = _List;
	reset();
}

OMarketPlaceItem OMItemListModel::item(const QModelIndex& _Index)
{
	OMarketPlaceItem Item;
	if (_Index.isValid() && _Index.row() < rowCount())
		Item = ItemList[_Index.row()];

	return Item;
}

//_____________________________________________________________________________
//
// Class OMarketPlaceDatabase
//_____________________________________________________________________________

OMarketPlaceDatabase::OMarketPlaceDatabase(const QString& _DatabaseName) : QSqlDatabase("QSQLITE")
{
	setDatabaseName(_DatabaseName);
}

void OMarketPlaceDatabase::open()
{
	if (databaseName() == defaultDatabaseName())
	{
		QFileInfo FInfo(databaseName());
		if (!FInfo.dir().exists())
			FInfo.dir().mkpath(FInfo.dir().absolutePath());
	}
	Assert(QSqlDatabase::open(), Error(QString(QObject::tr("Could not open file %1 for database export: %2")).arg(databaseName()).arg(lastError().text())));
	if (!tables().contains("mpitems"))
	{
		FSqlQuery Query(*this);
		Query.exec("CREATE TABLE mpitems (description VARCHAR(300) NULL,installedver INT4 DEFAULT 0 NULL,src VARCHAR(150),"
				   "type INT4 DEFAULT 0 NULL,urlinfo VARCHAR(150) NULL,ver INT4 DEFAULT 0 NULL, PRIMARY KEY (src));");
	}
}

void OMarketPlaceDatabase::exportTable(const FSqlDatabaseManager& _SourceDBManager, const QString& _TableName, const QString& _ImportOrderBy)
{
	QSqlDatabase SourceDB = _SourceDBManager.database();
	FTableMetaData  NewTMetadata = _SourceDBManager.tableMetaData(_TableName);
	FSqlQuery DestDbQuery(*this);
	if (tables().contains(_TableName))
	{
		DestDbQuery.exec("DROP table " + _TableName);
	}
	FSqlSqliteDbTableManager TManager(NewTMetadata, *this);
	TManager.create();
	DestDbQuery.exec("DELETE from " + _TableName); //Delete aftercreate inserts

	FSqlQuery SourceQuery(SourceDB);
	QString ImportOrderBy;
	if (!_ImportOrderBy.isEmpty())
		ImportOrderBy = " ORDER BY " + _ImportOrderBy;
	SourceQuery.exec("SELECT * FROM " + _TableName + ImportOrderBy);
	while (SourceQuery.next())
	{
		DestDbQuery.prepareInsert(SourceQuery.record(), _TableName);
		DestDbQuery.exec();
	}
}



//_____________________________________________________________________________
//
// Class OMarketPlace
//_____________________________________________________________________________


OMPItemList OMarketPlace::listFromSelect(const QString& _Select)
{
	OMPItemList Res;
	OMarketPlaceDatabase Database;
	Database.open();
	FSqlQuery Query(Database);

	Query.exec(_Select);
	while (Query.next())
	{
		Res.push_back(OMarketPlaceItem(Query.record()));
	}

	Database.close();
	return Res;
}

OMarketPlace::OMarketPlace() : FtpTrans(0)
{
}


void OMarketPlace::sync(const QString& _Dir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,
		  const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode)
{
	//Download remote file to temp dir.
	if (FtpTrans)
		delete FtpTrans;

	FtpTrans = new STDom::STFtpOrderTransfer;
	QString TempFile = QDir(QDir::tempPath()).filePath("tempmarketplace.db");
	FtpTrans->getFile(OMarketPlaceDatabase::defaultDbFileName(), _Dir, TempFile, _Host, _Port, _User, _Password, _TransferMode);

	//Import table to remotempitems table.
	OMarketPlaceDatabase SourceDatabase(TempFile);
	OMarketPlaceDatabase DestDatabase;

	SourceDatabase.open();
	DestDatabase.open();

	FSqlQuery SourceQuery(SourceDatabase);
	FSqlQuery DestQuery(DestDatabase);


	DestQuery.exec("CREATE TEMP TABLE remotempitems AS SELECT * FROM mpitems");

	SourceQuery.exec("SELECT * FROM mpitems");
	while (SourceQuery.next())
	{
		DestQuery.prepareInsert(SourceQuery.record(), "remotempitems");
		DestQuery.exec();
	}

	//Remove items that not exist and isn't installed
	DestQuery.exec("DELETE FROM mpitems WHERE NOT src IN (SELECT src FROM remotempitems) AND installedver=0");

	DestQuery.exec("SELECT * FROM remotempitems WHERE NOT src IN (SELECT src FROM mpitems)");
	if (DestQuery.next())
		registerChanges();

	//Insert new items
	DestQuery.exec("INSERT INTO mpitems SELECT * FROM remotempitems WHERE NOT src IN (SELECT src FROM mpitems)");

	//Update existing items
	DestQuery.exec("SELECT mpitems.src, remotempitems.ver, remotempitems.description FROM mpitems INNER JOIN remotempitems ON mpitems.src = remotempitems.src WHERE "
				   "remotempitems.ver > mpitems.ver");
	while (DestQuery.next())
	{
		FSqlQuery DestTempQuery(DestDatabase);
		DestTempQuery.prepare("UPDATE mpitems SET ver=:ver, description=:description WHERE src=:src");
		DestTempQuery.bindValue(":ver", DestQuery.value(1));
		DestTempQuery.bindValue(":src", DestQuery.value(0));
		DestTempQuery.bindValue(":description", DestQuery.value(2));
		DestTempQuery.exec();
		registerChanges();
	}
	setSyncDate(QDate::currentDate());
}


OMPItemList OMarketPlace::updates()
{
	return listFromSelect("SELECT * FROM mpitems WHERE installedver > 0 AND installedver < ver");
}

OMPItemList OMarketPlace::newItems()
{
	return listFromSelect("SELECT * FROM mpitems WHERE installedver = 0");
}

OMPItemList OMarketPlace::installed()
{
	return listFromSelect("SELECT * FROM mpitems WHERE installedver > 0");
}

void OMarketPlace::install(const OMarketPlaceItem& _Item, const QString& _Host, int _Port, const QString& _User, const QString& _Password,
						   const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode, STFtpStatusWidget* _StatusWidget )
{
	//Download remote file to temp dir.
	if (FtpTrans)
		delete FtpTrans;

	FtpTrans = new STDom::STFtpOrderTransfer;
	QString DestPath = _Item.installationPath();
	if (_StatusWidget)
	{
		_StatusWidget->setAbsoluteTotalBytes(FtpTrans->calcDirTransferBytes(_Item.Package, _Host, _Port, _User, _Password, _TransferMode));
		//Problems in windows disconecting ftps...
		FtpTrans->abortAll();
		delete FtpTrans;
		FtpTrans = new STDom::STFtpOrderTransfer;
		_StatusWidget->connectFtp(FtpTrans);
	}
	try
	{
		FtpTrans->getDir(_Item.Package, DestPath, _Host, _Port, _User, _Password, _TransferMode);
		if (_StatusWidget)
			_StatusWidget->disconnectFtp(FtpTrans);

		OMarketPlaceDatabase MPDatabase;
		MPDatabase.open();
		FSqlQuery Query(MPDatabase);
		Query.prepare("UPDATE mpitems SET installedver=:installedver WHERE src=:src");
		Query.bindValue(":installedver", _Item.Version);
		Query.bindValue(":src", _Item.Package);
		Query.exec();
		MPDatabase.close();

	}
	catch(...)
	{
		if (_StatusWidget)
			_StatusWidget->disconnectFtp(FtpTrans);
		throw;
	}
}

void OMarketPlace::uninstall(const OMarketPlaceItem& _Item)
{
	Assert(!_Item.Package.isEmpty(), Error("Empty package name found trying to uninstall it."));
	QString DestPath = QDir(_Item.installationPath()).absoluteFilePath(QDir(_Item.Package).dirName());
	STUtils::removeRecursiveForced(QDir(DestPath));
	OMarketPlaceDatabase MPDatabase;
	MPDatabase.open();
	FSqlQuery Query(MPDatabase);
	Query.prepare("UPDATE mpitems SET installedver=0 WHERE src=:src");
	Query.bindValue(":src", _Item.Package);
	Query.exec();
	MPDatabase.close();
}

void OMarketPlace::exportDatabase(const FSqlDatabaseManager& _SourceDBManager, const QString& _FilePath)
{
	OMarketPlaceDatabase MPDatabase(_FilePath);
	MPDatabase.open();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	try
	{
		MPDatabase.exportTable(_SourceDBManager, "mpitems");
		MPDatabase.close();
		QApplication::restoreOverrideCursor();
	}
	catch(...)
	{
		QApplication::restoreOverrideCursor();
		throw;
	}
}

void OMarketPlace::registerChanges()
{
	QSettings Settings;
	Settings.setValue("omarketplacechanges", true);
}

bool OMarketPlace::isThereChanges()
{
	QSettings Settings;
	return Settings.value("omarketplacechanges", "false").toBool();
}

void OMarketPlace::clearChanges()
{
	QSettings Settings;
	Settings.setValue("omarketplacechanges", false);
}

void OMarketPlace::setSyncDate(const QDate& _Date)
{
	QSettings Settings;
	Settings.setValue("mplastsyncdate", _Date);
}

QDate OMarketPlace::lastSyncDate()
{
	QSettings Settings;
	return Settings.value("mplastsyncdate", QDate(2010,1,1)).toDate();
}
