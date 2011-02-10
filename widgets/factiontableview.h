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

#ifndef FACTIONTABLEVIEW_H
#define FACTIONTABLEVIEW_H

#include <QWidget>

#include "stwidgetsexport.h"
#include "fabstractactioncontainer.h"

/**
Viewer amb un FSearchWidget que conte implementacions per defecte d'algunes accions standard de flam.
 
@author Shadow
*/
class FSearchWidget;
class QAbstractTableModel;
class QAbstractItemModel;
class SActionButtonsFrame;
class FAction;
class QModelIndex;
class QItemSelection;
class SGeometrySaveDialog;
class ST_WIDGETS_EXPORT FActionTableView : public QWidget, public FAbstractActionContainer
{
Q_OBJECT
	SActionButtonsFrame* MActionBBox;
	QAbstractItemModel* Model;
	
	void init(FSearchWidget* _SearchWidget);

public:
	enum EnState 
	{
		StateNone, 
		StateInserting, 
		StateEditing,
		StateDeleting
	};

protected:	
	SGeometrySaveDialog* RecDialog;
	FSearchWidget* SearchWidget;
	int EditColumn;
	FAction* AddAction; 
	FAction* EditAction; 
	FAction* DeleteAction; 
	EnState State;

public:
	FActionTableView(QAbstractTableModel* _Model, QWidget* _Parent = 0);
	FActionTableView(FSearchWidget* _SearchWidget, QWidget* _Parent = 0);
	virtual ~FActionTableView();
	void setRecordDialog(SGeometrySaveDialog* _RecDialog);
	void addAction(QAction* _Action);
	FSearchWidget* searchWidget() {return SearchWidget;}
	void setEditColumn(int _EditColumn) {EditColumn = _EditColumn;}
	//! Estat en que està la gestió actual. No te gaire sentit si no utilitzem formularis modals.
	EnState state() const { return State;}
	void removeAction(int _ActionId);
	
protected slots: 
	void slotSelectionChanged(const QItemSelection& _Selected, const QItemSelection& _Deselected);

public slots:
	virtual void actionTriggered();
	virtual void editCurrentRow(int _Index);
	//! Aquí index indica la fila on s'insertarà la nova fila.
	virtual void insertRow(int _Index);
	virtual void removeCurrentRow(int _Index);
	
signals: 
	//! Abans de cridar a insertRow()
	void beforeInsertRow(int _Index, bool& _PreformOp);
	//! S'emet just després de cridar insertRow();
	void afterInsertRow(int _Index); 
	//! Abans de cridar a edittRow()
	void beforeEditRow(int _Index, bool& _PreformOp); 
	//! Just després d'insertar una row al model sense ser editada ni validada.
	void rowInserted(int _Index);
	//! S'emet just després de cridar editRow(), si no hi ha RecDialog és equivalent a rowInserted();
	void afterEditRow(int _Index); 
	void beforeRemoveRow(int _Index, bool& _PreformOp); 
	//! S'emet just després de cridar removeRow();
	void afterRemoveRow(int _Index); 
	void selectionChanged(const QItemSelection& _Selected, const QItemSelection& _Deselected);
};

#endif
