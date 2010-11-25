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

#ifndef FDBUTILS_H
#define FDBUTILS_H
#include <QStringList>
#include "stdatabaseexport.h"

/**
Utils per a base de dades.

@author Shadow
*/
class QSqlRecord;
class QSqlDatabase;
class QSqlRelationalTableModel;
class QSqlRecord;
class ST_DATABASE_EXPORT FDBUtils
{
public:
	static QStringList fieldList(const QSqlRecord& _Record);
	static void copyRecords(const QSqlDatabase& _Database, const QString& _FromTable, const QString& _ToTable, const QString& _Filter);
	static void setRelationDefaults(QSqlRelationalTableModel* _Model, QSqlRecord& _Record);
	static bool isSqlite(const QSqlDatabase& _Database); 
};

#endif
