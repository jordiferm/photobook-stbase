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

#ifndef FSQLRELATIONALDELEGATE_H
#define FSQLRELATIONALDELEGATE_H
#include <QItemDelegate> 
#include "stdatabaseexport.h"


//TODO: Translate comments to english.
/**
Delegate per a models SqlRelational.
També acota els camps dels models Sql segons el tipus i la mida obtingudes dels QSqlRecord.
I és derivada e FItemDelegate. \sa FItemDelegate.
@author Shadow
*/
class ST_DATABASE_EXPORT FSqlRelationalDelegate : public QItemDelegate
{
Q_OBJECT
public:
	FSqlRelationalDelegate(QObject* _Parent);
	QWidget* createEditor(QWidget* _Parent,
	                      const QStyleOptionViewItem& _Option,
	                      const QModelIndex& _Index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

#endif
