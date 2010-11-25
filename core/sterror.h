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

#include <QString> 
#ifndef STERROR_H
#define STERROR_H
#include "stcoreexport.h"
#include <QDateTime> 


/**
Classe genèrica d'error.

@author	Shadow.
*/


class ST_CORE_EXPORT STError
{

	QString Description; 
	QString AdvancedDesc;
	QDateTime DateTime; 
	qint32 Id;

public: 
	STError(const QString& _Description = "Unknown", const QString& _AdvancedDesc = "") : Description(_Description), 
		AdvancedDesc(_AdvancedDesc) {} 
	QString description() const {
		QString Desc = Description;
		return Desc.replace("ST Error:", ""); }
	void setDescription(const QString& _Description) { Description = _Description; }
	QString advancedDesc() const 	{	return AdvancedDesc;	}
	QString stDescription() const { return Description; }
	void setDateTime(const QDateTime& _Value) { DateTime = _Value; }
	QDateTime dateTime() const { return DateTime; }
	void setId(qint32 _Value) { Id = _Value; }
	qint32 id() const { return Id; }
};


//_____________________________________________________________________________
 /*!
   Aquesta macro defineix una classe d'error especifica per la classe class 
	La classe es dirà $(class)Error i a la descripció hi afegirà el nom de la 
	classe del qual prové l'error.
   \param class Nom de la classe error.
*/
//_____________________________________________________________________________


//! Classe d'error dins de l'ambit de membre de classe.

#define ST_DECLARE_ERRORCLASS() \
   class Error : public STError { \
public: \
	Error(const QString& _Description, const QString& _AdvancedDesc) : STError("ST Error: " + _Description, _AdvancedDesc) {} \
	Error(const QString& _Description) : STError("ST Error: " + _Description) {} \
}


//! Classe d'error fora de l'ambit de membre de classe.

#define ST_DECLARE_OUTERRORCLASS(Class) \
   class Class##Error : public STError { \
public: \
	Class##Error(const QString& _Description) : STError("#Class ST Error: " + _Description) {} \
}



//_____________________________________________________________________________
 /*!
   Funcio Assert, llença una exepcio del tipus Tr_Cls si no els compleix
   la condicio _Cond. La crida sera de la forma Assert(Excepcio("Error"), p=!NULL)
   per exemple. La diferencia amb l'assert anterior és que l'excepcio
   està instanciada a fora.Es amb majuscula per ambiguetat amb la macro de C.
   \param _Cond condicio del tipus Cond_Cls
   \param _Throw_Cls Instancia de la classe d'error que serà llençada.
	Aquí, Tr_Cls ha de ser una subclasse de GError*/
//_____________________________________________________________________________




// Les següents macros configuren el comportament de Assert.

// Si està definit BEL_VERBOSEASSERT, tots els asserts cridaran a qFatal.
//#define QT_FATAL_ASSERT 		qDebug(QString("Adv:" + _Error.advancedDesc()).toLatin1());


// Assert Mostrarà informació de l'assert per stderr en cas de que es produeixi.
#define BEL_VERBOSEASSERT 

#ifdef BEL_VERBOSEASSERT
	#define Assert(Cond, TrCls) GAssert(Cond, TrCls, QString("ASSERT: \"%1\" in %2 (%3)").arg(#Cond).arg(__FILE__).arg(__LINE__))
	#define Throw(TrCls) GThrow(TrCls, QString("EXCEPTION THROW: in %1 (%2)").arg(__FILE__).arg(__LINE__))
 #else 
 	#define Assert(Cond, TrCls) GAssert(Cond, TrCls, "ASSERT: Noinfo")
	#define Throw(TrCls) GThrow(TrCls, "EXCEPTION THROW: Noinfo")
 #endif 

template <class Tr_Cls, class Cond_Cls>
 inline void GAssert(Cond_Cls _Cond, const Tr_Cls& _Throw_Cls, const QString& _Info)
{
   if (! _Cond ) 
	{
		#ifdef BEL_VERBOSEASSERT
			qWarning( QString("[%1] ----> %2 - %3").arg(_Info).arg(_Throw_Cls.description()).arg(
				_Throw_Cls.advancedDesc()).toAscii().data());
		#endif 
		#ifdef BEL_DEBUG
			gDebug << IGDebugIO::L0 << IGDebugIO::DateTime << "<assert>" << "[" + _Info + "] ----> " << _Throw_Cls.description() << "</assert>" << IGDebugIO::Endl; 
		#endif 
		throw _Throw_Cls;
	}
}


template <class Tr_Cls>
 inline void GThrow(const Tr_Cls& _Throw_Cls, const QString& _Info)
{
		#ifdef BEL_VERBOSEASSERT
			qWarning( "[" + _Info + "] ----> " + _Throw_Cls.description());
		#endif 
		#ifdef BEL_DEBUG
			gDebug << IGDebugIO::L0 << IGDebugIO::DateTime << "<assert>" << "[" + _Info + "] ----> " << _Throw_Cls.description() << _Throw_Cls.advancedDesc() << "</assert>" << IGDebugIO::Endl; 
		#endif 
		throw _Throw_Cls;
}

#endif 
