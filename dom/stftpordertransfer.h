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
#ifndef STFTPORDERTRANSFER_H
#define STFTPORDERTRANSFER_H

#include <QFtp>
#include <QList> 
#include <QMap>
#include <QTime>
#include "sterror.h"
#include "stdomexport.h"

class QDir;
class SProcessStatusWidget;
namespace STDom
{

/**
Class to send Starphob Orders via Ftp.

	@author
*/
class Publisher;
class ST_DOM_EXPORT STFtpOrderTransfer : public QFtp
{
	Q_OBJECT
	bool Aborted; 
			
public:
	ST_DECLARE_ERRORCLASS();
	
private:	
	QList<int> IgnoreErrorCommands;
	QMap<QString, QUrlInfo> FilesTransfered;
	int CurrCommandFinished; 
	static int DefaultTimeOutSecs;
	QTime InitTime;
	
	bool waitForCommand(int _CommandId, int _TimeOut = -1);
	void getDirInt(const QString& _RemoteDirName, const QString& _LocalDestDir);
	qint64 calcDirTransferBytesInt(const QString& _RemoteDirName);
	void putDirInmer(const QString& _SourceDirPath, const QString& _RemoteDestDir, SProcessStatusWidget* _ProcessWidget = 0);

	
public:
	STFtpOrderTransfer(QObject* parent = 0);
	QString orderGlobalId(const QDir& _OrderDir);
	void getFile(const QString& _SourceFilePath, const QString& _RemoteTemplatesDir, const QString& _DestFilePath, const QString& _Host, int _Port,
				 const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode);
	void putDir(const QString& _SourceDirPath, const QString& _Host, int _Port, const QString& _User,
				const QString& _Password, const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode,
				SProcessStatusWidget* _ProcessWidget = 0);
	void putFile(const QString& _SourceFilePath, const QString& _Host, int _Port, const QString& _User, const QString& _Password, const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode);
	qint64 calcDirTransferBytes(const QString& _RemoteDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode);
	void getDir(const QString& _RemoteDir, const QString& _LocalDestDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode);
	QList<QUrlInfo> getFilesList(const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode);
	void syncRemoteDir(const QString& _DestDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode);
	void syncRemoteFile(const QString& _FileName, const QString& _DestDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode);
	void transferOrder(const QString& _OrderId, const Publisher& _Publisher);
	~STFtpOrderTransfer();
	//!Aborts current command command and all pending commands.
	void abortAll();
	void clearAbortFlag() { Aborted = false; }
	bool isAborted() { return Aborted; }
	void closeConnection();
	static void setDefaultTimeOutSecs(int _Value) { DefaultTimeOutSecs = _Value; }
	static bool sameFileSize(quint64 _SizeA, quint64 _SizeB);

	
private slots:
	void slotListInfo(const QUrlInfo& _UrlInfo);
	void commandFinished(int _Id, bool _Error);
	void slotDataTransferProgress(qint64, qint64);
	
signals: 
	void transferError(const QString& _Error);	
	void overallProcessStep(int _Total, int _Done);
};
} //namespace
#endif
