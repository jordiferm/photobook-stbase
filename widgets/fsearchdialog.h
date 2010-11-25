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
#ifndef FSEARCHDIALOG_H
#define FSEARCHDIALOG_H

#include "fdialog.h"
#include "fsearchdialogbase.h"
#include "stwidgetsexport.h"

/**
Dialeg per a cerques basades en model/View
Pot servir com a classe base de molts tipus de cerca.


@author Shadow
*/
class FSearchWidget;
class QPushButton;
class QModelIndex; 
class QAbstractTableModel;
class QAbstractItemView;
class QAbstractItemModel;
class ST_WIDGETS_EXPORT FSearchDialog : public FDialog, public FSearchDialogBase
{
Q_OBJECT


		
private: 
	void init(FSearchWidget* _SearchWidget);

public:
	FSearchDialog(FSearchWidget* _SearchWidget, QWidget* _Parent = 0);
	int exec();
	
	static QModelIndex exec(QAbstractTableModel* _Model, QWidget* _Parent = 0);
	static QModelIndex exec(QAbstractItemView* _View, QAbstractItemModel* _Model, QWidget* _Parent = 0);

};

#endif
