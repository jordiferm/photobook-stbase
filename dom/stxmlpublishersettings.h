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

// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#ifndef STXMLPUBLISHERSETTINGS_H
#define STXMLPUBLISHERSETTINGS_H

#include <QString> 
#include "stdomexport.h"
#include "sterror.h"

namespace STDom
{

/**
	@author
*/
class ST_DOM_EXPORT STXmlPublisherSettings
{
	QString Id, Name, Email, Web;
	QString DBUser, DBPassword, DBHost, DBDir; 
	bool CodPayment, CCPayment, PayPalPayment, TransferBackPrintAsFileName, HasDealer;  
	QString Url, UserName, Password, InitDir; 
	QString DealerUrl, DealerUserName, DealerPassword, DealerInitDir; 
	int TransferMode, DBTransferMode, DealerTransferMode; 
	int DBPort, Port, DealerPort;
	QString UpdatesDir;

public:
	ST_DECLARE_ERRORCLASS();
	
public:
	STXmlPublisherSettings();
	void clear(); 
	bool loadXml(const QString& _FileName); 
	QString dbUser() const;
	QString dbPassword() const;
	QString dbHost() const;
	int dbPort() const { return DBPort; }
	QString dbDir() const { return DBDir; }
	int dbTransferMode() const { return DBTransferMode; }
	bool cODPayment() const { return CodPayment; }
	bool cCPayment() const { return CCPayment; }
	bool payPalPayment() const { return PayPalPayment; }
	QString url() const;
	int port() const { return Port; }
	QString userName() const;
	QString password() const;
	QString initDir() const { return InitDir; }
	QString updatesDir() const { return UpdatesDir; }
	int transferMode() const { return TransferMode; }
	bool transferBackprintTextAsFileName() const { return TransferBackPrintAsFileName; }
	QString id() const { return Id; }
	QString name() const { return Name; } 
	QString email() const { return Email; } 
	QString web() const { return Web; }
	//Dealer info
	QString dealerUrl() const { return DealerUrl; }
	int dealerPort() const { return DealerPort; }
	QString dealerUserName() const { return DealerUserName; }
	QString dealerPassword() const { return DealerPassword; }
	QString dealerInitDir() const { return DealerInitDir; }
	int dealerTransferMode() const { return DealerTransferMode; }
	bool hasDealer() const { return HasDealer; }
	static STXmlPublisherSettings getDefaultSettings();
	bool isEmpty() const;
};
} //namespace stdom

#endif
