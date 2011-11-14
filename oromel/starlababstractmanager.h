/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
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
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef STARLABABSTRACTMANAGER_H
#define STARLABABSTRACTMANAGER_H

#include "storomelexport.h"
#include "publisher.h"
#include "templateinfolist.h"
#include "publisherdatabase.h"
#include "iddesctablemodel.h"

class ST_OROMEL_EXPORT StarlabAbstractManager
{

public:
    StarlabAbstractManager();
	virtual STDom::Publisher getPublisher() = 0;
	virtual bool productsUpToDate(const QString& _Hash) { return false; }
	virtual void updateProductHash(const QString& _Hash) {}
	virtual bool templatesUpToDate(const QString& _Hash) { return false; }
	virtual void updateTemplateHash(const QString& _Hash) {}
	virtual void syncProducts(const STDom::PublisherDatabase& ) {}
	virtual void syncTemplates(const SPhotoBook::TemplateInfoList& ) {}
	virtual STDom::IdDescTableModel::TKeyValueList getCollectionPoints() const { return STDom::IdDescTableModel::TKeyValueList(); }
	virtual STDom::IdDescTableModel::TKeyValueList getPaymentTypes() const { return STDom::IdDescTableModel::TKeyValueList(); }
	virtual STDom::IdDescTableModel::TKeyValueList getShippingMethods() const { return STDom::IdDescTableModel::TKeyValueList(); }
	virtual double getShippingMethodAmount(const QVariant& _IdShippingMethod) const { return 0; }
	virtual QUrl infoUrl(const SPhotoBook::TemplateInfo& _Template) { return QUrl(); }
	virtual void editTemplateInfo(const SPhotoBook::TemplateInfo& _Template) {}
};

#endif // STARLABABSTRACTMANAGER_H
