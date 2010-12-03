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

#ifndef DDOCLIST_H
#define DDOCLIST_H

#include "stdomexport.h"
#include <QList>
#include <QFileInfoList>

namespace STDom
{
/*!
List of DDocs.
It is the owner of docs and deletes them in destructor.

@author Shadow
*/

class DDoc;
class ST_DOM_EXPORT DDocList
{
public:
	typedef QList<DDoc*> TDDocList;

private:
	TDDocList Docs;

public:
    DDocList();
	~DDocList();
	void addDoc(DDoc* _Doc);
	void addDocs(const QFileInfoList& _FileList);
	void insertDoc(int _Index, DDoc* _Doc);
	void setDoc(int _Index, DDoc* _Doc);
	void removeAt(int _Index);
	TDDocList dDocs() const;
	void clear();
	int size() const;
	//! O(n)
	bool contains(const DDoc* _Doc) const;
	//! O(n)
	int indexOf(const DDoc* _Doc) const;
};
}//Namespace
#endif // DDOCLIST_H
