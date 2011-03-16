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

#include "kpiprinter.h"
#include <QSettings>
#include <QTcpServer>
#include <QDataStream>
#include <QTcpSocket>
#include <QStringList>



// _________________________________________________________________________*/
//
//  Class KPIPrinterStatusRep
// _________________________________________________________________________*/


KPIPrinterStatusRep::KPIPrinterStatusRep(QTcpSocket* _ClientConnection, QObject* _Parent)
	: QObject(_Parent), ClientConnection(_ClientConnection)
{
	
}


void KPIPrinterStatusRep::setStatus(EnStatus _Status, int _Percent, const QString& _Error)
{
	if (ClientConnection->state() == QAbstractSocket::ConnectedState)
	{
		
		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_0);
		out << (quint16) 0; // La Mida que posarem empres
	
		out << (quint16) _Status;
		out << (quint16) _Percent;
		out << _Error;
		
		out.device()->seek(0);
		out << (quint16)(block.size() - sizeof(quint16));
	
		ClientConnection->write(block);
		//qDebug("Writing %d bytes...", ClientConnection->bytesToWrite());
		ClientConnection->flush();
		ClientConnection->waitForBytesWritten(5000);
		if (!_Error.isEmpty())
			qDebug(QString("Error: %1 --- REPORTED !!! ---").arg(_Error).toLatin1());
		
	}

}

//! Llegeis l'status a partir d'un Socket
KPIPrinterStatusRep::EnStatus KPIPrinterStatusRep::readStatus(QTcpSocket* _Socket, int& _Percent, QString& _Error)
{
	EnStatus Res = StatusError;
	if ( _Socket->state() ==  QAbstractSocket::ConnectedState)
	{
		quint16 BlockSize = 0;
		QDataStream in(_Socket);
		in.setVersion(QDataStream::Qt_4_0);
		if (_Socket->bytesAvailable() < (int)sizeof(quint16))
		{
			qDebug("No data avaliable!");
			return Res;
		}
	
		in >> BlockSize;
	
		if (_Socket->bytesAvailable() < BlockSize)
		{
			//qDebug("BlockSize: %d is bigger than BytesAvaliable: %d", BlockSize, _Socket->bytesAvailable());
			return Res;
		}
	
		quint16 Status;
		in >> Status;

		quint16 Percent;
		in >> Percent;
		_Percent = Percent;
		
		QString ErrorDesc;
		in >> ErrorDesc;
		_Error = ErrorDesc;

		Res = static_cast<EnStatus>(Status);
		
	}
	return Res;
}

// _________________________________________________________________________*/
//
//  Class KPIPrinter::FormatInfo
// _________________________________________________________________________*/

void KPIPrinter::FormatInfo::fromString(const QString& _Format)
{
	Width = _Format.left(_Format.indexOf("x")).toInt();
	Height = _Format.right(_Format.length() - (_Format.indexOf("x") + 1)).toInt();
}

// _________________________________________________________________________*/
//
//  Class KPIPrinter
// _________________________________________________________________________*/

KPIPrinter::KPIPrinter(int _DefaultPort, QObject* _Parent )
 : QObject(_Parent)
{

 	QSettings Settings;
 	if (!Settings.contains("port"))
 	{
 		Settings.setValue("port", _DefaultPort); 
 		Settings.sync();
 	}
 	Port = Settings.value("port").toInt();
 	
 	Server = new QTcpServer;
 	connect(Server, SIGNAL(newConnection()), this, SLOT(newConnection()));
 	Server->setMaxPendingConnections(1);
}


void KPIPrinter::start()
{
	if (!Server->listen(QHostAddress::Any, Port))
	{
		qFatal(QString("SharedDirPrinter Server: Unable to start the server: %1.")
		       .arg(Server->errorString()).toAscii());
	}
	qDebug(QString("SharedDirPrinter is running on port %1.\n.").arg(Server->serverPort()).toAscii());
}


void KPIPrinter::runCommand(EnCommandId _CommandId, const QStringList& _Parameters, 
	QTcpSocket* _ClientConnection)
{
	KPIPrinterStatusRep* StatusRep = new KPIPrinterStatusRep(_ClientConnection, this);
	try
	{
		switch (_CommandId)
		{
			case PrintCommandId: 
			{
				printCommand(_Parameters[0], StatusRep);
			}
			break;
			case PrintImageId:
			{
				printImage(_Parameters[0], _Parameters[1].toInt(), StatusRep, FormatInfo(_Parameters[2]));
			}
			break;
			case EndOfJobId:
			{
				endOfJob();
			}
			break;
			case ConfigPrinterId: 
			{
				configPrinter();
			}
			break;
		}
		delete StatusRep;
	}
	///!!!!! Error per a printers !!!!!
	catch (STError& _Error)
	{
		qDebug("KPSPrnLibError:");
		qDebug(_Error.description().toLatin1());
		StatusRep->setStatus(KPIPrinterStatusRep::StatusError,0, _Error.description());
		delete StatusRep;
	}
// 	catch (KPSError& _Error)
// 	{
// 		qWarning(QString("Error: " + _Error.description()).toLatin1());
// 	}
	catch (...)
	{
		qWarning("Unknown error!");
	}
}


void KPIPrinter::printCommand(const QString& /*_CommandId*/, KPIPrinterStatusRep* /*_StatusRep*/, const FormatInfo& )
{
	qDebug("KPIPrinter::printCommand not implemented !");
}

void KPIPrinter::printImage(const QString& /*_SourceFileName*/, int /*_Copies*/, KPIPrinterStatusRep* /*_StatusRep*/,
		const FormatInfo& /*_FormatInfo*/)
{
	qDebug("KPIPrinter::printImage not implemented !");
}

void KPIPrinter::endOfJob()
{
	qDebug("KPIPrinter::endOfJob not implemented !");
}

void KPIPrinter::configPrinter()
{
	qDebug("KPIPrinter::configPrinter not implemented !");
}
	
int KPIPrinter::nCopies(const QString& _FileNaseName)
{
	return _FileNaseName.right(_FileNaseName.length() - (_FileNaseName.indexOf("_X_") + 3)).toInt();
}


void KPIPrinter::readCommand()
{
	if (QTcpSocket* ClientConnection = qobject_cast<QTcpSocket*>(sender()))
	{
		quint16 BlockSize = 0;
		QDataStream in(ClientConnection);
		in.setVersion(QDataStream::Qt_4_0);
		if (ClientConnection->bytesAvailable() < (int)sizeof(quint16))
		{
			qDebug("No data avaliable!");
			return;
		}
	
		in >> BlockSize;
	
		if (ClientConnection->bytesAvailable() < BlockSize)
		{
			//qDebug("BlockSize: %d is bigger than BytesAvaliable: %d", BlockSize, ClientConnection->bytesAvailable());
			return;
		}
	
		quint16 Command;
		in >> Command;
		qDebug("Received command: %d" , Command);
		QStringList Parameters;
		while (ClientConnection->bytesAvailable())
		{
			QString TmpString;
			in >> TmpString;
			Parameters << TmpString;
			qDebug(QString("Received parameter:" + TmpString).toLatin1());
		}
		// La conexió ja la tancarà el client si li cal !.
		//!!! ClientConnection->close(); 
		runCommand((EnCommandId)Command, Parameters, ClientConnection);
	}
}

void KPIPrinter::newConnection()
{
	QTcpSocket* ClientConnection = Server->nextPendingConnection();
	connect(ClientConnection, SIGNAL(readyRead()), this, SLOT(readCommand()));
	connect(ClientConnection, SIGNAL(disconnected()), ClientConnection, SLOT(deleteLater()));
	connect(ClientConnection, SIGNAL(disconnected()), this, SLOT(disconnected()));
	qint16 Port = ClientConnection->localPort();
	qDebug("Responding connection from  %s:%d",
	       ClientConnection->peerAddress().toString().toLatin1().data(), Port);
}

void KPIPrinter::disconnected()
{
	qDebug("-------------------------------- Disconnected !!! ------------------------------------------------------------------------");
}
