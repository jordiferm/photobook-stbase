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

#ifndef FSQLMODELVIEWUTILS_H
#define FSQLMODELVIEWUTILS_H

#include <QString>
#include "stdatabaseexport.h"

/**
Utils per a models i viewers de Bases de dades.

@author Shadow
*/


class QSqlTableModel;
class QStringList; 
class QString;
class QTableView;
class QSqlRelationalTableModel;

class FActionWidget;
class QWidget;
class FSqlFieldEditor;
class ST_DATABASE_EXPORT FSqlModelViewUtils
{
public:
	//! S'han de cridar abans que select.
	//! Amb Qt<=4.0.1 No funcionen per els models Relational.
	static void setColumns(QSqlTableModel* _Model, const QString& _Cols);
	static void setColumns(QSqlTableModel* _Model, const QStringList& _Cols);
	static void setColumns(QTableView* _View, const QStringList& _Cols);
	static void setColumns(QTableView* _View, const QString& _Cols);
	static void sortColumns(QTableView* _View, const QStringList& _Cols);
	static void sortColumns(QTableView* _View, const QString& _Cols);
	static int indexOf(const QSqlTableModel* _Model, const QString& _FieldName);
	static int indexOf(const QSqlRelationalTableModel* _Model, const QString& _FieldName);
	static int indexOf(const QSqlRelationalTableModel* _Model, const QString& _TableName, 
		const QString& _FieldName);
	//! Afegeix una SearchAction a un _ActionWidget a partir del _TableName.
	static void addSearchAction(FActionWidget* _ActionWidget, const QString& _TableName, const QString&
	 	_IndexCol,	bool _TouchScreen = false, const QString& _Columns = "", const QString& _Filter = "", 
		int _SortColumn = 0, QWidget* _Parent = 0);
	//! Afegeix una SearchAction a partir d'una query.
	static void addSearchAction(FActionWidget* _ActionWidget, const QString& _Sql, int _IndexCol, 
		bool _TouchScreen = false, QWidget* _Parent = 0 );
	//! Un fieldEditor ja sap quin indexfield ha de rebre
	static void addSearchAction(FSqlFieldEditor* _ActionWidget, const QString& _TableName,
	 	bool _TouchScreen = false, const QString& _Columns = "", const QString& _Filter = "", 
		int _SortColumn = 0, QWidget* _Parent = 0);
	
};
	
#endif
