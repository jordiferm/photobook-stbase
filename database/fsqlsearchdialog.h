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

#ifndef FSQLSEARCHDIALOG_H
#define FSQLSEARCHDIALOG_H

#include "fsearchdialog.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include "stdatabaseexport.h"

/**
Dialeg de cerca "F3", que a partir d'una query o d'un model permet a l'usuari seleccionar un valor i el retorna en cas de que s'accepti la selecci�.

@author Shadow
*/
class FSqlSearchWidget;
class ST_DATABASE_EXPORT FSqlSearchDialog : public FSearchDialog
{
Q_OBJECT

public:
	FSqlSearchDialog(FSqlSearchWidget* _SqlSearchWidget, QWidget* _Parent = 0);
	static QSqlRecord exec(const QString& _Sql, QWidget* _Parent = 0, QSqlDatabase _Database = QSqlDatabase());
	static QSqlRecord exec(const QString& _TableName, const QString& _Columns = "", 
		const QString& _Filter = "", int _SortColumn = 0, QWidget* _Parent = 0, QSqlDatabase _Database = QSqlDatabase());
	static QSqlRecord exec(FSqlSearchDialog& _Dialog, const QString& _Columns, int _SortColumn);
};

#endif
