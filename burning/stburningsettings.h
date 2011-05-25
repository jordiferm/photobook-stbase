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
#ifndef STBURNINGSETTINGS_H
#define STBURNINGSETTINGS_H

#include "sappsettings.h"
#include "stburningexport.h"

/**
Settings class for CDBurning.

	@author
*/
class ST_BURNING_EXPORT STBurningSettings : public SAppSettings
{

	Q_OBJECT
public:
	STBurningSettings(QObject* _Parent = 0);

	bool burningEnabled() const;
	QString mkIsoFsBinary() const;
	QString tmpIsoFile() const;
	QString cdRecordBinary() const;
	QString device() const; 
	int speed() const;
	bool simulationMode() const;
	bool multiSession() const;
	bool writeDebugInfo() const;
};

#endif
