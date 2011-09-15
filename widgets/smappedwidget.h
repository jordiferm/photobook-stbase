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
#ifndef SMAPPEDWIDGET_H
#define SMAPPEDWIDGET_H
#include <QWidget> 
#include <QMap> 

#include "stwidgetsexport.h"

/**
Widget with mapper suitable to create for example data forms related to a model.

	@author
*/
class QModelIndex; 
class QDataWidgetMapper;
class QLabel;
class QAbstractItemModel;
class ST_WIDGETS_EXPORT SMappedWidget : public QWidget
{
	Q_OBJECT
	QDataWidgetMapper* Mapper;
	QAbstractItemModel* Model;
	QMap<int, QLabel*>	SectionLabels;

public:
	SMappedWidget(QAbstractItemModel* _Model, QDataWidgetMapper* _Mapper, QWidget* _Parent = 0);
	~SMappedWidget();
	void setSuffix(QWidget* _Editor, const QString& _Suffix);
	QWidget* createEditor(const QModelIndex& _Index, const QString& _Suffix = "", QWidget* _Parent = 0, const QString& _ObjectName = "");
	QString headerLabelText(const QModelIndex& _Index);
	QLabel* createHeaderLabel(const QModelIndex& _Index, QWidget* _Parent = 0, const QString& _ObjectName = "");
	//! Returns de label created for index _Index.
	QLabel* sectionLabel(const QModelIndex& _Index);
};

#endif
