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

#include "stphotobookfolderdocsmodel.h"
#include <QFileInfoList>
#include <QDir>
#include <QUrl>
#include <QMimeData>

STPhotoBookFolderDocsModel::STPhotoBookFolderDocsModel(QObject* _Parent) : QAbstractListModel(_Parent)
{
}

QStringList STPhotoBookFolderDocsModel::mimeTypes() const
{
	QStringList types;
	//See http://en.wikipedia.org/wiki/MIME_type
	types << "text/uri-list";
	return types;
}

QMimeData* STPhotoBookFolderDocsModel::mimeData(const QModelIndexList &indexes) const
{
	QList<QUrl> Urls;
	foreach(QModelIndex Index, indexes)
	{
		if (Index.isValid())
		{
			Urls.push_back(QUrl::fromLocalFile(filePath(Index)));
		}
	}
	if (!Urls.isEmpty())
	{
		QMimeData *mimeData = new QMimeData();
		mimeData->setUrls(Urls);
		return mimeData;
	}
	return QAbstractListModel::mimeData(indexes);
}


/*! \return A list of files inside _PathName that matches _Filter. _Directories will be filled with all explored dirs.
*/
QFileInfoList STPhotoBookFolderDocsModel::loadPathRecursive(const QString& _PathName, const QStringList& _Filter, QStringList& _Directories)
{
	QFileInfoList Res;
	QDir Dir(_PathName);
	QFileInfoList PhotoFiles = Dir.entryInfoList(_Filter, QDir::Files, QDir::Time & QDir::Reversed);

	QFileInfoList::iterator it;
	Res += PhotoFiles;

	QStringList StrLDirs = Dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::const_iterator itD;
	_Directories += StrLDirs;
	for (itD = StrLDirs.begin(); itD != StrLDirs.end(); ++itD)
	{
		Res += loadPathRecursive(Dir.absolutePath() + "/" + (*itD), _Filter, _Directories);
	}

	return Res;
}

QFileInfoList STPhotoBookFolderDocsModel::loadPath(const QString& _PathName)
{
	FolderList.clear();
	return loadPathRecursive(_PathName, fileFilter(), FolderList);
}
