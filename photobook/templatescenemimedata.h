/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TEMPLATESCENEMIMEDATA_H
#define TEMPLATESCENEMIMEDATA_H

#include <QMimeData>
#include "stphotobookexport.h"
#include "templatescene.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT TemplateSceneMimeData : public QMimeData
{
	Q_OBJECT
	TemplateScene* TScene;

public:
	TemplateSceneMimeData();
	void setTemplate(TemplateScene* _TScene) { TScene = _TScene; }
	TemplateScene* getTemplateScene() const { return TScene; }
};
}
#endif // STTEMPLATEMIMEDATA_H
