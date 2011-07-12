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

#ifndef STTEMPLATEMIMEDATA_H
#define STTEMPLATEMIMEDATA_H

#include <QMimeData>
#include "stphotobookexport.h"
#include "templatescene.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT SceneMimeData : public QMimeData
{
	Q_OBJECT
	TemplateScene* Scene;

public:
	SceneMimeData();
	void setScene(TemplateScene* _Scene);
	TemplateScene* getScene() const;
};
}

#endif // STTEMPLATEMIMEDATA_H
