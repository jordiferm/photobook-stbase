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

#include "stlog.h"
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>

#ifndef Q_OS_WIN32
	#include <syslog.h>
#endif


//_____________________________________________________________________________
// ISTLogIO
//_____________________________________________________________________________

ISTLogIO& ISTLogIO::operator<<(const QString& _Value)
{
	QString LogStr; 
	if (Level <= STLog::level())
		LogStr = _Value; 
		
	return putString(LogStr);
}

ISTLogIO& ISTLogIO::operator<<(double _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(unsigned long long _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(long long _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(unsigned long _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(long _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(unsigned int _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(int _Value)
{
	operator<< (QString::number(_Value)); 
}

ISTLogIO& ISTLogIO::operator<<(bool _Value)
{
	if (_Value) 
		operator<<(QString("true"));
	else 
		operator<<(QString("false"));
	return *this;
}

ISTLogIO& ISTLogIO::operator<<(const char* _Value)
{
	operator<<(QString(_Value));
	return *this; 
}

ISTLogIO& ISTLogIO::operator<<(ISTLogIOSpecial _Special ) 
{
	switch (_Special)
	{
		case ISTLogIO::Endl: 
			flush();
			Priority = ISTLogIO::Info;
		break; 
		case ISTLogIO::DateTime: 
		#ifdef Q_OS_WIN32
			operator<<("<b>(" + QDateTime::currentDateTime().toString(Qt::ISODate) + ") - </b>");
		#endif 
		break;
		case ISTLogIO::PreciseTime: 
			operator<<("<b>(" + QTime::currentTime().toString("hh:mm:ss,zzz") + ") - </b>");
		break;
	}
	return *this;
}

void ISTLogIO::flush()
{
}

//_____________________________________________________________________________
// SysLogIO
//_____________________________________________________________________________

SysLogIO::SysLogIO(const QString& _Identity) : Identity(_Identity)
{
	#ifdef Q_OS_WIN32
		FileName = "c:/" + _Identity + ".log";
		//Mirem el dia del fitxer i si no és l'actual fem la copia i el trunquem. Per fer Log Rotatiu...
		if (QFile::exists(FileName))
		{
			QFileInfo FInfo(FileName);
			if (FInfo.created().date().dayOfWeek() != QDate::currentDate().dayOfWeek());
			{
				//Copia de seguretat i trunquem el log.
				QFile LFile(FileName);
				LFile.copy(FileName + "_" + QString::number(FInfo.created().date().dayOfWeek()));
				LFile.remove();
			}
		}
	#else 
		openlog(Identity.toAscii(), LOG_CONS, LOG_USER);
	#endif
}


int SysLogIO::syslogPriority()
{
	int Res = 0;
#ifndef Q_OS_WIN32
	switch (Priority)
	{
		case ISTLogIO::Emerg :
			Res = LOG_EMERG;
		break;
		case ISTLogIO::Alert :
			Res = LOG_ALERT;
		break;
		case ISTLogIO::Crit :
			Res = LOG_CRIT;
		break;
		case ISTLogIO::Err :
			Res = LOG_ERR;
		break;
		case ISTLogIO::Warning :
			Res = LOG_WARNING;
		break;
		case ISTLogIO::Notice :
			Res = LOG_NOTICE;
		break;
		case ISTLogIO::Info :
			Res = LOG_INFO;
		break;
		case ISTLogIO::Debug :
			Res = LOG_DEBUG;
		break;
	};
#endif 
	return Res;
}

QString SysLogIO::priorityStr() const
{
	QString Res; 
	switch (Priority)
	{
		case ISTLogIO::Emerg :
			Res = "(EMERGENCY) - ";
		break;
		case ISTLogIO::Alert :
			Res = "(Alert) - ";
		break;
		case ISTLogIO::Crit :
			Res = "(Critical) - ";
		break;
		case ISTLogIO::Err :
			Res = "(Error) - ";
		break;
		case ISTLogIO::Warning :
			Res = "Warning:";
		break;
		case ISTLogIO::Notice :
			Res = "Notice:";
		break;
		case ISTLogIO::Info :
			Res = "";
		break;
		case ISTLogIO::Debug :
			Res = "Debug:";
		break;
	};
	return Res;
}

ISTLogIO& SysLogIO::putString(const QString& _Value)
{
	CurrLogStr += _Value;
	return *this;
}



void SysLogIO::flush()
{
	if (!CurrLogStr.isEmpty())
	{
#ifdef Q_OS_WIN32
		QFile LogFile(FileName);
		if ( LogFile.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			QTextStream Strm( &LogFile );
			Strm << priorityStr() << CurrLogStr << "\n";
			LogFile.close();
		}
		else 
			qWarning(QString("SysLogIO Could not open Debug File: " + LogFile.fileName()).toLatin1() );
#else
		syslog(syslogPriority(), QString(priorityStr() + CurrLogStr).toAscii());
#endif 
		CurrLogStr.clear();
	}
}

//_____________________________________________________________________________
// STLog
//_____________________________________________________________________________

ISTLogIO* STLog::LogCls = new ISTLogIO();;
ISTLogIO::ISTLogIOLevel STLog::Level = ISTLogIO::L4;
bool STLog::Enabled = false;
QString STLog::Identity = "stlog"; 


void STLog::setEnabled(bool _Enabled)
{
	Enabled = _Enabled;
	delete LogCls;
	if (Enabled)
		LogCls = new SysLogIO(Identity);
	else
		LogCls = new ISTLogIO();
}

void STLog::setLogCls(ISTLogIO* _Value)
{
	LogCls = _Value;
}

ISTLogIO* STLog::logCls()
{
	return LogCls;
}

