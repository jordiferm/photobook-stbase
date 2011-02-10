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

#ifndef SGEOMETRYSAVEDIALOG_H
#define SGEOMETRYSAVEDIALOG_H

#include <QDialog>
#include <QString>
#include "stwidgetsexport.h"

class ST_WIDGETS_EXPORT SGeometrySaveDialog : public QDialog
{
	Q_OBJECT

	QString WidgetId;
	void loadSettings();
	void writeSettings();

public:
	explicit SGeometrySaveDialog(QWidget *parent = 0, Qt::WindowFlags f = 0 );
	bool saveState() const { return !WidgetId.isEmpty(); }
	void enableSaveState(const QString& _WidgetId);
	void disableSaveState();

protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);

signals:

public slots:

};

#endif // SGEOMETRYSAVEDIALOG_H
