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

#ifndef OMARKETPLACE_H
#define OMARKETPLACE_H

#include <QList>
#include <QAbstractListModel>
#include <QFtp>
#include <QSqlDatabase>
#include "sterror.h"
#include "storomelexport.h"
#include "sapplication.h"
#include "stftpordertransfer.h"


class ST_OROMEL_EXPORT OMarketPlaceItem
{
public:
	enum EnType
	{
		TypeBook,
		TypeCalendar,
		TypeCard,
		TypeIdPhoto,
		TypeMultiPhoto,
		TypeBackgrounds,
		TypeFrames,
		TypeCliparts,
		TypeMasks
	};

public:
	int Version;
	int InstalledVersion;
	QString Package;
	QString UrlInfo;
	QString Description;
	EnType Type;
	OMarketPlaceItem() {}
	OMarketPlaceItem(QSqlRecord _Record);
	static QIcon iconByType(EnType _Type);
	QString installationPath() const;

};

typedef ST_OROMEL_EXPORT QList<OMarketPlaceItem> OMPItemList;

class ST_OROMEL_EXPORT OMItemListModel : public QAbstractListModel
{
	OMPItemList ItemList;
	bool ShowInstalledVer;

public:
	OMItemListModel(QObject* _Parent = 0);
	int rowCount(const QModelIndex & parent = QModelIndex()) const ;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	void setItemList(const OMPItemList& _List);
	OMarketPlaceItem item(const QModelIndex& _Index);
	void showInstalledVer(bool _Value) { ShowInstalledVer = _Value; }
};


class FSqlDatabaseManager;
class ST_OROMEL_EXPORT OMarketPlaceDatabase : public QSqlDatabase
{
public:
	ST_DECLARE_ERRORCLASS();
public:
	OMarketPlaceDatabase(const QString& _DatabaseName = defaultDatabaseName());
	void open();
	void exportTable(const FSqlDatabaseManager& _SourceDBManager, const QString& _TableName,
					 const QString& _ImportOrderBy = "");
	static QString defaultDbFileName() { return "marketplace.db"; }
	static QString defaultDatabaseName() { return SApplication::dbFilesPath() + "/" + defaultDbFileName(); }
};

class STFtpStatusWidget;
class ST_OROMEL_EXPORT OMarketPlace
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	STDom::STFtpOrderTransfer* FtpTrans;

	OMPItemList listFromSelect(const QString& _Select);

public:
	OMarketPlace();
	void sync(const QString& _Dir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,
			  const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode);

	OMPItemList updates();
	OMPItemList newItems();
	OMPItemList installed();
	void install(const OMarketPlaceItem& _Item, const QString& _Host, int _Port, const QString& _User, const QString& _Password,
				 const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode, STFtpStatusWidget* _StatusWidget = 0);
	void uninstall(const OMarketPlaceItem& _Item);
	static void exportDatabase(const FSqlDatabaseManager& _SourceDBManager, const QString& _FilePath);
	static void registerChanges();
	static bool isThereChanges();
	static void clearChanges();
	static void setSyncDate(const QDate& _Date);
	static QDate lastSyncDate();
};

#endif // OMARKETPLACE_H
