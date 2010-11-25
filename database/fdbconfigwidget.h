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

#ifndef FDBCONFIGWIDGET_H
#define FDBCONFIGWIDGET_H

#include <QWidget>
#include "stdatabaseexport.h"
#include "smappedwidget.h"

/**
Widget per a la configuració d'una base de dades.
 
@author Shadow
*/
class FSqlDatabaseSettings;
class ST_DATABASE_EXPORT FSingleDBConfigWidget : public SMappedWidget
{
	Q_OBJECT

	QString ConnectionName;
public:
	FSingleDBConfigWidget(FSqlDatabaseSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent);
	virtual QSize sizeHint () const;

private slots:
	void slotBackupDb(); 
	void slotRestoreDb();
};

/**
Widget per a la configuració de n base de dades. (Segons Settings)
 
@author Shadow
*/
class QTabWidget;
class ST_DATABASE_EXPORT FDBConfigWidget : public QWidget
{
	Q_OBJECT

	QTabWidget* TWidget;

public:
	FDBConfigWidget(QDataWidgetMapper* _Mapper, QWidget* _Parent);
};

#endif
