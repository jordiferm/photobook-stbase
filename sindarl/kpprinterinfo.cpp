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

#include "kpprinterinfo.h"

KPPrinterInfo::KPPrinterInfo(EnPrinterType _Type, const QString& _Name, const QString& _Description, const QString& _Id) 
	: Name(_Name), Description(_Description), Id(_Id), Type(_Type)
{
}

QString KPPrinterInfo::typeName() const
{
	QString Res; 
	switch (Type)
	{
		case TypeKPSPrinter:
			Res = "KPSPrinter";
		break;
		case TypeRemoteFTP:
			Res = "RemoteFTP";
		break;
		case TypeKDEPrinter:
			Res = "KDEPrinter";
		break;
		case TypeHotFolder:
			Res = "HotFolder";
		break;
		case TypeMailSMTP:
			Res = "MailSMTP";
		break;
		case TypeFrontierC8Printer:
			Res = "FrontierC8Printer";
		break;
		case TypeAgfa101DPrinter:
			Res = "Agfa101DPrinter";
		break;
		case TypeIOSlavePrinter:
			Res = "IOSlavePrinter";
		break;
		case TypeNoritzuOSSPrinter:
			Res = "NoritsuOSSPrinter ";
		break;
		case TypeKisPrinter:
			Res = "KisPrinter";
		break;
		case TypeDVDTorch:
			Res = "DVDTorch";
		break;
	}
	return Res;
}





/*KPPrinterInfo KPPrinterInfo::fetchFromDb(const QVariant& _PrinterId)
{
	KPPrinterInfo Res; 
	FSqlQuery Query;
	Query.prepare("SELECT * FROM printers WHERE idprinters=:idprinters");
	Query.bindValue(":idprinters", _PrinterId.toString()); 
	Query.exec(); 
	if (Query.next())
	{
		QSqlRecord Rec = Query.record();
		QVariant IdPrinter = Rec.value("idprinters");
		KPPrinterInfo CPrinterInfo(static_cast<KPPrinterInfo::EnPrinterType>(Rec.value("type").toInt()),
		Rec.value("description").toString(),Rec.value("comment").toString(), Rec.value("idprinters").toString());
		
		CPrinterInfo.setServerAddress(Rec.value("hosturl").toString());
		CPrinterInfo.setPort(Rec.value("hostport").toInt());
		CPrinterInfo.setRecord(Rec);
		Res = CPrinterInfo; 
	}
	return Res; 
}


QVariant KPPrinterInfo::defaultPrinter(const KPhotoProduct& _Product, int _Priority, const QString& _KioskId, KPSqlCommand::EnCommandType _Type, const QVariant& _CustomerId)
{
	QVariant Res;
	if (_Type == KPSqlCommand::TypeEmail)
	{
		FSqlQuery Query(
				QString("SELECT idprinters FROM printers WHERE type=%1").arg(KPPrinterInfo::TypeMailSMTP)); 
		if (Query.next())
		{
			Res = Query.value(0); 
		}
	}
	else 
	{
		FSqlQuery Query; 
		Query.prepare("SELECT printworkflows.*, printers.type FROM "
				"printworkflows INNER JOIN printers ON printworkflows.printers_idprinters = printers.idprinters WHERE "
				"( commandpriorities_idpriorities=:commandpriorities_idpriorities OR commandpriorities_idpriorities IS NULL ) AND "
				"( customers_idcustomers=:customers_idcustomers OR customers_idcustomers IS NULL ) AND "
				"( kiosks_idkiosks=:kiosks_idkiosks OR kiosks_idkiosks IS NULL ) AND "
				"( products_publicreference=:products_publicreference OR products_publicreference IS NULL ) ");
		Query.bindValue(":commandpriorities_idpriorities", _Priority);
		Query.bindValue(":customers_idcustomers", _CustomerId);
		Query.bindValue(":kiosks_idkiosks", _KioskId);
		Query.bindValue(":products_publicreference", _Product.ref());
		Query.exec();
		int CurrNumNulls = 4;
		while (Query.next())
		{
			QSqlRecord CRecord = Query.record();
			int NumNulls = 0;
			for (int Vfor=0; Vfor < CRecord.count(); Vfor++)
			{
				if (CRecord.isNull(Vfor))
					NumNulls++;
			}
			if (NumNulls < CurrNumNulls || Res.isNull())
			{
				if (KPPrinterInfo::printerSupportsCommandType(static_cast<KPPrinterInfo::EnPrinterType>(
								CRecord.value("type").toInt()), _Type))
				{
					Res = CRecord.value("printers_idprinters");
					CurrNumNulls = NumNulls; 
				}
			}
		}		 
	}
	return Res;


}*/



