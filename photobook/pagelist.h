/****************************************************************************
**
** Copyright (C) 2006-2011 SoftTopia. All rights reserved.
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

#ifndef PAGELIST_H
#define PAGELIST_H
#include <QList>
#include "templatescene.h"
#include "stphotobookexport.h"
#include "sterror.h"

class STProgressIndicator;
namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT PageList : public QList<TemplateScene*>
{
public:
	ST_DECLARE_ERRORCLASS();

private:
	QDomDocument createDoc();

public:
	explicit PageList();
	void saveXml(const QString& _XmlFileName, const QString& _EncryptionKey = "");
	//Return a list of stored files.
	QStringList saveResources(const SPhotoBook::CollectionInfo& _CInfo, bool _OnlyDesignImages = false, STProgressIndicator* _Progress = 0);
	void loadXml(const QString& _AbsoluteFileName, QObject* _PagesParent, const QString& _EncryptionKey = "", QProgressBar* _ProgressBar = 0);
	void removeTextPages();
	//! Texts items with %Title% pattern.
	void setTitleText(const QString& _String);
	bool containsTemplate(TemplateScene* _Template) const;
	void sort();
};

bool PageListItemLessThan(TemplateScene* _Scene1, TemplateScene* _Scene2);

}
#endif // PAGELIST_H
