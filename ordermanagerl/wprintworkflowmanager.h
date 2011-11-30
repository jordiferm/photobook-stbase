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

#ifndef WPRINTWORKFLOWMANAGER_H
#define WPRINTWORKFLOWMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QSqlTableModel>
#include <QMap>

#include "frecordwidget.h"
#include "stordermanagerlexport.h"


/**
ComboBox with an USER property that gets values from a specific column of his model.

@author Shadow
*/
class FRecDialog;
class ST_ORDERMANAGERL_EXPORT RefColComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY ( QVariant value READ value WRITE setValue USER true )

	int RefCol;

public:
	//! _RefCol is the model column from combo fetch data and gives data.
	RefColComboBox ( int _RefCol, QWidget* _Parent = 0 );
	QVariant value() const;
	void setValue ( const QVariant& _Value );
	void setModel ( QAbstractItemModel* _Model, const QString& _NullLabel, int _ModelColumn );
};



/**
Widget for printworkflows.

@author Shadow
*/
class FRecDialog;
class ST_ORDERMANAGERL_EXPORT WPrintWorkflowRecord : public FRecordWidget
{
	Q_OBJECT
	QWidget* newLookUpCombo ( const QString& _LookupTable, const QString& _LookupCol, const QString& _DisplayCol, const QString& _SourceCol );
	void createWidget();
public:
	WPrintWorkflowRecord ( FRecDialog* _Parent );
};



class ST_ORDERMANAGERL_EXPORT SqlLookupTableModel : public QSqlTableModel
{

public:
	class LookUp
	{
		QString LookUpTable;
		QString LookupCol;
		QString DisplayCol;

	public:
		LookUp ( const QString& _LookupTable = "", const QString& _LookupCol = "", const QString& _DisplayCol = "" ) :
				LookUpTable ( _LookupTable ), LookupCol ( _LookupCol ), DisplayCol ( _DisplayCol )
		{}
		QString lookupTable() const { return LookUpTable; }
		QString lookupCol() const { return LookupCol; }
		QString displayCol() const { return DisplayCol; }

	};

private:
	typedef QMap<QString, LookUp> TLookupMap;
	typedef QMap<QString, QIcon> TIconMap;
	TLookupMap Lookups;
	TIconMap Icons;
	QString NullLabel;

public:
	SqlLookupTableModel ( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() );
	virtual QVariant data ( const QModelIndex& _Item, int _Role = Qt::DisplayRole ) const;
	void addLookup ( const QString& _SourceField, const LookUp& _Lookup );
	void addIcon ( const QString& _FieldNAme, const QIcon& _Icon );
	void setNullLabel ( const QString& _Label ) { NullLabel = _Label; }
};

/**
	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class QSqlQueryModel;
class ST_ORDERMANAGERL_EXPORT WPrintWorkflowManager : public QWidget
{
	Q_OBJECT
	QSqlQueryModel* Model;

public:
	WPrintWorkflowManager ( QWidget* parent );
};



#endif
