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

#ifndef OOFFICEDATASUPPLIER_H
#define OOFFICEDATASUPPLIER_H
#include "stoofficeexport.h"
#include <QMap>
#include <QList>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

class ST_OOFFICE_EXPORT OOfficeDataSupplier
{
public:
	typedef QMap<QString, QVariant> TVarList;
	typedef QList<QSqlRecord> TVarView;
	typedef QMap<QString, TVarView> TVarViewCache;

private:
	TVarViewCache VarViewCache;
	QVariant execOp(const QString& _Op, const TVarView& , const QString& _FieldName);
	QString viewName(const QString& _View, const QString& _Filter) const;

public:
	virtual TVarList globalVars() const = 0;
	virtual QVariant getVar(const QString& _View, const QString& _FieldName, const QString& _Filter, const QString& _Op = "", int _Index = 0);
	virtual TVarView getView(const QString& _View, const QString& _Filter) const = 0;
	virtual bool accepts(const QString _View) const { return true; }

	TVarView viewFromQuery(QSqlQuery& _Query) const;
	void clearCache();
	void cacheVarView(const QString& _ViewName, const TVarView& _VarView);
};

#endif // OOFFICEDATASUPPLIER_H
