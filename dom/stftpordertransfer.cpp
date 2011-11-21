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
#include "stftpordertransfer.h"
#include <QTime> 
#include <QDir> 
#include <QApplication> 
#include <QNetworkInterface> 
#include <QTemporaryFile> 

#include "stimage.h"
#include "stcollectionpublishermodel.h"
#include "stxmlpublishersettings.h"
#include "stlog.h"
#include "xmlorder.h"
#include "publisher.h"
#include "sprocessstatuswidget.h"

using namespace STDom;

int STFtpOrderTransfer::DefaultTimeOutSecs = 46;

/*!
	\return false when there is a time out on operation.
*/
bool STFtpOrderTransfer::waitForCommand(int _CommandId, int _TimeOutSecs)
{
	int TimeOutSecs;
	if (_TimeOutSecs = -1)
		TimeOutSecs = DefaultTimeOutSecs; 
	else 
		TimeOutSecs = _TimeOutSecs;

	CurrCommandFinished = -1;
	bool TimeOut = false;
	InitTime = QTime::currentTime();
	while(CurrCommandFinished != _CommandId && !TimeOut && !Aborted)
	{
		QApplication::processEvents();
		//msleep(200);
		TimeOut = (InitTime.secsTo(QTime::currentTime()) >= TimeOutSecs) ;
	}
	return !TimeOut;
}		

void STFtpOrderTransfer::getDirInt(const QString& _RemoteDirName, const QString& _LocalDestDir, SProcessStatusWidget* _ProcessWidget)
{
	waitForCommand(cd(_RemoteDirName));
	Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDirName)));

	QMap<QString, QUrlInfo> SavedFilesTrans = FilesTransfered;
	FilesTransfered.clear();

	Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));

	//Mkdir for destdir
	QDir DestDir(_LocalDestDir);
	DestDir.mkdir(_RemoteDirName);
	DestDir.cd(_RemoteDirName);


	QMap<QString, QUrlInfo>::iterator it;
	if(_ProcessWidget)
	{
		_ProcessWidget->showProgressBar(tr("Downloading files from %1").arg(_RemoteDirName), FilesTransfered.size());
		QApplication::processEvents();
	}
	for (it = FilesTransfered.begin(); it != FilesTransfered.end(); ++it)
	{
		if (it.value().isDir())
			getDirInt(it.key(), DestDir.absolutePath());
		else //Its a file
		{
			QFileInfo DestFileInfo(DestDir.absoluteFilePath(it.key()));
			QFile File(DestFileInfo.absoluteFilePath());
			Assert(File.open(QIODevice::WriteOnly), Error(QString(tr("Could not open file: '%1'")).arg(DestFileInfo.absoluteFilePath())));
			Assert(waitForCommand(get(it.key(), &File)), Error(tr("Time out Error getting file %1.").arg(it.key())));
			if (error() != QFtp::NoError)
			{
				File.remove();
				throw Error(tr("Could not download file: %1").arg(it.key()));
			}
			File.close();
			if(_ProcessWidget)
			{
				_ProcessWidget->incrementProgress();
				QApplication::processEvents();
			}
		}
	}
	waitForCommand(cd(".."));
	FilesTransfered= SavedFilesTrans;
}

qint64 STFtpOrderTransfer::calcDirTransferBytesInt(const QString& _RemoteDirName)
{
	qint64 Res = 0;
	waitForCommand(cd(_RemoteDirName));
	Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDirName)));

	QMap<QString, QUrlInfo> SavedFilesTrans = FilesTransfered;
	FilesTransfered.clear();

	Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));

	QMap<QString, QUrlInfo>::iterator it;
	for (it = FilesTransfered.begin(); it != FilesTransfered.end(); ++it)
	{
		if (it.value().isDir())
			Res += calcDirTransferBytesInt(it.key());
		else //Its a file
		{
			Res += it.value().size();
		}
	}
	waitForCommand(cd(".."));
	FilesTransfered= SavedFilesTrans;
	return Res;
}

void STFtpOrderTransfer::putDirInmer(const QString& _SourceDirPath, const QString& _RemoteDestDir, SProcessStatusWidget* _ProcessWidget)
{
	int MKDirCommand = mkdir(_RemoteDestDir);
	IgnoreErrorCommands.push_back(MKDirCommand);
	waitForCommand(MKDirCommand);

	waitForCommand(cd(_RemoteDestDir));
	Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDestDir)));

	QDir SourceDir(_SourceDirPath);
	QStringList SourceDirsList = SourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::iterator it;
	for (it = SourceDirsList.begin(); it != SourceDirsList.end(); ++it)
	{
		QDir NewSourceDir(*it);
		QDir DestDir(_RemoteDestDir);
		putDirInmer(NewSourceDir.absolutePath(), DestDir.absoluteFilePath(NewSourceDir.dirName()), _ProcessWidget);
	}

	//For each file
	QStringList SourceFilesList = SourceDir.entryList(QDir::Files);
	if(_ProcessWidget)
		_ProcessWidget->showProgressBar(tr("Uploading files from %1").arg(_SourceDirPath), SourceFilesList.size());
	for (it = SourceFilesList.begin(); it != SourceFilesList.end(); ++it)
	{
		QFileInfo FInfo(SourceDir.absoluteFilePath(*it));
		QFile File(FInfo.absoluteFilePath());
		Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Could not open file %1")).arg(*it)));
		Assert(waitForCommand(put(&File, FInfo.fileName())), Error(tr("Time out Error putting file %1.").arg(*it)));
		if(_ProcessWidget)
			_ProcessWidget->incrementProgress();
	}
}

STFtpOrderTransfer::STFtpOrderTransfer(QObject* parent): QFtp(parent)
{
	connect(this, SIGNAL(listInfo( const QUrlInfo& )), this, SLOT(slotListInfo(const QUrlInfo& )));
	connect(this, SIGNAL(commandFinished( int, bool )), this, SLOT(commandFinished(int, bool)));
	connect(this, SIGNAL(dataTransferProgress( qint64, qint64 )), this, SLOT(slotDataTransferProgress( qint64, qint64 )));
}

STFtpOrderTransfer::~STFtpOrderTransfer()
{
}

void STFtpOrderTransfer::abortAll()
{
	Aborted = true;
	clearPendingCommands(); 
	abort();
}

void STFtpOrderTransfer::closeConnection()
{
	Assert(waitForCommand(close()), Error(tr("Time out Error disconnecting from host.")));
}

/*!
	\return true if _SizeA and _SizeB are considered to be the same size.
*/
bool STFtpOrderTransfer::sameFileSize(quint64 _SizeA, quint64 _SizeB)
{
	return abs(_SizeA - _SizeB) < 1024 * 2;
}

QString STFtpOrderTransfer::orderGlobalId(const QDir& _OrderDir)
{
	QString Res = _OrderDir.path().right(_OrderDir.path().length() - _OrderDir.path().lastIndexOf("/"));
	QFileInfoList OrderFiles = _OrderDir.entryInfoList(QDir::Files, QDir::Name); 
	int Cnt = 0; 
	bool Found = false; 
	while (Cnt < OrderFiles.size() && !Found)
	{
		Found = OrderFiles[Cnt].completeSuffix().toLower() != "xml";
		if (!Found)
			Cnt++;
	}
	if (Found)
		Res = STImage::hashString(OrderFiles[Cnt].absoluteFilePath());
		//Res = OrderFiles[Cnt].baseName();
	
	
	QList<QNetworkInterface> AllIFaces = QNetworkInterface::allInterfaces(); 
	QList<QNetworkInterface>::iterator it;
	Found = false; 
	QString MacAddress; 
	it = AllIFaces.begin();
	while (it != AllIFaces.end() && !Found)
	{
		Found = (it->hardwareAddress() != "00:00:00:00:00:00");
		if (Found)
			MacAddress = it->hardwareAddress();
		++it;
	}
	if (!Found)
		MacAddress = QDir().home().dirName();
	
	return MacAddress.replace(":", "_") + "_" + _OrderDir.dirName() + "_" + Res; 
}


void STFtpOrderTransfer::getFile(const QString& _FileName, const QString& _RemoteDir, const QString& _DestFilePath, const QString& _Host, int _Port, const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
	try
	{
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		waitForCommand(cd(_RemoteDir));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDir)));
		QFileInfo FInfo(_DestFilePath);
		QFile File(FInfo.absoluteFilePath());
		Assert(File.open(QIODevice::WriteOnly), Error(QString(tr("Could not open file: '%1'")).arg(FInfo.absoluteFilePath())));
		Assert(waitForCommand(get(_FileName, &File)), Error(tr("Time out Error getting file %1.").arg(_FileName)));
		if (error() != QFtp::NoError)
		{
			File.remove();
			throw Error(tr("Could not download file: %1").arg(_RemoteDir + "/" + _FileName));
		}
		File.close();
		close();
	}
	catch(...)
	{
		close();
		throw;
	}
}


void STFtpOrderTransfer::putDir(const QString& _SourceDirPath, const QString& _Host, int _Port, const QString& _User,
								const QString& _Password, const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode,
								SProcessStatusWidget* _ProcessWidget)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));

	try{
		Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		putDirInmer(_SourceDirPath, _RemoteDestDir, _ProcessWidget);
		close();
	}
	catch(...)
	{
		close();
		throw;
	}



}


void STFtpOrderTransfer::putFile(const QString& _SourceFilePath, const QString& _Host, int _Port, const QString& _User, const QString& _Password, const QString& _RemoteDestDir, QFtp::TransferMode _TransferMode)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	try{
		Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		waitForCommand(cd(_RemoteDestDir));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDestDir)));

		QFileInfo FInfo(_SourceFilePath);
		QFile File(FInfo.absoluteFilePath());
		Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Could not open file %1")).arg(_SourceFilePath)));
		Assert(waitForCommand(put(&File, FInfo.fileName())), Error(tr("Time out Error putting file %1.").arg(_SourceFilePath)));
		close();
	}
	catch(...)
	{
		close();
		throw;
	}
}


qint64 STFtpOrderTransfer::calcDirTransferBytes(const QString& _RemoteDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode)
{
	qint64 Res = 0;
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
	try
	{
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		waitForCommand(cd(_RemoteDir));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDir)));
		waitForCommand(cd(".."));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg("..")));

		QDir RDir(_RemoteDir);
		Res = calcDirTransferBytesInt(RDir.dirName());
		close();
	}
	catch(...)
	{
		close();
		throw;
	}
	return Res;
}

void STFtpOrderTransfer::getDir(const QString& _RemoteDir, const QString& _LocalDestDir, const QString& _Host, int _Port,
								const QString& _User, const QString& _Password,QFtp::TransferMode _TransferMode,
								SProcessStatusWidget* _ProcessWidget)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
	try
	{
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		waitForCommand(cd(_RemoteDir));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteDir)));
		waitForCommand(cd(".."));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg("..")));

		QDir RDir(_RemoteDir);
		getDirInt(RDir.dirName(), _LocalDestDir, _ProcessWidget);
		close();
	}
	catch(...)
	{
		close();
		throw;
	}

}

QList<QUrlInfo> STFtpOrderTransfer::getFilesList(const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: '%1' -> '%2'").arg(_Host).arg(errorString())));
	waitForCommand(login(_User,_Password));
	Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
	waitForCommand(cd(_RemoteSourceDir));
	Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteSourceDir)));
	FilesTransfered.clear();
	Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));
	close();
	return FilesTransfered.values();
}

void STFtpOrderTransfer::syncRemoteDir(const QString& _DestDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
	waitForCommand(login(_User,_Password));
	Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
	waitForCommand(cd(_RemoteSourceDir));
	Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteSourceDir))); 
	FilesTransfered.clear();
	Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));

	QDir DestDir(_DestDir); 
	QFileInfoList DestDirInfoList = DestDir.entryInfoList(QDir::Files); 
	
	QFileInfoList::const_iterator it; 
	for (it = DestDirInfoList.begin(); it != DestDirInfoList.end(); ++it)
	{
		QString CurrFileName = it->fileName(); 
		if (FilesTransfered.contains(CurrFileName))
		{
			if (FilesTransfered[CurrFileName].lastModified() > it->lastModified())
			{
//				qDebug(QString("Updating file %1").arg(CurrFileName).toLatin1());
				QFile File(it->absoluteFilePath());
				Assert(File.open(QIODevice::WriteOnly | QIODevice::Truncate), Error(QString(tr("Could not open file %1")).arg(it->absoluteFilePath())));
				Assert(waitForCommand(get(CurrFileName, &File)), Error(tr("Time out Error getting file %1.").arg(it->fileName())));
			}
		}
	}
	close();
// 	QList<QUrlInfo> UrlInfoList = FilesTransfered.values(); 
// 	QList<QUrlInfo>::const_iterator it; 
// 	for (it = UrlInfoList.begin(); it != UrlInfoList.end(); ++it)
// 	{
// 		qDebug(it->name().toLatin1()); 
// 	}
}

void STFtpOrderTransfer::syncRemoteFile(const QString& _FileName, const QString& _DestDir, const QString& _Host, int _Port, const QString& _User, const QString& _Password,  const QString& _RemoteSourceDir, QFtp::TransferMode _TransferMode)
{
	clearAbortFlag();
	setTransferMode(_TransferMode);
	Assert(waitForCommand(connectToHost(_Host, _Port)), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1 -> %2").arg(_Host).arg(errorString())));
	try
	{
		waitForCommand(login(_User,_Password));
		Assert(state() == QFtp::LoggedIn, Error(tr("Could login to host: %1 ").arg(_Host)));
		waitForCommand(cd(_RemoteSourceDir));
		Assert(error() == QFtp::NoError, Error(tr("Could not chdir to: %1").arg(_RemoteSourceDir)));
		FilesTransfered.clear();
		Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));


		QFileInfo DestFileInfo = QFileInfo(QDir(_DestDir).absoluteFilePath(_FileName));
		//qDebug() << FilesTransfered.keys();
		//qDebug() << "Requested file" << _FileName;
		if (FilesTransfered.contains(_FileName))
		{
			//qDebug() << "File in dest list";
			if (FilesTransfered[_FileName].lastModified() > DestFileInfo.lastModified() || !DestFileInfo.exists() || DestFileInfo.size() == 0)
			{
				//qDebug() << "Downloading file...";
				QFile File(DestFileInfo.absoluteFilePath());
				Assert(File.open(QIODevice::WriteOnly | QIODevice::Truncate), Error(QString(tr("Could not open file %1")).arg(DestFileInfo.absoluteFilePath())));
				Assert(waitForCommand(get(_FileName, &File)), Error(tr("Time out Error getting file %1.").arg(DestFileInfo.fileName())));
			}
		}
		close();
	}
	catch(...)
	{
		close();
		throw;
	}
}

//TODO: Pass the publisher
void STFtpOrderTransfer::transferOrder(const QString& _OrderId,const Publisher& _Publisher)
{
	QDir OrderDir(XmlOrderInfo::defaultOrderPath(_OrderId));

	clearAbortFlag();
	setTransferMode(static_cast<QFtp::TransferMode>(_Publisher.transferMode()));
	Assert(waitForCommand(connectToHost(_Publisher.ftpUrl(), _Publisher.ftpPort())), Error(tr("Time out Error connecting to host.")));
	Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1").arg(errorString())));
	login(_Publisher.userName(), _Publisher.password());
	if (!_Publisher.initDir().isEmpty())
		cd(_Publisher.initDir());
	QString OrderGlobalId = orderGlobalId(OrderDir); 
	QString RemoteDir = OrderGlobalId;
	int MKDirCommand = mkdir(RemoteDir);
	IgnoreErrorCommands.push_back(MKDirCommand);
	waitForCommand(MKDirCommand);
	
	waitForCommand(cd(RemoteDir));

	int Cnt = 0;
	int TotalSteps = 0;
	int NRetries = 0; 
	bool FilesTransferedOK = false;
	while (!FilesTransferedOK && NRetries++<5 && !Aborted)
	{
		//qDebug("Getting files list."); 
		FilesTransfered.clear();
		Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));
		QFileInfoList FilesToPut = OrderDir.entryInfoList(QDir::Files); 
		QFileInfoList::iterator it; 
		QString DestAbsDirPath = _Publisher.initDir() + "/" + RemoteDir;
		Cnt = 0; 
		TotalSteps = FilesToPut.size() -1;
		emit overallProcessStep(TotalSteps, Cnt++);	 
		it = FilesToPut.begin();

		QMap<QString, QUrlInfo>::iterator dit; 
		while (!Aborted && it != FilesToPut.end())
		{
			bool AlreadyTransfered = false; 
			if (FilesTransfered.contains(it->fileName()))
			{
				AlreadyTransfered = sameFileSize(FilesTransfered[it->fileName()].size(), it->size());
				if (!AlreadyTransfered)//If File exists but the size is diferent we'll remove it.
				{
					stLog << ISTLogIO::Warning << ISTLogIO::L2 << "Wrong file transfer for file" <<
							//it->fileName() //<<  ", local size:" << it->size() << ", remote size:" <<  FilesTransfered[it->fileName()].size() -- Size call breaks STLog I don't know why...
						  it->fileName()  << ", trying to remove it" << ISTLogIO::Endl;
					//qDebug(QString("Removing File %1").arg(DestAbsDirPath + "/" + it->fileName()).toLatin1()); 
					int RmFileCommand = remove(DestAbsDirPath + "/" + it->fileName());
					IgnoreErrorCommands.push_back(RmFileCommand);
					waitForCommand(RmFileCommand);					
				}
			}
	
			if (!AlreadyTransfered &&
					it->completeSuffix().toLower() != "xml")
			{
				//qDebug(QString("... Transfering %1 ...").arg(DestAbsDirPath + "/" + it->fileName()).toLatin1()); 
				QFile File(it->absoluteFilePath());
				Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Could not open file %1")).arg(it->absoluteFilePath())));
				
				Assert(waitForCommand(put(&File, DestAbsDirPath + "/" + it->fileName())), Error(tr("Time out Error sending file %1.").arg(it->fileName())));
				emit overallProcessStep(TotalSteps, Cnt);
			}
			Cnt++;
			++it;
		}
		if (!Aborted)
		{
			//Check if all files are correct.
// 			for (dit = FilesTransfered.begin(); dit != FilesTransfered.end(); ++dit)
// 				qDebug(QString("FilesTransfered->%1").arg(dit.key()).toLatin1());
	
			FilesTransfered.clear();
			Assert(waitForCommand(list()), Error(tr("Time out Error getting files list.")));
			FilesToPut = OrderDir.entryInfoList(QDir::Files); 
			it = FilesToPut.begin();
			FilesTransferedOK = true; 
			while (!Aborted && it != FilesToPut.end() && FilesTransferedOK)
			{
				if (it->completeSuffix().toLower() != "xml" )
				{
					//qDebug(QString("Checking for file %1").arg(it->fileName()).toLatin1()); 
					if (FilesTransfered.contains(it->fileName()))
					{
						if (!sameFileSize(FilesTransfered[it->fileName()].size(), it->size()))
						{
							FilesTransferedOK = false; 
							stLog << ISTLogIO::Warning << ISTLogIO::L2 << "File sizes are not the same for file" <<
								it->fileName() <<  ", local size:" << it->size() << ", remote size:" <<  FilesTransfered[it->fileName()].size() << ISTLogIO::Endl; 

							//Delete remote file.
							/*qDebug(QString("Removing File %1").arg(DestAbsDirPath + "/" + it->fileName()).toLatin1()); 
							int RmFileCommand = remove(DestAbsDirPath + "/" + it->fileName());
							IgnoreErrorCommands.push_back(RmFileCommand);
							waitForCommand(RmFileCommand);*/
						}
					}
					else 
					{
						FilesTransferedOK = false; 
						stLog << ISTLogIO::Warning << ISTLogIO::L2 << "File is not transfered: " << it->fileName() << ISTLogIO::Endl; 
					}
				}
				++it;
			}
		}
	}		

	if (FilesTransferedOK)
	{
		//Now send xml command file.
		QStringList Filters; 
		Filters << "*.xml" << "*.XML";
		QFileInfoList FilesToPut = OrderDir.entryInfoList(Filters, QDir::Files); 
		QFileInfoList::iterator it = FilesToPut.begin();
		while(!Aborted && it != FilesToPut.end())
		{
			bool AlreadyTransfered = false; 
			if (FilesTransfered.contains(it->fileName()))
				AlreadyTransfered = FilesTransfered[it->fileName()].size() == it->size();
			if (!AlreadyTransfered )
			{
				//Try to load order and clean sender data.
				STDom::XmlOrder Order;
				if (Order.loadXml(it->absoluteFilePath()))
				{
					Order.setGlobalId(OrderGlobalId); 
					QTemporaryFile TempXmlOFile; 
					Assert(TempXmlOFile.open(), Error(tr("Could not create a temporary file")));
					TempXmlOFile.setAutoRemove(true);
					Order.saveXml(TempXmlOFile.fileName()); 
					Assert(TempXmlOFile.open(), Error(QString(tr("Could not open file %1")).arg(TempXmlOFile.fileName())));
					Assert(waitForCommand(put(&TempXmlOFile, it->fileName())), Error(tr("Time out Error sending file %1.").arg(it->fileName())));
				}
				else //Is another xml file, not order file.
				{
					QFile File(it->absoluteFilePath());
					Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Could not open file %1")).arg(it->absoluteFilePath())));
					Assert(waitForCommand(put(&File, it->fileName())), Error(tr("Time out Error sending file %1.").arg(it->fileName())));
				}
			}
			Cnt++;
			++it;
		}
		//--- Transfer order.xml to Dealer.
		/*if (PubSettings.hasDealer()) //Transfer order.xml to dealer
		{
			Assert(waitForCommand(close()), Error(tr("Time out Error closing connection.")));
			Assert(waitForCommand(connectToHost(PubSettings.dealerUrl(), PubSettings.dealerPort())), Error(tr("Time out Error connecting to host.")));
			Assert(state() == QFtp::Connected, Error(tr("Could not connect to host: %1").arg(errorString())));
			login(PubSettings.dealerUserName(), PubSettings.dealerPassword());
			if (!PubSettings.dealerInitDir().isEmpty())
				cd(PubSettings.dealerInitDir());

			it = FilesToPut.begin(); 
			while(!Aborted && it != FilesToPut.end())
			{
				if (it->fileName() != PubInfo.publisherXmlFile().fileName())
				{
					//Load order and set globalid
					STDom::XmlOrder Order;
					if (Order.loadXml(it->absoluteFilePath()))
					{
						Order.setGlobalId(OrderGlobalId); 
						QTemporaryFile TempXmlOFile; 
						Assert(TempXmlOFile.open(), Error(tr("Could not create a temporary file")));
						TempXmlOFile.setAutoRemove(true);
						Order.saveXml(TempXmlOFile.fileName()); 
						QString DestFileName = it->baseName() + "-" + OrderGlobalId + ".xml";
						Assert(TempXmlOFile.open(), Error(QString(tr("Could not open file %1")).arg(TempXmlOFile.fileName())));
						Assert(waitForCommand(put(&TempXmlOFile, DestFileName)), Error(tr("Time out Error sending file %1.").arg(it->fileName())));
					}
				}
				Cnt++;
				++it;
			}
		}*/

		emit overallProcessStep(TotalSteps, Cnt);
	}
	else 
		emit transferError(tr("File size check failed"));		
}


void STFtpOrderTransfer::slotListInfo(const QUrlInfo& _UrlInfo)
{
	FilesTransfered.insert(_UrlInfo.name(), _UrlInfo);
}

void STFtpOrderTransfer::commandFinished(int _Id, bool _Error)
{
	CurrCommandFinished = _Id;
	if (_Error && !IgnoreErrorCommands.contains(_Id) )
	{
		emit transferError(errorString());		
	}
	if (IgnoreErrorCommands.contains(_Id))
	#if QT_VERSION >= 0x040400
		IgnoreErrorCommands.removeOne(_Id);
	#else 
		IgnoreErrorCommands.removeAt(IgnoreErrorCommands.indexOf(_Id));
	#endif 
}

void STFtpOrderTransfer::slotDataTransferProgress(qint64, qint64)
{
	InitTime = QTime::currentTime();
}

