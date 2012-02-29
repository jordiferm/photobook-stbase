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

#ifndef FSQLDATABASE_H
#define FSQLDATABASE_H

#include "fsqldatabasesettings.h"
#include <QSqlDatabase> 
#include "stdatabaseexport.h"
#include "sterror.h"


/**
Representa la conexi� amb una base de dades. 
Incorpora les seg�ents caracter�stiques: 
- Creaci� autom�tica de la base de dades a partir de la metainformaci�.
- Actualitzaci� autom�tica de les taules a partir de la comprovaci� de versions dels fitxers de metainformaci�. Les versions poden ser autom�ticament controlades a partir de la substituci� de tags de CVS.
- Gesti� de la configuraci� de conexi� a QSettings. 
 
 
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
