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

#ifndef PRINTJOB_H
#define PRINTJOB_H

#include <QList>
#include <QMap>
#include <QString>
#include <QFileInfoList>
#include "ddoc.h"
#include "stdomexport.h"


namespace STDom
{

class XmlOrder;
class ST_DOM_EXPORT PrintJob
{
	DDocPrintList Prints;
	typedef QList<int> TIndexList;
	typedef QMap<QString, TIndexList> TDocPrintsMap;
	TDocPrintsMap DocPrintsMap;
	TIndexList indexList(const QFileInfo& _FileInfo) const;
	int findPrintIndex(const DDocProduct& _Product, const QFileInfo& _FileInfo) const;

public:
    PrintJob();
	DDocProductList products() const;
	DDocProductList products(const QFileInfo& _File) const;
	DDocFormatList formats() const;
	DDocFormatList formats(const QFileInfo& _File) const;
	int copies(const DDocProduct& _Product, const QFileInfo& _FileInfo) const;
	int totalCopies() const;
	DDocPrintList prints() const;
	DDocPrintList prints(const DDocProduct& _Product) const;
	DDocPrintList prints(const QFileInfo& _File) const;
	QFileInfoList files() const;

	void addPrint(const DDocPrint& _Print);
	void addPrints(const DDocPrintList& _Prints);
	//! removes print at _Index
	void removePrintAt(int _Index);
	void removeAllPrints(const QFileInfo& _Source);
	void addCopies(const DDocProduct& _Product, const QFileInfo& _FileInfo, int _Copies);
	void setCopies(const DDocProduct& _Product, const QFileInfo& _FileInfo, int _Copies);
	void copyPrints(const QFileInfo& _Source, const QFileInfo& _Dest);
	//! Removes all prints of _Source
	void clear();
	void addOrderPrints(XmlOrder& _Order) const;
	bool isEmpty() const;
};
}
#endif // PRINTJOB_H
