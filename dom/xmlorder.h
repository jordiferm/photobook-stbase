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

#ifndef KPSXMLORDER_H
#define KPSXMLORDER_H

#include <QDateTime> 
#include <QString>
#include <QMap>
#include <QSize> 
#include <QDir>
#include "stdomexport.h"
#include "sterror.h"
#include "sterrorstack.h"

#define XMLO_CURRENT_VERSION "1.2.0"


class QDomElement; 
class QDomDocument;
class QDir;
class QProgressBar;

namespace STDom {

class ST_DOM_EXPORT XmlOrderDealer
{
	QString Id; 
	QString Name; 
	QString Address; 
	QString PostalCode; 
	QString City;
	QString State;
	QString Country;
	QString TransportRoute;
	QString Email; 
	QString Phone; 
	QString MobilePhone;
	bool IsNull; 
	
public:
	XmlOrderDealer() : IsNull(true) {}
	XmlOrderDealer(const QString& _Id, const QString& _Name = "", const QString& _Address = "", 
					  const QString& _Email = "", const QString& _Phone = "") : 
		Name(_Name), Address(_Address), Email(_Email), Phone(_Phone) { setId(_Id); }
	
	XmlOrderDealer(const QDomElement& _Element);

	void clear();

	void loadXml(const QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc, const QString& _TagName) const;
	
	void setId(const QString& _Id) { Id = _Id; IsNull = false; }
	QString id() const { return Id; }
	
	void setName(const QString& _Value) { Name = _Value; }
	QString name() const { return Name; }
	
	void setPostalCode(const QString& _Value) { PostalCode = _Value; }
	QString postalcode() const { return PostalCode; }
	
	void setAddress(const QString& _Value) { Address = _Value; }
	QString address() const { return Address; }
	
	void setCity(const QString& _Value) { City = _Value; }
	QString city() const { return City; }
	
	void setState(const QString& _Value) { State = _Value; }
	QString state() const { return State; }
	
	void setCountry(const QString& _Value) { Country = _Value; }
	QString country() const { return Country; }
	
	void setTransportRoute(const QString& _Value) { TransportRoute = _Value; }
	QString transportRoute() const { return TransportRoute; }
	
	void setEmail(const QString& _Value) { Email = _Value; }
	QString email() const { return Email; }
	
	void setPhone(const QString& _Value) { Phone = _Value; }
	QString phone() const { return Phone; }
	
	void setMobilePhone(const QString& _Value) { MobilePhone = _Value; }
	QString mobilePhone() const { return MobilePhone; }
	
	//!Id is not set. 
	bool isNull() const { return IsNull; }

	//! It has no id and no name.
	bool isEmpty() const; 
};

class ST_DOM_EXPORT XmlOrderPrint
{
private:
	QString Src; 
	uint PrintsRequested;
	QString BackPrintText, Description;
	bool IsNull;
	qulonglong ImageSize;
	double Price; 
	
	void init(); 

public:
	XmlOrderPrint() : IsNull(true)  { init(); }
	XmlOrderPrint(const QDomElement& _Element);
	void loadXml(const QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc) const;
	
	XmlOrderPrint(const QString& _Src, int _PrintsRequested = 1)
	:  PrintsRequested(_PrintsRequested), ImageSize(0), Description(""), Price(0)
	{ setSrc(_Src); }
	
	void setSrc(const QString& _Value) { Src = _Value; IsNull = false; }
	QString src() const { return Src; }
	bool isNull() const { return IsNull; }
	
	uint printsRequested() const { return PrintsRequested; }
	
	void setImageSize(qulonglong _Size) { ImageSize = _Size; }
	qulonglong imageSize() const { return ImageSize; } 
	
	void setBackPrintText(const QString& _Value) { BackPrintText = _Value; }
	QString backPrintText() const { return BackPrintText; }

	void setDescription(const QString& _Value) { Description = _Value; }
	QString description() const { return Description; }

	void setPrice(double _Value) { Price = _Value; }
	double price() const { return Price; }
	
	void clearDealerData(); 
};

class ST_DOM_EXPORT XmlOrderInfo
{
	static QString DefaultOrderFilesPath;
	QString OrderFilesPath;
	QString OrderId;

public:
	XmlOrderInfo(const QString& _OrderId, const QString _OrderFilesPath = DefaultOrderFilesPath)
			: OrderId(_OrderId), OrderFilesPath(_OrderFilesPath) {}
	QString orderPath() const { return OrderFilesPath + "/" + OrderId + "/"; }
	QString orderXmlFilePath() const { return orderPath() +  orderXmlFileName(); }
	static QString orderXmlFileName() { return "order.xml"; }
};


/**
	@author Jordi Fernàndez <shadow@softwarelliure.com>

\example 
<order version="1.1.0" type="0" id="1" globalid="08_00_27_BD_98_95_3_a9d63040nd1343021983092493" >
    <header>
      <kioskid>01</kioskid>
      <creationdateandtime>2007-01-12 09:46:46</creationdateandtime>
      <sentdateandtime>2007-01-12 11:28:38</sentdateandtime>
      <sender>
        <address>Some address</address>
        <email>email@server.com</email>
        <phone>555233455</phone>
        <name>My name</name>
        <id>001</id>
      </sender>
      <customer>
        <address>Some address</address>
        <email>email@server.com</email>
        <phone>555233455</phone>
        <name>My name</name>
        <id>001</id>
      </customer>
      <publisher>
        <address>Some address</address>
        <email>email@server.com</email>
        <phone>555233455</phone>
        <name>My name</name>
        <id>001</id>
      </publisher>
    </header>
    <prints prodpublicref="0000001">
      <print size="1024" rawprint="true" src="images/img001.jpg" >
        <papersurface>4</papersurface>
        <printwidth>152</printwidth>
        <printheight>102</printheight>
        <printsrequested price="0.5">1</printsrequested>
        <backprinttext>001 -> arbol.jpg</backprinttext>
        <description>Description 10x15 AF.Br.</description>
      </print>
    </prints>
    <products/>
  </order>
 
*/

class ST_DOM_EXPORT XmlOrder
{
public: 
	ST_DECLARE_ERRORCLASS();
	STErrorStack ErrorStack;
	typedef QList<XmlOrderPrint> TPrintList;
	typedef QMap<QString, TPrintList> TProductPrints;
	
private:
	
	QString LastError;
	QDateTime CreationDateTime;
	QDateTime SentDateTime;
	XmlOrderDealer Sender; 
	XmlOrderDealer Customer;
	XmlOrderDealer Publisher;
	TProductPrints Prints;
	int Type; 
	QString Version; 
	QString Id; 
	QString GlobalId; 
	QString SenderComment; 
	QDir CurrentXmlDir;
	bool IsNull;
	
//TODO: Define order types. 	
	void loadHeader(QDomElement& _HElement);
	QDomElement createHeader(QDomDocument& _Doc) const;
	void loadPrints(QDomElement& _HElement);
	QDomElement createPrints(const QString& _ProdRef, const TPrintList& _Prints, QDomDocument& _Doc) const;
	
public:
	XmlOrder(const QString& _Id = "") : Id(_Id), IsNull(true), Type(-1), Version(XMLO_CURRENT_VERSION) {}
	//! => not Loaded.
	bool isNull() const { return IsNull; }
	//! => isNull = true;
	void clear(); 
	//!Prepares data for publisher removing all data related to dealer.
	void clearDealerData(); 
	QString defaultXmlFilePath() const;
	XmlOrderInfo orderInfo() const;
	bool loadXml(const QString& _FileName);
	bool loadXml();
	bool saveXml(const QString& _FileName);
	bool saveXml();
	void saveFiles(QProgressBar* _ProgBar);
	void saveFiles(const QDir& _OrderDir, QProgressBar* _ProgBar = 0);
	void remove(const QDir& _OrderDir);
	void remove();
	void addPrint(const QString& _ProductRef, const XmlOrderPrint& _Print);
	TProductPrints prints() const { return Prints; }
	QFileInfoList  files() const;
	void clearPrints() { Prints.clear(); }
	
	//! Returns last error status if load or save returns false.
	QString lastError();
	
	void setSenderComment(const QString& _Comment) { SenderComment = _Comment.left(1024); }
	QString senderComment() const { return SenderComment; }
	
	void setCreationDateTime(const QDateTime& _Value) { CreationDateTime = _Value; }
	QDateTime creationDateTime() const { return CreationDateTime; }
	
	void setSentDateTime(const QDateTime& _Value) { SentDateTime = _Value; }
	QDateTime sentDateTime() const { return SentDateTime; }
	
	void setSender(const XmlOrderDealer& _Value) { Sender = _Value; }
	XmlOrderDealer sender() const { return Sender;}
	
	void setCustomer(const XmlOrderDealer& _Value) { Customer = _Value; }
	XmlOrderDealer customer() const { return Customer;}

	void setPublisher(const XmlOrderDealer& _Value) { Publisher = _Value; }
	XmlOrderDealer publisher() const { return Publisher;}
	
	void setType(const int _Type) { Type = _Type; }
	int type() const { return Type; }
	
	void setVersion(const QString& _Value) { Version = _Value; }
	QString version() const { return Version; }
	
	void setId(const QString& _Id) { Id = _Id; }
	QString id() const { return Id; }

	void setGlobalId(const QString& _Id) { GlobalId = _Id; }
	QString globalId() const { return GlobalId; }
	
	void relativePrintBasePath(const QDir& _Dir);
	//! Appends _BasePath to all print srcs.
	void setPrintBasePath(const QString& _BasePath);
	void clearPrintPath();

	//Errors
	STErrorStack errorStack() const { return ErrorStack; }

};
} //Dom Namespace

#endif
