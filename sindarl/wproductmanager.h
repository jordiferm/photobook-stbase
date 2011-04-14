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

#ifndef WPRODUCTMANAGER_H
#define WPRODUCTMANAGER_H

#include "fgenmanager.h"
#include "frecordwidget.h"
#include "stsindarlexport.h"

/**
FRecordWidget for commands.
 
@author Shadow
*/
class FRecDialog;
class QToolButton;
class ST_SINDARL_EXPORT WProductRecord : public FRecordWidget
{
Q_OBJECT
	QToolButton* TBFormatManager;
	void createWidget();

public:
	WProductRecord(FRecDialog* _Parent);
	virtual void setInserting(bool _Value);

signals:
	void openFormatManager();
};

/**
Format management

	@author Jordi Fernandez <jordif@starblitz-k.com>
*/
class ST_SINDARL_EXPORT WFormatManager : public FGenManager
{
	Q_OBJECT
	bool LocalAdded;

public:
	WFormatManager(QWidget* _Parent = 0, const QSqlDatabase& _Database = QSqlDatabase::database());
	bool localAdded() const { return LocalAdded; }
	//! Means that products added from this manager are not published.
	void setLocalAdded(bool _Value) { LocalAdded = _Value; }

protected slots:
	void primeInsert(int /*_Row*/, QSqlRecord& _Record);
	void beforeRemoveRow(int _Index, bool& _PerformOp);
};


/**
Product management

	@author Jordi Fernandez <shadow@softwarelliure.com>
*/
class FTSDialog;
class ST_SINDARL_EXPORT WProductManager : public FGenManager
{
	Q_OBJECT
	bool LocalAdded;
	WFormatManager* FormatManager;
	FTSDialog* FormatsDialog;
	WProductRecord* MRecWidget;

public:
	WProductManager(QWidget* _Parent = 0, const QSqlDatabase& _Database = QSqlDatabase::database());
	bool localAdded() const { return LocalAdded; }
	void setLocalAdded(bool _Value);

protected slots:
	void beforeRemoveRow(int , bool& );
	void primeInsert(int /*_Row*/, QSqlRecord& _Record);
	void slotOpenFormatManager();
};

#endif
