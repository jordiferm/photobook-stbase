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

#ifndef IMAGEEFFECTMODEL_H
#define IMAGEEFFECTMODEL_H

#include "abstractimageeffectmodel.h"
#include "stprnlibexport.h"
/**
A model to save ImageEffect Widget settings.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class QSettings;
class ST_PRNLIB_EXPORT ImageEffectModel : public AbstractImageEffectModel
{
	Q_OBJECT

private:
	QSettings& Settings;

public:
	ImageEffectModel(QSettings& _Settings, QObject* _Parent = 0);
	virtual void setValue(const QString& _Key, const QVariant& _Value);
	virtual QVariant value(const QString& _Key, const QVariant& _DefaultValue = QVariant());
};

#endif
