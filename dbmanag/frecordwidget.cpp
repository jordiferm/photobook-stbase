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

#include "frecordwidget.h"
#include <QtGui>
#include <QtSql>
#include <QItemDelegate> 
#include <QStyleOptionViewItem>
#include <QSqlRelationalTableModel>
#include <QLineEdit>
#include <QRegExp> 
#include <QTableView>
#include "fsqlmodelviewutils.h"
#include "fsqlrelationaldelegate.h" 
#include "fpixmapselector.h" 
#include "fsqldatabasemanager.h"

#include "fsqlmetadata.h" 
#include "fdbutils.h"


// _________________________________________________________________________*/
//
//	Class FRecordWidgetDelegate.
// _________________________________________________________________________*/

FRecordWidgetDelegate::FRecordWidgetDelegate(QSqlTableModel* _Model ,QObject* _Parent) : QSqlRelationalDelegate(_Parent), Model(_Model)
{}

QWidget * FRecordWidgetDelegate::createEditor(QWidget* _Parent, const QStyleOptionViewItem& _Option, const QModelIndex& _Index) const
{
	QWidget* Editor = 0;
	//const QSqlTableModel* Model = qobject_cast<const QSqlTableModel*>(_Index.model());
	if (Model)
	{
		bool RelField = false;
		if (const QSqlRelationalTableModel* RelModel = qobject_cast<const QSqlRelationalTableModel*>(Model))
			RelField = RelModel->relationModel(_Index.column()) != 0;
		
// 		if (Model->record().field(_Index.column()).length() > 150 && Model->data(Model->index(0, _Index.column())).type() == QVariant::String && !RelField)
 		if (Model->record().field(_Index.column()).length() > 150 && !RelField)
		{
			FSTextEdit* FSEdit = new FSTextEdit(_Parent);
			FSEdit->setWordWrapMode(QTextOption::WordWrap);
			FSEdit->setTabChangesFocus(true);
			//FSEdit->setMaximumHeight(((Model->record().field(_Index.column()).length() % 100) + 3) * FSEdit->fontMetrics().height());
			Editor = FSEdit;
		}
		else 
		{
			if (FDBUtils::isSqlite(Model->database())) //SQLite has no booleans and itemDelegate is not able to
			{
				FTableMetaData TMetaData = FSqlDatabaseManager::manager().tableMetaData(Model->tableName());
				if (TMetaData.fields.contains(Model->record().fieldName(_Index.column())))
				{
					FFieldMetaData FMetaData = TMetaData.fields[Model->record().fieldName(_Index.column())]; 
					if (FMetaData.type() == QVariant::Bool) //Boolean types on SQLite.
						Editor = new QCheckBox(_Parent);
				}
			}	
		}
	}	
	
	if (!Editor)
	{
		Editor = QSqlRelationalDelegate::createEditor(_Parent, _Option, _Index);
	}
	
	if (Model)	
	{
		if (QLineEdit* LEdit = qobject_cast<QLineEdit*>(Editor))
			LEdit->setMaxLength(Model->record().field(_Index.column()).length());
	}	
	return Editor;
	
}
// _________________________________________________________________________*/
//
//	Class FRecordWidget	
// _________________________________________________________________________*/

/*!
	By default it uses QSqlRelationalDelegate to be usable on SqlRelational models.
*/
FRecordWidget::FRecordWidget(QSqlTableModel* _Model, QWidget* _Parent)
		: QWidget(_Parent), Model(_Model)
{
	Mapper = new QDataWidgetMapper(this);
	Mapper->setModel(_Model);
	//Mapper->setItemDelegate(new FSqlRelationalDelegate(this)); //Does not work.
	Mapper->setItemDelegate(new FRecordWidgetDelegate(_Model, this));
	WidgetCreated = false;
}

void FRecordWidget::setSuffix(QWidget* _Editor, const QString& _Suffix)
{
	if (_Editor->inherits("QSpinBox") || _Editor->inherits("QDoubleSpinBox"))
		static_cast<QSpinBox*>(_Editor)->setSuffix(" " + _Suffix);
	else 
		qDebug(QString("Could not set a suffix to %1").arg(_Editor->metaObject()->className()).toLatin1());
}

QTableView* FRecordWidget::setAdvancedView(const QString& _FieldName, const QString& _ViewTable, const QString& _VisibleFields)
{
        QTableView* Res = 0;
        if (QComboBox* CBox = qobject_cast<QComboBox*>(fieldEditor(_FieldName)))
        {
                CBox->setMaxVisibleItems(25);
                QTableView* MView = new QTableView;
                QSqlTableModel* TModel = qobject_cast<QSqlTableModel*>(CBox->model());
                if (TModel)
                        FSqlDatabaseManager::manager().tableMetaData(_ViewTable).configureHeaderData(*TModel);
                MView->verticalHeader()->setVisible(false);
                //MView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
                //MView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
                CBox->setView(MView);
                FSqlModelViewUtils::setColumns(MView, _VisibleFields);
                FSqlModelViewUtils::sortColumns(MView, _VisibleFields);

                QFont VFont = MView->font();
                VFont.setPointSize(VFont.pointSize() - 1);
                MView->setFont(VFont);
                VFont.setBold(true);
                MView->horizontalHeader()->setFont(VFont);

                MView->setSelectionBehavior(QAbstractItemView::SelectRows);
                MView->setAlternatingRowColors(true);
                MView->setSortingEnabled(true);
                MView->sortByColumn(1, Qt::AscendingOrder);
                MView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
                MView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
                Res = MView;
        }
        return Res;
}

/*!
	This is an overloaded member function, provided for convenience.
*/

void FRecordWidget::setSuffix(const QString& _FieldName, const QString& _Suffix)
{
	QWidget* Editor = fieldEditor(_FieldName);
	if (Editor)
		setSuffix(Editor, _Suffix);
}

/*!
	Creates a group of editors and editorlabels inside a container. 
	\param _Fields Coma separated field names.
	\return A widget that contains the editors.
*/
QWidget* FRecordWidget::createEditorsGroup(const QString& _Fields, QWidget* _Container)
{
	QGridLayout* ContLayout = new QGridLayout(_Container);
	QStringList FieldList = _Fields.split(",");
	QStringList::iterator it; 
	for (it = FieldList.begin(); it != FieldList.end(); ++it )
	{
		ContLayout->addWidget(createHeaderLabel((*it).trimmed()), ContLayout->rowCount(), 0);
		ContLayout->addWidget(createEditor((*it).trimmed()), ContLayout->rowCount() -1, 1);
	}
	return _Container;
}

/*!
	Creates a group of editors and editor labels only for the current model image. 
	This function needs a SFSqlRelationalImageModel with image indexes added.
	\param _Container A container to put the editors.
	\return A widget that contains the editors.
*/

QWidget* FRecordWidget::createImageEditors(QWidget* _Container)
{
	if (FSqlRelationalImageModel* ImgModel = qobject_cast<FSqlRelationalImageModel*>(Model))
	{
		QGridLayout* ContLayout = new QGridLayout(_Container);
		createImageEditors(ContLayout, ImgModel->imageIndexs());
	}
	return _Container;
}

void FRecordWidget::createImageEditors(QGridLayout* _ContLayout, const FSqlRelationalImageModel::TImageIndexList& _ImageIndexs, int _NumCols)
{
	FSqlRelationalImageModel::TImageIndexList::const_iterator it; 
	for (it = _ImageIndexs.begin(); it != _ImageIndexs.end(); ++it)
	{
		QWidget* Editor = new FPixmapSelector(this);
		Mapper->addMapping(Editor, *it);
		int NCols = _NumCols * 2;
		_ContLayout->addWidget(createHeaderLabel(*it), _ContLayout->count() / NCols, _ContLayout->count() % NCols);
		_ContLayout->addWidget(Editor, _ContLayout->count() / NCols, _ContLayout->count() % NCols);
	}
}

QWidget* FRecordWidget::createImageEditors(QWidget* _Container, const QRegExp& _PrefixMatch, int _NumCols)
{
	if (FSqlRelationalImageModel* ImgModel = qobject_cast<FSqlRelationalImageModel*>(Model))
	{
		QGridLayout* ContLayout = new QGridLayout(_Container);
		createImageEditors(ContLayout, ImgModel->imageIndexs(_PrefixMatch), _NumCols);
	}
	return _Container;
}


/*!
	\return returns the mapped widget for field _FieldName.
	or NULL if it not exists.
*/

QWidget* FRecordWidget::fieldEditor(const QString& _FieldName)
{
	QWidget* Res = 0;	
	int Index = FSqlModelViewUtils::indexOf(Model,_FieldName);
	if (MappedSections.contains(Index))
		Res = MappedSections[Index];
	return Res;
}


/*!
	This is an overloaded member function, provided for convenience.
*/

QWidget* FRecordWidget::createEditor(int _Index, const QString& _Suffix, bool _ReadOnly)
{
	QStyleOptionViewItem Options;
	QWidget* Editor = Mapper->itemDelegate()->createEditor(this, Options, Model->index(0, _Index));
	if (!Editor)
		Editor = new QLineEdit(this);

	if (MappedSections.size() == 0)
		setFocusProxy(Editor);

	Mapper->addMapping(Editor, _Index);
	MappedSections.insert(_Index, Editor);
		
	if (!_Suffix.isEmpty())
		setSuffix(Editor, _Suffix);

	if (_ReadOnly)
	{
		if (QLineEdit* LEditEditor = qobject_cast<QLineEdit*>(Editor))
			LEditEditor->setReadOnly(true);
		else
			Editor->setEnabled(false);
	}
	
	return Editor;
}

/*!
	Creates the editor related to field _FieldName in current model and mapps it with current QDataWidgetMapper.
	To create the editor this function uses current QDataWidgetMapper itemDelegate.
*/

QWidget* FRecordWidget::createEditor(const QString& _FieldName, const QString& _Suffix, bool _ReadOnly)
{
	return createEditor(FSqlModelViewUtils::indexOf(Model,_FieldName), _Suffix, _ReadOnly);
}

QString FRecordWidget::labelForField(int _Index)
{
	return  Model->headerData(_Index, Qt::Horizontal).toString() + " :";
}

QString FRecordWidget::labelForField(const QString& _FieldName)
{
	return labelForField(FSqlModelViewUtils::indexOf(Model,_FieldName));
}

QLabel* FRecordWidget::createHeaderLabel(int _Index)
{
	QLabel* Res = new QLabel(labelForField(_Index), this);
	QFont Font = Res->font();
	Font.setBold(true);
	Res->setFont(Font);
	return Res;
}

QLabel* FRecordWidget::createHeaderLabel(const QString& _FieldName)
{
	return createHeaderLabel(FSqlModelViewUtils::indexOf(Model,_FieldName));
}

QDataWidgetMapper* FRecordWidget::mapper() const
{
	return Mapper;
}

void FRecordWidget::showEvent(QShowEvent* )
{
	if (!WidgetCreated)	
	{
		int CIndex = Mapper->currentIndex();
		createWidget();
 		WidgetCreated = true;
	 	Mapper->setCurrentIndex(CIndex);
	}
}

//!!!!! Això ho haurem de mirar millor, per fer-ho + genèric !!!!!
void FRecordWidget::clearEditors()
{
	QList<QLineEdit*> LineEditList = findChildren<QLineEdit*>();
	QList<QLineEdit*>::iterator it; 
	for (it = LineEditList.begin(); it != LineEditList.end(); ++it)
	{
		(*it)->setText("");
	}

	QList<QTextEdit*> TextEditList = findChildren<QTextEdit*>();
	QList<QTextEdit*>::iterator tit; 
	for (tit = TextEditList.begin(); tit != TextEditList.end(); ++tit)
	{
		(*tit)->clear();
	}	
}

// void FRecordWidget::addNew()
// {
// 	Model->insertRows(0,1);
// 	Mapper->toFirst();
// 	clearEditors();
//  	QWidget* FProxy = focusProxy();
//  	if (FProxy)
//  		FProxy->setFocus();
// 	
// }

