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

#include "oofficedatasupplier.h"

QVariant OOfficeDataSupplier::execOp(const QString& _Op, const TVarView& , const QString& _FieldName)
{
	QVariant Res;
	return Res;
}

QString OOfficeDataSupplier::viewName(const QString& _View, const QString& _Filter) const
{
	return QString("%1-%2").arg(_View).arg(_Filter);
}

QVariant OOfficeDataSupplier::getVar(const QString& _View, const QString& _FieldName, const QString& _Filter, const QString& _Op, int _Index)
{
	QVariant Res;
	QString ViewName = viewName(_View, _Filter);
	if (!VarViewCache.contains(ViewName))
	{
		TVarView View = getView(_View, _Filter);
		if (!View.isEmpty())
			cacheVarView(ViewName, View);
	}

	if (VarViewCache.contains(ViewName))
	{
		if (!_Op.isEmpty())
			Res = execOp(_Op, VarViewCache[ViewName], _FieldName);
		else
		{
			if (VarViewCache[ViewName].count() > _Index)
				Res = VarViewCache[ViewName][_Index].value(_FieldName);
			else
				Res = "";
		}
	}
	else
		Res = "";

	return Res;
}

OOfficeDataSupplier::TVarView OOfficeDataSupplier::viewFromQuery(QSqlQuery& _Query) const
{
	TVarView Res;
	while (_Query.next())
	{
		Res.append(_Query.record());
	}
	return Res;
}

void OOfficeDataSupplier::clearCache()
{
	VarViewCache.clear();
}

void OOfficeDataSupplier::cacheVarView(const QString& _ViewName, const TVarView& _VarView)
{
	VarViewCache.insert(_ViewName, _VarView);
}
