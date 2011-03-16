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

#ifndef KPIPRINTER_H
#define KPIPRINTER_H

#include <QObject>
#include "sterror.h"
#include "stprnlibexport.h"

/**
Classe perque les printers reportin el seu status per la red.

@author shadow
*/
class QTcpSocket;
class ST_PRNLIB_EXPORT KPIPrinterStatusRep : public QObject 
{
Q_OBJECT
public:
	enum EnStatus 
	{
		StatusReady,
		StatusStartPrinting,
		StatusPrinting, 
		StatusFinished, 
		StatusError 
	};


public:
	QTcpSocket* ClientConnection;

public:
	KPIPrinterStatusRep(QTcpSocket* _ClientConnection, QObject* _Parent = 0);
	void setStatus(EnStatus _Status, int _Percent = 0, const QString& _Error = "");
	static EnStatus readStatus(QTcpSocket* _Socket, int& _Percent, QString& _Error);

};

/**
Interficie per a totes les printers.

@author shadow
*/
class QTcpServer;
class ST_PRNLIB_EXPORT KPIPrinter : public QObject
{
Q_OBJECT
public:
	ST_DECLARE_ERRORCLASS();
	
	enum EnCommandId 	
	{
		PrintCommandId,
		PrintImageId, 
		EndOfJobId, 
		ConfigPrinterId
	};
	
class FormatInfo
{
	int Width;
	int Height;

public: 
	FormatInfo(int _Width = 0, int _Height = 0) 
	: Width(_Width), Height(_Height) {}
	FormatInfo(const QString& _Format) { fromString(_Format); }
	int width() const { return Width; }
	int height() const { return Height; }
	QString toString() const { return QString::number(Width) + "x" + QString::number(Height); }
	void fromString(const QString& _Format);
	

};

	
private:
	QTcpServer* Server;
	int Port;
	
public:
	//! _DefaultId: Id de la printer per defecte.
	KPIPrinter(int _DefaultPort, QObject* _Parent = 0);
	//! Engega el servidor al port que indiquen els settings.
	void start();
	void runCommand(EnCommandId _CommandId, const QStringList& _Parameters, 
		QTcpSocket* _ClientConnection);
	//! Prints all images in _RelatedPath, _RelatedPath is relative to CommandsSharedDataPath.
	virtual void printCommand(const QString& _RelatedPath, KPIPrinterStatusRep* _StatusRep, 
		const FormatInfo& _FormatInfo = FormatInfo());
	virtual void printImage(const QString& _SourceFileName, int _Copies, KPIPrinterStatusRep* /*_StatusRep*/,
		const FormatInfo& _FormatInfo = FormatInfo());
	//! It will be called at the end of an order.
	virtual void endOfJob();
	//! Command to call printer config dialog.
	virtual void configPrinter();
	static int ST_PRNLIB_EXPORT nCopies(const QString& _FileNaseName);
	//!Must be called before start.
	void setPort(int _Port) { Port = _Port; }

public slots: 
	void readCommand();
	void newConnection();
	void disconnected();
};

#endif
