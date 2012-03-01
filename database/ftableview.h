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

#ifndef FTABLEVIEW_H
#define FTABLEVIEW_H

#include <QTableView>
#include "stdatabaseexport.h"



/**
QTableView personalitzat per flam.

Per defecte te el mode de selecci� a selecci� de files. 
 
@author Shadow
*/
class FSqlRelationalTableModel;
class QSqlError;
class ST_DATABASE_EXPORT FTableView : public QTableView
{
	Q_OBJECT

public:
	FTableView(QWidget* _Parent = 0);
	virtual void setModel(FSqlRelationalTableModel* model );
	virtual void setModel ( QAbstractItemModel * model );

public slots:
	//! Es crida quan hi ha un error.
	virtual void submitError(const QSqlError& _Error);
	virtual void headerClicked(int _LogicalIndex);
	
};

#endif
