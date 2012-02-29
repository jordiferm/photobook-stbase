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

#ifndef FSQLRECORD_H
#define FSQLRECORD_H

#include <qsqlrecord.h>
#include "stdatabaseexport.h"


/**
Simplement QSqlRecord amb informaci� extesa.
Cont� el nom de la taula a la que pertany el QSqlRecord.
 
@author Shadow
*/
class ST_DATABASE_EXPORT FSqlRecord : public QSqlRecord
{
	QString TableName;

public:
	FSqlRecord(const QString& _TableName);
	FSqlRecord(const QSqlRecord& _Other, const QString& _TableName);
	~FSqlRecord();
	QString tableName() const {return TableName;}
	void setTableName(const QString& _TableName) { TableName = _TableName; }
	FSqlRecord& operator=(const QSqlRecord& _Other);

};

#endif
