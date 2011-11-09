/****************************************************************************
**
** Copyright (C) 2010-2011 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2011
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#include "remotepublishermanager.h"
#include "systemtemplates.h"
#include "stimage.h"
#include "stftpordertransfer.h"
#include "sindardatabase.h"
#include "fsqldatabasemanager.h"

RemotePublisherManager::RemotePublisherManager(StarlabAbstractManager* _Manager, const QString& _PublisherDatabaseFilePath) :
	Manager(_Manager), PublisherDatabaseFilePath(_PublisherDatabaseFilePath)
{

}

SPhotoBook::TemplateInfoList RemotePublisherManager::getTemplates()
{
	//Load local templates
	SPhotoBook::TemplateInfoList Res;
	Res = SPhotoBook::SystemTemplates::load();

	//And remote templates
	//TODO:
	// - Get publictemplateinfo.db
	// - Open it and merge template lists.
	// - If template metainfo isn't on disk download it.
	// - If public ver is greater than local ver remove local template files.
	// - If public ver is smaller don't download meta info, only update public ver.

	return Res;
}

void RemotePublisherManager::pullProducts(bool _PreserveLocalAdded)
{
	bool DownloadProducts = true;
	//If we already have products check if there are up to date.
	QFileInfo PublisherDbFileInfo(PublisherDatabaseFilePath);
	if (PublisherDbFileInfo.exists())
		DownloadProducts = !Manager->productsUpToDate(STImage::hashString(PublisherDbFileInfo.absoluteFilePath()));

	//Get it from publisher if needed
	if (DownloadProducts)
	{
		STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
		try
		{
			STDom::Publisher Publisher = Manager->getPublisher();
			//TODO: If preserve local added merge it (See ttpops sync code).
			FtpTrans->getFile(PublisherDbFileInfo.fileName(), Publisher.initDir(), PublisherDbFileInfo.absoluteFilePath(),
							  Publisher.ftpUrl(), Publisher.ftpPort(),
							  Publisher.userName(), Publisher.password(),
							  static_cast<QFtp::TransferMode>(Publisher.transferMode()));
		}
		catch (...)
		{

		}
		delete FtpTrans;

	}
}

bool RemotePublisherManager::productsDBExist()
{
	QFileInfo FInfo(PublisherDatabaseFilePath);
	return FInfo.exists();
}

void RemotePublisherManager::addDefaultProducts(DefaultSindarDatabase& _SindarDatabase)
{
	STDom::PublisherDatabase PDBase(PublisherDatabaseFilePath);
	Assert(PDBase.open(), Error(QString(QObject::tr("Could not open file %1 for database export.")).arg(PublisherDatabaseFilePath)));
	Assert(_SindarDatabase.open(), Error(QString(QObject::tr("Could not database %1 for database export.")).arg(_SindarDatabase.name())));
	FSqlDatabaseManager Manager(_SindarDatabase, ":/");
	PDBase.importAll(Manager);
	PDBase.close();
}

STDom::PublisherDatabase RemotePublisherManager::getProducts() const
{
	return STDom::PublisherDatabase::addDatabase(PublisherDatabaseFilePath);
}

void RemotePublisherManager::removeAllProducts()
{
	QFileInfo PubDbFile(PublisherDatabaseFilePath);
	if (PubDbFile.exists())
		Assert(QFile::remove(PubDbFile.absoluteFilePath()), Error(QObject::tr("Could not remove file %1").arg(PubDbFile.absoluteFilePath())));

}

//!Creates products for all templates if there isn't and deletes products without template
void RemotePublisherManager::updateTemplateProducts(STDom::PublisherDatabase& _ProductsDb, const SPhotoBook::TemplateInfoList& _Templates)
{
	SindarDatabase SindarDB(_ProductsDb);
	Assert(SindarDB.open(), Error(QObject::tr("Error opening publisher database %1").arg(SindarDB.connectionName())));
	SindarDB.importTemplateRefs(_Templates);
	SindarDB.insertProductsForTemplates(_Templates);
	// TODO: ?
	// SindarDB.removeProductsForTemplatesOtherThan(_Templates);
	SindarDB.close();

}
