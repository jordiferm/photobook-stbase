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

#ifndef FRECDIALOG_H
#define FRECDIALOG_H

#include "ftsdialog.h"
#include "stdbmanagexport.h"

/**
Widget base per les fitxes de registres.
Porta un control dels canvis de les taules relacionades.
 
@author Shadow
*/
class FieldEditorContainer;
class QSqlTableModel;
class FTableManager;
class ST_DBMANAG_EXPORT FRecDialog : public FTSDialog
{
Q_OBJECT 

	FieldEditorContainer* Container;
	QSqlTableModel* Model;
	bool InsertedRowSaved;

protected:
	FTableManager* ParentTManager;

public:
	FRecDialog(QSqlTableModel* _Model, FTableManager* _Parent = 0);
	//! Nou FTableManager a tenir en compte.
	void addTableManager(FTableManager* _TManager);
	
	//! Retorna l'etiqueta a partir del nom del camp.
	QVariant headerData(const QString& _Column);
	//! Retorna el valor del Model a la currentRow i la columna _Column.
	QVariant currentRowValue(const QString& _Column);
	QSqlTableModel* model() const { return Model; }

protected:
	void hideEvent( QHideEvent * _Event );

private slots:
	void beforeInsertParentRow(int _Row, bool& _PerformOp);
	

public slots:
	virtual void disableCancel();
	virtual void beforeInsertRelatedRow(int _Row, bool& _PerformOp);

};

#endif
