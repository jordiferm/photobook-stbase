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

#ifndef ABSTRACTIMAGEEFFECTMANAGER_H
#define ABSTRACTIMAGEEFFECTMANAGER_H

#include <QObject>
#include "stprnlibexport.h"

/**
	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class AbstractImageEffectModel; 
class QImage;
class IMImage;
class ST_PRNLIB_EXPORT AbstractImageEffectManager : public QObject
{
	Q_OBJECT
protected:
	AbstractImageEffectModel* Model;

public:
	AbstractImageEffectManager(AbstractImageEffectModel* _Model, QObject* _Parent);
	virtual bool anyEffectToApply() const = 0;
	virtual void applyEffects(IMImage& _SourceImage) = 0;
};

#endif
