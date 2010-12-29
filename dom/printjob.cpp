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

#include "printjob.h"
#include "xmlorder.h"

using namespace STDom;


PrintJob::TIndexList PrintJob::indexList(const QFileInfo& _FileInfo) const
{
	TIndexList  Res;
	QString FilePath = _FileInfo.absoluteFilePath();
	if (DocPrintsMap.contains(FilePath))
	{
		Res = DocPrintsMap[FilePath];
	}
	return Res;
}

/*!
  \return Index for a print with _Product and _FileInfo. -1 if it isn't found.
*/
int PrintJob::findPrintIndex(const DDocProduct& _Product, const QFileInfo& _FileInfo) const
{
	int Res = -1;

	TIndexList List = indexList(_FileInfo);
	if (!List.isEmpty())
	{
		TIndexList::const_iterator it = List.begin();
		bool Found = false;
		while (!Found && it != List.end())
		{
			Found = Prints[*it].product() == _Product;
			if (Found)
				Res = *it;
			++it;
		}
	}
	return Res;
}

PrintJob::PrintJob()
{}

DDocProductList PrintJob::products() const
{
	DDocProductList Res;
	DDocPrintList::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		if (!Res.contains(it->product()) && it->numCopies() > 0)
			Res.push_back(it->product());
	}
	return Res;
}

DDocProductList PrintJob::products(const QFileInfo& _File) const
{
	DDocProductList Res;
	TIndexList IndexList = indexList(_File);
	TIndexList::const_iterator it;
	for (it = IndexList.begin(); it != IndexList.end(); ++it)
	{
		Res.push_back(Prints[*it].product());
	}
	return Res;
}

DDocFormatList PrintJob::formats() const
{
	DDocFormatList Res;
	DDocProductList ProductList = products();

	DDocProductList::const_iterator it;
	for (it = ProductList.begin(); it != ProductList.end(); ++it)
	{
		if (!Res.contains(it->format()))
			Res.push_back(it->format());
	}
	return Res;
}

DDocFormatList PrintJob::formats(const QFileInfo& _File) const
{
	DDocFormatList Res;
	DDocProductList ProductList = products(_File);
	DDocProductList::const_iterator it;
	for (it = ProductList.begin(); it != ProductList.end(); ++it)
	{
		if (!Res.contains(it->format()))
			Res.push_back(it->format());
	}
	return Res;
}

int PrintJob::copies(const DDocProduct& _Product, const QFileInfo& _FileInfo) const
{
	int Res = 0;
	int Index = findPrintIndex(_Product, _FileInfo);
	if (Index != -1)
		Res = Prints[Index].numCopies();
	return Res;
}

QRect PrintJob::cropRect(const DDocProduct& _Product, const QFileInfo& _FileInfo) const
{
	QRect Res;
	int Index = findPrintIndex(_Product, _FileInfo);
	if (Index != -1)
		Res = Prints[Index].cropRect();

	return Res;
}


int PrintJob::totalCopies() const
{
	int Res;
	DDocPrintList Prints = prints();
	DDocPrintList::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		Res += it->numCopies();
	}
	return Res;
}


DDocPrintList PrintJob::prints() const
{
	DDocPrintList Res;
	DDocPrintList::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		if (it->numCopies() > 0)
			Res.push_back(*it);
	}
	return Res;
}

DDocPrintList PrintJob::prints(const DDocProduct& _Product) const
{
	DDocPrintList Res;
	DDocPrintList::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		if (it->product() == _Product && it->numCopies() > 0)
			Res.push_back(*it);
	}
	return Res;
}

DDocPrintList PrintJob::prints(const QFileInfo& _File) const
{
	DDocPrintList Res;
	TIndexList IndexList = indexList(_File);
	TIndexList::const_iterator it;
	for (it = IndexList.begin(); it != IndexList.end(); ++it)
	{
		Res.push_back(Prints[*it]);
	}
	return Res;
}

QFileInfoList PrintJob::files() const
{
	QFileInfoList Res;
	TDocPrintsMap::const_iterator it;
	for (it = DocPrintsMap.begin(); it != DocPrintsMap.end(); ++it)
	{
		Res.push_back(QFileInfo(it.key()));
	}
	return Res;
}

void PrintJob::addPrint(const DDocPrint& _Print)
{
	int NewIndex = Prints.size();
	Prints.push_back(_Print);
	DocPrintsMap[_Print.fileInfo().absoluteFilePath()].push_back(NewIndex);
}

void PrintJob::removePrintAt(int _Index)
{
	QFileInfo CurrentFileInfo = Prints[_Index].fileInfo();
	TIndexList& IndexList = DocPrintsMap[CurrentFileInfo.absoluteFilePath()];
	int IndexListIndex = IndexList.indexOf(_Index);
	if (IndexListIndex != -1)
	{
		IndexList.takeAt(IndexListIndex);
		if (IndexList.isEmpty())
			DocPrintsMap.remove(CurrentFileInfo.absoluteFilePath());
	}
	//Prints.removeAt(_Index);
	Prints[_Index].setNumCopies(0);
}

/*!
  Looks for a print with _Product and _FileInfo. If it finds it adds _Copies in the print else it adds a new DDocPrint.
 */
void PrintJob::addCopies(const DDocProduct& _Product, const QFileInfo& _FileInfo, int _Copies)
{
	int CurrCopies = copies(_Product, _FileInfo);
	setCopies(_Product, _FileInfo, qMax(CurrCopies + _Copies, 0));
}

/*!
	Looks for a DDocPrint with _Product and _FileInfo.
	If it finds it sets print numCopies to _Copies else it adds a new DDocPrint with _Copies numcopies.
*/
void PrintJob::setCopies(const DDocProduct& _Product, const QFileInfo& _FileInfo, int _Copies)
{
	int CurrentIndex = findPrintIndex(_Product, _FileInfo);
	if (CurrentIndex != -1)
	{
		if (_Copies > 0)
			Prints[CurrentIndex].setNumCopies(_Copies);
		else
			removePrintAt(CurrentIndex);
	}
	else
	{
		if (_Copies > 0)
			addPrint(DDocPrint(_FileInfo, _Product, _Copies));
	}
}

void PrintJob::setCropRect(const DDocProduct& _Product, const QFileInfo& _FileInfo, const QRect& _Rect)
{
	int CurrentIndex = findPrintIndex(_Product, _FileInfo);
	if (CurrentIndex != -1)
	{
		if (_Copies > 0)
			Prints[CurrentIndex].setCropRect(_Rect);
	}
}

void PrintJob::clear()
{
	Prints.clear();
	DocPrintsMap.clear();
}

void PrintJob::addOrderPrints(XmlOrder& _Order) const
{
	DDocPrintList::const_iterator it;
	for (it = Prints.begin(); it != Prints.end(); ++it)
	{
		if (it->numCopies() > 0)
		{
			DDocPrint CurrentPrint = *it;
			XmlOrderPrint Print(CurrentPrint.fileInfo().absoluteFilePath(), CurrentPrint.numCopies());
			Print.setBackPrintText(CurrentPrint.backPrintText());
			_Order.addPrint(CurrentPrint.product().ref(), Print);
		}
	}
}

bool PrintJob::isEmpty() const
{
	return DocPrintsMap.isEmpty();
}
