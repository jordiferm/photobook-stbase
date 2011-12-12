/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STARLABTABLE_H
#define STARLABTABLE_H

#include "storomelexport.h"
#include <QList>
#include <QSqlRecord>
#include <QString>

class ST_OROMEL_EXPORT StarlabTable : public QList<QSqlRecord>
{
	QString TableName;

public:
	StarlabTable(const QString& _TableName = "NoName");
	void setFilter(const QString& _Field, const QVariant& _Value);
	void setContent(const QString& _Content);//Sets the content directly from StarLab response.
};

#endif // STARLABTABLE_H
