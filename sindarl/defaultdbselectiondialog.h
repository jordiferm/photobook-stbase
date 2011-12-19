/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PUBDATABASECREATIONDIALOG_H
#define PUBDATABASECREATIONDIALOG_H

#include <QDialog>
#include "stsindarlexport.h"
#include "sindardatabase.h"

class QLabel;
class ST_SINDARL_EXPORT DefaultDbSelectionDialog : public QDialog
{
Q_OBJECT

	SindarDatabase::TDefaultDatabaseList DDBList;
	QLabel* DescriptionLabel;
	DefaultSindarDatabase SelectedDatabase;

	void selectDatabase(int _Index);

public:
	explicit DefaultDbSelectionDialog(QWidget *parent = 0);
	void updateDescription(int _Index);
	DefaultSindarDatabase selectedDatabase() const { return SelectedDatabase; }

signals:

public slots:
	void slotComboIndexActivated(int _Index);
};

#endif // PUBDATABASECREATIONDIALOG_H
