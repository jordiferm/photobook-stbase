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
#include "smappedwidget.h"
#include <QDataWidgetMapper> 
#include <QLineEdit> 
#include <QSpinBox> 
#include <QStyleOptionViewItem>
#include <QAbstractItemDelegate>
#include <QModelIndex> 
#include <QLabel>

SMappedWidget::SMappedWidget(QAbstractItemModel* _Model, QDataWidgetMapper* _Mapper, QWidget* _Parent) : QWidget(_Parent), Model(_Model) , Mapper(_Mapper)
{	
}

SMappedWidget::~SMappedWidget()
{
}

void SMappedWidget::setSuffix(QWidget* _Editor, const QString& _Suffix)
{
	if (_Editor->inherits("QSpinBox") || _Editor->inherits("QDoubleSpinBox"))
		static_cast<QSpinBox*>(_Editor)->setSuffix(" " + _Suffix);
	else 
		qDebug(QString("Could not set suffix to %1").arg(_Editor->metaObject()->className()).toLatin1());
}

/*!
	Creates the editor related to _Index in current model and mapps it with current QDataWidgetMapper.
	To create the editor this function uses current QDataWidgetMapper itemDelegate.
*/

QWidget* SMappedWidget::createEditor(const QModelIndex& _Index, const QString& _Suffix, QWidget* _Parent, const QString& _ObjectName)
{
	QWidget* Editor; 
	QWidget* Parent = _Parent; 
	if (!Parent)
		Parent = this;
	QStyleOptionViewItem Options;
	Editor = Mapper->itemDelegate()->createEditor(Parent, Options, _Index);
	
	if (!Editor)
		Editor = new QLineEdit(this);
	
	Mapper->addMapping(Editor, _Index.column());
	
	if (!_Suffix.isEmpty())
		setSuffix(Editor, _Suffix);
	
	if (!_ObjectName.isEmpty())
		Editor->setObjectName(_ObjectName); 

	return Editor;
}

QString SMappedWidget::headerLabelText(const QModelIndex& _Index)
{
	return Model->headerData(_Index.column(), Qt::Horizontal).toString() + " :";
}

//! It needs that Mapper has model assigned.
QLabel* SMappedWidget::createHeaderLabel(const QModelIndex& _Index, QWidget* _Parent, const QString& _ObjectName)
{
	QWidget* Parent = _Parent; 
	if (!Parent)
		Parent = this;
	
	QString LabelText = headerLabelText(_Index); 
	QLabel* Res = new QLabel(LabelText, Parent);
	if (!_ObjectName.isEmpty())
		Res->setObjectName(_ObjectName); 
	else
		Res->setObjectName("SMappedWidgetHeaderLabel");
	SectionLabels.insert(_Index.column(), Res);
	QFont Font = Res->font();
	Font.setBold(true);
	Res->setFont(Font);
	return Res;
}

QLabel* SMappedWidget::sectionLabel(const QModelIndex& _Index)
{
	QLabel* Res = 0;	
	if (SectionLabels.contains(_Index.column()))
		Res = SectionLabels[_Index.column()];
	return Res;
}
