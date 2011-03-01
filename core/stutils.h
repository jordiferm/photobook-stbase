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
#ifndef STUTILS_H
#define STUTILS_H
#include <QString>
#include <QVariant>
#include <QDate>
#include <QThread>
#include <QObject>
#include <QFileInfo> 
#include <QProcess>

#include "stcoreexport.h"

class STSortFileInfo : public QFileInfo
{
public:
	STSortFileInfo(const QFileInfo& _FileInfo);
	bool operator<(const STSortFileInfo& _Other) const;
};

/**

Several usefull methods

@author Shadow
*/
class QDir;
class QFile; 
class ST_CORE_EXPORT STUtils : public QObject
{

	Q_OBJECT

public:
	//! Sleep en milisegons;
	template <class T> static std::string toString(const T& x, bool convert = false);
	static void sleep(int _MSecs);
	static void sleepAndProcessEvents(int _MSecs);
	//! Omple l'string _Str amb _Fill fins arrivar a _Length.
	static QString lFill(const QString& _Str, const QChar& _Fill, int _Length);
	static QString formatCurrency(const QVariant& _Value);
	static void randomize();
	static int random(int _Min, int _Max);
	static bool removeRecursiveForced(const QDir& _Dir);
	static bool rmDir(const QDir& _Dir);
	static bool copyDirRecursive(const QDir& _Source, const QDir& _Destination, bool _CopyHidden = true, const QString& _Exclude = "");
	static QDate pascuaDate(int _Year);
	//! Returns size of dir _Dir
	static quint64 dirSize(const QDir& _Dir, bool _Recursive = false);
	static QString friendlyUnit(float val);
	static QString userFriendlyDuration(qlonglong seconds);
	static bool overWriteFile(QFile& _File, const QString& _DestFilePath);
	static QString hashString(const QString& _FilePath);
	static bool existAndIsTheSame(const QString& _SourceFilePath, const QString& _DestFilePath);
	static QFileInfo findFile(const QDir& _SourceDir, const QString& _FileName, bool _Recursive = true);
	static QString procErrorToStr(QProcess::ProcessError _Error);
	static QString shortestCanonicalPath(const QFileInfoList& _Files);
	//BlowFish Encryption
	static QByteArray encode(const QByteArray& _Input, const QString& _Key);
	static QByteArray decode(const QByteArray& _Input, const QString& _Key);

	static QFileInfoList sortByCreatedDate(const QFileInfoList& _ListToSort);

	static int monthsTo(const QDate& _FromDate, const QDate& _ToDate);



};

//  Trick to get a portable sleep() function
class SleeperThread : public QThread
{
public:
	static void msleep(unsigned long msecs)
	{
		QThread::msleep(msecs);
	}
};


#endif
