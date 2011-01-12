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

#ifndef FRECORDWIDGET_H
#define FRECORDWIDGET_H

#include <QWidget>
#include <QMap> 
#include <QSqlRecord>
#include <QTextEdit> 
#include <QSqlRelationalDelegate>
#include "stdbmanagexport.h"


class ST_DBMANAG_EXPORT FRecordWidgetDelegate : public QSqlRelationalDelegate
{
Q_OBJECT
	QSqlTableModel* Model;
public:
	FRecordWidgetDelegate(QSqlTableModel* _Model, QObject* _Parent = 0);
	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class ST_DBMANAG_EXPORT FSTextEdit : public QTextEdit
{
Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText USER true)
public:
	FSTextEdit(QWidget* _Parent = 0) : QTextEdit(_Parent) {}
	QString text() const { return toPlainText(); }
	void setText(const QString& _Text) { setPlainText(_Text); }
};

/**
Classe base per Widgets amb records.
 
@author Shadow
*/
class QDataWidgetMapper;
class QSqlTableModel;
class QLabel;
class QAbstractItemDelegate;
class QShowEvent;
class QGridLayout;
class QRegExp;
class QTableView;
class ST_DBMANAG_EXPORT FRecordWidget : public QWidget
{
	Q_OBJECT

protected:
	QDataWidgetMapper* Mapper;
	QSqlTableModel* Model;
	bool WidgetCreated;
	QMap<int, QWidget*>	MappedSections; //mappedWidgetAt fails !!!!(Solution scheduled for qt 4.2.2)
	QSqlRecord PrimeInsertRecord;
		
public:
	FRecordWidget(QSqlTableModel* _Model, QWidget* _Parent = 0);
	void setDelegate(QAbstractItemDelegate* _Delegate);
	QTableView* setAdvancedView(const QString& _FieldName, const QString& _ViewTable, const QString& _VisibleFields);
	void setSuffix(QWidget* _Editor, const QString& _Suffix);
	void setSuffix(const QString& _FieldName, const QString& _Suffix);
	QWidget* createEditorsGroup(const QString& _Fields, QWidget* _Container);


	QWidget* fieldEditor(const QString& _FieldName);
	QWidget* createEditor(int _Index, const QString& _Suffix, bool _ReadOnly = false);
	QWidget* createEditor(const QString& _FieldName, const QString& _Suffix = "", bool _ReadOnly = false);
	QString labelForField(int _Index);
	QString labelForField(const QString& _FieldName);
	QLabel* createHeaderLabel(int _Index);
	QLabel* createHeaderLabel(const QString& _FieldName);
	QDataWidgetMapper* mapper() const;
	void setPrimeInsertRecord(const QSqlRecord& _Record) { PrimeInsertRecord = _Record; }
	QSqlRecord currentRecord() const;

protected: 
	//! Used to create the widget. This has to be created after primeInsert if the model is empty.
	virtual void createWidget() = 0;
	void showEvent(QShowEvent* );

public slots: 
	virtual void clearEditors();
	//void addNew();
};

#endif
