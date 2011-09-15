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

#include "ddoclist.h"
#include <QFileInfoList>
#include "ddocfactory.h"
#include "ddoc.h"

using namespace STDom;

DDocList::DDocList()
{}

DDocList::~DDocList()
{
	clear();
}

void DDocList::addDoc(DDoc* _Doc)
{
	Docs.push_back(_Doc);
}

void DDocList::insertDoc(int _Index, DDoc* _Doc)
{
	Docs.insert(_Index, _Doc);
}

//! Replaces existing doc on index _Index and deletes replaced DDoc. If index isn't valid it does nothing.
void DDocList::setDoc(int _Index, DDoc* _Doc)
{
	if (_Index >= 0 && _Index < Docs.size())
	{
		delete Docs[_Index];
		Docs[_Index] = _Doc;
	}
}

void DDocList::removeAt(int _Index)
{
	if (_Index >= 0 && _Index < Docs.size())
	{
		delete Docs[_Index];
		Docs.removeAt(_Index);
	}
}

void DDocList::addDocs(const QFileInfoList& _FileList)
{
	QFileInfoList::const_iterator it;
	for (it = _FileList.begin(); it != _FileList.end(); ++it)
	{
		addDoc(DDocFactory::newDoc(*it));
	}
}

DDocList::TDDocList DDocList::dDocs() const
{
	return Docs;
}

/*!
	Clears the list and deletes all DDocs that contains.
*/
void DDocList::clear()
{
	TDDocList::iterator it;
	for (it = Docs.begin(); it!=Docs.end(); ++it)
	{
		delete (*it);
	}
	Docs.clear();
}

int DDocList::size() const
{
	return Docs.size();
}

bool DDocList::contains(const DDoc* _Doc) const
{
	bool Found = false;
	TDDocList::const_iterator it = Docs.begin();
	while (it != Docs.end() && !Found)
	{
		Found = (*(*it) == (*_Doc));
		++it;
	}

	return Found;
}

int DDocList::indexOf(const DDoc* _Doc) const
{
	bool Found = false;
	int Row = 0;
	while (Row < Docs.size() && !Found)
	{
		Found = (* Docs[Row] == *_Doc);
		if (!Found)
			Row++;
	}
	int Res = Row;
	if (!Found)
		Res = -1;
	return Res;
}

