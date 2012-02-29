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

#ifndef FSQLSEARCHWIDGET_H
#define FSQLSEARCHWIDGET_H

#include "fsearchwidget.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include "stdatabaseexport.h"


/**
Dialeg de cerca "F3", que a partir d'una query o d'un model permet a l'usuari seleccionar un valor i el retorna en cas de que s'accepti la selecci�.

@author Shadow
*/
class QTableView;
class QSqlTableModel;
class QStringList;
class ST_DATABASE_EXPORT FSqlSearchWidget : public FSearchWidget
{
Q_OBJECT

public:
	enum EnSearchMode 
	{
		SelectionSearchMode, 
		FilterSearchMode
	};

	QSqlTableModel* Model;
	bool ColumnToContentSize;
	EnSearchMode SearchMode;
	QString CurrFilter;
	int CurrentSearchColumn;
	
protected: 
	QAbstractItemView* createView();
	QSqlTableModel* createModel(const QString& _TableName, const QString& _Filter = "", QObject* _Parent = 0, 
										 QSqlDatabase _Database = QSqlDatabase());
	void init();


public:
	FSqlSearchWidget(const QString& _TableName, const QString& _Filter, QWidget* _Parent = 0, QSqlDatabase _Database = QSqlDatabase());
	FSqlSearchWidget(QTableView* _View, const QString& _TableName, const QString& _Filter, QWidget* _Parent = 0, QSqlDatabase _Database = QSqlDatabase());
	FSqlSearchWidget(QSqlTableModel* _Model, QWidget* _Parent = 0);
	FSqlSearchWidget(QTableView* _View, QSqlTableModel* _Model, QWidget* _Parent = 0);
	~FSqlSearchWidget();
	
	void setColumns(const QStringList& _Cols, bool _Sort = true);
	void setColumns(const QString& _Cols, bool _Sort = true);
	void select();
	void setSearchColumn(int _Column);
	void setColumnToContentSize(bool _Active);
	bool columnToContentSize() const;
	void setSearchMode(EnSearchMode _SearchMode);
	EnSearchMode searchMode() const;
	//! \return QSqlRecord with the currentIndex record or an Empty QSqlRecord if there is no current row in tableView.
	QSqlRecord currentRecord() const;
	
public slots: 
	void slotSetSearchColumn(int _Column);
	void search(const QString& _SearchText);
	void nextSearchColumn();
	void previousSearchColumn();

};


#endif
