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

#ifndef ORDERDATABASE_H
#define ORDERDATABASE_H
#include "stdomexport.h"
#include <QSqlDatabase>
#include "sterror.h"
#include "publisherdatabase.h"

namespace STDom
{
class PublisherDatabase;
class XmlOrder;
class ST_DOM_EXPORT OrderDatabase : public QSqlDatabase
{
public:
	ST_DECLARE_ERRORCLASS();
	enum EnOrderStatus
	{
		StatusUnknown,
		StatusSended,
		StatusSending,
		StatusInSpooler,
		StatusPrinting,
		StatusPrinted,
		StatusPrintFailed,
		StatusCancelled,
		StatusDeleted //Automatically deleted.
	};
public:
	static const QString DefaultOrderDBConnectionName;

public:
	OrderDatabase(const QString& _DatabaseName);
	OrderDatabase(const QSqlDatabase& _Other = QSqlDatabase::database());
	static QString statusToStr(EnOrderStatus _Status, bool _Receiving = true);
	//! \return The new order id.
	QString insertOrder(const STDom::XmlOrder& _Order,const STDom::PublisherDatabase& _PublisherDb = QSqlDatabase());
};
} //namespace

#endif // ORDERDATABASE_H
