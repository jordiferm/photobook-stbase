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

#include "updatemanager.h"
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>

UpdateManager::UpdateManager(const QString& _Host, int _Port, const QString& _User, const QString& _Password,
							 const QString& _RemoteDir, int _TMode ) : Host(_Host), Port(_Port), User(_User),
								Password(_Password), RemoteDir(_RemoteDir), TMode(_TMode)
{

}

void UpdateManager::setAppVersion(int _Major, int _Minor, int _Release)
{
	Major = _Major;
	Minor = _Minor;
	Release = _Release;
}

void UpdateManager::getArguments()
{
	QStringList Args = QCoreApplication::arguments();
	Assert(Args.size() == 10, Error(QObject::tr("Invalid arguments for UpdateManager")));
	Host = Args.at(1);
	Port = Args.at(2).toInt();
	User = Args.at(3);
	Password = Args.at(4);
	RemoteDir = Args.at(5);
	TMode = Args.at(6).toInt();
	Major = Args.at(7).toInt();
	Minor = Args.at(8).toInt();
	Release = Args.at(9).toInt();

}

void UpdateManager::checkForUpdates()
{
	QStringList Arguments;
	Arguments << Host << QString::number(Port) << User << "\"" + Password + "\"" << "\"" + RemoteDir + "\"" <<
			QString::number(static_cast<int>(TMode)) <<  QString::number(Major) << QString::number(Minor) << QString::number(Release);
	#ifndef Q_OS_WIN32
		QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/updatenotifier " + Arguments.join(" "));
	#else
		QProcess::startDetached("\"" + QCoreApplication::applicationDirPath() + "\"/updatenotifier.exe " + Arguments.join(" "));
	#endif
}

QString UpdateManager::remoteDirBySystem() const
{
	QString SubDir;
	#ifdef Q_OS_MAC
		SubDir = "macosx";
	#else
		#ifdef Q_OS_WIN32
			SubDir = "win32";
		#else
			SubDir = "other";
		#endif
	#endif
	return RemoteDir + "/" + SubDir;
}


QDir UpdateManager::updatesDir()
{
	return  QDir(QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation)).absoluteFilePath("starphob_updates"));
}

void UpdateManager::registerUpdateFile(const QString& _FileName)
{
	QSettings Settings("Starblitz", "updatenotifier");
	Settings.setValue("updates/updatefile",_FileName);
}

QString UpdateManager::updateFile()
{
	QSettings Settings("Starblitz", "updatenotifier");
	return Settings.value("updates/updatefile", "").toString();
}

bool UpdateManager::updateAvailable()
{
	return !updateFile().isEmpty();
}

void UpdateManager::unregisterUpdateFile()
{
	registerUpdateFile("");
}

void UpdateManager::installUpdates()
{

	if (updateAvailable())
	{
		try
		{
			QFileInfo UpdateFile(updateFile());
			if (UpdateFile.suffix().toLower() == "lnk")
			{
				QFile File(UpdateFile.absoluteFilePath());
				Assert(File.open(QIODevice::ReadOnly | QIODevice::Text), Error(QObject::tr("Unable to open lnk file %1").arg(UpdateFile.absoluteFilePath())));

				QTextStream StrmIn(&File);
				if (!StrmIn.atEnd())
					QDesktopServices::openUrl(QUrl(StrmIn.readLine()));
			}
			else
				QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(UpdateFile.absoluteFilePath())));
			unregisterUpdateFile();
			//QFile::remove(UpdateFile.absoluteFilePath());
		}
		catch(...)
		{
			unregisterUpdateFile();
			throw;
		}
	}
}
