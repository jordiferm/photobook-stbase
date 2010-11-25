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

#ifndef FSQLACTIONTABLEVIEW_H
#define FSQLACTIONTABLEVIEW_H

#include "factiontableview.h"
#include <QSqlTableModel>
#include "stdatabaseexport.h"


/**
ActionTableView per a Models Sql.
A diferència de la seva classe base provoca submits al model.
@author Shadow
*/
class QSqlRecord;
class FSqlSearchWidget;
class QSqlError;
class ST_DATABASE_EXPORT FSqlActionTableView : public FActionTableView
{
Q_OBJECT

	QSqlTableModel* Model;
	QSqlTableModel::EditStrategy LastStrategy;
	bool Inserting;
	
	void init();
	
public:
	FSqlActionTableView(QSqlTableModel* _Model, QWidget* _Parent);
	FSqlActionTableView(FSqlSearchWidget* _SearchWidget, QWidget* _Parent);

	bool execDialog();
	virtual void insertRow(int _Index);
	virtual void editCurrentRow(int _Index);
	virtual void removeCurrentRow(int _Index);
	bool commit();
	bool isInserting() const;

protected: 
	void hideEvent( QHideEvent * _Event );

public slots:
	void commitInsert();
signals: 
	void submitError(const QSqlError&);
};

#endif
