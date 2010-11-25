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

#include "strecentfiles.h"
#include <QSettings>

int STRecentFiles::MaxRecentFiles = 10;

QStringList STRecentFiles::files()
{
	 QSettings Settings;
	 return Settings.value("recentFileList").toStringList();
}

QStringList STRecentFiles::files(int _NumFiles)
{
	QStringList Res;
	QStringList Files = files();
	int Cnt = 0;
	while (Cnt < _NumFiles && Cnt < Files.count())
	{
		Res << Files.at(Cnt);
		Cnt++;
	}
	return Res;
}

void STRecentFiles::addFile(const QString& _FilePath)
{
	 QSettings Settings;
	 QStringList Files = Settings.value("recentFileList").toStringList();
	 Files.removeAll(_FilePath);
	 Files.prepend(_FilePath);
	 while (Files.size() > MaxRecentFiles)
		 Files.removeLast();

	 Settings.setValue("recentFileList", Files);
}

void STRecentFiles::removeFile(const QString& _FilePath)
{
	 QSettings Settings;
	 QStringList Files = Settings.value("recentFileList").toStringList();
	 Files.removeAll(_FilePath);
	 Settings.setValue("recentFileList", Files);
}
