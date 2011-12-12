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

#ifndef RESOURCEMIMEDATA_H
#define RESOURCEMIMEDATA_H

#include <QMimeData>
#include "stphotobookexport.h"
#include "resource.h"


namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT ResourceMimeData : public QMimeData
{
	Q_OBJECT
	Resource MRes;

public:
	ResourceMimeData();
	Resource resource() const { return MRes; }
	void setResource(const Resource& _Value) { MRes = _Value; }
};
}
#endif // STMASKMIMEDATA_H
