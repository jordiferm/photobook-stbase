/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "sgeometrysavedialog.h"
#include <QSettings>
#include <QShowEvent>
#include <QHideEvent>
#include <QDebug>

void SGeometrySaveDialog::loadSettings()
{
	QSettings Settings;
	restoreGeometry(Settings.value(QString("%1/sgsdgeometry").arg(WidgetId)).toByteArray());
}

void SGeometrySaveDialog::writeSettings()
{
	QSettings Settings;
	Settings.setValue(QString("%1/sgsdgeometry").arg(WidgetId),  saveGeometry());
}


SGeometrySaveDialog::SGeometrySaveDialog(QWidget *parent, Qt::WindowFlags f) :
	QDialog(parent, f), WidgetId("")
{
}

void SGeometrySaveDialog::enableSaveState(const QString& _WidgetId)
{
	WidgetId = _WidgetId;
}

void SGeometrySaveDialog::disableSaveState()
{
	WidgetId = "";
}

void SGeometrySaveDialog::showEvent(QShowEvent* /*event */)
{
	if (saveState())
		loadSettings();
}

void SGeometrySaveDialog::hideEvent(QHideEvent * event)
{
	if (saveState())
	{
		 writeSettings();
		 event->accept();
	} else {
		event->ignore();
	}
 }
