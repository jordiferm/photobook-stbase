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

#include "starlabtable.h"
#include <QStringList>
#include <QVariant>
#include <QSqlField>
#include <QDebug>


StarlabTable::StarlabTable(const QString& _TableName) : TableName(_TableName)
{
}

void StarlabTable::setFilter(const QString& _Field, const QVariant& _Value)
{
	//TODO
}


void StarlabTable::setContent(const QString& _Content)//Sets the content directly from StarLab response.
{
	//For each record
	QStringList ContentRecords = _Content.split("||");
	QStringList::iterator rit;
	for (rit = ContentRecords.begin(); rit != ContentRecords.end(); ++rit)
	{
		//qDebug() << *rit;
		if (!rit->trimmed().isEmpty())
		{
			//For each field
			QStringList NameValueList = rit->split("|");
			QStringList::iterator it;
			QSqlRecord Rec;
			for (it = NameValueList.begin(); it != NameValueList.end(); ++it)
			{
				QStringList NameValue = it->split("=");
				if (NameValue.size() > 0)
				{
					Rec.append(QSqlField(NameValue[0], QVariant::String));
				}
				if (NameValue.size() > 1)
				{
					Rec.setValue(NameValue[0], NameValue[1]);
					//qDebug() << "Adding field" << NameValue[0] << "=" << NameValue[1];
				}
			}
			push_back(Rec);
		}
	}
}
