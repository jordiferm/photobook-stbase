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

#include "stmkisofs.h"
#include <QFile>
#include <QFileInfo>
#include "stburningsettings.h" 


STMkIsoFs::STMkIsoFs(QObject* _Parent)
 : QProcess(_Parent)
{
	//TODO Default settings for linux and Mac
	configure("/usr/bin/mkisofs", "/tmp/kpstmp.iso", "/dev/cdrom");
	connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadFromStdErr()));
}

int STMkIsoFs::parseProgress(const QString& _Line) 
{
	QString PerStr = _Line;
	PerStr.truncate(PerStr.indexOf('%'));
	bool Ok;
	double  Res = PerStr.toDouble(&Ok);
	return static_cast<int>(Res);
}


void STMkIsoFs::checkForBinFile()
{
  	Assert(QFile::exists(BinFile), Error(QString(tr("mkisofs can't be found at %0")).arg(BinFile)));

	QProcess MProcess;
	MProcess.start(BinFile, QStringList() << "-version");
	Assert(MProcess.waitForStarted(), Error(tr("mkisofs: the process can't be started")));
	Assert(MProcess.waitForFinished(),	Error(tr("mkisofs: the process can't finish")));

	QByteArray Result = MProcess.readAll();
	QString StrResult(Result);

	int Pos = StrResult.indexOf("mkisofs", 0, Qt::CaseInsensitive);
	Assert(Pos >= 0, Error(tr("mkisofs: the version can't be detected.")));		

	Pos = StrResult.indexOf(QRegExp("[0-9]"), Pos);
	Assert(Pos >= 0, Error(tr("mkisofs: the version can't be detected.")));		

	int EndPos = StrResult.indexOf(' ', Pos+1);
	Assert(EndPos >= 0, Error(tr("mkisofs: the version can't be detected.")));		
	Version = StrResult.mid(Pos, EndPos-Pos);
}

void STMkIsoFs::start(const QString& _SourcePath, const QString& _MSInfo)
{
	checkForBinFile();

	QStringList Arguments;
	
	if (!_MSInfo.isEmpty())
		Arguments << "-M" << Device << "-C" << _MSInfo; 
	
	Arguments << "-R" << "-r" << "-J" << QString("-o") << TmpIsoFile << _SourcePath;	
	qDebug(QString(BinFile + " " + Arguments.join(" ")).toLatin1() );
	QProcess::start(BinFile, Arguments);
}

void STMkIsoFs::configure(const QString& _STMkIsoFsBinary, const QString& _TmpIsoFile, const QString& _CDRecordDevice)
{
	BinFile = _STMkIsoFsBinary; 
	TmpIsoFile = _TmpIsoFile; 
	Device = _CDRecordDevice; 
}

void STMkIsoFs::configure(const STBurningSettings& _Settings)
{
	configure(_Settings.mkIsoFsBinary(), _Settings.tmpIsoFile(), _Settings.device());
}

qint64 STMkIsoFs::isoFileSize()
{
	return QFileInfo(TmpIsoFile).size();
}

void STMkIsoFs::slotReadFromStdErr()
{
	setReadChannel(QProcess::StandardError);
	while (canReadLine())
	{
		QString Line(readLine());
		if(Line.contains( "done, estimate" ) )
		{
			int ProgValue = parseProgress(Line);
			if(ProgValue != -1)
				emit statusChanged(QString("%0 Completed").arg(ProgValue), ProgValue);
		}
		else 
		if( Line.contains( "extents written" ) ) 
			emit statusChanged("Completed", 100);
		else 
			qDebug(Line.toLatin1());
	}
}

