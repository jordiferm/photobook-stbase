/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "sindardatabase.h"
#include "publisherdatabase.h"
#include "fsqlquery.h"
#include "stcollectiontemplatemodel.h"
#include "fsqldatabasemanager.h"
#include "stcollectionpublishermodel.h"
#include <QApplication>
#include <QDebug>

//_____________________________________________________________________________
//
// Class DefaultSindarDatabase
//_____________________________________________________________________________


DefaultSindarDatabase::DefaultSindarDatabase() : QSqlDatabase("QSQLITE")
{
}

DefaultSindarDatabase::DefaultSindarDatabase(const QString& _Name, const QString& _EmbedFileName, const QString& _Description) :
		QSqlDatabase("QSQLITE"), Name(_Name), EmbedFileName(_EmbedFileName), Description(_Description)
{
}

bool DefaultSindarDatabase::open()
{
	bool Res = false;
	//Create a temp file and open it
	QString EmbFile = EmbedFileName;
	QString TempFileName = QDir::tempPath() +  "/" + EmbFile.remove(":/sindarldefaultdb/");
	QFile SourceFile(EmbedFileName);
	QFile DestFile(TempFileName);
	if (SourceFile.open(QFile::ReadOnly))
	{
		if (DestFile.open(QFile::WriteOnly))
		{
			DestFile.write(SourceFile.readAll());
			DestFile.close();
			setDatabaseName(TempFileName);
			Res = QSqlDatabase::open();
		}
	}
	else
		qDebug() << "Couldn't read embeded file:" << EmbedFileName;
	return Res;
}


void DefaultSindarDatabase::exportPublisherDB(const QString& _FileName)
{
	STDom::PublisherDatabase PDBase(_FileName);
	Assert(PDBase.open(), Error(QString(QObject::tr("Could not open file %1 for database export.")).arg(_FileName)));
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	PDBase.importAll(FSqlDatabaseManager::manager());
	PDBase.close();
	QApplication::restoreOverrideCursor();
}

//_____________________________________________________________________________
//
// Class SindarDatabase
//_____________________________________________________________________________

STDom::PublisherDatabase::EnProductType SindarDatabase::productTypeFromLayoutType(STPhotoLayout::EnLayoutType _Type)
{
	STDom::PublisherDatabase::EnProductType Res;
	switch (_Type)
	{
		case STPhotoLayout::TypeCalendar:
			Res = STDom::PublisherDatabase::DecorationsProduct;
		break;
		case STPhotoLayout::TypeCard:
			Res = STDom::PublisherDatabase::DecorationsProduct;
		break;
		case STPhotoLayout::TypeIdPhoto:
			Res = STDom::PublisherDatabase::PhotoIdProduct;
		break;
		case STPhotoLayout::TypeMultiPhoto:
			Res = STDom::PublisherDatabase::DecorationsProduct;
		break;
		case STPhotoLayout::TypePhotoBook:
			Res = STDom::PublisherDatabase::PhotoBookProduct;
		break;
	}
	return Res;
}

SindarDatabase::SindarDatabase(const QSqlDatabase& _Other ) : QSqlDatabase(_Other)
{
}

int SindarDatabase::insertProductsForAllTemplates(STPhotoLayout::EnLayoutType _Type)
{
	STCollectionTemplateModel Model;
	int NInserts = 0;
	Model.loadTemplates(_Type);
	for (int Vfor = 0; Vfor < Model.rowCount(); Vfor++)
	{
		STPhotoLayout CTemplate;
		QModelIndex CIndex = Model.index(Vfor, 0);

		//For each format
		STDom::DDocFormatList AvailableFormats = Model.availableFormats(CIndex);
		STDom::DDocFormatList::const_iterator fit;
		for (fit = AvailableFormats.begin(); fit != AvailableFormats.end(); ++fit)
		{
			CTemplate.load(Model.photoLayoutFileInfo(CIndex, *fit).absoluteFilePath());
			//For each template
			STPhotoLayout::TAlbumsList::const_iterator it;
			STPhotoLayout::TAlbumsList Albums = CTemplate.albums();
			for (it = Albums.begin(); it != Albums.end(); ++it)
			{
				if (!it->ref().isEmpty() && it->ref() != STPhotoBookTemplate::DefaultRef)
				{
					//If there is no any product with this template ref:
					FSqlQuery PQuery(*this);
					PQuery.prepare("SELECT ref FROM products WHERE templates_ref=:templates_ref");
					PQuery.bindValue(":templates_ref", it->ref());
					PQuery.exec();
					if (!PQuery.next()) //Lets insert it
					{
						QSqlRecord RecProduct = PQuery.database().record("products");
						RecProduct.setValue("ref", it->ref());
						RecProduct.setValue("description", it->description());
						RecProduct.setValue("fixedprice", 0);
						RecProduct.setValue("label", it->description().left(25));
						RecProduct.setValue("templates_ref", it->ref());
						STDom::DDocFormat Format = *fit;
						RecProduct.setValue("width", Format.width());
						RecProduct.setValue("height", Format.height());
						RecProduct.setValue("type", productTypeFromLayoutType(_Type));
						FSqlQuery IQuery(*this);
						IQuery.prepareInsert(RecProduct, "products");
						IQuery.exec();
						NInserts++;
					}
				}
			}
		}
	}
	return NInserts;
}

int SindarDatabase::insertProductsForAllTemplates()
{
	int NInserts = 0;
	NInserts += insertProductsForAllTemplates(STPhotoLayout::TypePhotoBook);
	NInserts += insertProductsForAllTemplates(STPhotoLayout::TypeCalendar);
	NInserts += insertProductsForAllTemplates(STPhotoLayout::TypeCard);
	NInserts += insertProductsForAllTemplates(STPhotoLayout::TypeIdPhoto);
	NInserts += insertProductsForAllTemplates(STPhotoLayout::TypeMultiPhoto);
	return NInserts;
}

int SindarDatabase::importTemplateRefs(STPhotoLayout::EnLayoutType _Type)
{
	STCollectionTemplateModel Model;
	int NInserts = 0;
	Model.loadTemplates(_Type);
	for (int Vfor = 0; Vfor < Model.rowCount(); Vfor++)
	{
		STPhotoLayout CTemplate;
		QModelIndex CIndex = Model.index(Vfor, 0);

		//For each format
		STDom::DDocFormatList AvailableFormats = Model.availableFormats(CIndex);
		STDom::DDocFormatList::const_iterator fit;
		for (fit = AvailableFormats.begin(); fit != AvailableFormats.end(); ++fit)
		{
			CTemplate.load(Model.photoLayoutFileInfo(CIndex, *fit).absoluteFilePath());
			//For each template
			STPhotoLayout::TAlbumsList::const_iterator it;
			STPhotoLayout::TAlbumsList Albums = CTemplate.albums();
			for (it = Albums.begin(); it != Albums.end(); ++it)
			{
				if (!it->ref().isEmpty() && it->ref() != STPhotoBookTemplate::DefaultRef)
				{
					//Insert if it not exist
					FSqlQuery Query(*this);
					Query.prepare("SELECT ref FROM templates WHERE ref=:ref");
					Query.bindValue(":ref", it->ref());
					Query.exec();
					if (!Query.next())
					{
						Query.prepare("INSERT INTO templates (ref, description) VALUES (:ref, :description)");
						Query.bindValue(":ref", it->ref());
						Query.bindValue(":description", it->description());
						Query.exec();
						NInserts++;
					}
				}
			}
		}
	}
	return NInserts;
}

int SindarDatabase::importTemplateRefs()
{
	int NInserts = 0;
	NInserts += importTemplateRefs(STPhotoLayout::TypePhotoBook);
	NInserts += importTemplateRefs(STPhotoLayout::TypeCalendar);
	NInserts += importTemplateRefs(STPhotoLayout::TypeCard);
	NInserts += importTemplateRefs(STPhotoLayout::TypeIdPhoto);
	NInserts += importTemplateRefs(STPhotoLayout::TypeMultiPhoto);
	return NInserts;
}

SindarDatabase::TDefaultDatabaseList SindarDatabase::getDefaultDatabases()
{
	TDefaultDatabaseList Res;
	Res.push_back(DefaultSindarDatabase(QObject::tr("Sublimation Generic"), ":/sindarldefaultdb/sublimation_generic.db", QObject::tr("Generic database for sublimation printers")));
	Res.push_back(DefaultSindarDatabase(QObject::tr("Digital Generic"), ":/sindarldefaultdb/digital_generic.db", QObject::tr("Generic database for digital printers")));
	Res.push_back(DefaultSindarDatabase(QObject::tr("Sublimation and Digital Generic"), ":/sindarldefaultdb/digitalandsublimation_generic.db", QObject::tr("Generic database for sublimation and digital printers")));
	return Res;
}

void SindarDatabase::importDefaultPublisherProducts(const QFileInfo& _PubliserDbFile)
{

	//Check if we have publisher database
	Assert(_PubliserDbFile.exists(), Error(QObject::tr("Publisher database file: %1 , does not exists!").arg(_PubliserDbFile.absoluteFilePath())));
	STDom::PublisherDatabase SourceDataBase(_PubliserDbFile.absoluteFilePath());
	Assert(SourceDataBase.open(), Error(QString(QObject::tr("Could not open file %1 for database export.")).arg(_PubliserDbFile.absoluteFilePath())));
	STDom::PublisherDatabase DestDatabase(*this);
	FSqlDatabaseManager Manager(SourceDataBase, ":/");
	DestDatabase.importAll(Manager);
	SourceDataBase.close();
}
