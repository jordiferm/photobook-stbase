/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "pagelist.h"
#include <QTextCodec>
#include <QDomDocument>
#include <QDomElement>
#include <QObject>
#include <QTextStream>
#include <QApplication>
#include <QProgressBar>
#include <QSizeF>
#include "stutils.h"
#include "stprogressindicator.h"

using namespace SPhotoBook;

QDomDocument PageList::createDoc(const QDir& _StoreDir)
{
	QDomDocument Doc("aili");
	QDomElement Root = Doc.createElement("photobook");
	Doc.appendChild(Root);
	Root.setAttribute("version", "2.0.0");

	PageList::iterator it;
	for (it = begin(); it != end(); ++it)
	{
		Root.appendChild((*it)->createElement(Doc, _StoreDir.absolutePath()));
	}
	return Doc;
}

PageList::PageList()
{
}

void PageList::saveXml(const QString& _XmlFileName, const QString& _EncryptionKey)
{
	QFile PBFile(_XmlFileName);
	Assert(PBFile.open(QFile::WriteOnly | QFile::Truncate), Error(QString(QObject::tr("Could not open file %1")).arg(PBFile.fileName())));

	QDomDocument Doc = createDoc(QFileInfo(_XmlFileName).dir());
	if (_EncryptionKey.isEmpty())
	{
		QTextStream Out(&PBFile);
		//Out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
		Out.setCodec(QTextCodec::codecForName("UTF-8"));
		Out << Doc.toString();
	}
	else
	{
		QDataStream Out(&PBFile);
		QByteArray InputData(Doc.toString().toUtf8());
		QByteArray OutputData = STUtils::encode(InputData, _EncryptionKey);
		Out << OutputData;
		//Out.writeBytes(OutputData, OutputData.length());
	}
	PBFile.close();

}

QStringList PageList::saveResources(const SPhotoBook::CollectionInfo& _CInfo, bool _OnlyDesignImages, STProgressIndicator* _Progress)
{
	if (_Progress)
	{
		_Progress->setRange(0, size() - 1);
		_Progress->start(QObject::tr("Saving Model..."));
		_Progress->setValue(0);
	}

	QString StoreImagesError;
	PageList::iterator it;
	QStringList StoredFiles;
	for (it = begin(); it != end(); ++it)
	{
		try
		{
			StoredFiles += (*it)->storePhotoItemImages(_CInfo, _OnlyDesignImages);
		}
		catch(STError& _Error)
		{
			StoreImagesError += _Error.description() + "\n";
		}
		if (_Progress)
			_Progress->incValue();
	}

	if (_Progress)
		_Progress->stop();

	Assert(StoreImagesError.isNull(), Error(QString(QObject::tr("There was errors storing images: %1")).arg(StoreImagesError)));
	return StoredFiles;
}


void PageList::loadXml(const QString& _AbsoluteFileName, QObject* _PagesParent, const QString& _EncryptionKey, QProgressBar* _ProgressBar)
{
	clear();
	QDomDocument Doc("aili");
	QFile File(_AbsoluteFileName);
	QFileInfo XmlFInfo(File);
	Assert(File.open(QIODevice::ReadOnly), Error(QObject::tr("Loading Photo Book from file %1").arg(File.fileName())));
	{
		if (_EncryptionKey.isEmpty())
		{
			QTextStream StrIn(&File);
			StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
			Assert(Doc.setContent(StrIn.readAll()), Error(QObject::tr("Error setting document content")));
			File.close();
		}
		else
		{
			QDataStream DataIn(&File);
			QByteArray BArray;
			DataIn >> BArray;
			QByteArray DecodedData = STUtils::decode(BArray, _EncryptionKey);
			Assert(Doc.setContent(QString::fromUtf8(DecodedData)), Error(QObject::tr("Error setting document content")));
		}
	}
	QDomElement RootEl = Doc.documentElement();
	if (_ProgressBar)
	{
		_ProgressBar->setRange(0, RootEl.childNodes().count());
		_ProgressBar->setValue(0);
		QApplication::processEvents();
	}

	QDomNode CNode = RootEl.firstChild();
	while(!CNode.isNull())
	{
		if (_ProgressBar)
		{
			_ProgressBar->setValue(_ProgressBar->value() + 1);
			QApplication::processEvents();
		}
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "scene" )
			{
				TemplateScene* NewPage = new TemplateScene(_PagesParent);
				NewPage->loadElement(CEl, XmlFInfo.absolutePath());
				push_back(NewPage);
			}
		}
		CNode = CNode.nextSibling();
	}
}

void PageList::removeTextPages()
{

}

//! Texts items with %Title% pattern.
void PageList::setTitleText(const QString& _String)
{

}

bool PageList::containsTemplate(TemplateScene* _Template) const
{
	bool Found = false;
	const_iterator it = begin();
	while (!Found && it != end())
	{
		Found = *(*it) == *_Template;
		++it;
	}
	return Found;
}


bool SPhotoBook::PageListItemLessThan(TemplateScene* _Scene1, TemplateScene* _Scene2)
{
	return *(_Scene1) < *(_Scene2);
}

void PageList::sort()
{
	qSort(begin(), end(), SPhotoBook::PageListItemLessThan);
}

void PageList::resize(const QSizeF& _NewSize)
{
	iterator it = begin();
	while (it != end())
	{
		(*it)->resize(_NewSize);
		++it;
	}
}
