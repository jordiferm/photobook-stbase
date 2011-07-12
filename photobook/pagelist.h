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

class STProgressIndicator;
namespace STPhotoBook
{
class ST_PHOTOBOOK_EXPORT PageList : public QList<TemplateScene*>
{
public:
	explicit PageList();
	void save(const QString& _AbsoluteFileName, bool _SaveResources, bool _OnlyDesignImages, STProgressIndicator* _ProgressBar = 0);
	void load(const QString& _AbsoluteFileName, STProgressIndicator* _ProgressBar = 0);
	void removeTextPages();
	//! Texts items with %Title% pattern.
	void setTitleText(const QString& _String);
};

}
#endif // PAGELIST_H
