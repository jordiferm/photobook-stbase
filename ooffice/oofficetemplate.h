/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef OOFFICETEMPLATE_H
#define OOFFICETEMPLATE_H
#include <QString>
#include <QVariant>
#include <QMap>
#include "sterror.h"

class OOfficeTemplate
{
	ST_DECLARE_ERRORCLASS();

	static QString ZipBinFilePath;
	static QString UnzipBinFilePath;
	QMap<QString, QVariant> Vars;
	QString FilePath;

	QString parseLine(const QString& _Line);
	void parseFile(const QString& _FileName);

public:
	OOfficeTemplate(QString& _FilePath);
	void setFilePath(const QString& _Value) { FilePath = _Value; }
	void addVar(const QString& _Key, const QVariant& _Value);
	void clearVars();
	static void setZipBinFilePath(const QString& _FilePath);
	static void setUnzipBinFilePath(const QString& _FilePath);
	static bool isTemplate(const QString& _DocFilePath);
	static QString templateExtension();
	static QString destFileName(const QString& _FileName, const QString& _ProjectName = "");
	void parse(const QString& _OutputDocFilePath);

};

#endif // OOfficeTemplate_H
