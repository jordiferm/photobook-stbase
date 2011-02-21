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

#include "printkeeper.h"
#include <QSettings>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QIcon>
#include <QSettings>
#include "eprintkeeper.h"

#define PK_ORG "Starblitz"
#define PK_APP "printkeeper"

QMessageBox* PrintKeeper::MessageBox = 0;
EPrintKeeper* PrintKeeper::EPKeeper = 0;
qlonglong PrintKeeper::CurrentCopies = 0;

void PrintKeeper::enableEPrintKeeper(bool _Remote)
{
	if (_Remote)
	{
		if (!EPKeeper)
			EPKeeper = new EPrintKeeper;
		QSettings Settings;
		CurrentCopies = Settings.value("pkcurrentcopies").toLongLong();
	}
	else
	{
		if (EPKeeper)
			delete EPKeeper;
		EPKeeper = 0;
	}
}

void PrintKeeper::setRemoteUrl(const QUrl& _RemoteUrl)
{
	if (EPKeeper)
		EPKeeper->setRemoteUrl(_RemoteUrl);
}

bool PrintKeeper::login(const QString& _User, const QString& _Password)
{
	bool Res = true;
	if (EPKeeper)
		Res = EPKeeper->login(_User, _Password);
	return Res;
}

bool PrintKeeper::isValidKey(const QString& _Key)
{
	bool Res = false;
	if (EPKeeper)
		Res = EPKeeper->isValidKey(_Key);
	return Res;
}

QString PrintKeeper::userInfo()
{
	QString Res;
	if (EPKeeper)
		Res = QString("%1 - %2").arg(EPKeeper->userRealName()).arg(EPKeeper->userEmail());
	return Res;
}

bool PrintKeeper::userInfoIsComplete()
{
	bool Res = false;
	if (EPKeeper)
		Res = !EPKeeper->userEmail().isEmpty();
	
	return Res;
}


QString PrintKeeper::errorInfo()
{
	QString Res;
	if (EPKeeper)
		Res = EPKeeper->errorInfo();
	return Res;
}

void PrintKeeper::setPrintAccessKey(const QString& _Key)
{
	if (EPKeeper)
		EPKeeper->setPrintAccessKey(_Key);
}

bool PrintKeeper::printAcces(const QString& _PrinterName, qlonglong _Copies)
{
	bool Res = false;
	PKPrinterInfo Info;
	if (!EPKeeper)
	{
		if (isDisabled())
			return true;

		if (printerInfoCount() == 0)
			return false;

		Info = printerInfo(_PrinterName);
	}
	else
	{
		if (!EPKeeper->access(_PrinterName, Info, _Copies + CurrentCopies))
			return false;
	}

	if (!Info.ip().isEmpty())
	{
		QProcess Ping;
	#ifdef Q_OS_WIN32
		Ping.start("ping", QStringList() << Info.ip());
	#else
		Ping.start("ping", QStringList() << "-c1" << Info.ip());
	#endif
		Ping.waitForFinished(2000);
		Ping.terminate();

		QProcess ArpProcess;
		QStringList Arguments;
	#ifdef Q_OS_WIN32
		Arguments << "-a";
	#else
		Arguments << "-n";
	#endif
		ArpProcess.start("arp", Arguments);
		ArpProcess.waitForFinished(10000);
		QString CurrLine = ArpProcess.readLine();
		bool Found = false;
		while (!CurrLine.isEmpty() && !Found)
		{
			//qDebug() << CurrLine;
			CurrLine = ArpProcess.readLine();
			Found = (CurrLine.contains(Info.ip()));
		}
		Res = false;
		if (Found && !Info.mAC().isEmpty())
		{
			//Check if Mac address match:
			QRegExp MacRegExp("(?:[0-9a-f][0-9a-f][-:]){5}[0-9a-f][0-9a-f]");
			if (MacRegExp.indexIn(CurrLine, 0) != -1)
			{
				QString MacAddress = MacRegExp.cap(0);
				Res = (Info.mAC().remove("-").remove(":") == MacAddress.remove("-").remove(":"));
			}
		}
	}
	else
		Res = EPKeeper != 0;

	if (Res)
	{
		CurrentCopies += _Copies;
		QSettings Settings;
		Settings.setValue("pkcurrentcopies", CurrentCopies);
	}

	return Res;
}

void PrintKeeper::clearPrinterInfo()
{
	QSettings Settings(PK_ORG, PK_APP);
	Settings.remove("printkeeperprinters");
}

int PrintKeeper::printerInfoCount()
{
	QSettings Settings(PK_ORG, PK_APP);
	int Res = Settings.beginReadArray("printkeeperprinters");
	Settings.endArray();
	return Res;
}

void PrintKeeper::setPrinterInfo(int _Index, const PKPrinterInfo& _Info)
{
	QSettings Settings(PK_ORG, PK_APP);
	Settings.beginWriteArray("printkeeperprinters");
	Settings.setArrayIndex(_Index);
	Settings.setValue("printername", _Info.name());
	Settings.setValue("ip", _Info.ip());
	Settings.setValue("mac", _Info.mAC());
	Settings.endArray();
}

PKPrinterInfo PrintKeeper::printerInfo(int _Index)
{
	QSettings Settings(PK_ORG, PK_APP);
	PKPrinterInfo Res;
	Settings.beginReadArray("printkeeperprinters");
	Settings.setArrayIndex(_Index);
	Res.setName(Settings.value("printername").toString());
	Res.setIp(Settings.value("ip").toString());
	Res.setMAC(Settings.value("mac").toString());
	Settings.endArray();
	return Res;
}

/*!
	\return Printer info for _PrinterName printer. And and empty PrinterInfo if _Printername does not exist.
 */
PKPrinterInfo PrintKeeper::printerInfo(const QString& _PrinterName)
{
	QSettings Settings(PK_ORG, PK_APP);
	PKPrinterInfo Res;

	int NumEntries = Settings.beginReadArray("printkeeperprinters");

	bool Found = false;
	int Index = 0;
	while (!Found && Index < NumEntries)
	{
		Settings.setArrayIndex(Index);
		Found = Settings.value("printername").toString() == _PrinterName;
		if (!Found)
			Index++;
	}
	Settings.endArray();
	if (Found)
		Res = printerInfo(Index);
	return Res;
}

bool PrintKeeper::isDisabled()
{
	QSettings Settings(PK_ORG, PK_APP);
	return Settings.value("printkeeper/disabled", false).toBool();
}

void PrintKeeper::setDisabled(bool _Value)
{
	QSettings Settings(PK_ORG, PK_APP);
	Settings.setValue("printkeeper/disabled", _Value);
}

void PrintKeeper::accesDeniedWarning(const QString& _PrinterName)
{
	QString WarningMessage = QObject::tr("Access denied to %1 printer: %2").arg(_PrinterName).arg(PrintKeeper::errorInfo());
	qWarning() << WarningMessage;
	if (!MessageBox)
		MessageBox = new QMessageBox(QMessageBox::Warning, QObject::tr("PrintKeeper Warning"), WarningMessage, QMessageBox::Ok);
	if (!MessageBox->isVisible())
		MessageBox->show();
}
