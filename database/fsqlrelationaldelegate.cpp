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

#include "fsqlrelationaldelegate.h"
#include <QSqlField>
#include <QSqlRecord>
#include <QSpinBox>
#include <QLineEdit>
#include <QSqlRelationalDelegate>

FSqlRelationalDelegate::FSqlRelationalDelegate(QObject* _Parent)
		: QItemDelegate(_Parent)
{}

QWidget* FSqlRelationalDelegate::createEditor(QWidget* _Parent,
        const QStyleOptionViewItem& _Option,
        const QModelIndex& _Index) const
{
	QWidget* Res;
	const QSqlRelationalTableModel *SqlRelModel = qobject_cast<const QSqlRelationalTableModel *>(_Index.model());
	QSqlTableModel *childModel = SqlRelModel ? SqlRelModel->relationModel(_Index.column()) : 0;
	if (!childModel)
	{
		Res = QItemDelegate::createEditor(_Parent, _Option, _Index);
	}
	else
	{
		QComboBox *combo = new QComboBox(_Parent);
		connect(combo, SIGNAL(activated(int)), this, SLOT(editorCommitData()));
		//connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(editorCommitData()));
		combo->setModel(childModel);
		combo->setModelColumn(childModel->fieldIndex(SqlRelModel->relation(_Index.column()).displayColumn()));
		combo->installEventFilter(const_cast<FSqlRelationalDelegate *>(this));
		Res = combo;
	}



	const QSqlTableModel* SqlModel = qobject_cast<const QSqlTableModel*>(_Index.model());
	//Només per models Sql.
	if (SqlModel && Res)
	{
		QSqlField CField = SqlModel->record().field(_Index.column());
		//Mides segons el tipus de widget:
		if (Res->inherits("QSpinBox"))
		{
			QSpinBox* SpBox = static_cast<QSpinBox*>(Res);
			//SpBox->setMaximum(static_cast<int>(pow(10, CField.length())) -1);
			SpBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
			SpBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
		}
		if (Res->inherits("QLineEdit"))
		{
			QLineEdit* LEdit = static_cast<QLineEdit*>(Res);
			LEdit->setMaxLength(CField.length());
		}

	}
	return Res;

}

void FSqlRelationalDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
/*	QSqlRelationalDelegate* MDelegate = new QSqlRelationalDelegate;
	MDelegate->setEditorData(editor, index);
	delete MDelegate;*/

	if (!index.isValid())
		return;

	const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
	QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
	QComboBox *combo = qobject_cast<QComboBox *>(editor);
	if (!sqlModel || !childModel || !combo)
	{
		QItemDelegate::setEditorData(editor, index);
		return;
	}
	
	int ComboIndex = combo->findText(sqlModel->data(index).toString());
	//qDebug("Setting editor data combo: %s, data: %s, index.row():%d" , qPrintable(combo->objectName()),
		//qPrintable(sqlModel->data(index).toString()), index.row());

	if (ComboIndex >= 0)
		combo->setCurrentIndex(ComboIndex);
	else 
	{
		//qDebug("ComboIndex not found !");
		//Busquem indexcolumn corresponent
		int IndexCol = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
		int DisplayCol = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
		QModelIndexList ChildMatchLst = childModel->match(childModel->index(0, IndexCol), Qt::EditRole, 
			sqlModel->data(index, Qt::EditRole), Qt::MatchExactly | Qt::MatchWrap);
		if (!ChildMatchLst.isEmpty())
		{
			int FRow = ChildMatchLst.first().row();
			combo->setCurrentIndex(combo->findText(childModel->data(childModel->index(FRow, DisplayCol), Qt::EditRole).toString()));
			//qDebug("Curent index:%d", combo->currentIndex());
		}
	}
// 	int currentItem = combo->currentIndex();
// 	int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
// 	int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
// 	sqlModel->setData(index,
// 	                  childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole),
// 	                  Qt::DisplayRole);
// 	QVariant EditData = childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole);
// 	if (sqlModel->data(index, Qt::EditRole) != EditData)
// 		sqlModel->setData(index, EditData, Qt::EditRole);


}

void FSqlRelationalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if (!index.isValid())
		return;

	QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
	QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
	QComboBox *combo = qobject_cast<QComboBox *>(editor);
	if (!sqlModel || !childModel || !combo)
	{
		QItemDelegate::setModelData(editor, model, index);
		return;
	}

	int currentItem = combo->currentIndex();
	int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
	int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
	sqlModel->setData(index,
	                  childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole),
	                  Qt::DisplayRole);
	QVariant EditData = childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole);
	//if (sqlModel->data(index, Qt::EditRole) != EditData) //! Hi ha cops que no actualitzat bé !.
	sqlModel->setData(index, EditData, Qt::EditRole);
	//qDebug(QString("FSqlRelationalDelegate::setModelData: " + EditData.toString()).toLatin1() );

}
// void FSqlRelationalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
// {
// 	QSqlRelationalDelegate* MDelegate = new QSqlRelationalDelegate;
// 	MDelegate->setModelData(editor, model, index);
// 	delete MDelegate;
// 
// }
