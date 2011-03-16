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

#ifndef KPPRINTERINFO_H
#define KPPRINTERINFO_H

#include <QImage>
#include <QSqlRecord>
#include "stprnlibexport.h"

/**
Informació d'impressora.

@author shadow
*/
class QSqlRecord;
class KPhotoProduct;

class ST_PRNLIB_EXPORT KPPrinterInfo
{

public:
	enum EnPrinterType
	{
		TypeKPSPrinter, 
		TypeRemoteFTP, 
		TypeKDEPrinter,
		TypeHotFolder, 
		TypeMailSMTP, 
		TypeFrontierC8Printer,
		TypeAgfa101DPrinter, 
		TypeIOSlavePrinter,
		TypeNoritzuOSSPrinter,
		TypeKisPrinter, 
		TypeDVDTorch, 
		TypeSystemPrinter
	};

private:	
	QString Name;
	QString Description; 
	QString ServerAddress;
	QString Id; 
	int Port; 
	QImage Image;
	EnPrinterType Type;
	QSqlRecord Record;
	
public:
	KPPrinterInfo(EnPrinterType _Type = TypeKPSPrinter,  const QString& _Name = "", 
		const QString& _Description = "", const QString& _Id = "");
	void setId(const QString& _Id) { Id = _Id; }
	QString id() const { return Id; }
	void setName(const QString& _Name) { Name = _Name;}
	QString name() const { return Name; } 
	void setDescription(const QString& _Description) { Description = _Description; } 
	QString description() const { return Description; }
	void setServerAddress(const QString& _ServerAddress) { ServerAddress = _ServerAddress; }
	QString serverAddress() const { return ServerAddress; }
	void setPort(int _Port) { Port = _Port; }
	int port() const { return Port; }
	EnPrinterType type() const { return Type; }
	QString typeName() const;
	void setRecord(const QSqlRecord& _Record) { Record = _Record; }
	QSqlRecord record() const { return Record; }
	//static KPPrinterInfo fetchFromDb(const QVariant& _PrinterId);
	//static QVariant defaultPrinter(const KPhotoProduct& _Product, int _Priority, const QString& _KioskId, const QVariant& _CustomerId);


};

#endif
