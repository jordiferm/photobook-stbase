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

#ifndef FTABLEMANAGER_H
#define FTABLEMANAGER_H

#include <QWidget>
#include <QSqlRecord>
#include "stdbmanagexport.h"

/**
Gestió genèrica.
 
@author Shadow
*/
class FSqlRelationalTableModel;
class FSqlSearchWidget;
class FieldEditorContainer;
class FSqlActionTableView;
class QSqlError;
class QSqlTableModel;
class QSqlRelationalTableModel;
class FTSDialog;
class FRecDialog;
class FRecordWidget;
class FAction;
class ST_DBMANAG_EXPORT FTableManager : public QWidget
{
	Q_OBJECT

	FSqlSearchWidget* VarSearchWidget;
	FRecordWidget* MRecordWidget;
	FSqlActionTableView* VarView;
	QSqlRecord LastRemovedRow;
	QSqlTableModel* Model;
	FTSDialog* MRecDialog;
	FAction* EditAction;
	QString WidgetId;

	void loadSettings();
	void writeSettings();
	void init(QSqlTableModel* _Model, const QString& _Columns, bool _SortHeader);

public:
	FTableManager(QSqlTableModel* _Model, QWidget* _Parent = 0, const QString& _Columns = "", 
						bool _SortHeader = false);	
	FTableManager(QSqlRelationalTableModel* _Model, QWidget* _Parent = 0, const QString& _Columns = "", 
	bool _SortHeader = false);
	void setSearchEnabled(bool _Enabled);
	FSqlSearchWidget* searchWidget() const;
	FSqlActionTableView* actionTableView() const;
	
	void setRecordWidget(FRecordWidget* _RecordWidget);
	//! Default is true.
	void setAutoCenterRecordWidget(bool _AutoCenter);
	void setRecordWidget(FRecDialog* _RecordDialog);
	void updateLookups();
	//! Is the tableview editable ?
	void setEditable(bool _Editable);
	void setPrimeInsertRecord(const QSqlRecord& _Record);
	void editRow(int _Row);
	bool saveState() const { return !WidgetId.isEmpty(); }
	void enableSaveState(const QString& _WidgetId);
	void disableSaveState();

protected: 
	void showEvent(QShowEvent * event);			
	void hideEvent(QHideEvent * event);


	
protected slots:
	void selectFormRow(int _Index);
	void resizeColumns();
	void varViewError(const QSqlError& _Error);
	void beforeRemoveRow(int _Row, bool& _PerformOp);
	void afterRemoveRow();

signals: 
	void rowRemoved(const QSqlRecord& _RemovedRow);
};

#endif
