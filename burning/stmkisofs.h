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

#ifndef STMKISOFS_H
#define STMKISOFS_H

#include <QProcess>
#include <QString>
#include "sterror.h"
#include "stburningexport.h"

/**
Classe per a executar STMkIsoFs.

@author shadow
*/
class QProcess;
class STBurningSettings;
class ST_BURNING_EXPORT STMkIsoFs : public QProcess
{
Q_OBJECT

	QString BinFile;
	QString Version;
	QString TmpIsoFile;
	QString Device;

public:
	ST_DECLARE_ERRORCLASS();

private:
	int parseProgress(const QString& _Line);
	void checkForBinFile();

public:
	STMkIsoFs(QObject* _Parent = 0);
	void start(const QString& _SourcePath, const QString& _MSInfo = "");
	qint64 isoFileSize();
	void configure(const STBurningSettings& _Settings);
	void configure(const QString& _MKIsoFsBinary, const QString& _TmpIsoFile, const QString& _CDRecordDevice);

private slots: 
	void slotReadFromStdErr();

signals: 
	void statusChanged(const QString& _StatusStr, int _PercentComplete);	
};

#endif
