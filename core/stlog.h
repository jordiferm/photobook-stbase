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

#ifndef STLOG_H
#define STLOG_H

#include <QFile>
#include "stcoreexport.h"

/*! 
	Interfície de log. 
	Aquesta classe interpreta una sèrie de enums amb operadors de stream per a fer 
	més clar l'entrada d'informació al log. 
		
	\example 
	ISTLogIO Log;
	Log << ISTLogIO::L2 << ISTLogIO::DateTime << "Això és un exemple de log" << ISTLogIO::Endl; 
	\sa STLog
		
	@author Shadow
**/

class ST_CORE_EXPORT ISTLogIO
{
public: 
	enum ISTLogIOLevel
	{
		L0, 
		L1, 
		L2, // Log de SQl
		L3,
		L4 // Màxima informació de log. -> Log d'aplicació
	};

	enum ISTLogIOSpecial
	{
		Endl, 
		DateTime,
		PreciseTime,
	};
	
	enum ISTLogPriority
	{
		Emerg, //A panic condition was reported to all processes.
  		Alert, //A condition that should be corrected immediately.
	 	Crit, //A critical condition.
		Err, //An error message.
		Warning, //A warning message.
		Notice, //A condition requiring special handling.
 		Info, //A general information message.(* Default *)
		Debug //A message useful for debugging programs.
	};
	
		
private: 	
	ISTLogIOLevel Level; 
	
protected: 
	ISTLogPriority Priority;
	virtual ISTLogIO& putString(const QString& )
	{
		return *this; 
	}

public:
	ISTLogIO() : Level(L0) {} 
	ISTLogIO& operator<<(const QString& );
	ISTLogIO& operator<<(double _Value);
	ISTLogIO& operator<<(unsigned long long _Value);
	ISTLogIO& operator<<(long long _Value);
	ISTLogIO& operator<<(unsigned long _Value);
	ISTLogIO& operator<<(long _Value);
	ISTLogIO& operator<<(unsigned int _Value);
	ISTLogIO& operator<<(int _Value);
	ISTLogIO& operator<<(bool );
	ISTLogIO& operator<<(const char* );
	ISTLogIO& operator<<(ISTLogIOLevel _Level ) {Level = _Level; return *this;}
	ISTLogIO& operator<<(ISTLogPriority _Priority ) {Priority = _Priority; return *this;}
	ISTLogIO& operator<<(ISTLogIOSpecial _Special);
	virtual void flush();
};

class ST_CORE_EXPORT SysLogIO : public ISTLogIO
{
	QString Identity;
	QString CurrLogStr;
#ifdef Q_OS_WIN32
	QString FileName;
#endif 
	
public: 
	SysLogIO(const QString& _Identity = "stlog");	
	virtual ~SysLogIO() {}
	//! \return converts ISTLogIO to syslogPriority.
	int syslogPriority();
	QString priorityStr() const;
	
protected: 
	virtual ISTLogIO& putString(const QString& );
	virtual void flush();

};

/*! 
	Classe estàtica que retorna una instància estàtica per a la generació de logs.
	Existeix una macro stLog. 
	
	\example 
	stLog << ISTLogIO::L2 << ISTLogIO::DateTime << "Això és un exemple de log" << ISTLogIO::Endl; 
	
		
	@author Shadow
**/

class ST_CORE_EXPORT STLog 
{
	static ISTLogIO* LogCls; 
	static ISTLogIO::ISTLogIOLevel Level; // Per defecte L4
	static bool Enabled; //Per defecte a true.
	static QString Identity; 
	
public:
	static bool enabled() { return Enabled; }
	static void setEnabled(bool _Enabled);
	static ISTLogIO::ISTLogIOLevel level() {return Level;} 
	static void setLevel(ISTLogIO::ISTLogIOLevel _Level ) { Level = _Level;}
	static void setLogCls(ISTLogIO* _Value);
	static void setIdentity(const QString& _Identity) { Identity = _Identity; }
	static ISTLogIO* logCls(); 
};

#define stLog *STLog::logCls()

#endif 
