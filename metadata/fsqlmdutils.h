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
#ifndef FSQLMDUTILS_H
#define FSQLMDUTILS_H

#include "fsqlquery.h"
#include "sterror.h"
#include "stmetadataexport.h"

/**
Flam Sql MetaData Utils.
M�todes est�tics per contra la base de dades utilitzant MetaData.

@author Shadow
*/
class FTableMetaData;
class QSqlRecord;
class QString;
class FActionWidget;
class QWidget;
class FSqlFieldEditor;
class ST_METADATA_EXPORT FSqlMDUtils
{
public: 
	ST_DECLARE_ERRORCLASS();

public:
	static FSqlQuery relatedQuery(const QString& _RelatedTableName, const QString& _SelectValues, 
		const QSqlRecord& _Values, const FTableMetaData& _TableMetaData );
	static FSqlQuery relatedQuery(const QString& _RelatedTableName, const QString& _SelectValues, 
			const QSqlRecord& _Values, const QString& _TableName );
	//! Assigna els valors per defecte de MetaData a _Record en cas de que existeixin.
	static void setDefaultValues(const FTableMetaData& _MetaData, QSqlRecord& _Record);
};

#endif
