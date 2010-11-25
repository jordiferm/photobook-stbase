/* __________________________________________________________________________
//                        .__.
//       S^3.O.S.  _.--^^ ___ \
//             _../^   _.^   ^\!
//          _./     .-^    -_. |
//      _-/^     .-^        > \
//     /^--..   /  /|     ./   \._
//           ^^/  / ^_-./^        ^.__        >-.-. www.s3os.net 
//               <_____..--^^^---...__^..__..
//               Solucions i Serveis amb Sistemes Open Source
// __________________________________________________________________________
//
//    Revision             :- $Revision: 1.1 $
//    Init Date            : Fri Oct 7 2005
//    licence              : GNU/GPL-2
//    programmer           : Shadow, (C) 2005
//    email                : shadow@s3os.net
//    File                 : fsqlrecord.cpp
//    Classification       :- C++ (Gcc 3.xx) Qt 4.xx
//                                                                    
// _________________________________________________________________________
//                                                                         
//   This program is free software; you can redistribute it and/or modify  
//   it under the terms of the GNU General Public License as published by  
//   the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
// _________________________________________________________________________
// ------------------------------------ CVS --------------------------------
// $Header: /home/cvs/s3os/flam/database/fsqlrecord.cpp,v 1.1 2005/10/07 18:05:09 shadow Exp $
// _________________________________________________________________________*/
#include "fsqlrecord.h"

FSqlRecord::FSqlRecord(const QString& _TableName)
		: QSqlRecord(), TableName(_TableName)
{
}

FSqlRecord::FSqlRecord(const QSqlRecord& _Other, const QString& _TableName) : QSqlRecord(_Other),
		TableName(_TableName)
{}

FSqlRecord::~FSqlRecord()
{}

FSqlRecord& FSqlRecord::operator=(const QSqlRecord& _Other)
{
	QSqlRecord::operator=(_Other);
	return *this;
}



