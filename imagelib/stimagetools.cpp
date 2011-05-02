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
#include "stimagetools.h"
#include <QImageReader> 
#include <QDir> 
#include <QDebug>

QString STImageTools::commonDir(const QFileInfoList& _Files)
{
	QString Res;
	QDir CurrDir;
	QString CurrDirStr;

	if (_Files.size() > 0)
	{
		CurrDir = _Files[0].dir();
		CurrDirStr = QDir::cleanPath(CurrDir.absolutePath());
	}
	int Cnt = 1;
	bool Exist = true;

	while (Cnt < _Files.size() && Exist)
	{
		QFileInfo CFile = _Files[Cnt];
		if (!QDir::cleanPath(CFile.dir().absolutePath()).contains(CurrDirStr))
		{
			if (CurrDir.cdUp())
				CurrDirStr = QDir::cleanPath(CurrDir.absolutePath());
			else
				Exist = false;
			Cnt = 1;
		}
		else
			Cnt++;
	}
	if (Exist)
		Res = CurrDirStr;

	return Res;
}


QFileInfoList STImageTools::recursiveImagesEntryInfoList(const QDir& _Dir, bool _Recursive)
{
	QFileInfoList Res; 
	
	QFileInfoList PhotoFiles = _Dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
	QList<QByteArray> Supported = QImageReader::supportedImageFormats();
	
	QFileInfoList::iterator it; 
	for (it = PhotoFiles.begin(); it != PhotoFiles.end(); ++it)
	{
		if (Supported.contains((*it).suffix().toLower().toUtf8()) || Supported.contains ((*it).suffix().toUpper().toUtf8()))
			Res.push_back(*it);
	}
	
	if (_Recursive)
	{
		QStringList StrLDirs = _Dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList::const_iterator itD;
	
		for (itD = StrLDirs.begin(); itD != StrLDirs.end(); ++itD)
			Res += recursiveImagesEntryInfoList(QDir(_Dir.absolutePath() + "/" + (*itD)), true);
		
	}
	return Res; 
}


QFileInfoList STImageTools::searchDeviceImages(const QFileInfoList& _ExcludedDrives)
{
	QFileInfoList Res; 
	QFileInfoList Drives = QDir::drives();
	
	QFileInfoList::iterator it; 
	for ( it = Drives.begin(); it != Drives.end(); ++it)
	{
		if (!_ExcludedDrives.contains(*it))
		{
			Res += recursiveImagesEntryInfoList(it->dir(), true); 
		}
	}
	return Res; 
}

QFileInfo STImageTools::searchDriveWithFiles(const QFileInfoList& _ExcludedDrives)
{
	QFileInfo Res; 
	QFileInfoList Drives = QDir::drives();
	
	QFileInfoList::iterator it = Drives.begin(); 
	bool Found = false; 
	while (it != Drives.end() && !Found)
	{
		if (!_ExcludedDrives.contains(*it))
		{
			QStringList DriveFiles = it->dir().entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
			Found = !DriveFiles.isEmpty();
			if (Found)
				Res = *it;
		}
		++it; 
	}
	return Res; 
}

QDir STImageTools::searchFirstPathWithImages(const QDir& _SourceDir, bool& _Found)
{
	QDir Res; 
	QFileInfoList PhotoFiles = _SourceDir.entryInfoList(QDir::Files, QDir::Time & QDir::Reversed);
	QList<QByteArray> Supported = QImageReader::supportedImageFormats();
	
	QFileInfoList::iterator it = PhotoFiles.begin(); 
	bool Found = false; 
	while (!Found && it != PhotoFiles.end()) 
	{
		Found = Supported.contains((*it).completeSuffix().toLower().toUtf8()) || 
				Supported.contains((*it).completeSuffix().toUpper().toUtf8());
		if (Found)
			Res = _SourceDir; 
		++it; 		
	}
	if (!Found)
	{
		QFileInfoList StrLDirs = _SourceDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
		QFileInfoList::const_iterator itD = StrLDirs.begin();
		while (!Found && itD != StrLDirs.end())
		{
			Res = searchFirstPathWithImages(itD->absoluteFilePath(), Found); 
			++itD;
		}
	}
	_Found = Found; 
	return Res; 
}

QDir STImageTools::searchFirstDrivePathWithImages(bool& _Found)
{
	QDir Res; 
	QFileInfoList Drives = QDir::drives();
	QFileInfoList::iterator it = Drives.begin(); 
	bool Found = false; 
	while (!Found && it != Drives.end())
	{
		Res = searchFirstPathWithImages(it->dir(), Found); 
		++it; 
	}
	return Res; 
}
