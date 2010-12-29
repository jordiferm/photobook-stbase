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
#include "stutils.h"
#include <QTime>
#include <QDir>
#include <QProcess>
#include <QCryptographicHash>
#include <QApplication>
#include <QDebug>

#include <ctime>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

#include "blowfish.h"


// Convert any type of variable to C++ String
// convert=true will convert -1 to 0
template <class T> std::string STUtils::toString(const T& x, bool convert)
{
	std::ostringstream o;
	if (!(o << x))
	{
		throw std::runtime_error("::toString()");
	}
	if (o.str() == "-1" && convert)
		return "0";
	return o.str();
}

void STUtils::sleep(int _MSecs)
{
	QTime InitTime = QTime::currentTime();
	while (InitTime.msecsTo(QTime::currentTime()) < _MSecs)
		{}

}

void STUtils::sleepAndProcessEvents(int _MSecs)
{
	QTime InitTime = QTime::currentTime();
	while (InitTime.msecsTo(QTime::currentTime()) < _MSecs)
	{
		QApplication::processEvents();
	}
}

QString STUtils::lFill(const QString& _Str, const QChar& _Fill, int _Length)
{
	QString Res = _Str;
	if (Res.length() < _Length)
		Res = QString(_Length - Res.length(), _Fill) + Res;
	return Res;
}

QString STUtils::formatCurrency(const QVariant& _Value)
{
	QString Res;
	return Res.setNum(_Value.toDouble(), 'f', 2);
}

void STUtils::randomize()
{
	srand((unsigned)time(0));
}

int STUtils::random(int _Min, int _Max)
{
	int Range = (_Max - _Min) + 1;
	//return _Min + int(Range * rand() / (RAND_MAX + 1.0));
	return _Min + int(rand() % qMax(_Max, 1));
}


bool STUtils::removeRecursiveForced(const QDir& _Dir)
{
	QFileInfoList Dirs(_Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden));
	QFileInfoList::iterator dit;
	for (dit = Dirs.begin(); dit != Dirs.end(); ++dit)
	{
		removeRecursiveForced(dit->absoluteFilePath());
	}
	QFileInfoList Files(_Dir.entryInfoList(QDir::Files | QDir::Hidden));
	QFileInfoList::iterator it;
	QDir CDir(_Dir);
	for (it = Files.begin(); it != Files.end(); ++it)
	{
		QFile::setPermissions(it->absoluteFilePath(), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
		CDir.remove(it->fileName());
	}

	return CDir.rmdir(_Dir.absolutePath());
}

/*!
	Removes entire directory using system tools.
	\return true if dir was removed or it has never exist.
*/
bool STUtils::rmDir(const QDir& _Dir)
{
#ifdef Q_OS_WIN32
// 	QStringList Args;
// 	Args << "/S" << "/F" << "/Q" << _Dir.absolutePath();
// 	if (_Dir.exists())
// 		QProcess::execute("del", Args);
	removeRecursiveForced(_Dir); //It's slower than calling rm process.
#else
	QStringList Args;
	Args << "-rf" << _Dir.absolutePath();
	if (_Dir.exists())
		QProcess::execute("rm", Args);
#endif
	/*	#ifdef Q_OS_WIN32
			if (_Dir.exists())
				system(QString("del /S /F /Q " + _Dir.absolutePath()).toAscii() );
		#else
			if (_Dir.exists())
				system(QString("rm -rf " + _Dir.absolutePath()).toAscii() );
		#endif 	*/
	QDir Dir(_Dir.absolutePath());
	return !Dir.exists();
}


bool STUtils::copyDirRecursive(const QDir& _Source, const QDir& _Destination, bool _CopyHidden, const QString& _Exclude)
{
	bool Res = true;
	//!TODO Put log here !
	if (!_Source.exists())
	{
		qWarning(QString("Warning: STUtils::copyDirRecursive-> Directory %1 does not exist").arg(_Source.absolutePath()).toLatin1());
		return false;
	}

	if (!_Destination.exists())
	{
		qWarning(QString("Warning: STUtils::copyDirRecursive-> Directory %1 does not exist").arg(_Destination.absolutePath()).toLatin1());
		return false;
	}

	QFileInfoList Dirs(_Source.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden));
	QFileInfoList::iterator dit;
	dit = Dirs.begin();
	while (Res && dit != Dirs.end())
	{
		QDir DestDir(_Destination.absolutePath() + "/" + dit->fileName());
		DestDir.mkpath(DestDir.absolutePath());
		Res = copyDirRecursive(dit->absoluteFilePath(), DestDir, _CopyHidden, _Exclude);
		++dit; 
	}
	if (Res)
	{
		QFileInfoList Files;
		if (_CopyHidden)
			Files = QFileInfoList(_Source.entryInfoList(QDir::Files | QDir::Hidden));
		else
			Files = QFileInfoList(_Source.entryInfoList(QDir::Files));

		QFileInfoList::iterator it;
		for (it = Files.begin(); it != Files.end(); ++it)
		{
			QFile CFile(it->absoluteFilePath());
			if (_Exclude.isEmpty() || !CFile.fileName().contains(_Exclude))
			{
				QFileInfo DestFileInf(_Destination.absolutePath() + "/" + it->fileName());
				//!TODO Put log here !
				if (!DestFileInf.exists())
				{
					if (!CFile.copy(DestFileInf.absoluteFilePath()))
					{
						qWarning(QString("Warning: Error copying file: %1 to %2").arg(it->absoluteFilePath()).arg(_Destination.absolutePath() + "/" + it->fileName()).toLatin1());
						Res = false;
					}
				}
				else
					qWarning(QString("File %1 already exist, we don't copy it").arg(it->absoluteFilePath()).toLatin1());
			}
		}
	}
	return Res;
}
/*!
	Calc 'pascua' holiday: \sa http://es.wikipedia.org/wiki/Cálculo_de_la_fecha_de_Pascua
*/
QDate STUtils::pascuaDate(int _Year)
{
	int M = 24, N = 5;
	if (_Year >= 1583 && _Year <= 1699)
	{
		M = 22;
		N = 2;
	}
	else if (_Year >= 1700 && _Year <= 1799)
	{
		M = 23;
		N = 3;
	}
	else if (_Year >= 1800 && _Year <= 1899)
	{
		M = 23;
		N = 4;
	}
	else if (_Year >= 1900 && _Year <= 2099)
	{
		M = 24;
		N = 5;
	}
	else if (_Year >= 2100 && _Year <= 2199)
	{
		M = 24;
		N = 6;
	}
	else if (_Year >= 2200 && _Year <= 2299)
	{
		M = 25;
		N = 0;
	}

	int a, b, c, d, e, dia, mes;

	//Cálculo de residuos
	a = _Year % 19;
	b = _Year % 4;
	c = _Year % 7;
	d = (19 * a + M) % 30;
	e = (2 * b + 4 * c + 6 * d + N) % 7;

	// Decidir entre los 2 casos:
	if (d + e < 10)
	{
		dia = d + e + 22;
		mes = 3;
	}
	else
	{
		dia = d + e - 9;
		mes = 4;
	}

	// Excepciones especiales (según artículo)
	if (dia == 26  && mes == 4)
		dia = 19;
	if (dia == 25 && mes == 4 && d == 28 && e == 6 && a > 10)
		dia = 18;

	QDate Res(_Year, mes, dia);
	return Res;
}

quint64 STUtils::dirSize(const QDir& _Dir, bool _Recursive)
{
	quint64 Res = 0;
	if (_Recursive)
	{
		QFileInfoList Dirs(_Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden));
		QFileInfoList::iterator dit;
		for (dit = Dirs.begin(); dit != Dirs.end(); ++dit)
		{
			Res += dirSize(dit->absoluteFilePath(), _Recursive);
		}
	}
	QFileInfoList Files(_Dir.entryInfoList(QDir::Files | QDir::Hidden));
	QFileInfoList::iterator it;
	for (it = Files.begin(); it != Files.end(); ++it)
	{
		Res += it->size();
	}
	return Res;
}

// return best userfriendly storage unit (B, KiB, MiB, GiB, TiB)
// use Binary prefix standards from IEC 60027-2
// see http://en.wikipedia.org/wiki/Kilobyte
// value must be given in bytes
QString STUtils::friendlyUnit(float val)
{
	if (val < 0)
	{
		return tr("Unknown", "Unknown (size)");
	}
	const QString units[4] = {tr("B", "bytes"), tr("KiB", "kibibytes (1024 bytes)"), tr("MiB", "mebibytes (1024 kibibytes)"), tr("GiB", "gibibytes (1024 mibibytes)")};
	for (unsigned int i = 0; i < 5; ++i)
	{
		if (val < 1024.)
		{
			return QString(QByteArray::number(val, 'f', 1)) + units[i];
		}
		val /= 1024.;
	}
	return  QString(QByteArray::number(val, 'f', 1)) + tr("TiB", "tebibytes (1024 gibibytes)");
}

// Take a number of seconds and return an user-friendly
// time duration like "1d 2h 10m".
QString STUtils::userFriendlyDuration(qlonglong seconds)
{
	if (seconds < 0)
	{
		return QString::fromUtf8("âˆž");
	}
	if (seconds < 60)
	{
		return tr("< 1m", "< 1 minute");
	}
	int minutes = seconds / 60;
	if (minutes < 60)
	{
		return tr("%1m", "e.g: 10minutes").arg(QString::QString::fromUtf8(toString(minutes).c_str()));
	}
	int hours = minutes / 60;
	minutes = minutes - hours * 60;
	if (hours < 24)
	{
		return tr("%1h%2m", "e.g: 3hours 5minutes").arg(QString::fromUtf8(toString(hours).c_str())).arg(QString::fromUtf8(toString(minutes).c_str()));
	}
	int days = hours / 24;
	hours = hours - days * 24;
	if (days < 100)
	{
		return tr("%1d%2h%3m", "e.g: 2days 10hours 2minutes").arg(QString::fromUtf8(toString(days).c_str())).arg(QString::fromUtf8(toString(hours).c_str())).arg(QString::fromUtf8(toString(minutes).c_str()));
	}
	return QString::fromUtf8("âˆž");
}

bool STUtils::overWriteFile(QFile& _File, const QString& _DestFilePath)
{
	bool Res = true; 
	
	if (QFile::exists(_DestFilePath)) 
	{
		QFile DestFile(_DestFilePath); 
		Res = DestFile.remove(); 
	}
	if (Res)
		Res = _File.copy(_DestFilePath); 
	return Res; 
}

QString STUtils::hashString(const QString& _FilePath)
{
	QString Res; 
	QFile File(_FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		QCryptographicHash CryptoHash(QCryptographicHash::Md5);
		int BuffSize = 16384;
		QByteArray CBuff = File.read(BuffSize);
		while (!CBuff.isEmpty())
		{
			CryptoHash.addData(CBuff);
			CBuff = File.read(BuffSize);
		}
		QByteArray HashRes = CryptoHash.result();
		Res = QString(HashRes.toHex());
	}
	return Res;
}


bool STUtils::existAndIsTheSame(const QString& _SourceFilePath, const QString& _DestFilePath)
{
	bool Res = false; 
	if (QFile::exists(_DestFilePath))
	{
		Res = hashString(_SourceFilePath) == hashString(_DestFilePath); 
	}
	return Res; 
}

/*!
	\return fileInfo of file if found or empty FileInfo if not found.
*/
QFileInfo STUtils::findFile(const QDir& _SourceDir, const QString& _FileName, bool _Recursive)
{
	QFileInfo Res; 
	QFileInfoList PhotoFiles = _SourceDir.entryInfoList(QDir::Files, QDir::Time & QDir::Reversed);
	int FileIndex = PhotoFiles.indexOf(QFileInfo(_SourceDir.filePath(_FileName))); 
	bool Found = FileIndex >= 0; 
	if (!Found && _Recursive)
	{
		QFileInfoList StrLDirs = _SourceDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
		QFileInfoList::const_iterator itD = StrLDirs.begin();
		while (!Found && itD != StrLDirs.end())
		{
			Res = findFile(itD->absoluteFilePath(), _FileName, _Recursive); 
			Found = Res.exists(); 
			++itD;
		}
	}
	else 
		Res = PhotoFiles[FileIndex];
	return Res; 
}

QString STUtils::procErrorToStr(QProcess::ProcessError _Error)
{
	QString Res; 
	switch (_Error)
	{
		case QProcess::FailedToStart: 
			Res =	tr("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.");
		break; 
	
		case QProcess::Crashed	: 
			Res = tr("The process crashed some time after starting successfully."); 
		break; 
		case QProcess::Timedout	:
			Res = tr("The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again."); 
		break; 
		case QProcess::WriteError :
			Res = tr("An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.");
		break; 
		case QProcess::ReadError :
			Res = tr("An error occurred when attempting to read from the process. For example, the process may not be running.");
		break; 
		case QProcess::UnknownError : 
			Res = tr("An unknown error occurred. This is the default return value of error()"); 
		break; 
	}
	return Res; 
}

QString STUtils::shortestCanonicalPath(const QFileInfoList& _Files)
{
	QString Res = "";
	QFileInfoList::const_iterator it = _Files.begin();
	while (it != _Files.end())
	{
		if (Res.isEmpty() || it->canonicalPath().length() < Res.length() )
			Res = it->canonicalPath();
		++it;
	}
	return Res;
}

QByteArray STUtils::encode(const QByteArray& _Input, const QString& _Key)
{
	CBlowFish BFish;
	BFish.Initialize(reinterpret_cast<unsigned char*>(_Key.toAscii().data()), _Key.length());

	int DestSize = BFish.GetOutputLength(_Input.size());
	unsigned char* Data = reinterpret_cast<unsigned char*>(const_cast<char*>(_Input.data()));
	QByteArray Out(DestSize, 0);
	unsigned char* DataOut = reinterpret_cast<unsigned char*>(Out.data());
	BFish.Encode(Data, DataOut, DestSize);
	return Out;
}

QByteArray STUtils::decode(const QByteArray& _Input, const QString& _Key)
{
	CBlowFish BFish;
	BFish.Initialize(reinterpret_cast<unsigned char*>(_Key.toAscii().data()), _Key.length());


	int DestSize = BFish.GetOutputLength(_Input.size());
	unsigned char* Data = reinterpret_cast<unsigned char*>(const_cast<char*>(_Input.data()));
	QByteArray Out(DestSize, 0);
	unsigned char* DataOut = reinterpret_cast<unsigned char*>(Out.data());
	BFish.Decode(Data, DataOut, DestSize);
	return Out;
}

/*#ifdef _WIN32
	#include <windows.h>
#else // linux stuff
	#include <sys/vfs.h>
	#include <sys/stat.h>
#endif // _WIN32

bool getFreeTotalSpace(const QString& sDirPath,double& fTotal, double& fFree)
{
#ifdef _WIN32

	QString sCurDir = QDir::current().absPath();
	QDir::setCurrent( sDirPath );

	ULARGE_INTEGER free,total;
	bool bRes = ::GetDiskFreeSpaceExA( 0 , &free , &total , NULL );
	if ( !bRes ) return false;

	QDir::setCurrent( sCurDir );

	fFree = static_cast<double>( static_cast<__int64>(free.QuadPart) ) / fKB;
	fTotal = static_cast<double>( static_cast<__int64>(total.QuadPart) ) / fKB;

#else //linux

	struct stat stst;
	struct statfs stfs;

	if ( ::stat(sDirPath.local8Bit(),&stst) == -1 ) return false;
	if ( ::statfs(sDirPath.local8Bit(),&stfs) == -1 ) return false;

	fFree = stfs.f_bavail * ( stst.st_blksize / fKB );
	fTotal = stfs.f_blocks * ( stst.st_blksize / fKB );

#endif // _WIN32

	return true;
}
*/
