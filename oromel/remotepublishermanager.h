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
#ifndef REMOTEPUBLISHERMANAGER_H
#define REMOTEPUBLISHERMANAGER_H


#include "starlababstractmanager.h"
#include "storomelexport.h"
#include "publisher.h"
#include "templateinfolist.h"
#include "publisherdatabase.h"
#include "sterror.h"
#include "publisherinfo.h"

class DefaultSindarDatabase;
class SProcessStatusWidget;
class ST_OROMEL_EXPORT RemotePublisherManager
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	StarlabAbstractManager* Manager;
	QString PublisherDatabaseFilePath;
	void pullTemplates(const STDom::Publisher& _Publisher );
	void downloadTemplateMetaInfo(const SPhotoBook::TemplateInfoList& _Templates, const STDom::Publisher& _Publisher  );

public:
	RemotePublisherManager(StarlabAbstractManager* _Manager, const QString& _PublisherDatabaseFilePath);
	StarlabAbstractManager* starlabManager() { return Manager; }
	SPhotoBook::TemplateInfoList getTemplates();
	void pullProducts(bool _UpToDateOnly = true, bool _PreserveLocalAdded = false);
	void pushProducts();
	void pushTemplate(const SPhotoBook::TemplateInfo& _Template, SProcessStatusWidget* _L1Process = 0, SProcessStatusWidget* _L2Process = 0);
	void downloadTemplateDesign(const SPhotoBook::TemplateInfo& _Template, const SPhotoBook::DesignInfo& _Design, SProcessStatusWidget* _L1Process = 0);
	bool productsDBExist();
	void addDefaultProducts(DefaultSindarDatabase& _SindarDatabase);
	QString publisherDatabaseFilePath() const { return PublisherDatabaseFilePath; }
	STDom::PublisherDatabase getProducts() const;
	STDom::PublisherInfo getPublisherInfo() const;
	void removeAllProducts();
	void updateTemplateProducts(STDom::PublisherDatabase& _ProductsDb, const SPhotoBook::TemplateInfoList& _Templates);
};

#endif // REMOTEPUBLISHERMANAGER_H
