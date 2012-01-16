/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
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

#include "designinfolist.h"

using namespace SPhotoBook;

DesignInfoList::DesignInfoList()
{
}

int DesignInfoList::findDesignName(const QString& _DesignName)
{
	int Res = -1;
	bool Found = false;
	DesignInfoList::const_iterator it = begin();
	int Index = 0;
	while (!Found && it != end())
	{
		Found = it->name() == _DesignName;
		if (!Found)
			Index++;
		++it;
	}
	if (Found)
		Res = Index;

	return Res;
}

