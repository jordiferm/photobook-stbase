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

#ifndef MTDVERSION_H
#define MTDVERSION_H

#include <QString>
#include <QList>
#include "stmetadataexport.h"

/**
@author Shadow

Valors de versió desde 0.0.0.0 fins a 99.99.99.99
On el valor de més a l'esquerra fins el primer punt és el major() 
i el següent el minor(), llavors el subMajor i el subMinor respectivament: 
 __Major
|    _____Minor
|   |
99.99.99.99
      ^  ^
      \  \__________Sub Minor.
		\__ Sub Major.
*/
class ST_METADATA_EXPORT MtdVersion
{

	typedef QList<int> TVersList;
	
	
	TVersList CurrVer;
	TVersList versList(const QString& _Version) const;
	float versValue(const QString& ) const;
	float versValue(const TVersList& _Version) const;
	
public:
	MtdVersion(const QString& = "0.0.0.0" );
	MtdVersion& operator=(const QString& _Version);
	MtdVersion& operator=(const MtdVersion& _Other);
	bool operator>(const MtdVersion& );
	bool operator<(const MtdVersion& );
	bool operator<=(const MtdVersion& );
	bool operator>=(const MtdVersion& );
	bool operator==(const MtdVersion& );
	bool operator!=(const MtdVersion& );
	void setVersion(const QString& );
	QString version() const;
	QString toString() const;
	int major() const;
	void setMajor(int ); 
	int minor() const; 
	void setMinor(int ); 
	int subMajor() const;
	void setSubMajor(int ); 
	int subMinor() const; 
	void setSubMinor(int ); 
	static MtdVersion smallestVersion();
	
};

#endif
