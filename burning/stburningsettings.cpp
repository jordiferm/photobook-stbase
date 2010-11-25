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
#include "stburningsettings.h"

STBurningSettings::STBurningSettings(QObject* _Parent): SAppSettings(_Parent)
{

	if (!keySaved("stburning/enabled"))
	{
		addKey("stburning/enabled", true, QVariant::Bool,
			tr("Burning enabled ?"), tr(""));

		addKey("stburning/mkisofsbinary", "/usr/bin/mkisofs", QVariant::String,
			tr("Complete path of the mkisofs.."), tr("Binari to create iso images."));

		addKey("stburning/tmpisofile", "/tmp/kpstmp.iso", QVariant::String, 
			tr("Complete path of the  temporary image file."), tr("Temporary file for the CD images created with mkisofs."));

		addKey("stburning/cdrecordbinary", "/usr/bin/cdrecord", QVariant::String, 
			tr("Complete path of the cdrecord binari."), tr("Path of the cdrecord."));

		addKey("stburning/device", "/dev/cdrom", QVariant::String, 
			tr("Burning unit."), tr("Burning unit."));

		addKey("stburning/speed", 48, QVariant::Int, 
			tr("Burning speed"), tr("Burning speed"));

		addKey("stburning/simulationmode", false, QVariant::Bool, 
			tr("Simulation mode?"), tr("Simulation mode"));

		addKey("stburning/writedebuginfo", false, QVariant::Bool, 
			tr("write debug info ?"), tr("Console debug."));
	}
}

bool STBurningSettings::burningEnabled() const
{
	return value("stburning/enabled").toBool();
}

QString STBurningSettings::mkIsoFsBinary() const
{
	return value("stburning/mkisofsbinary").toString();
}

QString STBurningSettings::tmpIsoFile() const
{
	return value("stburning/tmpisofile").toString();
}

QString STBurningSettings::cdRecordBinary() const
{
	return value("stburning/cdrecordbinary").toString();
}

QString STBurningSettings::device() const
{
	return value("stburning/device").toString();
}

int STBurningSettings::speed() const
{
	return value("stburning/speed").toInt();
}

bool STBurningSettings::simulationMode() const
{
	return value("stburning/simulationmode").toBool();
}

bool STBurningSettings::writeDebugInfo() const
{
	return value("stburning/writedebuginfo").toBool();
}

