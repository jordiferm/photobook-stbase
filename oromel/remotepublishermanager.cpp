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
#include "starlabtable.h"
#include "sprocessstatuswidget.h"
#include "publictemplatesdatabase.h"

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

void RemotePublisherManager::pullProducts(bool _UpToDateOnly, bool _PreserveLocalAdded)
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
			if (_UpToDateOnly)
				FtpTrans->syncRemoteFile(PublisherDbFileInfo.fileName(), PublisherDbFileInfo.dir().absolutePath(),
										 Publisher.ftpUrl(), Publisher.ftpPort(),
										 Publisher.userName(), Publisher.password(), Publisher.initDir(),
										 static_cast<QFtp::TransferMode>(Publisher.transferMode()));
			else
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

void RemotePublisherManager::pushProducts()
{
	QFileInfo PublisherDbFileInfo(PublisherDatabaseFilePath);
	if (PublisherDbFileInfo.exists()) //Defensive
	{
		QString ProductsHash = STImage::hashString(PublisherDbFileInfo.absoluteFilePath());

		STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
		try
		{
			if (!Manager->productsUpToDate(ProductsHash))
			{
				STDom::Publisher Publisher = Manager->getPublisher();
				FtpTrans->putFile(PublisherDbFileInfo.absoluteFilePath(), Publisher.ftpUrl(),
								  Publisher.ftpPort(), Publisher.userName(), Publisher.password(),
								  Publisher.initDir(), static_cast<QFtp::TransferMode>(Publisher.transferMode()));
				Manager->updateProductHash(ProductsHash);
			}
		}
		catch (...)
		{
		}
		delete FtpTrans;
	}
}

void RemotePublisherManager::pushTemplate(const SPhotoBook::TemplateInfo& _Template, SProcessStatusWidget* _L1Process, SProcessStatusWidget* _L2Process)
{
	//sync starlab products for this template only.(Create template if not exist).
	Manager->syncTemplate(_Template);
	STDom::Publisher Publisher = Manager->getPublisher();

	if (_Template.hasPublicDesigns())
	{
		STDom::PublisherDatabase PubDatabase = getProducts();
		Assert(PubDatabase.open(), Error(QObject::tr("Could not open products database")));
		Manager->syncTemplateProducts(PubDatabase, _Template);
		//upload template files for all designs not up to date.


		SPhotoBook::DesignInfoList Designs = _Template.designs();
		SPhotoBook::DesignInfoList::const_iterator it;
		if (_L1Process)
			_L1Process->showProgressBar(QObject::tr("Exporting design files..."), Designs.size());
		for (it = Designs.begin(); it != Designs.end(); ++it )
		{
			SPhotoBook::DesignInfo DInfo = *it;
			if (DInfo.isPublic() && !DInfo.isUpToDate())
				SPhotoBook::SystemTemplates::uploadTemplateDesign(Publisher, _Template, DInfo, _L2Process);
			if (_L1Process)
				_L1Process->incrementProgress();
		}

	}

	//update publictemplatesinfo.db for this template and upload it.
	QString PTemplatesDbFile = SPhotoBook::SystemTemplates::publicTemplatesDatabaseFile();
	SPhotoBook::PublicTemplatesDatabase PDBase(PTemplatesDbFile);
	Assert(PDBase.open(), Error(QString(QObject::tr("Could not open file %1 for database export.")).arg(PTemplatesDbFile)));
	PDBase.updateTemplateInfo(_Template);
	PDBase.close();

	QString TemplatesHash = STImage::hashString(PTemplatesDbFile);

	STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
	try
	{
		if (!Manager->templatesUpToDate(TemplatesHash))
		{
			STDom::Publisher Publisher = Manager->getPublisher();
			FtpTrans->putFile(PTemplatesDbFile, Publisher.ftpUrl(),
							  Publisher.ftpPort(), Publisher.userName(), Publisher.password(),
							  Publisher.initDir(), static_cast<QFtp::TransferMode>(Publisher.transferMode()));
			Manager->updateTemplateHash(TemplatesHash);
		}
		delete FtpTrans;
	}
	catch (...)
	{
		delete FtpTrans;
		throw;
	}

}

bool RemotePublisherManager::productsDBExist()
{
	bool Res = false;
	QFileInfo FInfo(PublisherDatabaseFilePath);
	if (FInfo.exists())
	{
		Res = FInfo.size() > 0;
	}
	return Res;
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


STDom::PublisherInfo RemotePublisherManager::getPublisherInfo() const
{
	STDom::PublisherInfo Res;
	Res.setDatabase(getProducts(), PublisherDatabaseFilePath);
	Res.setPublisher(Manager->getPublisher());

	StarlabTable ShippingMethodTable = Manager->getPublisherTable(StarlabAbstractManager::TableShippingMethods);
	StarlabTable::iterator it;
	for (it = ShippingMethodTable.begin(); it != ShippingMethodTable.end(); ++it)
	{
		STDom::ShippingMethod ShippingMethod;
		QSqlRecord CRec = *it;
		ShippingMethod.setId(CRec.value("id").toString());
		ShippingMethod.setDescription(CRec.value("description").toString());
		ShippingMethod.setPrice(CRec.value("amount").toDouble());
		Res.addShippingMethod(ShippingMethod);
	}

	StarlabTable PaymentTypesTable = Manager->getPublisherTable(StarlabAbstractManager::TablePaymentTypes);
	for (it = PaymentTypesTable.begin(); it != PaymentTypesTable.end(); ++it)
	{
		STDom::PaymentType PType;
		QSqlRecord CRec = *it;
		PType.setId(CRec.value("id").toString());
		PType.setDescription(CRec.value("description").toString());
		PType.setPayRequired(CRec.value("pay_required").toString().toLower() == "true");
		Res.addPaymentType(PType);
	}

	StarlabTable CollectionPointsTable = Manager->getPublisherTable(StarlabAbstractManager::TableCollectionPoints);
	for (it = CollectionPointsTable.begin(); it != CollectionPointsTable.end(); ++it)
	{
		STDom::CollectionPoint CPoint;
		QSqlRecord CRec = *it;
		CPoint.setId(CRec.value("id").toString());
		CPoint.setCity(CRec.value("city").toString());
		CPoint.setName(CRec.value("name").toString());
		CPoint.setWeb(CRec.value("web").toString());
		CPoint.setCountry(CRec.value("country").toString());
		CPoint.setEmail(CRec.value("email").toString());
		CPoint.setState(CRec.value("state").toString());
		CPoint.setAddress(CRec.value("address").toString());
		CPoint.setPostalCode(CRec.value("postalcode").toString());
		CPoint.setWebInfo(CRec.value("web_info").toString());
		Res.addCollectionPoint(CPoint);
	}

	return Res;
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
