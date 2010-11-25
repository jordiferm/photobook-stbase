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

#define PK_ORG "Starblitz"
#define PK_APP "printkeeper"

QMessageBox* PrintKeeper::MessageBox = 0;

bool PrintKeeper::printAcces(const QString& _PrinterName)
{
	if (isDisabled())
		return true;

	if (printerInfoCount() == 0)
		return false;

	PrintKeeper::PrinterInfo Info = printerInfo(_PrinterName);

	if (Info.ip().isEmpty())
		return false;

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
	bool Res = false;
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

void PrintKeeper::setPrinterInfo(int _Index, const PrintKeeper::PrinterInfo& _Info)
{
	QSettings Settings(PK_ORG, PK_APP);
	Settings.beginWriteArray("printkeeperprinters");
	Settings.setArrayIndex(_Index);
	Settings.setValue("printername", _Info.name());
	Settings.setValue("ip", _Info.ip());
	Settings.setValue("mac", _Info.mAC());
	Settings.endArray();
}

PrintKeeper::PrinterInfo PrintKeeper::printerInfo(int _Index)
{
	QSettings Settings(PK_ORG, PK_APP);
	PrintKeeper::PrinterInfo Res;
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
PrintKeeper::PrinterInfo PrintKeeper::printerInfo(const QString& _PrinterName)
{
	QSettings Settings(PK_ORG, PK_APP);
	PrintKeeper::PrinterInfo Res;

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
	QString WarningMessage = QObject::tr("Access denied to %1 printer").arg(_PrinterName);
	qWarning() << WarningMessage;
	if (!MessageBox)
		MessageBox = new QMessageBox(QMessageBox::Warning, QObject::tr("PrintKeeper Warning"), WarningMessage, QMessageBox::Ok);
	if (!MessageBox->isVisible())
		MessageBox->show();
}
