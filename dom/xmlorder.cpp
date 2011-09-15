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

#include "xmlorder.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile> 
#include <QTextStream> 
#include <QTextCodec>
#include <QFileInfo> 
#include <QDir>
#include <QProgressBar>
#include "stutils.h"

using namespace STDom;

#define DATETIME_FORMAT "yyyy-MM-dd hh:mm:ss"

// _________________________________________________________________________
//
// Class XmlOrderDealer
// _________________________________________________________________________

XmlOrderDealer::XmlOrderDealer(const QDomElement& _Element)
{
	loadXml(_Element);
}

void XmlOrderDealer::clear()
{
	Id.clear(); 
	Name.clear(); 
	Address.clear(); 
	PostalCode.clear(); 
	City.clear(); 
	State.clear(); 
	Country.clear(); 
	TransportRoute.clear(); 
	Email.clear(); 
	Phone.clear(); 
	MobilePhone.clear(); 
	bool IsNull = true; 	
}

void XmlOrderDealer::loadXml(const QDomElement& _Element)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
 		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
 		if(!CEl.isNull())
 		{
 			if (CEl.tagName().toLower() == "postalcode" )
				PostalCode = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "city" )
				City = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "state" )
				State = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "country" )
				Country = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "tranportroute" )
				TransportRoute = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "address" )
				Address = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "email" )
				Email = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "phone" )
				Phone = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "mobilephone" )
				Phone = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "name" )
				Name = CEl.text();
			else
 			if (CEl.tagName().toLower() == "id" )
				Id = CEl.text();
 		}
		CNode = CNode.nextSibling();
	}	
}

QDomElement XmlOrderDealer::createElement(QDomDocument& _Doc, const QString& _TagName) const 
{
	QDomElement Dealer = _Doc.createElement(_TagName);
	
	if (!isEmpty())
	{
		QDomElement CElement = _Doc.createElement("address");
		CElement.appendChild(_Doc.createTextNode(Address));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("city");
		CElement.appendChild(_Doc.createTextNode(City));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("postalcode");
		CElement.appendChild(_Doc.createTextNode(PostalCode));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("state");
		CElement.appendChild(_Doc.createTextNode(State));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("country");
		CElement.appendChild(_Doc.createTextNode(Country));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("tranportroute");
		CElement.appendChild(_Doc.createTextNode(TransportRoute));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("mobilephone");
		CElement.appendChild(_Doc.createTextNode(MobilePhone));
		Dealer.appendChild(CElement);
		
		CElement = _Doc.createElement("email");
		CElement.appendChild(_Doc.createTextNode(Email));
		Dealer.appendChild(CElement);
	
		CElement = _Doc.createElement("phone");
		CElement.appendChild(_Doc.createTextNode(Phone));
		Dealer.appendChild(CElement);
	
		CElement = _Doc.createElement("name");
		CElement.appendChild(_Doc.createTextNode(Name));
		Dealer.appendChild(CElement);
	
		CElement = _Doc.createElement("id");
		CElement.appendChild(_Doc.createTextNode(Id));
		Dealer.appendChild(CElement);
	}
	return Dealer;
}

bool XmlOrderDealer::isEmpty() const
{
	return Id.isEmpty() && Name.isEmpty(); 
}


// _________________________________________________________________________
//
// Class XmlOrderPrint
// _________________________________________________________________________


void XmlOrderPrint::init()
{
	Src = ""; 
	PrintsRequested = 0; 
	BackPrintText = "";
	Description = ""; 
	ImageSize = 0; 
	Price = 0; 
}

XmlOrderPrint::XmlOrderPrint(const QDomElement& _Element)
{
	init(); 
	loadXml(_Element);
}

void XmlOrderPrint::loadXml(const QDomElement& _Element)
{
	Src = _Element.attribute("src", "");
	ImageSize = _Element.attribute("size", "0").toULongLong();
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
 		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
 		if(!CEl.isNull())
 		{
 			if (CEl.tagName().toLower() == "printsrequested" )
			{
				PrintsRequested = CEl.text().toUInt();
				Price = CEl.attribute("price", "0").toDouble(); 
			}	
			else 
 			if (CEl.tagName().toLower() == "backprinttext" )
				BackPrintText = CEl.text();
			else 
 			if (CEl.tagName().toLower() == "description" )
				Description = CEl.text();

 		}
		CNode = CNode.nextSibling();
	}		
}

QDomElement XmlOrderPrint::createElement(QDomDocument& _Doc) const
{
	QDomElement EPrint = _Doc.createElement("print");
	EPrint.setAttribute("size", ImageSize);
	EPrint.setAttribute("src", Src);

	QDomElement CElement = _Doc.createElement("printsrequested");
	CElement.appendChild(_Doc.createTextNode(QString::number(PrintsRequested)));
	CElement.setAttribute("price", Price); 
	EPrint.appendChild(CElement);

	CElement = _Doc.createElement("backprinttext");
	CElement.appendChild(_Doc.createTextNode(BackPrintText));
	EPrint.appendChild(CElement);

	CElement = _Doc.createElement("description");
	CElement.appendChild(_Doc.createTextNode(Description));
	EPrint.appendChild(CElement);

	
	return EPrint;
}


void XmlOrderPrint::clearDealerData()
{
	Price = 0; 
	Description = ""; 
}
	

// _________________________________________________________________________
//
// Class XmlOrder
// _________________________________________________________________________

void XmlOrder::loadHeader(QDomElement& _HElement)
{
	QDomNode CNode = _HElement.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "creationdatetime" )
				setCreationDateTime(QDateTime::fromString(CEl.text(), DATETIME_FORMAT)); 
			else 
			if (CEl.tagName().toLower() == "sentdate" )
				setSentDateTime(QDateTime::fromString(CEl.text(), DATETIME_FORMAT)); 
			else 
			if (CEl.tagName().toLower() == "sendercomment" )
				setSenderComment(CEl.text()); 
			else 
			if (CEl.tagName().toLower() == "sender" )
				Sender = XmlOrderDealer(CEl);
			else 
			if (CEl.tagName().toLower() == "customer" )
				Customer = XmlOrderDealer(CEl);
			else 
			if (CEl.tagName().toLower() == "publisher" )
				Publisher = XmlOrderDealer(CEl);
		}
		CNode = CNode.nextSibling();
	}	
}

QDomElement XmlOrder::createHeader(QDomDocument& _Doc) const
{
	QDomElement Header = _Doc.createElement("header");
	
	QDomElement ECDateTime = _Doc.createElement("creationdatetime");
	ECDateTime.appendChild(_Doc.createTextNode(CreationDateTime.toString(DATETIME_FORMAT)));
	Header.appendChild(ECDateTime);
	
	QDomElement ESentDate = _Doc.createElement("sentdate");
	ESentDate.appendChild(_Doc.createTextNode(SentDateTime.toString(DATETIME_FORMAT)));
	Header.appendChild(ESentDate);

	QDomElement SenderCommentEl = _Doc.createElement("sendercomment"); 
	SenderCommentEl.appendChild(_Doc.createTextNode(SenderComment)); 
	Header.appendChild(SenderCommentEl); 
	
	Header.appendChild(Sender.createElement(_Doc, "sender"));
	Header.appendChild(Customer.createElement(_Doc, "customer"));
	Header.appendChild(Publisher.createElement(_Doc, "publisher"));
	
	return Header;
}

void XmlOrder::loadPrints(QDomElement& _HElement)
{
	QDomNode CNode = _HElement.firstChild();

	Prints.clear();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "print" )
				addPrint(_HElement.attribute("prodpublicref", ""), XmlOrderPrint(CEl));
		}
		CNode = CNode.nextSibling();
	}	
}

QDomElement XmlOrder::createPrints(const QString& _ProdRef, const TPrintList& _Prints, QDomDocument& _Doc) const
{
	QDomElement EPrints = _Doc.createElement("prints");
	EPrints.setAttribute("prodpublicref", _ProdRef);
	TPrintList::const_iterator pit; 
	for (pit = _Prints.begin(); pit != _Prints.end(); ++pit)
	{
		EPrints.appendChild((*pit).createElement(_Doc));
	}
	return EPrints;
}

void XmlOrder::clear()
{
	(*this) = XmlOrder();
}

void XmlOrder::clearDealerData()
{
	Sender.clear(); 
	Customer.clear();
	SenderComment = ""; 
	//Clear prints dealer data
	TProductPrints::iterator it; 
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		TPrintList::iterator pit; 
		for (pit = it->begin(); pit != it->end(); ++pit)
		{
			pit->clearDealerData(); 
		}
	}
} 

QString XmlOrder::defaultXmlFilePath() const
{
	return XmlOrderInfo(Id).orderXmlFilePath();
}

XmlOrderInfo XmlOrder::orderInfo() const
{
	return XmlOrderInfo(Id);
}

bool XmlOrder::loadXml(const QString& _FileName)
{
	bool Res = false;
	QDomDocument Doc("kpsxmlorder");
	QFile File(_FileName);
	CurrentXmlDir = QFileInfo(_FileName).dir();
	if (File.open(QIODevice::ReadOnly))
	{
		QTextStream StrIn(&File);
		StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
		Res = Doc.setContent(StrIn.readAll());
		File.close();
	}
	if (Res)
	{
		Res = Doc.doctype().name().toLower() == "kpsxmlorder";
		QDomElement RootEl = Doc.documentElement();
		setId(RootEl.attribute("id", "0"));
		setGlobalId(RootEl.attribute("globalid", "")); 
		setType(RootEl.attribute("type", "0").toInt());
		setVersion(RootEl.attribute("version", "0.0.0"));
		QDomNode CNode = RootEl.firstChild();
		while(!CNode.isNull())
		{
			QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
			if(!CEl.isNull())
			{
				if (CEl.tagName().toLower() == "header" )
					loadHeader(CEl);
				if (CEl.tagName().toLower() == "prints" )
					loadPrints(CEl);
			}
			CNode = CNode.nextSibling();
		}
		IsNull = !Res;
	}
	return Res;
}

bool XmlOrder::loadXml()
{
	return loadXml(defaultXmlFilePath());
}

bool XmlOrder::saveXml(const QString& _FileName)
{
	bool Res = false;
	
	QFile TextFile(_FileName);
	if (TextFile.open(QFile::WriteOnly | QFile::Truncate) )
	{
		QTextStream Out(&TextFile);
		//Out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
		Out.setCodec(QTextCodec::codecForName("UTF-8"));
		QDomDocument Doc("kpsxmlorder");
		QDomElement Root = Doc.createElement("order");
		Doc.appendChild(Root);
		Root.setAttribute("version", "1.1.0");
		Root.setAttribute("type", Type);
		Root.setAttribute("id", Id);
		Root.setAttribute("globalid", GlobalId); 
		Root.appendChild(createHeader(Doc));
				relativePrintBasePath(QFileInfo(_FileName).dir());
		TProductPrints::const_iterator it; 
		for (it = Prints.begin(); it != Prints.end(); ++it)
		{	
			Root.appendChild(createPrints(it.key(), *it, Doc));
		}
		
		Out << Doc.toString();
		Res = true;
	}
	return Res;
}

bool XmlOrder::saveXml()
{
	return saveXml(defaultXmlFilePath());
}


void XmlOrder::saveFiles(QProgressBar* _ProgBar)
{
	saveFiles(XmlOrderInfo(Id).orderPath(), _ProgBar);
}

void XmlOrder::saveFiles(const QDir& _OrderDir, QProgressBar* _ProgBar)
{
	ErrorStack.clear();
	if (!_OrderDir.exists())
		_OrderDir.mkpath(_OrderDir.absolutePath());


	QFileInfoList Files = files();
	if (_ProgBar)
	{
		_ProgBar->setRange(0, Files.size() - 1);
		_ProgBar->setValue(0);
		QApplication::processEvents();
	}

	QFileInfoList::iterator it;
	for (it = Files.begin(); it != Files.end(); ++it)
	{
		QFile CurrFile(it->absoluteFilePath());
		QString DestFileName = _OrderDir.filePath(it->fileName());
		StackAssert(CurrFile.copy(DestFileName), Error(QObject::tr("Error copying from %1 to %2").arg(it->absoluteFilePath()).arg(DestFileName)), ErrorStack);
		if (_ProgBar)
		{
			_ProgBar->setValue(_ProgBar->value() + 1);
			QApplication::processEvents();
		}
	}
}


void XmlOrder::remove(const QDir& _CommandDir)
{
	Assert(STUtils::rmDir(_CommandDir),  Error(QObject::tr("Could not delete directory: %1").arg(_CommandDir.absolutePath())));
}

/*!
	Removes command path.
*/
void XmlOrder::remove()
{
	Assert(!Id.isEmpty(), Error(QObject::tr("Could not remove command without id")));
	QDir Dir = XmlOrderInfo(Id).orderPath();
	remove(Dir);
}


void XmlOrder::addPrint(const QString& _ProductRef, const XmlOrderPrint& _Print)
{
	Prints[_ProductRef].push_back(_Print);
}

QFileInfoList  XmlOrder::files() const
{
	QFileInfoList Res;
	TProductPrints::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		TPrintList CurrentPrintList = it.value();
		TPrintList::iterator pit;
		for (pit = CurrentPrintList.begin(); pit != CurrentPrintList.end(); ++pit)
		{
			QFileInfo CurrentFileInfo = CurrentXmlDir.filePath( pit->src());
			if (!Res.contains( CurrentFileInfo))
				Res.push_back(CurrentFileInfo);
		}
	}
	return Res;
}

QString XmlOrder::lastError()
{
	return QString("");
}

void XmlOrder::relativePrintBasePath(const QDir& _Dir)
{
	TProductPrints::iterator it; 
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		TPrintList::iterator pit; 
		for (pit = it->begin(); pit != it->end(); ++pit)
		{
			//pit->setSrc(pit->src().replace(_Before, _After));
			pit->setSrc(_Dir.relativeFilePath(pit->src()));
		}
	}
}

void XmlOrder::setPrintBasePath(const QString& _BasePath)
{
	TProductPrints::iterator it; 
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		TPrintList::iterator pit; 
		for (pit = it->begin(); pit != it->end(); ++pit)
		{
			pit->setSrc(_BasePath + "/" + pit->src());
		}
	}
}

void XmlOrder::clearPrintPath()
{
	TProductPrints::iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		TPrintList::iterator pit;
		for (pit = it->begin(); pit != it->end(); ++pit)
		{
			pit->setSrc(QFileInfo(pit->src()).fileName());
		}
	}

}
