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

#ifndef FSQLDATABASE_H
#define FSQLDATABASE_H

#include "fsqldatabasesettings.h"
#include <QSqlDatabase> 
#include "stdatabaseexport.h"
#include "sterror.h"


/**
Representa la conexió amb una base de dades. 
Incorpora les següents característiques: 
- Creació automàtica de la base de dades a partir de la metainformació.
- Actualització automàtica de les taules a partir de la comprovació de versions dels fitxers de metainformació. Les versions poden ser automàticament controlades a partir de la substitució de tags de CVS.
- Gestió de la configuració de conexió a QSettings. 
 
 
@author Shadow
*/
class FTableMetaData;
class ST_DATABASE_EXPORT FSqlDatabase 
{
public:
	ST_DECLARE_ERRORCLASS();
	
private:
				
public:
	static bool start(FSqlDatabaseSettings& _Settings, const QString& _SqlInitFile = "");
	static bool create(const FSqlDatabaseSettings& _Settings, QString& _WarningMessage, const QString& _SqlInitFile = "");
	static void backup(const FSqlDatabaseSettings& _Settings, QString& _FileName); 
	static void deleteAllTables(const FSqlDatabaseSettings& _Settings);
	static void restore(const FSqlDatabaseSettings& _Settings, QString& _FileName);
	
};



#endif
